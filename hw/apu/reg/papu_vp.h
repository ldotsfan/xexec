
enum {
        NV1BA0_PIO_FREE                                 = 0x00000010,
#define NV1BA0_PIO_FREE_COUNT                                                   0x00000ffc
#define NV1BA0_PIO_FREE_COUNT__BITSHIFT                                         2
#define NV1BA0_PIO_FREE_COUNT_FIFO_FULL                                         0
        NV1BA0_PIO_INFO                                 = 0x00000014,
#define NV1BA0_PIO_INFO_PIO                                                     0x00000001
#define NV1BA0_PIO_INFO_PIO__BITSHIFT                                           0
#define NV1BA0_PIO_INFO_PIO_EMPTY_AND_IDLE                                      0
#define NV1BA0_PIO_INFO_PIO_BUSY                                                1
#define NV1BA0_PIO_INFO_DMA                                                     0x00000002
#define NV1BA0_PIO_INFO_DMA__BITSHIFT                                           1
#define NV1BA0_PIO_INFO_DMA_EMPTY_AND_IDLE                                      0
#define NV1BA0_PIO_INFO_DMA_BUSY                                                1
        NV1BA0_CHANNEL_DMA_PUT                          = 0x00000040,
#define NV1BA0_CHANNEL_DMA_PUT_OFFSET                                           0x00000ffc
#define NV1BA0_CHANNEL_DMA_PUT_OFFSET__BITSHIFT                                 2
        NV1BA0_CHANNEL_DMA_GET                          = 0x00000044,
#define NV1BA0_CHANNEL_DMA_GET_OFFSET                                           0x00000ffc
#define NV1BA0_CHANNEL_DMA_GET_OFFSET__BITSHIFT                                 2
        NV1BA0_NOP                                      = 0x00000100,
#define NV1BA0_NOP_NOTIFICATION_STATUS_DONE_SUCCESS                             1
        NV1BA0_PIO_SYNCHRONIZE                          = 0x00000104,
#define NV1BA0_PIO_SYNCHRONIZE_PARAMETER_NO_OPERATION                           0
#define NV1BA0_PIO_SYNCHRONIZE_PARAMETER_WAIT_FOR_IDLE                          1
#define NV1BA0_PIO_SYNCHRONIZE_PARAMETER_WAIT_FOR_IDLE_WRITE_PE_NOTIFY          2
#define NV1BA0_PIO_SYNCHRONIZE_PARAMETER_WAIT_FOR_IDLE_WRITE_PE_NOTIFY_AWAKEN   3
        NV1BA0_PIO_SET_TIME                             = 0x00000108,
        NV1BA0_PIO_SET_MODE                             = 0x0000010c,
#define NV1BA0_PIO_SET_MODE_PARAMETER_OFF                                       0
#define NV1BA0_PIO_SET_MODE_PARAMETER_RUN                                       1
        NV1BA0_PIO_SET_ANTECEDENT_VOICE                 = 0x00000120,
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE_HANDLE                                  0x0000ffff
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE_HANDLE__BITSHIFT                        0
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE_HANDLE_NULL                             0x0000ffff
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST                                    0x00030000
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST__BITSHIFT                          16
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST_INHERIT                            0
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST_2D_TOP                             1
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST_3D_TOP                             2
#define NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST_MP_TOP                             3
        NV1BA0_PIO_VOICE_ON                             = 0x00000124,
#define NV1BA0_PIO_VOICE_ON_HANDLE                                              0x0000ffff
#define NV1BA0_PIO_VOICE_ON_HANDLE__BITSHIFT                                    0
#define NV1BA0_PIO_VOICE_ON_ENVF                                                0x0f000000
#define NV1BA0_PIO_VOICE_ON_ENVF__BITSHIFT                                      24
#define NV1BA0_PIO_VOICE_ON_ENVF_DISABLE                                        0
#define NV1BA0_PIO_VOICE_ON_ENVF_DELAY                                          1
#define NV1BA0_PIO_VOICE_ON_ENVF_ATTACK                                         2
#define NV1BA0_PIO_VOICE_ON_ENVF_HOLD                                           3
#define NV1BA0_PIO_VOICE_ON_ENVA                                                0xf0000000
#define NV1BA0_PIO_VOICE_ON_ENVA__BITSHIFT                                      28
#define NV1BA0_PIO_VOICE_ON_ENVA_DISABLE                                        0
#define NV1BA0_PIO_VOICE_ON_ENVA_DELAY                                          1
#define NV1BA0_PIO_VOICE_ON_ENVA_ATTACK                                         2
#define NV1BA0_PIO_VOICE_ON_ENVA_HOLD                                           3
        NV1BA0_PIO_VOICE_OFF                            = 0x00000128,
#define NV1BA0_PIO_VOICE_OFF_HANDLE                                             0x0000ffff
#define NV1BA0_PIO_VOICE_OFF_HANDLE__BITSHIFT                                   0
        NV1BA0_PIO_VOICE_RELEASE                        = 0x0000012c,
#define NV1BA0_PIO_VOICE_RELEASE_HANDLE                                         0x0000ffff
#define NV1BA0_PIO_VOICE_RELEASE_HANDLE__BITSHIFT                               0
        NV1BA0_PIO_GET_VOICE_POSITION                   = 0x00000130,
#define NV1BA0_PIO_GET_VOICE_POSITION_HANDLE                                    0x0000ffff
#define NV1BA0_PIO_GET_VOICE_POSITION_HANDLE__BITSHIFT                          0
        NV1BA0_PIO_VOICE_PAUSE                          = 0x00000140,
#define NV1BA0_PIO_VOICE_PAUSE_HANDLE                                           0x0000ffff
#define NV1BA0_PIO_VOICE_PAUSE_HANDLE__BITSHIFT                                 0
#define NV1BA0_PIO_VOICE_PAUSE_ACTION                                           0x00040000
#define NV1BA0_PIO_VOICE_PAUSE_ACTION__BITSHIFT                                 18
#define NV1BA0_PIO_VOICE_PAUSE_ACTION_RESUME                                    0
#define NV1BA0_PIO_VOICE_PAUSE_ACTION_STOP                                      1
        NV1BA0_PIO_SET_CONTEXT_DMA_NOTIFY               = 0x00000180,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_0                = 0x00000200,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_1                = 0x00000204,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_2                = 0x00000208,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_3                = 0x0000020c,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_4                = 0x00000210,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_5                = 0x00000214,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_6                = 0x00000218,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_7                = 0x0000021c,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_8                = 0x00000220,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_9                = 0x00000224,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_10               = 0x00000228,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_11               = 0x0000022c,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_12               = 0x00000230,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_13               = 0x00000234,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_14               = 0x00000238,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_15               = 0x0000023c,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_16               = 0x00000240,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_17               = 0x00000244,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_18               = 0x00000248,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_19               = 0x0000024c,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_20               = 0x00000250,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_21               = 0x00000254,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_22               = 0x00000258,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_23               = 0x0000025c,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_24               = 0x00000260,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_25               = 0x00000264,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_26               = 0x00000268,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_27               = 0x0000026c,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_28               = 0x00000270,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_29               = 0x00000274,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_30               = 0x00000278,
        NV1BA0_PIO_SET_SUBMIX_HEADROOM_31               = 0x0000027c,
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT                                   0x00000007
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT__BITSHIFT                         0
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_ZEROBITS                          0
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_ONEBIT                            1
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_TWOBITS                           2
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_THREEBITS                         3
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_FOURBITS                          4
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_FIVEBITS                          5
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_SIXBITS                           6
#define NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_SEVENBITS                         7
        NV1BA0_PIO_SET_HRTF_HEADROOM                    = 0x00000280,
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT                                     0x00000007
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT__BITSHIFT                           0
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_ZEROBITS                            0
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_ONEBIT                              1
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_TWOBITS                             2
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_THREEBITS                           3
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_FOURBITS                            4
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_FIVEBITS                            5
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_SIXBITS                             6
#define NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_SEVENBITS                           7
        NV1BA0_PIO_SET_HRTF_SUBMIX_0                    = 0x00000290,
        NV1BA0_PIO_SET_HRTF_SUBMIX_1                    = 0x00000294,
        NV1BA0_PIO_SET_HRTF_SUBMIX_2                    = 0x00000298,
        NV1BA0_PIO_SET_HRTF_SUBMIX_3                    = 0x0000029c,
#define NV1BA0_PIO_SET_HRTF_SUBMIX_BIN                                          0x0000001f
#define NV1BA0_PIO_SET_HRTF_SUBMIX_BIN__BITSHIFT                                0
        NV1BA0_PIO_SET_HRTF_SUBMIXES                    = 0x000002c0,
#define NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN0                                       0x0000001f
#define NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN0__BITSHIFT                             0
#define NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN1                                       0x00001f00
#define NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN1__BITSHIFT                             8
#define NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN2                                       0x001f0000
#define NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN2__BITSHIFT                             16
#define NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN3                                       0x1f000000
#define NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN3__BITSHIFT                             24
        NV1BA0_PIO_SET_VOLUME_TRACKING                  = 0x000002a0,
#define NV1BA0_PIO_SET_VOLUME_TRACKING_PARAMETER                                0x00000fff
#define NV1BA0_PIO_SET_VOLUME_TRACKING_PARAMETER__BITSHIFT                      0
        NV1BA0_PIO_SET_PITCH_TRACKING                   = 0x000002a4,
#define NV1BA0_PIO_SET_PITCH_TRACKING_PARAMETER                                 0x00000fff
#define NV1BA0_PIO_SET_PITCH_TRACKING_PARAMETER__BITSHIFT                       0
        NV1BA0_PIO_SET_HRTF_TRACKING                    = 0x000002a8,
#define NV1BA0_PIO_SET_HRTF_TRACKING_PARAMETER                                  0x00000fff
#define NV1BA0_PIO_SET_HRTF_TRACKING_PARAMETER__BITSHIFT                        0
        NV1BA0_PIO_SET_ITD_TRACKING                     = 0x000002ac,
#define NV1BA0_PIO_SET_ITD_TRACKING_PARAMETER                                   0x00000fff
#define NV1BA0_PIO_SET_ITD_TRACKING_PARAMETER__BITSHIFT                         0
        NV1BA0_PIO_SET_FILTER_TRACKING                  = 0x000002b0,
#define NV1BA0_PIO_SET_FILTER_TRACKING_PARAMETER                                0x00000fff
#define NV1BA0_PIO_SET_FILTER_TRACKING_PARAMETER__BITSHIFT                      0
        NV1BA0_PIO_SET_CURRENT_VOICE                    = 0x000002f8,
#define NV1BA0_PIO_SET_CURRENT_VOICE_HANDLE                                     0x0000ffff
#define NV1BA0_PIO_SET_CURRENT_VOICE_HANDLE__BITSHIFT                           0
        NV1BA0_PIO_VOICE_LOCK                           = 0x000002fc,
#define NV1BA0_PIO_VOICE_LOCK_PARAMETER                                         0x000000ff
#define NV1BA0_PIO_VOICE_LOCK_PARAMETER__BITSHIFT                               0
#define NV1BA0_PIO_VOICE_LOCK_PARAMETER_OFF                                     0
#define NV1BA0_PIO_VOICE_LOCK_PARAMETER_ON                                      1
        NV1BA0_PIO_SET_VOICE_CFG_VBIN                   = 0x00000300,
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V0BIN                                     0x0000001f
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V0BIN__BITSHIFT                           0
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V1BIN                                     0x000003e0
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V1BIN__BITSHIFT                           5
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V2BIN                                     0x00007c00
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V2BIN__BITSHIFT                           10
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V3BIN                                     0x001f0000
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V3BIN__BITSHIFT                           16
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V4BIN                                     0x03e00000
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V4BIN__BITSHIFT                           21
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V5BIN                                     0x7c000000
#define NV1BA0_PIO_SET_VOICE_CFG_VBIN_V5BIN__BITSHIFT                           26
        NV1BA0_PIO_SET_VOICE_CFG_FMT                    = 0x00000304,
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V6BIN                                      0x0000001f
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V6BIN__BITSHIFT                            0
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V7BIN                                      0x000003e0
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V7BIN__BITSHIFT                            5
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V6PHASE                                    0x00000400
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V6PHASE__BITSHIFT                          10
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V6PHASE_POSITIVE                           0
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V6PHASE_NEGATIVE                           1
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V7PHASE                                    0x00000800
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V7PHASE__BITSHIFT                          11
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V7PHASE_POSITIVE                           0
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_V7PHASE_NEGATIVE                           1
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_GAIN                                       0x00001000
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_GAIN__BITSHIFT                             12
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_GAIN_X1                                    0
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_GAIN_X2                                    1
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM                                   0x0000e000
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM__BITSHIFT                         13
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC0_FLT0                         0
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC0_FLT1                         1
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC0_FLT2                         2
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC0_FLT3                         3
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC1_FLT0                         4
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC1_FLT1                         5
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC1_FLT2                         6
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC1_FLT3                         7
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS0                        0
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS1                        1
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS2                        2
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS3                        3
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS4                        4
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS5                        5
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS6                        6
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS7                        7
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLES_PER_BLOCK                          0x001f0000
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLES_PER_BLOCK__BITSHIFT                16
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_MULTIPASS                                  0x00200000
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_MULTIPASS__BITSHIFT                        21
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_MULTIPASS_OFF                              0
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_MULTIPASS_ON                               1
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_LINKED_VOICE                               0x00400000
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_LINKED_VOICE__BITSHIFT                     22
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_LINKED_VOICE_OFF                           0
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_LINKED_VOICE_ON                            1
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST                                    0x00800000
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST__BITSHIFT                          23
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST_OFF                                0
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST_ON                                 1
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_DATA_TYPE                                  0x01000000
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_DATA_TYPE__BITSHIFT                        24
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_DATA_TYPE_BUFFER                           0
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_DATA_TYPE_STREAM                           1
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP                                       0x02000000
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP__BITSHIFT                             25
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP_OFF                                   0
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP_ON                                    1
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CLEAR_MIX                                  0x04000000
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CLEAR_MIX__BITSHIFT                        26
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CLEAR_MIX_FALSE                            0
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CLEAR_MIX_TRUE                             1
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_STEREO                                     0x08000000
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_STEREO__BITSHIFT                           27
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_STEREO_DISABLE                             0
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_STEREO_ENABLE                              1
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE                                0x30000000
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE__BITSHIFT                      28
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE_U8                             0
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE_S16                            1
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE_S24                            2
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE_S32                            3
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE                             0xc0000000
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE__BITSHIFT                   30
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_B8                          0
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_B16                         1
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_ADPCM                       2
#define NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_B32                         3
        NV1BA0_PIO_SET_VOICE_CFG_ENV0                   = 0x00000308,
#define NV1BA0_PIO_SET_VOICE_CFG_ENV0_EA_ATTACKRATE                             0x00000fff
#define NV1BA0_PIO_SET_VOICE_CFG_ENV0_EA_ATTACKRATE__BITSHIFT                   0
#define NV1BA0_PIO_SET_VOICE_CFG_ENV0_EA_DELAYTIME                              0x00fff000
#define NV1BA0_PIO_SET_VOICE_CFG_ENV0_EA_DELAYTIME__BITSHIFT                    12
#define NV1BA0_PIO_SET_VOICE_CFG_ENV0_EF_PITCHSCALE                             0xff000000
#define NV1BA0_PIO_SET_VOICE_CFG_ENV0_EF_PITCHSCALE__BITSHIFT                   24
        NV1BA0_PIO_SET_VOICE_CFG_ENVA                   = 0x0000030c,
#define NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_DECAYRATE                              0x00000fff
#define NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_DECAYRATE__BITSHIFT                    0
#define NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_HOLDTIME                               0x00fff000
#define NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_HOLDTIME__BITSHIFT                     12
#define NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_SUSTAINLEVEL                           0xff000000
#define NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_SUSTAINLEVEL__BITSHIFT                 24
        NV1BA0_PIO_SET_VOICE_CFG_ENV1                   = 0x00000310,
#define NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_ATTACKRATE                             0x00000fff
#define NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_ATTACKRATE__BITSHIFT                   0
#define NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_DELAYTIME                              0x00fff000
#define NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_DELAYTIME__BITSHIFT                    12
#define NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_FCSCALE                                0xff000000
#define NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_FCSCALE__BITSHIFT                      24
        NV1BA0_PIO_SET_VOICE_CFG_ENVF                   = 0x00000314,
#define NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_DECAYRATE                              0x00000fff
#define NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_DECAYRATE__BITSHIFT                    0
#define NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_HOLDTIME                               0x00fff000
#define NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_HOLDTIME__BITSHIFT                     12
#define NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_SUSTAINLEVEL                           0xff000000
#define NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_SUSTAINLEVEL__BITSHIFT                 24
        NV1BA0_PIO_SET_VOICE_CFG_MISC                   = 0x00000318,
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_EF_RELEASERATE                            0x00000fff
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_EF_RELEASERATE__BITSHIFT                  0
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOA_DELAYMODE                            0x00004000
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOA_DELAYMODE__BITSHIFT                  14
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOA_DELAYMODE_NORMAL                     0
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOA_DELAYMODE_DELAY                      1
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOF_DELAYMODE                            0x00008000
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOF_DELAYMODE__BITSHIFT                  15
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOF_DELAYMODE_NORMAL                     0
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOF_DELAYMODE_DELAY                      1
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE                                     0x00030000
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE__BITSHIFT                           16
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_BYPASS                              0
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_STEREO_DLS2                         1
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_STEREO_P_EQ                         2
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_STEREO_BYPASS                       3
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_MONO_DLS2                           1
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_MONO_P_EQ                           2
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_MONO_DLS2_P_EQ                      3
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_THREED_DLS2_I3DL2                   1
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_THREED_P_EQ_I3DL2                   2
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_THREED_I3DL2                        3
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_BPQ                                       0x001c0000
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_BPQ__BITSHIFT                             18
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_NOTIFY_INTERRUPT                          0x00800000
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_NOTIFY_INTERRUPT__BITSHIFT                23
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_NOTIFY_INTERRUPT_DISABLE                  0
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_NOTIFY_INTERRUPT_ENABLE                   1
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_V_GSCNT                                   0xff000000
#define NV1BA0_PIO_SET_VOICE_CFG_MISC_V_GSCNT__BITSHIFT                         24
        NV1BA0_PIO_SET_VOICE_TAR_HRTF                   = 0x0000031c,
#define NV1BA0_PIO_SET_VOICE_TAR_HRTF_HANDLE                                    0x0000ffff
#define NV1BA0_PIO_SET_VOICE_TAR_HRTF_HANDLE__BITSHIFT                          0
        NV1BA0_PIO_SET_VOICE_PAR_LFODLY                 = 0x00000350,
#define NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOADLY                                 0x00007fff
#define NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOADLY__BITSHIFT                       0
#define NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOADR                                  0x00008000
#define NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOADR__BITSHIFT                        15
#define NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOFDLY                                 0x7fff0000
#define NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOFDLY__BITSHIFT                       16
#define NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOFDR                                  0x80000000
#define NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOFDR__BITSHIFT                        31
        NV1BA0_PIO_SET_VOICE_SSL_A                      = 0x00000320,
#define NV1BA0_PIO_SET_VOICE_SSL_A_COUNT                                        0x000000ff
#define NV1BA0_PIO_SET_VOICE_SSL_A_COUNT__BITSHIFT                              0
#define NV1BA0_PIO_SET_VOICE_SSL_A_BASE                                         0x00ffff00
#define NV1BA0_PIO_SET_VOICE_SSL_A_BASE__BITSHIFT                               8
        NV1BA0_PIO_SET_VOICE_SSL_B                      = 0x0000035c,
#define NV1BA0_PIO_SET_VOICE_SSL_B_COUNT                                        0x000000ff
#define NV1BA0_PIO_SET_VOICE_SSL_B_COUNT__BITSHIFT                              0
#define NV1BA0_PIO_SET_VOICE_SSL_B_BASE                                         0x00ffff00
#define NV1BA0_PIO_SET_VOICE_SSL_B_BASE__BITSHIFT                               8
        NV1BA0_PIO_SET_VOICE_CFG_BUF_BASE               = 0x000003a0,
#define NV1BA0_PIO_SET_VOICE_CFG_BUF_BASE_OFFSET                                0x00ffffff
#define NV1BA0_PIO_SET_VOICE_CFG_BUF_BASE_OFFSET__BITSHIFT                      0
        NV1BA0_PIO_SET_VOICE_CFG_BUF_LBO                = 0x000003a4,
#define NV1BA0_PIO_SET_VOICE_CFG_BUF_LBO_OFFSET                                 0x00ffffff
#define NV1BA0_PIO_SET_VOICE_CFG_BUF_LBO_OFFSET__BITSHIFT                       0
        NV1BA0_PIO_SET_VOICE_BUF_CBOFRAC                = 0x000003d4,
#define NV1BA0_PIO_SET_VOICE_BUF_CBOFRAC_FRACTION                               0x0000ffff
#define NV1BA0_PIO_SET_VOICE_BUF_CBOFRAC_FRACTION__BITSHIFT                     0
        NV1BA0_PIO_SET_VOICE_BUF_CBO                    = 0x000003d8,
#define NV1BA0_PIO_SET_VOICE_BUF_CBO_OFFSET                                     0x00ffffff
#define NV1BA0_PIO_SET_VOICE_BUF_CBO_OFFSET__BITSHIFT                           0
        NV1BA0_PIO_SET_VOICE_CFG_BUF_EBO                = 0x000003dc,
#define NV1BA0_PIO_SET_VOICE_CFG_BUF_EBO_OFFSET                                 0x00ffffff
#define NV1BA0_PIO_SET_VOICE_CFG_BUF_EBO_OFFSET__BITSHIFT                       0
        NV1BA0_PIO_SET_VOICE_TAR_VOLA                   = 0x00000360,
#define NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME6_B3_0                              0x0000000f
#define NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME6_B3_0__BITSHIFT                    0
#define NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME0                                   0x0000fff0
#define NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME0__BITSHIFT                         4
#define NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME7_B3_0                              0x000f0000
#define NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME7_B3_0__BITSHIFT                    16
#define NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME1                                   0xfff00000
#define NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME1__BITSHIFT                         20
        NV1BA0_PIO_SET_VOICE_TAR_VOLB                   = 0x00000364,
#define NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME6_B7_4                              0x0000000f
#define NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME6_B7_4__BITSHIFT                    0
#define NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME2                                   0x0000fff0
#define NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME2__BITSHIFT                         4
#define NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME7_B7_4                              0x000f0000
#define NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME7_B7_4__BITSHIFT                    16
#define NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME3                                   0xfff00000
#define NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME3__BITSHIFT                         20
        NV1BA0_PIO_SET_VOICE_TAR_VOLC                   = 0x00000368,
#define NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME6_B11_8                             0x0000000f
#define NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME6_B11_8__BITSHIFT                   0
#define NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME4                                   0x0000fff0
#define NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME4__BITSHIFT                         4
#define NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME7_B11_8                             0x000f0000
#define NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME7_B11_8__BITSHIFT                   16
#define NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME5                                   0xfff00000
#define NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME5__BITSHIFT                         20
        NV1BA0_PIO_SET_VOICE_LFO_ENV                    = 0x0000036c,
#define NV1BA0_PIO_SET_VOICE_LFO_ENV_EA_RELEASERATE                             0x00000fff
#define NV1BA0_PIO_SET_VOICE_LFO_ENV_EA_RELEASERATE__BITSHIFT                   0
#define NV1BA0_PIO_SET_VOICE_LFO_ENV_LFOADLT                                    0x003ff000
#define NV1BA0_PIO_SET_VOICE_LFO_ENV_LFOADLT__BITSHIFT                          12
#define NV1BA0_PIO_SET_VOICE_LFO_ENV_LFOFDLT                                    0xffc00000
#define NV1BA0_PIO_SET_VOICE_LFO_ENV_LFOFDLT__BITSHIFT                          22
        NV1BA0_PIO_SET_VOICE_LFO_MOD                    = 0x00000370,
#define NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAAM                                     0x000000ff
#define NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAAM__BITSHIFT                           0
#define NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAFM                                     0x0000ff00
#define NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAFM__BITSHIFT                           8
#define NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAFC                                     0x00ff0000
#define NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAFC__BITSHIFT                           16
#define NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOFFM                                     0xff000000
#define NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOFFM__BITSHIFT                           24
        NV1BA0_PIO_SET_VOICE_TAR_FCA                    = 0x00000374,
#define NV1BA0_PIO_SET_VOICE_TAR_FCA_FC0                                        0x0000ffff
#define NV1BA0_PIO_SET_VOICE_TAR_FCA_FC0__BITSHIFT                              0
#define NV1BA0_PIO_SET_VOICE_TAR_FCA_FC1                                        0xffff0000
#define NV1BA0_PIO_SET_VOICE_TAR_FCA_FC1__BITSHIFT                              16
        NV1BA0_PIO_SET_VOICE_TAR_FCB                    = 0x00000378,
#define NV1BA0_PIO_SET_VOICE_TAR_FCB_FC2                                        0x0000ffff
#define NV1BA0_PIO_SET_VOICE_TAR_FCB_FC2__BITSHIFT                              0
#define NV1BA0_PIO_SET_VOICE_TAR_FCB_FC3                                        0xffff0000
#define NV1BA0_PIO_SET_VOICE_TAR_FCB_FC3__BITSHIFT                              16
        NV1BA0_PIO_SET_VOICE_TAR_PITCH                  = 0x0000037c,
#define NV1BA0_PIO_SET_VOICE_TAR_PITCH_STEP                                     0xffff0000
#define NV1BA0_PIO_SET_VOICE_TAR_PITCH_STEP__BITSHIFT                           16
#define NV1BA0_PIO_SET_VOICE_TAR_PITCH_STEP_SMAX                                0x00001fff
        NV1BA0_PIO_SET_CURRENT_HRTF_ENTRY               = 0x00000160,
#define NV1BA0_PIO_SET_CURRENT_HRTF_ENTRY_HANDLE                                0x0000ffff
#define NV1BA0_PIO_SET_CURRENT_HRTF_ENTRY_HANDLE__BITSHIFT                      0
        NV1BA0_PIO_SET_HRIR_0                           = 0x00000400,
        NV1BA0_PIO_SET_HRIR_1                           = 0x00000404,
        NV1BA0_PIO_SET_HRIR_2                           = 0x00000408,
        NV1BA0_PIO_SET_HRIR_3                           = 0x0000040c,
        NV1BA0_PIO_SET_HRIR_4                           = 0x00000410,
        NV1BA0_PIO_SET_HRIR_5                           = 0x00000414,
        NV1BA0_PIO_SET_HRIR_6                           = 0x00000418,
        NV1BA0_PIO_SET_HRIR_7                           = 0x0000041c,
        NV1BA0_PIO_SET_HRIR_8                           = 0x00000420,
        NV1BA0_PIO_SET_HRIR_9                           = 0x00000424,
        NV1BA0_PIO_SET_HRIR_10                          = 0x00000428,
        NV1BA0_PIO_SET_HRIR_11                          = 0x0000042c,
        NV1BA0_PIO_SET_HRIR_12                          = 0x00000430,
        NV1BA0_PIO_SET_HRIR_13                          = 0x00000434,
        NV1BA0_PIO_SET_HRIR_14                          = 0x00000438,
#define NV1BA0_PIO_SET_HRIR_LEFT0                                               0x000000ff
#define NV1BA0_PIO_SET_HRIR_LEFT0__BITSHIFT                                     0
#define NV1BA0_PIO_SET_HRIR_RIGHT0                                              0x0000ff00
#define NV1BA0_PIO_SET_HRIR_RIGHT0__BITSHIFT                                    8
#define NV1BA0_PIO_SET_HRIR_LEFT1                                               0x00ff0000
#define NV1BA0_PIO_SET_HRIR_LEFT1__BITSHIFT                                     16
#define NV1BA0_PIO_SET_HRIR_RIGHT1                                              0xff000000
#define NV1BA0_PIO_SET_HRIR_RIGHT1__BITSHIFT                                    24
        NV1BA0_PIO_SET_HRIR_X                           = 0x0000043c,
#define NV1BA0_PIO_SET_HRIR_X_LEFT30                                            0x000000ff
#define NV1BA0_PIO_SET_HRIR_X_LEFT30__BITSHIFT                                  0
#define NV1BA0_PIO_SET_HRIR_X_RIGHT30                                           0x0000ff00
#define NV1BA0_PIO_SET_HRIR_X_RIGHT30__BITSHIFT                                 8
#define NV1BA0_PIO_SET_HRIR_X_ITD                                               0xffff0000
#define NV1BA0_PIO_SET_HRIR_X_ITD__BITSHIFT                                     16
#define NV1BA0_PIO_SET_HRIR_X_ITD_SMIN                                          0x0000ac01
#define NV1BA0_PIO_SET_HRIR_X_ITD_SMAX                                          0x000053ff
        NV1BA0_PIO_SET_CURRENT_SSL_CONTEXT_DMA          = 0x0000018c,
        NV1BA0_PIO_SET_CURRENT_SSL                      = 0x00000190,
#define NV1BA0_PIO_SET_CURRENT_SSL_BASE_PAGE                                    0x003fffc0
#define NV1BA0_PIO_SET_CURRENT_SSL_BASE_PAGE__BITSHIFT                          6
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_0             = 0x00000600,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_1             = 0x00000608,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_2             = 0x00000610,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_3             = 0x00000618,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_4             = 0x00000620,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_5             = 0x00000628,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_6             = 0x00000630,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_7             = 0x00000638,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_8             = 0x00000640,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_9             = 0x00000648,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_10            = 0x00000650,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_11            = 0x00000658,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_12            = 0x00000660,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_13            = 0x00000668,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_14            = 0x00000670,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_15            = 0x00000678,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_16            = 0x00000680,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_17            = 0x00000688,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_18            = 0x00000690,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_19            = 0x00000698,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_20            = 0x000006a0,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_21            = 0x000006a8,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_22            = 0x000006b0,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_23            = 0x000006b8,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_24            = 0x000006c0,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_25            = 0x000006c8,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_26            = 0x000006d0,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_27            = 0x000006d8,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_28            = 0x000006e0,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_29            = 0x000006e8,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_30            = 0x000006f0,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_31            = 0x000006f8,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_32            = 0x00000700,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_33            = 0x00000708,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_34            = 0x00000710,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_35            = 0x00000718,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_36            = 0x00000720,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_37            = 0x00000728,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_38            = 0x00000730,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_39            = 0x00000738,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_40            = 0x00000740,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_41            = 0x00000748,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_42            = 0x00000750,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_43            = 0x00000758,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_44            = 0x00000760,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_45            = 0x00000768,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_46            = 0x00000770,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_47            = 0x00000778,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_48            = 0x00000780,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_49            = 0x00000788,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_50            = 0x00000790,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_51            = 0x00000798,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_52            = 0x000007a0,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_53            = 0x000007a8,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_54            = 0x000007b0,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_55            = 0x000007b8,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_56            = 0x000007c0,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_57            = 0x000007c8,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_58            = 0x000007d0,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_59            = 0x000007d8,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_60            = 0x000007e0,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_61            = 0x000007e8,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_62            = 0x000007f0,
        NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_63            = 0x000007f8,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_0             = 0x00000604,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_1             = 0x0000060c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_2             = 0x00000614,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_3             = 0x0000061c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_4             = 0x00000624,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_5             = 0x0000062c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_6             = 0x00000634,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_7             = 0x0000063c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_8             = 0x00000644,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_9             = 0x0000064c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_10            = 0x00000654,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_11            = 0x0000065c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_12            = 0x00000664,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_13            = 0x0000066c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_14            = 0x00000674,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_15            = 0x0000067c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_16            = 0x00000684,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_17            = 0x0000068c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_18            = 0x00000694,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_19            = 0x0000069c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_20            = 0x000006a4,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_21            = 0x000006ac,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_22            = 0x000006b4,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_23            = 0x000006bc,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_24            = 0x000006c4,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_25            = 0x000006cc,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_26            = 0x000006d4,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_27            = 0x000006dc,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_28            = 0x000006e4,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_29            = 0x000006ec,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_30            = 0x000006f4,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_31            = 0x000006fc,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_32            = 0x00000704,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_33            = 0x0000070c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_34            = 0x00000714,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_35            = 0x0000071c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_36            = 0x00000724,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_37            = 0x0000072c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_38            = 0x00000734,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_39            = 0x0000073c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_40            = 0x00000744,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_41            = 0x0000074c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_42            = 0x00000754,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_43            = 0x0000075c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_44            = 0x00000764,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_45            = 0x0000076c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_46            = 0x00000774,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_47            = 0x0000077c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_48            = 0x00000784,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_49            = 0x0000078c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_50            = 0x00000794,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_51            = 0x0000079c,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_52            = 0x000007a4,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_53            = 0x000007ac,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_54            = 0x000007b4,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_55            = 0x000007bc,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_56            = 0x000007c4,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_57            = 0x000007cc,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_58            = 0x000007d4,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_59            = 0x000007dc,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_60            = 0x000007e4,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_61            = 0x000007ec,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_62            = 0x000007f4,
        NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_63            = 0x000007fc,
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_PARAMETER                             0x0000ffff
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_PARAMETER__BITSHIFT                   0
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE                        0x00030000
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE__BITSHIFT              16
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE_B8                     0
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE_B16                    1
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE_ADPCM                  2
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE_B32                    3
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_SAMPLES_PER_BLOCK                     0x007c0000
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_SAMPLES_PER_BLOCK__BITSHIFT           18
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_STEREO                                0x00800000
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_STEREO__BITSHIFT                      23
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_STEREO_NOTOK                          0
#define NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_STEREO_OK                             1
        NV1BA0_PIO_SET_CURRENT_INBUF_SGE_CONTEXT_DMA    = 0x00000800,
        NV1BA0_PIO_SET_CURRENT_INBUF_SGE                = 0x00000804,
        NV1BA0_PIO_SET_CURRENT_INBUF_SGE_OFFSET         = 0x00000808,
#define NV1BA0_PIO_SET_CURRENT_INBUF_SGE_OFFSET_PARAMETER                       0xfffff000
#define NV1BA0_PIO_SET_CURRENT_INBUF_SGE_OFFSET_PARAMETER__BITSHIFT             12
        NV1BA0_PIO_SET_OUTBUF_BA_0                      = 0x00001000,
        NV1BA0_PIO_SET_OUTBUF_BA_1                      = 0x00001008,
        NV1BA0_PIO_SET_OUTBUF_BA_2                      = 0x00001010,
        NV1BA0_PIO_SET_OUTBUF_BA_3                      = 0x00001018,
#define NV1BA0_PIO_SET_OUTBUF_BA_ADDRESS                                        0x007fff00
#define NV1BA0_PIO_SET_OUTBUF_BA_ADDRESS__BITSHIFT                              8
        NV1BA0_PIO_SET_OUTBUF_LEN_0                     = 0x00001004,
        NV1BA0_PIO_SET_OUTBUF_LEN_1                     = 0x0000100c,
        NV1BA0_PIO_SET_OUTBUF_LEN_2                     = 0x00001014,
        NV1BA0_PIO_SET_OUTBUF_LEN_3                     = 0x0000101c,
#define NV1BA0_PIO_SET_OUTBUF_LEN_VALUE                                         0x007fff00
#define NV1BA0_PIO_SET_OUTBUF_LEN_VALUE__BITSHIFT                               8
        NV1BA0_PIO_SET_CURRENT_OUTBUF_SGE               = 0x00001800,
        NV1BA0_PIO_SET_CURRENT_BUFFER_SGE_CONTEXT_DMA   = 0x00001804,
        NV1BA0_PIO_SET_CURRENT_OUTBUF_SGE_OFFSET        = 0x00001808,
#define NV1BA0_PIO_SET_CURRENT_OUTBUF_SGE_OFFSET_PARAMETER                      0xfffff000
#define NV1BA0_PIO_SET_CURRENT_OUTBUF_SGE_OFFSET_PARAMETER__BITSHIFT            12
};

static const char *const apu_papu_vp_name[] = {
    NAMER(NV1BA0_PIO_FREE),
#if 0
NV1BA0_PIO_FREE_COUNT                                                   0x00000ffc
NV1BA0_PIO_FREE_COUNT__BITSHIFT                                         2
NV1BA0_PIO_FREE_COUNT_FIFO_FULL                                         0
#endif
    NAMER(NV1BA0_PIO_INFO),
#if 0
NV1BA0_PIO_INFO_PIO                                                     0x00000001
NV1BA0_PIO_INFO_PIO__BITSHIFT                                           0
NV1BA0_PIO_INFO_PIO_EMPTY_AND_IDLE                                      0
NV1BA0_PIO_INFO_PIO_BUSY                                                1
NV1BA0_PIO_INFO_DMA                                                     0x00000002
NV1BA0_PIO_INFO_DMA__BITSHIFT                                           1
NV1BA0_PIO_INFO_DMA_EMPTY_AND_IDLE                                      0
NV1BA0_PIO_INFO_DMA_BUSY                                                1
#endif
    NAMER(NV1BA0_CHANNEL_DMA_PUT),
#if 0
NV1BA0_CHANNEL_DMA_PUT_OFFSET                                           0x00000ffc
NV1BA0_CHANNEL_DMA_PUT_OFFSET__BITSHIFT                                 2
#endif
    NAMER(NV1BA0_CHANNEL_DMA_GET),
#if 0
NV1BA0_CHANNEL_DMA_GET_OFFSET                                           0x00000ffc
NV1BA0_CHANNEL_DMA_GET_OFFSET__BITSHIFT                                 2
#endif
    NAMER(NV1BA0_NOP),
#if 0
NV1BA0_NOP_NOTIFICATION_STATUS_DONE_SUCCESS                             1
#endif
    NAMER(NV1BA0_PIO_SYNCHRONIZE),
#if 0
NV1BA0_PIO_SYNCHRONIZE_PARAMETER_NO_OPERATION                           0
NV1BA0_PIO_SYNCHRONIZE_PARAMETER_WAIT_FOR_IDLE                          1
NV1BA0_PIO_SYNCHRONIZE_PARAMETER_WAIT_FOR_IDLE_WRITE_PE_NOTIFY          2
NV1BA0_PIO_SYNCHRONIZE_PARAMETER_WAIT_FOR_IDLE_WRITE_PE_NOTIFY_AWAKEN   3
#endif
    NAMER(NV1BA0_PIO_SET_TIME),
    NAMER(NV1BA0_PIO_SET_MODE),
#if 0
NV1BA0_PIO_SET_MODE_PARAMETER_OFF                                       0
NV1BA0_PIO_SET_MODE_PARAMETER_RUN                                       1
#endif
    NAMER(NV1BA0_PIO_SET_ANTECEDENT_VOICE),
#if 0
NV1BA0_PIO_SET_ANTECEDENT_VOICE_HANDLE                                  0x0000ffff
NV1BA0_PIO_SET_ANTECEDENT_VOICE_HANDLE__BITSHIFT                        0
NV1BA0_PIO_SET_ANTECEDENT_VOICE_HANDLE_NULL                             0x0000ffff
NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST                                    0x00030000
NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST__BITSHIFT                          16
NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST_INHERIT                            0
NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST_2D_TOP                             1
NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST_3D_TOP                             2
NV1BA0_PIO_SET_ANTECEDENT_VOICE_LIST_MP_TOP                             3
#endif
    NAMER(NV1BA0_PIO_VOICE_ON),
#if 0
NV1BA0_PIO_VOICE_ON_HANDLE                                              0x0000ffff
NV1BA0_PIO_VOICE_ON_HANDLE__BITSHIFT                                    0
NV1BA0_PIO_VOICE_ON_ENVF                                                0x0f000000
NV1BA0_PIO_VOICE_ON_ENVF__BITSHIFT                                      24
NV1BA0_PIO_VOICE_ON_ENVF_DISABLE                                        0
NV1BA0_PIO_VOICE_ON_ENVF_DELAY                                          1
NV1BA0_PIO_VOICE_ON_ENVF_ATTACK                                         2
NV1BA0_PIO_VOICE_ON_ENVF_HOLD                                           3
NV1BA0_PIO_VOICE_ON_ENVA                                                0xf0000000
NV1BA0_PIO_VOICE_ON_ENVA__BITSHIFT                                      28
NV1BA0_PIO_VOICE_ON_ENVA_DISABLE                                        0
NV1BA0_PIO_VOICE_ON_ENVA_DELAY                                          1
NV1BA0_PIO_VOICE_ON_ENVA_ATTACK                                         2
NV1BA0_PIO_VOICE_ON_ENVA_HOLD                                           3
#endif
    NAMER(NV1BA0_PIO_VOICE_OFF),
#if 0
NV1BA0_PIO_VOICE_OFF_HANDLE                                             0x0000ffff
NV1BA0_PIO_VOICE_OFF_HANDLE__BITSHIFT                                   0
#endif
    NAMER(NV1BA0_PIO_VOICE_RELEASE),
#if 0
NV1BA0_PIO_VOICE_RELEASE_HANDLE                                         0x0000ffff
NV1BA0_PIO_VOICE_RELEASE_HANDLE__BITSHIFT                               0
#endif
    NAMER(NV1BA0_PIO_GET_VOICE_POSITION),
#if 0
NV1BA0_PIO_GET_VOICE_POSITION_HANDLE                                    0x0000ffff
NV1BA0_PIO_GET_VOICE_POSITION_HANDLE__BITSHIFT                          0
#endif
    NAMER(NV1BA0_PIO_VOICE_PAUSE),
#if 0
NV1BA0_PIO_VOICE_PAUSE_HANDLE                                           0x0000ffff
NV1BA0_PIO_VOICE_PAUSE_HANDLE__BITSHIFT                                 0
NV1BA0_PIO_VOICE_PAUSE_ACTION                                           0x00040000
NV1BA0_PIO_VOICE_PAUSE_ACTION__BITSHIFT                                 18
NV1BA0_PIO_VOICE_PAUSE_ACTION_RESUME                                    0
NV1BA0_PIO_VOICE_PAUSE_ACTION_STOP                                      1
#endif
    NAMER(NV1BA0_PIO_SET_CONTEXT_DMA_NOTIFY),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_0),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_1),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_2),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_3),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_4),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_5),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_6),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_7),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_8),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_9),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_10),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_11),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_12),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_13),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_14),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_15),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_16),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_17),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_18),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_19),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_20),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_21),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_22),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_23),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_24),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_25),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_26),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_27),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_28),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_29),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_30),
    NAMER(NV1BA0_PIO_SET_SUBMIX_HEADROOM_31),
#if 0
NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT                                   0x00000007
NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT__BITSHIFT                         0
NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_ZEROBITS                          0
NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_ONEBIT                            1
NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_TWOBITS                           2
NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_THREEBITS                         3
NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_FOURBITS                          4
NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_FIVEBITS                          5
NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_SIXBITS                           6
NV1BA0_PIO_SET_SUBMIX_HEADROOM_AMOUNT_SEVENBITS                         7
#endif
    NAMER(NV1BA0_PIO_SET_HRTF_HEADROOM),
#if 0
NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT                                     0x00000007
NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT__BITSHIFT                           0
NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_ZEROBITS                            0
NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_ONEBIT                              1
NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_TWOBITS                             2
NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_THREEBITS                           3
NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_FOURBITS                            4
NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_FIVEBITS                            5
NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_SIXBITS                             6
NV1BA0_PIO_SET_HRTF_HEADROOM_AMOUNT_SEVENBITS                           7
#endif
    NAMER(NV1BA0_PIO_SET_HRTF_SUBMIX_0),
    NAMER(NV1BA0_PIO_SET_HRTF_SUBMIX_1),
    NAMER(NV1BA0_PIO_SET_HRTF_SUBMIX_2),
    NAMER(NV1BA0_PIO_SET_HRTF_SUBMIX_3),
#if 0
NV1BA0_PIO_SET_HRTF_SUBMIX_BIN                                          0x0000001f
NV1BA0_PIO_SET_HRTF_SUBMIX_BIN__BITSHIFT                                0
#endif
    NAMER(NV1BA0_PIO_SET_HRTF_SUBMIXES),
#if 0
NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN0                                       0x0000001f
NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN0__BITSHIFT                             0
NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN1                                       0x00001f00
NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN1__BITSHIFT                             8
NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN2                                       0x001f0000
NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN2__BITSHIFT                             16
NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN3                                       0x1f000000
NV1BA0_PIO_SET_HRTF_SUBMIXES_BIN3__BITSHIFT                             24
#endif
    NAMER(NV1BA0_PIO_SET_VOLUME_TRACKING),
#if 0
NV1BA0_PIO_SET_VOLUME_TRACKING_PARAMETER                                0x00000fff
NV1BA0_PIO_SET_VOLUME_TRACKING_PARAMETER__BITSHIFT                      0
#endif
    NAMER(NV1BA0_PIO_SET_PITCH_TRACKING),
#if 0
NV1BA0_PIO_SET_PITCH_TRACKING_PARAMETER                                 0x00000fff
NV1BA0_PIO_SET_PITCH_TRACKING_PARAMETER__BITSHIFT                       0
#endif
    NAMER(NV1BA0_PIO_SET_HRTF_TRACKING),
#if 0
NV1BA0_PIO_SET_HRTF_TRACKING_PARAMETER                                  0x00000fff
NV1BA0_PIO_SET_HRTF_TRACKING_PARAMETER__BITSHIFT                        0
#endif
    NAMER(NV1BA0_PIO_SET_ITD_TRACKING),
#if 0
NV1BA0_PIO_SET_ITD_TRACKING_PARAMETER                                   0x00000fff
NV1BA0_PIO_SET_ITD_TRACKING_PARAMETER__BITSHIFT                         0
#endif
    NAMER(NV1BA0_PIO_SET_FILTER_TRACKING),
#if 0
NV1BA0_PIO_SET_FILTER_TRACKING_PARAMETER                                0x00000fff
NV1BA0_PIO_SET_FILTER_TRACKING_PARAMETER__BITSHIFT                      0
#endif
    NAMER(NV1BA0_PIO_SET_CURRENT_VOICE),
#if 0
NV1BA0_PIO_SET_CURRENT_VOICE_HANDLE                                     0x0000ffff
NV1BA0_PIO_SET_CURRENT_VOICE_HANDLE__BITSHIFT                           0
#endif
    NAMER(NV1BA0_PIO_VOICE_LOCK),
#if 0
NV1BA0_PIO_VOICE_LOCK_PARAMETER                                         0x000000ff
NV1BA0_PIO_VOICE_LOCK_PARAMETER__BITSHIFT                               0
NV1BA0_PIO_VOICE_LOCK_PARAMETER_OFF                                     0
NV1BA0_PIO_VOICE_LOCK_PARAMETER_ON                                      1
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_CFG_VBIN),
#if 0
NV1BA0_PIO_SET_VOICE_CFG_VBIN_V0BIN                                     0x0000001f
NV1BA0_PIO_SET_VOICE_CFG_VBIN_V0BIN__BITSHIFT                           0
NV1BA0_PIO_SET_VOICE_CFG_VBIN_V1BIN                                     0x000003e0
NV1BA0_PIO_SET_VOICE_CFG_VBIN_V1BIN__BITSHIFT                           5
NV1BA0_PIO_SET_VOICE_CFG_VBIN_V2BIN                                     0x00007c00
NV1BA0_PIO_SET_VOICE_CFG_VBIN_V2BIN__BITSHIFT                           10
NV1BA0_PIO_SET_VOICE_CFG_VBIN_V3BIN                                     0x001f0000
NV1BA0_PIO_SET_VOICE_CFG_VBIN_V3BIN__BITSHIFT                           16
NV1BA0_PIO_SET_VOICE_CFG_VBIN_V4BIN                                     0x03e00000
NV1BA0_PIO_SET_VOICE_CFG_VBIN_V4BIN__BITSHIFT                           21
NV1BA0_PIO_SET_VOICE_CFG_VBIN_V5BIN                                     0x7c000000
NV1BA0_PIO_SET_VOICE_CFG_VBIN_V5BIN__BITSHIFT                           26
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_CFG_FMT),
#if 0
NV1BA0_PIO_SET_VOICE_CFG_FMT_V6BIN                                      0x0000001f
NV1BA0_PIO_SET_VOICE_CFG_FMT_V6BIN__BITSHIFT                            0
NV1BA0_PIO_SET_VOICE_CFG_FMT_V7BIN                                      0x000003e0
NV1BA0_PIO_SET_VOICE_CFG_FMT_V7BIN__BITSHIFT                            5
NV1BA0_PIO_SET_VOICE_CFG_FMT_V6PHASE                                    0x00000400
NV1BA0_PIO_SET_VOICE_CFG_FMT_V6PHASE__BITSHIFT                          10
NV1BA0_PIO_SET_VOICE_CFG_FMT_V6PHASE_POSITIVE                           0
NV1BA0_PIO_SET_VOICE_CFG_FMT_V6PHASE_NEGATIVE                           1
NV1BA0_PIO_SET_VOICE_CFG_FMT_V7PHASE                                    0x00000800
NV1BA0_PIO_SET_VOICE_CFG_FMT_V7PHASE__BITSHIFT                          11
NV1BA0_PIO_SET_VOICE_CFG_FMT_V7PHASE_POSITIVE                           0
NV1BA0_PIO_SET_VOICE_CFG_FMT_V7PHASE_NEGATIVE                           1
NV1BA0_PIO_SET_VOICE_CFG_FMT_GAIN                                       0x00001000
NV1BA0_PIO_SET_VOICE_CFG_FMT_GAIN__BITSHIFT                             12
NV1BA0_PIO_SET_VOICE_CFG_FMT_GAIN_X1                                    0
NV1BA0_PIO_SET_VOICE_CFG_FMT_GAIN_X2                                    1
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM                                   0x0000e000
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM__BITSHIFT                         13
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC0_FLT0                         0
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC0_FLT1                         1
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC0_FLT2                         2
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC0_FLT3                         3
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC1_FLT0                         4
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC1_FLT1                         5
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC1_FLT2                         6
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_SRC1_FLT3                         7
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS0                        0
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS1                        1
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS2                        2
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS3                        3
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS4                        4
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS5                        5
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS6                        6
NV1BA0_PIO_SET_VOICE_CFG_FMT_HEADROOM_MULTIPASS7                        7
NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLES_PER_BLOCK                          0x001f0000
NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLES_PER_BLOCK__BITSHIFT                16
NV1BA0_PIO_SET_VOICE_CFG_FMT_MULTIPASS                                  0x00200000
NV1BA0_PIO_SET_VOICE_CFG_FMT_MULTIPASS__BITSHIFT                        21
NV1BA0_PIO_SET_VOICE_CFG_FMT_MULTIPASS_OFF                              0
NV1BA0_PIO_SET_VOICE_CFG_FMT_MULTIPASS_ON                               1
NV1BA0_PIO_SET_VOICE_CFG_FMT_LINKED_VOICE                               0x00400000
NV1BA0_PIO_SET_VOICE_CFG_FMT_LINKED_VOICE__BITSHIFT                     22
NV1BA0_PIO_SET_VOICE_CFG_FMT_LINKED_VOICE_OFF                           0
NV1BA0_PIO_SET_VOICE_CFG_FMT_LINKED_VOICE_ON                            1
NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST                                    0x00800000
NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST__BITSHIFT                          23
NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST_OFF                                0
NV1BA0_PIO_SET_VOICE_CFG_FMT_PERSIST_ON                                 1
NV1BA0_PIO_SET_VOICE_CFG_FMT_DATA_TYPE                                  0x01000000
NV1BA0_PIO_SET_VOICE_CFG_FMT_DATA_TYPE__BITSHIFT                        24
NV1BA0_PIO_SET_VOICE_CFG_FMT_DATA_TYPE_BUFFER                           0
NV1BA0_PIO_SET_VOICE_CFG_FMT_DATA_TYPE_STREAM                           1
NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP                                       0x02000000
NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP__BITSHIFT                             25
NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP_OFF                                   0
NV1BA0_PIO_SET_VOICE_CFG_FMT_LOOP_ON                                    1
NV1BA0_PIO_SET_VOICE_CFG_FMT_CLEAR_MIX                                  0x04000000
NV1BA0_PIO_SET_VOICE_CFG_FMT_CLEAR_MIX__BITSHIFT                        26
NV1BA0_PIO_SET_VOICE_CFG_FMT_CLEAR_MIX_FALSE                            0
NV1BA0_PIO_SET_VOICE_CFG_FMT_CLEAR_MIX_TRUE                             1
NV1BA0_PIO_SET_VOICE_CFG_FMT_STEREO                                     0x08000000
NV1BA0_PIO_SET_VOICE_CFG_FMT_STEREO__BITSHIFT                           27
NV1BA0_PIO_SET_VOICE_CFG_FMT_STEREO_DISABLE                             0
NV1BA0_PIO_SET_VOICE_CFG_FMT_STEREO_ENABLE                              1
NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE                                0x30000000
NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE__BITSHIFT                      28
NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE_U8                             0
NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE_S16                            1
NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE_S24                            2
NV1BA0_PIO_SET_VOICE_CFG_FMT_SAMPLE_SIZE_S32                            3
NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE                             0xc0000000
NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE__BITSHIFT                   30
NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_B8                          0
NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_B16                         1
NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_ADPCM                       2
NV1BA0_PIO_SET_VOICE_CFG_FMT_CONTAINER_SIZE_B32                         3
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_CFG_ENV0),
#if 0
NV1BA0_PIO_SET_VOICE_CFG_ENV0_EA_ATTACKRATE                             0x00000fff
NV1BA0_PIO_SET_VOICE_CFG_ENV0_EA_ATTACKRATE__BITSHIFT                   0
NV1BA0_PIO_SET_VOICE_CFG_ENV0_EA_DELAYTIME                              0x00fff000
NV1BA0_PIO_SET_VOICE_CFG_ENV0_EA_DELAYTIME__BITSHIFT                    12
NV1BA0_PIO_SET_VOICE_CFG_ENV0_EF_PITCHSCALE                             0xff000000
NV1BA0_PIO_SET_VOICE_CFG_ENV0_EF_PITCHSCALE__BITSHIFT                   24
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_CFG_ENVA),
#if 0
NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_DECAYRATE                              0x00000fff
NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_DECAYRATE__BITSHIFT                    0
NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_HOLDTIME                               0x00fff000
NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_HOLDTIME__BITSHIFT                     12
NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_SUSTAINLEVEL                           0xff000000
NV1BA0_PIO_SET_VOICE_CFG_ENVA_EA_SUSTAINLEVEL__BITSHIFT                 24
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_CFG_ENV1),
#if 0
NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_ATTACKRATE                             0x00000fff
NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_ATTACKRATE__BITSHIFT                   0
NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_DELAYTIME                              0x00fff000
NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_DELAYTIME__BITSHIFT                    12
NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_FCSCALE                                0xff000000
NV1BA0_PIO_SET_VOICE_CFG_ENV1_EF_FCSCALE__BITSHIFT                      24
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_CFG_ENVF),
#if 0
NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_DECAYRATE                              0x00000fff
NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_DECAYRATE__BITSHIFT                    0
NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_HOLDTIME                               0x00fff000
NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_HOLDTIME__BITSHIFT                     12
NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_SUSTAINLEVEL                           0xff000000
NV1BA0_PIO_SET_VOICE_CFG_ENVF_EF_SUSTAINLEVEL__BITSHIFT                 24
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_CFG_MISC),
#if 0
NV1BA0_PIO_SET_VOICE_CFG_MISC_EF_RELEASERATE                            0x00000fff
NV1BA0_PIO_SET_VOICE_CFG_MISC_EF_RELEASERATE__BITSHIFT                  0
NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOA_DELAYMODE                            0x00004000
NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOA_DELAYMODE__BITSHIFT                  14
NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOA_DELAYMODE_NORMAL                     0
NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOA_DELAYMODE_DELAY                      1
NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOF_DELAYMODE                            0x00008000
NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOF_DELAYMODE__BITSHIFT                  15
NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOF_DELAYMODE_NORMAL                     0
NV1BA0_PIO_SET_VOICE_CFG_MISC_LFOF_DELAYMODE_DELAY                      1
NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE                                     0x00030000
NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE__BITSHIFT                           16
NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_BYPASS                              0
NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_STEREO_DLS2                         1
NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_STEREO_P_EQ                         2
NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_STEREO_BYPASS                       3
NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_MONO_DLS2                           1
NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_MONO_P_EQ                           2
NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_MONO_DLS2_P_EQ                      3
NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_THREED_DLS2_I3DL2                   1
NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_THREED_P_EQ_I3DL2                   2
NV1BA0_PIO_SET_VOICE_CFG_MISC_FMODE_THREED_I3DL2                        3
NV1BA0_PIO_SET_VOICE_CFG_MISC_BPQ                                       0x001c0000
NV1BA0_PIO_SET_VOICE_CFG_MISC_BPQ__BITSHIFT                             18
NV1BA0_PIO_SET_VOICE_CFG_MISC_NOTIFY_INTERRUPT                          0x00800000
NV1BA0_PIO_SET_VOICE_CFG_MISC_NOTIFY_INTERRUPT__BITSHIFT                23
NV1BA0_PIO_SET_VOICE_CFG_MISC_NOTIFY_INTERRUPT_DISABLE                  0
NV1BA0_PIO_SET_VOICE_CFG_MISC_NOTIFY_INTERRUPT_ENABLE                   1
NV1BA0_PIO_SET_VOICE_CFG_MISC_V_GSCNT                                   0xff000000
NV1BA0_PIO_SET_VOICE_CFG_MISC_V_GSCNT__BITSHIFT                         24
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_TAR_HRTF),
#if 0
NV1BA0_PIO_SET_VOICE_TAR_HRTF_HANDLE                                    0x0000ffff
NV1BA0_PIO_SET_VOICE_TAR_HRTF_HANDLE__BITSHIFT                          0
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_PAR_LFODLY),
#if 0
NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOADLY                                 0x00007fff
NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOADLY__BITSHIFT                       0
NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOADR                                  0x00008000
NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOADR__BITSHIFT                        15
NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOFDLY                                 0x7fff0000
NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOFDLY__BITSHIFT                       16
NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOFDR                                  0x80000000
NV1BA0_PIO_SET_VOICE_PAR_LFODLY_LFOFDR__BITSHIFT                        31
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_SSL_A),
#if 0
NV1BA0_PIO_SET_VOICE_SSL_A_COUNT                                        0x000000ff
NV1BA0_PIO_SET_VOICE_SSL_A_COUNT__BITSHIFT                              0
NV1BA0_PIO_SET_VOICE_SSL_A_BASE                                         0x00ffff00
NV1BA0_PIO_SET_VOICE_SSL_A_BASE__BITSHIFT                               8
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_SSL_B),
#if 0
NV1BA0_PIO_SET_VOICE_SSL_B_COUNT                                        0x000000ff
NV1BA0_PIO_SET_VOICE_SSL_B_COUNT__BITSHIFT                              0
NV1BA0_PIO_SET_VOICE_SSL_B_BASE                                         0x00ffff00
NV1BA0_PIO_SET_VOICE_SSL_B_BASE__BITSHIFT                               8
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_CFG_BUF_BASE),
#if 0
NV1BA0_PIO_SET_VOICE_CFG_BUF_BASE_OFFSET                                0x00ffffff
NV1BA0_PIO_SET_VOICE_CFG_BUF_BASE_OFFSET__BITSHIFT                      0
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_CFG_BUF_LBO),
#if 0
NV1BA0_PIO_SET_VOICE_CFG_BUF_LBO_OFFSET                                 0x00ffffff
NV1BA0_PIO_SET_VOICE_CFG_BUF_LBO_OFFSET__BITSHIFT                       0
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_BUF_CBOFRAC),
#if 0
NV1BA0_PIO_SET_VOICE_BUF_CBOFRAC_FRACTION                               0x0000ffff
NV1BA0_PIO_SET_VOICE_BUF_CBOFRAC_FRACTION__BITSHIFT                     0
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_BUF_CBO),
#if 0
NV1BA0_PIO_SET_VOICE_BUF_CBO_OFFSET                                     0x00ffffff
NV1BA0_PIO_SET_VOICE_BUF_CBO_OFFSET__BITSHIFT                           0
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_CFG_BUF_EBO),
#if 0
NV1BA0_PIO_SET_VOICE_CFG_BUF_EBO_OFFSET                                 0x00ffffff
NV1BA0_PIO_SET_VOICE_CFG_BUF_EBO_OFFSET__BITSHIFT                       0
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_TAR_VOLA),
#if 0
NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME6_B3_0                              0x0000000f
NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME6_B3_0__BITSHIFT                    0
NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME0                                   0x0000fff0
NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME0__BITSHIFT                         4
NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME7_B3_0                              0x000f0000
NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME7_B3_0__BITSHIFT                    16
NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME1                                   0xfff00000
NV1BA0_PIO_SET_VOICE_TAR_VOLA_VOLUME1__BITSHIFT                         20
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_TAR_VOLB),
#if 0
NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME6_B7_4                              0x0000000f
NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME6_B7_4__BITSHIFT                    0
NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME2                                   0x0000fff0
NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME2__BITSHIFT                         4
NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME7_B7_4                              0x000f0000
NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME7_B7_4__BITSHIFT                    16
NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME3                                   0xfff00000
NV1BA0_PIO_SET_VOICE_TAR_VOLB_VOLUME3__BITSHIFT                         20
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_TAR_VOLC),
#if 0
NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME6_B11_8                             0x0000000f
NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME6_B11_8__BITSHIFT                   0
NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME4                                   0x0000fff0
NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME4__BITSHIFT                         4
NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME7_B11_8                             0x000f0000
NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME7_B11_8__BITSHIFT                   16
NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME5                                   0xfff00000
NV1BA0_PIO_SET_VOICE_TAR_VOLC_VOLUME5__BITSHIFT                         20
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_LFO_ENV),
#if 0
NV1BA0_PIO_SET_VOICE_LFO_ENV_EA_RELEASERATE                             0x00000fff
NV1BA0_PIO_SET_VOICE_LFO_ENV_EA_RELEASERATE__BITSHIFT                   0
NV1BA0_PIO_SET_VOICE_LFO_ENV_LFOADLT                                    0x003ff000
NV1BA0_PIO_SET_VOICE_LFO_ENV_LFOADLT__BITSHIFT                          12
NV1BA0_PIO_SET_VOICE_LFO_ENV_LFOFDLT                                    0xffc00000
NV1BA0_PIO_SET_VOICE_LFO_ENV_LFOFDLT__BITSHIFT                          22
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_LFO_MOD),
#if 0
NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAAM                                     0x000000ff
NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAAM__BITSHIFT                           0
NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAFM                                     0x0000ff00
NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAFM__BITSHIFT                           8
NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAFC                                     0x00ff0000
NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOAFC__BITSHIFT                           16
NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOFFM                                     0xff000000
NV1BA0_PIO_SET_VOICE_LFO_MOD_LFOFFM__BITSHIFT                           24
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_TAR_FCA),
#if 0
NV1BA0_PIO_SET_VOICE_TAR_FCA_FC0                                        0x0000ffff
NV1BA0_PIO_SET_VOICE_TAR_FCA_FC0__BITSHIFT                              0
NV1BA0_PIO_SET_VOICE_TAR_FCA_FC1                                        0xffff0000
NV1BA0_PIO_SET_VOICE_TAR_FCA_FC1__BITSHIFT                              16
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_TAR_FCB),
#if 0
NV1BA0_PIO_SET_VOICE_TAR_FCB_FC2                                        0x0000ffff
NV1BA0_PIO_SET_VOICE_TAR_FCB_FC2__BITSHIFT                              0
NV1BA0_PIO_SET_VOICE_TAR_FCB_FC3                                        0xffff0000
NV1BA0_PIO_SET_VOICE_TAR_FCB_FC3__BITSHIFT                              16
#endif
    NAMER(NV1BA0_PIO_SET_VOICE_TAR_PITCH),
#if 0
NV1BA0_PIO_SET_VOICE_TAR_PITCH_STEP                                     0xffff0000
NV1BA0_PIO_SET_VOICE_TAR_PITCH_STEP__BITSHIFT                           16
NV1BA0_PIO_SET_VOICE_TAR_PITCH_STEP_SMAX                                0x00001fff
#endif
    NAMER(NV1BA0_PIO_SET_CURRENT_HRTF_ENTRY),
#if 0
NV1BA0_PIO_SET_CURRENT_HRTF_ENTRY_HANDLE                                0x0000ffff
NV1BA0_PIO_SET_CURRENT_HRTF_ENTRY_HANDLE__BITSHIFT                      0
#endif
    NAMER(NV1BA0_PIO_SET_HRIR_0),
    NAMER(NV1BA0_PIO_SET_HRIR_1),
    NAMER(NV1BA0_PIO_SET_HRIR_2),
    NAMER(NV1BA0_PIO_SET_HRIR_3),
    NAMER(NV1BA0_PIO_SET_HRIR_4),
    NAMER(NV1BA0_PIO_SET_HRIR_5),
    NAMER(NV1BA0_PIO_SET_HRIR_6),
    NAMER(NV1BA0_PIO_SET_HRIR_7),
    NAMER(NV1BA0_PIO_SET_HRIR_8),
    NAMER(NV1BA0_PIO_SET_HRIR_9),
    NAMER(NV1BA0_PIO_SET_HRIR_10),
    NAMER(NV1BA0_PIO_SET_HRIR_11),
    NAMER(NV1BA0_PIO_SET_HRIR_12),
    NAMER(NV1BA0_PIO_SET_HRIR_13),
    NAMER(NV1BA0_PIO_SET_HRIR_14),
#if 0
NV1BA0_PIO_SET_HRIR_LEFT0                                               0x000000ff
NV1BA0_PIO_SET_HRIR_LEFT0__BITSHIFT                                     0
NV1BA0_PIO_SET_HRIR_RIGHT0                                              0x0000ff00
NV1BA0_PIO_SET_HRIR_RIGHT0__BITSHIFT                                    8
NV1BA0_PIO_SET_HRIR_LEFT1                                               0x00ff0000
NV1BA0_PIO_SET_HRIR_LEFT1__BITSHIFT                                     16
NV1BA0_PIO_SET_HRIR_RIGHT1                                              0xff000000
NV1BA0_PIO_SET_HRIR_RIGHT1__BITSHIFT                                    24
#endif
    NAMER(NV1BA0_PIO_SET_HRIR_X),
#if 0
NV1BA0_PIO_SET_HRIR_X_LEFT30                                            0x000000ff
NV1BA0_PIO_SET_HRIR_X_LEFT30__BITSHIFT                                  0
NV1BA0_PIO_SET_HRIR_X_RIGHT30                                           0x0000ff00
NV1BA0_PIO_SET_HRIR_X_RIGHT30__BITSHIFT                                 8
NV1BA0_PIO_SET_HRIR_X_ITD                                               0xffff0000
NV1BA0_PIO_SET_HRIR_X_ITD__BITSHIFT                                     16
NV1BA0_PIO_SET_HRIR_X_ITD_SMIN                                          0x0000ac01
NV1BA0_PIO_SET_HRIR_X_ITD_SMAX                                          0x000053ff
#endif
    NAMER(NV1BA0_PIO_SET_CURRENT_SSL_CONTEXT_DMA),
    NAMER(NV1BA0_PIO_SET_CURRENT_SSL),
#if 0
NV1BA0_PIO_SET_CURRENT_SSL_BASE_PAGE                                    0x003fffc0
NV1BA0_PIO_SET_CURRENT_SSL_BASE_PAGE__BITSHIFT                          6
#endif
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_0),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_1),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_2),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_3),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_4),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_5),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_6),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_7),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_8),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_9),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_10),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_11),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_12),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_13),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_14),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_15),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_16),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_17),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_18),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_19),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_20),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_21),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_22),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_23),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_24),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_25),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_26),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_27),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_28),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_29),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_30),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_31),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_32),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_33),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_34),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_35),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_36),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_37),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_38),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_39),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_40),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_41),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_42),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_43),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_44),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_45),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_46),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_47),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_48),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_49),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_50),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_51),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_52),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_53),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_54),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_55),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_56),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_57),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_58),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_59),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_60),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_61),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_62),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_OFFSET_63),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_0),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_1),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_2),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_3),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_4),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_5),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_6),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_7),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_8),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_9),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_10),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_11),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_12),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_13),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_14),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_15),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_16),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_17),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_18),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_19),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_20),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_21),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_22),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_23),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_24),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_25),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_26),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_27),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_28),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_29),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_30),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_31),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_32),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_33),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_34),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_35),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_36),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_37),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_38),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_39),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_40),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_41),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_42),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_43),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_44),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_45),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_46),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_47),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_48),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_49),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_50),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_51),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_52),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_53),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_54),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_55),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_56),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_57),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_58),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_59),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_60),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_61),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_62),
    NAMER(NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_63),
#if 0
NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_PARAMETER                             0x0000ffff
NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_PARAMETER__BITSHIFT                   0
NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE                        0x00030000
NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE__BITSHIFT              16
NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE_B8                     0
NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE_B16                    1
NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE_ADPCM                  2
NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_CONTAINER_SIZE_B32                    3
NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_SAMPLES_PER_BLOCK                     0x007c0000
NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_SAMPLES_PER_BLOCK__BITSHIFT           18
NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_STEREO                                0x00800000
NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_STEREO__BITSHIFT                      23
NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_STEREO_NOTOK                          0
NV1BA0_PIO_SET_SSL_SEGMENT_LENGTH_STEREO_OK                             1
#endif
    NAMER(NV1BA0_PIO_SET_CURRENT_INBUF_SGE_CONTEXT_DMA),
    NAMER(NV1BA0_PIO_SET_CURRENT_INBUF_SGE),
    NAMER(NV1BA0_PIO_SET_CURRENT_INBUF_SGE_OFFSET),
#if 0
NV1BA0_PIO_SET_CURRENT_INBUF_SGE_OFFSET_PARAMETER                       0xfffff000
NV1BA0_PIO_SET_CURRENT_INBUF_SGE_OFFSET_PARAMETER__BITSHIFT             12
#endif
    NAMER(NV1BA0_PIO_SET_OUTBUF_BA_0),
    NAMER(NV1BA0_PIO_SET_OUTBUF_BA_1),
    NAMER(NV1BA0_PIO_SET_OUTBUF_BA_2),
    NAMER(NV1BA0_PIO_SET_OUTBUF_BA_3),
#if 0
NV1BA0_PIO_SET_OUTBUF_BA_ADDRESS                                        0x007fff00
NV1BA0_PIO_SET_OUTBUF_BA_ADDRESS__BITSHIFT                              8
#endif
    NAMER(NV1BA0_PIO_SET_OUTBUF_LEN_0),
    NAMER(NV1BA0_PIO_SET_OUTBUF_LEN_1),
    NAMER(NV1BA0_PIO_SET_OUTBUF_LEN_2),
    NAMER(NV1BA0_PIO_SET_OUTBUF_LEN_3),
#if 0
NV1BA0_PIO_SET_OUTBUF_LEN_VALUE                                         0x007fff00
NV1BA0_PIO_SET_OUTBUF_LEN_VALUE__BITSHIFT                               8
#endif
    NAMER(NV1BA0_PIO_SET_CURRENT_OUTBUF_SGE),
    NAMER(NV1BA0_PIO_SET_CURRENT_BUFFER_SGE_CONTEXT_DMA),
    NAMER(NV1BA0_PIO_SET_CURRENT_OUTBUF_SGE_OFFSET),
#if 0
NV1BA0_PIO_SET_CURRENT_OUTBUF_SGE_OFFSET_PARAMETER                      0xfffff000
NV1BA0_PIO_SET_CURRENT_OUTBUF_SGE_OFFSET_PARAMETER__BITSHIFT            12
#endif
};

