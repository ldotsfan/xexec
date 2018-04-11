
enum {
        NV_PCRTC_INTR_0                             = 0x00000100,
#define NV_PCRTC_INTR_0_VBLANK                                                  0x00000001
#define NV_PCRTC_INTR_0_VBLANK__BITSHIFT                                        0
#define NV_PCRTC_INTR_0_VBLANK_NOT_PENDING                                      0x00000000
#define NV_PCRTC_INTR_0_VBLANK_PENDING                                          0x00000001
#define NV_PCRTC_INTR_0_VBLANK_RESET                                            0x00000001
        NV_PCRTC_INTR_EN_0                          = 0x00000140,
#define NV_PCRTC_INTR_EN_0_VBLANK                                               0x00000001
#define NV_PCRTC_INTR_EN_0_VBLANK__BITSHIFT                                     0
#define NV_PCRTC_INTR_EN_0_VBLANK_DISABLED                                      0x00000000
#define NV_PCRTC_INTR_EN_0_VBLANK_ENABLED                                       0x00000001
        NV_PCRTC_START                              = 0x00000800,
#define NV_PCRTC_START_ADDRESS                                                  0xfffffffc
#define NV_PCRTC_START_ADDRESS__BITSHIFT                                        2
        NV_PCRTC_CONFIG                             = 0x00000804,
#define NV_PCRTC_CONFIG_START_ADDRESS                                           0x00000007
#define NV_PCRTC_CONFIG_START_ADDRESS__BITSHIFT                                 0
#define NV_PCRTC_CONFIG_START_ADDRESS_VGA                                       0x00000000
#define NV_PCRTC_CONFIG_START_ADDRESS_NON_VGA                                   0x00000001
#define NV_PCRTC_CONFIG_START_ADDRESS_HSYNC                                     0x00000002
#define NV_PCRTC_CONFIG_ENDIAN                                                  0x80000000
#define NV_PCRTC_CONFIG_ENDIAN__BITSHIFT                                        31
#define NV_PCRTC_CONFIG_ENDIAN_LITTLE                                           0x00000000
#define NV_PCRTC_CONFIG_ENDIAN_BIG                                              0x00000001
        NV_PCRTC_RASTER                             = 0x00000808,
#define NV_PCRTC_RASTER_POSITION                                                0x000007ff
#define NV_PCRTC_RASTER_POSITION__BITSHIFT                                      0
#define NV_PCRTC_RASTER_SA_LOAD                                                 0x00003000
#define NV_PCRTC_RASTER_SA_LOAD__BITSHIFT                                       12
#define NV_PCRTC_RASTER_SA_LOAD_DISPLAY                                         0x00000000
#define NV_PCRTC_RASTER_SA_LOAD_BEFORE                                          0x00000001
#define NV_PCRTC_RASTER_SA_LOAD_AFTER                                           0x00000002
#define NV_PCRTC_RASTER_VERT_BLANK                                              0x00010000
#define NV_PCRTC_RASTER_VERT_BLANK__BITSHIFT                                    16
#define NV_PCRTC_RASTER_VERT_BLANK_ACTIVE                                       0x00000001
#define NV_PCRTC_RASTER_VERT_BLANK_INACTIVE                                     0x00000000
#define NV_PCRTC_RASTER_FIELD                                                   0x00100000
#define NV_PCRTC_RASTER_FIELD__BITSHIFT                                         20
#define NV_PCRTC_RASTER_FIELD_EVEN                                              0x00000000
#define NV_PCRTC_RASTER_FIELD_ODD                                               0x00000001
        NV_PCRTC_CURSOR                             = 0x0000080c,
        NV_PCRTC_CURSOR_CONFIG                      = 0x00000810,
#define NV_PCRTC_CURSOR_CONFIG_ENABLE                                           0x00000001
#define NV_PCRTC_CURSOR_CONFIG_ENABLE__BITSHIFT                                 0
#define NV_PCRTC_CURSOR_CONFIG_ENABLE_ENABLE                                    0x00000001
#define NV_PCRTC_CURSOR_CONFIG_ENABLE_DISABLE                                   0x00000000
#define NV_PCRTC_CURSOR_CONFIG_SCAN_DOUBLE                                      0x00000010
#define NV_PCRTC_CURSOR_CONFIG_SCAN_DOUBLE__BITSHIFT                            4
#define NV_PCRTC_CURSOR_CONFIG_SCAN_DOUBLE_ENABLE                               0x00000001
#define NV_PCRTC_CURSOR_CONFIG_SCAN_DOUBLE_DISABLE                              0x00000000
#define NV_PCRTC_CURSOR_CONFIG_ADDRESS_SPACE                                    0x00000100
#define NV_PCRTC_CURSOR_CONFIG_ADDRESS_SPACE__BITSHIFT                          8
#define NV_PCRTC_CURSOR_CONFIG_ADDRESS_SPACE_PNVM                               0x00000001
#define NV_PCRTC_CURSOR_CONFIG_ADDRESS_SPACE_PINST                              0x00000000
#define NV_PCRTC_CURSOR_CONFIG_CUR_BPP                                          0x00001000
#define NV_PCRTC_CURSOR_CONFIG_CUR_BPP__BITSHIFT                                12
#define NV_PCRTC_CURSOR_CONFIG_CUR_BPP_16                                       0x00000000
#define NV_PCRTC_CURSOR_CONFIG_CUR_BPP_32                                       0x00000001
#define NV_PCRTC_CURSOR_CONFIG_CUR_PIXELS                                       0x00010000
#define NV_PCRTC_CURSOR_CONFIG_CUR_PIXELS__BITSHIFT                             16
#define NV_PCRTC_CURSOR_CONFIG_CUR_PIXELS_32                                    0x00000000
#define NV_PCRTC_CURSOR_CONFIG_CUR_PIXELS_64                                    0x00000001
#define NV_PCRTC_CURSOR_CONFIG_CUR_LINES                                        0x0ff00000
#define NV_PCRTC_CURSOR_CONFIG_CUR_LINES__BITSHIFT                              20
#define NV_PCRTC_CURSOR_CONFIG_CUR_LINES_32                                     0x00000020
#define NV_PCRTC_CURSOR_CONFIG_CUR_LINES_64                                     0x00000040
#define NV_PCRTC_CURSOR_CONFIG_CUR_BLEND                                        0x10000000
#define NV_PCRTC_CURSOR_CONFIG_CUR_BLEND__BITSHIFT                              28
#define NV_PCRTC_CURSOR_CONFIG_CUR_BLEND_ROP                                    0x00000000
#define NV_PCRTC_CURSOR_CONFIG_CUR_BLEND_ALPHA                                  0x00000001
        NV_PCRTC_VIP_RASTER                         = 0x00000814,
#define NV_PCRTC_VIP_RASTER_POSITION                                            0x000007ff
#define NV_PCRTC_VIP_RASTER_POSITION__BITSHIFT                                  0
        NV_PCRTC_GPIO                               = 0x00000818,
#define NV_PCRTC_GPIO_1_OUTPUT                                                  0x00000001
#define NV_PCRTC_GPIO_1_OUTPUT__BITSHIFT                                        0
#define NV_PCRTC_GPIO_1_OUTPUT_0                                                0x00000000
#define NV_PCRTC_GPIO_1_ENABLE                                                  0x00000010
#define NV_PCRTC_GPIO_1_ENABLE__BITSHIFT                                        4
#define NV_PCRTC_GPIO_1_ENABLE_DISABLE                                          0x00000001
#define NV_PCRTC_GPIO_1_ENABLE_ENABLE                                           0x00000000
#define NV_PCRTC_GPIO_0_INPUT                                                   0x00000100
#define NV_PCRTC_GPIO_0_INPUT__BITSHIFT                                         8
#define NV_PCRTC_GPIO_0_OUTPUT                                                  0x00010000
#define NV_PCRTC_GPIO_0_OUTPUT__BITSHIFT                                        16
#define NV_PCRTC_GPIO_0_OUTPUT_0                                                0x00000000
#define NV_PCRTC_GPIO_0_ENABLE                                                  0x00100000
#define NV_PCRTC_GPIO_0_ENABLE__BITSHIFT                                        20
#define NV_PCRTC_GPIO_0_ENABLE_DISABLE                                          0x00000001
#define NV_PCRTC_GPIO_0_ENABLE_ENABLE                                           0x00000000
#define NV_PCRTC_GPIO_1_INPUT                                                   0x01000000
#define NV_PCRTC_GPIO_1_INPUT__BITSHIFT                                         24
        NV_PCRTC_GPIO_EXT                           = 0x0000081c,
#define NV_PCRTC_GPIO_EXT_2_OUTPUT                                              0x00000001
#define NV_PCRTC_GPIO_EXT_2_OUTPUT__BITSHIFT                                    0
#define NV_PCRTC_GPIO_EXT_2_OUTPUT_0                                            0x00000000
#define NV_PCRTC_GPIO_EXT_2_ENABLE                                              0x00000002
#define NV_PCRTC_GPIO_EXT_2_ENABLE__BITSHIFT                                    1
#define NV_PCRTC_GPIO_EXT_2_ENABLE_DISABLE                                      0x00000001
#define NV_PCRTC_GPIO_EXT_2_ENABLE_ENABLE                                       0x00000000
#define NV_PCRTC_GPIO_EXT_2_INPUT                                               0x00000004
#define NV_PCRTC_GPIO_EXT_2_INPUT__BITSHIFT                                     2
#define NV_PCRTC_GPIO_EXT_3_OUTPUT                                              0x00000010
#define NV_PCRTC_GPIO_EXT_3_OUTPUT__BITSHIFT                                    4
#define NV_PCRTC_GPIO_EXT_3_OUTPUT_0                                            0x00000000
#define NV_PCRTC_GPIO_EXT_3_ENABLE                                              0x00000020
#define NV_PCRTC_GPIO_EXT_3_ENABLE__BITSHIFT                                    5
#define NV_PCRTC_GPIO_EXT_3_ENABLE_DISABLE                                      0x00000001
#define NV_PCRTC_GPIO_EXT_3_ENABLE_ENABLE                                       0x00000000
#define NV_PCRTC_GPIO_EXT_3_INPUT                                               0x00000040
#define NV_PCRTC_GPIO_EXT_3_INPUT__BITSHIFT                                     6
#define NV_PCRTC_GPIO_EXT_4_OUTPUT                                              0x00000100
#define NV_PCRTC_GPIO_EXT_4_OUTPUT__BITSHIFT                                    8
#define NV_PCRTC_GPIO_EXT_4_OUTPUT_0                                            0x00000000
#define NV_PCRTC_GPIO_EXT_4_ENABLE                                              0x00000200
#define NV_PCRTC_GPIO_EXT_4_ENABLE__BITSHIFT                                    9
#define NV_PCRTC_GPIO_EXT_4_ENABLE_DISABLE                                      0x00000001
#define NV_PCRTC_GPIO_EXT_4_ENABLE_ENABLE                                       0x00000000
#define NV_PCRTC_GPIO_EXT_4_INPUT                                               0x00000400
#define NV_PCRTC_GPIO_EXT_4_INPUT__BITSHIFT                                     10
#define NV_PCRTC_GPIO_EXT_5_OUTPUT                                              0x00001000
#define NV_PCRTC_GPIO_EXT_5_OUTPUT__BITSHIFT                                    12
#define NV_PCRTC_GPIO_EXT_5_OUTPUT_0                                            0x00000000
#define NV_PCRTC_GPIO_EXT_5_ENABLE                                              0x00002000
#define NV_PCRTC_GPIO_EXT_5_ENABLE__BITSHIFT                                    13
#define NV_PCRTC_GPIO_EXT_5_ENABLE_DISABLE                                      0x00000001
#define NV_PCRTC_GPIO_EXT_5_ENABLE_ENABLE                                       0x00000000
#define NV_PCRTC_GPIO_EXT_5_INPUT                                               0x00004000
#define NV_PCRTC_GPIO_EXT_5_INPUT__BITSHIFT                                     14
        NV_PCRTC_RASTER_START                       = 0x00000830,
#define NV_PCRTC_RASTER_START_VAL                                               0x0000ffff
#define NV_PCRTC_RASTER_START_VAL__BITSHIFT                                     0
#define NV_PCRTC_RASTER_POL                                                     0x80000000
#define NV_PCRTC_RASTER_POL__BITSHIFT                                           31
#define NV_PCRTC_RASTER_POL_IN_RANGE                                            0x00000000
#define NV_PCRTC_RASTER_POL_OUT_RANGE                                           0x00000001
        NV_PCRTC_RASTER_STOP                        = 0x00000834,
#define NV_PCRTC_RASTER_STOP_VAL                                                0x0000ffff
#define NV_PCRTC_RASTER_STOP_VAL__BITSHIFT                                      0
        NV_PCRTC_FIFO_CNTRL                         = 0x00000838,
#define NV_PCRTC_FIFO_CNTRL_ADDRESS                                             0x0000007f
#define NV_PCRTC_FIFO_CNTRL_ADDRESS__BITSHIFT                                   0
#define NV_PCRTC_FIFO_CNTRL_RAM                                                 0x00001000
#define NV_PCRTC_FIFO_CNTRL_RAM__BITSHIFT                                       12
#define NV_PCRTC_FIFO_CNTR_TESTMODE                                             0x00010000
#define NV_PCRTC_FIFO_CNTR_TESTMODE__BITSHIFT                                   16
#define NV_PCRTC_FIFO_CNTR_TESTMODE_ENABLE                                      0x00000001
#define NV_PCRTC_FIFO_CNTR_TESTMODE_DISABLE                                     0x00000000
        NV_PCRTC_FIFO_DATA_0                        = 0x00000840,
        NV_PCRTC_FIFO_DATA_1                        = 0x00000844,
        NV_PCRTC_FIFO_DATA_2                        = 0x00000848,
        NV_PCRTC_FIFO_DATA_3                        = 0x0000084c,
        NV_PCRTC_ENGINE_CTRL                        = 0x00000860,
#define NV_PCRTC_ENGINE_CTRL_GPIO                                               0x00000001
#define NV_PCRTC_ENGINE_CTRL_GPIO__BITSHIFT                                     0
#define NV_PCRTC_ENGINE_CTRL_GPIO_DEFAULT                                       0x00000001
#define NV_PCRTC_ENGINE_CTRL_GPIO_DEFAULT__BITSHIFT                             0x00000000
#define NV_PCRTC_ENGINE_CTRL_GPIO_ENABLE                                        0x00000002
#define NV_PCRTC_ENGINE_CTRL_GPIO_ENABLE__BITSHIFT                              0x00000001
#define NV_PCRTC_ENGINE_CTRL_I2C                                                0x00000010
#define NV_PCRTC_ENGINE_CTRL_I2C__BITSHIFT                                      4
#define NV_PCRTC_ENGINE_CTRL_I2C_DEFAULT                                        0x00000001
#define NV_PCRTC_ENGINE_CTRL_I2C_DEFAULT__BITSHIFT                              0x00000000
#define NV_PCRTC_ENGINE_CTRL_I2C_ENABLE                                         0x00000002
#define NV_PCRTC_ENGINE_CTRL_I2C_ENABLE__BITSHIFT                               0x00000001
#define NV_PCRTC_ENGINE_CTRL_TV                                                 0x00000100
#define NV_PCRTC_ENGINE_CTRL_TV__BITSHIFT                                       8
#define NV_PCRTC_ENGINE_CTRL_TV_DEFAULT                                         0x00000001
#define NV_PCRTC_ENGINE_CTRL_TV_DEFAULT__BITSHIFT                               0x00000000
#define NV_PCRTC_ENGINE_CTRL_TV_ENABLE                                          0x00000002
#define NV_PCRTC_ENGINE_CTRL_TV_ENABLE__BITSHIFT                                0x00000001
#define NV_PCRTC_ENGINE_CTRL_VS                                                 0x00001000
#define NV_PCRTC_ENGINE_CTRL_VS__BITSHIFT                                       12
#define NV_PCRTC_ENGINE_CTRL_VS_DEFAULT                                         0x00000001
#define NV_PCRTC_ENGINE_CTRL_VS_DEFAULT__BITSHIFT                               0x00000000
#define NV_PCRTC_ENGINE_CTRL_VS_ENABLE                                          0x00000002
#define NV_PCRTC_ENGINE_CTRL_VS_ENABLE__BITSHIFT                                0x00000001
};

static const char *const nv2a_pcrtc_intr_name[] = {
    NAMEB(0, vblank),
};

static const char *const nv2a_pcrtc_name[] = {
    NAMER(NV_PCRTC_INTR_0),
    NAMER(NV_PCRTC_INTR_EN_0),
    NAMER(NV_PCRTC_START),
    NAMER(NV_PCRTC_CONFIG),
    NAMER(NV_PCRTC_RASTER),
    NAMER(NV_PCRTC_CURSOR),
    NAMER(NV_PCRTC_CURSOR_CONFIG),
    NAMER(NV_PCRTC_VIP_RASTER),
    NAMER(NV_PCRTC_GPIO),
    NAMER(NV_PCRTC_GPIO_EXT),
    NAMER(NV_PCRTC_RASTER_START),
    NAMER(NV_PCRTC_RASTER_STOP),
    NAMER(NV_PCRTC_FIFO_CNTRL),
    NAMER(NV_PCRTC_FIFO_DATA_0),
    NAMER(NV_PCRTC_FIFO_DATA_1),
    NAMER(NV_PCRTC_FIFO_DATA_2),
    NAMER(NV_PCRTC_FIFO_DATA_3),
    NAMER(NV_PCRTC_ENGINE_CTRL),
};

