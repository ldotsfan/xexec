
#ifndef NV2A_H
#define NV2A_H

#define REG8(x)                     *(uint8_t *)((x))
#define REG16(x)                    *(uint16_t *)((x))
#define REG32(x)                    *(uint32_t *)((x))

#define NV2A_REGADDR(x,y,z)         ((x) + nv2a_blocks[y].offset + y ## _ ## z)

#define NV2A_MASK_GET(x,y)          ((x) & (y))
#define NV2A_MASK_SET(x,y)          ((x) |= (y))
#define NV2A_MASK_UNSET(x,y)        ((x) &= ~(y))

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
        NV2A_MASK_SET(NV2A_REG32((x), y, z), NV2A_MASK_GET((v), y ## _ ## z ## _ ## m))

#define NV2A_REG32_MASK_BITSHIFT_GET(x,y,z,m) \
        (NV2A_REG32_MASK_GET(x,y,z,m) >> y ## _ ## z ## _ ## m ## __BITSHIFT)

#define NV2A_MAX_CHANNELS           32
#define NV2A_MAX_SUBCHANNELS        8

enum {  /* NV_PMC */    /* 0 */     /* card master control */
        NV_PMC_BOOT_0                           = 0x00000000,
        NV_PMC_INTR_0                           = 0x00000100,
#define NV_PMC_INTR_0_PFIFO                             (1 << 8)        /* 0x00000100 */
#define NV_PMC_INTR_0_PGRAPH                            (1 << 12)       /* 0x00001000 */
#define NV_PMC_INTR_0_PCRTC                             (1 << 24)       /* 0x01000000 */
#define NV_PMC_INTR_0_PBUS                              (1 << 28)       /* 0x10000000 */
#define NV_PMC_INTR_0_SOFTWARE                          (1 << 31)       /* 0x80000000 */
        NV_PMC_INTR_EN_0                        = 0x00000140,
#define NV_PMC_INTR_EN_0_INTA_HARDWARE                  0x00000001
#define NV_PMC_INTR_EN_0_INTA_SOFTWARE                  0x00000002
};
/* NV_PBUS */           /* 1 */     /* bus control */
enum {  /* NV_PFIFO */  /* 2 */     /* MMIO and DMA FIFO submission to PGRAPH and VPE */
        NV_PFIFO_INTR_0                         = 0x00000100,
#define NV_PFIFO_INTR_0_CACHE_ERROR                     (1 << 0)        /* 0x00000001 */
#define NV_PFIFO_INTR_0_RUNOUT                          (1 << 4)        /* 0x00000010 */
#define NV_PFIFO_INTR_0_RUNOUT_OVERFLOW                 (1 << 8)        /* 0x00000100 */
#define NV_PFIFO_INTR_0_DMA_PUSHER                      (1 << 12)       /* 0x00001000 */
#define NV_PFIFO_INTR_0_DMA_PT                          (1 << 16)       /* 0x00010000 */
#define NV_PFIFO_INTR_0_SEMAPHORE                       (1 << 20)       /* 0x00100000 */
#define NV_PFIFO_INTR_0_ACQUIRE_TIMEOUT                 (1 << 24)       /* 0x01000000 */
        NV_PFIFO_INTR_EN_0                      = 0x00000140,
#define NV_PFIFO_INTR_EN_0_CACHE_ERROR                  (1 << 0)        /* 0x00000001 */
#define NV_PFIFO_INTR_EN_0_RUNOUT                       (1 << 4)        /* 0x00000010 */
#define NV_PFIFO_INTR_EN_0_RUNOUT_OVERFLOW              (1 << 8)        /* 0x00000100 */
#define NV_PFIFO_INTR_EN_0_DMA_PUSHER                   (1 << 12)       /* 0x00001000 */
#define NV_PFIFO_INTR_EN_0_DMA_PT                       (1 << 16)       /* 0x00010000 */
#define NV_PFIFO_INTR_EN_0_SEMAPHORE                    (1 << 20)       /* 0x00100000 */
#define NV_PFIFO_INTR_EN_0_ACQUIRE_TIMEOUT              (1 << 24)       /* 0x01000000 */
        NV_PFIFO_RAMHT                          = 0x00000210,
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
        NV_PFIFO_RUNOUT_STATUS                  = 0x00000400,
#define NV_PFIFO_RUNOUT_STATUS_RANOUT_TRUE              (1 << 0)        /* 0x00000001 */
#define NV_PFIFO_RUNOUT_STATUS_LOW_MARK_EMPTY           (1 << 4)        /* 0x00000010 */
#define NV_PFIFO_RUNOUT_STATUS_HIGH_MARK_FULL           (1 << 8)        /* 0x00000100 */
        NV_PFIFO_MODE                           = 0x00000504,
//#define NV_PFIFO_MODE_DMA                               0xffffffff
//#define NV_PFIFO_MODE_PIO                               0x00000000
        NV_PFIFO_CACHE1_PUSH0                   = 0x00001200,
#define NV_PFIFO_CACHE1_PUSH0_ACCESS_ENABLED            (1 << 0)        /* 0x00000001 */    /* push access enabled */
        NV_PFIFO_CACHE1_PUSH1                   = 0x00001204,
#define NV_PFIFO_CACHE1_PUSH1_CHID                      0x0000001f
#define NV_PFIFO_CACHE1_PUSH1_MODE_DMA                  0x00000100
        NV_PFIFO_CACHE1_STATUS                  = 0x00001214,
#define NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY           (1 << 4)        /* 0x00000010 */
#define NV_PFIFO_CACHE1_STATUS_HIGH_MARK_FULL           (1 << 8)        /* 0x00000100 */
        NV_PFIFO_CACHE1_DMA_PUSH                = 0x00001220,
#define NV_PFIFO_CACHE1_DMA_PUSH_ACCESS_ENABLED         (1 << 0)        /* 0x00000001 */    /* push access enabled */
#define NV_PFIFO_CACHE1_DMA_PUSH_STATE_BUSY             (1 << 4)        /* 0x00000010 */    /* push state busy */
#define NV_PFIFO_CACHE1_DMA_PUSH_BUFFER_EMPTY           (1 << 8)        /* 0x00000100 */    /* push buffer empty */
#define NV_PFIFO_CACHE1_DMA_PUSH_STATUS_SUSPENDED       (1 << 12)       /* 0x00001000 */    /* push status suspended */
#define NV_PFIFO_CACHE1_DMA_PUSH_ACQUIRE_PENDING        (1 << 16)       /* 0x00010000 */    /* push acquire pending */
        NV_PFIFO_CACHE1_DMA_STATE               = 0x00001228,
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
        NV_PFIFO_CACHE1_DMA_INSTANCE            = 0x0000122c,
#define NV_PFIFO_CACHE1_DMA_INSTANCE_ADDRESS            0x0000ffff
        NV_PFIFO_CACHE1_DMA_PUT                 = 0x00001240,
#define NV_PFIFO_CACHE1_DMA_PUT_OFFSET                  0xfffffffc
        NV_PFIFO_CACHE1_DMA_GET                 = 0x00001244,
#define NV_PFIFO_CACHE1_DMA_GET_OFFSET                  0xfffffffc
        NV_PFIFO_CACHE1_DMA_SUBROUTINE          = 0x0000124c,
#define NV_PFIFO_CACHE1_DMA_SUBROUTINE_RETURN_OFFSET    0xfffffffc
#define NV_PFIFO_CACHE1_DMA_SUBROUTINE_STATE_ACTIVE     (1 << 0)        /* 0x00000001 */
        NV_PFIFO_CACHE1_DMA_DCOUNT              = 0x000012a0,
//#define NV_PFIFO_CACHE1_DMA_DCOUNT_VALUE                0x00001ffc
        NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW      = 0x000012a4,
#define NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET       0xfffffffc
        NV_PFIFO_CACHE1_DMA_RSVD_SHADOW         = 0x000012a8,
        NV_PFIFO_CACHE1_DMA_DATA_SHADOW         = 0x000012ac,
};
/* NV_PFIFO_CACHE */    /* 3 */
/* NV_PRMA */           /* 4 */     /* access to BAR0/BAR1 from real mode */
/* NV_PVIDEO */         /* 5 */     /* video overlay */
enum { /* NV_PTIMER */  /* 6 */     /* time measurement and time-based alarms */
        NV_PTIMER_INTR_0                        = 0x00000100,
#define NV_PTIMER_INTR_0_ALARM                          (1 << 0)        /* 0x00000001 */
        NV_PTIMER_INTR_EN_0                     = 0x00000140,
#define NV_PTIMER_INTR_EN_0_ALARM                       (1 << 0)        /* 0x00000001 */
};
/* NV_PCOUNTER */       /* 7 */     /* performance monitoring counters */
/* NV_PVPE */           /* 8 */     /* MPEG2 decoding engine */
/* NV_PTV */            /* 9 */     /* TV encoder */
/* NV_PRMFB */          /* 10 */    /* aliases VGA memory window */
/* NV_PRMVIO */         /* 11 */    /* aliases VGA sequencer and graphics controller registers */
enum {  /* NV_PFB */    /* 12 */    /* memory interface */
        NV_PFB_CFG0                             = 0x00000200,
#define NV_PFB_CFG0_PART_4                              0x00000003
        NV_PFB_CSTATUS                          = 0x0000020c,
        NV_PFB_WBC                              = 0x00000410,
//#define NV_PFB_WBC_FLUSH_PENDING                    (1 << 16)       /* 0x00010000 */
};
/* NV_PSTRAPS */        /* 13 */    /* straps readout / override */
enum {  /* NV_PGRAPH */ /* 14 */    /* accelerated 2d/3d drawing engine */
        NV_PGRAPH_INTR                          = 0x00000100,
#define NV_PGRAPH_INTR_NOTIFY                           (1 << 0)        /* 0x00000001 */
#define NV_PGRAPH_INTR_MISSING_HW                       (1 << 4)        /* 0x00000010 */
#define NV_PGRAPH_INTR_TLB_PRESENT_DMA_R                (1 << 6)        /* 0x00000040 */
#define NV_PGRAPH_INTR_TLB_PRESENT_DMA_W                (1 << 7)        /* 0x00000080 */
#define NV_PGRAPH_INTR_TLB_PRESENT_TEX_A                (1 << 8)        /* 0x00000100 */
#define NV_PGRAPH_INTR_TLB_PRESENT_TEX_B                (1 << 9)        /* 0x00000200 */
#define NV_PGRAPH_INTR_TLB_PRESENT_VTX                  (1 << 10)       /* 0x00000400 */
#define NV_PGRAPH_INTR_CONTEXT_SWITCH                   (1 << 12)       /* 0x00001000 */
#define NV_PGRAPH_INTR_STATE3D                          (1 << 13)       /* 0x00002000 */
#define NV_PGRAPH_INTR_BUFFER_NOTIFY                    (1 << 16)       /* 0x00010000 */
#define NV_PGRAPH_INTR_ERROR                            (1 << 20)       /* 0x00100000 */
#define NV_PGRAPH_INTR_SINGLE_STEP                      (1 << 24)       /* 0x01000000 */
        NV_PGRAPH_INTR_EN                       = 0x00000140,
#define NV_PGRAPH_INTR_EN_NOTIFY                        (1 << 0)        /* 0x00000001 */
#define NV_PGRAPH_INTR_EN_MISSING_HW                    (1 << 4)        /* 0x00000010 */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_DMA_R             (1 << 6)        /* 0x00000040 */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_DMA_W             (1 << 7)        /* 0x00000080 */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_TEX_A             (1 << 8)        /* 0x00000100 */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_TEX_B             (1 << 9)        /* 0x00000200 */
#define NV_PGRAPH_INTR_EN_TLB_PRESENT_VTX               (1 << 10)       /* 0x00000400 */
#define NV_PGRAPH_INTR_EN_CONTEXT_SWITCH                (1 << 12)       /* 0x00001000 */
#define NV_PGRAPH_INTR_EN_STATE3D                       (1 << 13)       /* 0x00002000 */
#define NV_PGRAPH_INTR_EN_BUFFER_NOTIFY                 (1 << 16)       /* 0x00010000 */
#define NV_PGRAPH_INTR_EN_ERROR                         (1 << 20)       /* 0x00100000 */
#define NV_PGRAPH_INTR_EN_SINGLE_STEP                   (1 << 24)       /* 0x01000000 */
};
enum {  /* NV_PCRTC */  /* 15 */    /* more CRTC controls */
        NV_PCRTC_INTR_0                         = 0x00000100,
#define NV_PCRTC_INTR_0_VBLANK                          (1 << 0)        /* 0x00000001 */
        NV_PCRTC_INTR_EN_0                      = 0x00000140,
#define NV_PCRTC_INTR_EN_0_VBLANK                       (1 << 0)        /* 0x00000001 */
};
/* NV_PRMCIO */         /* 16 */    /* aliases VGA CRTC and attribute controller registers */
/* NV_PRAMDAC */        /* 17 */    /* RAMDAC, cursor, and PLL control */
/* NV_PRMDIO */         /* 18 */    /* aliases VGA palette registers */
enum {  /* NV_PRAMIN */ /* 19 */    /* RAMIN access */
        NV_PRAMIN_BASE                          = 0x00000000,
};
enum {  /* NV_USER */   /* 20 */    /* PFIFO MMIO and DMA submission area */
#define NV_USER_ADDR_REG(x)                             (((x) & 0x0000ffff) >>  0)
#define NV_USER_ADDR_CHID(x)                            (((x) & 0xffff0000) >> 16)
        NV_USER_DMA_PUT                         = 0x00000040,
#define NV_USER_DMA_PUT_OFFSET                          0xfffffffc
        NV_USER_DMA_GET                         = 0x00000044,
#define NV_USER_DMA_GET_OFFSET                          0xfffffffc
        NV_USER_REF                             = 0x00000048,
};

#define NV_DMA_CLASS                                    0x00000fff
//#define NV_DMA_PAGE_TABLE                                      (1 << 12)
//#define NV_DMA_PAGE_ENTRY                                      (1 << 13)
//#define NV_DMA_FLAGS_ACCESS                                    (1 << 14)
//#define NV_DMA_FLAGS_MAPPING_COHERENCY                         (1 << 15)
#define NV_DMA_TARGET                                   0x00030000
//#   define NV_DMA_TARGET_NVM                                      0x00000000
//#   define NV_DMA_TARGET_NVM_TILED                                0x00010000
//#   define NV_DMA_TARGET_PCI                                      0x00020000
//#   define NV_DMA_TARGET_AGP                                      0x00030000
#define NV_DMA_ADJUST                                   0xfff00000
#define NV_DMA_ADDRESS                                  0xfffff000











































#endif

