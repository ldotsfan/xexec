/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  Copyright (C) 2012  espes
 *  Copyright (C) 2015  Jannik Vogel
 *  Copyright (C) 2012-2018  Michael Saga. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

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

typedef struct {
    union {
        uint32_t            flags;
        struct {
            uint32_t        class  : 12;
            uint32_t        pad1   : 4;
            uint32_t        target : 2;
            uint32_t        pad2   : 2;
            uint32_t        adjust : 12;
        } PACKED;
    } PACKED;
    uint32_t                limit;
    uint32_t                frame;
} PACKED nv2a_dma;

#define NV2A_DMA_ADDRESS(x) ((((x)->frame & /* NV_DMA_ADDRESS */ 0xfffff000) | ((x)->adjust << 20)) & (host->memreg_size - 1))

typedef struct {
    int                     dirty;
    size_t                  ref;
    GLenum                  gl_target;
    GLuint                  gl_texture;
} nv2a_pgraph_texture;

typedef struct {
    glo_context *           glo;

    GLuint                  gl_framebuffer;

    GLuint                  gl_color_buffer;
    GLuint                  gl_zeta_buffer;

    GLuint                  gl_converted_buffer[NV2A_MAX_VERTEX_ATTRIBS];
    GLuint                  gl_inline_buffer[NV2A_MAX_VERTEX_ATTRIBS];
    GLuint                  gl_inline_array_buffer;
    GLuint                  gl_element_buffer;
//    GLuint                  gl_memory_buffer;

    GLuint                  gl_vertex_array;

    nv2a_pgraph_texture     texture[NV2A_MAX_TEXTURES];
} nv2a_context;

static nv2a_context *       nv2a_ctx = NULL;

typedef struct {
    union {
        uint32_t            state;
        struct {
            uint32_t        method       : 13;      /*  0 */
            uint32_t        subchannel   : 3;       /* 13 */
            uint32_t        non_inc      : 2;       /* 16 */
            uint32_t        method_count : 11;      /* 18 */
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
#define PGRAPH_FIFO_SIGNAL  PGRAPH_FIFO_LOCK, \
                            pthread_cond_broadcast(&nv2a_pgraph_fifo_cond), \
                            PGRAPH_FIFO_UNLOCK
static pthread_mutex_t      nv2a_pgraph_intr_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t       nv2a_pgraph_intr_cond = PTHREAD_COND_INITIALIZER;
#define PGRAPH_INTR_LOCK    pthread_mutex_lock(&nv2a_pgraph_intr_mutex)
#define PGRAPH_INTR_UNLOCK  pthread_mutex_unlock(&nv2a_pgraph_intr_mutex)
#define PGRAPH_INTR_WAIT    pthread_cond_wait(&nv2a_pgraph_intr_cond, &nv2a_pgraph_intr_mutex)
#define PGRAPH_INTR_SIGNAL  PGRAPH_INTR_LOCK, \
                            pthread_cond_broadcast(&nv2a_pgraph_intr_cond), \
                            PGRAPH_INTR_UNLOCK

static pthread_mutex_t      nv2a_flip_stall_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t       nv2a_flip_stall_cond = PTHREAD_COND_INITIALIZER;
#define FLIP_STALL_LOCK     pthread_mutex_lock(&nv2a_flip_stall_mutex)
#define FLIP_STALL_UNLOCK   pthread_mutex_unlock(&nv2a_flip_stall_mutex)
#define FLIP_STALL_WAIT     pthread_cond_wait(&nv2a_flip_stall_cond, &nv2a_flip_stall_mutex)
#define FLIP_STALL_SIGNAL   FLIP_STALL_LOCK, \
                            pthread_cond_broadcast(&nv2a_flip_stall_cond), \
                            FLIP_STALL_UNLOCK

static void
nv2a_pfifo_cache_enqueue(register const nv2a_pfifo_cache *c, register uint32_t param) {
    register nv2a_pfifo_cache **q;
    register size_t i;
    ENTER_NV2A;

    CACHEQ_LOCK;

    for (q = &nv2a_cacheq, i = 1; *q; q = (typeof(q))&(**q).next, ++i);

    *q = calloc(1, sizeof(*c));

    (**q).state = c->state;
    (**q).param = param;

//    PRINT_NV2A("%s: CACHEQ: %u command%s in queue", nv2a->name, i, (i != 1) ? "s" : "");

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
        PRINT_NV2A("%s: CACHEQ: no commands in queue", nv2a->name);
    }

    CACHEQ_UNLOCK;

    LEAVE_NV2A;
    return ret;
}

static int
nv2a_offset(register uint32_t *addr) {
    if ((*addr & 0xff000000) == nv2a->memreg_base) {
        *addr &= 0x00ffffff;
        return 0;
    }
    return 1;
}

static const nv2a_block *
nv2a_block_lookup(register uint32_t addr, register const char **reg) {
    register const nv2a_block *b;
    register size_t i;
    ENTER_NV2A;

    if (reg) *reg = NULL;

    for (i = 0; i <= ARRAY_SIZE(nv2a_blocks); ++i) {
        if (i >= ARRAY_SIZE(nv2a_blocks)) INT3;
        b = &nv2a_blocks[i];
        if (addr >= b->offset && addr < b->offset + b->size) {
            if (reg) {
                i = (addr - b->offset) / 4;
                switch (b->index) {
                case NV_PMC:
                    ARRAY(*reg, nv2a_pmc_name, i);
                    break;
                case NV_PGRAPH:
                    ARRAY(*reg, nv2a_pgraph_name, i);
                    break;
                }
            }
            break;
        }
    }

    LEAVE_NV2A;
    return b;
}

#define NV2A_IRQ_PGRAPH (1 << 1)
#define NV2A_IRQ_FIFO   (1 << 2)

#define NV2A_IRQ_WAIT(x,y,z) \
        x ## _ ## y ## _LOCK; \
        while (nv2a->irq_busy || NV2A_REG32_MASK_GET(nv2a->memreg, NV_ ## x, y, z)) { \
            if (nv2a->irq_busy) nv2a->irq_busy |= NV2A_IRQ_ ## x; \
            x ## _ ## y ## _WAIT; \
        } \
        x ## _ ## y ## _UNLOCK
#define NV2A_IRQ_WAITN(x,y,z) \
        x ## _ ## y ## _LOCK; \
        while (nv2a->irq_busy || !NV2A_REG32_MASK_GET(nv2a->memreg, NV_ ## x, y, z)) { \
            if (nv2a->irq_busy) nv2a->irq_busy |= NV2A_IRQ_ ## x; \
            x ## _ ## y ## _WAIT; \
        } \
        x ## _ ## y ## _UNLOCK

void
nv2a_irq_restore(register int mask) {
    if (!mask) return;
    if (mask & NV2A_IRQ_PGRAPH) PGRAPH_INTR_SIGNAL;
    if (mask & NV2A_IRQ_FIFO) PGRAPH_FIFO_SIGNAL;
}

static void
nv2a_irq_raise(register int mask) {
    if (!mask) return;
    if (nv2a->irq_busy) {
        if (mask & NV2A_IRQ_PGRAPH) nv2a->irq_busy |= NV2A_IRQ_PGRAPH;
        if (mask & NV2A_IRQ_FIFO) nv2a->irq_busy |= NV2A_IRQ_FIFO;
    } else {
        nv2a_irq_restore(mask);
    }
}

int
nv2a_irq(void) {
    register const char *n;
    register void *p;
    register uint32_t enabled;
    register uint32_t pending;
    register int ret = 0;
    ENTER_NV2A;

    p = nv2a->memreg;

    if (!p ||
        !nv2a->irq_kinterrupt ||
        !nv2a->irq_connected ||
        !nv2a->irq_isr_routine) {
        LEAVE_NV2A;
        return ret;
    }

    n = nv2a->name;

    /* PFIFO */
    enabled = NV2A_REG32(p, NV_PFIFO, INTR_EN_0);
    pending = NV2A_REG32(p, NV_PFIFO, INTR_0);
    PRINT_NV2A("%s: IRQ: PFIFO: enabled 0x%.08x, pending 0x%.08x", n, enabled, pending);
    VARDUMP3_NV2A(DUMP, enabled, nv2a_pfifo_intr_name);
    VARDUMP3_NV2A(DUMP, pending, nv2a_pfifo_intr_name);
    if (pending & enabled) {
        PRINT_NV2A("%s: IRQ: PFIFO set", n);
        NV2A_REG32_MASK_SET(p, NV_PMC, INTR_0, PFIFO);
    } else {
        PRINT_NV2A("%s: IRQ: PFIFO unset", n);
        NV2A_REG32_MASK_UNSET(p, NV_PMC, INTR_0, PFIFO);
    }
    /* PCRTC */
    enabled = NV2A_REG32(p, NV_PCRTC, INTR_EN_0);
    pending = NV2A_REG32(p, NV_PCRTC, INTR_0);
    PRINT_NV2A("%s: IRQ: PCRTC: enabled 0x%.08x, pending 0x%.08x", n, enabled, pending);
    VARDUMP3_NV2A(DUMP, enabled, nv2a_pcrtc_intr_name);
    VARDUMP3_NV2A(DUMP, pending, nv2a_pcrtc_intr_name);
    if (pending & enabled) {
        PRINT_NV2A("%s: IRQ: PCRTC set", n);
        NV2A_REG32_MASK_SET(p, NV_PMC, INTR_0, PCRTC);
    } else {
        PRINT_NV2A("%s: IRQ: PCRTC unset", n);
        NV2A_REG32_MASK_UNSET(p, NV_PMC, INTR_0, PCRTC);
    }
    /* PGRAPH */
    enabled = NV2A_REG32(p, NV_PGRAPH, INTR_EN);
    pending = NV2A_REG32(p, NV_PGRAPH, INTR);
    PRINT_NV2A("%s: IRQ: PGRAPH: enabled 0x%.08x, pending 0x%.08x", n, enabled, pending);
    VARDUMP3_NV2A(DUMP, enabled, nv2a_pgraph_intr_name);
    VARDUMP3_NV2A(DUMP, pending, nv2a_pgraph_intr_name);
    if (pending & enabled) {
        PRINT_NV2A("%s: IRQ: PGRAPH set", n);
        NV2A_REG32_MASK_SET(p, NV_PMC, INTR_0, PGRAPH);
    } else {
        PRINT_NV2A("%s: IRQ: PGRAPH unset", n);
        NV2A_REG32_MASK_UNSET(p, NV_PMC, INTR_0, PGRAPH);
    }

    if (NV2A_REG32(p, NV_PMC, INTR_0) &&
        NV2A_REG32(p, NV_PMC, INTR_EN_0)) {
        PRINT_NV2A("%s: IRQ: raised!", n);
        nv2a->irq_level = 1;
        ret = 1;
    }

    LEAVE_NV2A;
    return ret;
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

    IRQ_NV2A_SIGNAL;

    NV2A_IRQ_WAIT(PGRAPH, INTR, CONTEXT_SWITCH);

    LEAVE_NV2A;
}

static void
nv2a_pgraph_texture_bind(register void *p, register size_t i) {
    ENTER_NV2A;

    if (i >= NV2A_MAX_TEXTURES) INT3;
#if 0
    uint32_t ctl_0        = pg->regs[NV_PGRAPH_TEXCTL0_0 + i*4];
    uint32_t ctl_1        = pg->regs[NV_PGRAPH_TEXCTL1_0 + i*4];
    uint32_t fmt          = pg->regs[NV_PGRAPH_TEXFMT0 + i*4];
    uint32_t filter       = pg->regs[NV_PGRAPH_TEXFILTER0 + i*4];
    uint32_t address      = pg->regs[NV_PGRAPH_TEXADDRESS0 + i*4];
    uint32_t palette      = pg->regs[NV_PGRAPH_TEXPALETTE0 + i*4];

    uint32_t rect_width   = GET_MASK(pg->regs[NV_PGRAPH_TEXIMAGERECT0 + i*4], NV_PGRAPH_TEXIMAGERECT0_WIDTH);
    uint32_t rect_height  = GET_MASK(pg->regs[NV_PGRAPH_TEXIMAGERECT0 + i*4], NV_PGRAPH_TEXIMAGERECT0_HEIGHT);

    uint32_t border_color = pg->regs[NV_PGRAPH_BORDERCOLOR0 + i*4];
    uint32_t offset       = pg->regs[NV_PGRAPH_TEXOFFSET0 + i*4];
#endif
    register size_t pos = i * 64;
    register uint32_t addr;
    register uint32_t offset = NV2A_REG32(p + pos, NV_097, SET_TEXTURE_OFFSET);      //0x00001b00,
    union {
        uint32_t     field;
        struct {
            uint32_t context_dma         : 2;   /*  0 */
            uint32_t cubemap_enable      : 1;   /*  2 */
            uint32_t border_source       : 1;   /*  3 */
            uint32_t dimensionality      : 4;   /*  4 */
            uint32_t color               : 8;   /*  8 */
            uint32_t mipmap_levels       : 4;   /* 16 */
            uint32_t base_size_u         : 4;   /* 20 */
            uint32_t base_size_v         : 4;   /* 24 */
            uint32_t base_size_p         : 4;   /* 28 */
        } PACKED;
    } PACKED format     = { .field = NV2A_REG32(p + pos, NV_097, SET_TEXTURE_FORMAT) };//0x00001b04
    uint32_t address    = NV2A_REG32(p + pos, NV_097, SET_TEXTURE_ADDRESS);     //0x00001b08,
    union {
        uint32_t     field;
        struct {
            uint32_t color_key_operation : 2;   /*  0 */
            uint32_t alpha_kill_enable   : 1;   /*  2 */
            uint32_t image_field_enable  : 1;   /*  3 */
            uint32_t log_max_aniso       : 2;   /*  4 */
            uint32_t max_lod_clamp       : 12;  /*  6 */
            uint32_t min_lod_clamp       : 12;  /* 18 */
            uint32_t enable              : 2;   /* 30 */
        } PACKED;
    } PACKED ctl0       = { .field = NV2A_REG32(p + pos, NV_097, SET_TEXTURE_CONTROL0) };//0x00001b0c
    uint32_t ctl1       = NV2A_REG32(p + pos, NV_097, SET_TEXTURE_CONTROL1);    //0x00001b10,
    union {
        uint32_t     field;
        struct {
            uint32_t mipmap_lod_bias     : 13;  /*  0 */
            uint32_t convolution_kernel  : 3;   /* 13 */
            uint32_t min                 : 8;   /* 16 */
            uint32_t mag                 : 4;   /* 24 */
            uint32_t asigned_enabled     : 1;   /* 28 */
            uint32_t rsigned_enabled     : 1;   /* 29 */
            uint32_t gsigned_enabled     : 1;   /* 30 */
            uint32_t bsigned_enabled     : 1;   /* 31 */
        } PACKED;
    } PACKED filter     = { .field = NV2A_REG32(p + pos, NV_097, SET_TEXTURE_FILTER) };//0x00001b14
    uint32_t width      = NV2A_REG32_MASK_BITSHIFT_GET(p + pos, NV_097, SET_TEXTURE_IMAGE_RECT, WIDTH);//0x00001b1c
    uint32_t height     = NV2A_REG32_MASK_BITSHIFT_GET(p + pos, NV_097, SET_TEXTURE_IMAGE_RECT, HEIGHT);//0x00001b1c
    uint32_t depth;
    union {
        uint32_t     field;
        struct {
            uint32_t context_dma         : 2;   /*  0 */
            uint32_t length              : 4;   /*  2 */
            uint32_t offset              : 26;  /*  6 */
        } PACKED;
    } PACKED palette    = { .field = NV2A_REG32(p + pos, NV_097, SET_TEXTURE_PALETTE) };//0x00001b20
    uint32_t bcolor     = NV2A_REG32(p + pos, NV_097, SET_TEXTURE_BORDER_COLOR);//0x00001b24,
    uint32_t plength;
    uint32_t mlevel;
    uint32_t bsizeu;
    uint32_t bsizev;
    uint32_t bsizep;
    uint32_t maxlod;
    nv2a_pgraph_texture *t;
    const nv2a_pgraph_color *c;
    nv2a_dma dma;
    void *tdata;
    void *pdata;

    if (filter.asigned_enabled ||
        filter.rsigned_enabled ||
        filter.gsigned_enabled ||
        filter.bsigned_enabled) INT3;

    switch (palette.length) {
    case NV_097_SET_TEXTURE_PALETTE_LENGTH_256 /* 0 */:
        plength = 256;
        break;
    case NV_097_SET_TEXTURE_PALETTE_LENGTH_128 /* 1 */:
        plength = 128;
        break;
    case NV_097_SET_TEXTURE_PALETTE_LENGTH_64  /* 2 */:
        plength = 64;
        break;
    case NV_097_SET_TEXTURE_PALETTE_LENGTH_32  /* 3 */:
        plength = 32;
        break;
    default:
        INT3;
        break;
    }

    do {
        glActiveTexture(GL_TEXTURE0 + i);
        if (!ctl0.enable) {
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            glBindTexture(GL_TEXTURE_RECTANGLE, 0);
            glBindTexture(GL_TEXTURE_1D, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindTexture(GL_TEXTURE_3D, 0);
            break;
        }
        t = &nv2a_ctx->texture[i];
        if (!t->dirty && t->ref) {
            glBindTexture(t->gl_target, t->gl_texture);
            break;
        }
        if (format.color >= ARRAY_SIZE(nv2a_colors)) INT3;
        c = &nv2a_colors[format.color];
        if (!c->name) INT3;
        if (c->linear) {
            if (format.dimensionality != 2) INT3;
            depth  = 1;
        } else {
            bsizeu = format.base_size_u;
            bsizev = format.base_size_v;
            bsizep = format.base_size_p;
            width  = 1 << bsizeu;
            height = 1 << bsizev;
            depth  = 1 << bsizep;
            mlevel = format.mipmap_levels;
            maxlod = ctl0.max_lod_clamp;
            mlevel = MIN(mlevel, maxlod + 1);//FIXME: What about 3D mipmaps?
            if (c->gl_format) {
                /* Discard mipmap levels that would be smaller than 1x1.
                 * FIXME: Is this actually needed?
                 *
                 * >> Level 0: 32 x 4
                 *    Level 1: 16 x 2
                 *    Level 2: 8 x 1
                 *    Level 3: 4 x 1
                 *    Level 4: 2 x 1
                 *    Level 5: 1 x 1
                 */
                mlevel = MIN(mlevel, MAX(bsizeu, bsizev) + 1);
            } else {
                /* OpenGL requires DXT textures to always have a width and
                 * height a multiple of 4. The Xbox and DirectX handles DXT
                 * textures smaller than 4 by padding the reset of the block.
                 *
                 * See:
                 * https://msdn.microsoft.com/en-us/library/windows/desktop/bb204843(v=vs.85).aspx
                 * https://msdn.microsoft.com/en-us/library/windows/desktop/bb694531%28v=vs.85%29.aspx#Virtual_Size
                 *
                 * Work around this for now by discarding mipmap levels that
                 * would result in too-small textures. A correct solution
                 * will be to decompress these levels manually, or add texture
                 * sampling logic.
                 *
                 * >> Level 0: 64 x 8
                 *    Level 1: 32 x 4
                 *    Level 2: 16 x 2 << Ignored
                 * >> Level 0: 16 x 16
                 *    Level 1: 8 x 8
                 *    Level 2: 4 x 4 << OK!
                 */
                if (bsizeu < 2 || bsizev < 2) {
                    /* Base level is smaller than 4x4... */
                    mlevel = 1;
                } else {
                    mlevel = MIN(mlevel, MIN(bsizeu, bsizev) - 1);
                }
            }
            if (!mlevel) INT3;
        }
        pdata = tdata = NV2A_REGADDR(p, NV_PRAMIN, BASE);
        switch (format.context_dma) {
        case NV_097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A /* 0 */:
            tdata += NV2A_REG32(p, NV_097, SET_CONTEXT_DMA_A);
            break;
        case NV_097_SET_TEXTURE_FORMAT_CONTEXT_DMA_B /* 1 */:
            tdata += NV2A_REG32(p, NV_097, SET_CONTEXT_DMA_B);
            break;
        default:
            INT3;
            break;
        }
        dma    = *(typeof(dma) *)tdata;
        addr   = NV2A_DMA_ADDRESS(&dma);
        if (addr + offset >= dma.limit) INT3;
        tdata  = (typeof(tdata))(addr + offset);
        switch (palette.context_dma) {
        case NV_097_SET_TEXTURE_PALETTE_CONTEXT_DMA_A /* 0 */:
            pdata += NV2A_REG32(p, NV_097, SET_CONTEXT_DMA_A);
            break;
        case NV_097_SET_TEXTURE_PALETTE_CONTEXT_DMA_B /* 1 */:
            pdata += NV2A_REG32(p, NV_097, SET_CONTEXT_DMA_B);
            break;
        default:
            INT3;
            break;
        }
        dma    = *(typeof(dma) *)pdata;
        addr   = NV2A_DMA_ADDRESS(&dma);
        offset = palette.offset << 6;
        if (addr + offset >= dma.limit) INT3;
        pdata  = (typeof(pdata))(addr + offset);
//TODO
    } while (0);

    LEAVE_NV2A;
}

static void
nv2a_pgraph_texture_bind_all(register void *p) {
    register size_t i;
    ENTER_NV2A;

    for (i = 0; i < NV2A_MAX_TEXTURES; nv2a_pgraph_texture_bind(p, i), ++i);

    LEAVE_NV2A;
}

static void
nv2a_pgraph_fifo(register void *p, register const nv2a_pfifo_cache *c) {
    register size_t pos;
    register uint32_t *get;
    register uint32_t v;
    register uint32_t t;
    nv2a_pgraph_ctx1 *switch1, *cache1;
    nv2a_pgraph_ctx2 *switch2, *cache2;
    nv2a_pgraph_ctx3 *switch3, *cache3;
    nv2a_pgraph_ctx4 *switch4, *cache4;
    nv2a_pgraph_ctx5 *switch5, *cache5;
    nv2a_dma dma;
    ENTER_NV2A;

    NV2A_IRQ_WAITN(PGRAPH, FIFO, ACCESS_ENABLED);

    VARDUMP_NV2A(VAR_IN, c->subchannel);

    switch1 = NV2A_REGADDR(p, NV_PGRAPH, CTX_SWITCH1);
    switch2 = NV2A_REGADDR(p, NV_PGRAPH, CTX_SWITCH2);
    switch3 = NV2A_REGADDR(p, NV_PGRAPH, CTX_SWITCH3);
    switch4 = NV2A_REGADDR(p, NV_PGRAPH, CTX_SWITCH4);
    switch5 = NV2A_REGADDR(p, NV_PGRAPH, CTX_SWITCH5);
    pos     = NV_PGRAPH_CTX_CACHE__OFFSET(c->subchannel);
    cache1  = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE1);
    cache2  = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE2);
    cache3  = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE3);
    cache4  = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE4);
    cache5  = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE5);

//fprintf(stderr,"pgraph: method:0x%x | subchannel:%u | param:0x%x | grclass:0x%x\n",c->method,c->subchannel,c->param,cache1->grclass);//XXX

    if (c->method == /* NV_SET_OBJECT */ 0) {
        VARDUMP_NV2A(VAR_IN, c->method);
        VARDUMP_NV2A(VAR_IN, c->param);
        pos = c->param;
        if (pos + 20 > NV2A_BLOCK_SIZE(NV_PRAMIN) || pos % 4) INT3;
        get = NV2A_REGADDR(p + pos, NV_PRAMIN, BASE);
        cache1->field = *get++;
        cache2->field = *get++;
        cache3->field = *get++;
        cache4->field = *get++;
        cache5->field = *get++;
        *switch1      = *cache1;
        *switch2      = *cache2;
        *switch3      = *cache3;
        *switch4      = *cache4;
        *switch5      = *cache5;
        VARDUMP3_NV2A(DUMP, cache1->field, nv2a_pgraph_ctx1_name);
        VARDUMP2_NV2A(DUMP, cache1->grclass, nv2a_pgraph_ctx1_grclass_name);
        VARDUMP2_NV2A(DUMP, cache1->patch_config, nv2a_pgraph_ctx1_patch_config_name);
        VARDUMP2_NV2A(DUMP, cache1->dither_mode, nv2a_pgraph_ctx1_dither_mode_name);
        VARDUMP2_NV2A(DUMP, cache1->class_type, nv2a_pgraph_ctx1_class_type_name);
        VARDUMP_NV2A(DUMP,  cache2->field);
        VARDUMP2_NV2A(DUMP, cache2->mono_format, nv2a_pgraph_ctx2_mono_format_name);
        VARDUMP2_NV2A(DUMP, cache2->color_format, nv2a_pgraph_ctx2_color_format_name);
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

    VARDUMP2_NV2A(DUMP, cache1->grclass, nv2a_pgraph_ctx1_grclass_name);

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
        case NV_097_WAIT_FOR_IDLE:
        case NV_097_FLIP_STALL:
        case NV_097_SET_CONTEXT_DMA_COLOR:
//            /* try to get any straggling draws in before the surface's changed :/ */
        case NV_097_SET_SURFACE_CLIP_HORIZONTAL:
        case NV_097_SET_SURFACE_CLIP_VERTICAL:
        case NV_097_SET_SURFACE_FORMAT:
        case NV_097_SET_SURFACE_PITCH:
        case NV_097_SET_SURFACE_COLOR_OFFSET:
        case NV_097_SET_SURFACE_ZETA_OFFSET:
        case NV_097_SET_CONTROL0:
        case NV_097_BACK_END_WRITE_SEMAPHORE_RELEASE:
//            pgraph_update_surface(d, false, true, true);//TODO
            break;
        case NV_097_SET_COLOR_MASK:
//        pg->surface_color.write_enabled_cache |= pgraph_color_write_enabled(pg);//TODO
            break;
        case NV_097_SET_DEPTH_MASK:
//        pg->surface_zeta.write_enabled_cache |= pgraph_zeta_write_enabled(pg);//TODO
            break;
        case NV_097_SET_FOG_PARAMS_0:
        case NV_097_SET_FOG_PARAMS_1:
        case NV_097_SET_FOG_PARAMS_2:
//        pg->ltctxa[NV_IGRAPH_XF_LTCTXA_FOG_K][slot] = parameter;//TODO
//        pg->ltctxa_dirty[NV_IGRAPH_XF_LTCTXA_FOG_K] = true;//TODO
            break;
        case NV_097_SET_TEXTURE_FORMAT_0:
        case NV_097_SET_TEXTURE_FORMAT_1:
        case NV_097_SET_TEXTURE_FORMAT_2:
        case NV_097_SET_TEXTURE_FORMAT_3:
//        pg->texture_dirty[slot] = true;//TODO
            break;
        case NV_097_SET_TEXTURE_IMAGE_RECT_0:
        case NV_097_SET_TEXTURE_IMAGE_RECT_1:
        case NV_097_SET_TEXTURE_IMAGE_RECT_2:
        case NV_097_SET_TEXTURE_IMAGE_RECT_3:
//        pg->texture_dirty[slot] = true;//TODO
            break;
        case NV_097_SET_TEXTURE_OFFSET_0:
        case NV_097_SET_TEXTURE_OFFSET_1:
        case NV_097_SET_TEXTURE_OFFSET_2:
        case NV_097_SET_TEXTURE_OFFSET_3:
//        pg->texture_dirty[slot] = true;//TODO
            break;
        case NV_097_SET_TEXTURE_PALETTE_0:
        case NV_097_SET_TEXTURE_PALETTE_1:
        case NV_097_SET_TEXTURE_PALETTE_2:
        case NV_097_SET_TEXTURE_PALETTE_3:
//        pg->texture_dirty[slot] = true;//TODO
            break;
        }
        switch (c->method) {
#define CASE(x,y) \
        case NV_097_ ## x: \
            VARDUMP_NV2A(VAR_IN, c->param); \
            NV2A_REG32(p, NV_PGRAPH, y) = c->param; \
            break
#define CASEV(x,y,z) \
        case NV_097_ ## x: \
            VARDUMP_NV2A(VAR_IN, c->param); \
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, y, z, c->param); \
            break
#define CASEN(x) \
        case NV_097_ ## x: \
            INT3; \
            break
#if 1
        case NV_097_NO_OPERATION:
            VARDUMP_NV2A(VAR_IN, c->param);
            if (c->param) {
                if (!NV2A_REG32_MASK_GET(p, NV_PGRAPH, INTR, ERROR)) {
                    v = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, CTX_USER, CHID);
                    NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, TRAPPED_ADDR, CHID, v);
                    NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, TRAPPED_ADDR, SUBCH, c->subchannel);
                    NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, TRAPPED_ADDR, MTHD, c->method);
                    NV2A_REG32(p, NV_PGRAPH, TRAPPED_DATA_LOW) = c->param;
                    NV2A_REG32_MASK_SET(p, NV_PGRAPH, NSOURCE, NOTIFICATION_PENDING);
                    NV2A_REG32_MASK_SET(p, NV_PGRAPH, INTR, ERROR);
                    IRQ_NV2A_SIGNAL;
                }
                NV2A_IRQ_WAIT(PGRAPH, INTR, ERROR);
            }
            break;
#endif
        CASEV(SET_FLIP_READ, SURFACE, READ_3D);
        CASEV(SET_FLIP_WRITE, SURFACE, WRITE_3D);
        CASEV(SET_FLIP_MODULO, SURFACE, MODULO_3D);
        case NV_097_FLIP_INCREMENT_WRITE:
            VARDUMP_NV2A(VAR_IN, c->param);
            v = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, SURFACE, WRITE_3D) + 1;
            t = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, SURFACE, MODULO_3D);
            if (t) v %= t;
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, SURFACE, WRITE_3D, v);
            if (glFrameTerminatorGREMEDY) glFrameTerminatorGREMEDY();
            break;
#if 0
        case NV_097_FLIP_STALL:
            VARDUMP_NV2A(VAR_IN, c->param);
            FLIP_STALL_LOCK;
            while (NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, SURFACE, READ_3D) ==
                NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, SURFACE, WRITE_3D)) FLIP_STALL_WAIT;
            FLIP_STALL_UNLOCK;
            break;
#endif
#if 1
        case NV_097_BACK_END_WRITE_SEMAPHORE_RELEASE:
            VARDUMP_NV2A(VAR_IN, c->param);
            pos  = NV2A_REG32(p, NV_097, SET_CONTEXT_DMA_SEMAPHORE);
            get  = NV2A_REGADDR(p + pos, NV_PRAMIN, BASE);
            dma  = *(typeof(dma) *)get;
            pos  = NV2A_REG32(p, NV_097, SET_SEMAPHORE_OFFSET);
            if (pos >= dma.limit) INT3;
            pos += NV2A_DMA_ADDRESS(&dma);
            xboxkrnl_dma_write(pos, c->param, 4);
            break;
#endif
#if 1
//#elif 0
#define CASE2(x,y,z) \
        case NV_097_ ## x: \
            VARDUMP_NV2A(VAR_IN, c->param); \
            switch (c->param) { \
            CASE2M(x,y,z); \
            default: \
                INT3; \
                break; \
            } \
            break
#define CASE2I(x,i,y,z) \
        case NV_097_ ## x ## _ ## i: \
            VARDUMP_NV2A(VAR_IN, c->param); \
            switch (c->param) { \
            CASE2M(x,y,z); \
            default: \
                INT3; \
                break; \
            } \
            break
#define CASE2V(x,y,z,v) \
            case NV_097_ ## x ## _V_ ## v: \
                NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PGRAPH, y, z, v); \
                break
#define CASE3(x,y) \
        case NV_097_ ## x: \
            VARDUMP_NV2A(VAR_IN, c->param); \
            CASE3M(x,y); \
            break
#define CASE3I(x,i,y) \
        case NV_097_ ## x ## _ ## i: \
            VARDUMP_NV2A(VAR_IN, c->param); \
            CASE3M(x,y); \
            break
#define CASE3V(x,y,z) \
            v = NV2A_MASK_BITSHIFT_GET(c->param, NV_097_ ## x ## _ ## z); \
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, y, z, v)
#define CASE3V2(x,y,m,z) \
            v = NV2A_MASK_BITSHIFT_GET(c->param, NV_097_ ## x ## _ ## m); \
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, y, z, v)
        CASE(SET_ANTI_ALIASING_CONTROL, ANTIALIASING);
        CASEV(SET_BLEND_ENABLE, BLEND, EN);
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,ZERO); \
            CASE2V(x,y,z,ONE); \
            CASE2V(x,y,z,SRC_COLOR); \
            CASE2V(x,y,z,ONE_MINUS_SRC_COLOR); \
            CASE2V(x,y,z,SRC_ALPHA); \
            CASE2V(x,y,z,ONE_MINUS_SRC_ALPHA); \
            CASE2V(x,y,z,DST_ALPHA); \
            CASE2V(x,y,z,ONE_MINUS_DST_ALPHA); \
            CASE2V(x,y,z,DST_COLOR); \
            CASE2V(x,y,z,ONE_MINUS_DST_COLOR); \
            CASE2V(x,y,z,SRC_ALPHA_SATURATE); \
            CASE2V(x,y,z,CONSTANT_COLOR); \
            CASE2V(x,y,z,ONE_MINUS_CONSTANT_COLOR); \
            CASE2V(x,y,z,CONSTANT_ALPHA); \
            CASE2V(x,y,z,ONE_MINUS_CONSTANT_ALPHA)
        CASE2(SET_BLEND_FUNC_SFACTOR, BLEND, SFACTOR);
        CASE2(SET_BLEND_FUNC_DFACTOR, BLEND, DFACTOR);
#undef CASE2M
        CASEV(SET_LOGIC_OP_ENABLE, BLEND, LOGICOP_ENABLE);
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,CLEAR); \
            CASE2V(x,y,z,AND); \
            CASE2V(x,y,z,AND_REVERSE); \
            CASE2V(x,y,z,COPY); \
            CASE2V(x,y,z,AND_INVERTED); \
            CASE2V(x,y,z,NOOP); \
            CASE2V(x,y,z,XOR); \
            CASE2V(x,y,z,OR); \
            CASE2V(x,y,z,NOR); \
            CASE2V(x,y,z,EQUIV); \
            CASE2V(x,y,z,INVERT); \
            CASE2V(x,y,z,OR_REVERSE); \
            CASE2V(x,y,z,COPY_INVERTED); \
            CASE2V(x,y,z,OR_INVERTED); \
            CASE2V(x,y,z,NAND); \
            CASE2V(x,y,z,SET)
        CASE2(SET_LOGIC_OP, BLEND, LOGICOP);
#undef CASE2M
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,FUNC_SUBTRACT); \
            CASE2V(x,y,z,FUNC_REVERSE_SUBTRACT); \
            CASE2V(x,y,z,FUNC_ADD); \
            CASE2V(x,y,z,MIN); \
            CASE2V(x,y,z,MAX); \
            CASE2V(x,y,z,FUNC_REVERSE_SUBTRACT_SIGNED); \
            CASE2V(x,y,z,FUNC_ADD_SIGNED)
        CASE2(SET_BLEND_EQUATION, BLEND, EQN);
#undef CASE2M
        CASE(SET_BLEND_COLOR, BLENDCOLOR);
        CASE(SET_TEXTURE_BORDER_COLOR_0, BORDERCOLOR0);
        CASE(SET_TEXTURE_BORDER_COLOR_1, BORDERCOLOR1);
        CASE(SET_TEXTURE_BORDER_COLOR_2, BORDERCOLOR2);
        CASE(SET_TEXTURE_BORDER_COLOR_3, BORDERCOLOR3);
        CASEN(SET_TEXTURE_SET_BUMP_ENV_MAT00_0);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT00_1, BUMPMAT00_1);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT00_2, BUMPMAT00_2);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT00_3, BUMPMAT00_3);
        CASEN(SET_TEXTURE_SET_BUMP_ENV_MAT01_0);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT01_1, BUMPMAT01_1);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT01_2, BUMPMAT01_2);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT01_3, BUMPMAT01_3);
        CASEN(SET_TEXTURE_SET_BUMP_ENV_MAT11_0);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT11_1, BUMPMAT11_1);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT11_2, BUMPMAT11_2);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT11_3, BUMPMAT11_3);
        CASEN(SET_TEXTURE_SET_BUMP_ENV_MAT10_0);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT10_1, BUMPMAT10_1);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT10_2, BUMPMAT10_2);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT10_3, BUMPMAT10_3);
        CASEN(SET_TEXTURE_SET_BUMP_ENV_SCALE_0);
        CASE(SET_TEXTURE_SET_BUMP_ENV_SCALE_1, BUMPSCALE1);
        CASE(SET_TEXTURE_SET_BUMP_ENV_SCALE_2, BUMPSCALE2);
        CASE(SET_TEXTURE_SET_BUMP_ENV_SCALE_3, BUMPSCALE3);
        CASEN(SET_TEXTURE_SET_BUMP_ENV_OFFSET_0);
        CASE(SET_TEXTURE_SET_BUMP_ENV_OFFSET_1, BUMPOFFSET1);
        CASE(SET_TEXTURE_SET_BUMP_ENV_OFFSET_2, BUMPOFFSET2);
        CASE(SET_TEXTURE_SET_BUMP_ENV_OFFSET_3, BUMPOFFSET3);
        CASEV(SET_TRANSFORM_PROGRAM_LOAD, CHEOPS_OFFSET, PROG_LD_PTR);
        CASEV(SET_TRANSFORM_PROGRAM_START, CSV0_C, CHEOPS_PROGRAM_START);
        CASEV(SET_TRANSFORM_CONSTANT_LOAD, CHEOPS_OFFSET, CONST_LD_PTR);
        CASEV(SET_NORMALIZATION_ENABLE, CSV0_C, NORMALIZATION_ENABLE);
        CASE(SET_CLEAR_RECT_HORIZONTAL, CLEARRECTX);
        CASE(SET_CLEAR_RECT_VERTICAL, CLEARRECTY);
        CASE(SET_COLOR_CLEAR_VALUE, COLORCLEARVALUE);
        CASE(SET_COLOR_KEY_COLOR_0, COLORKEYCOLOR0);
        CASE(SET_COLOR_KEY_COLOR_1, COLORKEYCOLOR1);
        CASE(SET_COLOR_KEY_COLOR_2, COLORKEYCOLOR2);
        CASE(SET_COLOR_KEY_COLOR_3, COLORKEYCOLOR3);
        CASE(SET_COMBINER_FACTOR0_0, COMBINEFACTOR0_0);
        CASE(SET_COMBINER_FACTOR0_1, COMBINEFACTOR0_1);
        CASE(SET_COMBINER_FACTOR0_2, COMBINEFACTOR0_2);
        CASE(SET_COMBINER_FACTOR0_3, COMBINEFACTOR0_3);
        CASE(SET_COMBINER_FACTOR0_4, COMBINEFACTOR0_4);
        CASE(SET_COMBINER_FACTOR0_5, COMBINEFACTOR0_5);
        CASE(SET_COMBINER_FACTOR0_6, COMBINEFACTOR0_6);
        CASE(SET_COMBINER_FACTOR0_7, COMBINEFACTOR0_7);
        CASE(SET_COMBINER_FACTOR1_0, COMBINEFACTOR1_0);
        CASE(SET_COMBINER_FACTOR1_1, COMBINEFACTOR1_1);
        CASE(SET_COMBINER_FACTOR1_2, COMBINEFACTOR1_2);
        CASE(SET_COMBINER_FACTOR1_3, COMBINEFACTOR1_3);
        CASE(SET_COMBINER_FACTOR1_4, COMBINEFACTOR1_4);
        CASE(SET_COMBINER_FACTOR1_5, COMBINEFACTOR1_5);
        CASE(SET_COMBINER_FACTOR1_6, COMBINEFACTOR1_6);
        CASE(SET_COMBINER_FACTOR1_7, COMBINEFACTOR1_7);
        CASE(SET_COMBINER_ALPHA_ICW_0, COMBINEALPHAI0);
        CASE(SET_COMBINER_ALPHA_ICW_1, COMBINEALPHAI1);
        CASE(SET_COMBINER_ALPHA_ICW_2, COMBINEALPHAI2);
        CASE(SET_COMBINER_ALPHA_ICW_3, COMBINEALPHAI3);
        CASE(SET_COMBINER_ALPHA_ICW_4, COMBINEALPHAI4);
        CASE(SET_COMBINER_ALPHA_ICW_5, COMBINEALPHAI5);
        CASE(SET_COMBINER_ALPHA_ICW_6, COMBINEALPHAI6);
        CASE(SET_COMBINER_ALPHA_ICW_7, COMBINEALPHAI7);
        CASE(SET_COMBINER_ALPHA_OCW_0, COMBINEALPHAO0);
        CASE(SET_COMBINER_ALPHA_OCW_1, COMBINEALPHAO1);
        CASE(SET_COMBINER_ALPHA_OCW_2, COMBINEALPHAO2);
        CASE(SET_COMBINER_ALPHA_OCW_3, COMBINEALPHAO3);
        CASE(SET_COMBINER_ALPHA_OCW_4, COMBINEALPHAO4);
        CASE(SET_COMBINER_ALPHA_OCW_5, COMBINEALPHAO5);
        CASE(SET_COMBINER_ALPHA_OCW_6, COMBINEALPHAO6);
        CASE(SET_COMBINER_ALPHA_OCW_7, COMBINEALPHAO7);
        CASE(SET_COMBINER_COLOR_ICW_0, COMBINECOLORI0);
        CASE(SET_COMBINER_COLOR_ICW_1, COMBINECOLORI1);
        CASE(SET_COMBINER_COLOR_ICW_2, COMBINECOLORI2);
        CASE(SET_COMBINER_COLOR_ICW_3, COMBINECOLORI3);
        CASE(SET_COMBINER_COLOR_ICW_4, COMBINECOLORI4);
        CASE(SET_COMBINER_COLOR_ICW_5, COMBINECOLORI5);
        CASE(SET_COMBINER_COLOR_ICW_6, COMBINECOLORI6);
        CASE(SET_COMBINER_COLOR_ICW_7, COMBINECOLORI7);
        CASE(SET_COMBINER_COLOR_OCW_0, COMBINECOLORO0);
        CASE(SET_COMBINER_COLOR_OCW_1, COMBINECOLORO1);
        CASE(SET_COMBINER_COLOR_OCW_2, COMBINECOLORO2);
        CASE(SET_COMBINER_COLOR_OCW_3, COMBINECOLORO3);
        CASE(SET_COMBINER_COLOR_OCW_4, COMBINECOLORO4);
        CASE(SET_COMBINER_COLOR_OCW_5, COMBINECOLORO5);
        CASE(SET_COMBINER_COLOR_OCW_6, COMBINECOLORO6);
        CASE(SET_COMBINER_COLOR_OCW_7, COMBINECOLORO7);
        CASE(SET_COMBINER_CONTROL, COMBINECTL);
        CASE(SET_COMBINER_SPECULAR_FOG_CW0, COMBINESPECFOG0);
        CASE(SET_COMBINER_SPECULAR_FOG_CW1, COMBINESPECFOG1);
#define CASE3M(x,y) \
            CASE3V(x,y,STENCIL_WRITE_ENABLE); \
            CASE3V(x,y,Z_PERSPECTIVE_ENABLE); \
            CASE3V2(x,y,COLOR_SPACE_CONVERT, CSCONVERT); \
            CASE3V2(x,CONTROL_3, PREMULTIPLIEDALPHA, PREMULTALPHA); \
            CASE3V(x,CONTROL_3, TEXTUREPERSPECTIVE); \
            CASE3V(x,SETUPRASTER, Z_FORMAT)
        CASE3(SET_CONTROL0, CONTROL_0);
#undef CASE3M
        CASEV(SET_ALPHA_TEST_ENABLE, CONTROL_0, ALPHATESTENABLE);
        CASEV(SET_DEPTH_TEST_ENABLE, CONTROL_0, ZENABLE);
        CASEV(SET_DITHER_ENABLE, CONTROL_0, DITHERENABLE);
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,NEVER); \
            CASE2V(x,y,z,LESS); \
            CASE2V(x,y,z,EQUAL); \
            CASE2V(x,y,z,LEQUAL); \
            CASE2V(x,y,z,GREATER); \
            CASE2V(x,y,z,NOTEQUAL); \
            CASE2V(x,y,z,GEQUAL); \
            CASE2V(x,y,z,ALWAYS)
        CASE2(SET_ALPHA_FUNC, CONTROL_0, ALPHAFUNC);
        CASEV(SET_ALPHA_REF, CONTROL_0, ALPHAREF);
        CASE2(SET_DEPTH_FUNC, CONTROL_0, ZFUNC);
#define CASE3M(x,y) \
            CASE3V(x,y,ALPHA_WRITE_ENABLE); \
            CASE3V(x,y,RED_WRITE_ENABLE); \
            CASE3V(x,y,GREEN_WRITE_ENABLE); \
            CASE3V(x,y,BLUE_WRITE_ENABLE)
        CASE3(SET_COLOR_MASK, CONTROL_0);
#undef CASE3M
        CASEV(SET_DEPTH_MASK, CONTROL_0, ZWRITEENABLE);
        CASEV(SET_STENCIL_TEST_ENABLE, CONTROL_1, STENCIL_TEST_ENABLE);
        CASEV(SET_STENCIL_MASK, CONTROL_1, STENCIL_MASK_WRITE);
        CASE2(SET_STENCIL_FUNC, CONTROL_1, STENCIL_FUNC);
#undef CASE2M
        CASEV(SET_STENCIL_FUNC_REF, CONTROL_1, STENCIL_REF);
        CASEV(SET_STENCIL_FUNC_MASK, CONTROL_1, STENCIL_MASK_READ);
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,KEEP); \
            CASE2V(x,y,z,ZERO); \
            CASE2V(x,y,z,REPLACE); \
            CASE2V(x,y,z,INCRSAT); \
            CASE2V(x,y,z,DECRSAT); \
            CASE2V(x,y,z,INVERT); \
            CASE2V(x,y,z,INCR); \
            CASE2V(x,y,z,DECR)
        CASE2(SET_STENCIL_OP_FAIL, CONTROL_2, STENCIL_OP_FAIL);
        CASE2(SET_STENCIL_OP_ZFAIL, CONTROL_2, STENCIL_OP_ZFAIL);
        CASE2(SET_STENCIL_OP_ZPASS, CONTROL_2, STENCIL_OP_ZPASS);
#undef CASE2M
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,LINEAR); \
            CASE2V(x,y,z,EXP); \
            CASE2V(x,y,z,EXP2); \
            CASE2V(x,y,z,EXP_ABS); \
            CASE2V(x,y,z,EXP2_ABS); \
            CASE2V(x,y,z,LINEAR_ABS)
        CASE2(SET_FOG_MODE, CONTROL_3, FOG_MODE);
#undef CASE2M
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,SPEC_ALPHA); \
            CASE2V(x,y,z,RADIAL); \
            CASE2V(x,y,z,PLANAR); \
            CASE2V(x,y,z,ABS_PLANAR); \
            CASE2V(x,y,z,FOG_X)
        CASE2(SET_FOG_GEN_MODE, CSV0_D, FOGGENMODE);
#undef CASE2M
        case NV_097_SET_FOG_ENABLE:
            VARDUMP_NV2A(VAR_IN, c->param);
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, CONTROL_3, FOGENABLE, c->param);
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, CSV0_D, FOGENABLE, c->param);
            break;
        case NV_097_SET_POINT_PARAMS_ENABLE:
            VARDUMP_NV2A(VAR_IN, c->param);
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, CONTROL_3, POINTPARAMSENABLE, c->param);
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, CSV0_D, POINTPARAMSENABLE, c->param);
            break;
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,FLAT); \
            CASE2V(x,y,z,SMOOTH)
        CASE2(SET_SHADE_MODE, CONTROL_3, SHADEMODE);
#undef CASE2M
        case NV_097_SET_SPECULAR_ENABLE:
            VARDUMP_NV2A(VAR_IN, c->param);
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, CONTROL_3, SPECULARENABLE, c->param);
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, CSV0_C, SPECULARENABLE, c->param);
            break;
        CASEV(SET_FLAT_SHADE_OP, CONTROL_3, FLATSHADE_OP);
        CASEV(SET_SKIN_MODE, CSV0_D, SKIN);
        CASEV(SET_LIGHT_ENABLE_MASK, CSV0_D, LIGHT);
        CASEV(SET_TEXGEN_VIEW_MODEL, CSV0_D, TEXGEN_REF);
#define CASE3M(x,y) \
            CASE3V(x,y,MODE); \
            CASE3V(x,y,RANGE_MODE)
        CASE3(SET_TRANSFORM_EXECUTION_MODE, CSV0_D);
#undef CASE3M
        CASEV(SET_TRANSFORM_PROGRAM_CXT_WRITE_EN, CSV0_D, CHEOPS_STALL);
#define CASE3M(x,y) \
            CASE3V(x,y,RED); \
            CASE3V(x,y,GREEN); \
            CASE3V(x,y,BLUE); \
            CASE3V(x,y,ALPHA)
        CASE3(SET_FOG_COLOR, FOGCOLOR);
#undef CASE3M
        CASE(SET_FOG_PARAMS_0, FOGPARAM0);
        CASE(SET_FOG_PARAMS_1, FOGPARAM1);
        CASE(SET_FOG_PARAMS_2, POINTSIZE);
        CASEV(SET_WINDOW_CLIP_TYPE, SETUPRASTER, WINDOWCLIPTYPE);
        CASEV(SET_CULL_FACE_ENABLE, SETUPRASTER, CULLENABLE);
        CASEV(SET_POINT_SMOOTH_ENABLE, SETUPRASTER, POINTSMOOTHENABLE);
        CASEV(SET_LINE_SMOOTH_ENABLE, SETUPRASTER, LINESMOOTHENABLE);
        CASEV(SET_POLY_SMOOTH_ENABLE, SETUPRASTER, POLYSMOOTHENABLE);
        CASEV(SET_STIPPLE_CONTROL, SETUPRASTER, STIPPLE_EN);
        CASEV(SET_POLY_OFFSET_POINT_ENABLE, SETUPRASTER, POFFSETPOINTENABLE);
        CASEV(SET_POLY_OFFSET_LINE_ENABLE, SETUPRASTER, POFFSETLINEENABLE);
        CASEV(SET_POLY_OFFSET_FILL_ENABLE, SETUPRASTER, POFFSETFILLENABLE);
        CASEV(SET_LINE_WIDTH, SETUPRASTER, LINEWIDTH);
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,POINT); \
            CASE2V(x,y,z,LINE); \
            CASE2V(x,y,z,FILL)
        CASE2(SET_FRONT_POLYGON_MODE, SETUPRASTER, FRONTFACEMODE);
        CASE2(SET_BACK_POLYGON_MODE, SETUPRASTER, BACKFACEMODE);
#undef CASE2M
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,FRONT); \
            CASE2V(x,y,z,BACK); \
            CASE2V(x,y,z,FRONT_AND_BACK)
        CASE2(SET_CULL_FACE, SETUPRASTER, CULLCTRL);
#undef CASE2M
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,CW); \
            CASE2V(x,y,z,CCW)
        CASE2(SET_FRONT_FACE, SETUPRASTER, FRONTFACE);
#undef CASE2M
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,8); \
            CASE2V(x,y,z,16); \
            CASE2V(x,y,z,32); \
            CASE2V(x,y,z,64); \
            CASE2V(x,y,z,128); \
            CASE2V(x,y,z,OFF)
        CASE2(SET_SWATH_WIDTH, SETUPRASTER, SWATHWIDTH);
#undef CASE2M
#define CASE3M(x,y) \
            CASE3V(x,y,CULL_NEAR_FAR_EN); \
            CASE3V(x,ZCOMPRESSOCCLUDE, ZCLAMP_EN); \
            CASE3V2(x,y,CULL_IGNORE_W, IGNORE_WSIGN)
        CASE3(SET_ZMIN_MAX_CONTROL, SETUPRASTER);
#undef CASE3M
        CASE(SET_SHADER_CLIP_PLANE_MODE, SHADERCLIPMODE);
#define CASE3M(x,y) \
            CASE3V2(x,y,STAGE1, MAP1); \
            CASE3V2(x,y,STAGE2, MAP2); \
            CASE3V2(x,y,STAGE3, MAP3)
        CASE3(SET_DOT_RGBMAPPING, SHADERCTL);
#undef CASE3M
#define CASE3M(x,y) \
            CASE3V2(x,y,STAGE1, OUT1); \
            CASE3V2(x,y,STAGE2, OUT2); \
            CASE3V2(x,y,STAGE3, OUT3)
        CASE3(SET_SHADER_OTHER_STAGE_INPUT, SHADERCTL);
#undef CASE3M
        CASE(SET_SHADER_STAGE_PROGRAM, SHADERPROG);
        CASE(SET_SEMAPHORE_OFFSET, SEMAPHOREOFFSET);
        CASE(SET_SHADOW_ZSLOPE_THRESHOLD, SHADOWZSLOPETHRESHOLD);
        CASE(SET_SPECULAR_FOG_FACTOR_0, SPECFOGFACTOR0);
        CASE(SET_SPECULAR_FOG_FACTOR_1, SPECFOGFACTOR1);
        CASE(SET_SURFACE_CLIP_HORIZONTAL, SURFACECLIPX);
        CASE(SET_SURFACE_CLIP_VERTICAL, SURFACECLIPY);
        CASE(SET_TEXTURE_ADDRESS_0, TEXADDRESS0);
        CASE(SET_TEXTURE_ADDRESS_1, TEXADDRESS1);
        CASE(SET_TEXTURE_ADDRESS_2, TEXADDRESS2);
        CASE(SET_TEXTURE_ADDRESS_3, TEXADDRESS3);
        CASE(SET_TEXTURE_CONTROL0_0, TEXCTL0_0);
        CASE(SET_TEXTURE_CONTROL0_1, TEXCTL0_1);
        CASE(SET_TEXTURE_CONTROL0_2, TEXCTL0_2);
        CASE(SET_TEXTURE_CONTROL0_3, TEXCTL0_3);
        CASE(SET_TEXTURE_CONTROL1_0, TEXCTL1_0);
        CASE(SET_TEXTURE_CONTROL1_1, TEXCTL1_1);
        CASE(SET_TEXTURE_CONTROL1_2, TEXCTL1_2);
        CASE(SET_TEXTURE_CONTROL1_3, TEXCTL1_3);
        CASE(SET_TEXTURE_FILTER_0, TEXFILTER0);
        CASE(SET_TEXTURE_FILTER_1, TEXFILTER1);
        CASE(SET_TEXTURE_FILTER_2, TEXFILTER2);
        CASE(SET_TEXTURE_FILTER_3, TEXFILTER3);
#define CASE3M(x,y) \
            CASE3V(x,y,CONTEXT_DMA); \
            CASE3V2(x,y,CUBEMAP_ENABLE, CUBEMAPENABLE); \
            CASE3V(x,y,BORDER_SOURCE); \
            CASE3V(x,y,DIMENSIONALITY); \
            CASE3V(x,y,COLOR); \
            CASE3V(x,y,MIPMAP_LEVELS); \
            CASE3V(x,y,BASE_SIZE_U); \
            CASE3V(x,y,BASE_SIZE_V); \
            CASE3V(x,y,BASE_SIZE_P)
        CASE3I(SET_TEXTURE_FORMAT, 0, TEXFMT0);
        CASE3I(SET_TEXTURE_FORMAT, 1, TEXFMT1);
        CASE3I(SET_TEXTURE_FORMAT, 2, TEXFMT2);
        CASE3I(SET_TEXTURE_FORMAT, 3, TEXFMT3);
#undef CASE3M
        CASE(SET_TEXTURE_IMAGE_RECT_0, TEXIMAGERECT0);
        CASE(SET_TEXTURE_IMAGE_RECT_1, TEXIMAGERECT1);
        CASE(SET_TEXTURE_IMAGE_RECT_2, TEXIMAGERECT2);
        CASE(SET_TEXTURE_IMAGE_RECT_3, TEXIMAGERECT3);
        CASE(SET_TEXTURE_OFFSET_0, TEXOFFSET0);
        CASE(SET_TEXTURE_OFFSET_1, TEXOFFSET1);
        CASE(SET_TEXTURE_OFFSET_2, TEXOFFSET2);
        CASE(SET_TEXTURE_OFFSET_3, TEXOFFSET3);
        CASE(SET_TEXTURE_PALETTE_0, TEXPALETTE0);
        CASE(SET_TEXTURE_PALETTE_1, TEXPALETTE1);
        CASE(SET_TEXTURE_PALETTE_2, TEXPALETTE2);
        CASE(SET_TEXTURE_PALETTE_3, TEXPALETTE3);
        CASE(SET_WINDOW_CLIP_HORIZONTAL_0, WINDOWCLIPX0);
        CASE(SET_WINDOW_CLIP_HORIZONTAL_1, WINDOWCLIPX1);
        CASE(SET_WINDOW_CLIP_HORIZONTAL_2, WINDOWCLIPX2);
        CASE(SET_WINDOW_CLIP_HORIZONTAL_3, WINDOWCLIPX3);
        CASE(SET_WINDOW_CLIP_HORIZONTAL_4, WINDOWCLIPX4);
        CASE(SET_WINDOW_CLIP_HORIZONTAL_5, WINDOWCLIPX5);
        CASE(SET_WINDOW_CLIP_HORIZONTAL_6, WINDOWCLIPX6);
        CASE(SET_WINDOW_CLIP_HORIZONTAL_7, WINDOWCLIPX7);
        CASE(SET_WINDOW_CLIP_VERTICAL_0, WINDOWCLIPY0);
        CASE(SET_WINDOW_CLIP_VERTICAL_1, WINDOWCLIPY1);
        CASE(SET_WINDOW_CLIP_VERTICAL_2, WINDOWCLIPY2);
        CASE(SET_WINDOW_CLIP_VERTICAL_3, WINDOWCLIPY3);
        CASE(SET_WINDOW_CLIP_VERTICAL_4, WINDOWCLIPY4);
        CASE(SET_WINDOW_CLIP_VERTICAL_5, WINDOWCLIPY5);
        CASE(SET_WINDOW_CLIP_VERTICAL_6, WINDOWCLIPY6);
        CASE(SET_WINDOW_CLIP_VERTICAL_7, WINDOWCLIPY7);
        CASEV(SET_COMPRESS_ZBUFFER_EN, ZCOMPRESSOCCLUDE, COMPRESS_ZEN);
#define CASE3M(x,y) \
            CASE3V(x,y,OCCLUDE_ZEN); \
            CASE3V(x,y,OCCLUDE_STENCIL_EN)
        CASE3(SET_OCCLUDE_ZSTENCIL_EN, ZCOMPRESSOCCLUDE);
#undef CASE3M
        CASE(SET_ZSTENCIL_CLEAR_VALUE, ZSTENCILCLEARVALUE);
        CASE(SET_CLIP_MAX, ZCLIPMAX);
        CASE(SET_CLIP_MIN, ZCLIPMIN);
        CASE(SET_POLYGON_OFFSET_BIAS, ZOFFSETBIAS);
        CASE(SET_POLYGON_OFFSET_SCALE_FACTOR, ZOFFSETFACTOR);
        CASE(SET_EYE_VECTOR_0, EYEVEC0);
        CASE(SET_EYE_VECTOR_1, EYEVEC1);
        CASE(SET_EYE_VECTOR_2, EYEVEC2);
        CASE(SET_STIPPLE_PATTERN_0, STIPPLE0);
        CASE(SET_STIPPLE_PATTERN_1, STIPPLE1);
        CASE(SET_STIPPLE_PATTERN_2, STIPPLE2);
        CASE(SET_STIPPLE_PATTERN_3, STIPPLE3);
        CASE(SET_STIPPLE_PATTERN_4, STIPPLE4);
        CASE(SET_STIPPLE_PATTERN_5, STIPPLE5);
        CASE(SET_STIPPLE_PATTERN_6, STIPPLE6);
        CASE(SET_STIPPLE_PATTERN_7, STIPPLE7);
        CASE(SET_STIPPLE_PATTERN_8, STIPPLE8);
        CASE(SET_STIPPLE_PATTERN_9, STIPPLE9);
        CASE(SET_STIPPLE_PATTERN_10, STIPPLE10);
        CASE(SET_STIPPLE_PATTERN_11, STIPPLE11);
        CASE(SET_STIPPLE_PATTERN_12, STIPPLE12);
        CASE(SET_STIPPLE_PATTERN_13, STIPPLE13);
        CASE(SET_STIPPLE_PATTERN_14, STIPPLE14);
        CASE(SET_STIPPLE_PATTERN_15, STIPPLE15);
        CASE(SET_STIPPLE_PATTERN_16, STIPPLE16);
        CASE(SET_STIPPLE_PATTERN_17, STIPPLE17);
        CASE(SET_STIPPLE_PATTERN_18, STIPPLE18);
        CASE(SET_STIPPLE_PATTERN_19, STIPPLE19);
        CASE(SET_STIPPLE_PATTERN_20, STIPPLE20);
        CASE(SET_STIPPLE_PATTERN_21, STIPPLE21);
        CASE(SET_STIPPLE_PATTERN_22, STIPPLE22);
        CASE(SET_STIPPLE_PATTERN_23, STIPPLE23);
        CASE(SET_STIPPLE_PATTERN_24, STIPPLE24);
        CASE(SET_STIPPLE_PATTERN_25, STIPPLE25);
        CASE(SET_STIPPLE_PATTERN_26, STIPPLE26);
        CASE(SET_STIPPLE_PATTERN_27, STIPPLE27);
        CASE(SET_STIPPLE_PATTERN_28, STIPPLE28);
        CASE(SET_STIPPLE_PATTERN_29, STIPPLE29);
        CASE(SET_STIPPLE_PATTERN_30, STIPPLE30);
        CASE(SET_STIPPLE_PATTERN_31, STIPPLE31);
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,DISABLE); \
            CASE2V(x,y,z,NORMAL_MAP); \
            CASE2V(x,y,z,REFLECTION_MAP); \
            CASE2V(x,y,z,EYE_LINEAR); \
            CASE2V(x,y,z,OBJECT_LINEAR); \
            CASE2V(x,y,z,SPHERE_MAP)
        CASE2I(SET_TEXGEN_S, 0, CSV1_A, T0_S);
        CASE2I(SET_TEXGEN_S, 1, CSV1_A, T1_S);
        CASE2I(SET_TEXGEN_S, 2, CSV1_B, T2_S);
        CASE2I(SET_TEXGEN_S, 3, CSV1_B, T3_S);
        CASE2I(SET_TEXGEN_T, 0, CSV1_A, T0_T);
        CASE2I(SET_TEXGEN_T, 1, CSV1_A, T1_T);
        CASE2I(SET_TEXGEN_T, 2, CSV1_B, T2_T);
        CASE2I(SET_TEXGEN_T, 3, CSV1_B, T3_T);
#undef CASE2M
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,DISABLE); \
            CASE2V(x,y,z,NORMAL_MAP); \
            CASE2V(x,y,z,REFLECTION_MAP); \
            CASE2V(x,y,z,EYE_LINEAR); \
            CASE2V(x,y,z,OBJECT_LINEAR)
        CASE2I(SET_TEXGEN_R, 0, CSV1_A, T0_R);
        CASE2I(SET_TEXGEN_R, 1, CSV1_A, T1_R);
        CASE2I(SET_TEXGEN_R, 2, CSV1_B, T2_R);
        CASE2I(SET_TEXGEN_R, 3, CSV1_B, T3_R);
#undef CASE2M
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,DISABLE); \
            CASE2V(x,y,z,EYE_LINEAR); \
            CASE2V(x,y,z,OBJECT_LINEAR)
        CASE2I(SET_TEXGEN_Q, 0, CSV1_A, T0_Q);
        CASE2I(SET_TEXGEN_Q, 1, CSV1_A, T1_Q);
        CASE2I(SET_TEXGEN_Q, 2, CSV1_B, T2_Q);
        CASE2I(SET_TEXGEN_Q, 3, CSV1_B, T3_Q);
#undef CASE2M
        CASEV(SET_TEXTURE_MATRIX_ENABLE_0, CSV1_A, T0_ENABLE);
        CASEV(SET_TEXTURE_MATRIX_ENABLE_1, CSV1_A, T1_ENABLE);
        CASEV(SET_TEXTURE_MATRIX_ENABLE_2, CSV1_B, T2_ENABLE);
        CASEV(SET_TEXTURE_MATRIX_ENABLE_3, CSV1_B, T3_ENABLE);
#if 0 //XXX

#endif
#undef CASE3V2
#undef CASE3V
#undef CASE3I
#undef CASE3
#undef CASE2V
#undef CASE2I
#undef CASE2
#undef CASEN
#undef CASEV
#undef CASE
#endif
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

void
nv2a_pfifo_puller(register void *p) {
    nv2a_pfifo_cache c;
    nv2a_pfifo_ramht r;
    int busy = 0;
    ENTER_NV2A;

    if (!p) p = nv2a->memreg;

    for (;;) {
        if (!NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PULL0, ACCESS_ENABLED) ||
            !nv2a_pfifo_cache_dequeue(&c)) {
            busy = 0;
            EVENT_WAIT;//FIXME
            continue;
        }
        if (!busy) {
            busy = 1;
            glo_current(nv2a_ctx->glo, 1);
        }
        if (c.method == /* NV_SET_OBJECT */ 0 || (c.method >= 0x180 && c.method <= 0x1a8)) {
fprintf(stderr,"ramht_lookup: method: 0x%x | handle:%u / 0x%x\n",c.method,c.param,c.param);//XXX
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
#define CASE(x) \
            case x: \
                NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PFIFO, CACHE1_ENGINE, x, r.engine); \
                break
            CASE(0);
            CASE(1);
            CASE(2);
            CASE(3);
            CASE(4);
            CASE(5);
            CASE(6);
            CASE(7);
#undef CASE
            }
        } else if (c.method >= 0x100) {
            switch (c.subchannel) {
#define CASE(x) \
            case x: \
                r.engine = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_ENGINE, x); \
                break
            CASE(0);
            CASE(1);
            CASE(2);
            CASE(3);
            CASE(4);
            CASE(5);
            CASE(6);
            CASE(7);
#undef CASE
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
    register uint32_t o;
    register uint32_t *get;
    register uint32_t *put;
    register nv2a_pfifo_cache *c;
    register uint32_t *d;
    nv2a_dma dma;
    uint32_t addr;
    uint32_t v;
    ENTER_NV2A;

    if (!NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH0, ACCESS_ENABLED) ||
        !NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_PUSH, ACCESS_ENABLED) ||
        NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_PUSH, STATUS_SUSPENDED)) {
        LEAVE_NV2A;
        return;
    }

    o = NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID);
//PRINT("    chid:%hu", o);//XXX
    /* TODO: PIO not supported */
    if (!(NV2A_REG32(p, NV_PFIFO, MODE) & (1 << o))) {
        PRINT_NV2A("pb error: channel %u is not in DMA mode", o);
        INT3;
    }
    if (!NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH1, MODE_DMA)) {
        PRINT_NV2A("pb error: PIO not supported", 0);
        INT3;
    }

    get   = NV2A_REGADDR(p, NV_PRAMIN, BASE);
//PRINT("    get:%p", get);//XXX
    get  += NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_INSTANCE, ADDRESS) << 2;
//PRINT("    get + instance:%p | instance:%p", get,((NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_INSTANCE, ADDRESS)) << 4));//XXX
    dma   = *(typeof(dma) *)get;
    addr  = NV2A_DMA_ADDRESS(&dma);
//PRINT("    addr:%p | flags:%p | limit:%p | frame:%p | class:%p | target:%p",addr,dma.flags,dma.limit,dma.frame,dma.class,dma.target);//XXX
//INT3;//XXX
    o   <<= 16;
    get   = NV2A_REGADDR(p + o, NV_USER, DMA_GET);
    put   = NV2A_REGADDR(p + o, NV_USER, DMA_PUT);
    c     = NV2A_REGADDR(p, NV_PFIFO, CACHE1_DMA_STATE);
    d     = NV2A_REGADDR(p, NV_PFIFO, CACHE1_DMA_DCOUNT);

    PRINT_NV2A("pb DMA pusher: begin: "
        "limit: 0x%.08x | "
        "get: 0x%.08x < put: 0x%.08x | "
        "method count: %zu",
        dma.limit,
        *get,
        *put,
        (*put - *get) / 4);

    /* based on the convenient pseudocode in envytools */
    while (*get != *put) {
        if (*get >= dma.limit) {
            c->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_PROTECTION;
            break;
        }
        xboxkrnl_dma_read(addr + *get, &v, 4);
        o     = v;
        *get += 4;
//PRINT("pfifo_pusher: state/param:0x%.08x | method:0x%.03hx | subchannel:0x%.02hx | method_count:0x%.02hx | non_inc:%hhu | dcount:%u | error:%u",o,c->method,c->subchannel,c->method_count,c->non_inc,*d,c->error);//XXX
        if (c->method_count) {
            /* data word of methods command */
            NV2A_REG32(p, NV_PFIFO, CACHE1_DMA_DATA_SHADOW) = o;
            nv2a_pfifo_cache_enqueue(c, o);
            if (!c->non_inc) c->method += 4;
            --c->method_count;
            ++*d;
        } else {
            /* no command active - this is the first word of a new one */
            NV2A_REG32(p, NV_PFIFO, CACHE1_DMA_RSVD_SHADOW) = o;
            /* match all forms */
            if ((o & 0xe0000003) == 0x20000000) {
                /* old jump */
                NV2A_REG32(p, NV_PFIFO, CACHE1_DMA_GET_JMP_SHADOW) = *get;
                *get = NV2A_MASK_GET(o, NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET);
                PRINT_NV2A("pb OLD_JMP 0x%.08x", *get);
            } else if ((o & 3) == 1) {
                /* jump */
                NV2A_REG32(p, NV_PFIFO, CACHE1_DMA_GET_JMP_SHADOW) = *get;
                *get = NV2A_MASK_GET(o, NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET);
                PRINT_NV2A("pb JMP 0x%.08x", *get);
            } else if ((o & 3) == 2) {
                /* call */
                if (NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, STATE_ACTIVE)) {
                    c->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_CALL;
                    break;
                }
                NV2A_REG32_MASK_SET_VAL(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, RETURN_OFFSET, *get);
                NV2A_REG32_MASK_SET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, STATE_ACTIVE);
                *get = NV2A_MASK_GET(o, NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET);
                PRINT_NV2A("pb CALL 0x%.08x", *get);
            } else if (o == 0x00020000) {
                /* return */
                if (!NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, STATE_ACTIVE)) {
                    c->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_RETURN;
                    break;
                }
                *get = NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, RETURN_OFFSET);
                NV2A_REG32_MASK_UNSET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, STATE_ACTIVE);
                PRINT_NV2A("pb RET 0x%.08x", *get);
            } else if (!(o & 0xe0030003)) {
                /* increasing methods */
                c->state   = o;
                c->non_inc = 0;
                c->error   = 0;
                *d         = 0;
            } else if ((o & 0xe0030003) == 0x40000000) {
                /* non-increasing methods */
                c->state   = o;
                c->non_inc = 1;
                c->error   = 0;
                *d         = 0;
            } else {
                PRINT_NV2A("pb RESERVED_CMD 0x%.08x, word 0x%.08x", *get, o);
                c->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_RESERVED_CMD;
                break;
            }
        }
    }

    PRINT_NV2A("pb DMA pusher:   end: "
        "limit: 0x%.08x | "
        "get: 0x%.08x < put: 0x%.08x",
        dma.limit,
        *get,
        *put);

    if (c->error) {
        PRINT_NV2A("pb error: 0x%.08x", c->error);
        NV2A_REG32_MASK_SET(p, NV_PFIFO, CACHE1_DMA_PUSH, STATUS_SUSPENDED);
        NV2A_REG32_MASK_SET(p, NV_PFIFO, INTR_0, DMA_PUSHER);
        IRQ_NV2A_SIGNAL;
    } else {
        EVENT_SIGNAL;
    }

    LEAVE_NV2A;
}

void
nv2a_framebuffer_set(uint32_t addr) {
    ENTER_NV2A;

    NV2A_REG32(nv2a->memreg, NV_PCRTC, START) = addr;

    LEAVE_NV2A;
}

int
nv2a_write(uint32_t addr, const void *val, size_t sz) {
    register const nv2a_block *b;
    register const char *n;
    register void *p;
    register uint32_t r;
    register uint32_t v;
    register uint32_t o;
    const char *reg;
    register int ret = 0;
    ENTER_NV2A;

    if (nv2a_offset(&addr)) {
        LEAVE_NV2A;
        return ret;
    }

    b = nv2a_block_lookup(addr, &reg);
    n = nv2a->name;
    p = nv2a->memreg;
    r = addr - b->offset;

    switch (sz) {
    case 1:
        v = REG08(val);
        o = REG08(p + addr);
        PRINT_NV2A("%s: "
            "write: "
            "[0x%.08x+0x%.08x] (0x%.02hhx)       <- 0x%.02hhx       | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            o,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG08(p + addr) = v;
        ret = 1;
        break;
    case 2:
        v = REG16(val);
        o = REG16(p + addr);
        PRINT_NV2A("%s: "
            "write: "
            "[0x%.08x+0x%.08x] (0x%.04hx)     <- 0x%.04hx     | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            o,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
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
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            o,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG32(p + addr) = v;
        ret = 1;
        break;
    default:
        LEAVE_NV2A;
        return ret;
    }

    switch (b->index) {
    case NV_PMC:
        switch (r) {
        case NV_PMC_INTR_0:
            NV2A_REG32(p, NV_PMC, INTR_0) = o & ~v;
        case NV_PMC_INTR_EN_0:
            IRQ_NV2A_SIGNAL;
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
            IRQ_NV2A_SIGNAL;
            break;
        case NV_PFIFO_CACHE1_DMA_PUSH:
            if (NV2A_MASK_GET(o, NV_PFIFO_CACHE1_DMA_PUSH_STATUS_SUSPENDED) &&
                !NV2A_MASK_GET(v, NV_PFIFO_CACHE1_DMA_PUSH_STATUS_SUSPENDED)) {
                nv2a_pfifo_pusher(p);
            }
            break;
        case NV_PFIFO_CACHE1_DMA_PUT:
            o = NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID) << 16;
            NV2A_REG32_MASK_SET_VAL(p + o, NV_USER, DMA_PUT, OFFSET, v);
            break;
        case NV_PFIFO_CACHE1_DMA_GET:
            o = NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID) << 16;
            NV2A_REG32_MASK_SET_VAL(p + o, NV_USER, DMA_GET, OFFSET, v);
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
            IRQ_NV2A_SIGNAL;
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
            nv2a_irq_raise(NV2A_IRQ_PGRAPH);
            break;
        case NV_PGRAPH_INCREMENT:
            if (NV2A_MASK_BITSHIFT_GET(v, NV_PGRAPH_INCREMENT_READ_3D) ==
                NV_PGRAPH_INCREMENT_READ_3D_TRIGGER) {
                v = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, SURFACE, READ_3D) + 1;
                o = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, SURFACE, MODULO_3D);
                if (o) v %= o;
                NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, SURFACE, READ_3D, v);
                FLIP_STALL_SIGNAL;
            }
            break;
        case NV_PGRAPH_FIFO:
            if (!NV2A_MASK_GET(o, NV_PGRAPH_FIFO_ACCESS_ENABLED) &&
                NV2A_MASK_GET(v, NV_PGRAPH_FIFO_ACCESS_ENABLED)) nv2a_irq_raise(NV2A_IRQ_FIFO);
            break;
        }
        break;
    case NV_PCRTC:
        switch (r) {
        case NV_PCRTC_INTR_0:
            NV2A_REG32(p, NV_PCRTC, INTR_0) = o & ~v;
        case NV_PCRTC_INTR_EN_0:
            IRQ_NV2A_SIGNAL;
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
                    nv2a_pfifo_pusher(p);
                    break;
                }
            } else { /* pio */
                INT3;
            }
        }
        break;
    }

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
    const char *reg;
    register int ret = 0;
    ENTER_NV2A;

    if (nv2a_offset(&addr)) {
        LEAVE_NV2A;
        return ret;
    }

    b = nv2a_block_lookup(addr, &reg);
    n = nv2a->name;
    p = nv2a->memreg;
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
            v = host->memreg_size; /* vram size */
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
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG08(val) = v;
        ret = 1;
        break;
    case 2:
        v &= 0xffff;
        PRINT_NV2A("%s: "
            " read: "
            "[0x%.08x+0x%.08x]              -> 0x%.04hx     | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG16(val) = v;
        ret = 1;
        break;
    case 4:
        PRINT_NV2A("%s: "
            " read: "
            "[0x%.08x+0x%.08x]              -> 0x%.08x | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            n,
            p,
            addr,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG32(val) = v;
        ret = 1;
        break;
    default:
        break;
    }

    LEAVE_NV2A;
    return ret;
}

void
nv2a_destroy(void) {
    ENTER_NV2A;

    if (!nv2a_ctx) {
        LEAVE_NV2A;
        return;
    }

    if (nv2a_cacheq) {
        //TODO
    }

    glo_current(nv2a_ctx->glo, 1);
    glDeleteVertexArrays(1, &nv2a_ctx->gl_vertex_array);
//    glDeleteBuffers(1, &nv2a_ctx->gl_memory_buffer);
    glDeleteBuffers(1, &nv2a_ctx->gl_element_buffer);
    glDeleteBuffers(1, &nv2a_ctx->gl_inline_array_buffer);
    glDeleteBuffers(NV2A_MAX_VERTEX_ATTRIBS, nv2a_ctx->gl_inline_buffer);
    glDeleteBuffers(NV2A_MAX_VERTEX_ATTRIBS, nv2a_ctx->gl_converted_buffer);
    glDeleteTextures(1, &nv2a_ctx->gl_zeta_buffer);
    glDeleteTextures(1, &nv2a_ctx->gl_color_buffer);
    glDeleteFramebuffers(1, &nv2a_ctx->gl_framebuffer);
    glo_destroy(&nv2a_ctx->glo);

    free(nv2a_ctx);
    nv2a_ctx = NULL;

    LEAVE_NV2A;
}

int
nv2a_init(void) {
    GLint tmp;
    register int ret = 1;
    ENTER_NV2A;

    nv2a_destroy();

    nv2a_ctx = calloc(1, sizeof(*nv2a_ctx));

    do {
        if (glo_create(&nv2a_ctx->glo)) break;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &tmp);
        if (tmp < NV2A_MAX_VERTEX_ATTRIBS) {
            fprintf(stderr, "error: %s(): not enough GL vertex attributes supported\n", __func__);
            break;
        }
        glGenFramebuffers(1, &nv2a_ctx->gl_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, nv2a_ctx->gl_framebuffer);
        glGenTextures(1, &nv2a_ctx->gl_color_buffer);
        glBindTexture(GL_TEXTURE_2D, nv2a_ctx->gl_color_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 640, 480, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nv2a_ctx->gl_color_buffer, 0);
        if ((tmp = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
            fprintf(stderr, "error: %s(): GL framebuffer operation did not complete: error %i\n", __func__, tmp);
            break;
        }
        glGenBuffers(NV2A_MAX_VERTEX_ATTRIBS, nv2a_ctx->gl_converted_buffer);
        glGenBuffers(NV2A_MAX_VERTEX_ATTRIBS, nv2a_ctx->gl_inline_buffer);
        glGenBuffers(1, &nv2a_ctx->gl_inline_array_buffer);
        glGenBuffers(1, &nv2a_ctx->gl_element_buffer);
//        glGenBuffers(1, &nv2a_ctx->gl_memory_buffer);
//        glBindBuffer(GL_ARRAY_BUFFER, nv2a_ctx->gl_memory_buffer);
//        glBufferData(GL_ARRAY_BUFFER, 0x08000000, NULL, GL_DYNAMIC_DRAW);
        glGenVertexArrays(1, &nv2a_ctx->gl_vertex_array);
        glBindVertexArray(nv2a_ctx->gl_vertex_array);
        if ((ret = ((tmp = glGetError()) != GL_NO_ERROR))) {
            fprintf(stderr, "error: %s(): GL error occurred: error %i\n", __func__, tmp);
            break;
        }
        glo_current(nv2a_ctx->glo, 0);
    } while (0);

    if (ret) nv2a_destroy();

    LEAVE_NV2A;
    return ret;
}

