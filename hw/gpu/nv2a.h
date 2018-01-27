
#ifndef NV2A_H
#define NV2A_H

#define NV2A_REGADDR(x,y,z)         ((x) + nv2a_blocks[y].offset + y ## _ ## z)

#define NV2A_MASK_GET(x,y)          ((x) & (y))
#define NV2A_MASK_SET(x,y)          ((x) |= (y))
#define NV2A_MASK_UNSET(x,y)        ((x) &= ~(y))

#define NV2A_MASK_BITSHIFT_GET(x,y) (NV2A_MASK_GET((x), y) >> y ## __BITSHIFT)

/*
 * x = memreg base,
 * y = block index name,
 * z = register name (after block index name),
 * m = mask name (after register name),
 * v = variable
 */
#define NV2A_REG32(x,y,z) \
        REG32(NV2A_REGADDR((x), y, z))

#define NV2A_REG32_MASK_GET(x,y,z,m) \
        NV2A_MASK_GET(NV2A_REG32((x), y, z), y ## _ ## z ## _ ## m)
#define NV2A_REG32_MASK_SET(x,y,z,m) \
        NV2A_MASK_SET(NV2A_REG32((x), y, z), y ## _ ## z ## _ ## m)
#define NV2A_REG32_MASK_UNSET(x,y,z,m) \
        NV2A_MASK_UNSET(NV2A_REG32((x), y, z), y ## _ ## z ## _ ## m)

#define NV2A_REG32_MASK_SET_VAL(x,y,z,m,v) \
        NV2A_REG32_MASK_UNSET((x), y, z, m); \
        if ((v)) NV2A_MASK_SET(NV2A_REG32((x), y, z), NV2A_MASK_GET((v), y ## _ ## z ## _ ## m))

#define NV2A_REG32_MASK_BITSHIFT_GET(x,y,z,m) \
        (NV2A_REG32_MASK_GET(x,y,z,m) >> y ## _ ## z ## _ ## m ## __BITSHIFT)
#define NV2A_REG32_MASK_BITSHIFT_SET(x,y,z,m,v) \
        NV2A_REG32_MASK_UNSET((x), y, z, m); \
        if ((y ## _ ## z ## _ ## m ## _ ## v)) NV2A_MASK_SET(NV2A_REG32((x), y, z), NV2A_MASK_GET((y ## _ ## z ## _ ## m ## _ ## v) << y ## _ ## z ## _ ## m ## __BITSHIFT, y ## _ ## z ## _ ## m))
#define NV2A_REG32_MASK_BITSHIFT_SET_VAL(x,y,z,m,v) \
        NV2A_REG32_MASK_UNSET((x), y, z, m); \
        if ((v)) NV2A_MASK_SET(NV2A_REG32((x), y, z), NV2A_MASK_GET((v) << y ## _ ## z ## _ ## m ## __BITSHIFT, y ## _ ## z ## _ ## m))

#define NV2A_MAX_CHANNELS           32
#define NV2A_MAX_SUBCHANNELS        8
#define NV2A_MAX_VERTEX_ATTRIBS     16
#define NV2A_MAX_TEXTURES           4

enum {
    NV_PMC          =   0,      /* card master control */
    NV_PBUS,        /*  1 */    /* bus control */
    NV_PFIFO,       /*  2 */    /* MMIO and DMA FIFO submission to PGRAPH and VPE */
    NV_PFIFO_CACHE, /*  3 */
    NV_PRMA,        /*  4 */    /* access to BAR0/BAR1 from real mode */
    NV_PVIDEO,      /*  5 */    /* video overlay */
    NV_PTIMER,      /*  6 */    /* time measurement and time-based alarms */
    NV_PCOUNTER,    /*  7 */    /* performance monitoring counters */
    NV_PVPE,        /*  8 */    /* MPEG2 decoding engine */
    NV_PTV,         /*  9 */    /* TV encoder */
    NV_PRMFB,       /* 10 */    /* aliases VGA memory window */
    NV_PRMVIO,      /* 11 */    /* aliases VGA sequencer and graphics controller registers */
    NV_PFB,         /* 12 */    /* memory interface */
    NV_PSTRAPS,     /* 13 */    /* straps readout / override */
    NV_PGRAPH,      /* 14 */    /* accelerated 2d/3d drawing engine */
    NV_09F,         /* 15 */    /* NV12_IMAGE_BLIT object */
    NV_097,         /* 16 */    /* NV20_KELVIN_PRIMITIVE object */
    NV_PCRTC,       /* 17 */    /* more CRTC controls */
    NV_PRMCIO,      /* 18 */    /* aliases VGA CRTC and attribute controller registers */
    NV_PRAMDAC,     /* 19 */    /* RAMDAC, cursor, and PLL control */
    NV_PRMDIO,      /* 20 */    /* aliases VGA palette registers */
    NV_062,         /* 21 */    /* NV10_CONTEXT_SURFACES_2D object */
    NV_PRAMIN,      /* 22 */    /* RAMIN access */
    NV_USER,        /* 23 */    /* PFIFO MMIO and DMA submission area */
};

typedef struct {
    const uint32_t          index;
    const char *            name;
    const uint32_t          offset;
    const uint32_t          size;
} nv2a_block;

#define NV2A_BLOCK_SIZE(x) nv2a_blocks[x].size

static const nv2a_block const nv2a_blocks[] = {
#define NV2A_BLOCK(x,y,z) \
    [NV_ ## x] = { \
        .index = NV_ ## x, \
        .name = #x, \
        .offset = y, \
        .size = z \
    }
    NV2A_BLOCK(PMC,         0x000000, 0x001000),    /*  0 */
    NV2A_BLOCK(PBUS,        0x001000, 0x001000),    /*  1 */
    NV2A_BLOCK(PFIFO,       0x002000, 0x002000),    /*  2 */
    NV2A_BLOCK(PFIFO_CACHE, 0x004000, 0x003000),    /*  3 */
    NV2A_BLOCK(PRMA,        0x007000, 0x001000),    /*  4 */    /* not implemented */
    NV2A_BLOCK(PVIDEO,      0x008000, 0x001000),    /*  5 */
    NV2A_BLOCK(PTIMER,      0x009000, 0x001000),    /*  6 */
    NV2A_BLOCK(PCOUNTER,    0x00a000, 0x001000),    /*  7 */    /* not implemented */
    NV2A_BLOCK(PVPE,        0x00b000, 0x001000),    /*  8 */    /* not implemented */
    NV2A_BLOCK(PTV,         0x00d000, 0x001000),    /*  9 */    /* not implemented */
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

/* 21: NV_062 - NV10_CONTEXT_SURFACES_2D object */
#include "reg/nv10.h"
/* 15: NV_09F - NV12_IMAGE_BLIT object */
#include "reg/nv12.h"
/* 16: NV_097 - NV20_KELVIN_PRIMITIVE object */
#include "reg/nv20.h"

enum {                      /*  0: NV_PMC */            /* 0 */     /* card master control */
        NV_PMC_BOOT_0                               = 0x00000000,
        NV_PMC_INTR_0                               = 0x00000100,
#define NV_PMC_INTR_0_PFIFO                             (1 << 8)        /* 0x00000100 */
#define NV_PMC_INTR_0_PGRAPH                            (1 << 12)       /* 0x00001000 */
#define NV_PMC_INTR_0_PCRTC                             (1 << 24)       /* 0x01000000 */
#define NV_PMC_INTR_0_PBUS                              (1 << 28)       /* 0x10000000 */
#define NV_PMC_INTR_0_SOFTWARE                          (1 << 31)       /* 0x80000000 */
        NV_PMC_INTR_EN_0                            = 0x00000140,
#define NV_PMC_INTR_EN_0_INTA_HARDWARE                  0x00000001
#define NV_PMC_INTR_EN_0_INTA_SOFTWARE                  0x00000002
};
static const char *const nv2a_pmc_name[] = {
    NAMER(NV_PMC_BOOT_0),
    NAMER(NV_PMC_INTR_0),
    NAMER(NV_PMC_INTR_EN_0),
};
                            /*  1: NV_PBUS */           /* 1 */     /* bus control */
enum {                      /*  2: NV_PFIFO */          /* 2 */     /* MMIO and DMA FIFO submission to PGRAPH and VPE */
        NV_PFIFO_INTR_0                             = 0x00000100,
#define NV_PFIFO_INTR_0_CACHE_ERROR                     (1 << 0)        /* 0x00000001 */
#define NV_PFIFO_INTR_0_RUNOUT                          (1 << 4)        /* 0x00000010 */
#define NV_PFIFO_INTR_0_RUNOUT_OVERFLOW                 (1 << 8)        /* 0x00000100 */
#define NV_PFIFO_INTR_0_DMA_PUSHER                      (1 << 12)       /* 0x00001000 */
#define NV_PFIFO_INTR_0_DMA_PT                          (1 << 16)       /* 0x00010000 */
#define NV_PFIFO_INTR_0_SEMAPHORE                       (1 << 20)       /* 0x00100000 */
#define NV_PFIFO_INTR_0_ACQUIRE_TIMEOUT                 (1 << 24)       /* 0x01000000 */
        NV_PFIFO_INTR_EN_0                          = 0x00000140,
#define NV_PFIFO_INTR_EN_0_CACHE_ERROR                  (1 << 0)        /* 0x00000001 */
#define NV_PFIFO_INTR_EN_0_RUNOUT                       (1 << 4)        /* 0x00000010 */
#define NV_PFIFO_INTR_EN_0_RUNOUT_OVERFLOW              (1 << 8)        /* 0x00000100 */
#define NV_PFIFO_INTR_EN_0_DMA_PUSHER                   (1 << 12)       /* 0x00001000 */
#define NV_PFIFO_INTR_EN_0_DMA_PT                       (1 << 16)       /* 0x00010000 */
#define NV_PFIFO_INTR_EN_0_SEMAPHORE                    (1 << 20)       /* 0x00100000 */
#define NV_PFIFO_INTR_EN_0_ACQUIRE_TIMEOUT              (1 << 24)       /* 0x01000000 */
        NV_PFIFO_RAMHT                              = 0x00000210,
#define NV_PFIFO_RAMHT_BASE_ADDRESS                     0x000001f0
#define NV_PFIFO_RAMHT_BASE_ADDRESS__BITSHIFT           4
#define NV_PFIFO_RAMHT_SIZE                             0x00030000
#define NV_PFIFO_RAMHT_SIZE__BITSHIFT                   16
#define NV_PFIFO_RAMHT_SIZE_4K                          0               /* 0x00000000 */
#define NV_PFIFO_RAMHT_SIZE_8K                          1               /* 0x00010000 */
#define NV_PFIFO_RAMHT_SIZE_16K                         2               /* 0x00020000 */
#define NV_PFIFO_RAMHT_SIZE_32K                         3               /* 0x00030000 */
//#define NV_PFIFO_RAMHT_SEARCH                              0x03000000
//#define NV_PFIFO_RAMHT_SEARCH_16                           0
//#define NV_PFIFO_RAMHT_SEARCH_32                           1
//#define NV_PFIFO_RAMHT_SEARCH_64                           2
//#define NV_PFIFO_RAMHT_SEARCH_128                          3
        NV_PFIFO_RUNOUT_STATUS                      = 0x00000400,
#define NV_PFIFO_RUNOUT_STATUS_RANOUT_TRUE              (1 << 0)        /* 0x00000001 */
#define NV_PFIFO_RUNOUT_STATUS_LOW_MARK_EMPTY           (1 << 4)        /* 0x00000010 */
#define NV_PFIFO_RUNOUT_STATUS_HIGH_MARK_FULL           (1 << 8)        /* 0x00000100 */
        NV_PFIFO_MODE                               = 0x00000504,
//#define NV_PFIFO_MODE_DMA                               0xffffffff
//#define NV_PFIFO_MODE_PIO                               0x00000000
        NV_PFIFO_CACHE1_PUSH0                       = 0x00001200,
#define NV_PFIFO_CACHE1_PUSH0_ACCESS_ENABLED            (1 << 0)        /* 0x00000001 */    /* push access enabled */
        NV_PFIFO_CACHE1_PUSH1                       = 0x00001204,
#define NV_PFIFO_CACHE1_PUSH1_CHID                      0x0000001f
#define NV_PFIFO_CACHE1_PUSH1_MODE_DMA                  0x00000100
        NV_PFIFO_CACHE1_STATUS                      = 0x00001214,
#define NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY           (1 << 4)        /* 0x00000010 */
#define NV_PFIFO_CACHE1_STATUS_HIGH_MARK_FULL           (1 << 8)        /* 0x00000100 */
        NV_PFIFO_CACHE1_DMA_PUSH                    = 0x00001220,
#define NV_PFIFO_CACHE1_DMA_PUSH_ACCESS_ENABLED         (1 << 0)        /* 0x00000001 */    /* push access enabled */
#define NV_PFIFO_CACHE1_DMA_PUSH_STATE_BUSY             (1 << 4)        /* 0x00000010 */    /* push state busy */
#define NV_PFIFO_CACHE1_DMA_PUSH_BUFFER_EMPTY           (1 << 8)        /* 0x00000100 */    /* push buffer empty */
#define NV_PFIFO_CACHE1_DMA_PUSH_STATUS_SUSPENDED       (1 << 12)       /* 0x00001000 */    /* push status suspended */
#define NV_PFIFO_CACHE1_DMA_PUSH_ACQUIRE_PENDING        (1 << 16)       /* 0x00010000 */    /* push acquire pending */
        NV_PFIFO_CACHE1_DMA_STATE                   = 0x00001228,
#define NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE_NON_INC   (1 << 0)        /* 0x00000001 */
#define NV_PFIFO_CACHE1_DMA_STATE_METHOD                0x00001ffc
#define NV_PFIFO_CACHE1_DMA_STATE_SUBCHANNEL            0x0000e000
#define NV_PFIFO_CACHE1_DMA_STATE_METHOD_COUNT          0x1ffc0000
#define NV_PFIFO_CACHE1_DMA_STATE_ERROR                 0xe0000000
#define NV_PFIFO_CACHE1_DMA_STATE_ERROR__BITSHIFT       29
#define NV_PFIFO_CACHE1_DMA_STATE_ERROR_NONE            0               /* 0x00000000 */
#define NV_PFIFO_CACHE1_DMA_STATE_ERROR_CALL            1               /* 0x20000000 */
#define NV_PFIFO_CACHE1_DMA_STATE_ERROR_NON_CACHE       2               /* 0x40000000 */
#define NV_PFIFO_CACHE1_DMA_STATE_ERROR_RETURN          3               /* 0x60000000 */
#define NV_PFIFO_CACHE1_DMA_STATE_ERROR_RESERVED_CMD    4               /* 0x80000000 */
#define NV_PFIFO_CACHE1_DMA_STATE_ERROR_PROTECTION      6               /* 0xc0000000 */
        NV_PFIFO_CACHE1_DMA_INSTANCE                = 0x0000122c,
#define NV_PFIFO_CACHE1_DMA_INSTANCE_ADDRESS            0x0000ffff
        NV_PFIFO_CACHE1_DMA_PUT                     = 0x00001240,
#define NV_PFIFO_CACHE1_DMA_PUT_OFFSET                  0xfffffffc
        NV_PFIFO_CACHE1_DMA_GET                     = 0x00001244,
#define NV_PFIFO_CACHE1_DMA_GET_OFFSET                  0xfffffffc
        NV_PFIFO_CACHE1_DMA_SUBROUTINE              = 0x0000124c,
#define NV_PFIFO_CACHE1_DMA_SUBROUTINE_RETURN_OFFSET    0xfffffffc
#define NV_PFIFO_CACHE1_DMA_SUBROUTINE_STATE_ACTIVE     (1 << 0)        /* 0x00000001 */
        NV_PFIFO_CACHE1_PULL0                       = 0x00001250,
#define NV_PFIFO_CACHE1_PULL0_ACCESS_ENABLED            (1 << 0)        /* 0x00000001 */    /* pull access enabled */
        NV_PFIFO_CACHE1_ENGINE                      = 0x00001280,
#define NV_PFIFO_CACHE1_ENGINE_0                        0x00000003
#define NV_PFIFO_CACHE1_ENGINE_0__BITSHIFT              0
#define NV_PFIFO_CACHE1_ENGINE_1                        0x00000030
#define NV_PFIFO_CACHE1_ENGINE_1__BITSHIFT              4
#define NV_PFIFO_CACHE1_ENGINE_2                        0x00000300
#define NV_PFIFO_CACHE1_ENGINE_2__BITSHIFT              8
#define NV_PFIFO_CACHE1_ENGINE_3                        0x00003000
#define NV_PFIFO_CACHE1_ENGINE_3__BITSHIFT              12
#define NV_PFIFO_CACHE1_ENGINE_4                        0x00030000
#define NV_PFIFO_CACHE1_ENGINE_4__BITSHIFT              16
#define NV_PFIFO_CACHE1_ENGINE_5                        0x00300000
#define NV_PFIFO_CACHE1_ENGINE_5__BITSHIFT              20
#define NV_PFIFO_CACHE1_ENGINE_6                        0x03000000
#define NV_PFIFO_CACHE1_ENGINE_6__BITSHIFT              24
#define NV_PFIFO_CACHE1_ENGINE_7                        0x30000000
#define NV_PFIFO_CACHE1_ENGINE_7__BITSHIFT              28
        NV_PFIFO_CACHE1_DMA_DCOUNT                  = 0x000012a0,
//#define NV_PFIFO_CACHE1_DMA_DCOUNT_VALUE                0x00001ffc
        NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW          = 0x000012a4,
#define NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET       0xfffffffc
        NV_PFIFO_CACHE1_DMA_RSVD_SHADOW             = 0x000012a8,
        NV_PFIFO_CACHE1_DMA_DATA_SHADOW             = 0x000012ac,
};
static const char *const nv2a_pfifo_intr_name[] = {
    NAMEB(0,  cache_error),
    NAMEB(4,  runout),
    NAMEB(8,  runout_overflow),
    NAMEB(12, dma_pusher),
    NAMEB(16, dma_pt),
    NAMEB(20, semaphore),
    NAMEB(24, acquire_timeout),
};
                            /*  3: NV_PFIFO_CACHE */    /* 3 */
                            /*  4: NV_PRMA */           /* 4 */     /* access to BAR0/BAR1 from real mode */
                            /*  5: NV_PVIDEO */         /* 5 */     /* video overlay */
enum {                      /*  6: NV_PTIMER */         /* 6 */     /* time measurement and time-based alarms */
        NV_PTIMER_INTR_0                            = 0x00000100,
#define NV_PTIMER_INTR_0_ALARM                          (1 << 0)        /* 0x00000001 */
        NV_PTIMER_INTR_EN_0                         = 0x00000140,
#define NV_PTIMER_INTR_EN_0_ALARM                       (1 << 0)        /* 0x00000001 */
};
                            /*  7: NV_PCOUNTER */       /* 7 */     /* performance monitoring counters */
                            /*  8: NV_PVPE */           /* 8 */     /* MPEG2 decoding engine */
                            /*  9: NV_PTV */            /* 9 */     /* TV encoder */
                            /* 10: NV_PRMFB */          /* 10 */    /* aliases VGA memory window */
                            /* 11: NV_PRMVIO */         /* 11 */    /* aliases VGA sequencer and graphics controller registers */
enum {                      /* 12: NV_PFB */            /* 12 */    /* memory interface */
        NV_PFB_CFG0                                 = 0x00000200,
#define NV_PFB_CFG0_PART_4                              0x00000003
        NV_PFB_CSTATUS                              = 0x0000020c,
        NV_PFB_WBC                                  = 0x00000410,
//#define NV_PFB_WBC_FLUSH_PENDING                    (1 << 16)       /* 0x00010000 */
};
                            /* 13: NV_PSTRAPS */        /* straps readout / override */
/* 14: NV_PGRAPH - accelerated 2d/3d drawing engine */
#include "reg/pgraph.h"
enum {                      /* 15: NV_PCRTC */          /* 15 */    /* more CRTC controls */
        NV_PCRTC_INTR_0                             = 0x00000100,
#define NV_PCRTC_INTR_0_VBLANK                          (1 << 0)        /* 0x00000001 */
        NV_PCRTC_INTR_EN_0                          = 0x00000140,
#define NV_PCRTC_INTR_EN_0_VBLANK                       (1 << 0)        /* 0x00000001 */
        NV_PCRTC_START                              = 0x00000800,
};
static const char *const nv2a_pcrtc_intr_name[] = {
    NAMEB(0,  vblank),
};
                            /* 16: NV_PRMCIO */         /* 16 */    /* aliases VGA CRTC and attribute controller registers */
                            /* 17: NV_PRAMDAC */        /* 17 */    /* RAMDAC, cursor, and PLL control */
                            /* 18: NV_PRMDIO */         /* 18 */    /* aliases VGA palette registers */
enum {                      /* 19: NV_PRAMIN */         /* 19 */    /* RAMIN access */
        NV_PRAMIN_BASE                              = 0x00000000,
};
enum {                      /* 20: NV_USER */           /* 20 */    /* PFIFO MMIO and DMA submission area */
#define NV_USER_ADDR_REG(x)                             (((x) & 0x0000ffff) >>  0)
#define NV_USER_ADDR_CHID(x)                            (((x) & 0xffff0000) >> 16)
        NV_USER_OBJECT                              = 0x00000000,
#define NV_USER_OBJECT_OFFSET(x,y)                      (((x) << 16) + ((y) << 13)) /* x = channel, y = subchannel */
        NV_USER_DMA_PUT                             = 0x00000040,
#define NV_USER_DMA_PUT_OFFSET                          0xfffffffc
        NV_USER_DMA_GET                             = 0x00000044,
#define NV_USER_DMA_GET_OFFSET                          0xfffffffc
        NV_USER_REF                                 = 0x00000048,
};

////#define NV_DMA_CLASS                                    0x00000fff
//#define NV_DMA_PAGE_TABLE                                      (1 << 12)
//#define NV_DMA_PAGE_ENTRY                                      (1 << 13)
//#define NV_DMA_FLAGS_ACCESS                                    (1 << 14)
//#define NV_DMA_FLAGS_MAPPING_COHERENCY                         (1 << 15)
////#define NV_DMA_TARGET                                   0x00030000
//#   define NV_DMA_TARGET_NVM                                      0x00000000
//#   define NV_DMA_TARGET_NVM_TILED                                0x00010000
//#   define NV_DMA_TARGET_PCI                                      0x00020000
//#   define NV_DMA_TARGET_AGP                                      0x00030000
////#define NV_DMA_ADJUST                                   0xfff00000
////#define NV_DMA_ADDRESS                                  0xfffff000

typedef struct {
    const char *            name;
    uint32_t                bytes_per_pixel;
    int                     linear;
    GLint                   gl_internal_format;
    GLenum                  gl_format;
    GLenum                  gl_type;
    GLenum                  gl_swizzle_mask[4];
} nv2a_pgraph_color;

static const nv2a_pgraph_color const nv2a_colors[] = {
#define NV2A_COLOR1(a,b,c,d,e,f) \
        NV2A_COLOR2(a,b,c,d,e,f,0,0,0,0)
#define NV2A_COLOR2(a,b,c,d,e,f,g,h,i,j) \
        [NV_097_SET_TEXTURE_FORMAT_COLOR_ ## a] = { #a, b, c, d, e, f, { g, h, i, j } }
    /*  0 */ NV2A_COLOR2(SZ_Y8,
                1, 0, GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_RED, GL_RED, GL_RED, GL_ONE),
    /*  1 */ NV2A_COLOR2(SZ_AY8,
                1, 0, GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_RED, GL_RED, GL_RED, GL_RED),
    /*  2 */ NV2A_COLOR1(SZ_A1R5G5B5,
                2, 0, GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV),
    /*  3 */ NV2A_COLOR1(SZ_X1R5G5B5,
                2, 0, GL_RGB5, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV),
    /*  4 */ NV2A_COLOR1(SZ_A4R4G4B4,
                2, 0, GL_RGBA4, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV),
    /*  5 */ NV2A_COLOR1(SZ_R5G6B5,
                2, 0, GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5),
    /*  6 */ NV2A_COLOR1(SZ_A8R8G8B8,
                4, 0, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV),
    /*  7 */ NV2A_COLOR1(SZ_X8R8G8B8,
                4, 0, GL_RGB8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV),
    /* 11 */ NV2A_COLOR1(SZ_I8_A8R8G8B8,
                1, 0, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV),
    /* 12 */ NV2A_COLOR1(L_DXT1_A1R5G5B5,
                4, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 0, GL_RGBA),
    /* 14 */ NV2A_COLOR1(L_DXT23_A8R8G8B8,
                4, 0, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 0, GL_RGBA),
    /* 15 */ NV2A_COLOR1(L_DXT45_A8R8G8B8,
                4, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 0, GL_RGBA),
    /* 16 */ NV2A_COLOR1(LU_IMAGE_A1R5G5B5,
                2, 1, GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV),
    /* 17 */ NV2A_COLOR1(LU_IMAGE_R5G6B5,
                2, 1, GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5),
    /* 18 */ NV2A_COLOR1(LU_IMAGE_A8R8G8B8,
                4, 1, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV),
    /* 19 */ NV2A_COLOR2(LU_IMAGE_Y8,
                1, 1, GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_RED, GL_RED, GL_RED, GL_ONE),
    /* 20 */ // LU_IMAGE_SY8
    /* 21 */ // LU_IMAGE_X7SY9
    /* 22 */ // LU_IMAGE_R8B8
    /* 23 */ // LU_IMAGE_G8B8
    /* 24 */ // LU_IMAGE_SG8SB8
    /* 25 */ NV2A_COLOR2(SZ_A8,
                1, 0, GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_ONE, GL_ONE, GL_ONE, GL_RED),
    /* 26 */ NV2A_COLOR2(SZ_A8Y8,
                2, 0, GL_RG8, GL_RG, GL_UNSIGNED_BYTE, GL_GREEN, GL_GREEN, GL_GREEN, GL_RED),
    /* 27 */ NV2A_COLOR2(LU_IMAGE_AY8,
                1, 1, GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_RED, GL_RED, GL_RED, GL_RED),
    /* 28 */ NV2A_COLOR1(LU_IMAGE_X1R5G5B5,
                2, 1, GL_RGB5, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV),
    /* 29 */ NV2A_COLOR1(LU_IMAGE_A4R4G4B4,
                2, 0, GL_RGBA4, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV),
    /* 30 */ NV2A_COLOR1(LU_IMAGE_X8R8G8B8,
                4, 1, GL_RGB8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV),
    /* 31 */ NV2A_COLOR2(LU_IMAGE_A8,
                1, 1, GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_ONE, GL_ONE, GL_ONE, GL_RED),
    /* 32 */ NV2A_COLOR2(LU_IMAGE_A8Y8,
                2, 1, GL_RG8, GL_RG, GL_UNSIGNED_BYTE, GL_GREEN, GL_GREEN, GL_GREEN, GL_RED),
    /* 36 */ NV2A_COLOR1(LC_IMAGE_CR8YB8CB8YA8,
                2, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV),//TODO format conversion ?
    /* 37 */ // LC_IMAGE_YB8CR8YA8CB8
    /* 38 */ // LU_IMAGE_A8CR8CB8Y8
    /* 39 */ NV2A_COLOR1(SZ_R6G5B5,
                2, 0, GL_RGB8_SNORM, GL_RGB, GL_BYTE),//FIXME signed?
    /* 40 */ NV2A_COLOR2(SZ_G8B8,
                2, 0, GL_RG8_SNORM, GL_RG, GL_BYTE, GL_ZERO, GL_RED, GL_GREEN, GL_ONE),//FIXME signed?
    /* 41 */ NV2A_COLOR2(SZ_R8B8,
                2, 0, GL_RG8_SNORM, GL_RG, GL_BYTE, GL_RED, GL_ZERO, GL_GREEN, GL_ONE),//FIXME signed?
    /* 42 */ NV2A_COLOR1(SZ_DEPTH_X8_Y24_FIXED,
                4, 1, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8),//TODO format conversion
    /* 43 */ // SZ_DEPTH_X8_Y24_FLOAT
    /* 44 */ // SZ_DEPTH_Y16_FIXED
    /* 45 */ // SZ_DEPTH_Y16_FLOAT
    /* 46 */ // LU_IMAGE_DEPTH_X8_Y24_FIXED
    /* 47 */ // LU_IMAGE_DEPTH_X8_Y24_FLOAT
    /* 48 */ NV2A_COLOR1(LU_IMAGE_DEPTH_Y16_FIXED,
                2, 1, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT),//TODO format conversion
    /* 49 */ // LU_IMAGE_DEPTH_Y16_FLOAT
    /* 50 */ // SZ_Y16
    /* 51 */ // SZ_YB_16_YA_16
    /* 52 */ // LC_IMAGE_A4V6YB6A4U6YA6
    /* 53 */ NV2A_COLOR2(LU_IMAGE_Y16,
                2, 1, GL_R16, GL_RED, GL_UNSIGNED_SHORT, GL_RED, GL_RED, GL_RED, GL_ONE),//TODO format conversion
    /* 54 */ // LU_IMAGE_YB16YA16
    /* 55 */ // LU_IMAGE_R6G5B5
    /* 56 */ // SZ_R5G5B5A1
    /* 57 */ // SZ_R4G4B4A4
    /* 58 */ NV2A_COLOR1(SZ_A8B8G8R8,
                4, 0, GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV),//TODO format conversion
    /* 59 */ // SZ_B8G8R8A8
    /* 60 */ NV2A_COLOR1(SZ_R8G8B8A8,
                4, 0, GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8),//TODO format conversion
    /* 61 */ // LU_IMAGE_R5G5B5A1
    /* 62 */ // LU_IMAGE_R4G4B4A4
    /* 63 */ NV2A_COLOR1(LU_IMAGE_A8B8G8R8,
                4, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV),//TODO format conversion
    /* 64 */ NV2A_COLOR1(LU_IMAGE_B8G8R8A8,
                4, 1, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8),//TODO format conversion
    /* 65 */ NV2A_COLOR1(LU_IMAGE_R8G8B8A8,
                4, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8),//TODO format conversion
#undef NV2A_COLOR2
#undef NV2A_COLOR1
};

#endif

