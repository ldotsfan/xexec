
#include "nv2a.h"

#define DEBUG_NV2A 2

#ifdef DEBUG_NV2A
# if DEBUG_NV2A == 1
#  define PRINT_NV2A        PRINT
# elif DEBUG_NV2A == 2
#  define PRINT_NV2A        PRINT
#  define VARDUMP_NV2A      VARDUMP
#  define VARDUMP2_NV2A     VARDUMP2
#  define VARDUMP3_NV2A     VARDUMP3
#  define VARDUMP4_NV2A     VARDUMP4
# else
#  define ENTER_NV2A        ENTER
#  define LEAVE_NV2A        LEAVE
#  define PRINT_NV2A        PRINT
#  define VARDUMP_NV2A      VARDUMP
#  define VARDUMP2_NV2A     VARDUMP2
#  define VARDUMP3_NV2A     VARDUMP3
#  define VARDUMP4_NV2A     VARDUMP4
# endif
#endif
#ifndef ENTER_NV2A
# define ENTER_NV2A
#endif
#ifndef LEAVE_NV2A
# define LEAVE_NV2A
#endif
#ifndef PRINT_NV2A
# define PRINT_NV2A(...)
#endif
#ifndef VARDUMP_NV2A
# define VARDUMP_NV2A(...)
#endif
#ifndef VARDUMP2_NV2A
# define VARDUMP2_NV2A(...)
#endif
#ifndef VARDUMP3_NV2A
# define VARDUMP3_NV2A(...)
#endif
#ifndef VARDUMP4_NV2A
# define VARDUMP4_NV2A(...)
#endif

static void *               xboxkrnl_gpuinstmem = NULL;
static size_t               xboxkrnl_gpuinstsz = 0;

static uint32_t             nv2a_dma[16] = { };     /* base: 0x80000000 */

typedef struct {
    union {
        uint32_t            state;
        struct {
            uint32_t        non_inc      : 1;       /*  0 */
            uint32_t        pad1         : 1;       /*  1 */
            uint32_t        method       : 10;      /*  2 */
            uint32_t        pad2         : 1;       /* 12 */
            uint32_t        subchannel   : 3;       /* 13 */
            uint32_t        pad3         : 2;       /* 16 */
            uint32_t        method_count : 10;      /* 18 */
            uint32_t        pad4         : 1;       /* 28 */
            uint32_t        error        : 3;       /* 29 */
        } PACKED;
    } PACKED;
    uint32_t                param;
    void *                  next;
} PACKED nv2a_pfifo_cache;

static nv2a_pfifo_cache *   nv2a_cacheq = NULL;
static pthread_mutex_t      nv2a_cacheq_mutex = PTHREAD_MUTEX_INITIALIZER;
#define CACHEQ_LOCK         pthread_mutex_lock(&nv2a_cacheq_mutex)
#define CACHEQ_UNLOCK       pthread_mutex_unlock(&nv2a_cacheq_mutex)

typedef struct {
    uint32_t                handle;
    union {
        uint32_t            context;
        struct {
            uint32_t        instance : 16;          /*  0 */
            uint32_t        engine   : 2;           /* 16 */
#define NV_RAMHT_ENGINE_SW                              0
#define NV_RAMHT_ENGINE_GRAPHICS                        1
#define NV_RAMHT_ENGINE_DVD                             2
            uint32_t        pad1     : 6;           /* 18 */
            uint32_t        chid     : 5;           /* 24 */
            uint32_t        pad2     : 2;           /* 29 */
            uint32_t        valid    : 1;           /* 31 */
        } PACKED;
    } PACKED;
} PACKED nv2a_pfifo_ramht;

static pthread_mutex_t      nv2a_pgraph_fifo_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t       nv2a_pgraph_fifo_cond = PTHREAD_COND_INITIALIZER;
#define PGRAPH_FIFO_LOCK    pthread_mutex_lock(&nv2a_pgraph_fifo_mutex)
#define PGRAPH_FIFO_UNLOCK  pthread_mutex_unlock(&nv2a_pgraph_fifo_mutex)
#define PGRAPH_FIFO_WAIT    pthread_cond_wait(&nv2a_pgraph_fifo_cond, &nv2a_pgraph_fifo_mutex)
#define PGRAPH_FIFO_SIGNAL  pthread_cond_signal(&nv2a_pgraph_fifo_cond)
#define PGRAPH_FIFO_BROADCAST pthread_cond_broadcast(&nv2a_pgraph_fifo_cond)

static void
nv2a_pfifo_cache_enqueue(register const nv2a_pfifo_cache *c) {
    register nv2a_pfifo_cache **q;
    register size_t i;
    ENTER_NV2A;

    CACHEQ_LOCK;

    for (q = &nv2a_cacheq, i = 1; *q; q = (typeof(q))&(**q).next, ++i);

    *q = calloc(1, sizeof(*c));
    **q = *c;
    (**q).next = NULL;
//PRINT("            param:0x%.08x | method:0x%.03hx | subchannel:0x%.02hx | method_count:0x%.02hx | non_inc:%hhu | error:%u",c->param,c->method,c->subchannel,c->method_count,c->non_inc,c->error);//XXX
    PRINT_NV2A("%s: CACHEQ: %u command%s in queue",
        xpci[XPCI_GPU].name,
        i,
        (i != 1) ? "s" : "");

    EVENT_SIGNAL;

    CACHEQ_UNLOCK;

    LEAVE_NV2A;
}

static int
nv2a_pfifo_cache_dequeue(register nv2a_pfifo_cache *c) {
    register nv2a_pfifo_cache **q;
    register int ret = 0;
    ENTER_NV2A;

    CACHEQ_LOCK;

    q = &nv2a_cacheq;

    if (*q) {
        *c = **q;
        c->next = NULL;
        free(*q);
        *q = (**q).next;
        ret = 1;
    } else {
        PRINT_NV2A("%s: CACHEQ: no commands in queue", xpci[XPCI_GPU].name);
    }

    CACHEQ_UNLOCK;

    LEAVE_NV2A;
    return ret;
}

#if 0
typedef struct {
    uint32_t                dma_put;
    uint32_t                dma_get;
    uint32_t                ref;
} nv2a_user_channel;

typedef struct {
    

    struct {
        nv2a_user_channel   channel[NV2A_MAX_CHANNELS];
    } user;
} nv2a_state;
#endif
typedef struct {
    const uint32_t          index;
    const char *            name;
    const uint32_t          offset;
    const uint32_t          size;
} nv2a_block;

enum {
    NV_PMC          =  0,       /* card master control */
    NV_PBUS,        /* 1 */     /* bus control */
    NV_PFIFO,       /* 2 */     /* MMIO and DMA FIFO submission to PGRAPH and VPE */
    NV_PFIFO_CACHE, /* 3 */
    NV_PRMA,        /* 4 */     /* access to BAR0/BAR1 from real mode */
    NV_PVIDEO,      /* 5 */     /* video overlay */
    NV_PTIMER,      /* 6 */     /* time measurement and time-based alarms */
    NV_PCOUNTER,    /* 7 */     /* performance monitoring counters */
    NV_PVPE,        /* 8 */     /* MPEG2 decoding engine */
    NV_PTV,         /* 9 */     /* TV encoder */
    NV_PRMFB,       /* 10 */    /* aliases VGA memory window */
    NV_PRMVIO,      /* 11 */    /* aliases VGA sequencer and graphics controller registers */
    NV_PFB,         /* 12 */    /* memory interface */
    NV_PSTRAPS,     /* 13 */    /* straps readout / override */
    NV_PGRAPH,      /* 14 */    /* accelerated 2d/3d drawing engine */
    NV_09F,         /* 15 */    /* NV12_IMAGE_BLIT object */
    NV_097,         /* 16 */    /* Kelvin PGRAPH object */
    NV_PCRTC,       /* 17 */    /* more CRTC controls */
    NV_PRMCIO,      /* 18 */    /* aliases VGA CRTC and attribute controller registers */
    NV_PRAMDAC,     /* 19 */    /* RAMDAC, cursor, and PLL control */
    NV_PRMDIO,      /* 20 */    /* aliases VGA palette registers */
    NV_062,         /* 21 */    /* NV10_CONTEXT_SURFACES_2D object */
    NV_PRAMIN,      /* 22 */    /* RAMIN access */
    NV_USER,        /* 23 */    /* PFIFO MMIO and DMA submission area */
};

#define NV2A_BLOCK_SIZE(x) nv2a_blocks[x].size

static const nv2a_block const nv2a_blocks[] = {
#define NV2A_BLOCK(x,y,z) \
    [NV_ ## x] = { \
        .index = NV_ ## x, \
        .name = #x, \
        .offset = y, \
        .size = z \
    }
    NV2A_BLOCK(PMC,         0x000000, 0x001000),    /* 0 */
    NV2A_BLOCK(PBUS,        0x001000, 0x001000),    /* 1 */
    NV2A_BLOCK(PFIFO,       0x002000, 0x002000),    /* 2 */
    NV2A_BLOCK(PFIFO_CACHE, 0x004000, 0x003000),    /* 3 */
    NV2A_BLOCK(PRMA,        0x007000, 0x001000),    /* 4 */     /* not implemented */
    NV2A_BLOCK(PVIDEO,      0x008000, 0x001000),    /* 5 */
    NV2A_BLOCK(PTIMER,      0x009000, 0x001000),    /* 6 */
    NV2A_BLOCK(PCOUNTER,    0x00a000, 0x001000),    /* 7 */     /* not implemented */
    NV2A_BLOCK(PVPE,        0x00b000, 0x001000),    /* 8 */     /* not implemented */
    NV2A_BLOCK(PTV,         0x00d000, 0x001000),    /* 9 */     /* not implemented */
    NV2A_BLOCK(PRMFB,       0x0a0000, 0x020000),    /* 10 */    /* not implemented */
    NV2A_BLOCK(PRMVIO,      0x0c0000, 0x001000),    /* 11 */
    NV2A_BLOCK(PFB,         0x100000, 0x001000),    /* 12 */
    NV2A_BLOCK(PSTRAPS,     0x101000, 0x001000),    /* 13 */    /* not implemented */
    NV2A_BLOCK(PGRAPH,      0x400000, 0x002000),    /* 14 */
    NV2A_BLOCK(09F,         0x500000, 0x002000),    /* 15 */
    NV2A_BLOCK(097,         0x5c0000, 0x002000),    /* 16 */
    NV2A_BLOCK(PCRTC,       0x600000, 0x001000),    /* 17 */
    NV2A_BLOCK(PRMCIO,      0x601000, 0x001000),    /* 18 */
    NV2A_BLOCK(PRAMDAC,     0x680000, 0x001000),    /* 19 */
    NV2A_BLOCK(PRMDIO,      0x681000, 0x001000),    /* 20 */    /* not implemented */
    NV2A_BLOCK(062,         0x6d0000, 0x002000),    /* 21 */
    NV2A_BLOCK(PRAMIN,      0x700000, 0x100000),    /* 22 */
    NV2A_BLOCK(USER,        0x800000, 0x800000),    /* 23 */
#undef NV2A_BLOCK
};

static int
nv2a_base_offset(register uint32_t *base) {
    if ((*base & 0xff000000) == xpci[XPCI_GPU].memreg_base) {
        *base &= 0x00ffffff;
        return 0;
    }
    return 1;
}

static const nv2a_block *
nv2a_block_lookup(register uint32_t base) {
    register const nv2a_block *b;
    register size_t i;
    ENTER_NV2A;
    VARDUMP_NV2A(VAR_IN, base);

    for (i = 0; i <= sizeof(nv2a_blocks) / sizeof(*nv2a_blocks); ++i) {
        if (i >= sizeof(nv2a_blocks) / sizeof(*nv2a_blocks)) INT3;
        b = &nv2a_blocks[i];
        if (b->offset <= base && b->offset + b->size > base) break;
    }

    LEAVE_NV2A;
    return b;
}

int
nv2a_dma_write(register uint32_t addr, register uint32_t val) {
    register const char *n;
    register size_t i;
    ENTER_NV2A;

    n = xpci[XPCI_GPU].name;

    if (addr < sizeof(nv2a_dma)) {
        i = addr / 4;
        PRINT_NV2A("%s: DMA: write: nv2a_dma[%u] (0x%.08x) <- 0x%.08x", n, i, nv2a_dma[i], val);
        nv2a_dma[i] = val;
    } else {
        PRINT_NV2A("%s: DMA: write: [0x%.08x] (0x%.08x) <- 0x%.08x", n, addr, val);
        *(uint32_t *)addr = val;
    }

    LEAVE_NV2A;
    return 1;
}

int
nv2a_dma_read(register uint32_t addr, register uint32_t *val) {
    register const char *n;
    register size_t i;
    ENTER_NV2A;

    n = xpci[XPCI_GPU].name;

    if (addr < sizeof(nv2a_dma)) {
        i = addr / 4;
        *val = nv2a_dma[i];
        PRINT_NV2A("%s: DMA:  read: nv2a_dma[%u] -> 0x%.08x", n, i, *val);
    } else {
        *val = *(uint32_t *)addr;
//        PRINT_NV2A("%s: DMA:  read: [0x%.08x]              -> 0x%.08x", n, addr, *val);
    }

    LEAVE_NV2A;
    return 1;
}

static void
nv2a_irq(register void *p) {
    register const char *n;
    ENTER_NV2A;

    n = xpci[XPCI_GPU].name;

    /* PFIFO */
    if (NV2A_REG32(p, NV_PFIFO, INTR_0) & NV2A_REG32(p, NV_PFIFO, INTR_EN_0)) {
        PRINT_NV2A("%s: IRQ:   set: PFIFO", n);
        NV2A_REG32_MASK_SET(p, NV_PMC, INTR_0, PFIFO);
    } else {
        PRINT_NV2A("%s: IRQ: unset: PFIFO", n);
        NV2A_REG32_MASK_UNSET(p, NV_PMC, INTR_0, PFIFO);
    }
    /* PCRTC */
    if (NV2A_REG32(p, NV_PCRTC, INTR_0) & NV2A_REG32(p, NV_PCRTC, INTR_EN_0)) {
        PRINT_NV2A("%s: IRQ:   set: PCRTC", n);
        NV2A_REG32_MASK_SET(p, NV_PMC, INTR_0, PCRTC);
    } else {
        PRINT_NV2A("%s: IRQ: unset: PCRTC", n);
        NV2A_REG32_MASK_UNSET(p, NV_PMC, INTR_0, PCRTC);
    }
    /* PGRAPH */
    if (NV2A_REG32(p, NV_PGRAPH, INTR) & NV2A_REG32(p, NV_PGRAPH, INTR_EN)) {
        PRINT_NV2A("%s: IRQ:   set: PGRAPH", n);
        NV2A_REG32_MASK_SET(p, NV_PMC, INTR_0, PGRAPH);
    } else {
        PRINT_NV2A("%s: IRQ: unset: PGRAPH", n);
        NV2A_REG32_MASK_UNSET(p, NV_PMC, INTR_0, PGRAPH);
    }

    if (NV2A_REG32(p, NV_PMC, INTR_0) && NV2A_REG32(p, NV_PMC, INTR_EN_0)) {
        PRINT_NV2A("%s: IRQ: raised!", n);
        xpci[XPCI_GPU].irq_level = 1;
        //TODO
    }

    LEAVE_NV2A;
}

static void
nv2a_pgraph_intr_context_switch(register void *p, register uint32_t chid) {
    ENTER_NV2A;

    if (//NV2A_REG32_MASK_GET(p, NV_PGRAPH, CTX_CONTROL, CHID_VALID) &&//TODO
        NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, CTX_USER, CHID) == chid) {
        LEAVE_NV2A;
        return;
    }
INT3;//XXX
    NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, TRAPPED_ADDR, CHID, chid);
    NV2A_REG32_MASK_SET(p, NV_PGRAPH, INTR, CONTEXT_SWITCH);

    nv2a_irq(p);

    while (NV2A_REG32_MASK_GET(p, NV_PGRAPH, INTR, CONTEXT_SWITCH)) {
//        qemu_cond_wait(&d->pgraph.interrupt_cond, &d->pgraph.lock);//TODO
    }

    LEAVE_NV2A;
}

static const char *const nv2a_pgraph_ctx_cache1_name[] = {
    NAMEB(12, chroma_key),
    NAMEB(13, user_clip),
    NAMEB(14, swizzle),
    NAMEB(18, synchronize),
    NAMEB(19, endian_mode),
    NAMEB(23, single_step),
    NAMEB(24, patch_status),
    NAMEB(25, context_surface0),
    NAMEB(26, context_surface1),
    NAMEB(27, context_pattern),
    NAMEB(28, context_rop),
    NAMEB(29, context_beta1),
    NAMEB(30, context_beta4),
    NAMEB(31, volatile_reset),
};

static const char *const nv2a_pgraph_ctx_cache1_grclass_name[] = {
    NAME(NV10_CONTEXT_SURFACES_2D), /* 0x62 */
    NAME(NV12_IMAGE_BLIT),          /* 0x9f */
    NAME(NV20_KELVIN_PRIMITIVE),    /* 0x97 */
};

static const char *const nv2a_pgraph_ctx_cache1_patch_config_name[] = {
    NAME(NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_SRCCOPY_AND),
    NAME(NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_ROP_AND),
    NAME(NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_BLEND_AND),
    NAME(NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_SRCCOPY),
    NAME(NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_SRCCOPY_PRE),
    NAME(NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_BLEND_PRE),
};

static const char *const nv2a_pgraph_ctx_cache1_dither_mode_name[] = {
    NAME(NV_PGRAPH_CTX_SWITCH1_DITHER_MODE_COMPATIBILITY),
    NAME(NV_PGRAPH_CTX_SWITCH1_DITHER_MODE_DITHER),
    NAME(NV_PGRAPH_CTX_SWITCH1_DITHER_MODE_TRUNCATE),
    NAME(NV_PGRAPH_CTX_SWITCH1_DITHER_MODE_MS),
};

static const char *const nv2a_pgraph_ctx_cache1_class_type_name[] = {
    NAME(NV_PGRAPH_CTX_SWITCH1_CLASS_TYPE_COMPATIBILITY),
    NAME(NV_PGRAPH_CTX_SWITCH1_CLASS_TYPE_PERFORMANCE),
};

static const char *const nv2a_pgraph_ctx_cache2_mono_format_name[] = {
    NAME(NV_PGRAPH_CTX_SWITCH2_MONO_FORMAT_INVALID),
    NAME(NV_PGRAPH_CTX_SWITCH2_MONO_FORMAT_CGA6_M1),
    NAME(NV_PGRAPH_CTX_SWITCH2_MONO_FORMAT_LE_M1),
};

static const char *const nv2a_pgraph_ctx_cache2_color_format_name[] = {
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_INVALID),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_Y8),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_X16A8Y8),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_X24Y8),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_A1R5G5B5),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_X1R5G5B5),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_X16A1R5G5B5),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_X17R5G5B5),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_R5G6B5),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_A16R5G6B5),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_X16R5G6B5),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_A8R8G8B8),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_X8R8G8B8),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_Y16),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_A16Y16),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_X16Y16),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_V8YB8U8YA8),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_YB8V8YA8U8),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_Y32),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_AY8),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_EYB8ECR8EYA8ECB8),
    NAME(NV_PGRAPH_CTX_SWITCH2_COLOR_FORMAT_LE_ECR8EYB8ECB8EYA8),
};

static const char *const nv2a_062_name[] = {
    NAMER(NV_062_NV10_CONTEXT_SURFACES_2D),
    NAMER(NV_062_NOP),
    NAMER(NV_062_PM_TRIGGER),
    NAMER(NV_062_NOTIFY),
    NAMER(NV_062_WAIT_FOR_IDLE),
    NAMER(NV_062_SET_CONTEXT_DMA_NOTIFY),
    NAMER(NV_062_SET_CONTEXT_DMA_IMAGE_SOURCE),
    NAMER(NV_062_SET_CONTEXT_DMA_IMAGE_DESTIN),
    NAMER(NV_062_COLOR_FORMAT),
    NAMER(NV_062_PITCH),
    NAMER(NV_062_OFFSET_SOURCE),
    NAMER(NV_062_OFFSET_DESTIN),
};

static const char *const nv2a_062_color_format_name[] = {
    NAME(NV_062_COLOR_FORMAT_LE_Y8),
    NAME(NV_062_COLOR_FORMAT_LE_X1R5G5B5_Z1R5G5B5),
    NAME(NV_062_COLOR_FORMAT_LE_X1R5G5B5_O1R5G5B5),
    NAME(NV_062_COLOR_FORMAT_LE_R5G6B5),
    NAME(NV_062_COLOR_FORMAT_LE_Y16),
    NAME(NV_062_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8),
    NAME(NV_062_COLOR_FORMAT_LE_X8R8G8B8_O8R8G8B8),
    NAME(NV_062_COLOR_FORMAT_LE_X1A7R8G8B8_Z1A7R8G8B8),
    NAME(NV_062_COLOR_FORMAT_LE_X1A7R8G8B8_O1A7R8G8B8),
    NAME(NV_062_COLOR_FORMAT_LE_A8R8G8B8),
    NAME(NV_062_COLOR_FORMAT_LE_Y32),
};

static const char *const nv2a_09f_name[] = {
    NAMER(NV_09F_NV12_IMAGE_BLIT),
    NAMER(NV_09F_NOP),
    NAMER(NV_09F_NOTIFY),
    NAMER(NV_09F_WAIT_FOR_IDLE),
    NAMER(NV_09F_WAIT_FOR_CRTC),
    NAMER(NV_09F_SET_CONTEXT_DMA_NOTIFY),
    NAMER(NV_09F_SET_CONTEXT_COLOR_KEY),
    NAMER(NV_09F_SET_CONTEXT_CLIP_RECTANGLE),
    NAMER(NV_09F_SET_CONTEXT_PATTERN),
    NAMER(NV_09F_SET_CONTEXT_ROP),
    NAMER(NV_09F_SET_CONTEXT_BETA1),
    NAMER(NV_09F_SET_CONTEXT_BETA4),
    NAMER(NV_09F_SET_CONTEXT_SURFACE),
    NAMER(NV_09F_SET_OPERATION),
    NAMER(NV_09F_POINT_IN),
    NAMER(NV_09F_POINT_OUT),
    NAMER(NV_09F_SIZE),
};

static const char *const nv2a_09f_set_operation_name[] = {
    NAME(NV_09F_SET_OPERATION_MODE_SRCCOPY_AND),
    NAME(NV_09F_SET_OPERATION_MODE_ROP_AND),
    NAME(NV_09F_SET_OPERATION_MODE_BLEND_AND),
    NAME(NV_09F_SET_OPERATION_MODE_SRCCOPY),
    NAME(NV_09F_SET_OPERATION_MODE_SRCCOPY_PREMULT),
    NAME(NV_09F_SET_OPERATION_MODE_BLEND_PREMULT),
};

static const char *const nv2a_097_name[] = {
    NAMER(NV_097_NV20_KELVIN_PRIMITIVE),
    NAMER(NV_097_NO_OPERATION),
    NAMER(NV_097_NOTIFY),
    NAMER(NV_097_SET_WARNING_ENABLE),
    NAMER(NV_097_GET_STATE),
    NAMER(NV_097_WAIT_FOR_IDLE),
    NAMER(NV_097_PM_TRIGGER),
    NAMER(NV_097_SET_FLIP_READ),
    NAMER(NV_097_SET_FLIP_WRITE),
    NAMER(NV_097_SET_FLIP_MODULO),
    NAMER(NV_097_FLIP_INCREMENT_WRITE),
    NAMER(NV_097_FLIP_STALL),
    NAMER(NV_097_SET_CONTEXT_DMA_NOTIFIES),
    NAMER(NV_097_SET_CONTEXT_DMA_A),
    NAMER(NV_097_SET_CONTEXT_DMA_B),
    NAMER(NV_097_SET_CONTEXT_DMA_STATE),
    NAMER(NV_097_SET_CONTEXT_DMA_COLOR),
    NAMER(NV_097_SET_CONTEXT_DMA_ZETA),
    NAMER(NV_097_SET_CONTEXT_DMA_VERTEX_A),
    NAMER(NV_097_SET_CONTEXT_DMA_VERTEX_B),
    NAMER(NV_097_SET_CONTEXT_DMA_SEMAPHORE),
    NAMER(NV_097_SET_CONTEXT_DMA_REPORT),
    NAMER(NV_097_SET_SURFACE_CLIP_HORIZONTAL),
    NAMER(NV_097_SET_SURFACE_CLIP_VERTICAL),
    NAMER(NV_097_SET_SURFACE_PITCH),
    NAMER(NV_097_SET_SURFACE_COLOR_OFFSET),
    NAMER(NV_097_SET_SURFACE_ZETA_OFFSET),
    NAMER(NV_097_SET_COMBINER_ALPHA_ICW_0),
    NAMER(NV_097_SET_COMBINER_ALPHA_ICW_1),
    NAMER(NV_097_SET_COMBINER_ALPHA_ICW_2),
    NAMER(NV_097_SET_COMBINER_ALPHA_ICW_3),
    NAMER(NV_097_SET_COMBINER_ALPHA_ICW_4),
    NAMER(NV_097_SET_COMBINER_ALPHA_ICW_5),
    NAMER(NV_097_SET_COMBINER_ALPHA_ICW_6),
    NAMER(NV_097_SET_COMBINER_ALPHA_ICW_7),
#if 0
NV_097_SET_COMBINER_ALPHA_ICW_A_MAP                             0xe0000000
NV_097_SET_COMBINER_ALPHA_ICW_A_MAP__BITSHIFT                   29
NV_097_SET_COMBINER_ALPHA_ICW_A_MAP_UNSIGNED_IDENTITY           0
NV_097_SET_COMBINER_ALPHA_ICW_A_MAP_UNSIGNED_INVERT             1
NV_097_SET_COMBINER_ALPHA_ICW_A_MAP_EXPAND_NORMAL               2
NV_097_SET_COMBINER_ALPHA_ICW_A_MAP_EXPAND_NEGATE               3
NV_097_SET_COMBINER_ALPHA_ICW_A_MAP_HALFBIAS_NORMAL             4
NV_097_SET_COMBINER_ALPHA_ICW_A_MAP_HALFBIAS_NEGATE             5
NV_097_SET_COMBINER_ALPHA_ICW_A_MAP_SIGNED_IDENTITY             6
NV_097_SET_COMBINER_ALPHA_ICW_A_MAP_SIGNED_NEGATE               7
NV_097_SET_COMBINER_ALPHA_ICW_A_ALPHA                           0x10000000
NV_097_SET_COMBINER_ALPHA_ICW_A_ALPHA__BITSHIFT                 28
NV_097_SET_COMBINER_ALPHA_ICW_A_ALPHA_FALSE                     0
NV_097_SET_COMBINER_ALPHA_ICW_A_ALPHA_TRUE                      1
NV_097_SET_COMBINER_ALPHA_ICW_A_SOURCE                          0x0f000000
NV_097_SET_COMBINER_ALPHA_ICW_A_SOURCE__BITSHIFT                24
NV_097_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_0                    0
NV_097_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_1                    1
NV_097_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_2                    2
NV_097_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_3                    3
NV_097_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4                    4
NV_097_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_5                    5
NV_097_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_8                    8
NV_097_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_9                    9
NV_097_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_A                    10
NV_097_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_B                    11
NV_097_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_C                    12
NV_097_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_D                    13
NV_097_SET_COMBINER_ALPHA_ICW_B_MAP                             0x00e00000
NV_097_SET_COMBINER_ALPHA_ICW_B_MAP__BITSHIFT                   21
NV_097_SET_COMBINER_ALPHA_ICW_B_MAP_UNSIGNED_IDENTITY           0
NV_097_SET_COMBINER_ALPHA_ICW_B_MAP_UNSIGNED_INVERT             1
NV_097_SET_COMBINER_ALPHA_ICW_B_MAP_EXPAND_NORMAL               2
NV_097_SET_COMBINER_ALPHA_ICW_B_MAP_EXPAND_NEGATE               3
NV_097_SET_COMBINER_ALPHA_ICW_B_MAP_HALFBIAS_NORMAL             4
NV_097_SET_COMBINER_ALPHA_ICW_B_MAP_HALFBIAS_NEGATE             5
NV_097_SET_COMBINER_ALPHA_ICW_B_MAP_SIGNED_IDENTITY             6
NV_097_SET_COMBINER_ALPHA_ICW_B_MAP_SIGNED_NEGATE               7
NV_097_SET_COMBINER_ALPHA_ICW_B_ALPHA                           0x00100000
NV_097_SET_COMBINER_ALPHA_ICW_B_ALPHA__BITSHIFT                 20
NV_097_SET_COMBINER_ALPHA_ICW_B_ALPHA_FALSE                     0
NV_097_SET_COMBINER_ALPHA_ICW_B_ALPHA_TRUE                      1
NV_097_SET_COMBINER_ALPHA_ICW_B_SOURCE                          0x000f0000
NV_097_SET_COMBINER_ALPHA_ICW_B_SOURCE__BITSHIFT                16
NV_097_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_0                    0
NV_097_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_1                    1
NV_097_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_2                    2
NV_097_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_3                    3
NV_097_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_4                    4
NV_097_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_5                    5
NV_097_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_8                    8
NV_097_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_9                    9
NV_097_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_A                    10
NV_097_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_B                    11
NV_097_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_C                    12
NV_097_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_D                    13
NV_097_SET_COMBINER_ALPHA_ICW_C_MAP                             0x0000e000
NV_097_SET_COMBINER_ALPHA_ICW_C_MAP__BITSHIFT                   13
NV_097_SET_COMBINER_ALPHA_ICW_C_MAP_UNSIGNED_IDENTITY           0
NV_097_SET_COMBINER_ALPHA_ICW_C_MAP_UNSIGNED_INVERT             1
NV_097_SET_COMBINER_ALPHA_ICW_C_MAP_EXPAND_NORMAL               2
NV_097_SET_COMBINER_ALPHA_ICW_C_MAP_EXPAND_NEGATE               3
NV_097_SET_COMBINER_ALPHA_ICW_C_MAP_HALFBIAS_NORMAL             4
NV_097_SET_COMBINER_ALPHA_ICW_C_MAP_HALFBIAS_NEGATE             5
NV_097_SET_COMBINER_ALPHA_ICW_C_MAP_SIGNED_IDENTITY             6
NV_097_SET_COMBINER_ALPHA_ICW_C_MAP_SIGNED_NEGATE               7
NV_097_SET_COMBINER_ALPHA_ICW_C_ALPHA                           0x00001000
NV_097_SET_COMBINER_ALPHA_ICW_C_ALPHA__BITSHIFT                 12
NV_097_SET_COMBINER_ALPHA_ICW_C_ALPHA_FALSE                     0
NV_097_SET_COMBINER_ALPHA_ICW_C_ALPHA_TRUE                      1
NV_097_SET_COMBINER_ALPHA_ICW_C_SOURCE                          0x00000f00
NV_097_SET_COMBINER_ALPHA_ICW_C_SOURCE__BITSHIFT                8
NV_097_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_0                    0
NV_097_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_1                    1
NV_097_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_2                    2
NV_097_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_3                    3
NV_097_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_4                    4
NV_097_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_5                    5
NV_097_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_8                    8
NV_097_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_9                    9
NV_097_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_A                    10
NV_097_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_B                    11
NV_097_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_C                    12
NV_097_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_D                    13
NV_097_SET_COMBINER_ALPHA_ICW_D_MAP                             0x000000e0
NV_097_SET_COMBINER_ALPHA_ICW_D_MAP__BITSHIFT                   5
NV_097_SET_COMBINER_ALPHA_ICW_D_MAP_UNSIGNED_IDENTITY           0
NV_097_SET_COMBINER_ALPHA_ICW_D_MAP_UNSIGNED_INVERT             1
NV_097_SET_COMBINER_ALPHA_ICW_D_MAP_EXPAND_NORMAL               2
NV_097_SET_COMBINER_ALPHA_ICW_D_MAP_EXPAND_NEGATE               3
NV_097_SET_COMBINER_ALPHA_ICW_D_MAP_HALFBIAS_NORMAL             4
NV_097_SET_COMBINER_ALPHA_ICW_D_MAP_HALFBIAS_NEGATE             5
NV_097_SET_COMBINER_ALPHA_ICW_D_MAP_SIGNED_IDENTITY             6
NV_097_SET_COMBINER_ALPHA_ICW_D_MAP_SIGNED_NEGATE               7
NV_097_SET_COMBINER_ALPHA_ICW_D_ALPHA                           0x00000010
NV_097_SET_COMBINER_ALPHA_ICW_D_ALPHA__BITSHIFT                 4
NV_097_SET_COMBINER_ALPHA_ICW_D_ALPHA_FALSE                     0
NV_097_SET_COMBINER_ALPHA_ICW_D_ALPHA_TRUE                      1
NV_097_SET_COMBINER_ALPHA_ICW_D_SOURCE                          0x0000000f
NV_097_SET_COMBINER_ALPHA_ICW_D_SOURCE__BITSHIFT                0
NV_097_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_0                    0
NV_097_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_1                    1
NV_097_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_2                    2
NV_097_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_3                    3
NV_097_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_4                    4
NV_097_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_5                    5
NV_097_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_8                    8
NV_097_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_9                    9
NV_097_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_A                    10
NV_097_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_B                    11
NV_097_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_C                    12
NV_097_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_D                    13
#endif
    NAMER(NV_097_SET_COMBINER_COLOR_ICW_0),
    NAMER(NV_097_SET_COMBINER_COLOR_ICW_1),
    NAMER(NV_097_SET_COMBINER_COLOR_ICW_2),
    NAMER(NV_097_SET_COMBINER_COLOR_ICW_3),
    NAMER(NV_097_SET_COMBINER_COLOR_ICW_4),
    NAMER(NV_097_SET_COMBINER_COLOR_ICW_5),
    NAMER(NV_097_SET_COMBINER_COLOR_ICW_6),
    NAMER(NV_097_SET_COMBINER_COLOR_ICW_7),
#if 0
NV_097_SET_COMBINER_COLOR_ICW_A_MAP                             0xe0000000
NV_097_SET_COMBINER_COLOR_ICW_A_MAP__BITSHIFT                   29
NV_097_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY           0
NV_097_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_INVERT             1
NV_097_SET_COMBINER_COLOR_ICW_A_MAP_EXPAND_NORMAL               2
NV_097_SET_COMBINER_COLOR_ICW_A_MAP_EXPAND_NEGATE               3
NV_097_SET_COMBINER_COLOR_ICW_A_MAP_HALFBIAS_NORMAL             4
NV_097_SET_COMBINER_COLOR_ICW_A_MAP_HALFBIAS_NEGATE             5
NV_097_SET_COMBINER_COLOR_ICW_A_MAP_SIGNED_IDENTITY             6
NV_097_SET_COMBINER_COLOR_ICW_A_MAP_SIGNED_NEGATE               7
NV_097_SET_COMBINER_COLOR_ICW_A_ALPHA                           0x10000000
NV_097_SET_COMBINER_COLOR_ICW_A_ALPHA__BITSHIFT                 28
NV_097_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE                     0
NV_097_SET_COMBINER_COLOR_ICW_A_ALPHA_TRUE                      1
NV_097_SET_COMBINER_COLOR_ICW_A_SOURCE                          0x0f000000
NV_097_SET_COMBINER_COLOR_ICW_A_SOURCE__BITSHIFT                24
NV_097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_0                    0
NV_097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1                    1
NV_097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_2                    2
NV_097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_3                    3
NV_097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_4                    4
NV_097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_5                    5
NV_097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_8                    8
NV_097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_9                    9
NV_097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_A                    10
NV_097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_B                    11
NV_097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_C                    12
NV_097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_D                    13
NV_097_SET_COMBINER_COLOR_ICW_B_MAP                             0x00e00000
NV_097_SET_COMBINER_COLOR_ICW_B_MAP__BITSHIFT                   21
NV_097_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY           0
NV_097_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_INVERT             1
NV_097_SET_COMBINER_COLOR_ICW_B_MAP_EXPAND_NORMAL               2
NV_097_SET_COMBINER_COLOR_ICW_B_MAP_EXPAND_NEGATE               3
NV_097_SET_COMBINER_COLOR_ICW_B_MAP_HALFBIAS_NORMAL             4
NV_097_SET_COMBINER_COLOR_ICW_B_MAP_HALFBIAS_NEGATE             5
NV_097_SET_COMBINER_COLOR_ICW_B_MAP_SIGNED_IDENTITY             6
NV_097_SET_COMBINER_COLOR_ICW_B_MAP_SIGNED_NEGATE               7
NV_097_SET_COMBINER_COLOR_ICW_B_ALPHA                           0x00100000
NV_097_SET_COMBINER_COLOR_ICW_B_ALPHA__BITSHIFT                 20
NV_097_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE                     0
NV_097_SET_COMBINER_COLOR_ICW_B_ALPHA_TRUE                      1
NV_097_SET_COMBINER_COLOR_ICW_B_SOURCE                          0x000f0000
NV_097_SET_COMBINER_COLOR_ICW_B_SOURCE__BITSHIFT                16
NV_097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_0                    0
NV_097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_1                    1
NV_097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_2                    2
NV_097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_3                    3
NV_097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_4                    4
NV_097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_5                    5
NV_097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8                    8
NV_097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_9                    9
NV_097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_A                    10
NV_097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_B                    11
NV_097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_C                    12
NV_097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_D                    13
NV_097_SET_COMBINER_COLOR_ICW_C_MAP                             0x0000e000
NV_097_SET_COMBINER_COLOR_ICW_C_MAP__BITSHIFT                   13
NV_097_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY           0
NV_097_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_INVERT             1
NV_097_SET_COMBINER_COLOR_ICW_C_MAP_EXPAND_NORMAL               2
NV_097_SET_COMBINER_COLOR_ICW_C_MAP_EXPAND_NEGATE               3
NV_097_SET_COMBINER_COLOR_ICW_C_MAP_HALFBIAS_NORMAL             4
NV_097_SET_COMBINER_COLOR_ICW_C_MAP_HALFBIAS_NEGATE             5
NV_097_SET_COMBINER_COLOR_ICW_C_MAP_SIGNED_IDENTITY             6
NV_097_SET_COMBINER_COLOR_ICW_C_MAP_SIGNED_NEGATE               7
NV_097_SET_COMBINER_COLOR_ICW_C_ALPHA                           0x00001000
NV_097_SET_COMBINER_COLOR_ICW_C_ALPHA__BITSHIFT                 12
NV_097_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE                     0
NV_097_SET_COMBINER_COLOR_ICW_C_ALPHA_TRUE                      1
NV_097_SET_COMBINER_COLOR_ICW_C_SOURCE                          0x00000f00
NV_097_SET_COMBINER_COLOR_ICW_C_SOURCE__BITSHIFT                8
NV_097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_0                    0
NV_097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_1                    1
NV_097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_2                    2
NV_097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_3                    3
NV_097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_4                    4
NV_097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_5                    5
NV_097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_8                    8
NV_097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_9                    9
NV_097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_A                    10
NV_097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_B                    11
NV_097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_C                    12
NV_097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_D                    13
NV_097_SET_COMBINER_COLOR_ICW_D_MAP                             0x000000e0
NV_097_SET_COMBINER_COLOR_ICW_D_MAP__BITSHIFT                   5
NV_097_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY           0
NV_097_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_INVERT             1
NV_097_SET_COMBINER_COLOR_ICW_D_MAP_EXPAND_NORMAL               2
NV_097_SET_COMBINER_COLOR_ICW_D_MAP_EXPAND_NEGATE               3
NV_097_SET_COMBINER_COLOR_ICW_D_MAP_HALFBIAS_NORMAL             4
NV_097_SET_COMBINER_COLOR_ICW_D_MAP_HALFBIAS_NEGATE             5
NV_097_SET_COMBINER_COLOR_ICW_D_MAP_SIGNED_IDENTITY             6
NV_097_SET_COMBINER_COLOR_ICW_D_MAP_SIGNED_NEGATE               7
NV_097_SET_COMBINER_COLOR_ICW_D_ALPHA                           0x00000010
NV_097_SET_COMBINER_COLOR_ICW_D_ALPHA__BITSHIFT                 4
NV_097_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE                     0
NV_097_SET_COMBINER_COLOR_ICW_D_ALPHA_TRUE                      1
NV_097_SET_COMBINER_COLOR_ICW_D_SOURCE                          0x0000000f
NV_097_SET_COMBINER_COLOR_ICW_D_SOURCE__BITSHIFT                0
NV_097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_0                    0
NV_097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_1                    1
NV_097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_2                    2
NV_097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_3                    3
NV_097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_4                    4
NV_097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_5                    5
NV_097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_8                    8
NV_097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9                    9
NV_097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_A                    10
NV_097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_B                    11
NV_097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_C                    12
NV_097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_D                    13
#endif
    NAMER(NV_097_SET_COMBINER_FACTOR0_0),
    NAMER(NV_097_SET_COMBINER_FACTOR0_1),
    NAMER(NV_097_SET_COMBINER_FACTOR0_2),
    NAMER(NV_097_SET_COMBINER_FACTOR0_3),
    NAMER(NV_097_SET_COMBINER_FACTOR0_4),
    NAMER(NV_097_SET_COMBINER_FACTOR0_5),
    NAMER(NV_097_SET_COMBINER_FACTOR0_6),
    NAMER(NV_097_SET_COMBINER_FACTOR0_7),
#if 0
NV_097_SET_COMBINER_FACTOR0_BLUE                                0x000000ff
NV_097_SET_COMBINER_FACTOR0_BLUE__BITSHIFT                      0
NV_097_SET_COMBINER_FACTOR0_GREEN                               0x0000ff00
NV_097_SET_COMBINER_FACTOR0_GREEN__BITSHIFT                     8
NV_097_SET_COMBINER_FACTOR0_RED                                 0x00ff0000
NV_097_SET_COMBINER_FACTOR0_RED__BITSHIFT                       16
NV_097_SET_COMBINER_FACTOR0_ALPHA                               0xff000000
NV_097_SET_COMBINER_FACTOR0_ALPHA__BITSHIFT                     24
#endif
    NAMER(NV_097_SET_COMBINER_FACTOR1_0),
    NAMER(NV_097_SET_COMBINER_FACTOR1_1),
    NAMER(NV_097_SET_COMBINER_FACTOR1_2),
    NAMER(NV_097_SET_COMBINER_FACTOR1_3),
    NAMER(NV_097_SET_COMBINER_FACTOR1_4),
    NAMER(NV_097_SET_COMBINER_FACTOR1_5),
    NAMER(NV_097_SET_COMBINER_FACTOR1_6),
    NAMER(NV_097_SET_COMBINER_FACTOR1_7),
#if 0
NV_097_SET_COMBINER_FACTOR1_BLUE                                0x000000ff
NV_097_SET_COMBINER_FACTOR1_BLUE__BITSHIFT                      0
NV_097_SET_COMBINER_FACTOR1_GREEN                               0x0000ff00
NV_097_SET_COMBINER_FACTOR1_GREEN__BITSHIFT                     8
NV_097_SET_COMBINER_FACTOR1_RED                                 0x00ff0000
NV_097_SET_COMBINER_FACTOR1_RED__BITSHIFT                       16
NV_097_SET_COMBINER_FACTOR1_ALPHA                               0xff000000
NV_097_SET_COMBINER_FACTOR1_ALPHA__BITSHIFT                     24
#endif
    NAMER(NV_097_SET_COMBINER_ALPHA_OCW_0),
    NAMER(NV_097_SET_COMBINER_ALPHA_OCW_1),
    NAMER(NV_097_SET_COMBINER_ALPHA_OCW_2),
    NAMER(NV_097_SET_COMBINER_ALPHA_OCW_3),
    NAMER(NV_097_SET_COMBINER_ALPHA_OCW_4),
    NAMER(NV_097_SET_COMBINER_ALPHA_OCW_5),
    NAMER(NV_097_SET_COMBINER_ALPHA_OCW_6),
    NAMER(NV_097_SET_COMBINER_ALPHA_OCW_7),
#if 0
NV_097_SET_COMBINER_ALPHA_OCW_OPERATION                         0xffff8000
NV_097_SET_COMBINER_ALPHA_OCW_OPERATION__BITSHIFT               15
NV_097_SET_COMBINER_ALPHA_OCW_OPERATION_NOSHIFT                 0
NV_097_SET_COMBINER_ALPHA_OCW_OPERATION_NOSHIFT_BIAS            1
NV_097_SET_COMBINER_ALPHA_OCW_OPERATION_SHIFTLEFTBY1            2
NV_097_SET_COMBINER_ALPHA_OCW_OPERATION_SHIFTLEFTBY1_BIAS       3
NV_097_SET_COMBINER_ALPHA_OCW_OPERATION_SHIFTLEFTBY2            4
NV_097_SET_COMBINER_ALPHA_OCW_OPERATION_SHIFTRIGHTBY1           6
NV_097_SET_COMBINER_ALPHA_OCW_MUX_ENABLE                        0x00004000
NV_097_SET_COMBINER_ALPHA_OCW_MUX_ENABLE__BITSHIFT              14
NV_097_SET_COMBINER_ALPHA_OCW_MUX_ENABLE_FALSE                  0
NV_097_SET_COMBINER_ALPHA_OCW_MUX_ENABLE_TRUE                   1
NV_097_SET_COMBINER_ALPHA_OCW_SUM_DST                           0x00000f00
NV_097_SET_COMBINER_ALPHA_OCW_SUM_DST__BITSHIFT                 8
NV_097_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_0                     0
NV_097_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_4                     4
NV_097_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_5                     5
NV_097_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_8                     8
NV_097_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_9                     9
NV_097_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_A                     10
NV_097_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_B                     11
NV_097_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_C                     12
NV_097_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_D                     13
NV_097_SET_COMBINER_ALPHA_OCW_AB_DST                            0x000000f0
NV_097_SET_COMBINER_ALPHA_OCW_AB_DST__BITSHIFT                  4
NV_097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0                      0
NV_097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_4                      4
NV_097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_5                      5
NV_097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_8                      8
NV_097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_9                      9
NV_097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_A                      10
NV_097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_B                      11
NV_097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_C                      12
NV_097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_D                      13
NV_097_SET_COMBINER_ALPHA_OCW_CD_DST                            0x0000000f
NV_097_SET_COMBINER_ALPHA_OCW_CD_DST__BITSHIFT                  0
NV_097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0                      0
NV_097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_4                      4
NV_097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_5                      5
NV_097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_8                      8
NV_097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_9                      9
NV_097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_A                      10
NV_097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_B                      11
NV_097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_C                      12
NV_097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_D                      13
#endif
    NAMER(NV_097_SET_COMBINER_SPECULAR_FOG_CW0),
#if 0
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_INVERSE                  0xe0000000
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_INVERSE__BITSHIFT        29
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_INVERSE_FALSE            0
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_INVERSE_TRUE             1
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_ALPHA                    0x10000000
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_ALPHA__BITSHIFT          28
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_ALPHA_FALSE              0
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_ALPHA_TRUE               1
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE                   0x0f000000
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE__BITSHIFT         24
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_0             0
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_1             1
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_2             2
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_3             3
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_4             4
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_5             5
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_8             8
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_9             9
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_A             10
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_B             11
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_C             12
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_D             13
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_SPECLIT       14
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_EF_PROD       15
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_INVERSE                  0x00e00000
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_INVERSE__BITSHIFT        21
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_INVERSE_FALSE            0
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_INVERSE_TRUE             1
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_ALPHA                    0x00100000
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_ALPHA__BITSHIFT          20
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_ALPHA_FALSE              0
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_ALPHA_TRUE               1
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE                   0x000f0000
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE__BITSHIFT         16
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_0             0
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_1             1
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_2             2
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_3             3
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_4             4
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_5             5
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_8             8
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_9             9
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_A             10
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_B             11
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_C             12
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_D             13
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_SPECLIT       14
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_EF_PROD       15
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_INVERSE                  0x0000e000
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_INVERSE__BITSHIFT        13
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_INVERSE_FALSE            0
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_INVERSE_TRUE             1
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_ALPHA                    0x00001000
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_ALPHA__BITSHIFT          12
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_ALPHA_FALSE              0
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_ALPHA_TRUE               1
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE                   0x00000f00
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE__BITSHIFT         8
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_0             0
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_1             1
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_2             2
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_3             3
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_4             4
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_5             5
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_8             8
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_9             9
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_A             10
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_B             11
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_C             12
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_D             13
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_SPECLIT       14
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_EF_PROD       15
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_INVERSE                  0x000000e0
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_INVERSE__BITSHIFT        5
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_INVERSE_FALSE            0
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_INVERSE_TRUE             1
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_ALPHA                    0x00000010
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_ALPHA__BITSHIFT          4
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_ALPHA_FALSE              0
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_ALPHA_TRUE               1
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE                   0x0000000f
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE__BITSHIFT         0
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_0             0
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_1             1
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_2             2
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_3             3
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_4             4
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_5             5
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_8             8
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_9             9
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_A             10
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_B             11
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_C             12
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_D             13
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_SPECLIT       14
NV_097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_EF_PROD       15
#endif
    NAMER(NV_097_SET_COMBINER_SPECULAR_FOG_CW1),
#if 0
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_INVERSE                  0xe0000000
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_INVERSE__BITSHIFT        29
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_INVERSE_FALSE            0
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_INVERSE_TRUE             1
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_ALPHA                    0x10000000
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_ALPHA__BITSHIFT          28
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_ALPHA_FALSE              0
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_ALPHA_TRUE               1
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE                   0x0f000000
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE__BITSHIFT         24
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_0             0
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_1             1
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_2             2
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_3             3
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_4             4
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_5             5
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_8             8
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_9             9
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_A             10
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_B             11
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_C             12
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE_REG_D             13
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_INVERSE                  0x00e00000
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_INVERSE__BITSHIFT        21
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_INVERSE_FALSE            0
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_INVERSE_TRUE             1
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_ALPHA                    0x00100000
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_ALPHA__BITSHIFT          20
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_ALPHA_FALSE              0
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_ALPHA_TRUE               1
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE                   0x000f0000
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE__BITSHIFT         16
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_0             0
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_1             1
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_2             2
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_3             3
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_4             4
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_5             5
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_8             8
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_9             9
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_A             10
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_B             11
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_C             12
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE_REG_D             13
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_INVERSE                  0x0000e000
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_INVERSE__BITSHIFT        13
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_INVERSE_FALSE            0
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_INVERSE_TRUE             1
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_ALPHA                    0x00001000
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_ALPHA__BITSHIFT          12
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_ALPHA_FALSE              0
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_ALPHA_TRUE               1
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE                   0x00000f00
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE__BITSHIFT         8
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_0             0
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_1             1
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_2             2
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_3             3
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_4             4
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_5             5
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_8             8
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_9             9
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_A             10
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_B             11
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_C             12
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE_REG_D             13
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_CLAMP             0x00000080
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_CLAMP__BITSHIFT   7
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_CLAMP_FALSE       0
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_CLAMP_TRUE        1
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R5             0x00000040
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R5__BITSHIFT   6
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R5_FALSE       0
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R5_TRUE        1
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R12            0x0000003f
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R12__BITSHIFT  0
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R12_FALSE      0
NV_097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R12_TRUE       32
#endif
    NAMER(NV_097_SET_CONTROL0),
#if 0
NV_097_SET_CONTROL0_COLOR_SPACE_CONVERT                         0xf0000000
NV_097_SET_CONTROL0_COLOR_SPACE_CONVERT__BITSHIFT               28
NV_097_SET_CONTROL0_COLOR_SPACE_CONVERT_PASS                    0
NV_097_SET_CONTROL0_COLOR_SPACE_CONVERT_CRYCB_TO_RGB            1
NV_097_SET_CONTROL0_COLOR_SPACE_CONVERT_SCRYSCB_TO_RGB          2
NV_097_SET_CONTROL0_PREMULTIPLIEDALPHA                          0x0f000000
NV_097_SET_CONTROL0_PREMULTIPLIEDALPHA__BITSHIFT                24
NV_097_SET_CONTROL0_PREMULTIPLIEDALPHA_FALSE                    0
NV_097_SET_CONTROL0_PREMULTIPLIEDALPHA_TRUE                     1
NV_097_SET_CONTROL0_TEXTUREPERSPECTIVE                          0x00f00000
NV_097_SET_CONTROL0_TEXTUREPERSPECTIVE__BITSHIFT                20
NV_097_SET_CONTROL0_TEXTUREPERSPECTIVE_FALSE                    0
NV_097_SET_CONTROL0_TEXTUREPERSPECTIVE_TRUE                     1
NV_097_SET_CONTROL0_Z_PERSPECTIVE_ENABLE                        0x000f0000
NV_097_SET_CONTROL0_Z_PERSPECTIVE_ENABLE__BITSHIFT              16
NV_097_SET_CONTROL0_Z_PERSPECTIVE_ENABLE_FALSE                  0
NV_097_SET_CONTROL0_Z_PERSPECTIVE_ENABLE_TRUE                   1
NV_097_SET_CONTROL0_Z_FORMAT                                    0x0000f000
NV_097_SET_CONTROL0_Z_FORMAT__BITSHIFT                          12
NV_097_SET_CONTROL0_Z_FORMAT_FIXED                              0
NV_097_SET_CONTROL0_Z_FORMAT_FLOAT                              1
NV_097_SET_CONTROL0_STENCIL_WRITE_ENABLE                        0x000000ff
NV_097_SET_CONTROL0_STENCIL_WRITE_ENABLE__BITSHIFT              0
NV_097_SET_CONTROL0_STENCIL_WRITE_ENABLE_FALSE                  0
NV_097_SET_CONTROL0_STENCIL_WRITE_ENABLE_TRUE                   1
#endif
    NAMER(NV_097_SET_LIGHT_CONTROL),
#if 0
NV_097_SET_LIGHT_CONTROL_LOCALEYE                               0x00010000
NV_097_SET_LIGHT_CONTROL_LOCALEYE__BITSHIFT                     16
NV_097_SET_LIGHT_CONTROL_LOCALEYE_FALSE                         0
NV_097_SET_LIGHT_CONTROL_LOCALEYE_TRUE                          1
NV_097_SET_LIGHT_CONTROL_SOUT                                   0xfffe0000
NV_097_SET_LIGHT_CONTROL_SOUT__BITSHIFT                         17
NV_097_SET_LIGHT_CONTROL_SOUT_ZERO_OUT                          0
NV_097_SET_LIGHT_CONTROL_SOUT_PASSTHROUGH                       1
NV_097_SET_LIGHT_CONTROL_SEPARATE_SPECULAR_EN                   0x00000003
NV_097_SET_LIGHT_CONTROL_SEPARATE_SPECULAR_EN__BITSHIFT         0
NV_097_SET_LIGHT_CONTROL_SEPARATE_SPECULAR_EN_FALSE             0
NV_097_SET_LIGHT_CONTROL_SEPARATE_SPECULAR_EN_TRUE              1
#endif
    NAMER(NV_097_SET_COLOR_MATERIAL),
#if 0
NV_097_SET_COLOR_MATERIAL_EMISSIVE_MATERIAL                     0x00000003
NV_097_SET_COLOR_MATERIAL_EMISSIVE_MATERIAL__BITSHIFT           0
NV_097_SET_COLOR_MATERIAL_EMISSIVE_MATERIAL_DISABLE             0
NV_097_SET_COLOR_MATERIAL_EMISSIVE_MATERIAL_DIFFUSE_VTX_COLOR   1
NV_097_SET_COLOR_MATERIAL_EMISSIVE_MATERIAL_SPECULAR_VTX_COLOR  2
NV_097_SET_COLOR_MATERIAL_AMBIENT_MATERIAL                      0x0000000c
NV_097_SET_COLOR_MATERIAL_AMBIENT_MATERIAL__BITSHIFT            2
NV_097_SET_COLOR_MATERIAL_AMBIENT_MATERIAL_DISABLE              0
NV_097_SET_COLOR_MATERIAL_AMBIENT_MATERIAL_DIFFUSE_VTX_COLOR    1
NV_097_SET_COLOR_MATERIAL_AMBIENT_MATERIAL_SPECULAR_VTX_COLOR   2
NV_097_SET_COLOR_MATERIAL_DIFF_MATERIAL                         0x00000030
NV_097_SET_COLOR_MATERIAL_DIFF_MATERIAL__BITSHIFT               4
NV_097_SET_COLOR_MATERIAL_DIFF_MATERIAL_DISABLE                 0
NV_097_SET_COLOR_MATERIAL_DIFF_MATERIAL_DIFFUSE_VTX_COLOR       1
NV_097_SET_COLOR_MATERIAL_DIFF_MATERIAL_SPECULAR_VTX_COLOR      2
NV_097_SET_COLOR_MATERIAL_SPECULAR_MATERIAL                     0x000000c0
NV_097_SET_COLOR_MATERIAL_SPECULAR_MATERIAL__BITSHIFT           6
NV_097_SET_COLOR_MATERIAL_SPECULAR_MATERIAL_DISABLE             0
NV_097_SET_COLOR_MATERIAL_SPECULAR_MATERIAL_DIFFUSE_VTX_COLOR   1
NV_097_SET_COLOR_MATERIAL_SPECULAR_MATERIAL_SPECULAR_VTX_COLOR  2
NV_097_SET_COLOR_MATERIAL_BACK_EMISSIVE_MATERIAL                0x00000300
NV_097_SET_COLOR_MATERIAL_BACK_EMISSIVE_MATERIAL__BITSHIFT      8
NV_097_SET_COLOR_MATERIAL_BACK_EMISSIVE_MATERIAL_DISABLE        0
NV_097_SET_COLOR_MATERIAL_BACK_EMISSIVE_MATERIAL_DIFF_VTX_COLOR 1
NV_097_SET_COLOR_MATERIAL_BACK_EMISSIVE_MATERIAL_SPEC_VTX_COLOR 2
NV_097_SET_COLOR_MATERIAL_BACK_AMBIENT_MATERIAL                 0x00000c00
NV_097_SET_COLOR_MATERIAL_BACK_AMBIENT_MATERIAL__BITSHIFT       10
NV_097_SET_COLOR_MATERIAL_BACK_AMBIENT_MATERIAL_DISABLE         0
NV_097_SET_COLOR_MATERIAL_BACK_AMBIENT_MATERIAL_DIFF_VTX_COLOR  1
NV_097_SET_COLOR_MATERIAL_BACK_AMBIENT_MATERIAL_SPEC_VTX_COLOR  2
NV_097_SET_COLOR_MATERIAL_BACK_DIFF_MATERIAL                    0x00003000
NV_097_SET_COLOR_MATERIAL_BACK_DIFF_MATERIAL__BITSHIFT          12
NV_097_SET_COLOR_MATERIAL_BACK_DIFF_MATERIAL_DISABLE            0
NV_097_SET_COLOR_MATERIAL_BACK_DIFF_MATERIAL_DIFF_VTX_COLOR     1
NV_097_SET_COLOR_MATERIAL_BACK_DIFF_MATERIAL_SPEC_VTX_COLOR     2
NV_097_SET_COLOR_MATERIAL_BACK_SPECULAR_MATERIAL                0x0000c000
NV_097_SET_COLOR_MATERIAL_BACK_SPECULAR_MATERIAL__BITSHIFT      14
NV_097_SET_COLOR_MATERIAL_BACK_SPECULAR_MATERIAL_DISABLE        0
NV_097_SET_COLOR_MATERIAL_BACK_SPECULAR_MATERIAL_DIFF_VTX_COLOR 1
NV_097_SET_COLOR_MATERIAL_BACK_SPECULAR_MATERIAL_SPEC_VTX_COLOR 2
#endif
    NAMER(NV_097_SET_FOG_MODE),
#if 0
NV_097_SET_FOG_MODE_V_LINEAR                                    0x00002601
NV_097_SET_FOG_MODE_V_EXP                                       0x00000800
NV_097_SET_FOG_MODE_V_EXP2                                      0x00000801
NV_097_SET_FOG_MODE_V_EXP_ABS                                   0x00000802
NV_097_SET_FOG_MODE_V_EXP2_ABS                                  0x00000803
NV_097_SET_FOG_MODE_V_LINEAR_ABS                                0x00000804
#endif
    NAMER(NV_097_SET_FOG_GEN_MODE),
#if 0
NV_097_SET_FOG_GEN_MODE_V_SPEC_ALPHA                            0
NV_097_SET_FOG_GEN_MODE_V_RADIAL                                1
NV_097_SET_FOG_GEN_MODE_V_PLANAR                                2
NV_097_SET_FOG_GEN_MODE_V_ABS_PLANAR                            3
NV_097_SET_FOG_GEN_MODE_V_FOG_X                                 6
#endif
    NAMER(NV_097_SET_FOG_ENABLE),
#if 0
NV_097_SET_FOG_ENABLE_V_FALSE                                   0
NV_097_SET_FOG_ENABLE_V_TRUE                                    1
#endif
    NAMER(NV_097_SET_FOG_COLOR),
#if 0
NV_097_SET_FOG_COLOR_RED                                        0x000000ff
NV_097_SET_FOG_COLOR_RED__BITSHIFT                              0
NV_097_SET_FOG_COLOR_GREEN                                      0x0000ff00
NV_097_SET_FOG_COLOR_GREEN__BITSHIFT                            8
NV_097_SET_FOG_COLOR_BLUE                                       0x00ff0000
NV_097_SET_FOG_COLOR_BLUE__BITSHIFT                             16
NV_097_SET_FOG_COLOR_ALPHA                                      0xff000000
NV_097_SET_FOG_COLOR_ALPHA__BITSHIFT                            24
#endif
    NAMER(NV_097_SET_COLOR_KEY_COLOR_0),
    NAMER(NV_097_SET_COLOR_KEY_COLOR_1),
    NAMER(NV_097_SET_COLOR_KEY_COLOR_2),
    NAMER(NV_097_SET_COLOR_KEY_COLOR_3),
    NAMER(NV_097_SET_WINDOW_CLIP_TYPE),
#if 0
NV_097_SET_WINDOW_CLIP_TYPE_V_INCLUSIVE                         0
NV_097_SET_WINDOW_CLIP_TYPE_V_EXCLUSIVE                         1
#endif
    NAMER(NV_097_SET_WINDOW_CLIP_HORIZONTAL_0),
    NAMER(NV_097_SET_WINDOW_CLIP_HORIZONTAL_1),
    NAMER(NV_097_SET_WINDOW_CLIP_HORIZONTAL_2),
    NAMER(NV_097_SET_WINDOW_CLIP_HORIZONTAL_3),
    NAMER(NV_097_SET_WINDOW_CLIP_HORIZONTAL_4),
    NAMER(NV_097_SET_WINDOW_CLIP_HORIZONTAL_5),
    NAMER(NV_097_SET_WINDOW_CLIP_HORIZONTAL_6),
    NAMER(NV_097_SET_WINDOW_CLIP_HORIZONTAL_7),
#if 0
NV_097_SET_WINDOW_CLIP_HORIZONTAL_XMIN                          0x00000fff
NV_097_SET_WINDOW_CLIP_HORIZONTAL_XMIN__BITSHIFT                0
NV_097_SET_WINDOW_CLIP_HORIZONTAL_XMAX                          0x0fff0000
NV_097_SET_WINDOW_CLIP_HORIZONTAL_XMAX__BITSHIFT                16
#endif
    NAMER(NV_097_SET_WINDOW_CLIP_VERTICAL_0),
    NAMER(NV_097_SET_WINDOW_CLIP_VERTICAL_1),
    NAMER(NV_097_SET_WINDOW_CLIP_VERTICAL_2),
    NAMER(NV_097_SET_WINDOW_CLIP_VERTICAL_3),
    NAMER(NV_097_SET_WINDOW_CLIP_VERTICAL_4),
    NAMER(NV_097_SET_WINDOW_CLIP_VERTICAL_5),
    NAMER(NV_097_SET_WINDOW_CLIP_VERTICAL_6),
    NAMER(NV_097_SET_WINDOW_CLIP_VERTICAL_7),
#if 0
NV_097_SET_WINDOW_CLIP_VERTICAL_YMIN                            0x00000fff
NV_097_SET_WINDOW_CLIP_VERTICAL_YMIN__BITSHIFT                  0
NV_097_SET_WINDOW_CLIP_VERTICAL_YMAX                            0x0fff0000
NV_097_SET_WINDOW_CLIP_VERTICAL_YMAX__BITSHIFT                  16
#endif
    NAMER(NV_097_SET_ALPHA_TEST_ENABLE),
#if 0
NV_097_SET_ALPHA_TEST_ENABLE_V_FALSE                            0
NV_097_SET_ALPHA_TEST_ENABLE_V_TRUE                             1
#endif
    NAMER(NV_097_SET_BLEND_ENABLE),
#if 0
NV_097_SET_BLEND_ENABLE_V_FALSE                                 0
NV_097_SET_BLEND_ENABLE_V_TRUE                                  1
#endif
    NAMER(NV_097_SET_CULL_FACE_ENABLE),
#if 0
NV_097_SET_CULL_FACE_ENABLE_V_FALSE                             0
NV_097_SET_CULL_FACE_ENABLE_V_TRUE                              1
#endif
    NAMER(NV_097_SET_DEPTH_TEST_ENABLE),
#if 0
NV_097_SET_DEPTH_TEST_ENABLE_V_FALSE                            0
NV_097_SET_DEPTH_TEST_ENABLE_V_TRUE                             1
#endif
    NAMER(NV_097_SET_DITHER_ENABLE),
#if 0
NV_097_SET_DITHER_ENABLE_V_FALSE                                0
NV_097_SET_DITHER_ENABLE_V_TRUE                                 1
#endif
    NAMER(NV_097_SET_LIGHTING_ENABLE),
#if 0
NV_097_SET_LIGHTING_ENABLE_V_FALSE                              0
NV_097_SET_LIGHTING_ENABLE_V_TRUE                               1
#endif
    NAMER(NV_097_SET_POINT_PARAMS_ENABLE),
#if 0
NV_097_SET_POINT_PARAMS_ENABLE_V_FALSE                          0
NV_097_SET_POINT_PARAMS_ENABLE_V_TRUE                           1
#endif
    NAMER(NV_097_SET_POINT_SMOOTH_ENABLE),
#if 0
NV_097_SET_POINT_SMOOTH_ENABLE_V_FALSE                          0
NV_097_SET_POINT_SMOOTH_ENABLE_V_TRUE                           1
#endif
    NAMER(NV_097_SET_LINE_SMOOTH_ENABLE),
#if 0
NV_097_SET_LINE_SMOOTH_ENABLE_V_FALSE                           0
NV_097_SET_LINE_SMOOTH_ENABLE_V_TRUE                            1
#endif
    NAMER(NV_097_SET_POLY_SMOOTH_ENABLE),
#if 0
NV_097_SET_POLY_SMOOTH_ENABLE_V_FALSE                           0
NV_097_SET_POLY_SMOOTH_ENABLE_V_TRUE                            1
#endif
    NAMER(NV_097_SET_STIPPLE_CONTROL),
#if 0
NV_097_SET_STIPPLE_CONTROL_V_OFF                                0
NV_097_SET_STIPPLE_CONTROL_V_POLYGON                            1
#endif
    NAMER(NV_097_SET_STIPPLE_PATTERN_0),
    NAMER(NV_097_SET_STIPPLE_PATTERN_1),
    NAMER(NV_097_SET_STIPPLE_PATTERN_2),
    NAMER(NV_097_SET_STIPPLE_PATTERN_3),
    NAMER(NV_097_SET_STIPPLE_PATTERN_4),
    NAMER(NV_097_SET_STIPPLE_PATTERN_5),
    NAMER(NV_097_SET_STIPPLE_PATTERN_6),
    NAMER(NV_097_SET_STIPPLE_PATTERN_7),
    NAMER(NV_097_SET_STIPPLE_PATTERN_8),
    NAMER(NV_097_SET_STIPPLE_PATTERN_9),
    NAMER(NV_097_SET_STIPPLE_PATTERN_10),
    NAMER(NV_097_SET_STIPPLE_PATTERN_11),
    NAMER(NV_097_SET_STIPPLE_PATTERN_12),
    NAMER(NV_097_SET_STIPPLE_PATTERN_13),
    NAMER(NV_097_SET_STIPPLE_PATTERN_14),
    NAMER(NV_097_SET_STIPPLE_PATTERN_15),
    NAMER(NV_097_SET_STIPPLE_PATTERN_16),
    NAMER(NV_097_SET_STIPPLE_PATTERN_17),
    NAMER(NV_097_SET_STIPPLE_PATTERN_18),
    NAMER(NV_097_SET_STIPPLE_PATTERN_19),
    NAMER(NV_097_SET_STIPPLE_PATTERN_20),
    NAMER(NV_097_SET_STIPPLE_PATTERN_21),
    NAMER(NV_097_SET_STIPPLE_PATTERN_22),
    NAMER(NV_097_SET_STIPPLE_PATTERN_23),
    NAMER(NV_097_SET_STIPPLE_PATTERN_24),
    NAMER(NV_097_SET_STIPPLE_PATTERN_25),
    NAMER(NV_097_SET_STIPPLE_PATTERN_26),
    NAMER(NV_097_SET_STIPPLE_PATTERN_27),
    NAMER(NV_097_SET_STIPPLE_PATTERN_28),
    NAMER(NV_097_SET_STIPPLE_PATTERN_29),
    NAMER(NV_097_SET_STIPPLE_PATTERN_30),
    NAMER(NV_097_SET_STIPPLE_PATTERN_31),
    NAMER(NV_097_SET_SKIN_MODE),
#if 0
NV_097_SET_SKIN_MODE_V_OFF                                      0
NV_097_SET_SKIN_MODE_V_2G                                       1
NV_097_SET_SKIN_MODE_V_2                                        2
NV_097_SET_SKIN_MODE_V_3G                                       3
NV_097_SET_SKIN_MODE_V_3                                        4
NV_097_SET_SKIN_MODE_V_4G                                       5
NV_097_SET_SKIN_MODE_V_4                                        6
#endif
    NAMER(NV_097_SET_STENCIL_TEST_ENABLE),
#if 0
NV_097_SET_STENCIL_TEST_ENABLE_V_FALSE                          0
NV_097_SET_STENCIL_TEST_ENABLE_V_TRUE                           1
#endif
    NAMER(NV_097_SET_POLY_OFFSET_POINT_ENABLE),
#if 0
NV_097_SET_POLY_OFFSET_POINT_ENABLE_V_FALSE                     0
NV_097_SET_POLY_OFFSET_POINT_ENABLE_V_TRUE                      1
#endif
    NAMER(NV_097_SET_POLY_OFFSET_LINE_ENABLE),
#if 0
NV_097_SET_POLY_OFFSET_LINE_ENABLE_V_FALSE                      0
NV_097_SET_POLY_OFFSET_LINE_ENABLE_V_TRUE                       1
#endif
    NAMER(NV_097_SET_POLY_OFFSET_FILL_ENABLE),
#if 0
NV_097_SET_POLY_OFFSET_FILL_ENABLE_V_FALSE                      0
NV_097_SET_POLY_OFFSET_FILL_ENABLE_V_TRUE                       1
#endif
    NAMER(NV_097_SET_ALPHA_FUNC),
#if 0
NV_097_SET_ALPHA_FUNC_V_NEVER                                   0x00000200
NV_097_SET_ALPHA_FUNC_V_LESS                                    0x00000201
NV_097_SET_ALPHA_FUNC_V_EQUAL                                   0x00000202
NV_097_SET_ALPHA_FUNC_V_LEQUAL                                  0x00000203
NV_097_SET_ALPHA_FUNC_V_GREATER                                 0x00000204
NV_097_SET_ALPHA_FUNC_V_NOTEQUAL                                0x00000205
NV_097_SET_ALPHA_FUNC_V_GEQUAL                                  0x00000206
NV_097_SET_ALPHA_FUNC_V_ALWAYS                                  0x00000207
#endif
    NAMER(NV_097_SET_ALPHA_REF),
    NAMER(NV_097_SET_BLEND_FUNC_SFACTOR),
#if 0
NV_097_SET_BLEND_FUNC_SFACTOR_V_ZERO                            0x00000000
NV_097_SET_BLEND_FUNC_SFACTOR_V_ONE                             0x00000001
NV_097_SET_BLEND_FUNC_SFACTOR_V_SRC_COLOR                       0x00000300
NV_097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_SRC_COLOR             0x00000301
NV_097_SET_BLEND_FUNC_SFACTOR_V_SRC_ALPHA                       0x00000302
NV_097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_SRC_ALPHA             0x00000303
NV_097_SET_BLEND_FUNC_SFACTOR_V_DST_ALPHA                       0x00000304
NV_097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_DST_ALPHA             0x00000305
NV_097_SET_BLEND_FUNC_SFACTOR_V_DST_COLOR                       0x00000306
NV_097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_DST_COLOR             0x00000307
NV_097_SET_BLEND_FUNC_SFACTOR_V_SRC_ALPHA_SATURATE              0x00000308
NV_097_SET_BLEND_FUNC_SFACTOR_V_CONSTANT_COLOR                  0x00008001
NV_097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_CONSTANT_COLOR        0x00008002
NV_097_SET_BLEND_FUNC_SFACTOR_V_CONSTANT_ALPHA                  0x00008003
NV_097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_CONSTANT_ALPHA        0x00008004
#endif
    NAMER(NV_097_SET_BLEND_FUNC_DFACTOR),
#if 0
NV_097_SET_BLEND_FUNC_DFACTOR_V_ZERO                            0x00000000
NV_097_SET_BLEND_FUNC_DFACTOR_V_ONE                             0x00000001
NV_097_SET_BLEND_FUNC_DFACTOR_V_SRC_COLOR                       0x00000300
NV_097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_SRC_COLOR             0x00000301
NV_097_SET_BLEND_FUNC_DFACTOR_V_SRC_ALPHA                       0x00000302
NV_097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_SRC_ALPHA             0x00000303
NV_097_SET_BLEND_FUNC_DFACTOR_V_DST_ALPHA                       0x00000304
NV_097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_DST_ALPHA             0x00000305
NV_097_SET_BLEND_FUNC_DFACTOR_V_DST_COLOR                       0x00000306
NV_097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_DST_COLOR             0x00000307
NV_097_SET_BLEND_FUNC_DFACTOR_V_SRC_ALPHA_SATURATE              0x00000308
NV_097_SET_BLEND_FUNC_DFACTOR_V_CONSTANT_COLOR                  0x00008001
NV_097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_CONSTANT_COLOR        0x00008002
NV_097_SET_BLEND_FUNC_DFACTOR_V_CONSTANT_ALPHA                  0x00008003
NV_097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_CONSTANT_ALPHA        0x00008004
#endif
    NAMER(NV_097_SET_BLEND_COLOR),
    NAMER(NV_097_SET_BLEND_EQUATION),
#if 0
NV_097_SET_BLEND_EQUATION_V_FUNC_SUBTRACT                       0x0000800a
NV_097_SET_BLEND_EQUATION_V_FUNC_REVERSE_SUBTRACT               0x0000800b
NV_097_SET_BLEND_EQUATION_V_FUNC_ADD                            0x00008006
NV_097_SET_BLEND_EQUATION_V_MIN                                 0x00008007
NV_097_SET_BLEND_EQUATION_V_MAX                                 0x00008008
NV_097_SET_BLEND_EQUATION_V_FUNC_REVERSE_SUBTRACT_SIGNED        0x0000f005
NV_097_SET_BLEND_EQUATION_V_FUNC_ADD_SIGNED                     0x0000f006
#endif
    NAMER(NV_097_SET_DEPTH_FUNC),
#if 0
NV_097_SET_DEPTH_FUNC_V_NEVER                                   0x00000200
NV_097_SET_DEPTH_FUNC_V_LESS                                    0x00000201
NV_097_SET_DEPTH_FUNC_V_EQUAL                                   0x00000202
NV_097_SET_DEPTH_FUNC_V_LEQUAL                                  0x00000203
NV_097_SET_DEPTH_FUNC_V_GREATER                                 0x00000204
NV_097_SET_DEPTH_FUNC_V_NOTEQUAL                                0x00000205
NV_097_SET_DEPTH_FUNC_V_GEQUAL                                  0x00000206
NV_097_SET_DEPTH_FUNC_V_ALWAYS                                  0x00000207
#endif
    NAMER(NV_097_SET_COLOR_MASK),
#if 0
NV_097_SET_COLOR_MASK_ALPHA_WRITE_ENABLE                        0xff000000
NV_097_SET_COLOR_MASK_ALPHA_WRITE_ENABLE__BITSHIFT              24
NV_097_SET_COLOR_MASK_ALPHA_WRITE_ENABLE_FALSE                  0
NV_097_SET_COLOR_MASK_ALPHA_WRITE_ENABLE_TRUE                   1
NV_097_SET_COLOR_MASK_RED_WRITE_ENABLE                          0x00ff0000
NV_097_SET_COLOR_MASK_RED_WRITE_ENABLE__BITSHIFT                16
NV_097_SET_COLOR_MASK_RED_WRITE_ENABLE_FALSE                    0
NV_097_SET_COLOR_MASK_RED_WRITE_ENABLE_TRUE                     1
NV_097_SET_COLOR_MASK_GREEN_WRITE_ENABLE                        0x0000ff00
NV_097_SET_COLOR_MASK_GREEN_WRITE_ENABLE__BITSHIFT              8
NV_097_SET_COLOR_MASK_GREEN_WRITE_ENABLE_FALSE                  0
NV_097_SET_COLOR_MASK_GREEN_WRITE_ENABLE_TRUE                   1
NV_097_SET_COLOR_MASK_BLUE_WRITE_ENABLE                         0x000000ff
NV_097_SET_COLOR_MASK_BLUE_WRITE_ENABLE__BITSHIFT               0
NV_097_SET_COLOR_MASK_BLUE_WRITE_ENABLE_FALSE                   0
NV_097_SET_COLOR_MASK_BLUE_WRITE_ENABLE_TRUE                    1
#endif
    NAMER(NV_097_SET_DEPTH_MASK),
#if 0
NV_097_SET_DEPTH_MASK_V_FALSE                                   0
NV_097_SET_DEPTH_MASK_V_TRUE                                    1
#endif
    NAMER(NV_097_SET_STENCIL_MASK),
    NAMER(NV_097_SET_STENCIL_FUNC),
#if 0
NV_097_SET_STENCIL_FUNC_V_NEVER                                 0x00000200
NV_097_SET_STENCIL_FUNC_V_LESS                                  0x00000201
NV_097_SET_STENCIL_FUNC_V_EQUAL                                 0x00000202
NV_097_SET_STENCIL_FUNC_V_LEQUAL                                0x00000203
NV_097_SET_STENCIL_FUNC_V_GREATER                               0x00000204
NV_097_SET_STENCIL_FUNC_V_NOTEQUAL                              0x00000205
NV_097_SET_STENCIL_FUNC_V_GEQUAL                                0x00000206
NV_097_SET_STENCIL_FUNC_V_ALWAYS                                0x00000207
#endif
    NAMER(NV_097_SET_STENCIL_FUNC_REF),
    NAMER(NV_097_SET_STENCIL_FUNC_MASK),
    NAMER(NV_097_SET_STENCIL_OP_FAIL),
#if 0
NV_097_SET_STENCIL_OP_FAIL_V_KEEP                               0x00001e00
NV_097_SET_STENCIL_OP_FAIL_V_ZERO                               0x00000000
NV_097_SET_STENCIL_OP_FAIL_V_REPLACE                            0x00001e01
NV_097_SET_STENCIL_OP_FAIL_V_INCRSAT                            0x00001e02
NV_097_SET_STENCIL_OP_FAIL_V_DECRSAT                            0x00001e03
NV_097_SET_STENCIL_OP_FAIL_V_INVERT                             0x0000150a
NV_097_SET_STENCIL_OP_FAIL_V_INCR                               0x00008507
NV_097_SET_STENCIL_OP_FAIL_V_DECR                               0x00008508
#endif
    NAMER(NV_097_SET_STENCIL_OP_ZFAIL),
#if 0
NV_097_SET_STENCIL_OP_ZFAIL_V_KEEP                              0x00001e00
NV_097_SET_STENCIL_OP_ZFAIL_V_ZERO                              0x00000000
NV_097_SET_STENCIL_OP_ZFAIL_V_REPLACE                           0x00001e01
NV_097_SET_STENCIL_OP_ZFAIL_V_INCRSAT                           0x00001e02
NV_097_SET_STENCIL_OP_ZFAIL_V_DECRSAT                           0x00001e03
NV_097_SET_STENCIL_OP_ZFAIL_V_INVERT                            0x0000150a
NV_097_SET_STENCIL_OP_ZFAIL_V_INCR                              0x00008507
NV_097_SET_STENCIL_OP_ZFAIL_V_DECR                              0x00008508
#endif
    NAMER(NV_097_SET_STENCIL_OP_ZPASS),
#if 0
NV_097_SET_STENCIL_OP_ZPASS_V_KEEP                              0x00001e00
NV_097_SET_STENCIL_OP_ZPASS_V_ZERO                              0x00000000
NV_097_SET_STENCIL_OP_ZPASS_V_REPLACE                           0x00001e01
NV_097_SET_STENCIL_OP_ZPASS_V_INCRSAT                           0x00001e02
NV_097_SET_STENCIL_OP_ZPASS_V_DECRSAT                           0x00001e03
NV_097_SET_STENCIL_OP_ZPASS_V_INVERT                            0x0000150a
NV_097_SET_STENCIL_OP_ZPASS_V_INCR                              0x00008507
NV_097_SET_STENCIL_OP_ZPASS_V_DECR                              0x00008508
#endif
    NAMER(NV_097_SET_SHADE_MODE),
#if 0
NV_097_SET_SHADE_MODE_V_FLAT                                    0x00001d00
NV_097_SET_SHADE_MODE_V_SMOOTH                                  0x00001d01
#endif
    NAMER(NV_097_SET_LINE_WIDTH),
    NAMER(NV_097_SET_POLYGON_OFFSET_SCALE_FACTOR),
    NAMER(NV_097_SET_POLYGON_OFFSET_BIAS),
    NAMER(NV_097_SET_FRONT_POLYGON_MODE),
#if 0
NV_097_SET_FRONT_POLYGON_MODE_V_POINT                           0x00001b00
NV_097_SET_FRONT_POLYGON_MODE_V_LINE                            0x00001b01
NV_097_SET_FRONT_POLYGON_MODE_V_FILL                            0x00001b02
#endif
    NAMER(NV_097_SET_BACK_POLYGON_MODE),
#if 0
NV_097_SET_BACK_POLYGON_MODE_V_POINT                            0x00001b00
NV_097_SET_BACK_POLYGON_MODE_V_LINE                             0x00001b01
NV_097_SET_BACK_POLYGON_MODE_V_FILL                             0x00001b02
#endif
    NAMER(NV_097_SET_CLIP_MIN),
    NAMER(NV_097_SET_CLIP_MAX),
    NAMER(NV_097_SET_CULL_FACE),
#if 0
NV_097_SET_CULL_FACE_V_FRONT                                    0x00000404
NV_097_SET_CULL_FACE_V_BACK                                     0x00000405
NV_097_SET_CULL_FACE_V_FRONT_AND_BACK                           0x00000408
#endif
    NAMER(NV_097_SET_FRONT_FACE),
#if 0
NV_097_SET_FRONT_FACE_V_CW                                      0x00000900
NV_097_SET_FRONT_FACE_V_CCW                                     0x00000901
#endif
    NAMER(NV_097_SET_NORMALIZATION_ENABLE),
#if 0
NV_097_SET_NORMALIZATION_ENABLE_V_FALSE                         0
NV_097_SET_NORMALIZATION_ENABLE_V_TRUE                          1
#endif
    NAMER(NV_097_SET_MATERIAL_EMISSION_0),
    NAMER(NV_097_SET_MATERIAL_EMISSION_1),
    NAMER(NV_097_SET_MATERIAL_EMISSION_2),
    NAMER(NV_097_SET_MATERIAL_ALPHA),
    NAMER(NV_097_SET_BACK_MATERIAL_ALPHA),
    NAMER(NV_097_SET_SPECULAR_ENABLE),
#if 0
NV_097_SET_SPECULAR_ENABLE_V_FALSE                              0
NV_097_SET_SPECULAR_ENABLE_V_TRUE                               1
#endif
    NAMER(NV_097_SET_LIGHT_ENABLE_MASK),
#if 0
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT0                             0x00000003
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT0__BITSHIFT                   0
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT0_OFF                         0
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT0_INFINITE                    1
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT0_LOCAL                       2
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT0_SPOT                        3
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT1                             0x0000000c
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT1__BITSHIFT                   2
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT1_OFF                         0
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT1_INFINITE                    1
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT1_LOCAL                       2
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT1_SPOT                        3
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT2                             0x00000030
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT2__BITSHIFT                   4
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT2_OFF                         0
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT2_INFINITE                    1
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT2_LOCAL                       2
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT2_SPOT                        3
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT3                             0x000000c0
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT3__BITSHIFT                   6
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT3_OFF                         0
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT3_INFINITE                    1
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT3_LOCAL                       2
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT3_SPOT                        3
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT4                             0x00000300
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT4__BITSHIFT                   8
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT4_OFF                         0
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT4_INFINITE                    1
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT4_LOCAL                       2
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT4_SPOT                        3
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT5                             0x00000c00
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT5__BITSHIFT                   10
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT5_OFF                         0
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT5_INFINITE                    1
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT5_LOCAL                       2
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT5_SPOT                        3
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT6                             0x00003000
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT6__BITSHIFT                   12
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT6_OFF                         0
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT6_INFINITE                    1
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT6_LOCAL                       2
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT6_SPOT                        3
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT7                             0x0000c000
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT7__BITSHIFT                   14
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT7_OFF                         0
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT7_INFINITE                    1
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT7_LOCAL                       2
NV_097_SET_LIGHT_ENABLE_MASK_LIGHT7_SPOT                        3
#endif
    NAMER(NV_097_SET_VERTEX_DATA2F_M_0_0),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_0_1),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_1_0),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_1_1),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_2_0),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_2_1),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_3_0),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_3_1),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_4_0),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_4_1),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_5_0),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_5_1),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_6_0),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_6_1),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_7_0),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_7_1),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_8_0),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_8_1),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_9_0),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_9_1),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_10_0),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_10_1),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_11_0),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_11_1),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_12_0),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_12_1),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_13_0),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_13_1),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_14_0),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_14_1),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_15_0),
    NAMER(NV_097_SET_VERTEX_DATA2F_M_15_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_0_0),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_0_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_0_2),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_0_3),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_1_0),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_1_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_1_2),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_1_3),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_2_0),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_2_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_2_2),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_2_3),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_3_0),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_3_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_3_2),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_3_3),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_4_0),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_4_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_4_2),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_4_3),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_5_0),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_5_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_5_2),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_5_3),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_6_0),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_6_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_6_2),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_6_3),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_7_0),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_7_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_7_2),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_7_3),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_8_0),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_8_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_8_2),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_8_3),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_9_0),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_9_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_9_2),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_9_3),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_10_0),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_10_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_10_2),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_10_3),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_11_0),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_11_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_11_2),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_11_3),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_12_0),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_12_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_12_2),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_12_3),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_13_0),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_13_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_13_2),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_13_3),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_14_0),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_14_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_14_2),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_14_3),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_15_0),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_15_1),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_15_2),
    NAMER(NV_097_SET_VERTEX_DATA4F_M_15_3),
    NAMER(NV_097_SET_VERTEX_DATA2S_0),
    NAMER(NV_097_SET_VERTEX_DATA2S_1),
    NAMER(NV_097_SET_VERTEX_DATA2S_2),
    NAMER(NV_097_SET_VERTEX_DATA2S_3),
    NAMER(NV_097_SET_VERTEX_DATA2S_4),
    NAMER(NV_097_SET_VERTEX_DATA2S_5),
    NAMER(NV_097_SET_VERTEX_DATA2S_6),
    NAMER(NV_097_SET_VERTEX_DATA2S_7),
    NAMER(NV_097_SET_VERTEX_DATA2S_8),
    NAMER(NV_097_SET_VERTEX_DATA2S_9),
    NAMER(NV_097_SET_VERTEX_DATA2S_10),
    NAMER(NV_097_SET_VERTEX_DATA2S_11),
    NAMER(NV_097_SET_VERTEX_DATA2S_12),
    NAMER(NV_097_SET_VERTEX_DATA2S_13),
    NAMER(NV_097_SET_VERTEX_DATA2S_14),
    NAMER(NV_097_SET_VERTEX_DATA2S_15),
    NAMER(NV_097_SET_VERTEX_DATA4UB_0),
    NAMER(NV_097_SET_VERTEX_DATA4UB_1),
    NAMER(NV_097_SET_VERTEX_DATA4UB_2),
    NAMER(NV_097_SET_VERTEX_DATA4UB_3),
    NAMER(NV_097_SET_VERTEX_DATA4UB_4),
    NAMER(NV_097_SET_VERTEX_DATA4UB_5),
    NAMER(NV_097_SET_VERTEX_DATA4UB_6),
    NAMER(NV_097_SET_VERTEX_DATA4UB_7),
    NAMER(NV_097_SET_VERTEX_DATA4UB_8),
    NAMER(NV_097_SET_VERTEX_DATA4UB_9),
    NAMER(NV_097_SET_VERTEX_DATA4UB_10),
    NAMER(NV_097_SET_VERTEX_DATA4UB_11),
    NAMER(NV_097_SET_VERTEX_DATA4UB_12),
    NAMER(NV_097_SET_VERTEX_DATA4UB_13),
    NAMER(NV_097_SET_VERTEX_DATA4UB_14),
    NAMER(NV_097_SET_VERTEX_DATA4UB_15),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_0_0),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_0_1),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_1_0),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_1_1),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_2_0),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_2_1),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_3_0),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_3_1),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_4_0),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_4_1),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_5_0),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_5_1),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_6_0),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_6_1),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_7_0),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_7_1),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_8_0),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_8_1),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_9_0),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_9_1),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_10_0),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_10_1),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_11_0),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_11_1),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_12_0),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_12_1),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_13_0),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_13_1),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_14_0),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_14_1),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_15_0),
    NAMER(NV_097_SET_VERTEX_DATA4S_M_15_1),
    NAMER(NV_097_SET_TEXGEN_S_0),
    NAMER(NV_097_SET_TEXGEN_S_1),
    NAMER(NV_097_SET_TEXGEN_S_2),
    NAMER(NV_097_SET_TEXGEN_S_3),
#if 0
NV_097_SET_TEXGEN_S_V_DISABLE                                   0x00000000
NV_097_SET_TEXGEN_S_V_NORMAL_MAP                                0x00008511
NV_097_SET_TEXGEN_S_V_REFLECTION_MAP                            0x00008512
NV_097_SET_TEXGEN_S_V_EYE_LINEAR                                0x00002400
NV_097_SET_TEXGEN_S_V_OBJECT_LINEAR                             0x00002401
NV_097_SET_TEXGEN_S_V_SPHERE_MAP                                0x00002402
#endif
    NAMER(NV_097_SET_TEXGEN_T_0),
    NAMER(NV_097_SET_TEXGEN_T_1),
    NAMER(NV_097_SET_TEXGEN_T_2),
    NAMER(NV_097_SET_TEXGEN_T_3),
#if 0
NV_097_SET_TEXGEN_T_V_DISABLE                                   0x00000000
NV_097_SET_TEXGEN_T_V_NORMAL_MAP                                0x00008511
NV_097_SET_TEXGEN_T_V_REFLECTION_MAP                            0x00008512
NV_097_SET_TEXGEN_T_V_EYE_LINEAR                                0x00002400
NV_097_SET_TEXGEN_T_V_OBJECT_LINEAR                             0x00002401
NV_097_SET_TEXGEN_T_V_SPHERE_MAP                                0x00002402
#endif
    NAMER(NV_097_SET_TEXGEN_R_0),
    NAMER(NV_097_SET_TEXGEN_R_1),
    NAMER(NV_097_SET_TEXGEN_R_2),
    NAMER(NV_097_SET_TEXGEN_R_3),
#if 0
NV_097_SET_TEXGEN_R_V_DISABLE                                   0x00000000
NV_097_SET_TEXGEN_R_V_NORMAL_MAP                                0x00008511
NV_097_SET_TEXGEN_R_V_REFLECTION_MAP                            0x00008512
NV_097_SET_TEXGEN_R_V_EYE_LINEAR                                0x00002400
NV_097_SET_TEXGEN_R_V_OBJECT_LINEAR                             0x00002401
#endif
    NAMER(NV_097_SET_TEXGEN_Q_0),
    NAMER(NV_097_SET_TEXGEN_Q_1),
    NAMER(NV_097_SET_TEXGEN_Q_2),
    NAMER(NV_097_SET_TEXGEN_Q_3),
#if 0
NV_097_SET_TEXGEN_Q_V_DISABLE                                   0x00000000
NV_097_SET_TEXGEN_Q_V_EYE_LINEAR                                0x00002400
NV_097_SET_TEXGEN_Q_V_OBJECT_LINEAR                             0x00002401
#endif
    NAMER(NV_097_SET_TEXGEN_VIEW_MODEL),
#if 0
NV_097_SET_TEXGEN_VIEW_MODEL_V_LOCAL_VIEWER                     0
NV_097_SET_TEXGEN_VIEW_MODEL_V_INFINITE_VIEWER                  1
#endif
    NAMER(NV_097_SET_TEXTURE_MATRIX_ENABLE_0),
    NAMER(NV_097_SET_TEXTURE_MATRIX_ENABLE_1),
    NAMER(NV_097_SET_TEXTURE_MATRIX_ENABLE_2),
    NAMER(NV_097_SET_TEXTURE_MATRIX_ENABLE_3),
#if 0
NV_097_SET_TEXTURE_MATRIX_ENABLE_V_FALSE                        0
NV_097_SET_TEXTURE_MATRIX_ENABLE_V_TRUE                         1
#endif
    NAMER(NV_097_SET_POINT_SIZE),
    NAMER(NV_097_SET_SWATH_WIDTH),
#if 0
NV_097_SET_SWATH_WIDTH_V_8                                      0
NV_097_SET_SWATH_WIDTH_V_16                                     1
NV_097_SET_SWATH_WIDTH_V_32                                     2
NV_097_SET_SWATH_WIDTH_V_64                                     3
NV_097_SET_SWATH_WIDTH_V_128                                    4
NV_097_SET_SWATH_WIDTH_V_OFF                                    15
#endif
    NAMER(NV_097_SET_FLAT_SHADE_OP),
#if 0
NV_097_SET_FLAT_SHADE_OP_V_LAST_VTX                             0
NV_097_SET_FLAT_SHADE_OP_V_FIRST_VTX                            1
#endif
    NAMER(NV_097_SET_PROJECTION_MATRIX_0),
    NAMER(NV_097_SET_PROJECTION_MATRIX_1),
    NAMER(NV_097_SET_PROJECTION_MATRIX_2),
    NAMER(NV_097_SET_PROJECTION_MATRIX_3),
    NAMER(NV_097_SET_PROJECTION_MATRIX_4),
    NAMER(NV_097_SET_PROJECTION_MATRIX_5),
    NAMER(NV_097_SET_PROJECTION_MATRIX_6),
    NAMER(NV_097_SET_PROJECTION_MATRIX_7),
    NAMER(NV_097_SET_PROJECTION_MATRIX_8),
    NAMER(NV_097_SET_PROJECTION_MATRIX_9),
    NAMER(NV_097_SET_PROJECTION_MATRIX_10),
    NAMER(NV_097_SET_PROJECTION_MATRIX_11),
    NAMER(NV_097_SET_PROJECTION_MATRIX_12),
    NAMER(NV_097_SET_PROJECTION_MATRIX_13),
    NAMER(NV_097_SET_PROJECTION_MATRIX_14),
    NAMER(NV_097_SET_PROJECTION_MATRIX_15),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX0_0),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX0_1),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX0_2),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX0_3),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX0_4),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX0_5),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX0_6),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX0_7),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX0_8),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX0_9),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX0_10),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX0_11),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX0_12),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX0_13),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX0_14),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX0_15),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX1_0),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX1_1),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX1_2),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX1_3),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX1_4),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX1_5),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX1_6),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX1_7),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX1_8),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX1_9),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX1_10),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX1_11),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX1_12),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX1_13),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX1_14),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX1_15),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX2_0),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX2_1),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX2_2),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX2_3),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX2_4),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX2_5),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX2_6),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX2_7),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX2_8),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX2_9),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX2_10),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX2_11),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX2_12),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX2_13),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX2_14),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX2_15),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX3_0),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX3_1),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX3_2),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX3_3),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX3_4),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX3_5),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX3_6),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX3_7),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX3_8),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX3_9),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX3_10),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX3_11),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX3_12),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX3_13),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX3_14),
    NAMER(NV_097_SET_MODEL_VIEW_MATRIX3_15),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX0_0),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX0_1),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX0_2),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX0_3),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX0_4),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX0_5),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX0_6),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX0_7),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX0_8),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX0_9),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX0_10),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX0_11),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX0_12),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX0_13),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX0_14),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX0_15),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX1_0),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX1_1),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX1_2),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX1_3),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX1_4),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX1_5),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX1_6),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX1_7),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX1_8),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX1_9),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX1_10),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX1_11),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX1_12),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX1_13),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX1_14),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX1_15),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX2_0),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX2_1),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX2_2),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX2_3),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX2_4),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX2_5),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX2_6),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX2_7),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX2_8),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX2_9),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX2_10),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX2_11),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX2_12),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX2_13),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX2_14),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX2_15),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX3_0),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX3_1),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX3_2),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX3_3),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX3_4),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX3_5),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX3_6),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX3_7),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX3_8),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX3_9),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX3_10),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX3_11),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX3_12),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX3_13),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX3_14),
    NAMER(NV_097_SET_INVERSE_MODEL_VIEW_MATRIX3_15),
    NAMER(NV_097_SET_COMPOSITE_MATRIX_0),
    NAMER(NV_097_SET_COMPOSITE_MATRIX_1),
    NAMER(NV_097_SET_COMPOSITE_MATRIX_2),
    NAMER(NV_097_SET_COMPOSITE_MATRIX_3),
    NAMER(NV_097_SET_COMPOSITE_MATRIX_4),
    NAMER(NV_097_SET_COMPOSITE_MATRIX_5),
    NAMER(NV_097_SET_COMPOSITE_MATRIX_6),
    NAMER(NV_097_SET_COMPOSITE_MATRIX_7),
    NAMER(NV_097_SET_COMPOSITE_MATRIX_8),
    NAMER(NV_097_SET_COMPOSITE_MATRIX_9),
    NAMER(NV_097_SET_COMPOSITE_MATRIX_10),
    NAMER(NV_097_SET_COMPOSITE_MATRIX_11),
    NAMER(NV_097_SET_COMPOSITE_MATRIX_12),
    NAMER(NV_097_SET_COMPOSITE_MATRIX_13),
    NAMER(NV_097_SET_COMPOSITE_MATRIX_14),
    NAMER(NV_097_SET_COMPOSITE_MATRIX_15),
    NAMER(NV_097_SET_TEXTURE_MATRIX0_0),
    NAMER(NV_097_SET_TEXTURE_MATRIX0_1),
    NAMER(NV_097_SET_TEXTURE_MATRIX0_2),
    NAMER(NV_097_SET_TEXTURE_MATRIX0_3),
    NAMER(NV_097_SET_TEXTURE_MATRIX0_4),
    NAMER(NV_097_SET_TEXTURE_MATRIX0_5),
    NAMER(NV_097_SET_TEXTURE_MATRIX0_6),
    NAMER(NV_097_SET_TEXTURE_MATRIX0_7),
    NAMER(NV_097_SET_TEXTURE_MATRIX0_8),
    NAMER(NV_097_SET_TEXTURE_MATRIX0_9),
    NAMER(NV_097_SET_TEXTURE_MATRIX0_10),
    NAMER(NV_097_SET_TEXTURE_MATRIX0_11),
    NAMER(NV_097_SET_TEXTURE_MATRIX0_12),
    NAMER(NV_097_SET_TEXTURE_MATRIX0_13),
    NAMER(NV_097_SET_TEXTURE_MATRIX0_14),
    NAMER(NV_097_SET_TEXTURE_MATRIX0_15),
    NAMER(NV_097_SET_TEXTURE_MATRIX1_0),
    NAMER(NV_097_SET_TEXTURE_MATRIX1_1),
    NAMER(NV_097_SET_TEXTURE_MATRIX1_2),
    NAMER(NV_097_SET_TEXTURE_MATRIX1_3),
    NAMER(NV_097_SET_TEXTURE_MATRIX1_4),
    NAMER(NV_097_SET_TEXTURE_MATRIX1_5),
    NAMER(NV_097_SET_TEXTURE_MATRIX1_6),
    NAMER(NV_097_SET_TEXTURE_MATRIX1_7),
    NAMER(NV_097_SET_TEXTURE_MATRIX1_8),
    NAMER(NV_097_SET_TEXTURE_MATRIX1_9),
    NAMER(NV_097_SET_TEXTURE_MATRIX1_10),
    NAMER(NV_097_SET_TEXTURE_MATRIX1_11),
    NAMER(NV_097_SET_TEXTURE_MATRIX1_12),
    NAMER(NV_097_SET_TEXTURE_MATRIX1_13),
    NAMER(NV_097_SET_TEXTURE_MATRIX1_14),
    NAMER(NV_097_SET_TEXTURE_MATRIX1_15),
    NAMER(NV_097_SET_TEXTURE_MATRIX2_0),
    NAMER(NV_097_SET_TEXTURE_MATRIX2_1),
    NAMER(NV_097_SET_TEXTURE_MATRIX2_2),
    NAMER(NV_097_SET_TEXTURE_MATRIX2_3),
    NAMER(NV_097_SET_TEXTURE_MATRIX2_4),
    NAMER(NV_097_SET_TEXTURE_MATRIX2_5),
    NAMER(NV_097_SET_TEXTURE_MATRIX2_6),
    NAMER(NV_097_SET_TEXTURE_MATRIX2_7),
    NAMER(NV_097_SET_TEXTURE_MATRIX2_8),
    NAMER(NV_097_SET_TEXTURE_MATRIX2_9),
    NAMER(NV_097_SET_TEXTURE_MATRIX2_10),
    NAMER(NV_097_SET_TEXTURE_MATRIX2_11),
    NAMER(NV_097_SET_TEXTURE_MATRIX2_12),
    NAMER(NV_097_SET_TEXTURE_MATRIX2_13),
    NAMER(NV_097_SET_TEXTURE_MATRIX2_14),
    NAMER(NV_097_SET_TEXTURE_MATRIX2_15),
    NAMER(NV_097_SET_TEXTURE_MATRIX3_0),
    NAMER(NV_097_SET_TEXTURE_MATRIX3_1),
    NAMER(NV_097_SET_TEXTURE_MATRIX3_2),
    NAMER(NV_097_SET_TEXTURE_MATRIX3_3),
    NAMER(NV_097_SET_TEXTURE_MATRIX3_4),
    NAMER(NV_097_SET_TEXTURE_MATRIX3_5),
    NAMER(NV_097_SET_TEXTURE_MATRIX3_6),
    NAMER(NV_097_SET_TEXTURE_MATRIX3_7),
    NAMER(NV_097_SET_TEXTURE_MATRIX3_8),
    NAMER(NV_097_SET_TEXTURE_MATRIX3_9),
    NAMER(NV_097_SET_TEXTURE_MATRIX3_10),
    NAMER(NV_097_SET_TEXTURE_MATRIX3_11),
    NAMER(NV_097_SET_TEXTURE_MATRIX3_12),
    NAMER(NV_097_SET_TEXTURE_MATRIX3_13),
    NAMER(NV_097_SET_TEXTURE_MATRIX3_14),
    NAMER(NV_097_SET_TEXTURE_MATRIX3_15),
    NAMER(NV_097_SET_TEXGEN_PLANE_S_0_0),
    NAMER(NV_097_SET_TEXGEN_PLANE_S_0_1),
    NAMER(NV_097_SET_TEXGEN_PLANE_S_0_2),
    NAMER(NV_097_SET_TEXGEN_PLANE_S_0_3),
    NAMER(NV_097_SET_TEXGEN_PLANE_S_1_0),
    NAMER(NV_097_SET_TEXGEN_PLANE_S_1_1),
    NAMER(NV_097_SET_TEXGEN_PLANE_S_1_2),
    NAMER(NV_097_SET_TEXGEN_PLANE_S_1_3),
    NAMER(NV_097_SET_TEXGEN_PLANE_S_2_0),
    NAMER(NV_097_SET_TEXGEN_PLANE_S_2_1),
    NAMER(NV_097_SET_TEXGEN_PLANE_S_2_2),
    NAMER(NV_097_SET_TEXGEN_PLANE_S_2_3),
    NAMER(NV_097_SET_TEXGEN_PLANE_S_3_0),
    NAMER(NV_097_SET_TEXGEN_PLANE_S_3_1),
    NAMER(NV_097_SET_TEXGEN_PLANE_S_3_2),
    NAMER(NV_097_SET_TEXGEN_PLANE_S_3_3),
    NAMER(NV_097_SET_TEXGEN_PLANE_T_0_0),
    NAMER(NV_097_SET_TEXGEN_PLANE_T_0_1),
    NAMER(NV_097_SET_TEXGEN_PLANE_T_0_2),
    NAMER(NV_097_SET_TEXGEN_PLANE_T_0_3),
    NAMER(NV_097_SET_TEXGEN_PLANE_T_1_0),
    NAMER(NV_097_SET_TEXGEN_PLANE_T_1_1),
    NAMER(NV_097_SET_TEXGEN_PLANE_T_1_2),
    NAMER(NV_097_SET_TEXGEN_PLANE_T_1_3),
    NAMER(NV_097_SET_TEXGEN_PLANE_T_2_0),
    NAMER(NV_097_SET_TEXGEN_PLANE_T_2_1),
    NAMER(NV_097_SET_TEXGEN_PLANE_T_2_2),
    NAMER(NV_097_SET_TEXGEN_PLANE_T_2_3),
    NAMER(NV_097_SET_TEXGEN_PLANE_T_3_0),
    NAMER(NV_097_SET_TEXGEN_PLANE_T_3_1),
    NAMER(NV_097_SET_TEXGEN_PLANE_T_3_2),
    NAMER(NV_097_SET_TEXGEN_PLANE_T_3_3),
    NAMER(NV_097_SET_TEXGEN_PLANE_R_0_0),
    NAMER(NV_097_SET_TEXGEN_PLANE_R_0_1),
    NAMER(NV_097_SET_TEXGEN_PLANE_R_0_2),
    NAMER(NV_097_SET_TEXGEN_PLANE_R_0_3),
    NAMER(NV_097_SET_TEXGEN_PLANE_R_1_0),
    NAMER(NV_097_SET_TEXGEN_PLANE_R_1_1),
    NAMER(NV_097_SET_TEXGEN_PLANE_R_1_2),
    NAMER(NV_097_SET_TEXGEN_PLANE_R_1_3),
    NAMER(NV_097_SET_TEXGEN_PLANE_R_2_0),
    NAMER(NV_097_SET_TEXGEN_PLANE_R_2_1),
    NAMER(NV_097_SET_TEXGEN_PLANE_R_2_2),
    NAMER(NV_097_SET_TEXGEN_PLANE_R_2_3),
    NAMER(NV_097_SET_TEXGEN_PLANE_R_3_0),
    NAMER(NV_097_SET_TEXGEN_PLANE_R_3_1),
    NAMER(NV_097_SET_TEXGEN_PLANE_R_3_2),
    NAMER(NV_097_SET_TEXGEN_PLANE_R_3_3),
    NAMER(NV_097_SET_TEXGEN_PLANE_Q_0_0),
    NAMER(NV_097_SET_TEXGEN_PLANE_Q_0_1),
    NAMER(NV_097_SET_TEXGEN_PLANE_Q_0_2),
    NAMER(NV_097_SET_TEXGEN_PLANE_Q_0_3),
    NAMER(NV_097_SET_TEXGEN_PLANE_Q_1_0),
    NAMER(NV_097_SET_TEXGEN_PLANE_Q_1_1),
    NAMER(NV_097_SET_TEXGEN_PLANE_Q_1_2),
    NAMER(NV_097_SET_TEXGEN_PLANE_Q_1_3),
    NAMER(NV_097_SET_TEXGEN_PLANE_Q_2_0),
    NAMER(NV_097_SET_TEXGEN_PLANE_Q_2_1),
    NAMER(NV_097_SET_TEXGEN_PLANE_Q_2_2),
    NAMER(NV_097_SET_TEXGEN_PLANE_Q_2_3),
    NAMER(NV_097_SET_TEXGEN_PLANE_Q_3_0),
    NAMER(NV_097_SET_TEXGEN_PLANE_Q_3_1),
    NAMER(NV_097_SET_TEXGEN_PLANE_Q_3_2),
    NAMER(NV_097_SET_TEXGEN_PLANE_Q_3_3),
    NAMER(NV_097_SET_FOG_PARAMS_0),
    NAMER(NV_097_SET_FOG_PARAMS_1),
    NAMER(NV_097_SET_FOG_PARAMS_2),
    NAMER(NV_097_SET_FOG_PLANE_0),
    NAMER(NV_097_SET_FOG_PLANE_1),
    NAMER(NV_097_SET_FOG_PLANE_2),
    NAMER(NV_097_SET_FOG_PLANE_3),
    NAMER(NV_097_SET_SPECULAR_PARAMS_0),
    NAMER(NV_097_SET_SPECULAR_PARAMS_1),
    NAMER(NV_097_SET_SPECULAR_PARAMS_2),
    NAMER(NV_097_SET_SPECULAR_PARAMS_3),
    NAMER(NV_097_SET_SPECULAR_PARAMS_4),
    NAMER(NV_097_SET_SPECULAR_PARAMS_5),
    NAMER(NV_097_SET_BACK_SPECULAR_PARAMS_0),
    NAMER(NV_097_SET_BACK_SPECULAR_PARAMS_1),
    NAMER(NV_097_SET_BACK_SPECULAR_PARAMS_2),
    NAMER(NV_097_SET_BACK_SPECULAR_PARAMS_3),
    NAMER(NV_097_SET_BACK_SPECULAR_PARAMS_4),
    NAMER(NV_097_SET_BACK_SPECULAR_PARAMS_5),
    NAMER(NV_097_SET_SCENE_AMBIENT_COLOR_0),
    NAMER(NV_097_SET_SCENE_AMBIENT_COLOR_1),
    NAMER(NV_097_SET_SCENE_AMBIENT_COLOR_2),
    NAMER(NV_097_SET_VIEWPORT_SCALE_0),
    NAMER(NV_097_SET_VIEWPORT_SCALE_1),
    NAMER(NV_097_SET_VIEWPORT_SCALE_2),
    NAMER(NV_097_SET_VIEWPORT_SCALE_3),
    NAMER(NV_097_SET_VIEWPORT_OFFSET_0),
    NAMER(NV_097_SET_VIEWPORT_OFFSET_1),
    NAMER(NV_097_SET_VIEWPORT_OFFSET_2),
    NAMER(NV_097_SET_VIEWPORT_OFFSET_3),
    NAMER(NV_097_SET_POINT_PARAMS_0),
    NAMER(NV_097_SET_POINT_PARAMS_1),
    NAMER(NV_097_SET_POINT_PARAMS_2),
    NAMER(NV_097_SET_POINT_PARAMS_3),
    NAMER(NV_097_SET_POINT_PARAMS_4),
    NAMER(NV_097_SET_POINT_PARAMS_5),
    NAMER(NV_097_SET_POINT_PARAMS_6),
    NAMER(NV_097_SET_POINT_PARAMS_7),
    NAMER(NV_097_SET_EYE_POSITION_0),
    NAMER(NV_097_SET_EYE_POSITION_1),
    NAMER(NV_097_SET_EYE_POSITION_2),
    NAMER(NV_097_SET_EYE_POSITION_3),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_0_0),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_0_1),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_0_2),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_1_0),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_1_1),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_1_2),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_2_0),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_2_1),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_2_2),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_3_0),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_3_1),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_3_2),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_4_0),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_4_1),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_4_2),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_5_0),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_5_1),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_5_2),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_6_0),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_6_1),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_6_2),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_7_0),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_7_1),
    NAMER(NV_097_SET_BACK_LIGHT_AMBIENT_COLOR_7_2),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_0_0),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_0_1),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_0_2),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_1_0),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_1_1),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_1_2),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_2_0),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_2_1),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_2_2),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_3_0),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_3_1),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_3_2),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_4_0),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_4_1),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_4_2),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_5_0),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_5_1),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_5_2),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_6_0),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_6_1),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_6_2),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_7_0),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_7_1),
    NAMER(NV_097_SET_BACK_LIGHT_DIFFUSE_COLOR_7_2),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_0_0),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_0_1),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_0_2),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_1_0),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_1_1),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_1_2),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_2_0),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_2_1),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_2_2),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_3_0),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_3_1),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_3_2),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_4_0),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_4_1),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_4_2),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_5_0),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_5_1),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_5_2),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_6_0),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_6_1),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_6_2),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_7_0),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_7_1),
    NAMER(NV_097_SET_BACK_LIGHT_SPECULAR_COLOR_7_2),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_0_0),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_0_1),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_0_2),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_1_0),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_1_1),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_1_2),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_2_0),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_2_1),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_2_2),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_3_0),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_3_1),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_3_2),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_4_0),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_4_1),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_4_2),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_5_0),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_5_1),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_5_2),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_6_0),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_6_1),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_6_2),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_7_0),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_7_1),
    NAMER(NV_097_SET_LIGHT_AMBIENT_COLOR_7_2),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_0_0),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_0_1),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_0_2),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_1_0),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_1_1),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_1_2),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_2_0),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_2_1),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_2_2),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_3_0),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_3_1),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_3_2),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_4_0),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_4_1),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_4_2),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_5_0),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_5_1),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_5_2),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_6_0),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_6_1),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_6_2),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_7_0),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_7_1),
    NAMER(NV_097_SET_LIGHT_DIFFUSE_COLOR_7_2),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_0_0),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_0_1),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_0_2),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_1_0),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_1_1),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_1_2),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_2_0),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_2_1),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_2_2),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_3_0),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_3_1),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_3_2),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_4_0),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_4_1),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_4_2),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_5_0),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_5_1),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_5_2),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_6_0),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_6_1),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_6_2),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_7_0),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_7_1),
    NAMER(NV_097_SET_LIGHT_SPECULAR_COLOR_7_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_RANGE_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_RANGE_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_RANGE_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_RANGE_3),
    NAMER(NV_097_SET_LIGHT_LOCAL_RANGE_4),
    NAMER(NV_097_SET_LIGHT_LOCAL_RANGE_5),
    NAMER(NV_097_SET_LIGHT_LOCAL_RANGE_6),
    NAMER(NV_097_SET_LIGHT_LOCAL_RANGE_7),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_0_0),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_0_1),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_0_2),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_1_0),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_1_1),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_1_2),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_2_0),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_2_1),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_2_2),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_3_0),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_3_1),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_3_2),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_4_0),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_4_1),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_4_2),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_5_0),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_5_1),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_5_2),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_6_0),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_6_1),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_6_2),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_7_0),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_7_1),
    NAMER(NV_097_SET_LIGHT_INFINITE_HALF_VECTOR_7_2),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_0_0),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_0_1),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_0_2),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_1_0),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_1_1),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_1_2),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_2_0),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_2_1),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_2_2),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_3_0),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_3_1),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_3_2),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_4_0),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_4_1),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_4_2),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_5_0),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_5_1),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_5_2),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_6_0),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_6_1),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_6_2),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_7_0),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_7_1),
    NAMER(NV_097_SET_LIGHT_INFINITE_DIRECTION_7_2),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_0_0),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_0_1),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_0_2),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_1_0),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_1_1),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_1_2),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_2_0),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_2_1),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_2_2),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_3_0),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_3_1),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_3_2),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_4_0),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_4_1),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_4_2),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_5_0),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_5_1),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_5_2),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_6_0),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_6_1),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_6_2),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_7_0),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_7_1),
    NAMER(NV_097_SET_LIGHT_SPOT_FALLOFF_7_2),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_0_0),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_0_1),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_0_2),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_0_3),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_1_0),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_1_1),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_1_2),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_1_3),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_2_0),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_2_1),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_2_2),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_2_3),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_3_0),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_3_1),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_3_2),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_3_3),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_4_0),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_4_1),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_4_2),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_4_3),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_5_0),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_5_1),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_5_2),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_5_3),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_6_0),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_6_1),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_6_2),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_6_3),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_7_0),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_7_1),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_7_2),
    NAMER(NV_097_SET_LIGHT_SPOT_DIRECTION_7_3),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_0_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_0_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_0_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_1_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_1_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_1_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_2_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_2_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_2_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_3_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_3_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_3_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_4_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_4_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_4_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_5_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_5_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_5_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_6_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_6_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_6_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_7_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_7_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_POSITION_7_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_0_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_0_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_0_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_1_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_1_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_1_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_2_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_2_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_2_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_3_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_3_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_3_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_4_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_4_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_4_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_5_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_5_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_5_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_6_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_6_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_6_2),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_7_0),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_7_1),
    NAMER(NV_097_SET_LIGHT_LOCAL_ATTENUATION_7_2),
    NAMER(NV_097_SET_VERTEX3F_0),
    NAMER(NV_097_SET_VERTEX3F_1),
    NAMER(NV_097_SET_VERTEX3F_2),
    NAMER(NV_097_SET_VERTEX4F_0),
    NAMER(NV_097_SET_VERTEX4F_1),
    NAMER(NV_097_SET_VERTEX4F_2),
    NAMER(NV_097_SET_VERTEX4F_3),
    NAMER(NV_097_SET_VERTEX4S_0),
    NAMER(NV_097_SET_VERTEX4S_1),
    NAMER(NV_097_SET_NORMAL3F_0),
    NAMER(NV_097_SET_NORMAL3F_1),
    NAMER(NV_097_SET_NORMAL3F_2),
    NAMER(NV_097_SET_NORMAL3S_0),
    NAMER(NV_097_SET_NORMAL3S_1),
    NAMER(NV_097_SET_DIFFUSE_COLOR4F_0),
    NAMER(NV_097_SET_DIFFUSE_COLOR4F_1),
    NAMER(NV_097_SET_DIFFUSE_COLOR4F_2),
    NAMER(NV_097_SET_DIFFUSE_COLOR4F_3),
    NAMER(NV_097_SET_DIFFUSE_COLOR3F_0),
    NAMER(NV_097_SET_DIFFUSE_COLOR3F_1),
    NAMER(NV_097_SET_DIFFUSE_COLOR3F_2),
    NAMER(NV_097_SET_DIFFUSE_COLOR4UB),
    NAMER(NV_097_SET_SPECULAR_COLOR4F_0),
    NAMER(NV_097_SET_SPECULAR_COLOR4F_1),
    NAMER(NV_097_SET_SPECULAR_COLOR4F_2),
    NAMER(NV_097_SET_SPECULAR_COLOR4F_3),
    NAMER(NV_097_SET_SPECULAR_COLOR3F_0),
    NAMER(NV_097_SET_SPECULAR_COLOR3F_1),
    NAMER(NV_097_SET_SPECULAR_COLOR3F_2),
    NAMER(NV_097_SET_SPECULAR_COLOR4UB),
    NAMER(NV_097_SET_TEXCOORD0_2F_0),
    NAMER(NV_097_SET_TEXCOORD0_2F_1),
    NAMER(NV_097_SET_TEXCOORD0_2S),
    NAMER(NV_097_SET_TEXCOORD0_4F_0),
    NAMER(NV_097_SET_TEXCOORD0_4F_1),
    NAMER(NV_097_SET_TEXCOORD0_4F_2),
    NAMER(NV_097_SET_TEXCOORD0_4F_3),
    NAMER(NV_097_SET_TEXCOORD0_4S_0),
    NAMER(NV_097_SET_TEXCOORD0_4S_1),
    NAMER(NV_097_SET_TEXCOORD1_2F_0),
    NAMER(NV_097_SET_TEXCOORD1_2F_1),
    NAMER(NV_097_SET_TEXCOORD1_2S),
    NAMER(NV_097_SET_TEXCOORD1_4F_0),
    NAMER(NV_097_SET_TEXCOORD1_4F_1),
    NAMER(NV_097_SET_TEXCOORD1_4F_2),
    NAMER(NV_097_SET_TEXCOORD1_4F_3),
    NAMER(NV_097_SET_TEXCOORD1_4S_0),
    NAMER(NV_097_SET_TEXCOORD1_4S_1),
    NAMER(NV_097_SET_TEXCOORD2_2F_0),
    NAMER(NV_097_SET_TEXCOORD2_2F_1),
    NAMER(NV_097_SET_TEXCOORD2_2S),
    NAMER(NV_097_SET_TEXCOORD2_4F_0),
    NAMER(NV_097_SET_TEXCOORD2_4F_1),
    NAMER(NV_097_SET_TEXCOORD2_4F_2),
    NAMER(NV_097_SET_TEXCOORD2_4F_3),
    NAMER(NV_097_SET_TEXCOORD2_4S_0),
    NAMER(NV_097_SET_TEXCOORD2_4S_1),
    NAMER(NV_097_SET_TEXCOORD3_2F_0),
    NAMER(NV_097_SET_TEXCOORD3_2F_1),
    NAMER(NV_097_SET_TEXCOORD3_2S),
    NAMER(NV_097_SET_TEXCOORD3_4F_0),
    NAMER(NV_097_SET_TEXCOORD3_4F_1),
    NAMER(NV_097_SET_TEXCOORD3_4F_2),
    NAMER(NV_097_SET_TEXCOORD3_4F_3),
    NAMER(NV_097_SET_TEXCOORD3_4S_0),
    NAMER(NV_097_SET_TEXCOORD3_4S_1),
    NAMER(NV_097_SET_FOG1F),
    NAMER(NV_097_SET_WEIGHT1F),
    NAMER(NV_097_SET_WEIGHT2F_0),
    NAMER(NV_097_SET_WEIGHT2F_1),
    NAMER(NV_097_SET_WEIGHT3F_0),
    NAMER(NV_097_SET_WEIGHT3F_1),
    NAMER(NV_097_SET_WEIGHT3F_2),
    NAMER(NV_097_SET_WEIGHT4F_0),
    NAMER(NV_097_SET_WEIGHT4F_1),
    NAMER(NV_097_SET_WEIGHT4F_2),
    NAMER(NV_097_SET_WEIGHT4F_3),
    NAMER(NV_097_SET_EDGE_FLAG),
#if 0
NV_097_SET_EDGE_FLAG_V_FALSE                                    0
NV_097_SET_EDGE_FLAG_V_TRUE                                     1
#endif
    NAMER(NV_097_SET_TRANSFORM_FIXED_CONST0_0),
    NAMER(NV_097_SET_TRANSFORM_FIXED_CONST0_1),
    NAMER(NV_097_SET_TRANSFORM_FIXED_CONST0_2),
    NAMER(NV_097_SET_TRANSFORM_FIXED_CONST0_3),
    NAMER(NV_097_SET_TRANSFORM_FIXED_CONST1_0),
    NAMER(NV_097_SET_TRANSFORM_FIXED_CONST1_1),
    NAMER(NV_097_SET_TRANSFORM_FIXED_CONST1_2),
    NAMER(NV_097_SET_TRANSFORM_FIXED_CONST1_3),
    NAMER(NV_097_SET_TRANSFORM_FIXED_CONST2_0),
    NAMER(NV_097_SET_TRANSFORM_FIXED_CONST2_1),
    NAMER(NV_097_SET_TRANSFORM_FIXED_CONST2_2),
    NAMER(NV_097_SET_TRANSFORM_FIXED_CONST2_3),
    NAMER(NV_097_SET_TRANSFORM_FIXED_CONST3_0),
    NAMER(NV_097_SET_TRANSFORM_FIXED_CONST3_1),
    NAMER(NV_097_SET_TRANSFORM_FIXED_CONST3_2),
    NAMER(NV_097_SET_TRANSFORM_FIXED_CONST3_3),
    NAMER(NV_097_SET_TLCONST_ZERO_0),
    NAMER(NV_097_SET_TLCONST_ZERO_1),
    NAMER(NV_097_SET_TLCONST_ZERO_2),
    NAMER(NV_097_SET_EYE_DIRECTION_0),
    NAMER(NV_097_SET_EYE_DIRECTION_1),
    NAMER(NV_097_SET_EYE_DIRECTION_2),
    NAMER(NV_097_SET_LINEAR_FOG_CONST_0),
    NAMER(NV_097_SET_LINEAR_FOG_CONST_1),
    NAMER(NV_097_SET_LINEAR_FOG_CONST_2),
    NAMER(NV_097_INVALIDATE_VERTEX_CACHE_FILE),
    NAMER(NV_097_INVALIDATE_VERTEX_FILE),
    NAMER(NV_097_TL_NOP),
    NAMER(NV_097_TL_SYNC),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_0),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_1),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_2),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_3),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_4),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_5),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_6),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_7),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_8),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_9),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_10),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_11),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_12),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_13),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_14),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_15),
#if 0
NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_CONTEXT_DMA                 0x80000000
NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_CONTEXT_DMA__BITSHIFT       31
NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_CONTEXT_DMA_VERTEX_A        0
NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_CONTEXT_DMA_VERTEX_B        1
NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_OFFSET                      0x7fffffff
NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_OFFSET__BITSHIFT            0
#endif
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_0),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_1),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_2),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_3),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_4),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_5),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_6),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_7),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_8),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_9),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_10),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_11),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_12),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_13),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_14),
    NAMER(NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_15),
#if 0
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE                      0xffffff00
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE__BITSHIFT            8
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE                        0x000000f0
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE__BITSHIFT              4
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED               0
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_1                      1
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_2                      2
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_3                      3
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_4                      4
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_3W                     7
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE                        0x0000000f
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE__BITSHIFT              0
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_UB_D3D                 0
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S1                     1
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F                      2
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_UB_OGL                 4
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K                   5
NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_CMP                    6
#endif
    NAMER(NV_097_SET_LOGIC_OP_ENABLE),
#if 0
NV_097_SET_LOGIC_OP_ENABLE_V_FALSE                              0
NV_097_SET_LOGIC_OP_ENABLE_V_TRUE                               1
#endif
    NAMER(NV_097_SET_LOGIC_OP),
#if 0
NV_097_SET_LOGIC_OP_V_CLEAR                                     0x00001500
NV_097_SET_LOGIC_OP_V_AND                                       0x00001501
NV_097_SET_LOGIC_OP_V_AND_REVERSE                               0x00001502
NV_097_SET_LOGIC_OP_V_COPY                                      0x00001503
NV_097_SET_LOGIC_OP_V_AND_INVERTED                              0x00001504
NV_097_SET_LOGIC_OP_V_NOOP                                      0x00001505
NV_097_SET_LOGIC_OP_V_XOR                                       0x00001506
NV_097_SET_LOGIC_OP_V_OR                                        0x00001507
NV_097_SET_LOGIC_OP_V_NOR                                       0x00001508
NV_097_SET_LOGIC_OP_V_EQUIV                                     0x00001509
NV_097_SET_LOGIC_OP_V_INVERT                                    0x0000150a
NV_097_SET_LOGIC_OP_V_OR_REVERSE                                0x0000150b
NV_097_SET_LOGIC_OP_V_COPY_INVERTED                             0x0000150c
NV_097_SET_LOGIC_OP_V_OR_INVERTED                               0x0000150d
NV_097_SET_LOGIC_OP_V_NAND                                      0x0000150e
NV_097_SET_LOGIC_OP_V_SET                                       0x0000150f
#endif
    NAMER(NV_097_SET_BEGIN_END),
#if 0
NV_097_SET_BEGIN_END_OP_END                                     0
NV_097_SET_BEGIN_END_OP_POINTS                                  1
NV_097_SET_BEGIN_END_OP_LINES                                   2
NV_097_SET_BEGIN_END_OP_LINE_LOOP                               3
NV_097_SET_BEGIN_END_OP_LINE_STRIP                              4
NV_097_SET_BEGIN_END_OP_TRIANGLES                               5
NV_097_SET_BEGIN_END_OP_TRIANGLE_STRIP                          6
NV_097_SET_BEGIN_END_OP_TRIANGLE_FAN                            7
NV_097_SET_BEGIN_END_OP_QUADS                                   8
NV_097_SET_BEGIN_END_OP_QUAD_STRIP                              9
NV_097_SET_BEGIN_END_OP_POLYGON                                 10
#endif
    NAMER(NV_097_ARRAY_ELEMENT16),
#if 0
NV_097_ARRAY_ELEMENT16_VERTEX0                                  0x0000ffff
NV_097_ARRAY_ELEMENT16_VERTEX0__BITSHIFT                        0
NV_097_ARRAY_ELEMENT16_VERTEX1                                  0xffff0000
NV_097_ARRAY_ELEMENT16_VERTEX1__BITSHIFT                        16
#endif
    NAMER(NV_097_ARRAY_ELEMENT32),
    NAMER(NV_097_DRAW_ARRAYS),
#if 0
NV_097_DRAW_ARRAYS_COUNT                                        0xff000000
NV_097_DRAW_ARRAYS_COUNT__BITSHIFT                              24
NV_097_DRAW_ARRAYS_START_INDEX                                  0x00ffffff
NV_097_DRAW_ARRAYS_START_INDEX__BITSHIFT                        0
#endif
    NAMER(NV_097_INLINE_VERTEX_REUSE),
    NAMER(NV_097_INLINE_ARRAY),
    NAMER(NV_097_SET_TEXTURE_OFFSET_0),
    NAMER(NV_097_SET_TEXTURE_OFFSET_1),
    NAMER(NV_097_SET_TEXTURE_OFFSET_2),
    NAMER(NV_097_SET_TEXTURE_OFFSET_3),
    NAMER(NV_097_SET_TEXTURE_FORMAT_0),
    NAMER(NV_097_SET_TEXTURE_FORMAT_1),
    NAMER(NV_097_SET_TEXTURE_FORMAT_2),
    NAMER(NV_097_SET_TEXTURE_FORMAT_3),
#if 0
NV_097_SET_TEXTURE_FORMAT_CONTEXT_DMA                           0x00000003
NV_097_SET_TEXTURE_FORMAT_CONTEXT_DMA__BITSHIFT                 0
NV_097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A                         1
NV_097_SET_TEXTURE_FORMAT_CONTEXT_DMA_B                         2
NV_097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE                        0x00000004
NV_097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE__BITSHIFT              2
NV_097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE                  0
NV_097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_TRUE                   1
NV_097_SET_TEXTURE_FORMAT_BORDER_SOURCE                         0x00000008
NV_097_SET_TEXTURE_FORMAT_BORDER_SOURCE__BITSHIFT               3
NV_097_SET_TEXTURE_FORMAT_BORDER_SOURCE_TEXTURE                 0
NV_097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR                   1
NV_097_SET_TEXTURE_FORMAT_DIMENSIONALITY                        0x000000f0
NV_097_SET_TEXTURE_FORMAT_DIMENSIONALITY__BITSHIFT              4
NV_097_SET_TEXTURE_FORMAT_DIMENSIONALITY_ONE                    1
NV_097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO                    2
NV_097_SET_TEXTURE_FORMAT_DIMENSIONALITY_THREE                  3
NV_097_SET_TEXTURE_FORMAT_COLOR                                 0x0000ff00
NV_097_SET_TEXTURE_FORMAT_COLOR__BITSHIFT                       8
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_Y8                           0
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_AY8                          1
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_A1R5G5B5                     2
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_X1R5G5B5                     3
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_A4R4G4B4                     4
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_R5G6B5                       5
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_A8R8G8B8                     6
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_X8R8G8B8                     7
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_I8_A8R8G8B8                  11
NV_097_SET_TEXTURE_FORMAT_COLOR_L_DXT1_A1R5G5B5                 12
NV_097_SET_TEXTURE_FORMAT_COLOR_L_DXT23_A8R8G8B8                14
NV_097_SET_TEXTURE_FORMAT_COLOR_L_DXT45_A8R8G8B8                15
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A1R5G5B5               16
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5                 17
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8               18
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8                     19
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8                    20
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X7SY9                  21
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8                   22
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8                   23
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SG8SB8                 24
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_A8                           25
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_A8Y8                         26
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_AY8                    27
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5               28
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4               29
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X8R8G8B8               30
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8                     31
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8Y8                   32
NV_097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_CR8YB8CB8YA8           36
NV_097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_YB8CR8YA8CB8           37
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8CR8CB8Y8             38
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_R6G5B5                       39
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_G8B8                         40
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_R8B8                         41
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_DEPTH_X8_Y24_FIXED           42
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_DEPTH_X8_Y24_FLOAT           43
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_DEPTH_Y16_FIXED              44
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_DEPTH_Y16_FLOAT              45
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FIXED     46
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FLOAT     47
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FIXED        48
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FLOAT        49
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_Y16                          50
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_YB_16_YA_16                  51
NV_097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_A4V6YB6A4U6YA6         52
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y16                    53
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_YB16YA16               54
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R6G5B5                 55
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_R5G5B5A1                     56
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_R4G4B4A4                     57
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_A8B8G8R8                     58
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_B8G8R8A8                     59
NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_R8G8B8A8                     60
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G5B5A1               61
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R4G4B4A4               62
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8B8G8R8               63
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_B8G8R8A8               64
NV_097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8G8B8A8               65
NV_097_SET_TEXTURE_FORMAT_MIPMAP_LEVELS                         0x000f0000
NV_097_SET_TEXTURE_FORMAT_MIPMAP_LEVELS__BITSHIFT               16
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_U                           0x00f00000
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_U__BITSHIFT                 20
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_U_1                         0
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_U_2                         1
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_U_4                         2
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_U_8                         3
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_U_16                        4
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_U_32                        5
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_U_64                        6
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_U_128                       7
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_U_256                       8
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_U_512                       9
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_U_1024                      10
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_U_2048                      11
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_U_4096                      12
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_V                           0x0f000000
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_V__BITSHIFT                 24
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_V_1                         0
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_V_2                         1
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_V_4                         2
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_V_8                         3
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_V_16                        4
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_V_32                        5
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_V_64                        6
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_V_128                       7
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_V_256                       8
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_V_512                       9
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_V_1024                      10
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_V_2048                      11
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_V_4096                      12
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_P                           0xf0000000
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_P__BITSHIFT                 28
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_P_1                         0
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_P_2                         1
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_P_4                         2
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_P_8                         3
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_P_16                        4
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_P_32                        5
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_P_64                        6
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_P_128                       7
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_P_256                       8
NV_097_SET_TEXTURE_FORMAT_BASE_SIZE_P_512                       9
#endif
    NAMER(NV_097_SET_TEXTURE_ADDRESS_0),
    NAMER(NV_097_SET_TEXTURE_ADDRESS_1),
    NAMER(NV_097_SET_TEXTURE_ADDRESS_2),
    NAMER(NV_097_SET_TEXTURE_ADDRESS_3),
#if 0
NV_097_SET_TEXTURE_ADDRESS_U                                    0x0000000f
NV_097_SET_TEXTURE_ADDRESS_U__BITSHIFT                          0
NV_097_SET_TEXTURE_ADDRESS_U_WRAP                               1
NV_097_SET_TEXTURE_ADDRESS_U_MIRROR                             2
NV_097_SET_TEXTURE_ADDRESS_U_CLAMP_TO_EDGE                      3
NV_097_SET_TEXTURE_ADDRESS_U_BORDER                             4
NV_097_SET_TEXTURE_ADDRESS_U_CLAMP_OGL                          5
NV_097_SET_TEXTURE_ADDRESS_CYLWRAP_U                            0x000000f0
NV_097_SET_TEXTURE_ADDRESS_CYLWRAP_U__BITSHIFT                  4
NV_097_SET_TEXTURE_ADDRESS_CYLWRAP_U_FALSE                      0
NV_097_SET_TEXTURE_ADDRESS_CYLWRAP_U_TRUE                       1
NV_097_SET_TEXTURE_ADDRESS_V                                    0x00000f00
NV_097_SET_TEXTURE_ADDRESS_V__BITSHIFT                          8
NV_097_SET_TEXTURE_ADDRESS_V_WRAP                               1
NV_097_SET_TEXTURE_ADDRESS_V_MIRROR                             2
NV_097_SET_TEXTURE_ADDRESS_V_CLAMP_TO_EDGE                      3
NV_097_SET_TEXTURE_ADDRESS_V_BORDER                             4
NV_097_SET_TEXTURE_ADDRESS_V_CLAMP_OGL                          5
NV_097_SET_TEXTURE_ADDRESS_CYLWRAP_V                            0x0000f000
NV_097_SET_TEXTURE_ADDRESS_CYLWRAP_V__BITSHIFT                  12
NV_097_SET_TEXTURE_ADDRESS_CYLWRAP_V_FALSE                      0
NV_097_SET_TEXTURE_ADDRESS_CYLWRAP_V_TRUE                       1
NV_097_SET_TEXTURE_ADDRESS_P                                    0x000f0000
NV_097_SET_TEXTURE_ADDRESS_P__BITSHIFT                          16
NV_097_SET_TEXTURE_ADDRESS_P_WRAP                               1
NV_097_SET_TEXTURE_ADDRESS_P_MIRROR                             2
NV_097_SET_TEXTURE_ADDRESS_P_CLAMP_TO_EDGE                      3
NV_097_SET_TEXTURE_ADDRESS_P_BORDER                             4
NV_097_SET_TEXTURE_ADDRESS_P_CLAMP_OGL                          5
NV_097_SET_TEXTURE_ADDRESS_CYLWRAP_P                            0x00f00000
NV_097_SET_TEXTURE_ADDRESS_CYLWRAP_P__BITSHIFT                  20
NV_097_SET_TEXTURE_ADDRESS_CYLWRAP_P_FALSE                      0
NV_097_SET_TEXTURE_ADDRESS_CYLWRAP_P_TRUE                       1
NV_097_SET_TEXTURE_ADDRESS_CYLWRAP_Q                            0xff000000
NV_097_SET_TEXTURE_ADDRESS_CYLWRAP_Q__BITSHIFT                  24
NV_097_SET_TEXTURE_ADDRESS_CYLWRAP_Q_FALSE                      0
NV_097_SET_TEXTURE_ADDRESS_CYLWRAP_Q_TRUE                       1
#endif
    NAMER(NV_097_SET_TEXTURE_CONTROL0_0),
    NAMER(NV_097_SET_TEXTURE_CONTROL0_1),
    NAMER(NV_097_SET_TEXTURE_CONTROL0_2),
    NAMER(NV_097_SET_TEXTURE_CONTROL0_3),
#if 0
NV_097_SET_TEXTURE_CONTROL0_ENABLE                              0xc0000000
NV_097_SET_TEXTURE_CONTROL0_ENABLE__BITSHIFT                    30
NV_097_SET_TEXTURE_CONTROL0_ENABLE_FALSE                        0
NV_097_SET_TEXTURE_CONTROL0_ENABLE_TRUE                         1
NV_097_SET_TEXTURE_CONTROL0_MIN_LOD_CLAMP                       0x3ffc0000
NV_097_SET_TEXTURE_CONTROL0_MIN_LOD_CLAMP__BITSHIFT             18
NV_097_SET_TEXTURE_CONTROL0_MAX_LOD_CLAMP                       0x0003ffc0
NV_097_SET_TEXTURE_CONTROL0_MAX_LOD_CLAMP__BITSHIFT             6
NV_097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO                       0x00000030
NV_097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO__BITSHIFT             4
NV_097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0                     0
NV_097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_1                     1
NV_097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_2                     2
NV_097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_3                     3
NV_097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE                  0x00000008
NV_097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE__BITSHIFT        3
NV_097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_FALSE            0
NV_097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_TRUE             1
NV_097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE                   0x00000004
NV_097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE__BITSHIFT         2
NV_097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_FALSE             0
NV_097_SET_TEXTURE_CONTROL0_ALPHA_KILL_ENABLE_TRUE              1
NV_097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION                 0x00000003
NV_097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION__BITSHIFT       0
NV_097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_FALSE           0
NV_097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_ALPHA           1
NV_097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_RGBA            2
NV_097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_KILL            3
#endif
    NAMER(NV_097_SET_TEXTURE_CONTROL1_0),
    NAMER(NV_097_SET_TEXTURE_CONTROL1_1),
    NAMER(NV_097_SET_TEXTURE_CONTROL1_2),
    NAMER(NV_097_SET_TEXTURE_CONTROL1_3),
#if 0
NV_097_SET_TEXTURE_CONTROL1_IMAGE_PITCH                         0xffff0000
NV_097_SET_TEXTURE_CONTROL1_IMAGE_PITCH__BITSHIFT               16
#endif
    NAMER(NV_097_SET_TEXTURE_FILTER_0),
    NAMER(NV_097_SET_TEXTURE_FILTER_1),
    NAMER(NV_097_SET_TEXTURE_FILTER_2),
    NAMER(NV_097_SET_TEXTURE_FILTER_3),
#if 0
NV_097_SET_TEXTURE_FILTER_MIPMAP_LOD_BIAS                       0x00001fff
NV_097_SET_TEXTURE_FILTER_MIPMAP_LOD_BIAS__BITSHIFT             0
NV_097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL                    0x0000e000
NV_097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL__BITSHIFT          13
NV_097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_QUINCUNX           1
NV_097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_GAUSSIAN_3         2
NV_097_SET_TEXTURE_FILTER_MIN                                   0x00ff0000
NV_097_SET_TEXTURE_FILTER_MIN__BITSHIFT                         16
NV_097_SET_TEXTURE_FILTER_MIN_BOX_LOD0                          1
NV_097_SET_TEXTURE_FILTER_MIN_TENT_LOD0                         2
NV_097_SET_TEXTURE_FILTER_MIN_BOX_NEARESTLOD                    3
NV_097_SET_TEXTURE_FILTER_MIN_TENT_NEARESTLOD                   4
NV_097_SET_TEXTURE_FILTER_MIN_BOX_TENT_LOD                      5
NV_097_SET_TEXTURE_FILTER_MIN_TENT_TENT_LOD                     6
NV_097_SET_TEXTURE_FILTER_MIN_CONVOLUTION_2D_LOD0               7
NV_097_SET_TEXTURE_FILTER_MAG                                   0x0f000000
NV_097_SET_TEXTURE_FILTER_MAG__BITSHIFT                         24
NV_097_SET_TEXTURE_FILTER_MAG_BOX_LOD0                          1
NV_097_SET_TEXTURE_FILTER_MAG_TENT_LOD0                         2
NV_097_SET_TEXTURE_FILTER_MAG_CONVOLUTION_2D_LOD0               4
NV_097_SET_TEXTURE_FILTER_ASIGNED                               0x10000000
NV_097_SET_TEXTURE_FILTER_ASIGNED__BITSHIFT                     28
NV_097_SET_TEXTURE_FILTER_ASIGNED_BIT_DISABLED                  0
NV_097_SET_TEXTURE_FILTER_ASIGNED_BIT_ENABLED                   1
NV_097_SET_TEXTURE_FILTER_RSIGNED                               0x20000000
NV_097_SET_TEXTURE_FILTER_RSIGNED__BITSHIFT                     29
NV_097_SET_TEXTURE_FILTER_RSIGNED_BIT_DISABLED                  0
NV_097_SET_TEXTURE_FILTER_RSIGNED_BIT_ENABLED                   1
NV_097_SET_TEXTURE_FILTER_GSIGNED                               0x40000000
NV_097_SET_TEXTURE_FILTER_GSIGNED__BITSHIFT                     30
NV_097_SET_TEXTURE_FILTER_GSIGNED_BIT_DISABLED                  0
NV_097_SET_TEXTURE_FILTER_GSIGNED_BIT_ENABLED                   1
NV_097_SET_TEXTURE_FILTER_BSIGNED                               0x80000000
NV_097_SET_TEXTURE_FILTER_BSIGNED__BITSHIFT                     31
NV_097_SET_TEXTURE_FILTER_BSIGNED_BIT_DISABLED                  0
NV_097_SET_TEXTURE_FILTER_BSIGNED_BIT_ENABLED                   1
#endif
    NAMER(NV_097_SET_TEXTURE_IMAGE_RECT_0),
    NAMER(NV_097_SET_TEXTURE_IMAGE_RECT_1),
    NAMER(NV_097_SET_TEXTURE_IMAGE_RECT_2),
    NAMER(NV_097_SET_TEXTURE_IMAGE_RECT_3),
#if 0
NV_097_SET_TEXTURE_IMAGE_RECT_WIDTH                             0xffff0000
NV_097_SET_TEXTURE_IMAGE_RECT_WIDTH__BITSHIFT                   16
NV_097_SET_TEXTURE_IMAGE_RECT_HEIGHT                            0x0000ffff
NV_097_SET_TEXTURE_IMAGE_RECT_HEIGHT__BITSHIFT                  0
#endif
    NAMER(NV_097_SET_TEXTURE_PALETTE_0),
    NAMER(NV_097_SET_TEXTURE_PALETTE_1),
    NAMER(NV_097_SET_TEXTURE_PALETTE_2),
    NAMER(NV_097_SET_TEXTURE_PALETTE_3),
#if 0
NV_097_SET_TEXTURE_PALETTE_CONTEXT_DMA                          0x00000003
NV_097_SET_TEXTURE_PALETTE_CONTEXT_DMA__BITSHIFT                0
NV_097_SET_TEXTURE_PALETTE_CONTEXT_DMA_A                        0
NV_097_SET_TEXTURE_PALETTE_CONTEXT_DMA_B                        1
NV_097_SET_TEXTURE_PALETTE_LENGTH                               0x0000003c
NV_097_SET_TEXTURE_PALETTE_LENGTH__BITSHIFT                     2
NV_097_SET_TEXTURE_PALETTE_LENGTH_256                           0
NV_097_SET_TEXTURE_PALETTE_LENGTH_128                           1
NV_097_SET_TEXTURE_PALETTE_LENGTH_64                            2
NV_097_SET_TEXTURE_PALETTE_LENGTH_32                            3
NV_097_SET_TEXTURE_PALETTE_OFFSET                               0xffffffc0
NV_097_SET_TEXTURE_PALETTE_OFFSET__BITSHIFT                     6
#endif
    NAMER(NV_097_SET_TEXTURE_BORDER_COLOR_0),
    NAMER(NV_097_SET_TEXTURE_BORDER_COLOR_1),
    NAMER(NV_097_SET_TEXTURE_BORDER_COLOR_2),
    NAMER(NV_097_SET_TEXTURE_BORDER_COLOR_3),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_MAT00_0),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_MAT00_1),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_MAT00_2),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_MAT00_3),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_MAT01_0),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_MAT01_1),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_MAT01_2),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_MAT01_3),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_MAT11_0),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_MAT11_1),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_MAT11_2),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_MAT11_3),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_MAT10_0),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_MAT10_1),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_MAT10_2),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_MAT10_3),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_SCALE_0),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_SCALE_1),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_SCALE_2),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_SCALE_3),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_OFFSET_0),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_OFFSET_1),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_OFFSET_2),
    NAMER(NV_097_SET_TEXTURE_SET_BUMP_ENV_OFFSET_3),
    NAMER(NV_097_PARK_ATTRIBUTE),
    NAMER(NV_097_UNPARK_ATTRIBUTE),
    NAMER(NV_097_SET_SEMAPHORE_OFFSET),
    NAMER(NV_097_BACK_END_WRITE_SEMAPHORE_RELEASE),
    NAMER(NV_097_TEXTURE_READ_SEMAPHORE_RELEASE),
    NAMER(NV_097_SET_ZMIN_MAX_CONTROL),
#if 0
NV_097_SET_ZMIN_MAX_CONTROL_CULL_NEAR_FAR_EN                    0x0000000f
NV_097_SET_ZMIN_MAX_CONTROL_CULL_NEAR_FAR_EN__BITSHIFT          0
NV_097_SET_ZMIN_MAX_CONTROL_CULL_NEAR_FAR_EN_FALSE              0
NV_097_SET_ZMIN_MAX_CONTROL_CULL_NEAR_FAR_EN_TRUE               1
NV_097_SET_ZMIN_MAX_CONTROL_ZCLAMP_EN                           0x000000f0
NV_097_SET_ZMIN_MAX_CONTROL_ZCLAMP_EN__BITSHIFT                 4
NV_097_SET_ZMIN_MAX_CONTROL_ZCLAMP_EN_CULL                      0
NV_097_SET_ZMIN_MAX_CONTROL_ZCLAMP_EN_CLAMP                     1
NV_097_SET_ZMIN_MAX_CONTROL_CULL_IGNORE_W                       0x00000f00
NV_097_SET_ZMIN_MAX_CONTROL_CULL_IGNORE_W__BITSHIFT             8
NV_097_SET_ZMIN_MAX_CONTROL_CULL_IGNORE_W_FALSE                 0
NV_097_SET_ZMIN_MAX_CONTROL_CULL_IGNORE_W_TRUE                  1
#endif
    NAMER(NV_097_SET_ANTI_ALIASING_CONTROL),
#if 0
NV_097_SET_ANTI_ALIASING_CONTROL_ENABLE                         0x0000000f
NV_097_SET_ANTI_ALIASING_CONTROL_ENABLE__BITSHIFT               0
NV_097_SET_ANTI_ALIASING_CONTROL_ENABLE_FALSE                   0
NV_097_SET_ANTI_ALIASING_CONTROL_ENABLE_TRUE                    1
NV_097_SET_ANTI_ALIASING_CONTROL_ALPHA_TO_COVERAGE              0x000000f0
NV_097_SET_ANTI_ALIASING_CONTROL_ALPHA_TO_COVERAGE__BITSHIFT    4
NV_097_SET_ANTI_ALIASING_CONTROL_ALPHA_TO_COVERAGE_DISABLE      0
NV_097_SET_ANTI_ALIASING_CONTROL_ALPHA_TO_COVERAGE_ENABLE       1
NV_097_SET_ANTI_ALIASING_CONTROL_ALPHA_TO_ONE                   0x00000f00
NV_097_SET_ANTI_ALIASING_CONTROL_ALPHA_TO_ONE__BITSHIFT         8
NV_097_SET_ANTI_ALIASING_CONTROL_ALPHA_TO_ONE_DISABLE           0
NV_097_SET_ANTI_ALIASING_CONTROL_ALPHA_TO_ONE_ENABLE            1
NV_097_SET_ANTI_ALIASING_CONTROL_SAMPLE_MASK                    0xffff0000
NV_097_SET_ANTI_ALIASING_CONTROL_SAMPLE_MASK__BITSHIFT          16
#endif
    NAMER(NV_097_SET_DXT_DITHER_ENABLE_SW),
#if 0
NV_097_SET_DXT_DITHER_ENABLE_SW_V_DISABLE                       0
NV_097_SET_DXT_DITHER_ENABLE_SW_V_ENABLE                        1
#endif
    NAMER(NV_097_SET_COMPRESS_ZBUFFER_EN),
#if 0
NV_097_SET_COMPRESS_ZBUFFER_EN_V_DISABLE                        0
NV_097_SET_COMPRESS_ZBUFFER_EN_V_ENABLE                         1
#endif
    NAMER(NV_097_SET_OCCLUDE_ZSTENCIL_EN),
#if 0
NV_097_SET_OCCLUDE_ZSTENCIL_EN_OCCLUDE_ZEN                      0x00000001
NV_097_SET_OCCLUDE_ZSTENCIL_EN_OCCLUDE_ZEN__BITSHIFT            0
NV_097_SET_OCCLUDE_ZSTENCIL_EN_OCCLUDE_ZEN_DISABLE              0
NV_097_SET_OCCLUDE_ZSTENCIL_EN_OCCLUDE_ZEN_ENABLE               1
NV_097_SET_OCCLUDE_ZSTENCIL_EN_OCCLUDE_STENCIL_EN               0x00000002
NV_097_SET_OCCLUDE_ZSTENCIL_EN_OCCLUDE_STENCIL_EN__BITSHIFT     1
NV_097_SET_OCCLUDE_ZSTENCIL_EN_OCCLUDE_STENCIL_EN_DISABLE       0
NV_097_SET_OCCLUDE_ZSTENCIL_EN_OCCLUDE_STENCIL_EN_ENABLE        1
#endif
    NAMER(NV_097_SET_SURFACE_FORMAT),
#if 0
NV_097_SET_SURFACE_FORMAT_COLOR                                 0x0000000f
NV_097_SET_SURFACE_FORMAT_COLOR__BITSHIFT                       0
NV_097_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5            1
NV_097_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_O1R5G5B5            2
NV_097_SET_SURFACE_FORMAT_COLOR_LE_R5G6B5                       3
NV_097_SET_SURFACE_FORMAT_COLOR_LE_X8R8G8B8_Z8R8G8B8            4
NV_097_SET_SURFACE_FORMAT_COLOR_LE_X8R8G8B8_O8R8G8B8            5
NV_097_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_Z1A7R8G8B8        6
NV_097_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_O1A7R8G8B8        7
NV_097_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8                     8
NV_097_SET_SURFACE_FORMAT_COLOR_LE_B8                           9
NV_097_SET_SURFACE_FORMAT_COLOR_LE_G8B8                         10
NV_097_SET_SURFACE_FORMAT_ZETA                                  0x000000f0
NV_097_SET_SURFACE_FORMAT_ZETA__BITSHIFT                        4
NV_097_SET_SURFACE_FORMAT_ZETA_Z16                              1
NV_097_SET_SURFACE_FORMAT_ZETA_Z24S8                            2
NV_097_SET_SURFACE_FORMAT_TYPE                                  0x00000f00
NV_097_SET_SURFACE_FORMAT_TYPE__BITSHIFT                        8
NV_097_SET_SURFACE_FORMAT_TYPE_PITCH                            1
NV_097_SET_SURFACE_FORMAT_TYPE_SWIZZLE                          2
NV_097_SET_SURFACE_FORMAT_ANTI_ALIASING                         0x0000f000
NV_097_SET_SURFACE_FORMAT_ANTI_ALIASING__BITSHIFT               12
NV_097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_1                0
NV_097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_CORNER_2         1
NV_097_SET_SURFACE_FORMAT_ANTI_ALIASING_SQUARE_OFFSET_4         2
NV_097_SET_SURFACE_FORMAT_WIDTH                                 0x00ff0000
NV_097_SET_SURFACE_FORMAT_WIDTH__BITSHIFT                       16
NV_097_SET_SURFACE_FORMAT_WIDTH_1                               0
NV_097_SET_SURFACE_FORMAT_WIDTH_2                               1
NV_097_SET_SURFACE_FORMAT_WIDTH_4                               2
NV_097_SET_SURFACE_FORMAT_WIDTH_8                               3
NV_097_SET_SURFACE_FORMAT_WIDTH_16                              4
NV_097_SET_SURFACE_FORMAT_WIDTH_32                              5
NV_097_SET_SURFACE_FORMAT_WIDTH_64                              6
NV_097_SET_SURFACE_FORMAT_WIDTH_128                             7
NV_097_SET_SURFACE_FORMAT_WIDTH_256                             8
NV_097_SET_SURFACE_FORMAT_WIDTH_512                             9
NV_097_SET_SURFACE_FORMAT_WIDTH_1024                            10
NV_097_SET_SURFACE_FORMAT_WIDTH_2048                            11
NV_097_SET_SURFACE_FORMAT_WIDTH_4096                            12
NV_097_SET_SURFACE_FORMAT_HEIGHT                                0xff000000
NV_097_SET_SURFACE_FORMAT_HEIGHT__BITSHIFT                      24
NV_097_SET_SURFACE_FORMAT_HEIGHT_1                              0
NV_097_SET_SURFACE_FORMAT_HEIGHT_2                              1
NV_097_SET_SURFACE_FORMAT_HEIGHT_4                              2
NV_097_SET_SURFACE_FORMAT_HEIGHT_8                              3
NV_097_SET_SURFACE_FORMAT_HEIGHT_16                             4
NV_097_SET_SURFACE_FORMAT_HEIGHT_32                             5
NV_097_SET_SURFACE_FORMAT_HEIGHT_64                             6
NV_097_SET_SURFACE_FORMAT_HEIGHT_128                            7
NV_097_SET_SURFACE_FORMAT_HEIGHT_256                            8
NV_097_SET_SURFACE_FORMAT_HEIGHT_512                            9
NV_097_SET_SURFACE_FORMAT_HEIGHT_1024                           10
NV_097_SET_SURFACE_FORMAT_HEIGHT_2048                           11
NV_097_SET_SURFACE_FORMAT_HEIGHT_4096                           12
#endif
    NAMER(NV_097_SET_ZSTENCIL_CLEAR_VALUE),
    NAMER(NV_097_SET_COLOR_CLEAR_VALUE),
    NAMER(NV_097_CLEAR_SURFACE),
#if 0
NV_097_CLEAR_SURFACE_Z                                          0x00000001
NV_097_CLEAR_SURFACE_Z__BITSHIFT                                0
NV_097_CLEAR_SURFACE_Z_DISABLE                                  0
NV_097_CLEAR_SURFACE_Z_ENABLE                                   1
NV_097_CLEAR_SURFACE_STENCIL                                    0x00000002
NV_097_CLEAR_SURFACE_STENCIL__BITSHIFT                          1
NV_097_CLEAR_SURFACE_STENCIL_DISABLE                            0
NV_097_CLEAR_SURFACE_STENCIL_ENABLE                             1
NV_097_CLEAR_SURFACE_R                                          0x00000010
NV_097_CLEAR_SURFACE_R__BITSHIFT                                4
NV_097_CLEAR_SURFACE_R_DISABLE                                  0
NV_097_CLEAR_SURFACE_R_ENABLE                                   1
NV_097_CLEAR_SURFACE_G                                          0x00000020
NV_097_CLEAR_SURFACE_G__BITSHIFT                                5
NV_097_CLEAR_SURFACE_G_DISABLE                                  0
NV_097_CLEAR_SURFACE_G_ENABLE                                   1
NV_097_CLEAR_SURFACE_B                                          0x00000040
NV_097_CLEAR_SURFACE_B__BITSHIFT                                6
NV_097_CLEAR_SURFACE_B_DISABLE                                  0
NV_097_CLEAR_SURFACE_B_ENABLE                                   1
NV_097_CLEAR_SURFACE_A                                          0x00000080
NV_097_CLEAR_SURFACE_A__BITSHIFT                                7
NV_097_CLEAR_SURFACE_A_DISABLE                                  0
NV_097_CLEAR_SURFACE_A_ENABLE                                   1
#endif
    NAMER(NV_097_SET_CLEAR_RECT_HORIZONTAL),
#if 0
NV_097_SET_CLEAR_RECT_HORIZONTAL_XMIN                           0x0000ffff
NV_097_SET_CLEAR_RECT_HORIZONTAL_XMIN__BITSHIFT                 0
NV_097_SET_CLEAR_RECT_HORIZONTAL_XMAX                           0xffff0000
NV_097_SET_CLEAR_RECT_HORIZONTAL_XMAX__BITSHIFT                 16
#endif
    NAMER(NV_097_SET_CLEAR_RECT_VERTICAL),
#if 0
NV_097_SET_CLEAR_RECT_VERTICAL_YMIN                             0x0000ffff
NV_097_SET_CLEAR_RECT_VERTICAL_YMIN__BITSHIFT                   0
NV_097_SET_CLEAR_RECT_VERTICAL_YMAX                             0xffff0000
NV_097_SET_CLEAR_RECT_VERTICAL_YMAX__BITSHIFT                   16
#endif
    NAMER(NV_097_SET_BEGIN_PATCH0),
#if 0
NV_097_SET_BEGIN_PATCH0_POSITION_DEGREE                         0x0000000f
NV_097_SET_BEGIN_PATCH0_POSITION_DEGREE__BITSHIFT               0
NV_097_SET_BEGIN_PATCH0_PARAM1_DEGREE                           0x000000f0
NV_097_SET_BEGIN_PATCH0_PARAM1_DEGREE__BITSHIFT                 4
NV_097_SET_BEGIN_PATCH0_PARAM2_DEGREE                           0x00000f00
NV_097_SET_BEGIN_PATCH0_PARAM2_DEGREE__BITSHIFT                 8
NV_097_SET_BEGIN_PATCH0_PARAM3_DEGREE                           0x0000f000
NV_097_SET_BEGIN_PATCH0_PARAM3_DEGREE__BITSHIFT                 12
NV_097_SET_BEGIN_PATCH0_PARAM4_DEGREE                           0x000f0000
NV_097_SET_BEGIN_PATCH0_PARAM4_DEGREE__BITSHIFT                 16
NV_097_SET_BEGIN_PATCH0_PARAM5_DEGREE                           0x00f00000
NV_097_SET_BEGIN_PATCH0_PARAM5_DEGREE__BITSHIFT                 20
NV_097_SET_BEGIN_PATCH0_PARAM6_DEGREE                           0x0f000000
NV_097_SET_BEGIN_PATCH0_PARAM6_DEGREE__BITSHIFT                 24
NV_097_SET_BEGIN_PATCH0_PARAM7_DEGREE                           0xf0000000
NV_097_SET_BEGIN_PATCH0_PARAM7_DEGREE__BITSHIFT                 28
#endif
    NAMER(NV_097_SET_BEGIN_PATCH1),
#if 0
NV_097_SET_BEGIN_PATCH1_PARAM8_DEGREE                           0x0000000f
NV_097_SET_BEGIN_PATCH1_PARAM8_DEGREE__BITSHIFT                 0
NV_097_SET_BEGIN_PATCH1_PARAM9_DEGREE                           0x000000f0
NV_097_SET_BEGIN_PATCH1_PARAM9_DEGREE__BITSHIFT                 4
NV_097_SET_BEGIN_PATCH1_PARAM10_DEGREE                          0x00000f00
NV_097_SET_BEGIN_PATCH1_PARAM10_DEGREE__BITSHIFT                8
NV_097_SET_BEGIN_PATCH1_PARAM11_DEGREE                          0x0000f000
NV_097_SET_BEGIN_PATCH1_PARAM11_DEGREE__BITSHIFT                12
NV_097_SET_BEGIN_PATCH1_PARAM12_DEGREE                          0x000f0000
NV_097_SET_BEGIN_PATCH1_PARAM12_DEGREE__BITSHIFT                16
NV_097_SET_BEGIN_PATCH1_PARAM13_DEGREE                          0x00f00000
NV_097_SET_BEGIN_PATCH1_PARAM13_DEGREE__BITSHIFT                20
NV_097_SET_BEGIN_PATCH1_PARAM14_DEGREE                          0x0f000000
NV_097_SET_BEGIN_PATCH1_PARAM14_DEGREE__BITSHIFT                24
NV_097_SET_BEGIN_PATCH1_PARAM15_DEGREE                          0xf0000000
NV_097_SET_BEGIN_PATCH1_PARAM15_DEGREE__BITSHIFT                28
#endif
    NAMER(NV_097_SET_BEGIN_PATCH2),
#if 0
NV_097_SET_BEGIN_PATCH2_SWATCH_ROWS                             0x000000ff
NV_097_SET_BEGIN_PATCH2_SWATCH_ROWS__BITSHIFT                   0
NV_097_SET_BEGIN_PATCH2_SWATCH_COLS                             0x0000ff00
NV_097_SET_BEGIN_PATCH2_SWATCH_COLS__BITSHIFT                   8
NV_097_SET_BEGIN_PATCH2_SWATCH_SIZE                             0x001f0000
NV_097_SET_BEGIN_PATCH2_SWATCH_SIZE__BITSHIFT                   16
NV_097_SET_BEGIN_PATCH2_PARTIAL_SWATCH_WIDTH                    0x03e00000
NV_097_SET_BEGIN_PATCH2_PARTIAL_SWATCH_WIDTH__BITSHIFT          21
NV_097_SET_BEGIN_PATCH2_PARTIAL_SWATCH_HEIGHT                   0x7c000000
NV_097_SET_BEGIN_PATCH2_PARTIAL_SWATCH_HEIGHT__BITSHIFT         26
NV_097_SET_BEGIN_PATCH2_PATCH_TYPE                              0x80000000
NV_097_SET_BEGIN_PATCH2_PATCH_TYPE__BITSHIFT                    31
NV_097_SET_BEGIN_PATCH2_PATCH_TYPE_SQUARE                       0
#endif
    NAMER(NV_097_SET_BEGIN_PATCH3),
#if 0
NV_097_SET_BEGIN_PATCH3_ROW_TRNS                                0x00000007
NV_097_SET_BEGIN_PATCH3_ROW_TRNS__BITSHIFT                      0
NV_097_SET_BEGIN_PATCH3_ROW_TRNS_NONE                           0
NV_097_SET_BEGIN_PATCH3_ROW_TRNS_FIRST                          1
NV_097_SET_BEGIN_PATCH3_ROW_TRNS_LAST                           2
NV_097_SET_BEGIN_PATCH3_ROW_TRNS_FIRST_AND_LAST                 3
NV_097_SET_BEGIN_PATCH3_ROW_TRNS_REV_FIRST                      5
NV_097_SET_BEGIN_PATCH3_ROW_TRNS_REV_LAST                       6
NV_097_SET_BEGIN_PATCH3_COL_TRNS                                0x00000038
NV_097_SET_BEGIN_PATCH3_COL_TRNS__BITSHIFT                      3
NV_097_SET_BEGIN_PATCH3_COL_TRNS_NONE                           0
NV_097_SET_BEGIN_PATCH3_COL_TRNS_FIRST                          1
NV_097_SET_BEGIN_PATCH3_COL_TRNS_LAST                           2
NV_097_SET_BEGIN_PATCH3_COL_TRNS_FIRST_AND_LAST                 3
NV_097_SET_BEGIN_PATCH3_COL_TRNS_REV_FIRST                      5
NV_097_SET_BEGIN_PATCH3_COL_TRNS_REV_LAST                       6
NV_097_SET_BEGIN_PATCH3_POSITION_GUARD_CURVE_DEGREE             0x000003c0
NV_097_SET_BEGIN_PATCH3_POSITION_GUARD_CURVE_DEGREE__BITSHIFT   6
NV_097_SET_BEGIN_PATCH3_NORMAL_GUARD_CURVE_DEGREE               0x00003c00
NV_097_SET_BEGIN_PATCH3_NORMAL_GUARD_CURVE_DEGREE__BITSHIFT     10
NV_097_SET_BEGIN_PATCH3_PRIMITIVE                               0x0000c000
NV_097_SET_BEGIN_PATCH3_PRIMITIVE__BITSHIFT                     14
NV_097_SET_BEGIN_PATCH3_PRIMITIVE_TRI_STRIP                     0
NV_097_SET_BEGIN_PATCH3_PRIMITIVE_REVERSED_TRI_STRIP            1
NV_097_SET_BEGIN_PATCH3_PRIMITIVE_BW_TRI_STRIP                  2
NV_097_SET_BEGIN_PATCH3_PRIMITIVE_BW_REVERSED_TRI_STRIP         3
NV_097_SET_BEGIN_PATCH3_TESSELATION                             0x00010000
NV_097_SET_BEGIN_PATCH3_TESSELATION__BITSHIFT                   16
NV_097_SET_BEGIN_PATCH3_TESSELATION_ADAPTIVE_STITCH             0
NV_097_SET_BEGIN_PATCH3_TESSELATION_FIXED_STITCH                1
NV_097_SET_BEGIN_PATCH3_NUM_COEFFS                              0xff000000
NV_097_SET_BEGIN_PATCH3_NUM_COEFFS__BITSHIFT                    24
#endif
    NAMER(NV_097_SET_END_PATCH),
    NAMER(NV_097_SET_BEGIN_END_SWATCH),
#if 0
NV_097_SET_BEGIN_END_SWATCH_SWATCH_CMD                          0x0000000f
NV_097_SET_BEGIN_END_SWATCH_SWATCH_CMD__BITSHIFT                0
NV_097_SET_BEGIN_END_SWATCH_SWATCH_CMD_END                      0
NV_097_SET_BEGIN_END_SWATCH_SWATCH_CMD_BEGIN                    1
NV_097_SET_BEGIN_END_SWATCH_NEW_SWATH                           0x000000f0
NV_097_SET_BEGIN_END_SWATCH_NEW_SWATH__BITSHIFT                 4
NV_097_SET_BEGIN_END_SWATCH_NEW_SWATH_CONTINUE                  0
NV_097_SET_BEGIN_END_SWATCH_NEW_SWATH_NEW                       1
NV_097_SET_BEGIN_END_SWATCH_SKIP_FIRST_ROW                      0x00000f00
NV_097_SET_BEGIN_END_SWATCH_SKIP_FIRST_ROW__BITSHIFT            8
NV_097_SET_BEGIN_END_SWATCH_SKIP_FIRST_ROW_FALSE                0
NV_097_SET_BEGIN_END_SWATCH_SKIP_FIRST_ROW_TRUE                 1
NV_097_SET_BEGIN_END_SWATCH_SKIP_FIRST_COL                      0x0000f000
NV_097_SET_BEGIN_END_SWATCH_SKIP_FIRST_COL__BITSHIFT            12
NV_097_SET_BEGIN_END_SWATCH_SKIP_FIRST_COL_FALSE                0
NV_097_SET_BEGIN_END_SWATCH_SKIP_FIRST_COL_TRUE                 1
NV_097_SET_BEGIN_END_SWATCH_SHORT_SWATCH                        0x000f0000
NV_097_SET_BEGIN_END_SWATCH_SHORT_SWATCH__BITSHIFT              16
NV_097_SET_BEGIN_END_SWATCH_SHORT_SWATCH_FULL_HEIGHT            0
NV_097_SET_BEGIN_END_SWATCH_SHORT_SWATCH_PARTIAL_HEIGHT         1
NV_097_SET_BEGIN_END_SWATCH_NARROW_SWATCH                       0xfff00000
NV_097_SET_BEGIN_END_SWATCH_NARROW_SWATCH__BITSHIFT             20
NV_097_SET_BEGIN_END_SWATCH_NARROW_SWATCH_FULL_WIDTH            0
NV_097_SET_BEGIN_END_SWATCH_NARROW_SWATCH_PARTIAL_WIDTH         1
#endif
    NAMER(NV_097_SET_BEGIN_END_CURVE),
#if 0
NV_097_SET_BEGIN_END_CURVE_CMD                                  0x0000000f
NV_097_SET_BEGIN_END_CURVE_CMD__BITSHIFT                        0
NV_097_SET_BEGIN_END_CURVE_CMD_END_CURVE_DATA                   0
NV_097_SET_BEGIN_END_CURVE_CMD_STRIP_CURVE                      1
NV_097_SET_BEGIN_END_CURVE_CMD_LEFT_GUARD_CURVE                 2
NV_097_SET_BEGIN_END_CURVE_CMD_RIGHT_GUARD_CURVE                3
NV_097_SET_BEGIN_END_CURVE_CMD_OUTER_TRANSITION_CURVE           4
NV_097_SET_BEGIN_END_CURVE_CMD_INNER_TRANSITION_CURVE           5
NV_097_SET_BEGIN_END_CURVE_CMD_OUTER_END_PT                     6
NV_097_SET_BEGIN_END_CURVE_CMD_INNER_END_PT                     7
#endif
    NAMER(NV_097_SET_CURVE_COEFFICIENTS_0),
    NAMER(NV_097_SET_CURVE_COEFFICIENTS_1),
    NAMER(NV_097_SET_CURVE_COEFFICIENTS_2),
    NAMER(NV_097_SET_CURVE_COEFFICIENTS_3),
    NAMER(NV_097_SET_BEGIN_TRANSITION0),
#if 0
NV_097_SET_BEGIN_TRANSITION0_POSITION_DEGREE                    0x0000000f
NV_097_SET_BEGIN_TRANSITION0_POSITION_DEGREE__BITSHIFT          0
NV_097_SET_BEGIN_TRANSITION0_PARAM1_DEGREE                      0x000000f0
NV_097_SET_BEGIN_TRANSITION0_PARAM1_DEGREE__BITSHIFT            4
NV_097_SET_BEGIN_TRANSITION0_PARAM2_DEGREE                      0x00000f00
NV_097_SET_BEGIN_TRANSITION0_PARAM2_DEGREE__BITSHIFT            8
NV_097_SET_BEGIN_TRANSITION0_PARAM3_DEGREE                      0x0000f000
NV_097_SET_BEGIN_TRANSITION0_PARAM3_DEGREE__BITSHIFT            12
NV_097_SET_BEGIN_TRANSITION0_PARAM4_DEGREE                      0x000f0000
NV_097_SET_BEGIN_TRANSITION0_PARAM4_DEGREE__BITSHIFT            16
NV_097_SET_BEGIN_TRANSITION0_PARAM5_DEGREE                      0x00f00000
NV_097_SET_BEGIN_TRANSITION0_PARAM5_DEGREE__BITSHIFT            20
NV_097_SET_BEGIN_TRANSITION0_PARAM6_DEGREE                      0x0f000000
NV_097_SET_BEGIN_TRANSITION0_PARAM6_DEGREE__BITSHIFT            24
NV_097_SET_BEGIN_TRANSITION0_PARAM7_DEGREE                      0xf0000000
NV_097_SET_BEGIN_TRANSITION0_PARAM7_DEGREE__BITSHIFT            28
#endif
    NAMER(NV_097_SET_BEGIN_TRANSITION1),
#if 0
NV_097_SET_BEGIN_TRANSITION1_PARAM8_DEGREE                      0x0000000f
NV_097_SET_BEGIN_TRANSITION1_PARAM8_DEGREE__BITSHIFT            0
NV_097_SET_BEGIN_TRANSITION1_PARAM9_DEGREE                      0x000000f0
NV_097_SET_BEGIN_TRANSITION1_PARAM9_DEGREE__BITSHIFT            4
NV_097_SET_BEGIN_TRANSITION1_PARAM10_DEGREE                     0x00000f00
NV_097_SET_BEGIN_TRANSITION1_PARAM10_DEGREE__BITSHIFT           8
NV_097_SET_BEGIN_TRANSITION1_PARAM11_DEGREE                     0x0000f000
NV_097_SET_BEGIN_TRANSITION1_PARAM11_DEGREE__BITSHIFT           12
NV_097_SET_BEGIN_TRANSITION1_PARAM12_DEGREE                     0x000f0000
NV_097_SET_BEGIN_TRANSITION1_PARAM12_DEGREE__BITSHIFT           16
NV_097_SET_BEGIN_TRANSITION1_PARAM13_DEGREE                     0x00f00000
NV_097_SET_BEGIN_TRANSITION1_PARAM13_DEGREE__BITSHIFT           20
NV_097_SET_BEGIN_TRANSITION1_PARAM14_DEGREE                     0x0f000000
NV_097_SET_BEGIN_TRANSITION1_PARAM14_DEGREE__BITSHIFT           24
NV_097_SET_BEGIN_TRANSITION1_PARAM15_DEGREE                     0xf0000000
NV_097_SET_BEGIN_TRANSITION1_PARAM15_DEGREE__BITSHIFT           28
#endif
    NAMER(NV_097_SET_BEGIN_TRANSITION2),
#if 0
NV_097_SET_BEGIN_TRANSITION2_INSIDE_SEGMENTS                    0x000003ff
NV_097_SET_BEGIN_TRANSITION2_INSIDE_SEGMENTS__BITSHIFT          0
NV_097_SET_BEGIN_TRANSITION2_OUTSIDE_SEGMENTS                   0x000ffc00
NV_097_SET_BEGIN_TRANSITION2_OUTSIDE_SEGMENTS__BITSHIFT         10
NV_097_SET_BEGIN_TRANSITION2_NUM_COEFFS                         0xff000000
NV_097_SET_BEGIN_TRANSITION2_NUM_COEFFS__BITSHIFT               24
#endif
    NAMER(NV_097_SET_END_TRANSITION),
    NAMER(NV_097_SET_SHADOW_ZSLOPE_THRESHOLD),
    NAMER(NV_097_SET_SHADOW_DEPTH_FUNC),
#if 0
NV_097_SET_SHADOW_DEPTH_FUNC_V_NEVER                            0
NV_097_SET_SHADOW_DEPTH_FUNC_V_LESS                             1
NV_097_SET_SHADOW_DEPTH_FUNC_V_EQUAL                            2
NV_097_SET_SHADOW_DEPTH_FUNC_V_LEQUAL                           3
NV_097_SET_SHADOW_DEPTH_FUNC_V_GREATER                          4
NV_097_SET_SHADOW_DEPTH_FUNC_V_NOTEQUAL                         5
NV_097_SET_SHADOW_DEPTH_FUNC_V_GEQUAL                           6
NV_097_SET_SHADOW_DEPTH_FUNC_V_ALWAYS                           7
#endif
    NAMER(NV_097_SET_SHADER_STAGE_PROGRAM),
#if 0
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE0                          0x0000001f
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE0__BITSHIFT                0
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE0_PROGRAM_NONE             0
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE0_2D_PROJECTIVE            1
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE0_3D_PROJECTIVE            2
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE0_CUBE_MAP                 3
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE0_PASS_THROUGH             4
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE0_CLIP_PLANE               5
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE1                          0x000003e0
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE1__BITSHIFT                5
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE1_PROGRAM_NONE             0
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE1_2D_PROJECTIVE            1
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE1_3D_PROJECTIVE            2
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE1_CUBE_MAP                 3
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE1_PASS_THROUGH             4
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE1_CLIP_PLANE               5
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP               6
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP_LUMINANCE     7
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE1_DEPENDENT_AR             15
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE1_DEPENDENT_GB             16
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE1_DOT_PRODUCT              17
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2                          0x00007c00
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2__BITSHIFT                10
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2_PROGRAM_NONE             0
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2_2D_PROJECTIVE            1
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2_3D_PROJECTIVE            2
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2_CUBE_MAP                 3
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2_PASS_THROUGH             4
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2_CLIP_PLANE               5
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2_BUMPENVMAP               6
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2_BUMPENVMAP_LUMINANCE     7
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2_BRDF                     8
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_ST                   9
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_ZW                   10
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_REFLECT_DIFFUSE      11
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2_DEPENDENT_AR             15
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2_DEPENDENT_GB             16
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_PRODUCT              17
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3                          0x000f8000
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3__BITSHIFT                15
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_PROGRAM_NONE             0
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_2D_PROJECTIVE            1
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_3D_PROJECTIVE            2
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_CUBE_MAP                 3
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_PASS_THROUGH             4
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_CLIP_PLANE               5
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_BUMPENVMAP               6
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_BUMPENVMAP_LUMINANCE     7
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_BRDF                     8
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_ST                   9
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_ZW                   10
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_REFLECT_SPECULAR     12
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_STR_3D               13
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_STR_CUBE             14
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_DEPENDENT_AR             15
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_DEPENDENT_GB             16
NV_097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_REFLECT_SPECULAR_CONST 18
#endif
    NAMER(NV_097_SET_EYE_VECTOR_0),
    NAMER(NV_097_SET_EYE_VECTOR_1),
    NAMER(NV_097_SET_EYE_VECTOR_2),
    NAMER(NV_097_SET_DOT_RGBMAPPING),
#if 0
NV_097_SET_DOT_RGBMAPPING_STAGE1                                0x0000000f
NV_097_SET_DOT_RGBMAPPING_STAGE1__BITSHIFT                      0
NV_097_SET_DOT_RGBMAPPING_STAGE1_ZERO_TO_1                      0
NV_097_SET_DOT_RGBMAPPING_STAGE1_MINUS_1_TO_1_MS                1
NV_097_SET_DOT_RGBMAPPING_STAGE1_MINUS_1_TO_1_GL                2
NV_097_SET_DOT_RGBMAPPING_STAGE1_MINUS_1_TO_1_NV                3
NV_097_SET_DOT_RGBMAPPING_STAGE1_HILO_1                         4
NV_097_SET_DOT_RGBMAPPING_STAGE1_HILO_HEMISPHERE_MS             5
NV_097_SET_DOT_RGBMAPPING_STAGE1_HILO_HEMISPHERE_GL             6
NV_097_SET_DOT_RGBMAPPING_STAGE1_HILO_HEMISPHERE_NV             7
NV_097_SET_DOT_RGBMAPPING_STAGE2                                0x000000f0
NV_097_SET_DOT_RGBMAPPING_STAGE2__BITSHIFT                      4
NV_097_SET_DOT_RGBMAPPING_STAGE2_ZERO_TO_1                      0
NV_097_SET_DOT_RGBMAPPING_STAGE2_MINUS_1_TO_1_MS                1
NV_097_SET_DOT_RGBMAPPING_STAGE2_MINUS_1_TO_1_GL                2
NV_097_SET_DOT_RGBMAPPING_STAGE2_MINUS_1_TO_1_NV                3
NV_097_SET_DOT_RGBMAPPING_STAGE2_HILO_1                         4
NV_097_SET_DOT_RGBMAPPING_STAGE2_HILO_HEMISPHERE_MS             5
NV_097_SET_DOT_RGBMAPPING_STAGE2_HILO_HEMISPHERE_GL             6
NV_097_SET_DOT_RGBMAPPING_STAGE2_HILO_HEMISPHERE_NV             7
NV_097_SET_DOT_RGBMAPPING_STAGE3                                0x00000f00
NV_097_SET_DOT_RGBMAPPING_STAGE3__BITSHIFT                      8
NV_097_SET_DOT_RGBMAPPING_STAGE3_ZERO_TO_1                      0
NV_097_SET_DOT_RGBMAPPING_STAGE3_MINUS_1_TO_1_MS                1
NV_097_SET_DOT_RGBMAPPING_STAGE3_MINUS_1_TO_1_GL                2
NV_097_SET_DOT_RGBMAPPING_STAGE3_MINUS_1_TO_1_NV                3
NV_097_SET_DOT_RGBMAPPING_STAGE3_HILO_1                         4
NV_097_SET_DOT_RGBMAPPING_STAGE3_HILO_HEMISPHERE_MS             5
NV_097_SET_DOT_RGBMAPPING_STAGE3_HILO_HEMISPHERE_GL             6
NV_097_SET_DOT_RGBMAPPING_STAGE3_HILO_HEMISPHERE_NV             7
#endif
    NAMER(NV_097_SET_SHADER_CLIP_PLANE_MODE),
#if 0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE0_S                      0x00000001
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE0_S__BITSHIFT            0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE0_S_CLIPLTZ              0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE0_S_CLIPGEZ              1
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE0_T                      0x00000002
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE0_T__BITSHIFT            1
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE0_T_CLIPLTZ              0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE0_T_CLIPGEZ              1
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE0_R                      0x00000004
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE0_R__BITSHIFT            2
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE0_R_CLIPLTZ              0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE0_R_CLIPGEZ              1
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE0_Q                      0x00000008
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE0_Q__BITSHIFT            3
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE0_Q_CLIPLTZ              0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE0_Q_CLIPGEZ              1
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE1_S                      0x00000010
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE1_S__BITSHIFT            4
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE1_S_CLIPLTZ              0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE1_S_CLIPGEZ              1
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE1_T                      0x00000020
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE1_T__BITSHIFT            5
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE1_T_CLIPLTZ              0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE1_T_CLIPGEZ              1
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE1_R                      0x00000040
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE1_R__BITSHIFT            6
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE1_R_CLIPLTZ              0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE1_R_CLIPGEZ              1
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE1_Q                      0x00000080
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE1_Q__BITSHIFT            7
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE1_Q_CLIPLTZ              0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE1_Q_CLIPGEZ              1
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE2_S                      0x00000100
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE2_S__BITSHIFT            8
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE2_S_CLIPLTZ              0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE2_S_CLIPGEZ              1
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE2_T                      0x00000200
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE2_T__BITSHIFT            9
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE2_T_CLIPLTZ              0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE2_T_CLIPGEZ              1
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE2_R                      0x00000400
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE2_R__BITSHIFT            10
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE2_R_CLIPLTZ              0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE2_R_CLIPGEZ              1
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE2_Q                      0x00000800
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE2_Q__BITSHIFT            11
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE2_Q_CLIPLTZ              0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE2_Q_CLIPGEZ              1
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE3_S                      0x00001000
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE3_S__BITSHIFT            12
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE3_S_CLIPLTZ              0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE3_S_CLIPGEZ              1
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE3_T                      0x00002000
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE3_T__BITSHIFT            13
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE3_T_CLIPLTZ              0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE3_T_CLIPGEZ              1
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE3_R                      0x00004000
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE3_R__BITSHIFT            14
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE3_R_CLIPLTZ              0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE3_R_CLIPGEZ              1
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE3_Q                      0x00008000
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE3_Q__BITSHIFT            15
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE3_Q_CLIPLTZ              0
NV_097_SET_SHADER_CLIP_PLANE_MODE_STAGE3_Q_CLIPGEZ              1
#endif
    NAMER(NV_097_SET_SHADER_OTHER_STAGE_INPUT),
#if 0
NV_097_SET_SHADER_OTHER_STAGE_INPUT_STAGE1                      0x0000ffff
NV_097_SET_SHADER_OTHER_STAGE_INPUT_STAGE1__BITSHIFT            0
NV_097_SET_SHADER_OTHER_STAGE_INPUT_STAGE1_INSTAGE_0            0
NV_097_SET_SHADER_OTHER_STAGE_INPUT_STAGE2                      0x000f0000
NV_097_SET_SHADER_OTHER_STAGE_INPUT_STAGE2__BITSHIFT            16
NV_097_SET_SHADER_OTHER_STAGE_INPUT_STAGE2_INSTAGE_0            0
NV_097_SET_SHADER_OTHER_STAGE_INPUT_STAGE2_INSTAGE_1            1
NV_097_SET_SHADER_OTHER_STAGE_INPUT_STAGE3                      0x00f00000
NV_097_SET_SHADER_OTHER_STAGE_INPUT_STAGE3__BITSHIFT            20
NV_097_SET_SHADER_OTHER_STAGE_INPUT_STAGE3_INSTAGE_0            0
NV_097_SET_SHADER_OTHER_STAGE_INPUT_STAGE3_INSTAGE_1            1
NV_097_SET_SHADER_OTHER_STAGE_INPUT_STAGE3_INSTAGE_2            2
#endif
    NAMER(NV_097_SET_SPECULAR_FOG_FACTOR_0),
    NAMER(NV_097_SET_SPECULAR_FOG_FACTOR_1),
#if 0
NV_097_SET_SPECULAR_FOG_FACTOR_BLUE                             0x000000ff
NV_097_SET_SPECULAR_FOG_FACTOR_BLUE__BITSHIFT                   0
NV_097_SET_SPECULAR_FOG_FACTOR_GREEN                            0x0000ff00
NV_097_SET_SPECULAR_FOG_FACTOR_GREEN__BITSHIFT                  8
NV_097_SET_SPECULAR_FOG_FACTOR_RED                              0x00ff0000
NV_097_SET_SPECULAR_FOG_FACTOR_RED__BITSHIFT                    16
NV_097_SET_SPECULAR_FOG_FACTOR_ALPHA                            0xff000000
NV_097_SET_SPECULAR_FOG_FACTOR_ALPHA__BITSHIFT                  24
#endif
    NAMER(NV_097_SET_COMBINER_CONTROL),
#if 0
NV_097_SET_COMBINER_CONTROL_ITERATION_COUNT                     0x000000ff
NV_097_SET_COMBINER_CONTROL_ITERATION_COUNT__BITSHIFT           0
NV_097_SET_COMBINER_CONTROL_ITERATION_COUNT_ONE                 1
NV_097_SET_COMBINER_CONTROL_ITERATION_COUNT_TWO                 2
NV_097_SET_COMBINER_CONTROL_ITERATION_COUNT_THREE               3
NV_097_SET_COMBINER_CONTROL_ITERATION_COUNT_FOUR                4
NV_097_SET_COMBINER_CONTROL_ITERATION_COUNT_FIVE                5
NV_097_SET_COMBINER_CONTROL_ITERATION_COUNT_SIX                 6
NV_097_SET_COMBINER_CONTROL_ITERATION_COUNT_SEVEN               7
NV_097_SET_COMBINER_CONTROL_ITERATION_COUNT_EIGHT               8
NV_097_SET_COMBINER_CONTROL_MUX_SELECT                          0x00000f00
NV_097_SET_COMBINER_CONTROL_MUX_SELECT__BITSHIFT                8
NV_097_SET_COMBINER_CONTROL_MUX_SELECT_LSB                      0
NV_097_SET_COMBINER_CONTROL_MUX_SELECT_MSB                      1
NV_097_SET_COMBINER_CONTROL_FACTOR0                             0x0000f000
NV_097_SET_COMBINER_CONTROL_FACTOR0__BITSHIFT                   12
NV_097_SET_COMBINER_CONTROL_FACTOR0_SAME_FACTOR_ALL             0
NV_097_SET_COMBINER_CONTROL_FACTOR0_EACH_STAGE                  1
NV_097_SET_COMBINER_CONTROL_FACTOR1                             0xffff0000
NV_097_SET_COMBINER_CONTROL_FACTOR1__BITSHIFT                   16
NV_097_SET_COMBINER_CONTROL_FACTOR1_SAME_FACTOR_ALL             0
NV_097_SET_COMBINER_CONTROL_FACTOR1_EACH_STAGE                  1
#endif
    NAMER(NV_097_SET_COMBINER_COLOR_OCW_0),
    NAMER(NV_097_SET_COMBINER_COLOR_OCW_1),
    NAMER(NV_097_SET_COMBINER_COLOR_OCW_2),
    NAMER(NV_097_SET_COMBINER_COLOR_OCW_3),
    NAMER(NV_097_SET_COMBINER_COLOR_OCW_4),
    NAMER(NV_097_SET_COMBINER_COLOR_OCW_5),
    NAMER(NV_097_SET_COMBINER_COLOR_OCW_6),
    NAMER(NV_097_SET_COMBINER_COLOR_OCW_7),
#if 0
NV_097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB                    0x00080000
NV_097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB__BITSHIFT          19
NV_097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_DISABLE            0
NV_097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_AB_DST_ENABLE      1
NV_097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD                    0x00040000
NV_097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD__BITSHIFT          18
NV_097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_DISABLE            0
NV_097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_CD_DST_ENABLE      1
NV_097_SET_COMBINER_COLOR_OCW_OP                                0x00008000
NV_097_SET_COMBINER_COLOR_OCW_OP__BITSHIFT                      15
NV_097_SET_COMBINER_COLOR_OCW_OP_NOSHIFT                        0
NV_097_SET_COMBINER_COLOR_OCW_OP_NOSHIFT_BIAS                   1
NV_097_SET_COMBINER_COLOR_OCW_OP_SHIFTLEFTBY1                   2
NV_097_SET_COMBINER_COLOR_OCW_OP_SHIFTLEFTBY1_BIAS              3
NV_097_SET_COMBINER_COLOR_OCW_OP_SHIFTLEFTBY2                   4
NV_097_SET_COMBINER_COLOR_OCW_OP_SHIFTRIGHTBY1                  6
NV_097_SET_COMBINER_COLOR_OCW_MUX_ENABLE                        0x00004000
NV_097_SET_COMBINER_COLOR_OCW_MUX_ENABLE__BITSHIFT              14
NV_097_SET_COMBINER_COLOR_OCW_MUX_ENABLE_FALSE                  0
NV_097_SET_COMBINER_COLOR_OCW_MUX_ENABLE_TRUE                   1
NV_097_SET_COMBINER_COLOR_OCW_AB_DOT_ENABLE                     0x00002000
NV_097_SET_COMBINER_COLOR_OCW_AB_DOT_ENABLE__BITSHIFT           13
NV_097_SET_COMBINER_COLOR_OCW_AB_DOT_ENABLE_FALSE               0
NV_097_SET_COMBINER_COLOR_OCW_AB_DOT_ENABLE_TRUE                1
NV_097_SET_COMBINER_COLOR_OCW_CD_DOT_ENABLE                     0x00001000
NV_097_SET_COMBINER_COLOR_OCW_CD_DOT_ENABLE__BITSHIFT           12
NV_097_SET_COMBINER_COLOR_OCW_CD_DOT_ENABLE_FALSE               0
NV_097_SET_COMBINER_COLOR_OCW_CD_DOT_ENABLE_TRUE                1
NV_097_SET_COMBINER_COLOR_OCW_SUM_DST                           0x00000f00
NV_097_SET_COMBINER_COLOR_OCW_SUM_DST__BITSHIFT                 8
NV_097_SET_COMBINER_COLOR_OCW_SUM_DST_REG_0                     0
NV_097_SET_COMBINER_COLOR_OCW_SUM_DST_REG_4                     4
NV_097_SET_COMBINER_COLOR_OCW_SUM_DST_REG_5                     5
NV_097_SET_COMBINER_COLOR_OCW_SUM_DST_REG_8                     8
NV_097_SET_COMBINER_COLOR_OCW_SUM_DST_REG_9                     9
NV_097_SET_COMBINER_COLOR_OCW_SUM_DST_REG_A                     10
NV_097_SET_COMBINER_COLOR_OCW_SUM_DST_REG_B                     11
NV_097_SET_COMBINER_COLOR_OCW_SUM_DST_REG_C                     12
NV_097_SET_COMBINER_COLOR_OCW_SUM_DST_REG_D                     13
NV_097_SET_COMBINER_COLOR_OCW_AB_DST                            0x000000f0
NV_097_SET_COMBINER_COLOR_OCW_AB_DST__BITSHIFT                  4
NV_097_SET_COMBINER_COLOR_OCW_AB_DST_REG_0                      0
NV_097_SET_COMBINER_COLOR_OCW_AB_DST_REG_4                      4
NV_097_SET_COMBINER_COLOR_OCW_AB_DST_REG_5                      5
NV_097_SET_COMBINER_COLOR_OCW_AB_DST_REG_8                      8
NV_097_SET_COMBINER_COLOR_OCW_AB_DST_REG_9                      9
NV_097_SET_COMBINER_COLOR_OCW_AB_DST_REG_A                      10
NV_097_SET_COMBINER_COLOR_OCW_AB_DST_REG_B                      11
NV_097_SET_COMBINER_COLOR_OCW_AB_DST_REG_C                      12
NV_097_SET_COMBINER_COLOR_OCW_AB_DST_REG_D                      13
NV_097_SET_COMBINER_COLOR_OCW_CD_DST                            0x0000000f
NV_097_SET_COMBINER_COLOR_OCW_CD_DST__BITSHIFT                  0
NV_097_SET_COMBINER_COLOR_OCW_CD_DST_REG_0                      0
NV_097_SET_COMBINER_COLOR_OCW_CD_DST_REG_4                      4
NV_097_SET_COMBINER_COLOR_OCW_CD_DST_REG_5                      5
NV_097_SET_COMBINER_COLOR_OCW_CD_DST_REG_8                      8
NV_097_SET_COMBINER_COLOR_OCW_CD_DST_REG_9                      9
NV_097_SET_COMBINER_COLOR_OCW_CD_DST_REG_A                      10
NV_097_SET_COMBINER_COLOR_OCW_CD_DST_REG_B                      11
NV_097_SET_COMBINER_COLOR_OCW_CD_DST_REG_C                      12
NV_097_SET_COMBINER_COLOR_OCW_CD_DST_REG_D                      13
#endif
    NAMER(NV_097_SET_TRANSFORM_EXECUTION_MODE),
#if 0
NV_097_SET_TRANSFORM_EXECUTION_MODE_MODE                        0x00000003
NV_097_SET_TRANSFORM_EXECUTION_MODE_MODE__BITSHIFT              0
NV_097_SET_TRANSFORM_EXECUTION_MODE_MODE_FIXED                  0
NV_097_SET_TRANSFORM_EXECUTION_MODE_MODE_PROGRAM                2
NV_097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE                  0xfffffffc
NV_097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE__BITSHIFT        2
NV_097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_USER             0
NV_097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV             1
#endif
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN),
#if 0
NV_097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN_V_READ_ONLY           0
NV_097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN_V_READ_WRITE          1
#endif
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_LOAD),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_START),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_0),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_1),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_2),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_3),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_4),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_5),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_6),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_7),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_8),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_9),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_10),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_11),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_12),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_13),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_14),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_15),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_16),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_17),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_18),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_19),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_20),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_21),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_22),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_23),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_24),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_25),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_26),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_27),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_28),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_29),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_30),
    NAMER(NV_097_SET_TRANSFORM_PROGRAM_31),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_LOAD),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_0),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_1),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_2),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_3),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_4),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_5),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_6),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_7),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_8),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_9),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_10),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_11),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_12),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_13),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_14),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_15),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_16),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_17),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_18),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_19),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_20),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_21),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_22),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_23),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_24),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_25),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_26),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_27),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_28),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_29),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_30),
    NAMER(NV_097_SET_TRANSFORM_CONSTANT_31),
    NAMER(NV_097_SET_TRANSFORM_DATA_0),
    NAMER(NV_097_SET_TRANSFORM_DATA_1),
    NAMER(NV_097_SET_TRANSFORM_DATA_2),
    NAMER(NV_097_SET_TRANSFORM_DATA_3),
    NAMER(NV_097_LAUNCH_TRANSFORM_PROGRAM),
    NAMER(NV_097_SET_TWO_SIDE_LIGHT_EN),
#if 0
NV_097_SET_TWO_SIDE_LIGHT_EN_V_FALSE                            0
NV_097_SET_TWO_SIDE_LIGHT_EN_V_TRUE                             1
#endif
    NAMER(NV_097_SET_BACK_SCENE_AMBIENT_COLOR_0),
    NAMER(NV_097_SET_BACK_SCENE_AMBIENT_COLOR_1),
    NAMER(NV_097_SET_BACK_SCENE_AMBIENT_COLOR_2),
    NAMER(NV_097_SET_BACK_MATERIAL_EMISSION_0),
    NAMER(NV_097_SET_BACK_MATERIAL_EMISSION_1),
    NAMER(NV_097_SET_BACK_MATERIAL_EMISSION_2),
    NAMER(NV_097_CLEAR_REPORT_VALUE),
#if 0
NV_097_CLEAR_REPORT_VALUE_TYPE_ZPASS_PIXEL_CNT                  1
#endif
    NAMER(NV_097_SET_ZPASS_PIXEL_COUNT_ENABLE),
#if 0
NV_097_SET_ZPASS_PIXEL_COUNT_ENABLE_V_FALSE                     0
NV_097_SET_ZPASS_PIXEL_COUNT_ENABLE_V_TRUE                      1
#endif
    NAMER(NV_097_GET_REPORT),
#if 0
NV_097_GET_REPORT_OFFSET                                        0x00ffffff
NV_097_GET_REPORT_OFFSET__BITSHIFT                              0
NV_097_GET_REPORT_TYPE                                          0xff000000
NV_097_GET_REPORT_TYPE__BITSHIFT                                24
NV_097_GET_REPORT_TYPE_ZPASS_PIXEL_CNT                          1
#endif
    NAMER(NV_097_DEBUG_INIT_0),
    NAMER(NV_097_DEBUG_INIT_1),
    NAMER(NV_097_DEBUG_INIT_2),
    NAMER(NV_097_DEBUG_INIT_3),
    NAMER(NV_097_DEBUG_INIT_4),
    NAMER(NV_097_DEBUG_INIT_5),
    NAMER(NV_097_DEBUG_INIT_6),
    NAMER(NV_097_DEBUG_INIT_7),
    NAMER(NV_097_DEBUG_INIT_8),
    NAMER(NV_097_DEBUG_INIT_9),
};

static void
nv2a_pgraph_fifo(register void *p, register const nv2a_pfifo_cache *c) {
    register size_t pos;
    register uint32_t *get;
    nv2a_pgraph_ctx_cache1 *cache1;
    nv2a_pgraph_ctx_cache2 *cache2;
    nv2a_pgraph_ctx_cache3 *cache3;
    nv2a_pgraph_ctx_cache4 *cache4;
    nv2a_pgraph_ctx_cache5 *cache5;
    ENTER_NV2A;

    PGRAPH_FIFO_LOCK;

    while (!NV2A_REG32_MASK_GET(p, NV_PGRAPH, FIFO, ACCESS_ENABLED)) PGRAPH_FIFO_WAIT;

    PGRAPH_FIFO_UNLOCK;

    VARDUMP_NV2A(VAR_IN, c->subchannel);

    pos    = NV_PGRAPH_CTX_CACHE_OFFSET(c->subchannel);
    cache1 = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE1);
    cache2 = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE2);
    cache3 = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE3);
    cache4 = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE4);
    cache5 = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE5);

//fprintf(stderr,"pgraph: method:0x%x | subchannel:%u | param:0x%x | grclass:0x%x\n",c->method,c->subchannel,c->param,cache1->grclass);//XXX

    if (c->method == /* NV_SET_OBJECT */ 0) {
        VARDUMP_NV2A(VAR_IN, c->method);
        VARDUMP_NV2A(VAR_IN, c->param);
        pos = c->param;
        if (pos + 20 > NV2A_BLOCK_SIZE(NV_PRAMIN) || pos % 4) INT3;
        get = NV2A_REGADDR(p + pos, NV_PRAMIN, BASE);
//fprintf(stderr,"pgraph: switch1:0x%x | switch2:0x%x | switch3:0x%x | switch4:0x%x | switch5:0x%x\n",*get,*(get+1),*(get+2),*(get+3),*(get+4));//XXX
        cache1->field = *get++;
        cache2->field = *get++;
        cache3->field = *get++;
        cache4->field = *get++;
        cache5->field = *get++;
        VARDUMP3_NV2A(DUMP, cache1->field, nv2a_pgraph_ctx_cache1_name);
        VARDUMP2_NV2A(DUMP, cache1->grclass, nv2a_pgraph_ctx_cache1_grclass_name);
        VARDUMP2_NV2A(DUMP, cache1->patch_config, nv2a_pgraph_ctx_cache1_patch_config_name);
        VARDUMP2_NV2A(DUMP, cache1->dither_mode, nv2a_pgraph_ctx_cache1_dither_mode_name);
        VARDUMP2_NV2A(DUMP, cache1->class_type, nv2a_pgraph_ctx_cache1_class_type_name);
        VARDUMP_NV2A(DUMP,  cache2->field);
        VARDUMP2_NV2A(DUMP, cache2->mono_format, nv2a_pgraph_ctx_cache2_mono_format_name);
        VARDUMP2_NV2A(DUMP, cache2->color_format, nv2a_pgraph_ctx_cache2_color_format_name);
        VARDUMP_NV2A(DUMP,  cache2->notify_instance);
        VARDUMP_NV2A(DUMP,  cache3->field);
        VARDUMP_NV2A(DUMP,  cache3->dma_instance0);
        VARDUMP_NV2A(DUMP,  cache3->dma_instance1);
        VARDUMP_NV2A(DUMP,  cache4->field);
        VARDUMP_NV2A(DUMP,  cache4->user_instance);
        VARDUMP_NV2A(DUMP,  cache5->field);
        VARDUMP_NV2A(DUMP,  cache5->trap_bits);
        LEAVE_NV2A;
        return;
    }

    VARDUMP2_NV2A(DUMP, cache1->grclass, nv2a_pgraph_ctx_cache1_grclass_name);

    switch (cache1->grclass) {
    case NV10_CONTEXT_SURFACES_2D /* 0x62 */:
        VARDUMP4_NV2A(VAR_IN, c->method, nv2a_062_name);
        pos = c->method;
        if (pos + 4 > NV2A_BLOCK_SIZE(NV_062) || pos % 4) INT3;
        NV2A_REG32(p + pos, NV_062, NV10_CONTEXT_SURFACES_2D) = c->param;
        switch (c->method) {
        case NV_062_COLOR_FORMAT:
            VARDUMP2_NV2A(VAR_IN, c->param, nv2a_062_color_format_name);
            break;
        default:
            VARDUMP_NV2A(VAR_IN, c->param);
            break;
        }
        break;
    case NV12_IMAGE_BLIT /* 0x9f */:
        VARDUMP4_NV2A(VAR_IN, c->method, nv2a_09f_name);
        pos = c->method;
        if (pos + 4 > NV2A_BLOCK_SIZE(NV_09F) || pos % 4) INT3;
        NV2A_REG32(p + pos, NV_09F, NV12_IMAGE_BLIT) = c->param;
        switch (c->method) {
        case NV_09F_SET_OPERATION:
            VARDUMP2_NV2A(VAR_IN, c->param, nv2a_09f_set_operation_name);
            break;
        case NV_09F_SIZE:
            //TODO kick off
#if 0
            /* I guess this kicks it off? */
            if (image_blit->operation == NV09F_SET_OPERATION_SRCCOPY) {

                NV2A_GL_DPRINTF(true, "NV09F_SET_OPERATION_SRCCOPY");

                GraphicsObject *context_surfaces_obj =
                    lookup_graphics_object(pg, image_blit->context_surfaces);
                assert(context_surfaces_obj);
                assert(context_surfaces_obj->graphics_class
                    == NV_CONTEXT_SURFACES_2D);

                ContextSurfaces2DState *context_surfaces =
                    &context_surfaces_obj->data.context_surfaces_2d;

                unsigned int bytes_per_pixel;
                switch (context_surfaces->color_format) {
                case NV062_SET_COLOR_FORMAT_LE_Y8:
                    bytes_per_pixel = 1;
                    break;
                case NV062_SET_COLOR_FORMAT_LE_R5G6B5:
                    bytes_per_pixel = 2;
                    break;
                case NV062_SET_COLOR_FORMAT_LE_A8R8G8B8:
                    bytes_per_pixel = 4;
                    break;
                default:
                    fprintf(stderr, "Unknown blit surface format: 0x%x\n", context_surfaces->color_format);
                    assert(false);
                    break;
                }

                hwaddr source_dma_len, dest_dma_len;
                uint8_t *source, *dest;

                source = nv_dma_map(d, context_surfaces->dma_image_source,
                                    &source_dma_len);
                assert(context_surfaces->source_offset < source_dma_len);
                source += context_surfaces->source_offset;

                dest = nv_dma_map(d, context_surfaces->dma_image_dest,
                                  &dest_dma_len);
                assert(context_surfaces->dest_offset < dest_dma_len);
                dest += context_surfaces->dest_offset;

                NV2A_DPRINTF("  - 0x%tx -> 0x%tx\n", source - d->vram_ptr,
                                                     dest - d->vram_ptr);

                int y;
                for (y=0; y<image_blit->height; y++) {
                    uint8_t *source_row = source
                        + (image_blit->in_y + y) * context_surfaces->source_pitch
                        + image_blit->in_x * bytes_per_pixel;

                    uint8_t *dest_row = dest
                        + (image_blit->out_y + y) * context_surfaces->dest_pitch
                        + image_blit->out_x * bytes_per_pixel;

                    memmove(dest_row, source_row,
                            image_blit->width * bytes_per_pixel);
                }

            }
#endif
        default:
            VARDUMP_NV2A(VAR_IN, c->param);
            break;
        }
        break;
    case NV20_KELVIN_PRIMITIVE /* 0x97 */:
        VARDUMP4_NV2A(VAR_IN, c->method, nv2a_097_name);
        pos = c->method;
        if (pos + 4 > NV2A_BLOCK_SIZE(NV_097) || pos % 4) INT3;
        NV2A_REG32(p + pos, NV_097, NV20_KELVIN_PRIMITIVE) = c->param;
        switch (c->method) {
        default:
            VARDUMP_NV2A(VAR_IN, c->param);
            break;
        }
        break;
    default:
        VARDUMP_NV2A(VAR_IN, c->method);
        VARDUMP_NV2A(VAR_IN, c->param);
        break;
    }

    LEAVE_NV2A;
}

static int
nv2a_pfifo_ramht_lookup(register void *p, register uint32_t *handle, register nv2a_pfifo_ramht *r) {
    register uint32_t bits;
    register uint32_t size;
    register uint32_t hash;
    register void *ptr;
    ENTER_NV2A;

    bits = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, RAMHT, SIZE) + 12;
    size = 1 << bits;

    for (--bits, hash = 0; *handle; hash ^= *handle & (size - 1), *handle >>= bits);
    hash  ^= NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID) << (bits - 4);
    hash <<= 3;

    if (hash >= size) {
        LEAVE_NV2A;
        return 0;
    }

    ptr  = NV2A_REGADDR(p, NV_PRAMIN, BASE);
    ptr += NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, RAMHT, BASE_ADDRESS) << 12;
    ptr += hash;

    *r = *(typeof(r))ptr;
//PRINT("ramht_lookup: handle:%u | context:0x%.08x | pgraph param:0x%.04x | engine:%u | chid:%u | valid:%u | reg:%p", r->handle,r->context,r->instance << 4,r->engine,r->chid,r->valid,ptr-NV2A_REGADDR(p, NV_PRAMIN, BASE));//XXX
    if (r->valid) *handle = (typeof(*handle))r->instance << 4;

    LEAVE_NV2A;
    return r->valid;
}

static void
nv2a_pfifo_puller(register void *p) {
    nv2a_pfifo_cache c;
    nv2a_pfifo_ramht r;
    ENTER_NV2A;

    if (!p) p = xpci[XPCI_GPU].memreg;

    if (!NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PULL0, ACCESS_ENABLED)) {
        LEAVE_NV2A;
        return;
    }

//    glo_set_current(d->pgraph.gl_context);//TODO

    while (nv2a_pfifo_cache_dequeue(&c)) {
        if (c.method == /* NV_SET_OBJECT */ 0 || (c.method >= 0x180 && c.method <= 0x1a8)) {//TODO missing 0x18c register
fprintf(stderr,"ramht_lookup: handle:%u / 0x%x | method: 0x%x\n",c.param,c.param,c.method);//XXX
            if (!nv2a_pfifo_ramht_lookup(p, &c.param, &r)) continue;
            if (r.chid != NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID)) INT3;
        }
        if (c.method == /* NV_SET_OBJECT */ 0) {
            switch (r.engine) {
            case NV_RAMHT_ENGINE_GRAPHICS:
                nv2a_pgraph_intr_context_switch(p, r.chid);
                nv2a_pgraph_fifo(p, &c);
                break;
            case NV_RAMHT_ENGINE_SW:
            case NV_RAMHT_ENGINE_DVD:
            default:
                INT3;
                break;
            }
            switch (c.subchannel) {
            case 0:
                NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PFIFO, CACHE1_ENGINE, 0, r.engine);
                break;
            case 1:
                NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PFIFO, CACHE1_ENGINE, 1, r.engine);
                break;
            case 2:
                NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PFIFO, CACHE1_ENGINE, 2, r.engine);
                break;
            case 3:
                NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PFIFO, CACHE1_ENGINE, 3, r.engine);
                break;
            case 4:
                NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PFIFO, CACHE1_ENGINE, 4, r.engine);
                break;
            case 5:
                NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PFIFO, CACHE1_ENGINE, 5, r.engine);
                break;
            case 6:
                NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PFIFO, CACHE1_ENGINE, 6, r.engine);
                break;
            case 7:
                NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PFIFO, CACHE1_ENGINE, 7, r.engine);
                break;
            }
        } else if (c.method >= 0x100) {
            switch (c.subchannel) {
            case 0:
                r.engine = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_ENGINE, 0);
                break;
            case 1:
                r.engine = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_ENGINE, 1);
                break;
            case 2:
                r.engine = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_ENGINE, 2);
                break;
            case 3:
                r.engine = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_ENGINE, 3);
                break;
            case 4:
                r.engine = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_ENGINE, 4);
                break;
            case 5:
                r.engine = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_ENGINE, 5);
                break;
            case 6:
                r.engine = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_ENGINE, 6);
                break;
            case 7:
                r.engine = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_ENGINE, 7);
                break;
            }
            switch (r.engine) {
            case NV_RAMHT_ENGINE_GRAPHICS:
                nv2a_pgraph_fifo(p, &c);
                break;
            case NV_RAMHT_ENGINE_SW:
            case NV_RAMHT_ENGINE_DVD:
            default:
                INT3;
                break;
            }
        }
    }

    LEAVE_NV2A;
}

static void
nv2a_pfifo_pusher(register void *p) {
    register uint32_t chid;
    register uint32_t *get;
    register uint32_t *put;
    nv2a_pfifo_cache *s;
    nv2a_pfifo_cache c;
    struct {
        union {
            uint32_t     flags;
            struct {
                uint32_t class  : 12;
                uint32_t pad1   : 4;
                uint32_t target : 2;
                uint32_t pad2   : 2;
                uint32_t adjust : 12;
            } PACKED;
        } PACKED;
        uint32_t         limit;
        uint32_t         frame;
    } PACKED dma;
    uint32_t addr;
    uint32_t *dcount;
    ENTER_NV2A;

    if (!NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH0, ACCESS_ENABLED) ||
        !NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_PUSH, ACCESS_ENABLED) ||
        NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_PUSH, STATUS_SUSPENDED)) {
        LEAVE_NV2A;
        return;
    }

    chid = NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID);
//PRINT("    chid:%hu", chid);//XXX

    /* TODO: PIO not supported */
    if (!(NV2A_REG32(p, NV_PFIFO, MODE) & (1 << chid))) {
        PRINT_NV2A("pb error: channel %u is not in DMA mode", chid);
        INT3;
    }
    if (!NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH1, MODE_DMA)) {
        PRINT_NV2A("pb error: PIO not supported", 0);
        INT3;
    }

    get     = NV2A_REGADDR(p, NV_PRAMIN, BASE);
//PRINT("    get:%p", get);//XXX
    get    += NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_INSTANCE, ADDRESS) << 2;
//PRINT("    get + instance:%p | instance:%p", get,((NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_INSTANCE, ADDRESS)) << 4));//XXX
    dma     = *(typeof(dma) *)get;
//PRINT("    flags:%p | limit:%p | frame:%p | class:%p | target:%p",dma.flags,dma.limit,dma.frame,dma.class,dma.target);//XXX
    addr    = NV2A_MASK_GET(dma.frame, /* NV_DMA_ADDRESS */ 0xfffff000) | (dma.adjust << 20);
//PRINT("    addr:%p", addr);//XXX
    addr   &= 0x07ffffff;
//PRINT("    addr masked:%p", addr);//XXX

//INT3;//XXX

    chid  <<= 16;
    get     = NV2A_REGADDR(p + chid, NV_USER, DMA_GET);
    put     = NV2A_REGADDR(p + chid, NV_USER, DMA_PUT);

    s       = NV2A_REGADDR(p, NV_PFIFO, CACHE1_DMA_STATE);
    dcount  = NV2A_REGADDR(p, NV_PFIFO, CACHE1_DMA_DCOUNT);

    PRINT_NV2A("pb DMA pusher:  run: "
        "limit: 0x%.08x | "
        "get: 0x%.08x < put: 0x%.08x",
        dma.limit,
        *get,
        *put);

    /* based on the convenient pseudocode in envytools */
    while (*get != *put) {
        if (*get >= dma.limit) {
            s->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_PROTECTION;
            break;
        }

        nv2a_dma_read(addr + *get, &c.param);
        *get += 4;
//PRINT("pfifo_pusher: param:0x%.08x | method:0x%.03hx | subchannel:0x%.02hx | method_count:0x%.02hx | non_inc:%hhu | dcount:%u | error:%u",c.param,s->method,s->subchannel,s->method_count,s->non_inc,*dcount,s->error);//XXX
        if (s->method_count) {
            /* data word of methods command */
            NV2A_REG32(p, NV_PFIFO, CACHE1_DMA_DATA_SHADOW) = c.param;

            c.state = s->state;
            nv2a_pfifo_cache_enqueue(&c);
            if (!s->non_inc) s->method += 4;
            --s->method_count;
            ++*dcount;
        } else {
            /* no command active - this is the first word of a new one */
            NV2A_REG32(p, NV_PFIFO, CACHE1_DMA_RSVD_SHADOW) = c.param;

            /* match all forms */
            if ((c.param & 0xe0000003) == 0x20000000) {
                /* old jump */
                NV2A_REG32(p, NV_PFIFO, CACHE1_DMA_GET_JMP_SHADOW) = *get;
                *get = NV2A_MASK_GET(c.param, NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET);
                PRINT_NV2A("pb OLD_JMP 0x%.08x", *get);
            } else if ((c.param & 3) == 1) {
                /* jump */
                NV2A_REG32(p, NV_PFIFO, CACHE1_DMA_GET_JMP_SHADOW) = *get;
                *get = NV2A_MASK_GET(c.param, NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET);
                PRINT_NV2A("pb JMP 0x%.08x", *get);
            } else if ((c.param & 3) == 2) {
                /* call */
                if (NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, STATE_ACTIVE)) {
                    s->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_CALL;
                    break;
                }
                NV2A_REG32_MASK_SET_VAL(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, RETURN_OFFSET, *get);
                NV2A_REG32_MASK_SET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, STATE_ACTIVE);
                *get = NV2A_MASK_GET(c.param, NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET);
                PRINT_NV2A("pb CALL 0x%.08x", *get);
            } else if (c.param == 0x00020000) {
                /* return */
                if (!NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, STATE_ACTIVE)) {
                    s->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_RETURN;
                    break;
                }
                *get = NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, RETURN_OFFSET);
                NV2A_REG32_MASK_UNSET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, STATE_ACTIVE);
                PRINT_NV2A("pb RET 0x%.08x", *get);
            } else if ((c.param & 0xe0030003) == 0) {
                /* increasing methods */
                s->method       = (c.param >>  0) & 0x1fff;
                s->subchannel   = (c.param >> 13) & 7;
                s->method_count = (c.param >> 18) & 0x7ff;
                s->non_inc      = 0;
                *dcount         = 0;
            } else if ((c.param & 0xe0030003) == 0x40000000) {
                /* non-increasing methods */
                s->method       = (c.param >>  0) & 0x1fff;
                s->subchannel   = (c.param >> 13) & 7;
                s->method_count = (c.param >> 18) & 0x7ff;
                s->non_inc      = 1;
                *dcount         = 0;
            } else {
                PRINT_NV2A("pb RESERVED_CMD 0x%.08x, word 0x%.08x", *get, c.param);
                s->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_RESERVED_CMD;
                break;
            }
        }
    }

    PRINT_NV2A("pb DMA pusher: done: "
        "limit: 0x%.08x | "
        "get: 0x%.08x < put: 0x%.08x",
        dma.limit,
        *get,
        *put);

    if (s->error) {
        PRINT_NV2A("pb error: 0x%.08x", s->error);

        INT3;

        NV2A_REG32_MASK_SET(p, NV_PFIFO, CACHE1_DMA_PUSH, STATUS_SUSPENDED);
        NV2A_REG32_MASK_SET(p, NV_PFIFO, INTR_0, DMA_PUSHER);
        nv2a_irq(p);
    }

    
//INT3;//XXX
    LEAVE_NV2A;

#if 0
//    uint8_t channel_id;
    ChannelControl *control;
    Cache1State *state;
    CacheEntry *command;
    uint8_t *dma;
    hwaddr limit;
    uint32_t word;

    /* TODO: How is cache1 selected? */
//    state = &d->pfifo.cache1;
//    channel_id = state->channel_id;
//    control = &d->user.channel_control[channel_id];

//    if (!state->push_enabled) return; // NV_PFIFO_CACHE1_PUSH0_ACCESS_ENABLED


    /* only handling DMA for now... */

    /* Channel running DMA */
//    uint32_t channel_modes = d->pfifo.regs[NV_PFIFO_MODE];
//    assert(channel_modes & (1 << channel_id));
//    assert(state->mode == FIFO_DMA);

//    if (!state->dma_push_enabled) return; // NV_PFIFO_CACHE1_DMA_PUSH_ACCESS_ENABLED
//    if (state->dma_push_suspended) return; // NV_PFIFO_CACHE1_DMA_PUSH_STATUS_SUSPENDED

    /* We're running so there should be no pending errors... */
//    assert(state->error == NV_PFIFO_CACHE1_DMA_STATE_ERROR_NONE);
#if 0
static DMAObject nv_dma_load(NV2AState *d, hwaddr state->dma_instance)
{
    assert(state->dma_instance < memory_region_size(&d->ramin));

    uint32_t flags = *(uint32_t *)(d->ramin_ptr + state->dma_instance + 0);
    uint32_t limit = *(uint32_t *)(d->ramin_ptr + state->dma_instance + 4);
    uint32_t frame = *(uint32_t *)(d->ramin_ptr + state->dma_instance + 8);

    return (DMAObject){
        .dma_class = GET_MASK(flags, NV_DMA_CLASS),
        .dma_target = GET_MASK(flags, NV_DMA_TARGET),
        .address = (frame & NV_DMA_ADDRESS) | GET_MASK(flags, NV_DMA_ADJUST),
        .limit = limit,
    };
}
static void *nv_dma_map(NV2AState *d, hwaddr state->dma_instance, hwaddr *len)
{
    assert(state->dma_instance < memory_region_size(&d->ramin));

    DMAObject dma = nv_dma_load(d, state->dma_instance);

    /* TODO: Handle targets and classes properly */
    NV2A_DPRINTF("dma_map %x, %x, %" HWADDR_PRIx " %" HWADDR_PRIx "\n",
                 dma.dma_class, dma.dma_target, dma.address, dma.limit);

    dma.address &= 0x07FFFFFF;

    // assert(dma.address + dma.limit < memory_region_size(d->vram));
    *len = dma.limit;
    return d->vram_ptr + dma.address;
}
#endif
//    dma = nv_dma_map(d, state->dma_instance, &limit);

//    NV2A_DPRINTF("DMA pusher: max 0x%" HWADDR_PRIx ", 0x%" HWADDR_PRIx " - 0x%" HWADDR_PRIx "\n",
//                 limit, control->dma_get, control->dma_put);

    /* based on the convenient pseudocode in envytools */
//    while (control->dma_get != control->dma_put) {
//        if (control->dma_get >= limit) {

//            state->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_PROTECTION;
//            break;
//        }

//        word = *(uint32_t *)(dma + control->dma_get);
//        control->dma_get += 4;
#if 0
        if (state->method_count) {
            /* data word of methods command */
            state->data_shadow = word;

            command = g_malloc0(sizeof(CacheEntry));
            command->method = state->method;
            command->subchannel = state->subchannel;
            command->nonincreasing = state->method_nonincreasing;
            command->parameter = word;
            qemu_mutex_lock(&state->cache_lock);
            QSIMPLEQ_INSERT_TAIL(&state->cache, command, entry);
            qemu_cond_signal(&state->cache_cond);
            qemu_mutex_unlock(&state->cache_lock);

            if (!state->method_nonincreasing) {
                state->method += 4;
            }
            state->method_count--;
            state->dcount++;
        } else {
            /* no command active - this is the first word of a new one */
            state->rsvd_shadow = word;
            /* match all forms */
            if ((word & 0xe0000003) == 0x20000000) {
                /* old jump */
                state->get_jmp_shadow = control->dma_get;
                control->dma_get = word & 0x1fffffff;
                NV2A_DPRINTF("pb OLD_JMP 0x%" HWADDR_PRIx "\n", control->dma_get);
            } else if ((word & 3) == 1) {
                /* jump */
                state->get_jmp_shadow = control->dma_get;
                control->dma_get = word & 0xfffffffc;
                NV2A_DPRINTF("pb JMP 0x%" HWADDR_PRIx "\n", control->dma_get);
            } else if ((word & 3) == 2) {
                /* call */
                if (state->subroutine_active) {
                    state->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_CALL;
                    break;
                }
                state->subroutine_return = control->dma_get;
                state->subroutine_active = true;
                control->dma_get = word & 0xfffffffc;
                NV2A_DPRINTF("pb CALL 0x%" HWADDR_PRIx "\n", control->dma_get);
            } else if (word == 0x00020000) {
                /* return */
                if (!state->subroutine_active) {
                    state->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_RETURN;
                    break;
                }
                control->dma_get = state->subroutine_return;
                state->subroutine_active = false;
                NV2A_DPRINTF("pb RET 0x%" HWADDR_PRIx "\n", control->dma_get);
            } else if ((word & 0xe0030003) == 0) {
                /* increasing methods */
                state->method = word & 0x1fff;
                state->subchannel = (word >> 13) & 7;
                state->method_count = (word >> 18) & 0x7ff;
                state->method_nonincreasing = false;
                state->dcount = 0;
            } else if ((word & 0xe0030003) == 0x40000000) {
                /* non-increasing methods */
                state->method = word & 0x1fff;
                state->subchannel = (word >> 13) & 7;
                state->method_count = (word >> 18) & 0x7ff;
                state->method_nonincreasing = true;
                state->dcount = 0;
            } else {
                NV2A_DPRINTF("pb reserved cmd 0x%" HWADDR_PRIx " - 0x%x\n",
                             control->dma_get, word);
                state->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_RESERVED_CMD;
                break;
            }
        }
    }

    NV2A_DPRINTF("DMA pusher done: max 0x%" HWADDR_PRIx ", 0x%" HWADDR_PRIx " - 0x%" HWADDR_PRIx "\n",
                 limit, control->dma_get, control->dma_put);

    if (state->error) {
        NV2A_DPRINTF("pb error: %d\n", state->error);
        assert(false);

        state->dma_push_suspended = true;

        d->pfifo.pending_interrupts |= NV_PFIFO_INTR_0_DMA_PUSHER;
        update_irq(d);
    }
#endif
#endif
}

int
nv2a_write(uint32_t addr, const void *val, size_t sz) {
    register const nv2a_block *b;
    register const char *n;
    register void *p;
    register uint32_t r;
    register uint32_t v;
    register uint32_t o;
    register int ret = 0;
    ENTER_NV2A;

    if (nv2a_base_offset(&addr)) {
        VARDUMP_NV2A(DUMP, addr);
        VARDUMP_NV2A(DUMP, ret);
        LEAVE_NV2A;
        return ret;
    }

    b = nv2a_block_lookup(addr);
    n = xpci[XPCI_GPU].name;
    p = xpci[XPCI_GPU].memreg;
    r = addr - b->offset;

    switch (sz) {
    case 1:
        v = REG8(val);
        o = REG8(p + addr);
        PRINT_NV2A("%s: "
            "write: "
            "[0x%.08x+0x%.08x] (0x%.02hhx)       <- 0x%.02hhx       | "
            "block: '%s' | "
            "reg: 0x%x",
            n,
            p,
            addr,
            o,
            v,
            b->name,
            r);
        REG8(p + addr) = v;
        ret = 1;
        break;
    case 2:
        v = REG16(val);
        o = REG16(p + addr);
        PRINT_NV2A("%s: "
            "write: "
            "[0x%.08x+0x%.08x] (0x%.04hx)     <- 0x%.04hx     | "
            "block: '%s' | "
            "reg: 0x%x",
            n,
            p,
            addr,
            o,
            v,
            b->name,
            r);
        REG16(p + addr) = v;
        ret = 1;
        break;
    case 4:
        v = REG32(val);
        o = REG32(p + addr);
        PRINT_NV2A("%s: "
            "write: "
            "[0x%.08x+0x%.08x] (0x%.08x) <- 0x%.08x | "
            "block: '%s' | "
            "reg: 0x%x",
            n,
            p,
            addr,
            o,
            v,
            b->name,
            r);
        REG32(p + addr) = v;
        ret = 1;
        break;
    case 8:
#if 0
        PRINT_NV2A("%s: "
            "write: "
            "[0x%.08x+0x%.08x] <- 0x%.016llx | "
            "block: '%s' | "
            "reg: 0x%x",
            n,
            p,
            addr,
            *(uint64_t *)val,
            b->name,
            r);
        *(uint64_t *)(p + addr) = *(uint64_t *)val;
        ret = 1;
#else
        INT3;
#endif
        break;
    default:
        VARDUMP_NV2A(DUMP, ret);
        LEAVE_NV2A;
        return ret;
    }

    switch (b->index) {
    case NV_PMC:
        switch (r) {
        case NV_PMC_INTR_0:
            NV2A_REG32(p, NV_PMC, INTR_0) = o & ~v;
        case NV_PMC_INTR_EN_0:
            nv2a_irq(p);
            break;
        }
        break;
//    case NV_PBUS:
//        switch (r) {
//        }
//        break;
    case NV_PFIFO:
        switch (r) {
        case NV_PFIFO_INTR_0:
            NV2A_REG32(p, NV_PFIFO, INTR_0) = o & ~v;
        case NV_PFIFO_INTR_EN_0:
            nv2a_irq(p);
            break;
        case NV_PFIFO_CACHE1_DMA_PUSH:
            if (NV2A_MASK_GET(o, NV_PFIFO_CACHE1_DMA_PUSH_STATUS_SUSPENDED) &&
                !NV2A_MASK_GET(v, NV_PFIFO_CACHE1_DMA_PUSH_STATUS_SUSPENDED)) {
                nv2a_pfifo_pusher(p);
            }
            break;
        case NV_PFIFO_CACHE1_DMA_PUT:
            {
                register uint32_t chid;

                chid = NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID) << 16;
                NV2A_REG32_MASK_SET_VAL(p + chid, NV_USER, DMA_PUT, OFFSET, v);
            }
            break;
        case NV_PFIFO_CACHE1_DMA_GET:
            {
                register uint32_t chid;

                chid = NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID) << 16;
                NV2A_REG32_MASK_SET_VAL(p + chid, NV_USER, DMA_GET, OFFSET, v);
            }
            break;
        case NV_PFIFO_CACHE1_PULL0:
            if (!NV2A_MASK_GET(o, NV_PFIFO_CACHE1_PULL0_ACCESS_ENABLED) &&
                NV2A_MASK_GET(v, NV_PFIFO_CACHE1_PULL0_ACCESS_ENABLED)) {
                EVENT_SIGNAL;
            }
            break;
        }
        break;
//    case NV_PFIFO_CACHE:
//        switch (r) {
//        }
//        break;
//    case NV_PRMA: /* not implemented */
//        switch (r) {
//        }
//        break;
//    case NV_PVIDEO:
//        switch (r) {
//        }
//        break;
    case NV_PTIMER:
        switch (r) {
        case NV_PTIMER_INTR_0:
            NV2A_REG32(p, NV_PTIMER, INTR_0) = o & ~v;
        case NV_PTIMER_INTR_EN_0:
            nv2a_irq(p);
            break;
        }
        break;
//    case NV_PCOUNTER: /* not implemented */
//        switch (r) {
//        }
//        break;
//    case NV_PVPE: /* not implemented */
//        switch (r) {
//        }
//        break;
//    case NV_PTV: /* not implemented */
//        switch (r) {
//        }
//        break;
//    case NV_PRMFB: /* not implemented */
//        switch (r) {
//        }
//        break;
//    case NV_PRMVIO:
//        switch (r) {
//        }
//        break;
//    case NV_PFB:
//        switch (r) {
//        }
//        break;
//    case NV_PSTRAPS: /* not implemented */
//        switch (r) {
//        }
//        break;
    case NV_PGRAPH:
        switch (r) {
        case NV_PGRAPH_INTR:
            NV2A_REG32(p, NV_PGRAPH, INTR) = o & ~v;
//            qemu_cond_broadcast(&d->pgraph.interrupt_cond);
            break;
        case NV_PGRAPH_FIFO:
            if (!NV2A_MASK_GET(o, NV_PGRAPH_FIFO_ACCESS_ENABLED) &&
                NV2A_MASK_GET(v, NV_PGRAPH_FIFO_ACCESS_ENABLED)) {
                PGRAPH_FIFO_SIGNAL;
            }
            break;
        }
        break;
    case NV_PCRTC:
        switch (r) {
        case NV_PCRTC_INTR_0:
            NV2A_REG32(p, NV_PCRTC, INTR_0) = o & ~v;
        case NV_PCRTC_INTR_EN_0:
            nv2a_irq(p);
            break;
        }
        break;
//    case NV_PRMCIO:
//        switch (r) {
//        }
//        break;
//    case NV_PRAMDAC:
//        switch (r) {
//        }
//        break;
//    case NV_PRMDIO: /* not implemented */
//        switch (r) {
//        }
//        break;
//    case NV_PRAMIN:
//        switch (r) {
//        }
//        break;
    case NV_USER:
        {
            uint32_t modes = NV2A_REG32(p, NV_PFIFO, MODE);
            uint32_t chid = NV_USER_ADDR_CHID(r);
PRINT("nv_user: modes:%u", modes);
PRINT("nv_user: chid:%hu", chid);
            if (modes & (1 << chid)) { /* dma */
                switch (NV_USER_ADDR_REG(r)) {
                case NV_USER_DMA_PUT:
//                    if (NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH0, ACCESS_ENABLED)) {
                    nv2a_pfifo_pusher(p);
//                    }
                    break;
                }
            } else { /* pio */
                INT3;
            }
        }
        break;
    }

    VARDUMP_NV2A(DUMP, ret);
    LEAVE_NV2A;
    return ret;
}

int
nv2a_read(uint32_t addr, void *val, size_t sz) {
    register const nv2a_block *b;
    register const char *n;
    register void *p;
    register uint32_t r;
    register uint32_t v;
    register int ret = 0;
    ENTER_NV2A;

    if (nv2a_base_offset(&addr)) {
        VARDUMP_NV2A(DUMP, addr);
        VARDUMP_NV2A(DUMP, ret);
        LEAVE_NV2A;
        return ret;
    }

    b = nv2a_block_lookup(addr);
    n = xpci[XPCI_GPU].name;
    p = xpci[XPCI_GPU].memreg;
    r = addr - b->offset;
    v = REG32(p + addr);

    switch (b->index) {
    case NV_PMC:
        switch (r) {
        case NV_PMC_BOOT_0:
            v = 0x02a000a2; /* NV2A, A02, Rev 0 */
            break;
        }
        break;
//    case NV_PBUS:
//        switch (r) {
//        }
//        break;
    case NV_PFIFO:
        switch (r) {
        case NV_PFIFO_RUNOUT_STATUS:
            v = NV_PFIFO_RUNOUT_STATUS_LOW_MARK_EMPTY; /* low mark empty */
            break;
        case NV_PFIFO_CACHE1_STATUS:
#if 0
            qemu_mutex_lock(&d->pfifo.cache1.cache_lock);
            if (QSIMPLEQ_EMPTY(&d->pfifo.cache1.cache)) {
                r |= NV_PFIFO_CACHE1_STATUS_LOW_MARK; /* low mark empty */
            }
            qemu_mutex_unlock(&d->pfifo.cache1.cache_lock);
#endif
            //TODO
            NV2A_MASK_SET(v, NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY); /* low mark empty */
            break;
        case NV_PFIFO_CACHE1_DMA_PUSH:
            NV2A_MASK_SET(v, NV_PFIFO_CACHE1_DMA_PUSH_BUFFER_EMPTY); /* push buffer empty */
            break;
        case NV_PFIFO_CACHE1_DMA_PUT:
            {
                register uint32_t chid;

                chid = NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID) << 16;
                v    = NV2A_REG32_MASK_GET(p + chid, NV_USER, DMA_PUT, OFFSET);
            }
            break;
        case NV_PFIFO_CACHE1_DMA_GET:
            {
                register uint32_t chid;

                chid = NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID) << 16;
                v    = NV2A_REG32_MASK_GET(p + chid, NV_USER, DMA_GET, OFFSET);
            }
            break;
        }
        break;
//    case NV_PFIFO_CACHE:
//        switch (r) {
//        }
//        break;
//    case NV_PRMA: /* not implemented */
//        switch (r) {
//        }
//        break;
//    case NV_PVIDEO:
//        switch (r) {
//        }
//        break;
//    case NV_PTIMER:
//        switch (r) {
//        }
//        break;
//    case NV_PCOUNTER: /* not implemented */
//        switch (r) {
//        }
//        break;
//    case NV_PVPE: /* not implemented */
//        switch (r) {
//        }
//        break;
//    case NV_PTV: /* not implemented */
//        switch (r) {
//        }
//        break;
//    case NV_PRMFB: /* not implemented */
//        switch (r) {
//        }
//        break;
//    case NV_PRMVIO:
//        switch (r) {
//        }
//        break;
    case NV_PFB:
        switch (r) {
        case NV_PFB_CFG0:
            v = NV_PFB_CFG0_PART_4; /* 3-4 memory partitions; debug bios checks this */
            break;
        case NV_PFB_CSTATUS:
            v = 0x08000000; /* vram size */
            break;
        case NV_PFB_WBC:
            v = 0; /* flush not pending */
            break;
        }
        break;
//    case NV_PSTRAPS: /* not implemented */
//        switch (r) {
//        }
//        break;
//    case NV_PGRAPH:
//        switch (r) {
//        }
//        break;
//    case NV_PCRTC:
//        switch (r) {
//        }
//        break;
//    case NV_PRMCIO:
//        switch (r) {
//        }
//        break;
//    case NV_PRAMDAC:
//        switch (r) {
//        }
//        break;
//    case NV_PRMDIO: /* not implemented */
//        switch (r) {
//        }
//        break;
//    case NV_PRAMIN:
//        switch (r) {
//        }
//        break;
//    case NV_USER:
//        switch (r) {
//        }
//        break;
    }

    switch (sz) {
    case 1:
        v &= 0xff;
        PRINT_NV2A("%s: "
            " read: "
            "[0x%.08x+0x%.08x]              -> 0x%.02hhx       | "
            "block: '%s' | "
            "reg: 0x%x",
            n,
            p,
            addr,
            v,
            b->name,
            r);
        REG8(val) = v;
        ret = 1;
        break;
    case 2:
        v &= 0xffff;
        PRINT_NV2A("%s: "
            " read: "
            "[0x%.08x+0x%.08x]              -> 0x%.04hx     | "
            "block: '%s' | "
            "reg: 0x%x",
            n,
            p,
            addr,
            v,
            b->name,
            r);
        REG16(val) = v;
        ret = 1;
        break;
    case 4:
        PRINT_NV2A("%s: "
            " read: "
            "[0x%.08x+0x%.08x]              -> 0x%.08x | "
            "block: '%s' | "
            "reg: 0x%x",
            n,
            p,
            addr,
            v,
            b->name,
            r);
        REG32(val) = v;
        ret = 1;
        break;
    case 8:
#if 0
        PRINT_NV2A("%s: "
            " read: "
            "[0x%.08x+0x%.08x] -> 0x%.016llx | "
            "block: '%s' | "
            "reg: 0x%x",
            n,
            p,
            addr,
            *(uint64_t *)val,
            b->name,
            r);
        *(uint64_t *)val = *(uint64_t *)(p + addr);
        ret = 1;
#else
        INT3;
#endif
        break;
    default:
        break;
    }

    VARDUMP_NV2A(DUMP, ret);
    LEAVE_NV2A;
    return ret;
}

static void
nv2a_destroy(void) {
    ENTER_NV2A;

    if (nv2a_cacheq) {
        //TODO
    }

    LEAVE_NV2A;
}

static void
nv2a_init(void) {
    ENTER_NV2A;

    nv2a_destroy();

    LEAVE_NV2A;
}








