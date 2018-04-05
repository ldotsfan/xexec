
enum {
        NV_PVIDEO_DEBUG_0                           = 0x00000080,
#define NV_PVIDEO_DEBUG_0_HLF_RATE_ROW_RD                                       0x00000001
#define NV_PVIDEO_DEBUG_0_HLF_RATE_ROW_RD__BITSHIFT                             0
#define NV_PVIDEO_DEBUG_0_HLF_RATE_ROW_RD_DISABLED                              0x00000000
#define NV_PVIDEO_DEBUG_0_HLF_RATE_ROW_RD_ENABLED                               0x00000001
#define NV_PVIDEO_DEBUG_0_LIMIT_CHECK                                           0x00000010
#define NV_PVIDEO_DEBUG_0_LIMIT_CHECK__BITSHIFT                                 4
#define NV_PVIDEO_DEBUG_0_LIMIT_CHECK_DISABLED                                  0x00000000
#define NV_PVIDEO_DEBUG_0_LIMIT_CHECK_ENABLED                                   0x00000001
#define NV_PVIDEO_DEBUG_0_HUE_FOLD                                              0x00000100
#define NV_PVIDEO_DEBUG_0_HUE_FOLD__BITSHIFT                                    8
#define NV_PVIDEO_DEBUG_0_HUE_FOLD_DISABLED                                     0x00000000
#define NV_PVIDEO_DEBUG_0_HUE_FOLD_ENABLED                                      0x00000001
#define NV_PVIDEO_DEBUG_0_ODD_FIELD_TOP                                         0x00001000
#define NV_PVIDEO_DEBUG_0_ODD_FIELD_TOP__BITSHIFT                               12
#define NV_PVIDEO_DEBUG_0_ODD_FIELD_TOP_FALSE                                   0x00000000
#define NV_PVIDEO_DEBUG_0_ODD_FIELD_TOP_TRUE                                    0x00000001
        NV_PVIDEO_DEBUG_1                           = 0x00000084,
#define NV_PVIDEO_DEBUG_1_REQ_DELAY                                             0x000007ff
#define NV_PVIDEO_DEBUG_1_REQ_DELAY__BITSHIFT                                   0
#define NV_PVIDEO_DEBUG_1_REQ_DELAY_DEFAULT                                     0x00000064
#define NV_PVIDEO_DEBUG_1_REQ_DELAY_INIT                                        0x00000050
        NV_PVIDEO_DEBUG_2                           = 0x00000088,
#define NV_PVIDEO_DEBUG_2_BURST1                                                0x00000fc0
#define NV_PVIDEO_DEBUG_2_BURST1__BITSHIFT                                      6
#define NV_PVIDEO_DEBUG_2_BURST1_DEFAULT                                        0x00000008
#define NV_PVIDEO_DEBUG_2_BURST1_INIT                                           0x00000010
#define NV_PVIDEO_DEBUG_2_BURST2                                                0x0fc00000
#define NV_PVIDEO_DEBUG_2_BURST2__BITSHIFT                                      22
#define NV_PVIDEO_DEBUG_2_BURST2_DEFAULT                                        0x00000010
#define NV_PVIDEO_DEBUG_2_BURST2_INIT                                           0x00000018
        NV_PVIDEO_DEBUG_3                           = 0x0000008c,
#define NV_PVIDEO_DEBUG_3_WATER_MARK1                                           0x00000ff0
#define NV_PVIDEO_DEBUG_3_WATER_MARK1__BITSHIFT                                 4
#define NV_PVIDEO_DEBUG_3_WATER_MARK1_DEFAULT                                   0x0000004b
#define NV_PVIDEO_DEBUG_3_WATER_MARK1_INIT                                      0x00000040
#define NV_PVIDEO_DEBUG_3_WATER_MARK2                                           0x0ff00000
#define NV_PVIDEO_DEBUG_3_WATER_MARK2__BITSHIFT                                 20
#define NV_PVIDEO_DEBUG_3_WATER_MARK2_DEFAULT                                   0x0000003b
#define NV_PVIDEO_DEBUG_3_WATER_MARK2_INIT                                      0x00000040
        NV_PVIDEO_DEBUG_4                           = 0x00000090,
#define NV_PVIDEO_DEBUG_4_V_COEFF_B                                             0x00ffffe0
#define NV_PVIDEO_DEBUG_4_V_COEFF_B__BITSHIFT                                   5
#define NV_PVIDEO_DEBUG_4_V_COEFF_B_DEFAULT                                     0x0000b505
#define NV_PVIDEO_DEBUG_4_V_COEFF_B_ALWAYS                                      0x00000000
#define NV_PVIDEO_DEBUG_4_V_COEFF_B_NEVER                                       0x0007ffff
        NV_PVIDEO_DEBUG_5                           = 0x00000094,
#define NV_PVIDEO_DEBUG_5_H_L_COEFF_D                                           0x003ffff0
#define NV_PVIDEO_DEBUG_5_H_L_COEFF_D__BITSHIFT                                 4
#define NV_PVIDEO_DEBUG_5_H_L_COEFF_D_DEFAULT                                   0x00018816
#define NV_PVIDEO_DEBUG_5_H_L_COEFF_D_ALWAYS                                    0x00000000
#define NV_PVIDEO_DEBUG_5_H_L_COEFF_D_NEVER                                     0x0003ffff
        NV_PVIDEO_DEBUG_6                           = 0x00000098,
#define NV_PVIDEO_DEBUG_6_H_L_COEFF_C                                           0x003ffff0
#define NV_PVIDEO_DEBUG_6_H_L_COEFF_C__BITSHIFT                                 4
#define NV_PVIDEO_DEBUG_6_H_L_COEFF_C_DEFAULT                                   0x00012c73
#define NV_PVIDEO_DEBUG_6_H_L_COEFF_C_ALWAYS                                    0x00000000
#define NV_PVIDEO_DEBUG_6_H_L_COEFF_C_NEVER                                     0x0003ffff
        NV_PVIDEO_DEBUG_7                           = 0x0000009c,
#define NV_PVIDEO_DEBUG_7_H_L_COEFF_B                                           0x003ffff0
#define NV_PVIDEO_DEBUG_7_H_L_COEFF_B__BITSHIFT                                 4
#define NV_PVIDEO_DEBUG_7_H_L_COEFF_B_DEFAULT                                   0x00000000
#define NV_PVIDEO_DEBUG_7_H_L_COEFF_B_ALWAYS                                    0x00000000
#define NV_PVIDEO_DEBUG_7_H_L_COEFF_B_NEVER                                     0x0003ffff
        NV_PVIDEO_DEBUG_8                           = 0x000000a0,
#define NV_PVIDEO_DEBUG_8_PIPE_FILL                                             0x000007f0
#define NV_PVIDEO_DEBUG_8_PIPE_FILL__BITSHIFT                                   4
#define NV_PVIDEO_DEBUG_8_PIPE_FILL_DEFAULT                                     0x0000000b
        NV_PVIDEO_DEBUG_9                           = 0x000000a4,
#define NV_PVIDEO_DEBUG_9_FIFO_A_UNDERFLOW                                      0x00000001
#define NV_PVIDEO_DEBUG_9_FIFO_A_UNDERFLOW__BITSHIFT                            0
#define NV_PVIDEO_DEBUG_9_FIFO_A_UNDERFLOW_FALSE                                0x00000000
#define NV_PVIDEO_DEBUG_9_FIFO_A_UNDERFLOW_TRUE                                 0x00000001
#define NV_PVIDEO_DEBUG_9_FIFO_A_OVERFLOW                                       0x00000010
#define NV_PVIDEO_DEBUG_9_FIFO_A_OVERFLOW__BITSHIFT                             4
#define NV_PVIDEO_DEBUG_9_FIFO_A_OVERFLOW_FALSE                                 0x00000000
#define NV_PVIDEO_DEBUG_9_FIFO_A_OVERFLOW_TRUE                                  0x00000001
#define NV_PVIDEO_DEBUG_9_FIFO_B_UNDERFLOW                                      0x00000100
#define NV_PVIDEO_DEBUG_9_FIFO_B_UNDERFLOW__BITSHIFT                            8
#define NV_PVIDEO_DEBUG_9_FIFO_B_UNDERFLOW_FALSE                                0x00000000
#define NV_PVIDEO_DEBUG_9_FIFO_B_UNDERFLOW_TRUE                                 0x00000001
#define NV_PVIDEO_DEBUG_9_FIFO_B_OVERFLOW                                       0x00001000
#define NV_PVIDEO_DEBUG_9_FIFO_B_OVERFLOW__BITSHIFT                             12
#define NV_PVIDEO_DEBUG_9_FIFO_B_OVERFLOW_FALSE                                 0x00000000
#define NV_PVIDEO_DEBUG_9_FIFO_B_OVERFLOW_TRUE                                  0x00000001
        NV_PVIDEO_DEBUG_10                          = 0x000000a8,
#define NV_PVIDEO_DEBUG_10_SCREEN_LINE                                          0x00001fff
#define NV_PVIDEO_DEBUG_10_SCREEN_LINE__BITSHIFT                                0
#define NV_PVIDEO_DEBUG_10_SCREEN_LINE_FIRST                                    0x00000000
#define NV_PVIDEO_DEBUG_10_SCAN_COUNT                                           0x001f0000
#define NV_PVIDEO_DEBUG_10_SCAN_COUNT__BITSHIFT                                 16
#define NV_PVIDEO_DEBUG_10_SCAN_COUNT_FIRST                                     0x00000000
#define NV_PVIDEO_DEBUG_10_SCAN_COUNT_OVERFLOW                                  0x00000010
#define NV_PVIDEO_DEBUG_10_SCANNING                                             0x03000000
#define NV_PVIDEO_DEBUG_10_SCANNING__BITSHIFT                                   24
#define NV_PVIDEO_DEBUG_10_SCANNING_NEITHER                                     0x00000000
#define NV_PVIDEO_DEBUG_10_SCANNING_BUFFER_0                                    0x00000002
#define NV_PVIDEO_DEBUG_10_SCANNING_BUFFER_1                                    0x00000003
        NV_PVIDEO_INTR                              = 0x00000100,
#define NV_PVIDEO_INTR_BUFFER_0                                                 0x00000001
#define NV_PVIDEO_INTR_BUFFER_0__BITSHIFT                                       0
#define NV_PVIDEO_INTR_BUFFER_0_NOT_PENDING                                     0x00000000
#define NV_PVIDEO_INTR_BUFFER_0_PENDING                                         0x00000001
#define NV_PVIDEO_INTR_BUFFER_1                                                 0x00000010
#define NV_PVIDEO_INTR_BUFFER_1__BITSHIFT                                       4
#define NV_PVIDEO_INTR_BUFFER_1_NOT_PENDING                                     0x00000000
#define NV_PVIDEO_INTR_BUFFER_1_PENDING                                         0x00000001
        NV_PVIDEO_INTR_REASON                       = 0x00000104,
#define NV_PVIDEO_INTR_REASON_BUFFER_0                                          0x00000001
#define NV_PVIDEO_INTR_REASON_BUFFER_0__BITSHIFT                                0
#define NV_PVIDEO_INTR_REASON_BUFFER_0_NOTIFICATION                             0x00000000
#define NV_PVIDEO_INTR_REASON_BUFFER_0_PROTECTION_FAULT                         0x00000001
#define NV_PVIDEO_INTR_REASON_BUFFER_1                                          0x00000010
#define NV_PVIDEO_INTR_REASON_BUFFER_1__BITSHIFT                                4
#define NV_PVIDEO_INTR_REASON_BUFFER_1_NOTIFICATION                             0x00000000
#define NV_PVIDEO_INTR_REASON_BUFFER_1_PROTECTION_FAULT                         0x00000001
        NV_PVIDEO_INTR_EN                           = 0x00000140,
#define NV_PVIDEO_INTR_EN_BUFFER_0                                              0x00000001
#define NV_PVIDEO_INTR_EN_BUFFER_0__BITSHIFT                                    0
#define NV_PVIDEO_INTR_EN_BUFFER_0_DISABLED                                     0x00000000
#define NV_PVIDEO_INTR_EN_BUFFER_0_ENABLED                                      0x00000001
#define NV_PVIDEO_INTR_EN_BUFFER_1                                              0x00000010
#define NV_PVIDEO_INTR_EN_BUFFER_1__BITSHIFT                                    4
#define NV_PVIDEO_INTR_EN_BUFFER_1_DISABLED                                     0x00000000
#define NV_PVIDEO_INTR_EN_BUFFER_1_ENABLED                                      0x00000001
        NV_PVIDEO_BUFFER                            = 0x00000700,
#define NV_PVIDEO_BUFFER_0_USE                                                  0x00000001
#define NV_PVIDEO_BUFFER_0_USE__BITSHIFT                                        0
#define NV_PVIDEO_BUFFER_0_USE_NOT_PENDING                                      0x00000000
#define NV_PVIDEO_BUFFER_0_USE_PENDING                                          0x00000001
#define NV_PVIDEO_BUFFER_1_USE                                                  0x00000010
#define NV_PVIDEO_BUFFER_1_USE__BITSHIFT                                        4
#define NV_PVIDEO_BUFFER_1_USE_NOT_PENDING                                      0x00000000
#define NV_PVIDEO_BUFFER_1_USE_PENDING                                          0x00000001
        NV_PVIDEO_STOP                              = 0x00000704,
#define NV_PVIDEO_STOP_OVERLAY                                                  0x00000001
#define NV_PVIDEO_STOP_OVERLAY__BITSHIFT                                        0
#define NV_PVIDEO_STOP_OVERLAY_INACTIVE                                         0x00000000
#define NV_PVIDEO_STOP_OVERLAY_ACTIVE                                           0x00000001
#define NV_PVIDEO_STOP_METHOD                                                   0x00000010
#define NV_PVIDEO_STOP_METHOD__BITSHIFT                                         4
#define NV_PVIDEO_STOP_METHOD_IMMEDIATELY                                       0x00000000
#define NV_PVIDEO_STOP_METHOD_NORMALLY                                          0x00000001
        NV_PVIDEO_BASE_0                            = 0x00000900,
        NV_PVIDEO_BASE_1                            = 0x00000904,
#define NV_PVIDEO_BASE_VALUE                                                    0xffffffc0
#define NV_PVIDEO_BASE_VALUE__BITSHIFT                                          6
        NV_PVIDEO_LIMIT_0                           = 0x00000908,
        NV_PVIDEO_LIMIT_1                           = 0x0000090c,
#define NV_PVIDEO_LIMIT_VALUE                                                   0xffffffc0
#define NV_PVIDEO_LIMIT_VALUE__BITSHIFT                                         6
#define NV_PVIDEO_LIMIT_VALUE_MAX                                               0xffffffff
        NV_PVIDEO_LUMINANCE_0                       = 0x00000910,
        NV_PVIDEO_LUMINANCE_1                       = 0x00000914,
#define NV_PVIDEO_LUMINANCE_CONTRAST                                            0x00001ff8
#define NV_PVIDEO_LUMINANCE_CONTRAST__BITSHIFT                                  3
#define NV_PVIDEO_LUMINANCE_CONTRAST_47                                         0x00000200
#define NV_PVIDEO_LUMINANCE_CONTRAST_83                                         0x00000200
#define NV_PVIDEO_LUMINANCE_CONTRAST_UNITY                                      0x00000200
#define NV_PVIDEO_LUMINANCE_BRIGHTNESS                                          0x03ff0000
#define NV_PVIDEO_LUMINANCE_BRIGHTNESS__BITSHIFT                                16
#define NV_PVIDEO_LUMINANCE_BRIGHTNESS_47                                       0x00000000
#define NV_PVIDEO_LUMINANCE_BRIGHTNESS_83                                       0x00000000
#define NV_PVIDEO_LUMINANCE_BRIGHTNESS_UNITY                                    0x00000000
        NV_PVIDEO_CHROMINANCE_0                     = 0x00000918,
        NV_PVIDEO_CHROMINANCE_1                     = 0x0000091c,
#define NV_PVIDEO_CHROMINANCE_SAT_COS                                           0x00003ffc
#define NV_PVIDEO_CHROMINANCE_SAT_COS__BITSHIFT                                 2
#define NV_PVIDEO_CHROMINANCE_SAT_COS_47                                        0x00000400
#define NV_PVIDEO_CHROMINANCE_SAT_COS_83                                        0x00000400
#define NV_PVIDEO_CHROMINANCE_SAT_COS_UNITY                                     0x00000400
#define NV_PVIDEO_CHROMINANCE_SAT_SIN                                           0x3ffc0000
#define NV_PVIDEO_CHROMINANCE_SAT_SIN__BITSHIFT                                 18
#define NV_PVIDEO_CHROMINANCE_SAT_SIN_47                                        0x00000000
#define NV_PVIDEO_CHROMINANCE_SAT_SIN_83                                        0x00000000
#define NV_PVIDEO_CHROMINANCE_SAT_SIN_UNITY                                     0x00000000
        NV_PVIDEO_OFFSET_0                          = 0x00000920,
        NV_PVIDEO_OFFSET_1                          = 0x00000924,
#define NV_PVIDEO_OFFSET_VALUE                                                  0xffffffc0
#define NV_PVIDEO_OFFSET_VALUE__BITSHIFT                                        6
#define NV_PVIDEO_OFFSET_VALUE_47                                               0x00000000
#define NV_PVIDEO_OFFSET_VALUE_83                                               0x00000000
#define NV_PVIDEO_OFFSET_VALUE_ZERO                                             0x00000000
        NV_PVIDEO_SIZE_IN_0                         = 0x00000928,
        NV_PVIDEO_SIZE_IN_1                         = 0x0000092c,
#define NV_PVIDEO_SIZE_IN_WIDTH                                                 0x000007fe
#define NV_PVIDEO_SIZE_IN_WIDTH__BITSHIFT                                       1
#define NV_PVIDEO_SIZE_IN_WIDTH_47                                              0x00000001
#define NV_PVIDEO_SIZE_IN_WIDTH_83                                              0x00000001
#define NV_PVIDEO_SIZE_IN_HEIGHT                                                0x07ff0000
#define NV_PVIDEO_SIZE_IN_HEIGHT__BITSHIFT                                      16
#define NV_PVIDEO_SIZE_IN_HEIGHT_47                                             0x00000002
#define NV_PVIDEO_SIZE_IN_HEIGHT_83                                             0x00000002
        NV_PVIDEO_POINT_IN_0                        = 0x00000930,
        NV_PVIDEO_POINT_IN_1                        = 0x00000934,
#define NV_PVIDEO_POINT_IN_S                                                    0x00007fff
#define NV_PVIDEO_POINT_IN_S__BITSHIFT                                          0
#define NV_PVIDEO_POINT_IN_S_47                                                 0x00000000
#define NV_PVIDEO_POINT_IN_S_83                                                 0x00000000
#define NV_PVIDEO_POINT_IN_S_ORIGIN                                             0x00000000
#define NV_PVIDEO_POINT_IN_T                                                    0xfffe0000
#define NV_PVIDEO_POINT_IN_T__BITSHIFT                                          17
#define NV_PVIDEO_POINT_IN_T_47                                                 0x00000000
#define NV_PVIDEO_POINT_IN_T_83                                                 0x00000000
#define NV_PVIDEO_POINT_IN_T_ORIGIN                                             0x00000000
        NV_PVIDEO_DS_DX_0                           = 0x00000938,
        NV_PVIDEO_DS_DX_1                           = 0x0000093c,
#define NV_PVIDEO_DS_DX_RATIO                                                   0x00fffff8
#define NV_PVIDEO_DS_DX_RATIO__BITSHIFT                                         3
#define NV_PVIDEO_DS_DX_RATIO_47                                                0x00000000
#define NV_PVIDEO_DS_DX_RATIO_83                                                0x00000000
#define NV_PVIDEO_DS_DX_RATIO_UNITY                                             0x00020000
        NV_PVIDEO_DT_DY_0                           = 0x00000940,
        NV_PVIDEO_DT_DY_1                           = 0x00000944,
#define NV_PVIDEO_DT_DY_RATIO                                                   0x00fffff0
#define NV_PVIDEO_DT_DY_RATIO__BITSHIFT                                         4
#define NV_PVIDEO_DT_DY_RATIO_47                                                0x00000000
#define NV_PVIDEO_DT_DY_RATIO_83                                                0x00000000
#define NV_PVIDEO_DT_DY_RATIO_UNITY                                             0x00010000
        NV_PVIDEO_POINT_OUT_0                       = 0x00000948,
        NV_PVIDEO_POINT_OUT_1                       = 0x0000094c,
#define NV_PVIDEO_POINT_OUT_X                                                   0x00000fff
#define NV_PVIDEO_POINT_OUT_X__BITSHIFT                                         0
#define NV_PVIDEO_POINT_OUT_X_47                                                0x00000000
#define NV_PVIDEO_POINT_OUT_X_83                                                0x00000000
#define NV_PVIDEO_POINT_OUT_X_ORIGIN                                            0x00000000
#define NV_PVIDEO_POINT_OUT_Y                                                   0x0fff0000
#define NV_PVIDEO_POINT_OUT_Y__BITSHIFT                                         16
#define NV_PVIDEO_POINT_OUT_Y_47                                                0x00000000
#define NV_PVIDEO_POINT_OUT_Y_83                                                0x00000000
#define NV_PVIDEO_POINT_OUT_Y_ORIGIN                                            0x00000000
        NV_PVIDEO_SIZE_OUT_0                        = 0x00000950,
        NV_PVIDEO_SIZE_OUT_1                        = 0x00000954,
#define NV_PVIDEO_SIZE_OUT_WIDTH                                                0x00000fff
#define NV_PVIDEO_SIZE_OUT_WIDTH__BITSHIFT                                      0
#define NV_PVIDEO_SIZE_OUT_WIDTH_47                                             0x00000000
#define NV_PVIDEO_SIZE_OUT_WIDTH_83                                             0x00000000
#define NV_PVIDEO_SIZE_OUT_HEIGHT                                               0x0fff0000
#define NV_PVIDEO_SIZE_OUT_HEIGHT__BITSHIFT                                     16
#define NV_PVIDEO_SIZE_OUT_HEIGHT_47                                            0x00000000
#define NV_PVIDEO_SIZE_OUT_HEIGHT_83                                            0x00000000
        NV_PVIDEO_FORMAT_0                          = 0x00000958,
        NV_PVIDEO_FORMAT_1                          = 0x0000095c,
#define NV_PVIDEO_FORMAT_PITCH                                                  0x00001fc0
#define NV_PVIDEO_FORMAT_PITCH__BITSHIFT                                        6
#define NV_PVIDEO_FORMAT_PITCH_47                                               0x00000000
#define NV_PVIDEO_FORMAT_PITCH_83                                               0x00000000
#define NV_PVIDEO_FORMAT_COLOR                                                  0x00030000
#define NV_PVIDEO_FORMAT_COLOR__BITSHIFT                                        16
#define NV_PVIDEO_FORMAT_COLOR_47                                               0x00000000
#define NV_PVIDEO_FORMAT_COLOR_83                                               0x00000000
#define NV_PVIDEO_FORMAT_COLOR_LE_YB8CR8YA8CB8                                  0x00000000
#define NV_PVIDEO_FORMAT_COLOR_LE_CR8YB8CB8YA8                                  0x00000001
#define NV_PVIDEO_FORMAT_COLOR_LE_EYB8ECR8EYA8ECB8                              0x00000002
#define NV_PVIDEO_FORMAT_COLOR_LE_ECR8EYB8ECB8EYA8                              0x00000003
#define NV_PVIDEO_FORMAT_DISPLAY                                                0x00100000
#define NV_PVIDEO_FORMAT_DISPLAY__BITSHIFT                                      20
#define NV_PVIDEO_FORMAT_DISPLAY_47                                             0x00000000
#define NV_PVIDEO_FORMAT_DISPLAY_83                                             0x00000000
#define NV_PVIDEO_FORMAT_DISPLAY_ALWAYS                                         0x00000000
#define NV_PVIDEO_FORMAT_DISPLAY_COLOR_KEY_EQUAL                                0x00000001
#define NV_PVIDEO_FORMAT_MATRIX                                                 0x01000000
#define NV_PVIDEO_FORMAT_MATRIX__BITSHIFT                                       24
#define NV_PVIDEO_FORMAT_MATRIX_ITURBT601                                       0x00000000
#define NV_PVIDEO_FORMAT_MATRIX_ITURBT709                                       0x00000001
#define NV_PVIDEO_FORMAT_FIELD                                                  0x10000000
#define NV_PVIDEO_FORMAT_FIELD__BITSHIFT                                        28
#define NV_PVIDEO_FORMAT_FIELD_2A                                               0x00000000
#define NV_PVIDEO_FORMAT_FIELD_TOP                                              0x00000000
#define NV_PVIDEO_FORMAT_FIELD_BOTTOM                                           0x00000001
        NV_PVIDEO_COLOR_KEY                         = 0x00000b00,
#define NV_PVIDEO_COLOR_KEY_VALUE_47                                            0x00000000
#define NV_PVIDEO_COLOR_KEY_VALUE_83                                            0x00000000
#define NV_PVIDEO_COLOR_KEY_VALUE_DONT_CARE                                     0x00000000
        NV_PVIDEO_TEST                              = 0x00000d00,
#define NV_PVIDEO_TEST_MODE                                                     0x00000001
#define NV_PVIDEO_TEST_MODE__BITSHIFT                                           0
#define NV_PVIDEO_TEST_MODE_DISABLE                                             0x00000000
#define NV_PVIDEO_TEST_MODE_ENABLE                                              0x00000001
#define NV_PVIDEO_TEST_ADDRESS                                                  0x0000ff00
#define NV_PVIDEO_TEST_ADDRESS__BITSHIFT                                        8
        NV_PVIDEO_TST_WRITE_0                       = 0x00000d10,
        NV_PVIDEO_TST_WRITE_1                       = 0x00000d14,
        NV_PVIDEO_TST_WRITE_2                       = 0x00000d18,
        NV_PVIDEO_TST_WRITE_3                       = 0x00000d1c,
        NV_PVIDEO_TST_WRITE_4                       = 0x00000d20,
        NV_PVIDEO_TST_WRITE_5                       = 0x00000d24,
        NV_PVIDEO_TST_WRITE_6                       = 0x00000d28,
        NV_PVIDEO_TST_WRITE_7                       = 0x00000d2c,
        NV_PVIDEO_TST_WRITE_8                       = 0x00000d30,
        NV_PVIDEO_TST_WRITE_9                       = 0x00000d34,
        NV_PVIDEO_TST_WRITE_10                      = 0x00000d38,
        NV_PVIDEO_TST_WRITE_11                      = 0x00000d3c,
        NV_PVIDEO_TST_READ_0                        = 0x00000d40,
        NV_PVIDEO_TST_READ_1                        = 0x00000d44,
        NV_PVIDEO_TST_READ_2                        = 0x00000d48,
        NV_PVIDEO_TST_READ_3                        = 0x00000d4c,
        NV_PVIDEO_TST_READ_4                        = 0x00000d50,
        NV_PVIDEO_TST_READ_5                        = 0x00000d54,
        NV_PVIDEO_TST_READ_6                        = 0x00000d58,
        NV_PVIDEO_TST_READ_7                        = 0x00000d5c,
        NV_PVIDEO_TST_READ_8                        = 0x00000d60,
        NV_PVIDEO_TST_READ_9                        = 0x00000d64,
        NV_PVIDEO_TST_READ_10                       = 0x00000d68,
        NV_PVIDEO_TST_READ_11                       = 0x00000d6c,
        NV_PVIDEO_RCR                               = 0x00000d70,
#define NV_PVIDEO_RCR_RNDM_REQ                                                  0x000000ff
#define NV_PVIDEO_RCR_RNDM_REQ__BITSHIFT                                        0
#define NV_PVIDEO_RCR_RNDM_REQ_NONE                                             0x00000000
#define NV_PVIDEO_RCR_RNDM_REQ_4B                                               0x00000001
#define NV_PVIDEO_RCR_RNDM_REQ_8B                                               0x00000003
#define NV_PVIDEO_RCR_RNDM_REQ_16B                                              0x00000007
#define NV_PVIDEO_RCR_RNDM_REQ_32B                                              0x0000000f
#define NV_PVIDEO_RCR_RNDM_REQ_64B                                              0x0000001f
#define NV_PVIDEO_RCR_RNDM_REQ_128B                                             0x0000003f
#define NV_PVIDEO_RCR_RNDM_REQ_256B                                             0x0000007f
#define NV_PVIDEO_RCR_RNDM_REQ_512B                                             0x000000ff
#define NV_PVIDEO_RCR_MBI                                                       0x00010000
#define NV_PVIDEO_RCR_MBI__BITSHIFT                                             16
#define NV_PVIDEO_RCR_MBI_DISABLE                                               0x00000000
#define NV_PVIDEO_RCR_MBI_ENABLE                                                0x00000001
};

static const char *const nv2a_pvideo_name[] = {
    NAMER(NV_PVIDEO_DEBUG_0),
    NAMER(NV_PVIDEO_DEBUG_1),
    NAMER(NV_PVIDEO_DEBUG_2),
    NAMER(NV_PVIDEO_DEBUG_3),
    NAMER(NV_PVIDEO_DEBUG_4),
    NAMER(NV_PVIDEO_DEBUG_5),
    NAMER(NV_PVIDEO_DEBUG_6),
    NAMER(NV_PVIDEO_DEBUG_7),
    NAMER(NV_PVIDEO_DEBUG_8),
    NAMER(NV_PVIDEO_DEBUG_9),
    NAMER(NV_PVIDEO_DEBUG_10),
    NAMER(NV_PVIDEO_INTR),
    NAMER(NV_PVIDEO_INTR_REASON),
    NAMER(NV_PVIDEO_INTR_EN),
    NAMER(NV_PVIDEO_BUFFER),
    NAMER(NV_PVIDEO_STOP),
    NAMER(NV_PVIDEO_BASE_0),
    NAMER(NV_PVIDEO_BASE_1),
    NAMER(NV_PVIDEO_LIMIT_0),
    NAMER(NV_PVIDEO_LIMIT_1),
    NAMER(NV_PVIDEO_LUMINANCE_0),
    NAMER(NV_PVIDEO_LUMINANCE_1),
    NAMER(NV_PVIDEO_CHROMINANCE_0),
    NAMER(NV_PVIDEO_CHROMINANCE_1),
    NAMER(NV_PVIDEO_OFFSET_0),
    NAMER(NV_PVIDEO_OFFSET_1),
    NAMER(NV_PVIDEO_SIZE_IN_0),
    NAMER(NV_PVIDEO_SIZE_IN_1),
    NAMER(NV_PVIDEO_POINT_IN_0),
    NAMER(NV_PVIDEO_POINT_IN_1),
    NAMER(NV_PVIDEO_DS_DX_0),
    NAMER(NV_PVIDEO_DS_DX_1),
    NAMER(NV_PVIDEO_DT_DY_0),
    NAMER(NV_PVIDEO_DT_DY_1),
    NAMER(NV_PVIDEO_POINT_OUT_0),
    NAMER(NV_PVIDEO_POINT_OUT_1),
    NAMER(NV_PVIDEO_SIZE_OUT_0),
    NAMER(NV_PVIDEO_SIZE_OUT_1),
    NAMER(NV_PVIDEO_FORMAT_0),
    NAMER(NV_PVIDEO_FORMAT_1),
    NAMER(NV_PVIDEO_COLOR_KEY),
    NAMER(NV_PVIDEO_TEST),
    NAMER(NV_PVIDEO_TST_WRITE_0),
    NAMER(NV_PVIDEO_TST_WRITE_1),
    NAMER(NV_PVIDEO_TST_WRITE_2),
    NAMER(NV_PVIDEO_TST_WRITE_3),
    NAMER(NV_PVIDEO_TST_WRITE_4),
    NAMER(NV_PVIDEO_TST_WRITE_5),
    NAMER(NV_PVIDEO_TST_WRITE_6),
    NAMER(NV_PVIDEO_TST_WRITE_7),
    NAMER(NV_PVIDEO_TST_WRITE_8),
    NAMER(NV_PVIDEO_TST_WRITE_9),
    NAMER(NV_PVIDEO_TST_WRITE_10),
    NAMER(NV_PVIDEO_TST_WRITE_11),
    NAMER(NV_PVIDEO_TST_READ_0),
    NAMER(NV_PVIDEO_TST_READ_1),
    NAMER(NV_PVIDEO_TST_READ_2),
    NAMER(NV_PVIDEO_TST_READ_3),
    NAMER(NV_PVIDEO_TST_READ_4),
    NAMER(NV_PVIDEO_TST_READ_5),
    NAMER(NV_PVIDEO_TST_READ_6),
    NAMER(NV_PVIDEO_TST_READ_7),
    NAMER(NV_PVIDEO_TST_READ_8),
    NAMER(NV_PVIDEO_TST_READ_9),
    NAMER(NV_PVIDEO_TST_READ_10),
    NAMER(NV_PVIDEO_TST_READ_11),
    NAMER(NV_PVIDEO_RCR),
};

