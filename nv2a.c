
#include "nv2a.h"

#define DEBUG_NV2A 1

#ifdef DEBUG_NV2A
# if DEBUG_NV2A == 1
#  define PRINT_NV2A        PRINT
# else
#  define ENTER_NV2A        ENTER
#  define LEAVE_NV2A        LEAVE
#  define PRINT_NV2A        PRINT
#  define VARDUMP_NV2A      VARDUMP
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
            uint32_t        subchannel   : 2;       /* 13 */
            uint32_t        pad3         : 3;       /* 15 */
            uint32_t        method_count : 10;      /* 18 */
            uint32_t        pad4         : 1;       /* 28 */
            uint32_t        error        : 3;       /* 29 */
        } PACKED;
    } PACKED;
    uint32_t                param;
    void *                  next;
} PACKED nv2a_pfifo_cache;

static nv2a_pfifo_cache *   nv2a_cacheq = NULL;
static pthread_mutexattr_t *nv2a_cacheq_mattr = NULL;
static pthread_mutex_t *    nv2a_cacheq_mutex = NULL;
static pthread_cond_t *     nv2a_cacheq_cond = NULL;
#define CACHEQ_LOCK         pthread_mutex_lock(nv2a_cacheq_mutex)
#define CACHEQ_UNLOCK       pthread_mutex_unlock(nv2a_cacheq_mutex)
#define CACHEQ_WAIT         pthread_cond_wait(nv2a_cacheq_cond, nv2a_cacheq_mutex)
#define CACHEQ_SIGNAL       pthread_cond_signal(nv2a_cacheq_cond)
#define CACHEQ_BROADCAST    pthread_cond_broadcast(nv2a_cacheq_cond)

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
            uint32_t        chid     : 4;           /* 24 */ /* FIXME: 5 bits? */
            uint32_t        pad2     : 3;           /* 28 */
            uint32_t        valid    : 1;           /* 31 */
        } PACKED;
    } PACKED;
} PACKED nv2a_pfifo_ramht;

#if 0
static int
nv2a_pfifo_cache_empty(void) {
    return !nv2a_cacheq;
}
#endif
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

    CACHEQ_SIGNAL;
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
        PRINT_NV2A("%s: CACHEQ: no commands in queue",
            xpci[XPCI_GPU].name);
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
    NV_PCRTC,       /* 15 */    /* more CRTC controls */
    NV_PRMCIO,      /* 16 */    /* aliases VGA CRTC and attribute controller registers */
    NV_PRAMDAC,     /* 17 */    /* RAMDAC, cursor, and PLL control */
    NV_PRMDIO,      /* 18 */    /* aliases VGA palette registers */
    NV_PRAMIN,      /* 19 */    /* RAMIN access */
    NV_USER,        /* 20 */    /* PFIFO MMIO and DMA submission area */
};

#define NV2A_BLOCK(x,y,z) \
    [NV_ ## x] = { \
        .index = NV_ ## x, \
        .name = #x, \
        .offset = y, \
        .size = z \
    }

static const nv2a_block const nv2a_blocks[] = {
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
    NV2A_BLOCK(PCRTC,       0x600000, 0x001000),    /* 15 */
    NV2A_BLOCK(PRMCIO,      0x601000, 0x001000),    /* 16 */
    NV2A_BLOCK(PRAMDAC,     0x680000, 0x001000),    /* 17 */
    NV2A_BLOCK(PRMDIO,      0x681000, 0x001000),    /* 18 */    /* not implemented */
    NV2A_BLOCK(PRAMIN,      0x700000, 0x100000),    /* 19 */
    NV2A_BLOCK(USER,        0x800000, 0x800000),    /* 20 */
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
nv2a_dma_write(register uint32_t dma_get, register uint32_t val) {
    register const char *n;
    register size_t i;
    ENTER_NV2A;

    n = xpci[XPCI_GPU].name;

    if (dma_get < sizeof(nv2a_dma)) {
        i = dma_get / 4;
        PRINT_NV2A("%s: DMA: write: nv2a_dma[%u] (0x%.08x) <- 0x%.08x", n, i, nv2a_dma[i], val);
        nv2a_dma[i] = val;
    } else {
        PRINT_NV2A("%s: DMA: write: [0x%.08x] (0x%.08x) <- 0x%.08x", n, dma_get, val);
        *(uint32_t *)dma_get = val;
    }

    LEAVE_NV2A;
    return 1;
}

int
nv2a_dma_read(register uint32_t dma_get, register uint32_t *val) {
    register const char *n;
    register size_t i;
    ENTER_NV2A;

    n = xpci[XPCI_GPU].name;

    if (dma_get < sizeof(nv2a_dma)) {
        i = dma_get / 4;
        *val = nv2a_dma[i];
        PRINT_NV2A("%s: DMA:  read: nv2a_dma[%u] -> 0x%.08x", n, i, *val);
    } else {
        *val = *(uint32_t *)dma_get;
        PRINT_NV2A("%s: DMA:  read: [0x%.08x]              -> 0x%.08x", n, dma_get, *val);
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
#if 0
#elif 0
static uint32_t ramht_hash(NV2AState *d, uint32_t handle)
{
    unsigned int ramht_size =
        1 << (GET_MASK(d->pfifo.regs[NV_PFIFO_RAMHT], NV_PFIFO_RAMHT_SIZE)+12);

    /* XXX: Think this is different to what nouveau calculates... */
    unsigned int bits = ffs(ramht_size)-2;

    uint32_t hash = 0;
    while (handle) {
        hash ^= (handle & ((1 << bits) - 1));
        handle >>= bits;
    }
    hash ^= d->pfifo.cache1.channel_id << (bits - 4);

    return hash;
}

static RAMHTEntry ramht_lookup(NV2AState *d, uint32_t handle)
{
    unsigned int ramht_size =
        1 << (GET_MASK(d->pfifo.regs[NV_PFIFO_RAMHT], NV_PFIFO_RAMHT_SIZE)+12);

    uint32_t hash = ramht_hash(d, handle);
    assert(hash * 8 < ramht_size);

    uint32_t ramht_address =
        GET_MASK(d->pfifo.regs[NV_PFIFO_RAMHT],
                 NV_PFIFO_RAMHT_BASE_ADDRESS) << 12;

    uint8_t *entry_ptr = d->ramin_ptr + ramht_address + hash * 8;

    uint32_t entry_handle = ldl_le_p((uint32_t*)entry_ptr);
    uint32_t entry_context = ldl_le_p((uint32_t*)(entry_ptr + 4));

    return (RAMHTEntry){
        .handle = entry_handle,
        .instance = (entry_context & NV_RAMHT_INSTANCE) << 4,
        .engine = (entry_context & NV_RAMHT_ENGINE) >> 16,
        .channel_id = (entry_context & NV_RAMHT_CHID) >> 24,
        .valid = entry_context & NV_RAMHT_STATUS,
    };
}

#elif 0
static void* pfifo_puller_thread(void *arg)
{
//    NV2AState *d = arg;
//    Cache1State *state = &d->pfifo.cache1;

//    glo_set_current(d->pgraph.gl_context);

    while (true) {
        qemu_mutex_lock(&state->cache_lock);
        while (QSIMPLEQ_EMPTY(&state->cache) || !state->pull_enabled) {
            qemu_cond_wait(&state->cache_cond, &state->cache_lock);

            if (d->exiting) {
                qemu_mutex_unlock(&state->cache_lock);
                glo_set_current(NULL);
                return NULL;
            }
        }
        QSIMPLEQ_CONCAT(&state->working_cache, &state->cache);
        qemu_mutex_unlock(&state->cache_lock);

        qemu_mutex_lock(&d->pgraph.lock);

        while (!QSIMPLEQ_EMPTY(&state->working_cache)) {
            CacheEntry * command = QSIMPLEQ_FIRST(&state->working_cache);
            QSIMPLEQ_REMOVE_HEAD(&state->working_cache, entry);

            if (command->method == 0) {
                // qemu_mutex_lock_iothread();
                RAMHTEntry entry = ramht_lookup(d, command->parameter);
                assert(entry.valid);

                assert(entry.channel_id == state->channel_id);
                // qemu_mutex_unlock_iothread();

                switch (entry.engine) {
                case ENGINE_GRAPHICS:
                    pgraph_context_switch(d, entry.channel_id);
                    pgraph_wait_fifo_access(d);
                    pgraph_method(d, command->subchannel, 0, entry.instance);
                    break;
                default:
                    assert(false);
                    break;
                }

                /* the engine is bound to the subchannel */
                qemu_mutex_lock(&state->cache_lock);
                state->bound_engines[command->subchannel] = entry.engine;
                state->last_engine = entry.engine;
                qemu_mutex_unlock(&state->cache_lock);
            } else if (command->method >= 0x100) {
                /* method passed to engine */

                uint32_t parameter = command->parameter;

                /* methods that take objects.
                 * TODO: Check this range is correct for the nv2a */
                if (command->method >= 0x180 && command->method < 0x200) {
                    //qemu_mutex_lock_iothread();
                    RAMHTEntry entry = ramht_lookup(d, command->parameter);
                    assert(entry.valid);
                    assert(entry.channel_id == state->channel_id);
                    parameter = entry.instance;
                    //qemu_mutex_unlock_iothread();
                }

                // qemu_mutex_lock(&state->cache_lock);
                enum FIFOEngine engine = state->bound_engines[command->subchannel];
                // qemu_mutex_unlock(&state->cache_lock);

                switch (engine) {
                case ENGINE_GRAPHICS:
                    pgraph_wait_fifo_access(d);
                    pgraph_method(d, command->subchannel,
                                  command->method, parameter);
                    break;
                default:
                    assert(false);
                    break;
                }

                // qemu_mutex_lock(&state->cache_lock);
                state->last_engine = state->bound_engines[command->subchannel];
                // qemu_mutex_unlock(&state->cache_lock);
            }

            g_free(command);
        }

        qemu_mutex_unlock(&d->pgraph.lock);
    }

    return NULL;
}
#endif
static int
nv2a_pfifo_ramht_lookup(register void *p, register uint32_t handle, register nv2a_pfifo_ramht *r) {
    register uint32_t bits;
    register uint32_t size;
    register uint32_t hash;
    register void *ptr;
    ENTER_NV2A;
PRINT("ramht_lookup: handle:%u / 0x%x",handle,handle);//XXX
    bits = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, RAMHT, SIZE) + 12;
    size = 1 << bits;

    for (--bits, hash = 0; handle; hash ^= handle & ((1 << bits) - 1), handle >>= bits);
    hash ^= NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID) << (bits - 4);

    ptr  = NV2A_REGADDR(p, NV_PRAMIN, BASE);
    ptr += NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, RAMHT, BASE_ADDRESS) << 12;
    ptr += hash << 3;

    *r = *(typeof(r))ptr;
PRINT("ramht_lookup: handle:%u | context:0x%.08x | pg param:0x%.04x | engine:%u | chid:%u | valid:%u | reg:%p", r->handle,r->context,r->instance << 4,r->engine,r->chid,r->valid,ptr-NV2A_REGADDR(p, NV_PRAMIN, BASE));//XXX
    LEAVE_NV2A;
    return r->valid;
}

static void
nv2a_pfifo_puller(register void *p) {
    register uint32_t param;
    nv2a_pfifo_cache c;
    nv2a_pfifo_ramht r;
    ENTER_NV2A;

//    glo_set_current(d->pgraph.gl_context);

    while (nv2a_pfifo_cache_dequeue(&c)) {
PRINT("pg method:0x%.03x | subchannel:0x%.03x | param:0x%.03x",c.method,c.subchannel,c.param);//XXX
        if (c.method == /* NV_SET_OBJECT */ 0 || (c.method >= 0x180 && c.method <= 0x200)) {
            if (!nv2a_pfifo_ramht_lookup(p, c.param, &r)) continue;
            if (r.chid != NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID)) INT3;
        }
        if (c.method == /* NV_SET_OBJECT */ 0) {
            switch (r.engine) {
            case NV_RAMHT_ENGINE_GRAPHICS:
#if 0
                pgraph_context_switch(d, entry.channel_id);
                pgraph_wait_fifo_access(d);
                pgraph_method(d, command->subchannel, 0, r.instance << 4);
#endif
                
                break;
            case NV_RAMHT_ENGINE_SW:
            case NV_RAMHT_ENGINE_DVD:
            default:
                INT3;
                break;
            }
#if 0
            /* the engine is bound to the subchannel */
            state->bound_engines[command->subchannel] = entry.engine;
            state->last_engine = entry.engine;
#endif
        } else if (c.method >= 0x100) {
            if (c.method >= 0x180 && c.method <= 0x200) {
                /* methods that take objects */
                param = r.instance << 4;
            } else {
                /* method passed to engine */
                param = c.param;
            }
#if 0
            //enum FIFOEngine engine = state->bound_engines[command->subchannel];
            switch (engine) {
            case NV_RAMHT_ENGINE_GRAPHICS:
# if 0
                pgraph_wait_fifo_access(d);
                pgraph_method(d, command->subchannel, command->method, param);
# endif
                
                break;
            case NV_RAMHT_ENGINE_SW:
            case NV_RAMHT_ENGINE_DVD:
            default:
                INT3;
                break;
            }
            //state->last_engine = engine;
#endif
        }
    }

    LEAVE_NV2A;
}

static void
nv2a_pfifo_pusher(register void *p) {
    register uint32_t chid;
    uint32_t *dma;
    uint32_t flags;
    uint32_t limit;
    uint32_t frame;
    uint32_t class;
    uint32_t target;
    uint32_t addr;
    uint32_t *put;
    uint32_t *dcount;
    nv2a_pfifo_cache *s;
    nv2a_pfifo_cache c;
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
        PRINT_NV2A("pb error: channel %u not in DMA mode", chid);
        INT3;
    }
    if (!(NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_PUSH1, MODE_DMA))) {
        PRINT_NV2A("pb error: PIO not supported", 0);
        INT3;
    }

    dma     = NV2A_REGADDR(p, NV_PRAMIN, BASE);
//PRINT("    dma:%p", dma);//XXX
    dma    += NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_INSTANCE, ADDRESS) << 2;
//PRINT("    dma + instance:%p | instance:%p", dma,((NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_INSTANCE, ADDRESS)) << 4));//XXX
    flags   = *dma++;
    limit   = *dma++;
    frame   = *dma;
    class   = NV2A_MASK_GET(flags, NV_DMA_CLASS);
    target  = NV2A_MASK_GET(flags, NV_DMA_TARGET);
//PRINT("    flags:%p | limit:%p | frame:%p | class:%p | target:%p", flags,limit,frame,class,target);//XXX
    addr    = NV2A_MASK_GET(frame, NV_DMA_ADDRESS) | NV2A_MASK_GET(flags, NV_DMA_ADJUST);
//PRINT("    addr:%p", addr);//XXX
    addr   &= 0x07ffffff;
//PRINT("    addr masked:%p", addr);//XXX

//INT3;//XXX

    chid  <<= 16;
    dma     = NV2A_REGADDR(p + chid, NV_USER, DMA_GET);
    put     = NV2A_REGADDR(p + chid, NV_USER, DMA_PUT);

    s       = NV2A_REGADDR(p, NV_PFIFO, CACHE1_DMA_STATE);
    dcount  = NV2A_REGADDR(p, NV_PFIFO, CACHE1_DMA_DCOUNT);

    PRINT_NV2A("pb DMA pusher:  run: "
        "limit: 0x%.08x | "
        "get: 0x%.08x < put: 0x%.08x",
        limit,
        *dma,
        *put);

    /* based on the convenient pseudocode in envytools */
    while (*dma != *put) {
        if (*dma >= limit) {
            s->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_PROTECTION;
            break;
        }

        nv2a_dma_read(addr + *dma, &c.param);
        *dma += 4;
PRINT("    param:0x%.08x | method:0x%.03hx | subchannel:0x%.02hx | method_count:0x%.02hx | non_inc:%hhu | dcount:%u | error:%u",c.param,s->method,s->subchannel,s->method_count,s->non_inc,*dcount,s->error);//XXX
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
                NV2A_REG32(p, NV_PFIFO, CACHE1_DMA_GET_JMP_SHADOW) = *dma;
                *dma = NV2A_MASK_GET(c.param, NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET);
                PRINT_NV2A("pb OLD_JMP 0x%.08x", *dma);
            } else if ((c.param & 3) == 1) {
                /* jump */
                NV2A_REG32(p, NV_PFIFO, CACHE1_DMA_GET_JMP_SHADOW) = *dma;
                *dma = NV2A_MASK_GET(c.param, NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET);
                PRINT_NV2A("pb JMP 0x%.08x", *dma);
            } else if ((c.param & 3) == 2) {
                /* call */
                if (NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, STATE_ACTIVE)) {
                    s->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_CALL;
                    break;
                }
                NV2A_REG32_MASK_SET_VAL(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, RETURN_OFFSET, *dma);
                NV2A_REG32_MASK_SET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, STATE_ACTIVE);
                *dma = NV2A_MASK_GET(c.param, NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET);
                PRINT_NV2A("pb CALL 0x%.08x", *dma);
            } else if (c.param == 0x00020000) {
                /* return */
                if (!NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, STATE_ACTIVE)) {
                    s->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_RETURN;
                    break;
                }
                *dma = NV2A_REG32_MASK_GET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, RETURN_OFFSET);
                NV2A_REG32_MASK_UNSET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, STATE_ACTIVE);
                PRINT_NV2A("pb RET 0x%.08x", *dma);
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
                PRINT_NV2A("pb RESERVED_CMD 0x%.08x, word 0x%.08x", *dma, c.param);
                s->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_RESERVED_CMD;
                break;
            }
        }
    }

    PRINT_NV2A("pb DMA pusher: done: "
        "limit: 0x%.08x | "
        "get: 0x%.08x < put: 0x%.08x",
        limit,
        *dma,
        *put);

    if (s->error) {
        PRINT_NV2A("pb error: 0x%.08x", s->error);

        INT3;

        NV2A_REG32_MASK_SET(p, NV_PFIFO, CACHE1_DMA_PUSH, STATUS_SUSPENDED);
        NV2A_REG32_MASK_SET(p, NV_PFIFO, INTR_0, DMA_PUSHER);
        nv2a_irq(p);
    }

    nv2a_pfifo_puller(p);
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
            if (NV2A_MASK_GET(
                    o,
                    NV_PFIFO_CACHE1_DMA_PUSH_STATUS_SUSPENDED) &&
                !NV2A_MASK_GET(
                    v,
                    NV_PFIFO_CACHE1_DMA_PUSH_STATUS_SUSPENDED)) {
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

    if (nv2a_cacheq_cond) {
        CACHEQ_BROADCAST;
        CACHEQ_LOCK;
        CACHEQ_UNLOCK;
        pthread_cond_destroy(nv2a_cacheq_cond);
        free(nv2a_cacheq_cond);
        nv2a_cacheq_cond = NULL;
    }
    if (nv2a_cacheq_mutex) {
        pthread_mutex_destroy(nv2a_cacheq_mutex);
        free(nv2a_cacheq_mutex);
        nv2a_cacheq_mutex = NULL;
    }
    if (nv2a_cacheq_mattr) {
        pthread_mutexattr_destroy(nv2a_cacheq_mattr);
        free(nv2a_cacheq_mattr);
        nv2a_cacheq_mattr = NULL;
    }
    if (nv2a_cacheq) {
        //TODO
    }

    LEAVE_NV2A;
}

static void
nv2a_init(void) {
    ENTER_NV2A;

    nv2a_destroy();

    nv2a_cacheq_mattr = calloc(1, sizeof(*nv2a_cacheq_mattr));
    nv2a_cacheq_mutex = calloc(1, sizeof(*nv2a_cacheq_mutex));
    nv2a_cacheq_cond  = calloc(1, sizeof(*nv2a_cacheq_cond));

    VARDUMP_NV2A(DUMP, nv2a_cacheq_mattr);
    VARDUMP_NV2A(DUMP, nv2a_cacheq_mutex);
    VARDUMP_NV2A(DUMP, nv2a_cacheq_cond);

    pthread_mutexattr_init(nv2a_cacheq_mattr);
    pthread_mutexattr_settype(nv2a_cacheq_mattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(nv2a_cacheq_mutex, nv2a_cacheq_mattr);
    pthread_cond_init(nv2a_cacheq_cond, NULL);

    LEAVE_NV2A;
}








