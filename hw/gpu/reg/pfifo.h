
enum {
        NV_PFIFO_DELAY_0                            = 0x00000040,
#define NV_PFIFO_DELAY_0_WAIT_RETRY                                             0x000003ff
#define NV_PFIFO_DELAY_0_WAIT_RETRY__BITSHIFT                                   0
#define NV_PFIFO_DELAY_0_WAIT_RETRY_0                                           0
        NV_PFIFO_DELAY_1                            = 0x00000054,
#define NV_PFIFO_DELAY_1_ACQUIRE_WAIT                                           0x000003ff
#define NV_PFIFO_DELAY_1_ACQUIRE_WAIT__BITSHIFT                                 0
#define NV_PFIFO_DELAY_1_ACQUIRE_WAIT_2                                         2
        NV_PFIFO_DMA_TIMESLICE                      = 0x00000044,
#define NV_PFIFO_DMA_TIMESLICE_SELECT                                           0x0001ffff
#define NV_PFIFO_DMA_TIMESLICE_SELECT__BITSHIFT                                 0
#define NV_PFIFO_DMA_TIMESLICE_SELECT_1                                         0
#define NV_PFIFO_DMA_TIMESLICE_SELECT_16K                                       0x00003fff
#define NV_PFIFO_DMA_TIMESLICE_SELECT_32K                                       0x00007fff
#define NV_PFIFO_DMA_TIMESLICE_SELECT_64K                                       0x0000ffff
#define NV_PFIFO_DMA_TIMESLICE_SELECT_128K                                      0x0001ffff
#define NV_PFIFO_DMA_TIMESLICE_TIMEOUT                                          0x01000000
#define NV_PFIFO_DMA_TIMESLICE_TIMEOUT__BITSHIFT                                24
#define NV_PFIFO_DMA_TIMESLICE_TIMEOUT_DISABLED                                 0
#define NV_PFIFO_DMA_TIMESLICE_TIMEOUT_ENABLED                                  1
        NV_PFIFO_PIO_TIMESLICE                      = 0x00000048,
#define NV_PFIFO_PIO_TIMESLICE_SELECT                                           0x0001ffff
#define NV_PFIFO_PIO_TIMESLICE_SELECT__BITSHIFT                                 0
#define NV_PFIFO_PIO_TIMESLICE_SELECT_1                                         0
#define NV_PFIFO_PIO_TIMESLICE_SELECT_16K                                       0x00003fff
#define NV_PFIFO_PIO_TIMESLICE_SELECT_32K                                       0x00007fff
#define NV_PFIFO_PIO_TIMESLICE_SELECT_64K                                       0x0000ffff
#define NV_PFIFO_PIO_TIMESLICE_SELECT_128K                                      0x0001ffff
#define NV_PFIFO_PIO_TIMESLICE_TIMEOUT                                          0x01000000
#define NV_PFIFO_PIO_TIMESLICE_TIMEOUT__BITSHIFT                                24
#define NV_PFIFO_PIO_TIMESLICE_TIMEOUT_DISABLED                                 0
#define NV_PFIFO_PIO_TIMESLICE_TIMEOUT_ENABLED                                  1
        NV_PFIFO_TIMESLICE                          = 0x0000004c,
#define NV_PFIFO_TIMESLICE_TIMER                                                0x0003ffff
#define NV_PFIFO_TIMESLICE_TIMER__BITSHIFT                                      0
#define NV_PFIFO_TIMESLICE_TIMER_EXPIRED                                        0x0003ffff
        NV_PFIFO_NEXT_CHANNEL                       = 0x00000050,
#define NV_PFIFO_NEXT_CHANNEL_CHID                                              0x0000001f
#define NV_PFIFO_NEXT_CHANNEL_CHID__BITSHIFT                                    0
#define NV_PFIFO_NEXT_CHANNEL_MODE                                              0x00000100
#define NV_PFIFO_NEXT_CHANNEL_MODE__BITSHIFT                                    8
#define NV_PFIFO_NEXT_CHANNEL_MODE_PIO                                          0
#define NV_PFIFO_NEXT_CHANNEL_MODE_DMA                                          1
#define NV_PFIFO_NEXT_CHANNEL_SWITCH                                            0x00001000
#define NV_PFIFO_NEXT_CHANNEL_SWITCH__BITSHIFT                                  12
#define NV_PFIFO_NEXT_CHANNEL_SWITCH_NOT_PENDING                                0
#define NV_PFIFO_NEXT_CHANNEL_SWITCH_PENDING                                    1
        NV_PFIFO_DEBUG_0                            = 0x00000080,
#define NV_PFIFO_DEBUG_0_CACHE_ERROR0                                           0x00000001
#define NV_PFIFO_DEBUG_0_CACHE_ERROR0__BITSHIFT                                 0
#define NV_PFIFO_DEBUG_0_CACHE_ERROR0_NOT_PENDING                               0
#define NV_PFIFO_DEBUG_0_CACHE_ERROR0_PENDING                                   1
#define NV_PFIFO_DEBUG_0_CACHE_ERROR1                                           0x00000010
#define NV_PFIFO_DEBUG_0_CACHE_ERROR1__BITSHIFT                                 4
#define NV_PFIFO_DEBUG_0_CACHE_ERROR1_NOT_PENDING                               0
#define NV_PFIFO_DEBUG_0_CACHE_ERROR1_PENDING                                   1
        NV_PFIFO_INTR_0                             = 0x00000100,
#define NV_PFIFO_INTR_0_CACHE_ERROR                                             0x00000001
#define NV_PFIFO_INTR_0_CACHE_ERROR__BITSHIFT                                   0
#define NV_PFIFO_INTR_0_CACHE_ERROR_NOT_PENDING                                 0
#define NV_PFIFO_INTR_0_CACHE_ERROR_PENDING                                     1
#define NV_PFIFO_INTR_0_CACHE_ERROR_RESET                                       1
#define NV_PFIFO_INTR_0_RUNOUT                                                  0x00000010
#define NV_PFIFO_INTR_0_RUNOUT__BITSHIFT                                        4
#define NV_PFIFO_INTR_0_RUNOUT_NOT_PENDING                                      0
#define NV_PFIFO_INTR_0_RUNOUT_PENDING                                          1
#define NV_PFIFO_INTR_0_RUNOUT_RESET                                            1
#define NV_PFIFO_INTR_0_RUNOUT_OVERFLOW                                         0x00000100
#define NV_PFIFO_INTR_0_RUNOUT_OVERFLOW__BITSHIFT                               8
#define NV_PFIFO_INTR_0_RUNOUT_OVERFLOW_NOT_PENDING                             0
#define NV_PFIFO_INTR_0_RUNOUT_OVERFLOW_PENDING                                 1
#define NV_PFIFO_INTR_0_RUNOUT_OVERFLOW_RESET                                   1
#define NV_PFIFO_INTR_0_DMA_PUSHER                                              0x00001000
#define NV_PFIFO_INTR_0_DMA_PUSHER__BITSHIFT                                    12
#define NV_PFIFO_INTR_0_DMA_PUSHER_NOT_PENDING                                  0
#define NV_PFIFO_INTR_0_DMA_PUSHER_PENDING                                      1
#define NV_PFIFO_INTR_0_DMA_PUSHER_RESET                                        1
#define NV_PFIFO_INTR_0_DMA_PT                                                  0x00010000
#define NV_PFIFO_INTR_0_DMA_PT__BITSHIFT                                        16
#define NV_PFIFO_INTR_0_DMA_PT_NOT_PENDING                                      0
#define NV_PFIFO_INTR_0_DMA_PT_PENDING                                          1
#define NV_PFIFO_INTR_0_DMA_PT_RESET                                            1
#define NV_PFIFO_INTR_0_SEMAPHORE                                               0x00100000
#define NV_PFIFO_INTR_0_SEMAPHORE__BITSHIFT                                     20
#define NV_PFIFO_INTR_0_SEMAPHORE_NOT_PENDING                                   0
#define NV_PFIFO_INTR_0_SEMAPHORE_PENDING                                       1
#define NV_PFIFO_INTR_0_SEMAPHORE_RESET                                         1
#define NV_PFIFO_INTR_0_ACQUIRE_TIMEOUT                                         0x01000000
#define NV_PFIFO_INTR_0_ACQUIRE_TIMEOUT__BITSHIFT                               24
#define NV_PFIFO_INTR_0_ACQUIRE_TIMEOUT_NOT_PENDING                             0
#define NV_PFIFO_INTR_0_ACQUIRE_TIMEOUT_PENDING                                 1
#define NV_PFIFO_INTR_0_ACQUIRE_TIMEOUT_RESET                                   1
        NV_PFIFO_INTR_EN_0                          = 0x00000140,
#define NV_PFIFO_INTR_EN_0_CACHE_ERROR                                          0x00000001
#define NV_PFIFO_INTR_EN_0_CACHE_ERROR__BITSHIFT                                0
#define NV_PFIFO_INTR_EN_0_CACHE_ERROR_DISABLED                                 0
#define NV_PFIFO_INTR_EN_0_CACHE_ERROR_ENABLED                                  1
#define NV_PFIFO_INTR_EN_0_RUNOUT                                               0x00000010
#define NV_PFIFO_INTR_EN_0_RUNOUT__BITSHIFT                                     4
#define NV_PFIFO_INTR_EN_0_RUNOUT_DISABLED                                      0
#define NV_PFIFO_INTR_EN_0_RUNOUT_ENABLED                                       1
#define NV_PFIFO_INTR_EN_0_RUNOUT_OVERFLOW                                      0x00000100
#define NV_PFIFO_INTR_EN_0_RUNOUT_OVERFLOW__BITSHIFT                            8
#define NV_PFIFO_INTR_EN_0_RUNOUT_OVERFLOW_DISABLED                             0
#define NV_PFIFO_INTR_EN_0_RUNOUT_OVERFLOW_ENABLED                              1
#define NV_PFIFO_INTR_EN_0_DMA_PUSHER                                           0x00001000
#define NV_PFIFO_INTR_EN_0_DMA_PUSHER__BITSHIFT                                 12
#define NV_PFIFO_INTR_EN_0_DMA_PUSHER_DISABLED                                  0
#define NV_PFIFO_INTR_EN_0_DMA_PUSHER_ENABLED                                   1
#define NV_PFIFO_INTR_EN_0_DMA_PT                                               0x00010000
#define NV_PFIFO_INTR_EN_0_DMA_PT__BITSHIFT                                     16
#define NV_PFIFO_INTR_EN_0_DMA_PT_DISABLED                                      0
#define NV_PFIFO_INTR_EN_0_DMA_PT_ENABLED                                       1
#define NV_PFIFO_INTR_EN_0_SEMAPHORE                                            0x00100000
#define NV_PFIFO_INTR_EN_0_SEMAPHORE__BITSHIFT                                  20
#define NV_PFIFO_INTR_EN_0_SEMAPHORE_DISABLED                                   0
#define NV_PFIFO_INTR_EN_0_SEMAPHORE_ENABLED                                    1
#define NV_PFIFO_INTR_EN_0_ACQUIRE_TIMEOUT                                      0x01000000
#define NV_PFIFO_INTR_EN_0_ACQUIRE_TIMEOUT__BITSHIFT                            24
#define NV_PFIFO_INTR_EN_0_ACQUIRE_TIMEOUT_DISABLED                             0
#define NV_PFIFO_INTR_EN_0_ACQUIRE_TIMEOUT_ENABLED                              1
        NV_PFIFO_RAMHT                              = 0x00000210,
#define NV_PFIFO_RAMHT_BASE_ADDRESS                                             0x000001f0
#define NV_PFIFO_RAMHT_BASE_ADDRESS__BITSHIFT                                   4
#define NV_PFIFO_RAMHT_BASE_ADDRESS_10000                                       0x00000010
#define NV_PFIFO_RAMHT_SIZE                                                     0x00030000
#define NV_PFIFO_RAMHT_SIZE__BITSHIFT                                           16
#define NV_PFIFO_RAMHT_SIZE_4K                                                  0
#define NV_PFIFO_RAMHT_SIZE_8K                                                  1
#define NV_PFIFO_RAMHT_SIZE_16K                                                 2
#define NV_PFIFO_RAMHT_SIZE_32K                                                 3
#define NV_PFIFO_RAMHT_SEARCH                                                   0x03000000
#define NV_PFIFO_RAMHT_SEARCH__BITSHIFT                                         24
#define NV_PFIFO_RAMHT_SEARCH_16                                                0
#define NV_PFIFO_RAMHT_SEARCH_32                                                1
#define NV_PFIFO_RAMHT_SEARCH_64                                                2
#define NV_PFIFO_RAMHT_SEARCH_128                                               3
        NV_PFIFO_RAMFC                              = 0x00000214,
#define NV_PFIFO_RAMFC_BASE_ADDRESS                                             0x000001fc
#define NV_PFIFO_RAMFC_BASE_ADDRESS__BITSHIFT                                   2
#define NV_PFIFO_RAMFC_BASE_ADDRESS_11000                                       0x00000044
#define NV_PFIFO_RAMFC_SIZE                                                     0x00010000
#define NV_PFIFO_RAMFC_SIZE__BITSHIFT                                           16
#define NV_PFIFO_RAMFC_SIZE_1K                                                  0
#define NV_PFIFO_RAMFC_SIZE_2K                                                  1
#define NV_PFIFO_RAMFC_BASE_ADDRESS2                                            0x00fe0000
#define NV_PFIFO_RAMFC_BASE_ADDRESS2__BITSHIFT                                  17
#define NV_PFIFO_RAMFC_BASE_ADDRESS_11400                                       0x00000045
        NV_PFIFO_RAMRO                              = 0x00000218,
#define NV_PFIFO_RAMRO_BASE_ADDRESS                                             0x000001fe
#define NV_PFIFO_RAMRO_BASE_ADDRESS__BITSHIFT                                   1
#define NV_PFIFO_RAMRO_BASE_ADDRESS_11500                                       0x0000008b
#define NV_PFIFO_RAMRO_BASE_ADDRESS_11400                                       0x0000008a
#define NV_PFIFO_RAMRO_BASE_ADDRESS_11200                                       0x00000089
#define NV_PFIFO_RAMRO_BASE_ADDRESS_11800                                       0x0000008c
#define NV_PFIFO_RAMRO_BASE_ADDRESS_12000                                       0x00000090
#define NV_PFIFO_RAMRO_SIZE                                                     0x00010000
#define NV_PFIFO_RAMRO_SIZE__BITSHIFT                                           16
#define NV_PFIFO_RAMRO_SIZE_512                                                 0
#define NV_PFIFO_RAMRO_SIZE_8K                                                  1
        NV_PFIFO_CACHES                             = 0x00000500,
#define NV_PFIFO_CACHES_REASSIGN                                                0x00000001
#define NV_PFIFO_CACHES_REASSIGN__BITSHIFT                                      0
#define NV_PFIFO_CACHES_REASSIGN_DISABLED                                       0
#define NV_PFIFO_CACHES_REASSIGN_ENABLED                                        1
#define NV_PFIFO_CACHES_DMA_SUSPEND                                             0x00000010
#define NV_PFIFO_CACHES_DMA_SUSPEND__BITSHIFT                                   4
#define NV_PFIFO_CACHES_DMA_SUSPEND_IDLE                                        0
#define NV_PFIFO_CACHES_DMA_SUSPEND_BUSY                                        1
        NV_PFIFO_MODE                               = 0x00000504,
#define NV_PFIFO_MODE_CHANNEL_0                                                 0x00000001
#define NV_PFIFO_MODE_CHANNEL_0__BITSHIFT                                       0
#define NV_PFIFO_MODE_CHANNEL_0_PIO                                             0
#define NV_PFIFO_MODE_CHANNEL_0_DMA                                             1
#define NV_PFIFO_MODE_CHANNEL_1                                                 0x00000002
#define NV_PFIFO_MODE_CHANNEL_1__BITSHIFT                                       1
#define NV_PFIFO_MODE_CHANNEL_1_PIO                                             0
#define NV_PFIFO_MODE_CHANNEL_1_DMA                                             1
#define NV_PFIFO_MODE_CHANNEL_2                                                 0x00000004
#define NV_PFIFO_MODE_CHANNEL_2__BITSHIFT                                       2
#define NV_PFIFO_MODE_CHANNEL_2_PIO                                             0
#define NV_PFIFO_MODE_CHANNEL_2_DMA                                             1
#define NV_PFIFO_MODE_CHANNEL_3                                                 0x00000008
#define NV_PFIFO_MODE_CHANNEL_3__BITSHIFT                                       3
#define NV_PFIFO_MODE_CHANNEL_3_PIO                                             0
#define NV_PFIFO_MODE_CHANNEL_3_DMA                                             1
#define NV_PFIFO_MODE_CHANNEL_4                                                 0x00000010
#define NV_PFIFO_MODE_CHANNEL_4__BITSHIFT                                       4
#define NV_PFIFO_MODE_CHANNEL_4_PIO                                             0
#define NV_PFIFO_MODE_CHANNEL_4_DMA                                             1
#define NV_PFIFO_MODE_CHANNEL_5                                                 0x00000020
#define NV_PFIFO_MODE_CHANNEL_5__BITSHIFT                                       5
#define NV_PFIFO_MODE_CHANNEL_5_PIO                                             0
#define NV_PFIFO_MODE_CHANNEL_5_DMA                                             1
#define NV_PFIFO_MODE_CHANNEL_6                                                 0x00000040
#define NV_PFIFO_MODE_CHANNEL_6__BITSHIFT                                       6
#define NV_PFIFO_MODE_CHANNEL_6_PIO                                             0
#define NV_PFIFO_MODE_CHANNEL_6_DMA                                             1
#define NV_PFIFO_MODE_CHANNEL_7                                                 0x00000080
#define NV_PFIFO_MODE_CHANNEL_7__BITSHIFT                                       7
#define NV_PFIFO_MODE_CHANNEL_7_PIO                                             0
#define NV_PFIFO_MODE_CHANNEL_7_DMA                                             1
#define NV_PFIFO_MODE_CHANNEL_8                                                 0x00000100
#define NV_PFIFO_MODE_CHANNEL_8__BITSHIFT                                       8
#define NV_PFIFO_MODE_CHANNEL_8_PIO                                             0
#define NV_PFIFO_MODE_CHANNEL_8_DMA                                             1
#define NV_PFIFO_MODE_CHANNEL_9                                                 0x00000200
#define NV_PFIFO_MODE_CHANNEL_9__BITSHIFT                                       9
#define NV_PFIFO_MODE_CHANNEL_9_PIO                                             0
#define NV_PFIFO_MODE_CHANNEL_9_DMA                                             1
#define NV_PFIFO_MODE_CHANNEL_10                                                0x00000400
#define NV_PFIFO_MODE_CHANNEL_10__BITSHIFT                                      10
#define NV_PFIFO_MODE_CHANNEL_10_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_10_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_11                                                0x00000800
#define NV_PFIFO_MODE_CHANNEL_11__BITSHIFT                                      11
#define NV_PFIFO_MODE_CHANNEL_11_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_11_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_12                                                0x00001000
#define NV_PFIFO_MODE_CHANNEL_12__BITSHIFT                                      12
#define NV_PFIFO_MODE_CHANNEL_12_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_12_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_13                                                0x00002000
#define NV_PFIFO_MODE_CHANNEL_13__BITSHIFT                                      13
#define NV_PFIFO_MODE_CHANNEL_13_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_13_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_14                                                0x00004000
#define NV_PFIFO_MODE_CHANNEL_14__BITSHIFT                                      14
#define NV_PFIFO_MODE_CHANNEL_14_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_14_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_15                                                0x00008000
#define NV_PFIFO_MODE_CHANNEL_15__BITSHIFT                                      15
#define NV_PFIFO_MODE_CHANNEL_15_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_15_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_16                                                0x00010000
#define NV_PFIFO_MODE_CHANNEL_16__BITSHIFT                                      16
#define NV_PFIFO_MODE_CHANNEL_16_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_16_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_17                                                0x00020000
#define NV_PFIFO_MODE_CHANNEL_17__BITSHIFT                                      17
#define NV_PFIFO_MODE_CHANNEL_17_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_17_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_18                                                0x00040000
#define NV_PFIFO_MODE_CHANNEL_18__BITSHIFT                                      18
#define NV_PFIFO_MODE_CHANNEL_18_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_18_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_19                                                0x00080000
#define NV_PFIFO_MODE_CHANNEL_19__BITSHIFT                                      19
#define NV_PFIFO_MODE_CHANNEL_19_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_19_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_20                                                0x00100000
#define NV_PFIFO_MODE_CHANNEL_20__BITSHIFT                                      20
#define NV_PFIFO_MODE_CHANNEL_20_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_20_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_21                                                0x00200000
#define NV_PFIFO_MODE_CHANNEL_21__BITSHIFT                                      21
#define NV_PFIFO_MODE_CHANNEL_21_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_21_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_22                                                0x00400000
#define NV_PFIFO_MODE_CHANNEL_22__BITSHIFT                                      22
#define NV_PFIFO_MODE_CHANNEL_22_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_22_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_23                                                0x00800000
#define NV_PFIFO_MODE_CHANNEL_23__BITSHIFT                                      23
#define NV_PFIFO_MODE_CHANNEL_23_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_23_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_24                                                0x01000000
#define NV_PFIFO_MODE_CHANNEL_24__BITSHIFT                                      24
#define NV_PFIFO_MODE_CHANNEL_24_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_24_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_25                                                0x02000000
#define NV_PFIFO_MODE_CHANNEL_25__BITSHIFT                                      25
#define NV_PFIFO_MODE_CHANNEL_25_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_25_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_26                                                0x04000000
#define NV_PFIFO_MODE_CHANNEL_26__BITSHIFT                                      26
#define NV_PFIFO_MODE_CHANNEL_26_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_26_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_27                                                0x08000000
#define NV_PFIFO_MODE_CHANNEL_27__BITSHIFT                                      27
#define NV_PFIFO_MODE_CHANNEL_27_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_27_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_28                                                0x10000000
#define NV_PFIFO_MODE_CHANNEL_28__BITSHIFT                                      28
#define NV_PFIFO_MODE_CHANNEL_28_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_28_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_29                                                0x20000000
#define NV_PFIFO_MODE_CHANNEL_29__BITSHIFT                                      29
#define NV_PFIFO_MODE_CHANNEL_29_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_29_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_30                                                0x40000000
#define NV_PFIFO_MODE_CHANNEL_30__BITSHIFT                                      30
#define NV_PFIFO_MODE_CHANNEL_30_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_30_DMA                                            1
#define NV_PFIFO_MODE_CHANNEL_31                                                0x80000000
#define NV_PFIFO_MODE_CHANNEL_31__BITSHIFT                                      31
#define NV_PFIFO_MODE_CHANNEL_31_PIO                                            0
#define NV_PFIFO_MODE_CHANNEL_31_DMA                                            1
        NV_PFIFO_DMA                                = 0x00000508,
#define NV_PFIFO_DMA_CHANNEL_0                                                  0x00000001
#define NV_PFIFO_DMA_CHANNEL_0__BITSHIFT                                        0
#define NV_PFIFO_DMA_CHANNEL_0_NOT_PENDING                                      0
#define NV_PFIFO_DMA_CHANNEL_0_PENDING                                          1
#define NV_PFIFO_DMA_CHANNEL_1                                                  0x00000002
#define NV_PFIFO_DMA_CHANNEL_1__BITSHIFT                                        1
#define NV_PFIFO_DMA_CHANNEL_1_NOT_PENDING                                      0
#define NV_PFIFO_DMA_CHANNEL_1_PENDING                                          1
#define NV_PFIFO_DMA_CHANNEL_2                                                  0x00000004
#define NV_PFIFO_DMA_CHANNEL_2__BITSHIFT                                        2
#define NV_PFIFO_DMA_CHANNEL_2_NOT_PENDING                                      0
#define NV_PFIFO_DMA_CHANNEL_2_PENDING                                          1
#define NV_PFIFO_DMA_CHANNEL_3                                                  0x00000008
#define NV_PFIFO_DMA_CHANNEL_3__BITSHIFT                                        3
#define NV_PFIFO_DMA_CHANNEL_3_NOT_PENDING                                      0
#define NV_PFIFO_DMA_CHANNEL_3_PENDING                                          1
#define NV_PFIFO_DMA_CHANNEL_4                                                  0x00000010
#define NV_PFIFO_DMA_CHANNEL_4__BITSHIFT                                        4
#define NV_PFIFO_DMA_CHANNEL_4_NOT_PENDING                                      0
#define NV_PFIFO_DMA_CHANNEL_4_PENDING                                          1
#define NV_PFIFO_DMA_CHANNEL_5                                                  0x00000020
#define NV_PFIFO_DMA_CHANNEL_5__BITSHIFT                                        5
#define NV_PFIFO_DMA_CHANNEL_5_NOT_PENDING                                      0
#define NV_PFIFO_DMA_CHANNEL_5_PENDING                                          1
#define NV_PFIFO_DMA_CHANNEL_6                                                  0x00000040
#define NV_PFIFO_DMA_CHANNEL_6__BITSHIFT                                        6
#define NV_PFIFO_DMA_CHANNEL_6_NOT_PENDING                                      0
#define NV_PFIFO_DMA_CHANNEL_6_PENDING                                          1
#define NV_PFIFO_DMA_CHANNEL_7                                                  0x00000080
#define NV_PFIFO_DMA_CHANNEL_7__BITSHIFT                                        7
#define NV_PFIFO_DMA_CHANNEL_7_NOT_PENDING                                      0
#define NV_PFIFO_DMA_CHANNEL_7_PENDING                                          1
#define NV_PFIFO_DMA_CHANNEL_8                                                  0x00000100
#define NV_PFIFO_DMA_CHANNEL_8__BITSHIFT                                        8
#define NV_PFIFO_DMA_CHANNEL_8_NOT_PENDING                                      0
#define NV_PFIFO_DMA_CHANNEL_8_PENDING                                          1
#define NV_PFIFO_DMA_CHANNEL_9                                                  0x00000200
#define NV_PFIFO_DMA_CHANNEL_9__BITSHIFT                                        9
#define NV_PFIFO_DMA_CHANNEL_9_NOT_PENDING                                      0
#define NV_PFIFO_DMA_CHANNEL_9_PENDING                                          1
#define NV_PFIFO_DMA_CHANNEL_10                                                 0x00000400
#define NV_PFIFO_DMA_CHANNEL_10__BITSHIFT                                       10
#define NV_PFIFO_DMA_CHANNEL_10_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_10_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_11                                                 0x00000800
#define NV_PFIFO_DMA_CHANNEL_11__BITSHIFT                                       11
#define NV_PFIFO_DMA_CHANNEL_11_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_11_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_12                                                 0x00001000
#define NV_PFIFO_DMA_CHANNEL_12__BITSHIFT                                       12
#define NV_PFIFO_DMA_CHANNEL_12_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_12_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_13                                                 0x00002000
#define NV_PFIFO_DMA_CHANNEL_13__BITSHIFT                                       13
#define NV_PFIFO_DMA_CHANNEL_13_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_13_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_14                                                 0x00004000
#define NV_PFIFO_DMA_CHANNEL_14__BITSHIFT                                       14
#define NV_PFIFO_DMA_CHANNEL_14_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_14_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_15                                                 0x00008000
#define NV_PFIFO_DMA_CHANNEL_15__BITSHIFT                                       15
#define NV_PFIFO_DMA_CHANNEL_15_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_15_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_16                                                 0x00010000
#define NV_PFIFO_DMA_CHANNEL_16__BITSHIFT                                       16
#define NV_PFIFO_DMA_CHANNEL_16_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_16_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_17                                                 0x00020000
#define NV_PFIFO_DMA_CHANNEL_17__BITSHIFT                                       17
#define NV_PFIFO_DMA_CHANNEL_17_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_17_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_18                                                 0x00040000
#define NV_PFIFO_DMA_CHANNEL_18__BITSHIFT                                       18
#define NV_PFIFO_DMA_CHANNEL_18_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_18_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_19                                                 0x00080000
#define NV_PFIFO_DMA_CHANNEL_19__BITSHIFT                                       19
#define NV_PFIFO_DMA_CHANNEL_19_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_19_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_20                                                 0x00100000
#define NV_PFIFO_DMA_CHANNEL_20__BITSHIFT                                       20
#define NV_PFIFO_DMA_CHANNEL_20_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_20_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_21                                                 0x00200000
#define NV_PFIFO_DMA_CHANNEL_21__BITSHIFT                                       21
#define NV_PFIFO_DMA_CHANNEL_21_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_21_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_22                                                 0x00400000
#define NV_PFIFO_DMA_CHANNEL_22__BITSHIFT                                       22
#define NV_PFIFO_DMA_CHANNEL_22_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_22_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_23                                                 0x00800000
#define NV_PFIFO_DMA_CHANNEL_23__BITSHIFT                                       23
#define NV_PFIFO_DMA_CHANNEL_23_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_23_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_24                                                 0x01000000
#define NV_PFIFO_DMA_CHANNEL_24__BITSHIFT                                       24
#define NV_PFIFO_DMA_CHANNEL_24_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_24_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_25                                                 0x02000000
#define NV_PFIFO_DMA_CHANNEL_25__BITSHIFT                                       25
#define NV_PFIFO_DMA_CHANNEL_25_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_25_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_26                                                 0x04000000
#define NV_PFIFO_DMA_CHANNEL_26__BITSHIFT                                       26
#define NV_PFIFO_DMA_CHANNEL_26_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_26_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_27                                                 0x08000000
#define NV_PFIFO_DMA_CHANNEL_27__BITSHIFT                                       27
#define NV_PFIFO_DMA_CHANNEL_27_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_27_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_28                                                 0x10000000
#define NV_PFIFO_DMA_CHANNEL_28__BITSHIFT                                       28
#define NV_PFIFO_DMA_CHANNEL_28_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_28_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_29                                                 0x20000000
#define NV_PFIFO_DMA_CHANNEL_29__BITSHIFT                                       29
#define NV_PFIFO_DMA_CHANNEL_29_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_29_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_30                                                 0x40000000
#define NV_PFIFO_DMA_CHANNEL_30__BITSHIFT                                       30
#define NV_PFIFO_DMA_CHANNEL_30_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_30_PENDING                                         1
#define NV_PFIFO_DMA_CHANNEL_31                                                 0x80000000
#define NV_PFIFO_DMA_CHANNEL_31__BITSHIFT                                       31
#define NV_PFIFO_DMA_CHANNEL_31_NOT_PENDING                                     0
#define NV_PFIFO_DMA_CHANNEL_31_PENDING                                         1
        NV_PFIFO_SIZE                               = 0x0000050c,
#define NV_PFIFO_SIZE_CHANNEL_0                                                 0x00000001
#define NV_PFIFO_SIZE_CHANNEL_0__BITSHIFT                                       0
#define NV_PFIFO_SIZE_CHANNEL_0_124_BYTES                                       0
#define NV_PFIFO_SIZE_CHANNEL_0_512_BYTES                                       1
#define NV_PFIFO_SIZE_CHANNEL_1                                                 0x00000002
#define NV_PFIFO_SIZE_CHANNEL_1__BITSHIFT                                       1
#define NV_PFIFO_SIZE_CHANNEL_1_124_BYTES                                       0
#define NV_PFIFO_SIZE_CHANNEL_1_512_BYTES                                       1
#define NV_PFIFO_SIZE_CHANNEL_2                                                 0x00000004
#define NV_PFIFO_SIZE_CHANNEL_2__BITSHIFT                                       2
#define NV_PFIFO_SIZE_CHANNEL_2_124_BYTES                                       0
#define NV_PFIFO_SIZE_CHANNEL_2_512_BYTES                                       1
#define NV_PFIFO_SIZE_CHANNEL_3                                                 0x00000008
#define NV_PFIFO_SIZE_CHANNEL_3__BITSHIFT                                       3
#define NV_PFIFO_SIZE_CHANNEL_3_124_BYTES                                       0
#define NV_PFIFO_SIZE_CHANNEL_3_512_BYTES                                       1
#define NV_PFIFO_SIZE_CHANNEL_4                                                 0x00000010
#define NV_PFIFO_SIZE_CHANNEL_4__BITSHIFT                                       4
#define NV_PFIFO_SIZE_CHANNEL_4_124_BYTES                                       0
#define NV_PFIFO_SIZE_CHANNEL_4_512_BYTES                                       1
#define NV_PFIFO_SIZE_CHANNEL_5                                                 0x00000020
#define NV_PFIFO_SIZE_CHANNEL_5__BITSHIFT                                       5
#define NV_PFIFO_SIZE_CHANNEL_5_124_BYTES                                       0
#define NV_PFIFO_SIZE_CHANNEL_5_512_BYTES                                       1
#define NV_PFIFO_SIZE_CHANNEL_6                                                 0x00000040
#define NV_PFIFO_SIZE_CHANNEL_6__BITSHIFT                                       6
#define NV_PFIFO_SIZE_CHANNEL_6_124_BYTES                                       0
#define NV_PFIFO_SIZE_CHANNEL_6_512_BYTES                                       1
#define NV_PFIFO_SIZE_CHANNEL_7                                                 0x00000080
#define NV_PFIFO_SIZE_CHANNEL_7__BITSHIFT                                       7
#define NV_PFIFO_SIZE_CHANNEL_7_124_BYTES                                       0
#define NV_PFIFO_SIZE_CHANNEL_7_512_BYTES                                       1
#define NV_PFIFO_SIZE_CHANNEL_8                                                 0x00000100
#define NV_PFIFO_SIZE_CHANNEL_8__BITSHIFT                                       8
#define NV_PFIFO_SIZE_CHANNEL_8_124_BYTES                                       0
#define NV_PFIFO_SIZE_CHANNEL_8_512_BYTES                                       1
#define NV_PFIFO_SIZE_CHANNEL_9                                                 0x00000200
#define NV_PFIFO_SIZE_CHANNEL_9__BITSHIFT                                       9
#define NV_PFIFO_SIZE_CHANNEL_9_124_BYTES                                       0
#define NV_PFIFO_SIZE_CHANNEL_9_512_BYTES                                       1
#define NV_PFIFO_SIZE_CHANNEL_10                                                0x00000400
#define NV_PFIFO_SIZE_CHANNEL_10__BITSHIFT                                      10
#define NV_PFIFO_SIZE_CHANNEL_10_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_10_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_11                                                0x00000800
#define NV_PFIFO_SIZE_CHANNEL_11__BITSHIFT                                      11
#define NV_PFIFO_SIZE_CHANNEL_11_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_11_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_12                                                0x00001000
#define NV_PFIFO_SIZE_CHANNEL_12__BITSHIFT                                      12
#define NV_PFIFO_SIZE_CHANNEL_12_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_12_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_13                                                0x00002000
#define NV_PFIFO_SIZE_CHANNEL_13__BITSHIFT                                      13
#define NV_PFIFO_SIZE_CHANNEL_13_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_13_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_14                                                0x00004000
#define NV_PFIFO_SIZE_CHANNEL_14__BITSHIFT                                      14
#define NV_PFIFO_SIZE_CHANNEL_14_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_14_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_15                                                0x00008000
#define NV_PFIFO_SIZE_CHANNEL_15__BITSHIFT                                      15
#define NV_PFIFO_SIZE_CHANNEL_15_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_15_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_16                                                0x00010000
#define NV_PFIFO_SIZE_CHANNEL_16__BITSHIFT                                      16
#define NV_PFIFO_SIZE_CHANNEL_16_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_16_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_17                                                0x00020000
#define NV_PFIFO_SIZE_CHANNEL_17__BITSHIFT                                      17
#define NV_PFIFO_SIZE_CHANNEL_17_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_17_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_18                                                0x00040000
#define NV_PFIFO_SIZE_CHANNEL_18__BITSHIFT                                      18
#define NV_PFIFO_SIZE_CHANNEL_18_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_18_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_19                                                0x00080000
#define NV_PFIFO_SIZE_CHANNEL_19__BITSHIFT                                      19
#define NV_PFIFO_SIZE_CHANNEL_19_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_19_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_20                                                0x00100000
#define NV_PFIFO_SIZE_CHANNEL_20__BITSHIFT                                      20
#define NV_PFIFO_SIZE_CHANNEL_20_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_20_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_21                                                0x00200000
#define NV_PFIFO_SIZE_CHANNEL_21__BITSHIFT                                      21
#define NV_PFIFO_SIZE_CHANNEL_21_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_21_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_22                                                0x00400000
#define NV_PFIFO_SIZE_CHANNEL_22__BITSHIFT                                      22
#define NV_PFIFO_SIZE_CHANNEL_22_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_22_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_23                                                0x00800000
#define NV_PFIFO_SIZE_CHANNEL_23__BITSHIFT                                      23
#define NV_PFIFO_SIZE_CHANNEL_23_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_23_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_24                                                0x01000000
#define NV_PFIFO_SIZE_CHANNEL_24__BITSHIFT                                      24
#define NV_PFIFO_SIZE_CHANNEL_24_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_24_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_25                                                0x02000000
#define NV_PFIFO_SIZE_CHANNEL_25__BITSHIFT                                      25
#define NV_PFIFO_SIZE_CHANNEL_25_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_25_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_26                                                0x04000000
#define NV_PFIFO_SIZE_CHANNEL_26__BITSHIFT                                      26
#define NV_PFIFO_SIZE_CHANNEL_26_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_26_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_27                                                0x08000000
#define NV_PFIFO_SIZE_CHANNEL_27__BITSHIFT                                      27
#define NV_PFIFO_SIZE_CHANNEL_27_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_27_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_28                                                0x10000000
#define NV_PFIFO_SIZE_CHANNEL_28__BITSHIFT                                      28
#define NV_PFIFO_SIZE_CHANNEL_28_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_28_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_29                                                0x20000000
#define NV_PFIFO_SIZE_CHANNEL_29__BITSHIFT                                      29
#define NV_PFIFO_SIZE_CHANNEL_29_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_29_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_30                                                0x40000000
#define NV_PFIFO_SIZE_CHANNEL_30__BITSHIFT                                      30
#define NV_PFIFO_SIZE_CHANNEL_30_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_30_512_BYTES                                      1
#define NV_PFIFO_SIZE_CHANNEL_31                                                0x80000000
#define NV_PFIFO_SIZE_CHANNEL_31__BITSHIFT                                      31
#define NV_PFIFO_SIZE_CHANNEL_31_124_BYTES                                      0
#define NV_PFIFO_SIZE_CHANNEL_31_512_BYTES                                      1
        NV_PFIFO_CACHE0_PUSH0                       = 0x00001000,
#define NV_PFIFO_CACHE0_PUSH0_ACCESS                                            0x00000001
#define NV_PFIFO_CACHE0_PUSH0_ACCESS__BITSHIFT                                  0
#define NV_PFIFO_CACHE0_PUSH0_ACCESS_DISABLED                                   0
#define NV_PFIFO_CACHE0_PUSH0_ACCESS_ENABLED                                    1
        NV_PFIFO_CACHE1_PUSH0                       = 0x00001200,
#define NV_PFIFO_CACHE1_PUSH0_ACCESS                                            0x00000001
#define NV_PFIFO_CACHE1_PUSH0_ACCESS__BITSHIFT                                  0
#define NV_PFIFO_CACHE1_PUSH0_ACCESS_DISABLED                                   0
#define NV_PFIFO_CACHE1_PUSH0_ACCESS_ENABLED                                    1
        NV_PFIFO_CACHE0_PUSH1                       = 0x00001004,
#define NV_PFIFO_CACHE0_PUSH1_CHID                                              0x0000001f
#define NV_PFIFO_CACHE0_PUSH1_CHID__BITSHIFT                                    0
        NV_PFIFO_CACHE1_PUSH1                       = 0x00001204,
#define NV_PFIFO_CACHE1_PUSH1_CHID                                              0x0000001f
#define NV_PFIFO_CACHE1_PUSH1_CHID__BITSHIFT                                    0
#define NV_PFIFO_CACHE1_PUSH1_MODE                                              0x00000100
#define NV_PFIFO_CACHE1_PUSH1_MODE__BITSHIFT                                    8
#define NV_PFIFO_CACHE1_PUSH1_MODE_PIO                                          0
#define NV_PFIFO_CACHE1_PUSH1_MODE_DMA                                          1
        NV_PFIFO_CACHE1_DMA_PUSH                    = 0x00001220,
#define NV_PFIFO_CACHE1_DMA_PUSH_ACCESS                                         0x00000001
#define NV_PFIFO_CACHE1_DMA_PUSH_ACCESS__BITSHIFT                               0
#define NV_PFIFO_CACHE1_DMA_PUSH_ACCESS_DISABLED                                0
#define NV_PFIFO_CACHE1_DMA_PUSH_ACCESS_ENABLED                                 1
#define NV_PFIFO_CACHE1_DMA_PUSH_STATE                                          0x00000010
#define NV_PFIFO_CACHE1_DMA_PUSH_STATE__BITSHIFT                                4
#define NV_PFIFO_CACHE1_DMA_PUSH_STATE_IDLE                                     0
#define NV_PFIFO_CACHE1_DMA_PUSH_STATE_BUSY                                     1
#define NV_PFIFO_CACHE1_DMA_PUSH_BUFFER                                         0x00000100
#define NV_PFIFO_CACHE1_DMA_PUSH_BUFFER__BITSHIFT                               8
#define NV_PFIFO_CACHE1_DMA_PUSH_BUFFER_NOT_EMPTY                               0
#define NV_PFIFO_CACHE1_DMA_PUSH_BUFFER_EMPTY                                   1
#define NV_PFIFO_CACHE1_DMA_PUSH_STATUS                                         0x00001000
#define NV_PFIFO_CACHE1_DMA_PUSH_STATUS__BITSHIFT                               12
#define NV_PFIFO_CACHE1_DMA_PUSH_STATUS_RUNNING                                 0
#define NV_PFIFO_CACHE1_DMA_PUSH_STATUS_SUSPENDED                               1
#define NV_PFIFO_CACHE1_DMA_PUSH_ACQUIRE                                        0x00010000
#define NV_PFIFO_CACHE1_DMA_PUSH_ACQUIRE__BITSHIFT                              16
#define NV_PFIFO_CACHE1_DMA_PUSH_ACQUIRE_NOT_PENDING                            0
#define NV_PFIFO_CACHE1_DMA_PUSH_ACQUIRE_PENDING                                1
        NV_PFIFO_CACHE1_DMA_FETCH                   = 0x00001224,
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG                                          0x000000f8
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG__BITSHIFT                                3
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_8_BYTES                                  0
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_16_BYTES                                 1
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_24_BYTES                                 2
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_32_BYTES                                 3
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_40_BYTES                                 4
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_48_BYTES                                 5
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_56_BYTES                                 6
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_64_BYTES                                 7
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_72_BYTES                                 8
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_80_BYTES                                 9
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_88_BYTES                                 10
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_96_BYTES                                 11
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_104_BYTES                                12
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_112_BYTES                                13
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_120_BYTES                                14
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_128_BYTES                                15
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_136_BYTES                                16
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_144_BYTES                                17
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_152_BYTES                                18
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_160_BYTES                                19
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_168_BYTES                                20
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_176_BYTES                                21
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_184_BYTES                                22
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_192_BYTES                                23
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_200_BYTES                                24
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_208_BYTES                                25
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_216_BYTES                                26
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_224_BYTES                                27
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_232_BYTES                                28
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_240_BYTES                                29
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_248_BYTES                                30
#define NV_PFIFO_CACHE1_DMA_FETCH_TRIG_256_BYTES                                31
#define NV_PFIFO_CACHE1_DMA_FETCH_SIZE                                          0x0000e000
#define NV_PFIFO_CACHE1_DMA_FETCH_SIZE__BITSHIFT                                13
#define NV_PFIFO_CACHE1_DMA_FETCH_SIZE_32_BYTES                                 0
#define NV_PFIFO_CACHE1_DMA_FETCH_SIZE_64_BYTES                                 1
#define NV_PFIFO_CACHE1_DMA_FETCH_SIZE_96_BYTES                                 2
#define NV_PFIFO_CACHE1_DMA_FETCH_SIZE_128_BYTES                                3
#define NV_PFIFO_CACHE1_DMA_FETCH_SIZE_160_BYTES                                4
#define NV_PFIFO_CACHE1_DMA_FETCH_SIZE_192_BYTES                                5
#define NV_PFIFO_CACHE1_DMA_FETCH_SIZE_224_BYTES                                6
#define NV_PFIFO_CACHE1_DMA_FETCH_SIZE_256_BYTES                                7
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS                                      0x001f0000
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS__BITSHIFT                            16
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_0                                    0
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_1                                    1
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_2                                    2
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_3                                    3
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_4                                    4
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_5                                    5
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_6                                    6
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_7                                    7
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_8                                    8
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_9                                    9
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_10                                   10
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_11                                   11
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_12                                   12
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_13                                   13
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_14                                   14
#define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS_15                                   15
#define NV_PFIFO_CACHE1_DMA_FETCH_ENDIAN                                        0x80000000
#define NV_PFIFO_CACHE1_DMA_FETCH_ENDIAN__BITSHIFT                              31
#define NV_PFIFO_CACHE1_DMA_FETCH_LITTLE_ENDIAN                                 0
#define NV_PFIFO_CACHE1_DMA_FETCH_BIG_ENDIAN                                    1
        NV_PFIFO_CACHE1_DMA_PUT                     = 0x00001240,
#define NV_PFIFO_CACHE1_DMA_PUT_OFFSET                                          0xfffffffc
#define NV_PFIFO_CACHE1_DMA_PUT_OFFSET__BITSHIFT                                0
        NV_PFIFO_CACHE1_DMA_GET                     = 0x00001244,
#define NV_PFIFO_CACHE1_DMA_GET_OFFSET                                          0xfffffffc
#define NV_PFIFO_CACHE1_DMA_GET_OFFSET__BITSHIFT                                0
        NV_PFIFO_CACHE1_REF                         = 0x00001248,
        NV_PFIFO_CACHE1_DMA_SUBROUTINE              = 0x0000124c,
#define NV_PFIFO_CACHE1_DMA_SUBROUTINE_RETURN_OFFSET                            0xfffffffc
#define NV_PFIFO_CACHE1_DMA_SUBROUTINE_RETURN_OFFSET__BITSHIFT                  0
#define NV_PFIFO_CACHE1_DMA_SUBROUTINE_STATE                                    0x00000001
#define NV_PFIFO_CACHE1_DMA_SUBROUTINE_STATE__BITSHIFT                          0
#define NV_PFIFO_CACHE1_DMA_SUBROUTINE_STATE_INACTIVE                           0
#define NV_PFIFO_CACHE1_DMA_SUBROUTINE_STATE_ACTIVE                             1
        NV_PFIFO_CACHE1_DMA_DCOUNT                  = 0x000012a0,
#define NV_PFIFO_CACHE1_DMA_DCOUNT_VALUE                                        0x00001ffc
#define NV_PFIFO_CACHE1_DMA_DCOUNT_VALUE__BITSHIFT                              2
        NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW          = 0x000012a4,
#define NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET                               0xfffffffc
#define NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET__BITSHIFT                     0
        NV_PFIFO_CACHE1_DMA_RSVD_SHADOW             = 0x000012a8,
        NV_PFIFO_CACHE1_DMA_DATA_SHADOW             = 0x000012ac,
        NV_PFIFO_CACHE1_SUBCH_01_INST               = 0x000012b0,
#define NV_PFIFO_CACHE1_SUBCH_0_INST_VALUE                                      0x0000ffff
#define NV_PFIFO_CACHE1_SUBCH_0_INST_VALUE__BITSHIFT                            0
#define NV_PFIFO_CACHE1_SUBCH_1_INST_VALUE                                      0xffff0000
#define NV_PFIFO_CACHE1_SUBCH_1_INST_VALUE__BITSHIFT                            16
        NV_PFIFO_CACHE1_SUBCH_23_INST               = 0x000012b4,
#define NV_PFIFO_CACHE1_SUBCH_2_INST_VALUE                                      0x0000ffff
#define NV_PFIFO_CACHE1_SUBCH_2_INST_VALUE__BITSHIFT                            0
#define NV_PFIFO_CACHE1_SUBCH_3_INST_VALUE                                      0xffff0000
#define NV_PFIFO_CACHE1_SUBCH_3_INST_VALUE__BITSHIFT                            16
        NV_PFIFO_CACHE1_SUBCH_45_INST               = 0x000012b8,
#define NV_PFIFO_CACHE1_SUBCH_4_INST_VALUE                                      0x0000ffff
#define NV_PFIFO_CACHE1_SUBCH_4_INST_VALUE__BITSHIFT                            0
#define NV_PFIFO_CACHE1_SUBCH_5_INST_VALUE                                      0xffff0000
#define NV_PFIFO_CACHE1_SUBCH_5_INST_VALUE__BITSHIFT                            16
        NV_PFIFO_CACHE1_SUBCH_67_INST               = 0x000012bc,
#define NV_PFIFO_CACHE1_SUBCH_6_INST_VALUE                                      0x0000ffff
#define NV_PFIFO_CACHE1_SUBCH_6_INST_VALUE__BITSHIFT                            0
#define NV_PFIFO_CACHE1_SUBCH_7_INST_VALUE                                      0xffff0000
#define NV_PFIFO_CACHE1_SUBCH_7_INST_VALUE__BITSHIFT                            16
        NV_PFIFO_CACHE0_SUBCH_INST                  = 0x000012c0,
#define NV_PFIFO_CACHE0_SUBCH_INST_VALUE                                        0x0000ffff
#define NV_PFIFO_CACHE0_SUBCH_INST_VALUE__BITSHIFT                              0
        NV_PFIFO_CACHE1_DMA_STATE                   = 0x00001228,
#define NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE                                   0x00000001
#define NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE__BITSHIFT                         0
#define NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE_INC                               0
#define NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE_NON_INC                           1
#define NV_PFIFO_CACHE1_DMA_STATE_METHOD                                        0x00001ffc
#define NV_PFIFO_CACHE1_DMA_STATE_METHOD__BITSHIFT                              2
#define NV_PFIFO_CACHE1_DMA_STATE_SUBCHANNEL                                    0x0000e000
#define NV_PFIFO_CACHE1_DMA_STATE_SUBCHANNEL__BITSHIFT                          13
#define NV_PFIFO_CACHE1_DMA_STATE_METHOD_COUNT                                  0x1ffc0000
#define NV_PFIFO_CACHE1_DMA_STATE_METHOD_COUNT__BITSHIFT                        18
#define NV_PFIFO_CACHE1_DMA_STATE_METHOD_COUNT_0                                0
#define NV_PFIFO_CACHE1_DMA_STATE_ERROR                                         0xe0000000
#define NV_PFIFO_CACHE1_DMA_STATE_ERROR__BITSHIFT                               29
#define NV_PFIFO_CACHE1_DMA_STATE_ERROR_NONE                                    0
#define NV_PFIFO_CACHE1_DMA_STATE_ERROR_CALL                                    1
#define NV_PFIFO_CACHE1_DMA_STATE_ERROR_NON_CACHE                               2
#define NV_PFIFO_CACHE1_DMA_STATE_ERROR_RETURN                                  3
#define NV_PFIFO_CACHE1_DMA_STATE_ERROR_RESERVED_CMD                            4
#define NV_PFIFO_CACHE1_DMA_STATE_ERROR_PROTECTION                              6
        NV_PFIFO_CACHE1_DMA_INSTANCE                = 0x0000122c,
#define NV_PFIFO_CACHE1_DMA_INSTANCE_ADDRESS                                    0x0000ffff
#define NV_PFIFO_CACHE1_DMA_INSTANCE_ADDRESS__BITSHIFT                          0
        NV_PFIFO_CACHE1_DMA_CTL                     = 0x00001230,
#define NV_PFIFO_CACHE1_DMA_CTL_ADJUST                                          0x00000ffc
#define NV_PFIFO_CACHE1_DMA_CTL_ADJUST__BITSHIFT                                2
#define NV_PFIFO_CACHE1_DMA_CTL_PAGE_TABLE                                      0x00001000
#define NV_PFIFO_CACHE1_DMA_CTL_PAGE_TABLE__BITSHIFT                            12
#define NV_PFIFO_CACHE1_DMA_CTL_PAGE_TABLE_NOT_PRESENT                          0
#define NV_PFIFO_CACHE1_DMA_CTL_PAGE_TABLE_PRESENT                              1
#define NV_PFIFO_CACHE1_DMA_CTL_PAGE_ENTRY                                      0x00002000
#define NV_PFIFO_CACHE1_DMA_CTL_PAGE_ENTRY__BITSHIFT                            13
#define NV_PFIFO_CACHE1_DMA_CTL_PAGE_ENTRY_NOT_LINEAR                           0
#define NV_PFIFO_CACHE1_DMA_CTL_PAGE_ENTRY_LINEAR                               1
#define NV_PFIFO_CACHE1_DMA_CTL_TARGET_NODE                                     0x00030000
#define NV_PFIFO_CACHE1_DMA_CTL_TARGET_NODE__BITSHIFT                           16
#define NV_PFIFO_CACHE1_DMA_CTL_TARGET_NODE_NVM                                 0
#define NV_PFIFO_CACHE1_DMA_CTL_TARGET_NODE_PCI                                 2
#define NV_PFIFO_CACHE1_DMA_CTL_TARGET_NODE_AGP                                 3
#define NV_PFIFO_CACHE1_DMA_CTL_AT_INFO                                         0x80000000
#define NV_PFIFO_CACHE1_DMA_CTL_AT_INFO__BITSHIFT                               31
#define NV_PFIFO_CACHE1_DMA_CTL_AT_INFO_INVALID                                 0
#define NV_PFIFO_CACHE1_DMA_CTL_AT_INFO_VALID                                   1
        NV_PFIFO_CACHE1_DMA_LIMIT                   = 0x00001234,
#define NV_PFIFO_CACHE1_DMA_LIMIT_OFFSET                                        0xfffffffc
#define NV_PFIFO_CACHE1_DMA_LIMIT_OFFSET__BITSHIFT                              0
        NV_PFIFO_CACHE1_DMA_TLB_TAG                 = 0x00001238,
#define NV_PFIFO_CACHE1_DMA_TLB_TAG_ADDRESS                                     0xfffff000
#define NV_PFIFO_CACHE1_DMA_TLB_TAG_ADDRESS__BITSHIFT                           12
#define NV_PFIFO_CACHE1_DMA_TLB_TAG_STATE                                       0x00000001
#define NV_PFIFO_CACHE1_DMA_TLB_TAG_STATE__BITSHIFT                             0
#define NV_PFIFO_CACHE1_DMA_TLB_TAG_STATE_INVALID                               0
#define NV_PFIFO_CACHE1_DMA_TLB_TAG_STATE_VALID                                 1
        NV_PFIFO_CACHE1_DMA_TLB_PTE                 = 0x0000123c,
#define NV_PFIFO_CACHE1_DMA_TLB_PTE_FRAME_ADDRESS                               0xfffff000
#define NV_PFIFO_CACHE1_DMA_TLB_PTE_FRAME_ADDRESS__BITSHIFT                     12
        NV_PFIFO_CACHE0_PULL0                       = 0x00001050,
#define NV_PFIFO_CACHE0_PULL0_ACCESS                                            0x00000001
#define NV_PFIFO_CACHE0_PULL0_ACCESS__BITSHIFT                                  0
#define NV_PFIFO_CACHE0_PULL0_ACCESS_DISABLED                                   0
#define NV_PFIFO_CACHE0_PULL0_ACCESS_ENABLED                                    1
#define NV_PFIFO_CACHE0_PULL0_HASH                                              0x00000010
#define NV_PFIFO_CACHE0_PULL0_HASH__BITSHIFT                                    4
#define NV_PFIFO_CACHE0_PULL0_HASH_SUCCEEDED                                    0
#define NV_PFIFO_CACHE0_PULL0_HASH_FAILED                                       1
#define NV_PFIFO_CACHE0_PULL0_DEVICE                                            0x00000100
#define NV_PFIFO_CACHE0_PULL0_DEVICE__BITSHIFT                                  8
#define NV_PFIFO_CACHE0_PULL0_DEVICE_HARDWARE                                   0
#define NV_PFIFO_CACHE0_PULL0_DEVICE_SOFTWARE                                   1
#define NV_PFIFO_CACHE0_PULL0_HASH_STATE                                        0x00001000
#define NV_PFIFO_CACHE0_PULL0_HASH_STATE__BITSHIFT                              12
#define NV_PFIFO_CACHE0_PULL0_HASH_STATE_IDLE                                   0
#define NV_PFIFO_CACHE0_PULL0_HASH_STATE_BUSY                                   1
        NV_PFIFO_CACHE1_PULL0                       = 0x00001250,
#define NV_PFIFO_CACHE1_PULL0_ACCESS                                            0x00000001
#define NV_PFIFO_CACHE1_PULL0_ACCESS__BITSHIFT                                  0
#define NV_PFIFO_CACHE1_PULL0_ACCESS_DISABLED                                   0
#define NV_PFIFO_CACHE1_PULL0_ACCESS_ENABLED                                    1
#define NV_PFIFO_CACHE1_PULL0_HASH                                              0x00000010
#define NV_PFIFO_CACHE1_PULL0_HASH__BITSHIFT                                    4
#define NV_PFIFO_CACHE1_PULL0_HASH_SUCCEEDED                                    0
#define NV_PFIFO_CACHE1_PULL0_HASH_FAILED                                       1
#define NV_PFIFO_CACHE1_PULL0_DEVICE                                            0x00000100
#define NV_PFIFO_CACHE1_PULL0_DEVICE__BITSHIFT                                  8
#define NV_PFIFO_CACHE1_PULL0_DEVICE_HARDWARE                                   0
#define NV_PFIFO_CACHE1_PULL0_DEVICE_SOFTWARE                                   1
#define NV_PFIFO_CACHE1_PULL0_HASH_STATE                                        0x00001000
#define NV_PFIFO_CACHE1_PULL0_HASH_STATE__BITSHIFT                              12
#define NV_PFIFO_CACHE1_PULL0_HASH_STATE_IDLE                                   0
#define NV_PFIFO_CACHE1_PULL0_HASH_STATE_BUSY                                   1
#define NV_PFIFO_CACHE1_PULL0_ACQUIRE_STATE                                     0x00010000
#define NV_PFIFO_CACHE1_PULL0_ACQUIRE_STATE__BITSHIFT                           16
#define NV_PFIFO_CACHE1_PULL0_ACQUIRE_STATE_IDLE                                0
#define NV_PFIFO_CACHE1_PULL0_ACQUIRE_STATE_BUSY                                1
#define NV_PFIFO_CACHE1_PULL0_SEMAPHORE                                         0x00300000
#define NV_PFIFO_CACHE1_PULL0_SEMAPHORE__BITSHIFT                               20
#define NV_PFIFO_CACHE1_PULL0_SEMAPHORE_NO_ERROR                                0
#define NV_PFIFO_CACHE1_PULL0_SEMAPHORE_BAD_ARG                                 1
#define NV_PFIFO_CACHE1_PULL0_SEMAPHORE_INV_STATE                               2
        NV_PFIFO_CACHE0_PULL1                       = 0x00001054,
#define NV_PFIFO_CACHE0_PULL1_ENGINE                                            0x00000003
#define NV_PFIFO_CACHE0_PULL1_ENGINE__BITSHIFT                                  0
#define NV_PFIFO_CACHE0_PULL1_ENGINE_SW                                         0
#define NV_PFIFO_CACHE0_PULL1_ENGINE_GRAPHICS                                   1
#define NV_PFIFO_CACHE0_PULL1_ENGINE_DVD                                        2
        NV_PFIFO_CACHE1_PULL1                       = 0x00001254,
#define NV_PFIFO_CACHE1_PULL1_ENGINE                                            0x00000003
#define NV_PFIFO_CACHE1_PULL1_ENGINE__BITSHIFT                                  0
#define NV_PFIFO_CACHE1_PULL1_ENGINE_SW                                         0
#define NV_PFIFO_CACHE1_PULL1_ENGINE_GRAPHICS                                   1
#define NV_PFIFO_CACHE1_PULL1_ENGINE_DVD                                        2
#define NV_PFIFO_CACHE1_PULL1_ACQUIRE                                           0x00000010
#define NV_PFIFO_CACHE1_PULL1_ACQUIRE__BITSHIFT                                 4
#define NV_PFIFO_CACHE1_PULL1_ACQUIRE_INACTIVE                                  0
#define NV_PFIFO_CACHE1_PULL1_ACQUIRE_ACTIVE                                    1
#define NV_PFIFO_CACHE1_PULL1_SEM_TARGET_NODE                                   0x00030000
#define NV_PFIFO_CACHE1_PULL1_SEM_TARGET_NODE__BITSHIFT                         16
#define NV_PFIFO_CACHE1_PULL1_SEM_TARGET_NODE_NVM                               0
#define NV_PFIFO_CACHE1_PULL1_SEM_TARGET_NODE_PCI                               2
#define NV_PFIFO_CACHE1_PULL1_SEM_TARGET_NODE_AGP                               3
        NV_PFIFO_CACHE0_HASH                        = 0x00001058,
#define NV_PFIFO_CACHE0_HASH_INSTANCE                                           0x0000ffff
#define NV_PFIFO_CACHE0_HASH_INSTANCE__BITSHIFT                                 0
#define NV_PFIFO_CACHE0_HASH_VALID                                              0x00010000
#define NV_PFIFO_CACHE0_HASH_VALID__BITSHIFT                                    16
        NV_PFIFO_CACHE1_HASH                        = 0x00001258,
#define NV_PFIFO_CACHE1_HASH_INSTANCE                                           0x0000ffff
#define NV_PFIFO_CACHE1_HASH_INSTANCE__BITSHIFT                                 0
#define NV_PFIFO_CACHE1_HASH_VALID                                              0x00010000
#define NV_PFIFO_CACHE1_HASH_VALID__BITSHIFT                                    16
        NV_PFIFO_CACHE1_ACQUIRE_0                   = 0x00001260,
#define NV_PFIFO_CACHE1_ACQUIRE_0_TIMEOUT                                       0x7fffffff
#define NV_PFIFO_CACHE1_ACQUIRE_0_TIMEOUT__BITSHIFT                             0
        NV_PFIFO_CACHE1_ACQUIRE_1                   = 0x00001264,
        NV_PFIFO_CACHE1_ACQUIRE_2                   = 0x00001268,
        NV_PFIFO_CACHE1_SEMAPHORE                   = 0x0000126c,
#define NV_PFIFO_CACHE1_SEMAPHORE_CTXDMA                                        0x00000001
#define NV_PFIFO_CACHE1_SEMAPHORE_CTXDMA__BITSHIFT                              0
#define NV_PFIFO_CACHE1_SEMAPHORE_CTXDMA_INVALID                                0
#define NV_PFIFO_CACHE1_SEMAPHORE_CTXDMA_VALID                                  1
#define NV_PFIFO_CACHE1_SEMAPHORE_OFFSET                                        0x00000ffc
#define NV_PFIFO_CACHE1_SEMAPHORE_OFFSET__BITSHIFT                              2
#define NV_PFIFO_CACHE1_SEMAPHORE_PAGE_ADDRESS                                  0xfffff000
#define NV_PFIFO_CACHE1_SEMAPHORE_PAGE_ADDRESS__BITSHIFT                        12
        NV_PFIFO_CACHE0_STATUS                      = 0x00001014,
#define NV_PFIFO_CACHE0_STATUS_LOW_MARK                                         0x00000010
#define NV_PFIFO_CACHE0_STATUS_LOW_MARK__BITSHIFT                               4
#define NV_PFIFO_CACHE0_STATUS_LOW_MARK_NOT_EMPTY                               0
#define NV_PFIFO_CACHE0_STATUS_LOW_MARK_EMPTY                                   1
#define NV_PFIFO_CACHE0_STATUS_HIGH_MARK                                        0x00000100
#define NV_PFIFO_CACHE0_STATUS_HIGH_MARK__BITSHIFT                              8
#define NV_PFIFO_CACHE0_STATUS_HIGH_MARK_NOT_FULL                               0
#define NV_PFIFO_CACHE0_STATUS_HIGH_MARK_FULL                                   1
        NV_PFIFO_CACHE1_STATUS                      = 0x00001214,
#define NV_PFIFO_CACHE1_STATUS_LOW_MARK                                         0x00000010
#define NV_PFIFO_CACHE1_STATUS_LOW_MARK__BITSHIFT                               4
#define NV_PFIFO_CACHE1_STATUS_LOW_MARK_NOT_EMPTY                               0
#define NV_PFIFO_CACHE1_STATUS_LOW_MARK_EMPTY                                   1
#define NV_PFIFO_CACHE1_STATUS_HIGH_MARK                                        0x00000100
#define NV_PFIFO_CACHE1_STATUS_HIGH_MARK__BITSHIFT                              8
#define NV_PFIFO_CACHE1_STATUS_HIGH_MARK_NOT_FULL                               0
#define NV_PFIFO_CACHE1_STATUS_HIGH_MARK_FULL                                   1
        NV_PFIFO_CACHE1_STATUS1                     = 0x00001218,
#define NV_PFIFO_CACHE1_STATUS1_RANOUT                                          0x00000001
#define NV_PFIFO_CACHE1_STATUS1_RANOUT__BITSHIFT                                0
#define NV_PFIFO_CACHE1_STATUS1_RANOUT_FALSE                                    0
#define NV_PFIFO_CACHE1_STATUS1_RANOUT_TRUE                                     1
        NV_PFIFO_CACHE0_PUT                         = 0x00001010,
#define NV_PFIFO_CACHE0_PUT_ADDRESS                                             0x00000004
#define NV_PFIFO_CACHE0_PUT_ADDRESS__BITSHIFT                                   2
        NV_PFIFO_CACHE1_PUT                         = 0x00001210,
#define NV_PFIFO_CACHE1_PUT_ADDRESS                                             0x000003fc
#define NV_PFIFO_CACHE1_PUT_ADDRESS__BITSHIFT                                   2
        NV_PFIFO_CACHE0_GET                         = 0x00001070,
#define NV_PFIFO_CACHE0_GET_ADDRESS                                             0x00000004
#define NV_PFIFO_CACHE0_GET_ADDRESS__BITSHIFT                                   2
        NV_PFIFO_CACHE1_GET                         = 0x00001270,
#define NV_PFIFO_CACHE1_GET_ADDRESS                                             0x000003fc
#define NV_PFIFO_CACHE1_GET_ADDRESS__BITSHIFT                                   2
        NV_PFIFO_CACHE0_ENGINE                      = 0x00001080,
#define NV_PFIFO_CACHE0_ENGINE_0                                                0x00000003
#define NV_PFIFO_CACHE0_ENGINE_0__BITSHIFT                                      0
#define NV_PFIFO_CACHE0_ENGINE_0_SW                                             0
#define NV_PFIFO_CACHE0_ENGINE_0_GRAPHICS                                       1
#define NV_PFIFO_CACHE0_ENGINE_0_DVD                                            2
#define NV_PFIFO_CACHE0_ENGINE_1                                                0x00000030
#define NV_PFIFO_CACHE0_ENGINE_1__BITSHIFT                                      4
#define NV_PFIFO_CACHE0_ENGINE_1_SW                                             0
#define NV_PFIFO_CACHE0_ENGINE_1_GRAPHICS                                       1
#define NV_PFIFO_CACHE0_ENGINE_1_DVD                                            2
#define NV_PFIFO_CACHE0_ENGINE_2                                                0x00000300
#define NV_PFIFO_CACHE0_ENGINE_2__BITSHIFT                                      8
#define NV_PFIFO_CACHE0_ENGINE_2_SW                                             0
#define NV_PFIFO_CACHE0_ENGINE_2_GRAPHICS                                       1
#define NV_PFIFO_CACHE0_ENGINE_2_DVD                                            2
#define NV_PFIFO_CACHE0_ENGINE_3                                                0x00003000
#define NV_PFIFO_CACHE0_ENGINE_3__BITSHIFT                                      12
#define NV_PFIFO_CACHE0_ENGINE_3_SW                                             0
#define NV_PFIFO_CACHE0_ENGINE_3_GRAPHICS                                       1
#define NV_PFIFO_CACHE0_ENGINE_3_DVD                                            2
#define NV_PFIFO_CACHE0_ENGINE_4                                                0x00030000
#define NV_PFIFO_CACHE0_ENGINE_4__BITSHIFT                                      16
#define NV_PFIFO_CACHE0_ENGINE_4_SW                                             0
#define NV_PFIFO_CACHE0_ENGINE_4_GRAPHICS                                       1
#define NV_PFIFO_CACHE0_ENGINE_4_DVD                                            2
#define NV_PFIFO_CACHE0_ENGINE_5                                                0x00300000
#define NV_PFIFO_CACHE0_ENGINE_5__BITSHIFT                                      20
#define NV_PFIFO_CACHE0_ENGINE_5_SW                                             0
#define NV_PFIFO_CACHE0_ENGINE_5_GRAPHICS                                       1
#define NV_PFIFO_CACHE0_ENGINE_5_DVD                                            2
#define NV_PFIFO_CACHE0_ENGINE_6                                                0x03000000
#define NV_PFIFO_CACHE0_ENGINE_6__BITSHIFT                                      24
#define NV_PFIFO_CACHE0_ENGINE_6_SW                                             0
#define NV_PFIFO_CACHE0_ENGINE_6_GRAPHICS                                       1
#define NV_PFIFO_CACHE0_ENGINE_6_DVD                                            2
#define NV_PFIFO_CACHE0_ENGINE_7                                                0x30000000
#define NV_PFIFO_CACHE0_ENGINE_7__BITSHIFT                                      28
#define NV_PFIFO_CACHE0_ENGINE_7_SW                                             0
#define NV_PFIFO_CACHE0_ENGINE_7_GRAPHICS                                       1
#define NV_PFIFO_CACHE0_ENGINE_7_DVD                                            2
        NV_PFIFO_CACHE1_ENGINE                      = 0x00001280,
#define NV_PFIFO_CACHE1_ENGINE_0                                                0x00000003
#define NV_PFIFO_CACHE1_ENGINE_0__BITSHIFT                                      0
#define NV_PFIFO_CACHE1_ENGINE_0_SW                                             0
#define NV_PFIFO_CACHE1_ENGINE_0_GRAPHICS                                       1
#define NV_PFIFO_CACHE1_ENGINE_0_DVD                                            2
#define NV_PFIFO_CACHE1_ENGINE_1                                                0x00000030
#define NV_PFIFO_CACHE1_ENGINE_1__BITSHIFT                                      4
#define NV_PFIFO_CACHE1_ENGINE_1_SW                                             0
#define NV_PFIFO_CACHE1_ENGINE_1_GRAPHICS                                       1
#define NV_PFIFO_CACHE1_ENGINE_1_DVD                                            2
#define NV_PFIFO_CACHE1_ENGINE_2                                                0x00000300
#define NV_PFIFO_CACHE1_ENGINE_2__BITSHIFT                                      8
#define NV_PFIFO_CACHE1_ENGINE_2_SW                                             0
#define NV_PFIFO_CACHE1_ENGINE_2_GRAPHICS                                       1
#define NV_PFIFO_CACHE1_ENGINE_2_DVD                                            2
#define NV_PFIFO_CACHE1_ENGINE_3                                                0x00003000
#define NV_PFIFO_CACHE1_ENGINE_3__BITSHIFT                                      12
#define NV_PFIFO_CACHE1_ENGINE_3_SW                                             0
#define NV_PFIFO_CACHE1_ENGINE_3_GRAPHICS                                       1
#define NV_PFIFO_CACHE1_ENGINE_3_DVD                                            2
#define NV_PFIFO_CACHE1_ENGINE_4                                                0x00030000
#define NV_PFIFO_CACHE1_ENGINE_4__BITSHIFT                                      16
#define NV_PFIFO_CACHE1_ENGINE_4_SW                                             0
#define NV_PFIFO_CACHE1_ENGINE_4_GRAPHICS                                       1
#define NV_PFIFO_CACHE1_ENGINE_4_DVD                                            2
#define NV_PFIFO_CACHE1_ENGINE_5                                                0x00300000
#define NV_PFIFO_CACHE1_ENGINE_5__BITSHIFT                                      20
#define NV_PFIFO_CACHE1_ENGINE_5_SW                                             0
#define NV_PFIFO_CACHE1_ENGINE_5_GRAPHICS                                       1
#define NV_PFIFO_CACHE1_ENGINE_5_DVD                                            2
#define NV_PFIFO_CACHE1_ENGINE_6                                                0x03000000
#define NV_PFIFO_CACHE1_ENGINE_6__BITSHIFT                                      24
#define NV_PFIFO_CACHE1_ENGINE_6_SW                                             0
#define NV_PFIFO_CACHE1_ENGINE_6_GRAPHICS                                       1
#define NV_PFIFO_CACHE1_ENGINE_6_DVD                                            2
#define NV_PFIFO_CACHE1_ENGINE_7                                                0x30000000
#define NV_PFIFO_CACHE1_ENGINE_7__BITSHIFT                                      28
#define NV_PFIFO_CACHE1_ENGINE_7_SW                                             0
#define NV_PFIFO_CACHE1_ENGINE_7_GRAPHICS                                       1
#define NV_PFIFO_CACHE1_ENGINE_7_DVD                                            2
        NV_PFIFO_CACHE0_METHOD_0                    = 0x00001100,
#define NV_PFIFO_CACHE0_METHOD_ADDRESS                                          0x00001ffc
#define NV_PFIFO_CACHE0_METHOD_ADDRESS__BITSHIFT                                2
#define NV_PFIFO_CACHE0_METHOD_SUBCHANNEL                                       0x0000e000
#define NV_PFIFO_CACHE0_METHOD_SUBCHANNEL__BITSHIFT                             13
        NV_PFIFO_CACHE1_METHOD_0                    = 0x00001800,
        NV_PFIFO_CACHE1_METHOD_1                    = 0x00001808,
        NV_PFIFO_CACHE1_METHOD_2                    = 0x00001810,
        NV_PFIFO_CACHE1_METHOD_3                    = 0x00001818,
        NV_PFIFO_CACHE1_METHOD_4                    = 0x00001820,
        NV_PFIFO_CACHE1_METHOD_5                    = 0x00001828,
        NV_PFIFO_CACHE1_METHOD_6                    = 0x00001830,
        NV_PFIFO_CACHE1_METHOD_7                    = 0x00001838,
        NV_PFIFO_CACHE1_METHOD_8                    = 0x00001840,
        NV_PFIFO_CACHE1_METHOD_9                    = 0x00001848,
        NV_PFIFO_CACHE1_METHOD_10                   = 0x00001850,
        NV_PFIFO_CACHE1_METHOD_11                   = 0x00001858,
        NV_PFIFO_CACHE1_METHOD_12                   = 0x00001860,
        NV_PFIFO_CACHE1_METHOD_13                   = 0x00001868,
        NV_PFIFO_CACHE1_METHOD_14                   = 0x00001870,
        NV_PFIFO_CACHE1_METHOD_15                   = 0x00001878,
        NV_PFIFO_CACHE1_METHOD_16                   = 0x00001880,
        NV_PFIFO_CACHE1_METHOD_17                   = 0x00001888,
        NV_PFIFO_CACHE1_METHOD_18                   = 0x00001890,
        NV_PFIFO_CACHE1_METHOD_19                   = 0x00001898,
        NV_PFIFO_CACHE1_METHOD_20                   = 0x000018a0,
        NV_PFIFO_CACHE1_METHOD_21                   = 0x000018a8,
        NV_PFIFO_CACHE1_METHOD_22                   = 0x000018b0,
        NV_PFIFO_CACHE1_METHOD_23                   = 0x000018b8,
        NV_PFIFO_CACHE1_METHOD_24                   = 0x000018c0,
        NV_PFIFO_CACHE1_METHOD_25                   = 0x000018c8,
        NV_PFIFO_CACHE1_METHOD_26                   = 0x000018d0,
        NV_PFIFO_CACHE1_METHOD_27                   = 0x000018d8,
        NV_PFIFO_CACHE1_METHOD_28                   = 0x000018e0,
        NV_PFIFO_CACHE1_METHOD_29                   = 0x000018e8,
        NV_PFIFO_CACHE1_METHOD_30                   = 0x000018f0,
        NV_PFIFO_CACHE1_METHOD_31                   = 0x000018f8,
        NV_PFIFO_CACHE1_METHOD_32                   = 0x00001900,
        NV_PFIFO_CACHE1_METHOD_33                   = 0x00001908,
        NV_PFIFO_CACHE1_METHOD_34                   = 0x00001910,
        NV_PFIFO_CACHE1_METHOD_35                   = 0x00001918,
        NV_PFIFO_CACHE1_METHOD_36                   = 0x00001920,
        NV_PFIFO_CACHE1_METHOD_37                   = 0x00001928,
        NV_PFIFO_CACHE1_METHOD_38                   = 0x00001930,
        NV_PFIFO_CACHE1_METHOD_39                   = 0x00001938,
        NV_PFIFO_CACHE1_METHOD_40                   = 0x00001940,
        NV_PFIFO_CACHE1_METHOD_41                   = 0x00001948,
        NV_PFIFO_CACHE1_METHOD_42                   = 0x00001950,
        NV_PFIFO_CACHE1_METHOD_43                   = 0x00001958,
        NV_PFIFO_CACHE1_METHOD_44                   = 0x00001960,
        NV_PFIFO_CACHE1_METHOD_45                   = 0x00001968,
        NV_PFIFO_CACHE1_METHOD_46                   = 0x00001970,
        NV_PFIFO_CACHE1_METHOD_47                   = 0x00001978,
        NV_PFIFO_CACHE1_METHOD_48                   = 0x00001980,
        NV_PFIFO_CACHE1_METHOD_49                   = 0x00001988,
        NV_PFIFO_CACHE1_METHOD_50                   = 0x00001990,
        NV_PFIFO_CACHE1_METHOD_51                   = 0x00001998,
        NV_PFIFO_CACHE1_METHOD_52                   = 0x000019a0,
        NV_PFIFO_CACHE1_METHOD_53                   = 0x000019a8,
        NV_PFIFO_CACHE1_METHOD_54                   = 0x000019b0,
        NV_PFIFO_CACHE1_METHOD_55                   = 0x000019b8,
        NV_PFIFO_CACHE1_METHOD_56                   = 0x000019c0,
        NV_PFIFO_CACHE1_METHOD_57                   = 0x000019c8,
        NV_PFIFO_CACHE1_METHOD_58                   = 0x000019d0,
        NV_PFIFO_CACHE1_METHOD_59                   = 0x000019d8,
        NV_PFIFO_CACHE1_METHOD_60                   = 0x000019e0,
        NV_PFIFO_CACHE1_METHOD_61                   = 0x000019e8,
        NV_PFIFO_CACHE1_METHOD_62                   = 0x000019f0,
        NV_PFIFO_CACHE1_METHOD_63                   = 0x000019f8,
        NV_PFIFO_CACHE1_METHOD_64                   = 0x00001a00,
        NV_PFIFO_CACHE1_METHOD_65                   = 0x00001a08,
        NV_PFIFO_CACHE1_METHOD_66                   = 0x00001a10,
        NV_PFIFO_CACHE1_METHOD_67                   = 0x00001a18,
        NV_PFIFO_CACHE1_METHOD_68                   = 0x00001a20,
        NV_PFIFO_CACHE1_METHOD_69                   = 0x00001a28,
        NV_PFIFO_CACHE1_METHOD_70                   = 0x00001a30,
        NV_PFIFO_CACHE1_METHOD_71                   = 0x00001a38,
        NV_PFIFO_CACHE1_METHOD_72                   = 0x00001a40,
        NV_PFIFO_CACHE1_METHOD_73                   = 0x00001a48,
        NV_PFIFO_CACHE1_METHOD_74                   = 0x00001a50,
        NV_PFIFO_CACHE1_METHOD_75                   = 0x00001a58,
        NV_PFIFO_CACHE1_METHOD_76                   = 0x00001a60,
        NV_PFIFO_CACHE1_METHOD_77                   = 0x00001a68,
        NV_PFIFO_CACHE1_METHOD_78                   = 0x00001a70,
        NV_PFIFO_CACHE1_METHOD_79                   = 0x00001a78,
        NV_PFIFO_CACHE1_METHOD_80                   = 0x00001a80,
        NV_PFIFO_CACHE1_METHOD_81                   = 0x00001a88,
        NV_PFIFO_CACHE1_METHOD_82                   = 0x00001a90,
        NV_PFIFO_CACHE1_METHOD_83                   = 0x00001a98,
        NV_PFIFO_CACHE1_METHOD_84                   = 0x00001aa0,
        NV_PFIFO_CACHE1_METHOD_85                   = 0x00001aa8,
        NV_PFIFO_CACHE1_METHOD_86                   = 0x00001ab0,
        NV_PFIFO_CACHE1_METHOD_87                   = 0x00001ab8,
        NV_PFIFO_CACHE1_METHOD_88                   = 0x00001ac0,
        NV_PFIFO_CACHE1_METHOD_89                   = 0x00001ac8,
        NV_PFIFO_CACHE1_METHOD_90                   = 0x00001ad0,
        NV_PFIFO_CACHE1_METHOD_91                   = 0x00001ad8,
        NV_PFIFO_CACHE1_METHOD_92                   = 0x00001ae0,
        NV_PFIFO_CACHE1_METHOD_93                   = 0x00001ae8,
        NV_PFIFO_CACHE1_METHOD_94                   = 0x00001af0,
        NV_PFIFO_CACHE1_METHOD_95                   = 0x00001af8,
        NV_PFIFO_CACHE1_METHOD_96                   = 0x00001b00,
        NV_PFIFO_CACHE1_METHOD_97                   = 0x00001b08,
        NV_PFIFO_CACHE1_METHOD_98                   = 0x00001b10,
        NV_PFIFO_CACHE1_METHOD_99                   = 0x00001b18,
        NV_PFIFO_CACHE1_METHOD_100                  = 0x00001b20,
        NV_PFIFO_CACHE1_METHOD_101                  = 0x00001b28,
        NV_PFIFO_CACHE1_METHOD_102                  = 0x00001b30,
        NV_PFIFO_CACHE1_METHOD_103                  = 0x00001b38,
        NV_PFIFO_CACHE1_METHOD_104                  = 0x00001b40,
        NV_PFIFO_CACHE1_METHOD_105                  = 0x00001b48,
        NV_PFIFO_CACHE1_METHOD_106                  = 0x00001b50,
        NV_PFIFO_CACHE1_METHOD_107                  = 0x00001b58,
        NV_PFIFO_CACHE1_METHOD_108                  = 0x00001b60,
        NV_PFIFO_CACHE1_METHOD_109                  = 0x00001b68,
        NV_PFIFO_CACHE1_METHOD_110                  = 0x00001b70,
        NV_PFIFO_CACHE1_METHOD_111                  = 0x00001b78,
        NV_PFIFO_CACHE1_METHOD_112                  = 0x00001b80,
        NV_PFIFO_CACHE1_METHOD_113                  = 0x00001b88,
        NV_PFIFO_CACHE1_METHOD_114                  = 0x00001b90,
        NV_PFIFO_CACHE1_METHOD_115                  = 0x00001b98,
        NV_PFIFO_CACHE1_METHOD_116                  = 0x00001ba0,
        NV_PFIFO_CACHE1_METHOD_117                  = 0x00001ba8,
        NV_PFIFO_CACHE1_METHOD_118                  = 0x00001bb0,
        NV_PFIFO_CACHE1_METHOD_119                  = 0x00001bb8,
        NV_PFIFO_CACHE1_METHOD_120                  = 0x00001bc0,
        NV_PFIFO_CACHE1_METHOD_121                  = 0x00001bc8,
        NV_PFIFO_CACHE1_METHOD_122                  = 0x00001bd0,
        NV_PFIFO_CACHE1_METHOD_123                  = 0x00001bd8,
        NV_PFIFO_CACHE1_METHOD_124                  = 0x00001be0,
        NV_PFIFO_CACHE1_METHOD_125                  = 0x00001be8,
        NV_PFIFO_CACHE1_METHOD_126                  = 0x00001bf0,
        NV_PFIFO_CACHE1_METHOD_127                  = 0x00001bf8,
#define NV_PFIFO_CACHE1_METHOD_TYPE                                             0x00000001
#define NV_PFIFO_CACHE1_METHOD_TYPE__BITSHIFT                                   0
#define NV_PFIFO_CACHE1_METHOD_ADDRESS                                          0x00001ffc
#define NV_PFIFO_CACHE1_METHOD_ADDRESS__BITSHIFT                                2
#define NV_PFIFO_CACHE1_METHOD_SUBCHANNEL                                       0x0000e000
#define NV_PFIFO_CACHE1_METHOD_SUBCHANNEL__BITSHIFT                             13
        NV_PFIFO_CACHE1_METHOD_ALIAS_0              = 0x00001c00,
        NV_PFIFO_CACHE1_METHOD_ALIAS_1              = 0x00001c08,
        NV_PFIFO_CACHE1_METHOD_ALIAS_2              = 0x00001c10,
        NV_PFIFO_CACHE1_METHOD_ALIAS_3              = 0x00001c18,
        NV_PFIFO_CACHE1_METHOD_ALIAS_4              = 0x00001c20,
        NV_PFIFO_CACHE1_METHOD_ALIAS_5              = 0x00001c28,
        NV_PFIFO_CACHE1_METHOD_ALIAS_6              = 0x00001c30,
        NV_PFIFO_CACHE1_METHOD_ALIAS_7              = 0x00001c38,
        NV_PFIFO_CACHE1_METHOD_ALIAS_8              = 0x00001c40,
        NV_PFIFO_CACHE1_METHOD_ALIAS_9              = 0x00001c48,
        NV_PFIFO_CACHE1_METHOD_ALIAS_10             = 0x00001c50,
        NV_PFIFO_CACHE1_METHOD_ALIAS_11             = 0x00001c58,
        NV_PFIFO_CACHE1_METHOD_ALIAS_12             = 0x00001c60,
        NV_PFIFO_CACHE1_METHOD_ALIAS_13             = 0x00001c68,
        NV_PFIFO_CACHE1_METHOD_ALIAS_14             = 0x00001c70,
        NV_PFIFO_CACHE1_METHOD_ALIAS_15             = 0x00001c78,
        NV_PFIFO_CACHE1_METHOD_ALIAS_16             = 0x00001c80,
        NV_PFIFO_CACHE1_METHOD_ALIAS_17             = 0x00001c88,
        NV_PFIFO_CACHE1_METHOD_ALIAS_18             = 0x00001c90,
        NV_PFIFO_CACHE1_METHOD_ALIAS_19             = 0x00001c98,
        NV_PFIFO_CACHE1_METHOD_ALIAS_20             = 0x00001ca0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_21             = 0x00001ca8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_22             = 0x00001cb0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_23             = 0x00001cb8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_24             = 0x00001cc0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_25             = 0x00001cc8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_26             = 0x00001cd0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_27             = 0x00001cd8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_28             = 0x00001ce0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_29             = 0x00001ce8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_30             = 0x00001cf0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_31             = 0x00001cf8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_32             = 0x00001d00,
        NV_PFIFO_CACHE1_METHOD_ALIAS_33             = 0x00001d08,
        NV_PFIFO_CACHE1_METHOD_ALIAS_34             = 0x00001d10,
        NV_PFIFO_CACHE1_METHOD_ALIAS_35             = 0x00001d18,
        NV_PFIFO_CACHE1_METHOD_ALIAS_36             = 0x00001d20,
        NV_PFIFO_CACHE1_METHOD_ALIAS_37             = 0x00001d28,
        NV_PFIFO_CACHE1_METHOD_ALIAS_38             = 0x00001d30,
        NV_PFIFO_CACHE1_METHOD_ALIAS_39             = 0x00001d38,
        NV_PFIFO_CACHE1_METHOD_ALIAS_40             = 0x00001d40,
        NV_PFIFO_CACHE1_METHOD_ALIAS_41             = 0x00001d48,
        NV_PFIFO_CACHE1_METHOD_ALIAS_42             = 0x00001d50,
        NV_PFIFO_CACHE1_METHOD_ALIAS_43             = 0x00001d58,
        NV_PFIFO_CACHE1_METHOD_ALIAS_44             = 0x00001d60,
        NV_PFIFO_CACHE1_METHOD_ALIAS_45             = 0x00001d68,
        NV_PFIFO_CACHE1_METHOD_ALIAS_46             = 0x00001d70,
        NV_PFIFO_CACHE1_METHOD_ALIAS_47             = 0x00001d78,
        NV_PFIFO_CACHE1_METHOD_ALIAS_48             = 0x00001d80,
        NV_PFIFO_CACHE1_METHOD_ALIAS_49             = 0x00001d88,
        NV_PFIFO_CACHE1_METHOD_ALIAS_50             = 0x00001d90,
        NV_PFIFO_CACHE1_METHOD_ALIAS_51             = 0x00001d98,
        NV_PFIFO_CACHE1_METHOD_ALIAS_52             = 0x00001da0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_53             = 0x00001da8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_54             = 0x00001db0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_55             = 0x00001db8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_56             = 0x00001dc0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_57             = 0x00001dc8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_58             = 0x00001dd0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_59             = 0x00001dd8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_60             = 0x00001de0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_61             = 0x00001de8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_62             = 0x00001df0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_63             = 0x00001df8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_64             = 0x00001e00,
        NV_PFIFO_CACHE1_METHOD_ALIAS_65             = 0x00001e08,
        NV_PFIFO_CACHE1_METHOD_ALIAS_66             = 0x00001e10,
        NV_PFIFO_CACHE1_METHOD_ALIAS_67             = 0x00001e18,
        NV_PFIFO_CACHE1_METHOD_ALIAS_68             = 0x00001e20,
        NV_PFIFO_CACHE1_METHOD_ALIAS_69             = 0x00001e28,
        NV_PFIFO_CACHE1_METHOD_ALIAS_70             = 0x00001e30,
        NV_PFIFO_CACHE1_METHOD_ALIAS_71             = 0x00001e38,
        NV_PFIFO_CACHE1_METHOD_ALIAS_72             = 0x00001e40,
        NV_PFIFO_CACHE1_METHOD_ALIAS_73             = 0x00001e48,
        NV_PFIFO_CACHE1_METHOD_ALIAS_74             = 0x00001e50,
        NV_PFIFO_CACHE1_METHOD_ALIAS_75             = 0x00001e58,
        NV_PFIFO_CACHE1_METHOD_ALIAS_76             = 0x00001e60,
        NV_PFIFO_CACHE1_METHOD_ALIAS_77             = 0x00001e68,
        NV_PFIFO_CACHE1_METHOD_ALIAS_78             = 0x00001e70,
        NV_PFIFO_CACHE1_METHOD_ALIAS_79             = 0x00001e78,
        NV_PFIFO_CACHE1_METHOD_ALIAS_80             = 0x00001e80,
        NV_PFIFO_CACHE1_METHOD_ALIAS_81             = 0x00001e88,
        NV_PFIFO_CACHE1_METHOD_ALIAS_82             = 0x00001e90,
        NV_PFIFO_CACHE1_METHOD_ALIAS_83             = 0x00001e98,
        NV_PFIFO_CACHE1_METHOD_ALIAS_84             = 0x00001ea0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_85             = 0x00001ea8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_86             = 0x00001eb0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_87             = 0x00001eb8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_88             = 0x00001ec0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_89             = 0x00001ec8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_90             = 0x00001ed0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_91             = 0x00001ed8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_92             = 0x00001ee0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_93             = 0x00001ee8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_94             = 0x00001ef0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_95             = 0x00001ef8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_96             = 0x00001f00,
        NV_PFIFO_CACHE1_METHOD_ALIAS_97             = 0x00001f08,
        NV_PFIFO_CACHE1_METHOD_ALIAS_98             = 0x00001f10,
        NV_PFIFO_CACHE1_METHOD_ALIAS_99             = 0x00001f18,
        NV_PFIFO_CACHE1_METHOD_ALIAS_100            = 0x00001f20,
        NV_PFIFO_CACHE1_METHOD_ALIAS_101            = 0x00001f28,
        NV_PFIFO_CACHE1_METHOD_ALIAS_102            = 0x00001f30,
        NV_PFIFO_CACHE1_METHOD_ALIAS_103            = 0x00001f38,
        NV_PFIFO_CACHE1_METHOD_ALIAS_104            = 0x00001f40,
        NV_PFIFO_CACHE1_METHOD_ALIAS_105            = 0x00001f48,
        NV_PFIFO_CACHE1_METHOD_ALIAS_106            = 0x00001f50,
        NV_PFIFO_CACHE1_METHOD_ALIAS_107            = 0x00001f58,
        NV_PFIFO_CACHE1_METHOD_ALIAS_108            = 0x00001f60,
        NV_PFIFO_CACHE1_METHOD_ALIAS_109            = 0x00001f68,
        NV_PFIFO_CACHE1_METHOD_ALIAS_110            = 0x00001f70,
        NV_PFIFO_CACHE1_METHOD_ALIAS_111            = 0x00001f78,
        NV_PFIFO_CACHE1_METHOD_ALIAS_112            = 0x00001f80,
        NV_PFIFO_CACHE1_METHOD_ALIAS_113            = 0x00001f88,
        NV_PFIFO_CACHE1_METHOD_ALIAS_114            = 0x00001f90,
        NV_PFIFO_CACHE1_METHOD_ALIAS_115            = 0x00001f98,
        NV_PFIFO_CACHE1_METHOD_ALIAS_116            = 0x00001fa0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_117            = 0x00001fa8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_118            = 0x00001fb0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_119            = 0x00001fb8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_120            = 0x00001fc0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_121            = 0x00001fc8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_122            = 0x00001fd0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_123            = 0x00001fd8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_124            = 0x00001fe0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_125            = 0x00001fe8,
        NV_PFIFO_CACHE1_METHOD_ALIAS_126            = 0x00001ff0,
        NV_PFIFO_CACHE1_METHOD_ALIAS_127            = 0x00001ff8,
        NV_PFIFO_CACHE0_DATA_0                      = 0x00001104,
        NV_PFIFO_CACHE1_DATA_0                      = 0x00001804,
        NV_PFIFO_CACHE1_DATA_1                      = 0x0000180c,
        NV_PFIFO_CACHE1_DATA_2                      = 0x00001814,
        NV_PFIFO_CACHE1_DATA_3                      = 0x0000181c,
        NV_PFIFO_CACHE1_DATA_4                      = 0x00001824,
        NV_PFIFO_CACHE1_DATA_5                      = 0x0000182c,
        NV_PFIFO_CACHE1_DATA_6                      = 0x00001834,
        NV_PFIFO_CACHE1_DATA_7                      = 0x0000183c,
        NV_PFIFO_CACHE1_DATA_8                      = 0x00001844,
        NV_PFIFO_CACHE1_DATA_9                      = 0x0000184c,
        NV_PFIFO_CACHE1_DATA_10                     = 0x00001854,
        NV_PFIFO_CACHE1_DATA_11                     = 0x0000185c,
        NV_PFIFO_CACHE1_DATA_12                     = 0x00001864,
        NV_PFIFO_CACHE1_DATA_13                     = 0x0000186c,
        NV_PFIFO_CACHE1_DATA_14                     = 0x00001874,
        NV_PFIFO_CACHE1_DATA_15                     = 0x0000187c,
        NV_PFIFO_CACHE1_DATA_16                     = 0x00001884,
        NV_PFIFO_CACHE1_DATA_17                     = 0x0000188c,
        NV_PFIFO_CACHE1_DATA_18                     = 0x00001894,
        NV_PFIFO_CACHE1_DATA_19                     = 0x0000189c,
        NV_PFIFO_CACHE1_DATA_20                     = 0x000018a4,
        NV_PFIFO_CACHE1_DATA_21                     = 0x000018ac,
        NV_PFIFO_CACHE1_DATA_22                     = 0x000018b4,
        NV_PFIFO_CACHE1_DATA_23                     = 0x000018bc,
        NV_PFIFO_CACHE1_DATA_24                     = 0x000018c4,
        NV_PFIFO_CACHE1_DATA_25                     = 0x000018cc,
        NV_PFIFO_CACHE1_DATA_26                     = 0x000018d4,
        NV_PFIFO_CACHE1_DATA_27                     = 0x000018dc,
        NV_PFIFO_CACHE1_DATA_28                     = 0x000018e4,
        NV_PFIFO_CACHE1_DATA_29                     = 0x000018ec,
        NV_PFIFO_CACHE1_DATA_30                     = 0x000018f4,
        NV_PFIFO_CACHE1_DATA_31                     = 0x000018fc,
        NV_PFIFO_CACHE1_DATA_32                     = 0x00001904,
        NV_PFIFO_CACHE1_DATA_33                     = 0x0000190c,
        NV_PFIFO_CACHE1_DATA_34                     = 0x00001914,
        NV_PFIFO_CACHE1_DATA_35                     = 0x0000191c,
        NV_PFIFO_CACHE1_DATA_36                     = 0x00001924,
        NV_PFIFO_CACHE1_DATA_37                     = 0x0000192c,
        NV_PFIFO_CACHE1_DATA_38                     = 0x00001934,
        NV_PFIFO_CACHE1_DATA_39                     = 0x0000193c,
        NV_PFIFO_CACHE1_DATA_40                     = 0x00001944,
        NV_PFIFO_CACHE1_DATA_41                     = 0x0000194c,
        NV_PFIFO_CACHE1_DATA_42                     = 0x00001954,
        NV_PFIFO_CACHE1_DATA_43                     = 0x0000195c,
        NV_PFIFO_CACHE1_DATA_44                     = 0x00001964,
        NV_PFIFO_CACHE1_DATA_45                     = 0x0000196c,
        NV_PFIFO_CACHE1_DATA_46                     = 0x00001974,
        NV_PFIFO_CACHE1_DATA_47                     = 0x0000197c,
        NV_PFIFO_CACHE1_DATA_48                     = 0x00001984,
        NV_PFIFO_CACHE1_DATA_49                     = 0x0000198c,
        NV_PFIFO_CACHE1_DATA_50                     = 0x00001994,
        NV_PFIFO_CACHE1_DATA_51                     = 0x0000199c,
        NV_PFIFO_CACHE1_DATA_52                     = 0x000019a4,
        NV_PFIFO_CACHE1_DATA_53                     = 0x000019ac,
        NV_PFIFO_CACHE1_DATA_54                     = 0x000019b4,
        NV_PFIFO_CACHE1_DATA_55                     = 0x000019bc,
        NV_PFIFO_CACHE1_DATA_56                     = 0x000019c4,
        NV_PFIFO_CACHE1_DATA_57                     = 0x000019cc,
        NV_PFIFO_CACHE1_DATA_58                     = 0x000019d4,
        NV_PFIFO_CACHE1_DATA_59                     = 0x000019dc,
        NV_PFIFO_CACHE1_DATA_60                     = 0x000019e4,
        NV_PFIFO_CACHE1_DATA_61                     = 0x000019ec,
        NV_PFIFO_CACHE1_DATA_62                     = 0x000019f4,
        NV_PFIFO_CACHE1_DATA_63                     = 0x000019fc,
        NV_PFIFO_CACHE1_DATA_64                     = 0x00001a04,
        NV_PFIFO_CACHE1_DATA_65                     = 0x00001a0c,
        NV_PFIFO_CACHE1_DATA_66                     = 0x00001a14,
        NV_PFIFO_CACHE1_DATA_67                     = 0x00001a1c,
        NV_PFIFO_CACHE1_DATA_68                     = 0x00001a24,
        NV_PFIFO_CACHE1_DATA_69                     = 0x00001a2c,
        NV_PFIFO_CACHE1_DATA_70                     = 0x00001a34,
        NV_PFIFO_CACHE1_DATA_71                     = 0x00001a3c,
        NV_PFIFO_CACHE1_DATA_72                     = 0x00001a44,
        NV_PFIFO_CACHE1_DATA_73                     = 0x00001a4c,
        NV_PFIFO_CACHE1_DATA_74                     = 0x00001a54,
        NV_PFIFO_CACHE1_DATA_75                     = 0x00001a5c,
        NV_PFIFO_CACHE1_DATA_76                     = 0x00001a64,
        NV_PFIFO_CACHE1_DATA_77                     = 0x00001a6c,
        NV_PFIFO_CACHE1_DATA_78                     = 0x00001a74,
        NV_PFIFO_CACHE1_DATA_79                     = 0x00001a7c,
        NV_PFIFO_CACHE1_DATA_80                     = 0x00001a84,
        NV_PFIFO_CACHE1_DATA_81                     = 0x00001a8c,
        NV_PFIFO_CACHE1_DATA_82                     = 0x00001a94,
        NV_PFIFO_CACHE1_DATA_83                     = 0x00001a9c,
        NV_PFIFO_CACHE1_DATA_84                     = 0x00001aa4,
        NV_PFIFO_CACHE1_DATA_85                     = 0x00001aac,
        NV_PFIFO_CACHE1_DATA_86                     = 0x00001ab4,
        NV_PFIFO_CACHE1_DATA_87                     = 0x00001abc,
        NV_PFIFO_CACHE1_DATA_88                     = 0x00001ac4,
        NV_PFIFO_CACHE1_DATA_89                     = 0x00001acc,
        NV_PFIFO_CACHE1_DATA_90                     = 0x00001ad4,
        NV_PFIFO_CACHE1_DATA_91                     = 0x00001adc,
        NV_PFIFO_CACHE1_DATA_92                     = 0x00001ae4,
        NV_PFIFO_CACHE1_DATA_93                     = 0x00001aec,
        NV_PFIFO_CACHE1_DATA_94                     = 0x00001af4,
        NV_PFIFO_CACHE1_DATA_95                     = 0x00001afc,
        NV_PFIFO_CACHE1_DATA_96                     = 0x00001b04,
        NV_PFIFO_CACHE1_DATA_97                     = 0x00001b0c,
        NV_PFIFO_CACHE1_DATA_98                     = 0x00001b14,
        NV_PFIFO_CACHE1_DATA_99                     = 0x00001b1c,
        NV_PFIFO_CACHE1_DATA_100                    = 0x00001b24,
        NV_PFIFO_CACHE1_DATA_101                    = 0x00001b2c,
        NV_PFIFO_CACHE1_DATA_102                    = 0x00001b34,
        NV_PFIFO_CACHE1_DATA_103                    = 0x00001b3c,
        NV_PFIFO_CACHE1_DATA_104                    = 0x00001b44,
        NV_PFIFO_CACHE1_DATA_105                    = 0x00001b4c,
        NV_PFIFO_CACHE1_DATA_106                    = 0x00001b54,
        NV_PFIFO_CACHE1_DATA_107                    = 0x00001b5c,
        NV_PFIFO_CACHE1_DATA_108                    = 0x00001b64,
        NV_PFIFO_CACHE1_DATA_109                    = 0x00001b6c,
        NV_PFIFO_CACHE1_DATA_110                    = 0x00001b74,
        NV_PFIFO_CACHE1_DATA_111                    = 0x00001b7c,
        NV_PFIFO_CACHE1_DATA_112                    = 0x00001b84,
        NV_PFIFO_CACHE1_DATA_113                    = 0x00001b8c,
        NV_PFIFO_CACHE1_DATA_114                    = 0x00001b94,
        NV_PFIFO_CACHE1_DATA_115                    = 0x00001b9c,
        NV_PFIFO_CACHE1_DATA_116                    = 0x00001ba4,
        NV_PFIFO_CACHE1_DATA_117                    = 0x00001bac,
        NV_PFIFO_CACHE1_DATA_118                    = 0x00001bb4,
        NV_PFIFO_CACHE1_DATA_119                    = 0x00001bbc,
        NV_PFIFO_CACHE1_DATA_120                    = 0x00001bc4,
        NV_PFIFO_CACHE1_DATA_121                    = 0x00001bcc,
        NV_PFIFO_CACHE1_DATA_122                    = 0x00001bd4,
        NV_PFIFO_CACHE1_DATA_123                    = 0x00001bdc,
        NV_PFIFO_CACHE1_DATA_124                    = 0x00001be4,
        NV_PFIFO_CACHE1_DATA_125                    = 0x00001bec,
        NV_PFIFO_CACHE1_DATA_126                    = 0x00001bf4,
        NV_PFIFO_CACHE1_DATA_127                    = 0x00001bfc,
        NV_PFIFO_CACHE1_DATA_ALIAS_0                = 0x00001c04,
        NV_PFIFO_CACHE1_DATA_ALIAS_1                = 0x00001c0c,
        NV_PFIFO_CACHE1_DATA_ALIAS_2                = 0x00001c14,
        NV_PFIFO_CACHE1_DATA_ALIAS_3                = 0x00001c1c,
        NV_PFIFO_CACHE1_DATA_ALIAS_4                = 0x00001c24,
        NV_PFIFO_CACHE1_DATA_ALIAS_5                = 0x00001c2c,
        NV_PFIFO_CACHE1_DATA_ALIAS_6                = 0x00001c34,
        NV_PFIFO_CACHE1_DATA_ALIAS_7                = 0x00001c3c,
        NV_PFIFO_CACHE1_DATA_ALIAS_8                = 0x00001c44,
        NV_PFIFO_CACHE1_DATA_ALIAS_9                = 0x00001c4c,
        NV_PFIFO_CACHE1_DATA_ALIAS_10               = 0x00001c54,
        NV_PFIFO_CACHE1_DATA_ALIAS_11               = 0x00001c5c,
        NV_PFIFO_CACHE1_DATA_ALIAS_12               = 0x00001c64,
        NV_PFIFO_CACHE1_DATA_ALIAS_13               = 0x00001c6c,
        NV_PFIFO_CACHE1_DATA_ALIAS_14               = 0x00001c74,
        NV_PFIFO_CACHE1_DATA_ALIAS_15               = 0x00001c7c,
        NV_PFIFO_CACHE1_DATA_ALIAS_16               = 0x00001c84,
        NV_PFIFO_CACHE1_DATA_ALIAS_17               = 0x00001c8c,
        NV_PFIFO_CACHE1_DATA_ALIAS_18               = 0x00001c94,
        NV_PFIFO_CACHE1_DATA_ALIAS_19               = 0x00001c9c,
        NV_PFIFO_CACHE1_DATA_ALIAS_20               = 0x00001ca4,
        NV_PFIFO_CACHE1_DATA_ALIAS_21               = 0x00001cac,
        NV_PFIFO_CACHE1_DATA_ALIAS_22               = 0x00001cb4,
        NV_PFIFO_CACHE1_DATA_ALIAS_23               = 0x00001cbc,
        NV_PFIFO_CACHE1_DATA_ALIAS_24               = 0x00001cc4,
        NV_PFIFO_CACHE1_DATA_ALIAS_25               = 0x00001ccc,
        NV_PFIFO_CACHE1_DATA_ALIAS_26               = 0x00001cd4,
        NV_PFIFO_CACHE1_DATA_ALIAS_27               = 0x00001cdc,
        NV_PFIFO_CACHE1_DATA_ALIAS_28               = 0x00001ce4,
        NV_PFIFO_CACHE1_DATA_ALIAS_29               = 0x00001cec,
        NV_PFIFO_CACHE1_DATA_ALIAS_30               = 0x00001cf4,
        NV_PFIFO_CACHE1_DATA_ALIAS_31               = 0x00001cfc,
        NV_PFIFO_CACHE1_DATA_ALIAS_32               = 0x00001d04,
        NV_PFIFO_CACHE1_DATA_ALIAS_33               = 0x00001d0c,
        NV_PFIFO_CACHE1_DATA_ALIAS_34               = 0x00001d14,
        NV_PFIFO_CACHE1_DATA_ALIAS_35               = 0x00001d1c,
        NV_PFIFO_CACHE1_DATA_ALIAS_36               = 0x00001d24,
        NV_PFIFO_CACHE1_DATA_ALIAS_37               = 0x00001d2c,
        NV_PFIFO_CACHE1_DATA_ALIAS_38               = 0x00001d34,
        NV_PFIFO_CACHE1_DATA_ALIAS_39               = 0x00001d3c,
        NV_PFIFO_CACHE1_DATA_ALIAS_40               = 0x00001d44,
        NV_PFIFO_CACHE1_DATA_ALIAS_41               = 0x00001d4c,
        NV_PFIFO_CACHE1_DATA_ALIAS_42               = 0x00001d54,
        NV_PFIFO_CACHE1_DATA_ALIAS_43               = 0x00001d5c,
        NV_PFIFO_CACHE1_DATA_ALIAS_44               = 0x00001d64,
        NV_PFIFO_CACHE1_DATA_ALIAS_45               = 0x00001d6c,
        NV_PFIFO_CACHE1_DATA_ALIAS_46               = 0x00001d74,
        NV_PFIFO_CACHE1_DATA_ALIAS_47               = 0x00001d7c,
        NV_PFIFO_CACHE1_DATA_ALIAS_48               = 0x00001d84,
        NV_PFIFO_CACHE1_DATA_ALIAS_49               = 0x00001d8c,
        NV_PFIFO_CACHE1_DATA_ALIAS_50               = 0x00001d94,
        NV_PFIFO_CACHE1_DATA_ALIAS_51               = 0x00001d9c,
        NV_PFIFO_CACHE1_DATA_ALIAS_52               = 0x00001da4,
        NV_PFIFO_CACHE1_DATA_ALIAS_53               = 0x00001dac,
        NV_PFIFO_CACHE1_DATA_ALIAS_54               = 0x00001db4,
        NV_PFIFO_CACHE1_DATA_ALIAS_55               = 0x00001dbc,
        NV_PFIFO_CACHE1_DATA_ALIAS_56               = 0x00001dc4,
        NV_PFIFO_CACHE1_DATA_ALIAS_57               = 0x00001dcc,
        NV_PFIFO_CACHE1_DATA_ALIAS_58               = 0x00001dd4,
        NV_PFIFO_CACHE1_DATA_ALIAS_59               = 0x00001ddc,
        NV_PFIFO_CACHE1_DATA_ALIAS_60               = 0x00001de4,
        NV_PFIFO_CACHE1_DATA_ALIAS_61               = 0x00001dec,
        NV_PFIFO_CACHE1_DATA_ALIAS_62               = 0x00001df4,
        NV_PFIFO_CACHE1_DATA_ALIAS_63               = 0x00001dfc,
        NV_PFIFO_CACHE1_DATA_ALIAS_64               = 0x00001e04,
        NV_PFIFO_CACHE1_DATA_ALIAS_65               = 0x00001e0c,
        NV_PFIFO_CACHE1_DATA_ALIAS_66               = 0x00001e14,
        NV_PFIFO_CACHE1_DATA_ALIAS_67               = 0x00001e1c,
        NV_PFIFO_CACHE1_DATA_ALIAS_68               = 0x00001e24,
        NV_PFIFO_CACHE1_DATA_ALIAS_69               = 0x00001e2c,
        NV_PFIFO_CACHE1_DATA_ALIAS_70               = 0x00001e34,
        NV_PFIFO_CACHE1_DATA_ALIAS_71               = 0x00001e3c,
        NV_PFIFO_CACHE1_DATA_ALIAS_72               = 0x00001e44,
        NV_PFIFO_CACHE1_DATA_ALIAS_73               = 0x00001e4c,
        NV_PFIFO_CACHE1_DATA_ALIAS_74               = 0x00001e54,
        NV_PFIFO_CACHE1_DATA_ALIAS_75               = 0x00001e5c,
        NV_PFIFO_CACHE1_DATA_ALIAS_76               = 0x00001e64,
        NV_PFIFO_CACHE1_DATA_ALIAS_77               = 0x00001e6c,
        NV_PFIFO_CACHE1_DATA_ALIAS_78               = 0x00001e74,
        NV_PFIFO_CACHE1_DATA_ALIAS_79               = 0x00001e7c,
        NV_PFIFO_CACHE1_DATA_ALIAS_80               = 0x00001e84,
        NV_PFIFO_CACHE1_DATA_ALIAS_81               = 0x00001e8c,
        NV_PFIFO_CACHE1_DATA_ALIAS_82               = 0x00001e94,
        NV_PFIFO_CACHE1_DATA_ALIAS_83               = 0x00001e9c,
        NV_PFIFO_CACHE1_DATA_ALIAS_84               = 0x00001ea4,
        NV_PFIFO_CACHE1_DATA_ALIAS_85               = 0x00001eac,
        NV_PFIFO_CACHE1_DATA_ALIAS_86               = 0x00001eb4,
        NV_PFIFO_CACHE1_DATA_ALIAS_87               = 0x00001ebc,
        NV_PFIFO_CACHE1_DATA_ALIAS_88               = 0x00001ec4,
        NV_PFIFO_CACHE1_DATA_ALIAS_89               = 0x00001ecc,
        NV_PFIFO_CACHE1_DATA_ALIAS_90               = 0x00001ed4,
        NV_PFIFO_CACHE1_DATA_ALIAS_91               = 0x00001edc,
        NV_PFIFO_CACHE1_DATA_ALIAS_92               = 0x00001ee4,
        NV_PFIFO_CACHE1_DATA_ALIAS_93               = 0x00001eec,
        NV_PFIFO_CACHE1_DATA_ALIAS_94               = 0x00001ef4,
        NV_PFIFO_CACHE1_DATA_ALIAS_95               = 0x00001efc,
        NV_PFIFO_CACHE1_DATA_ALIAS_96               = 0x00001f04,
        NV_PFIFO_CACHE1_DATA_ALIAS_97               = 0x00001f0c,
        NV_PFIFO_CACHE1_DATA_ALIAS_98               = 0x00001f14,
        NV_PFIFO_CACHE1_DATA_ALIAS_99               = 0x00001f1c,
        NV_PFIFO_CACHE1_DATA_ALIAS_100              = 0x00001f24,
        NV_PFIFO_CACHE1_DATA_ALIAS_101              = 0x00001f2c,
        NV_PFIFO_CACHE1_DATA_ALIAS_102              = 0x00001f34,
        NV_PFIFO_CACHE1_DATA_ALIAS_103              = 0x00001f3c,
        NV_PFIFO_CACHE1_DATA_ALIAS_104              = 0x00001f44,
        NV_PFIFO_CACHE1_DATA_ALIAS_105              = 0x00001f4c,
        NV_PFIFO_CACHE1_DATA_ALIAS_106              = 0x00001f54,
        NV_PFIFO_CACHE1_DATA_ALIAS_107              = 0x00001f5c,
        NV_PFIFO_CACHE1_DATA_ALIAS_108              = 0x00001f64,
        NV_PFIFO_CACHE1_DATA_ALIAS_109              = 0x00001f6c,
        NV_PFIFO_CACHE1_DATA_ALIAS_110              = 0x00001f74,
        NV_PFIFO_CACHE1_DATA_ALIAS_111              = 0x00001f7c,
        NV_PFIFO_CACHE1_DATA_ALIAS_112              = 0x00001f84,
        NV_PFIFO_CACHE1_DATA_ALIAS_113              = 0x00001f8c,
        NV_PFIFO_CACHE1_DATA_ALIAS_114              = 0x00001f94,
        NV_PFIFO_CACHE1_DATA_ALIAS_115              = 0x00001f9c,
        NV_PFIFO_CACHE1_DATA_ALIAS_116              = 0x00001fa4,
        NV_PFIFO_CACHE1_DATA_ALIAS_117              = 0x00001fac,
        NV_PFIFO_CACHE1_DATA_ALIAS_118              = 0x00001fb4,
        NV_PFIFO_CACHE1_DATA_ALIAS_119              = 0x00001fbc,
        NV_PFIFO_CACHE1_DATA_ALIAS_120              = 0x00001fc4,
        NV_PFIFO_CACHE1_DATA_ALIAS_121              = 0x00001fcc,
        NV_PFIFO_CACHE1_DATA_ALIAS_122              = 0x00001fd4,
        NV_PFIFO_CACHE1_DATA_ALIAS_123              = 0x00001fdc,
        NV_PFIFO_CACHE1_DATA_ALIAS_124              = 0x00001fe4,
        NV_PFIFO_CACHE1_DATA_ALIAS_125              = 0x00001fec,
        NV_PFIFO_CACHE1_DATA_ALIAS_126              = 0x00001ff4,
        NV_PFIFO_CACHE1_DATA_ALIAS_127              = 0x00001ffc,
        NV_PFIFO_DEVICE_0                           = 0x00000800,
        NV_PFIFO_DEVICE_1                           = 0x00000804,
        NV_PFIFO_DEVICE_2                           = 0x00000808,
        NV_PFIFO_DEVICE_3                           = 0x0000080c,
        NV_PFIFO_DEVICE_4                           = 0x00000810,
        NV_PFIFO_DEVICE_5                           = 0x00000814,
        NV_PFIFO_DEVICE_6                           = 0x00000818,
        NV_PFIFO_DEVICE_7                           = 0x0000081c,
        NV_PFIFO_DEVICE_8                           = 0x00000820,
        NV_PFIFO_DEVICE_9                           = 0x00000824,
        NV_PFIFO_DEVICE_10                          = 0x00000828,
        NV_PFIFO_DEVICE_11                          = 0x0000082c,
        NV_PFIFO_DEVICE_12                          = 0x00000830,
        NV_PFIFO_DEVICE_13                          = 0x00000834,
        NV_PFIFO_DEVICE_14                          = 0x00000838,
        NV_PFIFO_DEVICE_15                          = 0x0000083c,
        NV_PFIFO_DEVICE_16                          = 0x00000840,
        NV_PFIFO_DEVICE_17                          = 0x00000844,
        NV_PFIFO_DEVICE_18                          = 0x00000848,
        NV_PFIFO_DEVICE_19                          = 0x0000084c,
        NV_PFIFO_DEVICE_20                          = 0x00000850,
        NV_PFIFO_DEVICE_21                          = 0x00000854,
        NV_PFIFO_DEVICE_22                          = 0x00000858,
        NV_PFIFO_DEVICE_23                          = 0x0000085c,
        NV_PFIFO_DEVICE_24                          = 0x00000860,
        NV_PFIFO_DEVICE_25                          = 0x00000864,
        NV_PFIFO_DEVICE_26                          = 0x00000868,
        NV_PFIFO_DEVICE_27                          = 0x0000086c,
        NV_PFIFO_DEVICE_28                          = 0x00000870,
        NV_PFIFO_DEVICE_29                          = 0x00000874,
        NV_PFIFO_DEVICE_30                          = 0x00000878,
        NV_PFIFO_DEVICE_31                          = 0x0000087c,
        NV_PFIFO_DEVICE_32                          = 0x00000880,
        NV_PFIFO_DEVICE_33                          = 0x00000884,
        NV_PFIFO_DEVICE_34                          = 0x00000888,
        NV_PFIFO_DEVICE_35                          = 0x0000088c,
        NV_PFIFO_DEVICE_36                          = 0x00000890,
        NV_PFIFO_DEVICE_37                          = 0x00000894,
        NV_PFIFO_DEVICE_38                          = 0x00000898,
        NV_PFIFO_DEVICE_39                          = 0x0000089c,
        NV_PFIFO_DEVICE_40                          = 0x000008a0,
        NV_PFIFO_DEVICE_41                          = 0x000008a4,
        NV_PFIFO_DEVICE_42                          = 0x000008a8,
        NV_PFIFO_DEVICE_43                          = 0x000008ac,
        NV_PFIFO_DEVICE_44                          = 0x000008b0,
        NV_PFIFO_DEVICE_45                          = 0x000008b4,
        NV_PFIFO_DEVICE_46                          = 0x000008b8,
        NV_PFIFO_DEVICE_47                          = 0x000008bc,
        NV_PFIFO_DEVICE_48                          = 0x000008c0,
        NV_PFIFO_DEVICE_49                          = 0x000008c4,
        NV_PFIFO_DEVICE_50                          = 0x000008c8,
        NV_PFIFO_DEVICE_51                          = 0x000008cc,
        NV_PFIFO_DEVICE_52                          = 0x000008d0,
        NV_PFIFO_DEVICE_53                          = 0x000008d4,
        NV_PFIFO_DEVICE_54                          = 0x000008d8,
        NV_PFIFO_DEVICE_55                          = 0x000008dc,
        NV_PFIFO_DEVICE_56                          = 0x000008e0,
        NV_PFIFO_DEVICE_57                          = 0x000008e4,
        NV_PFIFO_DEVICE_58                          = 0x000008e8,
        NV_PFIFO_DEVICE_59                          = 0x000008ec,
        NV_PFIFO_DEVICE_60                          = 0x000008f0,
        NV_PFIFO_DEVICE_61                          = 0x000008f4,
        NV_PFIFO_DEVICE_62                          = 0x000008f8,
        NV_PFIFO_DEVICE_63                          = 0x000008fc,
        NV_PFIFO_DEVICE_64                          = 0x00000900,
        NV_PFIFO_DEVICE_65                          = 0x00000904,
        NV_PFIFO_DEVICE_66                          = 0x00000908,
        NV_PFIFO_DEVICE_67                          = 0x0000090c,
        NV_PFIFO_DEVICE_68                          = 0x00000910,
        NV_PFIFO_DEVICE_69                          = 0x00000914,
        NV_PFIFO_DEVICE_70                          = 0x00000918,
        NV_PFIFO_DEVICE_71                          = 0x0000091c,
        NV_PFIFO_DEVICE_72                          = 0x00000920,
        NV_PFIFO_DEVICE_73                          = 0x00000924,
        NV_PFIFO_DEVICE_74                          = 0x00000928,
        NV_PFIFO_DEVICE_75                          = 0x0000092c,
        NV_PFIFO_DEVICE_76                          = 0x00000930,
        NV_PFIFO_DEVICE_77                          = 0x00000934,
        NV_PFIFO_DEVICE_78                          = 0x00000938,
        NV_PFIFO_DEVICE_79                          = 0x0000093c,
        NV_PFIFO_DEVICE_80                          = 0x00000940,
        NV_PFIFO_DEVICE_81                          = 0x00000944,
        NV_PFIFO_DEVICE_82                          = 0x00000948,
        NV_PFIFO_DEVICE_83                          = 0x0000094c,
        NV_PFIFO_DEVICE_84                          = 0x00000950,
        NV_PFIFO_DEVICE_85                          = 0x00000954,
        NV_PFIFO_DEVICE_86                          = 0x00000958,
        NV_PFIFO_DEVICE_87                          = 0x0000095c,
        NV_PFIFO_DEVICE_88                          = 0x00000960,
        NV_PFIFO_DEVICE_89                          = 0x00000964,
        NV_PFIFO_DEVICE_90                          = 0x00000968,
        NV_PFIFO_DEVICE_91                          = 0x0000096c,
        NV_PFIFO_DEVICE_92                          = 0x00000970,
        NV_PFIFO_DEVICE_93                          = 0x00000974,
        NV_PFIFO_DEVICE_94                          = 0x00000978,
        NV_PFIFO_DEVICE_95                          = 0x0000097c,
        NV_PFIFO_DEVICE_96                          = 0x00000980,
        NV_PFIFO_DEVICE_97                          = 0x00000984,
        NV_PFIFO_DEVICE_98                          = 0x00000988,
        NV_PFIFO_DEVICE_99                          = 0x0000098c,
        NV_PFIFO_DEVICE_100                         = 0x00000990,
        NV_PFIFO_DEVICE_101                         = 0x00000994,
        NV_PFIFO_DEVICE_102                         = 0x00000998,
        NV_PFIFO_DEVICE_103                         = 0x0000099c,
        NV_PFIFO_DEVICE_104                         = 0x000009a0,
        NV_PFIFO_DEVICE_105                         = 0x000009a4,
        NV_PFIFO_DEVICE_106                         = 0x000009a8,
        NV_PFIFO_DEVICE_107                         = 0x000009ac,
        NV_PFIFO_DEVICE_108                         = 0x000009b0,
        NV_PFIFO_DEVICE_109                         = 0x000009b4,
        NV_PFIFO_DEVICE_110                         = 0x000009b8,
        NV_PFIFO_DEVICE_111                         = 0x000009bc,
        NV_PFIFO_DEVICE_112                         = 0x000009c0,
        NV_PFIFO_DEVICE_113                         = 0x000009c4,
        NV_PFIFO_DEVICE_114                         = 0x000009c8,
        NV_PFIFO_DEVICE_115                         = 0x000009cc,
        NV_PFIFO_DEVICE_116                         = 0x000009d0,
        NV_PFIFO_DEVICE_117                         = 0x000009d4,
        NV_PFIFO_DEVICE_118                         = 0x000009d8,
        NV_PFIFO_DEVICE_119                         = 0x000009dc,
        NV_PFIFO_DEVICE_120                         = 0x000009e0,
        NV_PFIFO_DEVICE_121                         = 0x000009e4,
        NV_PFIFO_DEVICE_122                         = 0x000009e8,
        NV_PFIFO_DEVICE_123                         = 0x000009ec,
        NV_PFIFO_DEVICE_124                         = 0x000009f0,
        NV_PFIFO_DEVICE_125                         = 0x000009f4,
        NV_PFIFO_DEVICE_126                         = 0x000009f8,
        NV_PFIFO_DEVICE_127                         = 0x000009fc,
#define NV_PFIFO_DEVICE_CHID                                                    0x0000001f
#define NV_PFIFO_DEVICE_CHID__BITSHIFT                                          0
#define NV_PFIFO_DEVICE_SWITCH                                                  0x01000000
#define NV_PFIFO_DEVICE_SWITCH__BITSHIFT                                        24
#define NV_PFIFO_DEVICE_SWITCH_UNAVAILABLE                                      0
#define NV_PFIFO_DEVICE_SWITCH_AVAILABLE                                        1
        NV_PFIFO_RUNOUT_STATUS                      = 0x00000400,
#define NV_PFIFO_RUNOUT_STATUS_RANOUT                                           0x00000001
#define NV_PFIFO_RUNOUT_STATUS_RANOUT__BITSHIFT                                 0
#define NV_PFIFO_RUNOUT_STATUS_RANOUT_FALSE                                     0
#define NV_PFIFO_RUNOUT_STATUS_RANOUT_TRUE                                      1
#define NV_PFIFO_RUNOUT_STATUS_LOW_MARK                                         0x00000010
#define NV_PFIFO_RUNOUT_STATUS_LOW_MARK__BITSHIFT                               4
#define NV_PFIFO_RUNOUT_STATUS_LOW_MARK_NOT_EMPTY                               0
#define NV_PFIFO_RUNOUT_STATUS_LOW_MARK_EMPTY                                   1
#define NV_PFIFO_RUNOUT_STATUS_HIGH_MARK                                        0x00000100
#define NV_PFIFO_RUNOUT_STATUS_HIGH_MARK__BITSHIFT                              8
#define NV_PFIFO_RUNOUT_STATUS_HIGH_MARK_NOT_FULL                               0
#define NV_PFIFO_RUNOUT_STATUS_HIGH_MARK_FULL                                   1
        NV_PFIFO_RUNOUT_PUT                         = 0x00000410,
#define NV_PFIFO_RUNOUT_PUT_ADDRESS                                             0x00001ff8
#define NV_PFIFO_RUNOUT_PUT_ADDRESS__BITSHIFT                                   3
#define NV_PFIFO_RUNOUT_PUT_ADDRESS__SIZE_0                                     0x000001f8
#define NV_PFIFO_RUNOUT_PUT_ADDRESS__SIZE_0__BITSHIFT                           3
#define NV_PFIFO_RUNOUT_PUT_ADDRESS__SIZE_1                                     0x00001ff8
#define NV_PFIFO_RUNOUT_PUT_ADDRESS__SIZE_1__BITSHIFT                           3
        NV_PFIFO_RUNOUT_GET                         = 0x00000420,
#define NV_PFIFO_RUNOUT_GET_ADDRESS                                             0x00003ff8
#define NV_PFIFO_RUNOUT_GET_ADDRESS__BITSHIFT                                   3
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

static const char *const nv2a_pfifo_name[] = {
    NAMER(NV_PFIFO_DELAY_0),
    NAMER(NV_PFIFO_DELAY_1),
    NAMER(NV_PFIFO_DMA_TIMESLICE),
    NAMER(NV_PFIFO_PIO_TIMESLICE),
    NAMER(NV_PFIFO_TIMESLICE),
    NAMER(NV_PFIFO_NEXT_CHANNEL),
    NAMER(NV_PFIFO_DEBUG_0),
    NAMER(NV_PFIFO_INTR_0),
    NAMER(NV_PFIFO_INTR_EN_0),
    NAMER(NV_PFIFO_RAMHT),
    NAMER(NV_PFIFO_RAMFC),
    NAMER(NV_PFIFO_RAMRO),
    NAMER(NV_PFIFO_CACHES),
    NAMER(NV_PFIFO_MODE),
    NAMER(NV_PFIFO_DMA),
    NAMER(NV_PFIFO_SIZE),
    NAMER(NV_PFIFO_CACHE0_PUSH0),
    NAMER(NV_PFIFO_CACHE1_PUSH0),
    NAMER(NV_PFIFO_CACHE0_PUSH1),
    NAMER(NV_PFIFO_CACHE1_PUSH1),
    NAMER(NV_PFIFO_CACHE1_DMA_PUSH),
    NAMER(NV_PFIFO_CACHE1_DMA_FETCH),
    NAMER(NV_PFIFO_CACHE1_DMA_PUT),
    NAMER(NV_PFIFO_CACHE1_DMA_GET),
    NAMER(NV_PFIFO_CACHE1_REF),
    NAMER(NV_PFIFO_CACHE1_DMA_SUBROUTINE),
    NAMER(NV_PFIFO_CACHE1_DMA_DCOUNT),
    NAMER(NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW),
    NAMER(NV_PFIFO_CACHE1_DMA_RSVD_SHADOW),
    NAMER(NV_PFIFO_CACHE1_DMA_DATA_SHADOW),
    NAMER(NV_PFIFO_CACHE1_SUBCH_01_INST),
    NAMER(NV_PFIFO_CACHE1_SUBCH_23_INST),
    NAMER(NV_PFIFO_CACHE1_SUBCH_45_INST),
    NAMER(NV_PFIFO_CACHE1_SUBCH_67_INST),
    NAMER(NV_PFIFO_CACHE0_SUBCH_INST),
    NAMER(NV_PFIFO_CACHE1_DMA_STATE),
    NAMER(NV_PFIFO_CACHE1_DMA_INSTANCE),
    NAMER(NV_PFIFO_CACHE1_DMA_CTL),
    NAMER(NV_PFIFO_CACHE1_DMA_LIMIT),
    NAMER(NV_PFIFO_CACHE1_DMA_TLB_TAG),
    NAMER(NV_PFIFO_CACHE1_DMA_TLB_PTE),
    NAMER(NV_PFIFO_CACHE0_PULL0),
    NAMER(NV_PFIFO_CACHE1_PULL0),
    NAMER(NV_PFIFO_CACHE0_PULL1),
    NAMER(NV_PFIFO_CACHE1_PULL1),
    NAMER(NV_PFIFO_CACHE0_HASH),
    NAMER(NV_PFIFO_CACHE1_HASH),
    NAMER(NV_PFIFO_CACHE1_ACQUIRE_0),
    NAMER(NV_PFIFO_CACHE1_ACQUIRE_1),
    NAMER(NV_PFIFO_CACHE1_ACQUIRE_2),
    NAMER(NV_PFIFO_CACHE1_SEMAPHORE),
    NAMER(NV_PFIFO_CACHE0_STATUS),
    NAMER(NV_PFIFO_CACHE1_STATUS),
    NAMER(NV_PFIFO_CACHE1_STATUS1),
    NAMER(NV_PFIFO_CACHE0_PUT),
    NAMER(NV_PFIFO_CACHE1_PUT),
    NAMER(NV_PFIFO_CACHE0_GET),
    NAMER(NV_PFIFO_CACHE1_GET),
    NAMER(NV_PFIFO_CACHE0_ENGINE),
    NAMER(NV_PFIFO_CACHE1_ENGINE),
    NAMER(NV_PFIFO_CACHE0_METHOD_0),
    NAMER(NV_PFIFO_CACHE1_METHOD_0),
    NAMER(NV_PFIFO_CACHE1_METHOD_1),
    NAMER(NV_PFIFO_CACHE1_METHOD_2),
    NAMER(NV_PFIFO_CACHE1_METHOD_3),
    NAMER(NV_PFIFO_CACHE1_METHOD_4),
    NAMER(NV_PFIFO_CACHE1_METHOD_5),
    NAMER(NV_PFIFO_CACHE1_METHOD_6),
    NAMER(NV_PFIFO_CACHE1_METHOD_7),
    NAMER(NV_PFIFO_CACHE1_METHOD_8),
    NAMER(NV_PFIFO_CACHE1_METHOD_9),
    NAMER(NV_PFIFO_CACHE1_METHOD_10),
    NAMER(NV_PFIFO_CACHE1_METHOD_11),
    NAMER(NV_PFIFO_CACHE1_METHOD_12),
    NAMER(NV_PFIFO_CACHE1_METHOD_13),
    NAMER(NV_PFIFO_CACHE1_METHOD_14),
    NAMER(NV_PFIFO_CACHE1_METHOD_15),
    NAMER(NV_PFIFO_CACHE1_METHOD_16),
    NAMER(NV_PFIFO_CACHE1_METHOD_17),
    NAMER(NV_PFIFO_CACHE1_METHOD_18),
    NAMER(NV_PFIFO_CACHE1_METHOD_19),
    NAMER(NV_PFIFO_CACHE1_METHOD_20),
    NAMER(NV_PFIFO_CACHE1_METHOD_21),
    NAMER(NV_PFIFO_CACHE1_METHOD_22),
    NAMER(NV_PFIFO_CACHE1_METHOD_23),
    NAMER(NV_PFIFO_CACHE1_METHOD_24),
    NAMER(NV_PFIFO_CACHE1_METHOD_25),
    NAMER(NV_PFIFO_CACHE1_METHOD_26),
    NAMER(NV_PFIFO_CACHE1_METHOD_27),
    NAMER(NV_PFIFO_CACHE1_METHOD_28),
    NAMER(NV_PFIFO_CACHE1_METHOD_29),
    NAMER(NV_PFIFO_CACHE1_METHOD_30),
    NAMER(NV_PFIFO_CACHE1_METHOD_31),
    NAMER(NV_PFIFO_CACHE1_METHOD_32),
    NAMER(NV_PFIFO_CACHE1_METHOD_33),
    NAMER(NV_PFIFO_CACHE1_METHOD_34),
    NAMER(NV_PFIFO_CACHE1_METHOD_35),
    NAMER(NV_PFIFO_CACHE1_METHOD_36),
    NAMER(NV_PFIFO_CACHE1_METHOD_37),
    NAMER(NV_PFIFO_CACHE1_METHOD_38),
    NAMER(NV_PFIFO_CACHE1_METHOD_39),
    NAMER(NV_PFIFO_CACHE1_METHOD_40),
    NAMER(NV_PFIFO_CACHE1_METHOD_41),
    NAMER(NV_PFIFO_CACHE1_METHOD_42),
    NAMER(NV_PFIFO_CACHE1_METHOD_43),
    NAMER(NV_PFIFO_CACHE1_METHOD_44),
    NAMER(NV_PFIFO_CACHE1_METHOD_45),
    NAMER(NV_PFIFO_CACHE1_METHOD_46),
    NAMER(NV_PFIFO_CACHE1_METHOD_47),
    NAMER(NV_PFIFO_CACHE1_METHOD_48),
    NAMER(NV_PFIFO_CACHE1_METHOD_49),
    NAMER(NV_PFIFO_CACHE1_METHOD_50),
    NAMER(NV_PFIFO_CACHE1_METHOD_51),
    NAMER(NV_PFIFO_CACHE1_METHOD_52),
    NAMER(NV_PFIFO_CACHE1_METHOD_53),
    NAMER(NV_PFIFO_CACHE1_METHOD_54),
    NAMER(NV_PFIFO_CACHE1_METHOD_55),
    NAMER(NV_PFIFO_CACHE1_METHOD_56),
    NAMER(NV_PFIFO_CACHE1_METHOD_57),
    NAMER(NV_PFIFO_CACHE1_METHOD_58),
    NAMER(NV_PFIFO_CACHE1_METHOD_59),
    NAMER(NV_PFIFO_CACHE1_METHOD_60),
    NAMER(NV_PFIFO_CACHE1_METHOD_61),
    NAMER(NV_PFIFO_CACHE1_METHOD_62),
    NAMER(NV_PFIFO_CACHE1_METHOD_63),
    NAMER(NV_PFIFO_CACHE1_METHOD_64),
    NAMER(NV_PFIFO_CACHE1_METHOD_65),
    NAMER(NV_PFIFO_CACHE1_METHOD_66),
    NAMER(NV_PFIFO_CACHE1_METHOD_67),
    NAMER(NV_PFIFO_CACHE1_METHOD_68),
    NAMER(NV_PFIFO_CACHE1_METHOD_69),
    NAMER(NV_PFIFO_CACHE1_METHOD_70),
    NAMER(NV_PFIFO_CACHE1_METHOD_71),
    NAMER(NV_PFIFO_CACHE1_METHOD_72),
    NAMER(NV_PFIFO_CACHE1_METHOD_73),
    NAMER(NV_PFIFO_CACHE1_METHOD_74),
    NAMER(NV_PFIFO_CACHE1_METHOD_75),
    NAMER(NV_PFIFO_CACHE1_METHOD_76),
    NAMER(NV_PFIFO_CACHE1_METHOD_77),
    NAMER(NV_PFIFO_CACHE1_METHOD_78),
    NAMER(NV_PFIFO_CACHE1_METHOD_79),
    NAMER(NV_PFIFO_CACHE1_METHOD_80),
    NAMER(NV_PFIFO_CACHE1_METHOD_81),
    NAMER(NV_PFIFO_CACHE1_METHOD_82),
    NAMER(NV_PFIFO_CACHE1_METHOD_83),
    NAMER(NV_PFIFO_CACHE1_METHOD_84),
    NAMER(NV_PFIFO_CACHE1_METHOD_85),
    NAMER(NV_PFIFO_CACHE1_METHOD_86),
    NAMER(NV_PFIFO_CACHE1_METHOD_87),
    NAMER(NV_PFIFO_CACHE1_METHOD_88),
    NAMER(NV_PFIFO_CACHE1_METHOD_89),
    NAMER(NV_PFIFO_CACHE1_METHOD_90),
    NAMER(NV_PFIFO_CACHE1_METHOD_91),
    NAMER(NV_PFIFO_CACHE1_METHOD_92),
    NAMER(NV_PFIFO_CACHE1_METHOD_93),
    NAMER(NV_PFIFO_CACHE1_METHOD_94),
    NAMER(NV_PFIFO_CACHE1_METHOD_95),
    NAMER(NV_PFIFO_CACHE1_METHOD_96),
    NAMER(NV_PFIFO_CACHE1_METHOD_97),
    NAMER(NV_PFIFO_CACHE1_METHOD_98),
    NAMER(NV_PFIFO_CACHE1_METHOD_99),
    NAMER(NV_PFIFO_CACHE1_METHOD_100),
    NAMER(NV_PFIFO_CACHE1_METHOD_101),
    NAMER(NV_PFIFO_CACHE1_METHOD_102),
    NAMER(NV_PFIFO_CACHE1_METHOD_103),
    NAMER(NV_PFIFO_CACHE1_METHOD_104),
    NAMER(NV_PFIFO_CACHE1_METHOD_105),
    NAMER(NV_PFIFO_CACHE1_METHOD_106),
    NAMER(NV_PFIFO_CACHE1_METHOD_107),
    NAMER(NV_PFIFO_CACHE1_METHOD_108),
    NAMER(NV_PFIFO_CACHE1_METHOD_109),
    NAMER(NV_PFIFO_CACHE1_METHOD_110),
    NAMER(NV_PFIFO_CACHE1_METHOD_111),
    NAMER(NV_PFIFO_CACHE1_METHOD_112),
    NAMER(NV_PFIFO_CACHE1_METHOD_113),
    NAMER(NV_PFIFO_CACHE1_METHOD_114),
    NAMER(NV_PFIFO_CACHE1_METHOD_115),
    NAMER(NV_PFIFO_CACHE1_METHOD_116),
    NAMER(NV_PFIFO_CACHE1_METHOD_117),
    NAMER(NV_PFIFO_CACHE1_METHOD_118),
    NAMER(NV_PFIFO_CACHE1_METHOD_119),
    NAMER(NV_PFIFO_CACHE1_METHOD_120),
    NAMER(NV_PFIFO_CACHE1_METHOD_121),
    NAMER(NV_PFIFO_CACHE1_METHOD_122),
    NAMER(NV_PFIFO_CACHE1_METHOD_123),
    NAMER(NV_PFIFO_CACHE1_METHOD_124),
    NAMER(NV_PFIFO_CACHE1_METHOD_125),
    NAMER(NV_PFIFO_CACHE1_METHOD_126),
    NAMER(NV_PFIFO_CACHE1_METHOD_127),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_0),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_1),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_2),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_3),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_4),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_5),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_6),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_7),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_8),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_9),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_10),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_11),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_12),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_13),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_14),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_15),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_16),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_17),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_18),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_19),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_20),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_21),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_22),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_23),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_24),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_25),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_26),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_27),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_28),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_29),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_30),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_31),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_32),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_33),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_34),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_35),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_36),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_37),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_38),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_39),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_40),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_41),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_42),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_43),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_44),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_45),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_46),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_47),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_48),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_49),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_50),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_51),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_52),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_53),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_54),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_55),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_56),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_57),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_58),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_59),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_60),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_61),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_62),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_63),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_64),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_65),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_66),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_67),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_68),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_69),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_70),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_71),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_72),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_73),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_74),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_75),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_76),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_77),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_78),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_79),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_80),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_81),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_82),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_83),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_84),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_85),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_86),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_87),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_88),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_89),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_90),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_91),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_92),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_93),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_94),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_95),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_96),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_97),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_98),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_99),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_100),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_101),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_102),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_103),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_104),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_105),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_106),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_107),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_108),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_109),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_110),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_111),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_112),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_113),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_114),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_115),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_116),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_117),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_118),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_119),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_120),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_121),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_122),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_123),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_124),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_125),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_126),
    NAMER(NV_PFIFO_CACHE1_METHOD_ALIAS_127),
    NAMER(NV_PFIFO_CACHE0_DATA_0),
    NAMER(NV_PFIFO_CACHE1_DATA_0),
    NAMER(NV_PFIFO_CACHE1_DATA_1),
    NAMER(NV_PFIFO_CACHE1_DATA_2),
    NAMER(NV_PFIFO_CACHE1_DATA_3),
    NAMER(NV_PFIFO_CACHE1_DATA_4),
    NAMER(NV_PFIFO_CACHE1_DATA_5),
    NAMER(NV_PFIFO_CACHE1_DATA_6),
    NAMER(NV_PFIFO_CACHE1_DATA_7),
    NAMER(NV_PFIFO_CACHE1_DATA_8),
    NAMER(NV_PFIFO_CACHE1_DATA_9),
    NAMER(NV_PFIFO_CACHE1_DATA_10),
    NAMER(NV_PFIFO_CACHE1_DATA_11),
    NAMER(NV_PFIFO_CACHE1_DATA_12),
    NAMER(NV_PFIFO_CACHE1_DATA_13),
    NAMER(NV_PFIFO_CACHE1_DATA_14),
    NAMER(NV_PFIFO_CACHE1_DATA_15),
    NAMER(NV_PFIFO_CACHE1_DATA_16),
    NAMER(NV_PFIFO_CACHE1_DATA_17),
    NAMER(NV_PFIFO_CACHE1_DATA_18),
    NAMER(NV_PFIFO_CACHE1_DATA_19),
    NAMER(NV_PFIFO_CACHE1_DATA_20),
    NAMER(NV_PFIFO_CACHE1_DATA_21),
    NAMER(NV_PFIFO_CACHE1_DATA_22),
    NAMER(NV_PFIFO_CACHE1_DATA_23),
    NAMER(NV_PFIFO_CACHE1_DATA_24),
    NAMER(NV_PFIFO_CACHE1_DATA_25),
    NAMER(NV_PFIFO_CACHE1_DATA_26),
    NAMER(NV_PFIFO_CACHE1_DATA_27),
    NAMER(NV_PFIFO_CACHE1_DATA_28),
    NAMER(NV_PFIFO_CACHE1_DATA_29),
    NAMER(NV_PFIFO_CACHE1_DATA_30),
    NAMER(NV_PFIFO_CACHE1_DATA_31),
    NAMER(NV_PFIFO_CACHE1_DATA_32),
    NAMER(NV_PFIFO_CACHE1_DATA_33),
    NAMER(NV_PFIFO_CACHE1_DATA_34),
    NAMER(NV_PFIFO_CACHE1_DATA_35),
    NAMER(NV_PFIFO_CACHE1_DATA_36),
    NAMER(NV_PFIFO_CACHE1_DATA_37),
    NAMER(NV_PFIFO_CACHE1_DATA_38),
    NAMER(NV_PFIFO_CACHE1_DATA_39),
    NAMER(NV_PFIFO_CACHE1_DATA_40),
    NAMER(NV_PFIFO_CACHE1_DATA_41),
    NAMER(NV_PFIFO_CACHE1_DATA_42),
    NAMER(NV_PFIFO_CACHE1_DATA_43),
    NAMER(NV_PFIFO_CACHE1_DATA_44),
    NAMER(NV_PFIFO_CACHE1_DATA_45),
    NAMER(NV_PFIFO_CACHE1_DATA_46),
    NAMER(NV_PFIFO_CACHE1_DATA_47),
    NAMER(NV_PFIFO_CACHE1_DATA_48),
    NAMER(NV_PFIFO_CACHE1_DATA_49),
    NAMER(NV_PFIFO_CACHE1_DATA_50),
    NAMER(NV_PFIFO_CACHE1_DATA_51),
    NAMER(NV_PFIFO_CACHE1_DATA_52),
    NAMER(NV_PFIFO_CACHE1_DATA_53),
    NAMER(NV_PFIFO_CACHE1_DATA_54),
    NAMER(NV_PFIFO_CACHE1_DATA_55),
    NAMER(NV_PFIFO_CACHE1_DATA_56),
    NAMER(NV_PFIFO_CACHE1_DATA_57),
    NAMER(NV_PFIFO_CACHE1_DATA_58),
    NAMER(NV_PFIFO_CACHE1_DATA_59),
    NAMER(NV_PFIFO_CACHE1_DATA_60),
    NAMER(NV_PFIFO_CACHE1_DATA_61),
    NAMER(NV_PFIFO_CACHE1_DATA_62),
    NAMER(NV_PFIFO_CACHE1_DATA_63),
    NAMER(NV_PFIFO_CACHE1_DATA_64),
    NAMER(NV_PFIFO_CACHE1_DATA_65),
    NAMER(NV_PFIFO_CACHE1_DATA_66),
    NAMER(NV_PFIFO_CACHE1_DATA_67),
    NAMER(NV_PFIFO_CACHE1_DATA_68),
    NAMER(NV_PFIFO_CACHE1_DATA_69),
    NAMER(NV_PFIFO_CACHE1_DATA_70),
    NAMER(NV_PFIFO_CACHE1_DATA_71),
    NAMER(NV_PFIFO_CACHE1_DATA_72),
    NAMER(NV_PFIFO_CACHE1_DATA_73),
    NAMER(NV_PFIFO_CACHE1_DATA_74),
    NAMER(NV_PFIFO_CACHE1_DATA_75),
    NAMER(NV_PFIFO_CACHE1_DATA_76),
    NAMER(NV_PFIFO_CACHE1_DATA_77),
    NAMER(NV_PFIFO_CACHE1_DATA_78),
    NAMER(NV_PFIFO_CACHE1_DATA_79),
    NAMER(NV_PFIFO_CACHE1_DATA_80),
    NAMER(NV_PFIFO_CACHE1_DATA_81),
    NAMER(NV_PFIFO_CACHE1_DATA_82),
    NAMER(NV_PFIFO_CACHE1_DATA_83),
    NAMER(NV_PFIFO_CACHE1_DATA_84),
    NAMER(NV_PFIFO_CACHE1_DATA_85),
    NAMER(NV_PFIFO_CACHE1_DATA_86),
    NAMER(NV_PFIFO_CACHE1_DATA_87),
    NAMER(NV_PFIFO_CACHE1_DATA_88),
    NAMER(NV_PFIFO_CACHE1_DATA_89),
    NAMER(NV_PFIFO_CACHE1_DATA_90),
    NAMER(NV_PFIFO_CACHE1_DATA_91),
    NAMER(NV_PFIFO_CACHE1_DATA_92),
    NAMER(NV_PFIFO_CACHE1_DATA_93),
    NAMER(NV_PFIFO_CACHE1_DATA_94),
    NAMER(NV_PFIFO_CACHE1_DATA_95),
    NAMER(NV_PFIFO_CACHE1_DATA_96),
    NAMER(NV_PFIFO_CACHE1_DATA_97),
    NAMER(NV_PFIFO_CACHE1_DATA_98),
    NAMER(NV_PFIFO_CACHE1_DATA_99),
    NAMER(NV_PFIFO_CACHE1_DATA_100),
    NAMER(NV_PFIFO_CACHE1_DATA_101),
    NAMER(NV_PFIFO_CACHE1_DATA_102),
    NAMER(NV_PFIFO_CACHE1_DATA_103),
    NAMER(NV_PFIFO_CACHE1_DATA_104),
    NAMER(NV_PFIFO_CACHE1_DATA_105),
    NAMER(NV_PFIFO_CACHE1_DATA_106),
    NAMER(NV_PFIFO_CACHE1_DATA_107),
    NAMER(NV_PFIFO_CACHE1_DATA_108),
    NAMER(NV_PFIFO_CACHE1_DATA_109),
    NAMER(NV_PFIFO_CACHE1_DATA_110),
    NAMER(NV_PFIFO_CACHE1_DATA_111),
    NAMER(NV_PFIFO_CACHE1_DATA_112),
    NAMER(NV_PFIFO_CACHE1_DATA_113),
    NAMER(NV_PFIFO_CACHE1_DATA_114),
    NAMER(NV_PFIFO_CACHE1_DATA_115),
    NAMER(NV_PFIFO_CACHE1_DATA_116),
    NAMER(NV_PFIFO_CACHE1_DATA_117),
    NAMER(NV_PFIFO_CACHE1_DATA_118),
    NAMER(NV_PFIFO_CACHE1_DATA_119),
    NAMER(NV_PFIFO_CACHE1_DATA_120),
    NAMER(NV_PFIFO_CACHE1_DATA_121),
    NAMER(NV_PFIFO_CACHE1_DATA_122),
    NAMER(NV_PFIFO_CACHE1_DATA_123),
    NAMER(NV_PFIFO_CACHE1_DATA_124),
    NAMER(NV_PFIFO_CACHE1_DATA_125),
    NAMER(NV_PFIFO_CACHE1_DATA_126),
    NAMER(NV_PFIFO_CACHE1_DATA_127),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_0),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_1),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_2),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_3),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_4),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_5),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_6),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_7),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_8),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_9),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_10),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_11),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_12),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_13),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_14),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_15),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_16),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_17),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_18),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_19),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_20),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_21),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_22),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_23),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_24),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_25),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_26),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_27),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_28),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_29),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_30),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_31),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_32),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_33),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_34),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_35),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_36),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_37),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_38),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_39),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_40),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_41),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_42),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_43),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_44),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_45),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_46),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_47),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_48),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_49),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_50),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_51),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_52),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_53),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_54),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_55),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_56),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_57),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_58),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_59),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_60),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_61),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_62),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_63),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_64),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_65),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_66),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_67),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_68),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_69),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_70),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_71),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_72),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_73),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_74),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_75),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_76),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_77),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_78),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_79),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_80),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_81),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_82),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_83),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_84),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_85),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_86),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_87),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_88),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_89),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_90),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_91),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_92),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_93),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_94),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_95),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_96),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_97),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_98),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_99),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_100),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_101),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_102),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_103),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_104),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_105),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_106),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_107),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_108),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_109),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_110),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_111),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_112),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_113),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_114),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_115),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_116),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_117),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_118),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_119),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_120),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_121),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_122),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_123),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_124),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_125),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_126),
    NAMER(NV_PFIFO_CACHE1_DATA_ALIAS_127),
    NAMER(NV_PFIFO_DEVICE_0),
    NAMER(NV_PFIFO_DEVICE_1),
    NAMER(NV_PFIFO_DEVICE_2),
    NAMER(NV_PFIFO_DEVICE_3),
    NAMER(NV_PFIFO_DEVICE_4),
    NAMER(NV_PFIFO_DEVICE_5),
    NAMER(NV_PFIFO_DEVICE_6),
    NAMER(NV_PFIFO_DEVICE_7),
    NAMER(NV_PFIFO_DEVICE_8),
    NAMER(NV_PFIFO_DEVICE_9),
    NAMER(NV_PFIFO_DEVICE_10),
    NAMER(NV_PFIFO_DEVICE_11),
    NAMER(NV_PFIFO_DEVICE_12),
    NAMER(NV_PFIFO_DEVICE_13),
    NAMER(NV_PFIFO_DEVICE_14),
    NAMER(NV_PFIFO_DEVICE_15),
    NAMER(NV_PFIFO_DEVICE_16),
    NAMER(NV_PFIFO_DEVICE_17),
    NAMER(NV_PFIFO_DEVICE_18),
    NAMER(NV_PFIFO_DEVICE_19),
    NAMER(NV_PFIFO_DEVICE_20),
    NAMER(NV_PFIFO_DEVICE_21),
    NAMER(NV_PFIFO_DEVICE_22),
    NAMER(NV_PFIFO_DEVICE_23),
    NAMER(NV_PFIFO_DEVICE_24),
    NAMER(NV_PFIFO_DEVICE_25),
    NAMER(NV_PFIFO_DEVICE_26),
    NAMER(NV_PFIFO_DEVICE_27),
    NAMER(NV_PFIFO_DEVICE_28),
    NAMER(NV_PFIFO_DEVICE_29),
    NAMER(NV_PFIFO_DEVICE_30),
    NAMER(NV_PFIFO_DEVICE_31),
    NAMER(NV_PFIFO_DEVICE_32),
    NAMER(NV_PFIFO_DEVICE_33),
    NAMER(NV_PFIFO_DEVICE_34),
    NAMER(NV_PFIFO_DEVICE_35),
    NAMER(NV_PFIFO_DEVICE_36),
    NAMER(NV_PFIFO_DEVICE_37),
    NAMER(NV_PFIFO_DEVICE_38),
    NAMER(NV_PFIFO_DEVICE_39),
    NAMER(NV_PFIFO_DEVICE_40),
    NAMER(NV_PFIFO_DEVICE_41),
    NAMER(NV_PFIFO_DEVICE_42),
    NAMER(NV_PFIFO_DEVICE_43),
    NAMER(NV_PFIFO_DEVICE_44),
    NAMER(NV_PFIFO_DEVICE_45),
    NAMER(NV_PFIFO_DEVICE_46),
    NAMER(NV_PFIFO_DEVICE_47),
    NAMER(NV_PFIFO_DEVICE_48),
    NAMER(NV_PFIFO_DEVICE_49),
    NAMER(NV_PFIFO_DEVICE_50),
    NAMER(NV_PFIFO_DEVICE_51),
    NAMER(NV_PFIFO_DEVICE_52),
    NAMER(NV_PFIFO_DEVICE_53),
    NAMER(NV_PFIFO_DEVICE_54),
    NAMER(NV_PFIFO_DEVICE_55),
    NAMER(NV_PFIFO_DEVICE_56),
    NAMER(NV_PFIFO_DEVICE_57),
    NAMER(NV_PFIFO_DEVICE_58),
    NAMER(NV_PFIFO_DEVICE_59),
    NAMER(NV_PFIFO_DEVICE_60),
    NAMER(NV_PFIFO_DEVICE_61),
    NAMER(NV_PFIFO_DEVICE_62),
    NAMER(NV_PFIFO_DEVICE_63),
    NAMER(NV_PFIFO_DEVICE_64),
    NAMER(NV_PFIFO_DEVICE_65),
    NAMER(NV_PFIFO_DEVICE_66),
    NAMER(NV_PFIFO_DEVICE_67),
    NAMER(NV_PFIFO_DEVICE_68),
    NAMER(NV_PFIFO_DEVICE_69),
    NAMER(NV_PFIFO_DEVICE_70),
    NAMER(NV_PFIFO_DEVICE_71),
    NAMER(NV_PFIFO_DEVICE_72),
    NAMER(NV_PFIFO_DEVICE_73),
    NAMER(NV_PFIFO_DEVICE_74),
    NAMER(NV_PFIFO_DEVICE_75),
    NAMER(NV_PFIFO_DEVICE_76),
    NAMER(NV_PFIFO_DEVICE_77),
    NAMER(NV_PFIFO_DEVICE_78),
    NAMER(NV_PFIFO_DEVICE_79),
    NAMER(NV_PFIFO_DEVICE_80),
    NAMER(NV_PFIFO_DEVICE_81),
    NAMER(NV_PFIFO_DEVICE_82),
    NAMER(NV_PFIFO_DEVICE_83),
    NAMER(NV_PFIFO_DEVICE_84),
    NAMER(NV_PFIFO_DEVICE_85),
    NAMER(NV_PFIFO_DEVICE_86),
    NAMER(NV_PFIFO_DEVICE_87),
    NAMER(NV_PFIFO_DEVICE_88),
    NAMER(NV_PFIFO_DEVICE_89),
    NAMER(NV_PFIFO_DEVICE_90),
    NAMER(NV_PFIFO_DEVICE_91),
    NAMER(NV_PFIFO_DEVICE_92),
    NAMER(NV_PFIFO_DEVICE_93),
    NAMER(NV_PFIFO_DEVICE_94),
    NAMER(NV_PFIFO_DEVICE_95),
    NAMER(NV_PFIFO_DEVICE_96),
    NAMER(NV_PFIFO_DEVICE_97),
    NAMER(NV_PFIFO_DEVICE_98),
    NAMER(NV_PFIFO_DEVICE_99),
    NAMER(NV_PFIFO_DEVICE_100),
    NAMER(NV_PFIFO_DEVICE_101),
    NAMER(NV_PFIFO_DEVICE_102),
    NAMER(NV_PFIFO_DEVICE_103),
    NAMER(NV_PFIFO_DEVICE_104),
    NAMER(NV_PFIFO_DEVICE_105),
    NAMER(NV_PFIFO_DEVICE_106),
    NAMER(NV_PFIFO_DEVICE_107),
    NAMER(NV_PFIFO_DEVICE_108),
    NAMER(NV_PFIFO_DEVICE_109),
    NAMER(NV_PFIFO_DEVICE_110),
    NAMER(NV_PFIFO_DEVICE_111),
    NAMER(NV_PFIFO_DEVICE_112),
    NAMER(NV_PFIFO_DEVICE_113),
    NAMER(NV_PFIFO_DEVICE_114),
    NAMER(NV_PFIFO_DEVICE_115),
    NAMER(NV_PFIFO_DEVICE_116),
    NAMER(NV_PFIFO_DEVICE_117),
    NAMER(NV_PFIFO_DEVICE_118),
    NAMER(NV_PFIFO_DEVICE_119),
    NAMER(NV_PFIFO_DEVICE_120),
    NAMER(NV_PFIFO_DEVICE_121),
    NAMER(NV_PFIFO_DEVICE_122),
    NAMER(NV_PFIFO_DEVICE_123),
    NAMER(NV_PFIFO_DEVICE_124),
    NAMER(NV_PFIFO_DEVICE_125),
    NAMER(NV_PFIFO_DEVICE_126),
    NAMER(NV_PFIFO_DEVICE_127),
    NAMER(NV_PFIFO_RUNOUT_STATUS),
    NAMER(NV_PFIFO_RUNOUT_PUT),
    NAMER(NV_PFIFO_RUNOUT_GET),
};

static const char *const nv2a_pfifo_cache1_dma_state_error_name[] = {
    NAME(NV_PFIFO_CACHE1_DMA_STATE_ERROR_NONE),
    NAME(NV_PFIFO_CACHE1_DMA_STATE_ERROR_CALL),
    NAME(NV_PFIFO_CACHE1_DMA_STATE_ERROR_NON_CACHE),
    NAME(NV_PFIFO_CACHE1_DMA_STATE_ERROR_RETURN),
    NAME(NV_PFIFO_CACHE1_DMA_STATE_ERROR_RESERVED_CMD),
    NAME(NV_PFIFO_CACHE1_DMA_STATE_ERROR_PROTECTION),
};

