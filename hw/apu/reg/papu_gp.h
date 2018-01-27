
enum {
        NV_PAPU_GPRST                               = 0x0000fffc,
#define NV_PAPU_GPRST_GPRST                                                     0x00000001
#define NV_PAPU_GPRST_GPRST__BITSHIFT                                           0
#define NV_PAPU_GPRST_GPRST_ENABLED                                             0
#define NV_PAPU_GPRST_GPRST_DISABLED                                            1
#define NV_PAPU_GPRST_GPDSPRST                                                  0x00000002
#define NV_PAPU_GPRST_GPDSPRST__BITSHIFT                                        1
#define NV_PAPU_GPRST_GPDSPRST_ENABLED                                          0
#define NV_PAPU_GPRST_GPDSPRST_DISABLED                                         1
#define NV_PAPU_GPRST_GPNMI                                                     0x00000004
#define NV_PAPU_GPRST_GPNMI__BITSHIFT                                           2
#define NV_PAPU_GPRST_GPNMI_DISABLED                                            0
#define NV_PAPU_GPRST_GPNMI_ENABLED                                             1
#define NV_PAPU_GPRST_GPABORT                                                   0x00000008
#define NV_PAPU_GPRST_GPABORT__BITSHIFT                                         3
#define NV_PAPU_GPRST_GPABORT_DISABLED                                          0
#define NV_PAPU_GPRST_GPABORT_ENABLED                                           1
        NV_PAPU_GPCTLG                              = 0x0000ff28,
#define NV_PAPU_GPCTLG_GPSTYP                                                   0x00000001
#define NV_PAPU_GPCTLG_GPSTYP__BITSHIFT                                         0
#define NV_PAPU_GPCTLG_GPSTYP_NON_ISO                                           0
#define NV_PAPU_GPCTLG_GPSTYP_ISO                                               1
#define NV_PAPU_GPCTLG_SPARE                                                    0x00fffffe
#define NV_PAPU_GPCTLG_SPARE__BITSHIFT                                          1
#define NV_PAPU_GPCTLG_SPARE_INIT                                               0
        NV_PAPU_GPCTLOUT                            = 0x0000ff20,
#define NV_PAPU_GPCTLOUT_GD0FMT                                                 0x00000003
#define NV_PAPU_GPCTLOUT_GD0FMT__BITSHIFT                                       0
#define NV_PAPU_GPCTLOUT_GD0FMT_MONO                                            0
#define NV_PAPU_GPCTLOUT_GD0FMT_STEREO                                          1
#define NV_PAPU_GPCTLOUT_GD0FMT_QUAD                                            2
#define NV_PAPU_GPCTLOUT_GD0FMT_HEX                                             3
#define NV_PAPU_GPCTLOUT_GP0TYP                                                 0x00000004
#define NV_PAPU_GPCTLOUT_GP0TYP__BITSHIFT                                       2
#define NV_PAPU_GPCTLOUT_GP0TYP_NON_ISO                                         0
#define NV_PAPU_GPCTLOUT_GP0TYP_ISO                                             1
#define NV_PAPU_GPCTLOUT_GD0SIZ                                                 0x00000018
#define NV_PAPU_GPCTLOUT_GD0SIZ__BITSHIFT                                       3
#define NV_PAPU_GPCTLOUT_GD0SIZ_8                                               0
#define NV_PAPU_GPCTLOUT_GD0SIZ_16                                              1
#define NV_PAPU_GPCTLOUT_GD0SIZ_32                                              2
#define NV_PAPU_GPCTLOUT_GD1FMT                                                 0x00000060
#define NV_PAPU_GPCTLOUT_GD1FMT__BITSHIFT                                       5
#define NV_PAPU_GPCTLOUT_GD1FMT_MONO                                            0
#define NV_PAPU_GPCTLOUT_GD1FMT_STEREO                                          1
#define NV_PAPU_GPCTLOUT_GD1FMT_QUAD                                            2
#define NV_PAPU_GPCTLOUT_GD1FMT_HEX                                             3
#define NV_PAPU_GPCTLOUT_GP1TYP                                                 0x00000080
#define NV_PAPU_GPCTLOUT_GP1TYP__BITSHIFT                                       7
#define NV_PAPU_GPCTLOUT_GP1TYP_NON_ISO                                         0
#define NV_PAPU_GPCTLOUT_GP1TYP_ISO                                             1
#define NV_PAPU_GPCTLOUT_GD1SIZ                                                 0x00000300
#define NV_PAPU_GPCTLOUT_GD1SIZ__BITSHIFT                                       8
#define NV_PAPU_GPCTLOUT_GD1SIZ_8                                               0
#define NV_PAPU_GPCTLOUT_GD1SIZ_16                                              1
#define NV_PAPU_GPCTLOUT_GD1SIZ_32                                              2
#define NV_PAPU_GPCTLOUT_GD2FMT                                                 0x00000c00
#define NV_PAPU_GPCTLOUT_GD2FMT__BITSHIFT                                       10
#define NV_PAPU_GPCTLOUT_GD2FMT_MONO                                            0
#define NV_PAPU_GPCTLOUT_GD2FMT_STEREO                                          1
#define NV_PAPU_GPCTLOUT_GD2FMT_QUAD                                            2
#define NV_PAPU_GPCTLOUT_GD2FMT_HEX                                             3
#define NV_PAPU_GPCTLOUT_GP2TYP                                                 0x00001000
#define NV_PAPU_GPCTLOUT_GP2TYP__BITSHIFT                                       12
#define NV_PAPU_GPCTLOUT_GP2TYP_NON_ISO                                         0
#define NV_PAPU_GPCTLOUT_GP2TYP_ISO                                             1
#define NV_PAPU_GPCTLOUT_GD2SIZ                                                 0x00006000
#define NV_PAPU_GPCTLOUT_GD2SIZ__BITSHIFT                                       13
#define NV_PAPU_GPCTLOUT_GD2SIZ_8                                               0
#define NV_PAPU_GPCTLOUT_GD2SIZ_16                                              1
#define NV_PAPU_GPCTLOUT_GD2SIZ_32                                              2
#define NV_PAPU_GPCTLOUT_GD3FMT                                                 0x00018000
#define NV_PAPU_GPCTLOUT_GD3FMT__BITSHIFT                                       15
#define NV_PAPU_GPCTLOUT_GD3FMT_MONO                                            0
#define NV_PAPU_GPCTLOUT_GD3FMT_STEREO                                          1
#define NV_PAPU_GPCTLOUT_GD3FMT_QUAD                                            2
#define NV_PAPU_GPCTLOUT_GD3FMT_HEX                                             3
#define NV_PAPU_GPCTLOUT_GP3TYP                                                 0x00020000
#define NV_PAPU_GPCTLOUT_GP3TYP__BITSHIFT                                       17
#define NV_PAPU_GPCTLOUT_GP3TYP_NON_ISO                                         0
#define NV_PAPU_GPCTLOUT_GP3TYP_ISO                                             1
#define NV_PAPU_GPCTLOUT_GD3SIZ                                                 0x000c0000
#define NV_PAPU_GPCTLOUT_GD3SIZ__BITSHIFT                                       18
#define NV_PAPU_GPCTLOUT_GD3SIZ_8                                               0
#define NV_PAPU_GPCTLOUT_GD3SIZ_16                                              1
#define NV_PAPU_GPCTLOUT_GD3SIZ_32                                              2
#define NV_PAPU_GPCTLOUT_SPARE                                                  0x00f00000
#define NV_PAPU_GPCTLOUT_SPARE__BITSHIFT                                        20
#define NV_PAPU_GPCTLOUT_SPARE_INIT                                             0
        NV_PAPU_GPCTLIN                             = 0x0000ff24,
#define NV_PAPU_GPCTLIN_GDI0FMT                                                 0x00000003
#define NV_PAPU_GPCTLIN_GDI0FMT__BITSHIFT                                       0
#define NV_PAPU_GPCTLIN_GDI0FMT_MONO                                            0
#define NV_PAPU_GPCTLIN_GDI0FMT_STEREO                                          1
#define NV_PAPU_GPCTLIN_GDI0FMT_QUAD                                            2
#define NV_PAPU_GPCTLIN_GDI0FMT_HEX                                             3
#define NV_PAPU_GPCTLIN_GPI0TYP                                                 0x00000004
#define NV_PAPU_GPCTLIN_GPI0TYP__BITSHIFT                                       2
#define NV_PAPU_GPCTLIN_GPI0TYP_NON_ISO                                         0
#define NV_PAPU_GPCTLIN_GPI0TYP_ISO                                             1
#define NV_PAPU_GPCTLIN_GDI0SIZ                                                 0x00000018
#define NV_PAPU_GPCTLIN_GDI0SIZ__BITSHIFT                                       3
#define NV_PAPU_GPCTLIN_GDI0SIZ_8                                               0
#define NV_PAPU_GPCTLIN_GDI0SIZ_16                                              1
#define NV_PAPU_GPCTLIN_GDI0SIZ_32                                              2
#define NV_PAPU_GPCTLIN_GDI1FMT                                                 0x00000060
#define NV_PAPU_GPCTLIN_GDI1FMT__BITSHIFT                                       5
#define NV_PAPU_GPCTLIN_GDI1FMT_MONO                                            0
#define NV_PAPU_GPCTLIN_GDI1FMT_STEREO                                          1
#define NV_PAPU_GPCTLIN_GDI1FMT_QUAD                                            2
#define NV_PAPU_GPCTLIN_GDI1FMT_HEX                                             3
#define NV_PAPU_GPCTLIN_GPI1TYP                                                 0x00000080
#define NV_PAPU_GPCTLIN_GPI1TYP__BITSHIFT                                       7
#define NV_PAPU_GPCTLIN_GPI1TYP_NON_ISO                                         0
#define NV_PAPU_GPCTLIN_GPI1TYP_ISO                                             1
#define NV_PAPU_GPCTLIN_GDI1SIZ                                                 0x00000300
#define NV_PAPU_GPCTLIN_GDI1SIZ__BITSHIFT                                       8
#define NV_PAPU_GPCTLIN_GDI1SIZ_8                                               0
#define NV_PAPU_GPCTLIN_GDI1SIZ_16                                              1
#define NV_PAPU_GPCTLIN_GDI1SIZ_32                                              2
#define NV_PAPU_GPCTLIN_SPARE                                                   0x00fffc00
#define NV_PAPU_GPCTLIN_SPARE__BITSHIFT                                         10
#define NV_PAPU_GPCTLIN_SPARE_INIT                                              0
        NV_PAPU_GPMBO                               = 0x0000ff08,
#define NV_PAPU_GPMBO_VALUE                                                     0x00ffffff
#define NV_PAPU_GPMBO_VALUE__BITSHIFT                                           0
        NV_PAPU_GPMBI                               = 0x0000ff0c,
#define NV_PAPU_GPMBI_VALUE                                                     0x00ffffff
#define NV_PAPU_GPMBI_VALUE__BITSHIFT                                           0
        NV_PAPU_GPGET                               = 0x0000ff00,
#define NV_PAPU_GPGET_VALUE                                                     0x00000ffc
#define NV_PAPU_GPGET_VALUE__BITSHIFT                                           2
        NV_PAPU_GPPUT                               = 0x0000ff04,
#define NV_PAPU_GPPUT_VALUE                                                     0x00000ffc
#define NV_PAPU_GPPUT_VALUE__BITSHIFT                                           2
        NV_PAPU_GPIDRDY                             = 0x0000ff10,
#define NV_PAPU_GPIDRDY_GPSETIDLE                                               0x00000001
#define NV_PAPU_GPIDRDY_GPSETIDLE__BITSHIFT                                     0
#define NV_PAPU_GPIDRDY_GPSETIDLE_NOT_SET                                       0
#define NV_PAPU_GPIDRDY_GPSETIDLE_SET                                           1
#define NV_PAPU_GPIDRDY_GPSTOPPED                                               0x00000002
#define NV_PAPU_GPIDRDY_GPSTOPPED__BITSHIFT                                     1
#define NV_PAPU_GPIDRDY_GPSTOPPED_NOT_SET                                       0
#define NV_PAPU_GPIDRDY_GPSTOPPED_SET                                           1
#define NV_PAPU_GPIDRDY_GPSTOP_ENABLE                                           0x00000004
#define NV_PAPU_GPIDRDY_GPSTOP_ENABLE__BITSHIFT                                 2
#define NV_PAPU_GPIDRDY_GPSTOP_NOT_ENABLED                                      0
#define NV_PAPU_GPIDRDY_GPSTOP_ENABLED                                          1
#define NV_PAPU_GPIDRDY_GPSETNINT                                               0x00000008
#define NV_PAPU_GPIDRDY_GPSETNINT__BITSHIFT                                     3
#define NV_PAPU_GPIDRDY_GPSETNINT_NOT_SET                                       0
#define NV_PAPU_GPIDRDY_GPSETNINT_SET                                           1
        NV_PAPU_GPT0CFG                             = 0x0000fec0,
#define NV_PAPU_GPT0CFG_GPT0RUN                                                 0x00000001
#define NV_PAPU_GPT0CFG_GPT0RUN__BITSHIFT                                       0
#define NV_PAPU_GPT0CFG_GPT0RUN_IDLE                                            0
#define NV_PAPU_GPT0CFG_GPT0RUN_RUN                                             1
#define NV_PAPU_GPT0CFG_GPT0DSTP                                                0x00000002
#define NV_PAPU_GPT0CFG_GPT0DSTP__BITSHIFT                                      1
#define NV_PAPU_GPT0CFG_GPT0DSTP_RUN                                            0
#define NV_PAPU_GPT0CFG_GPT0DSTP_HOLD                                           1
        NV_PAPU_GPT1CFG                             = 0x0000fed0,
#define NV_PAPU_GPT1CFG_GPT1RUN                                                 0x00000001
#define NV_PAPU_GPT1CFG_GPT1RUN__BITSHIFT                                       0
#define NV_PAPU_GPT1CFG_GPT1RUN_IDLE                                            0
#define NV_PAPU_GPT1CFG_GPT1RUN_RUN                                             1
#define NV_PAPU_GPT1CFG_GPT1DSTP                                                0x00000002
#define NV_PAPU_GPT1CFG_GPT1DSTP__BITSHIFT                                      1
#define NV_PAPU_GPT1CFG_GPT1DSTP_RUN                                            0
#define NV_PAPU_GPT1CFG_GPT1DSTP_HOLD                                           1
        NV_PAPU_GPT0CTL                             = 0x0000fec4,
#define NV_PAPU_GPT0CTL_GPT0ACT                                                 0x00000003
#define NV_PAPU_GPT0CTL_GPT0ACT__BITSHIFT                                       0
#define NV_PAPU_GPT0CTL_GPT0ACT_NOP                                             0
#define NV_PAPU_GPT0CTL_GPT0ACT_STZERO                                          1
#define NV_PAPU_GPT0CTL_GPT0ACT_STCUR                                           2
#define NV_PAPU_GPT0CTL_GPT0ACT_STOP                                            3
#define NV_PAPU_GPT0CTL_GPT0ST                                                  0x00000004
#define NV_PAPU_GPT0CTL_GPT0ST__BITSHIFT                                        2
#define NV_PAPU_GPT0CTL_GPT0ST_STOP                                             0
#define NV_PAPU_GPT0CTL_GPT0ST_RUN                                              1
        NV_PAPU_GPT1CTL                             = 0x0000fed4,
#define NV_PAPU_GPT1CTL_GPT1ACT                                                 0x00000003
#define NV_PAPU_GPT1CTL_GPT1ACT__BITSHIFT                                       0
#define NV_PAPU_GPT1CTL_GPT1ACT_NOP                                             0
#define NV_PAPU_GPT1CTL_GPT1ACT_STZERO                                          1
#define NV_PAPU_GPT1CTL_GPT1ACT_STCUR                                           2
#define NV_PAPU_GPT1CTL_GPT1ACT_STOP                                            3
#define NV_PAPU_GPT1CTL_GPT1ST                                                  0x00000004
#define NV_PAPU_GPT1CTL_GPT1ST__BITSHIFT                                        2
#define NV_PAPU_GPT1CTL_GPT1ST_STOP                                             0
#define NV_PAPU_GPT1CTL_GPT1ST_RUN                                              1
        NV_PAPU_GPT0TCNT                            = 0x0000fec8,
#define NV_PAPU_GPT0TCNT_VALUE                                                  0x00ffffff
#define NV_PAPU_GPT0TCNT_VALUE__BITSHIFT                                        0
        NV_PAPU_GPT1TCNT                            = 0x0000fed8,
#define NV_PAPU_GPT1TCNT_VALUE                                                  0x00ffffff
#define NV_PAPU_GPT1TCNT_VALUE__BITSHIFT                                        0
        NV_PAPU_GPT0CCNT                            = 0x0000fecc,
#define NV_PAPU_GPT0CCNT_VALUE                                                  0x00ffffff
#define NV_PAPU_GPT0CCNT_VALUE__BITSHIFT                                        0
        NV_PAPU_GPT1CCNT                            = 0x0000fedc,
#define NV_PAPU_GPT1CCNT_VALUE                                                  0x00ffffff
#define NV_PAPU_GPT1CCNT_VALUE__BITSHIFT                                        0
        NV_PAPU_GPISTS                              = 0x0000ff14,
#define NV_PAPU_GPISTS_GPSTRTSTS                                                0x00000001
#define NV_PAPU_GPISTS_GPSTRTSTS__BITSHIFT                                      0
#define NV_PAPU_GPISTS_GPSTRTSTS_NO_INTR                                        0
#define NV_PAPU_GPISTS_GPSTRTSTS_INTR                                           1
#define NV_PAPU_GPISTS_GPSTRTSTS_CLR                                            1
#define NV_PAPU_GPISTS_GPABRTSTS                                                0x00000002
#define NV_PAPU_GPISTS_GPABRTSTS__BITSHIFT                                      1
#define NV_PAPU_GPISTS_GPABRTSTS_NO_INTR                                        0
#define NV_PAPU_GPISTS_GPABRTSTS_INTR                                           1
#define NV_PAPU_GPISTS_GPABRTSTS_CLR                                            1
#define NV_PAPU_GPISTS_GPPUTSTS                                                 0x00000004
#define NV_PAPU_GPISTS_GPPUTSTS__BITSHIFT                                       2
#define NV_PAPU_GPISTS_GPPUTSTS_NO_INTR                                         0
#define NV_PAPU_GPISTS_GPPUTSTS_INTR                                            1
#define NV_PAPU_GPISTS_GPPUTSTS_CLR                                             1
#define NV_PAPU_GPISTS_GPMBISTS                                                 0x00000008
#define NV_PAPU_GPISTS_GPMBISTS__BITSHIFT                                       3
#define NV_PAPU_GPISTS_GPMBISTS_NO_INTR                                         0
#define NV_PAPU_GPISTS_GPMBISTS_INTR                                            1
#define NV_PAPU_GPISTS_GPMBISTS_CLR                                             1
#define NV_PAPU_GPISTS_GPD1STS                                                  0x00000010
#define NV_PAPU_GPISTS_GPD1STS__BITSHIFT                                        4
#define NV_PAPU_GPISTS_GPD1STS_NO_INTR                                          0
#define NV_PAPU_GPISTS_GPD1STS_INTR                                             1
#define NV_PAPU_GPISTS_GPD1STS_CLR                                              1
#define NV_PAPU_GPISTS_GPD2STS                                                  0x00000020
#define NV_PAPU_GPISTS_GPD2STS__BITSHIFT                                        5
#define NV_PAPU_GPISTS_GPD2STS_NO_INTR                                          0
#define NV_PAPU_GPISTS_GPD2STS_INTR                                             1
#define NV_PAPU_GPISTS_GPD2STS_CLR                                              1
#define NV_PAPU_GPISTS_GPD3STS                                                  0x00000040
#define NV_PAPU_GPISTS_GPD3STS__BITSHIFT                                        6
#define NV_PAPU_GPISTS_GPD3STS_NO_INTR                                          0
#define NV_PAPU_GPISTS_GPD3STS_INTR                                             1
#define NV_PAPU_GPISTS_GPD3STS_CLR                                              1
#define NV_PAPU_GPISTS_GPDEOLSTS                                                0x00000080
#define NV_PAPU_GPISTS_GPDEOLSTS__BITSHIFT                                      7
#define NV_PAPU_GPISTS_GPDEOLSTS_NO_INTR                                        0
#define NV_PAPU_GPISTS_GPDEOLSTS_INTR                                           1
#define NV_PAPU_GPISTS_GPDEOLSTS_CLR                                            1
#define NV_PAPU_GPISTS_GPT0STS                                                  0x00000100
#define NV_PAPU_GPISTS_GPT0STS__BITSHIFT                                        8
#define NV_PAPU_GPISTS_GPT0STS_NO_INTR                                          0
#define NV_PAPU_GPISTS_GPT0STS_INTR                                             1
#define NV_PAPU_GPISTS_GPT0STS_CLR                                              1
#define NV_PAPU_GPISTS_GPT1STS                                                  0x00000200
#define NV_PAPU_GPISTS_GPT1STS__BITSHIFT                                        9
#define NV_PAPU_GPISTS_GPT1STS_NO_INTR                                          0
#define NV_PAPU_GPISTS_GPT1STS_INTR                                             1
#define NV_PAPU_GPISTS_GPT1STS_CLR                                              1
#define NV_PAPU_GPISTS_GPDERRSTS                                                0x00000400
#define NV_PAPU_GPISTS_GPDERRSTS__BITSHIFT                                      10
#define NV_PAPU_GPISTS_GPDERRSTS_NO_INTR                                        0
#define NV_PAPU_GPISTS_GPDERRSTS_INTR                                           1
#define NV_PAPU_GPISTS_GPDERRSTS_CLR                                            1
        NV_PAPU_GPHDRV_0                            = 0x0000fe40,
        NV_PAPU_GPHDRV_1                            = 0x0000fe44,
        NV_PAPU_GPHDRV_2                            = 0x0000fe48,
        NV_PAPU_GPHDRV_3                            = 0x0000fe4c,
        NV_PAPU_GPHDRV_4                            = 0x0000fe50,
        NV_PAPU_GPHDRV_5                            = 0x0000fe54,
        NV_PAPU_GPHDRV_6                            = 0x0000fe58,
        NV_PAPU_GPHDRV_7                            = 0x0000fe5c,
        NV_PAPU_GPHDRV_8                            = 0x0000fe60,
        NV_PAPU_GPHDRV_9                            = 0x0000fe64,
        NV_PAPU_GPHDRV_10                           = 0x0000fe68,
        NV_PAPU_GPHDRV_11                           = 0x0000fe6c,
        NV_PAPU_GPHDRV_12                           = 0x0000fe70,
        NV_PAPU_GPHDRV_13                           = 0x0000fe74,
        NV_PAPU_GPHDRV_14                           = 0x0000fe78,
        NV_PAPU_GPHDRV_15                           = 0x0000fe7c,
        NV_PAPU_GPHDRV_16                           = 0x0000fe80,
        NV_PAPU_GPHDRV_17                           = 0x0000fe84,
        NV_PAPU_GPHDRV_18                           = 0x0000fe88,
        NV_PAPU_GPHDRV_19                           = 0x0000fe8c,
        NV_PAPU_GPHDRV_20                           = 0x0000fe90,
        NV_PAPU_GPHDRV_21                           = 0x0000fe94,
        NV_PAPU_GPHDRV_22                           = 0x0000fe98,
        NV_PAPU_GPHDRV_23                           = 0x0000fe9c,
        NV_PAPU_GPHDRV_24                           = 0x0000fea0,
        NV_PAPU_GPHDRV_25                           = 0x0000fea4,
        NV_PAPU_GPHDRV_26                           = 0x0000fea8,
        NV_PAPU_GPHDRV_27                           = 0x0000feac,
        NV_PAPU_GPHDRV_28                           = 0x0000feb0,
        NV_PAPU_GPHDRV_29                           = 0x0000feb4,
        NV_PAPU_GPHDRV_30                           = 0x0000feb8,
        NV_PAPU_GPHDRV_31                           = 0x0000febc,
#define NV_PAPU_GPHDRV_MHR                                                      0x00000007
#define NV_PAPU_GPHDRV_MHR__BITSHIFT                                            0
#define NV_PAPU_GPHDRV_VLD                                                      0x00000008
#define NV_PAPU_GPHDRV_VLD__BITSHIFT                                            3
#define NV_PAPU_GPHDRV_VLD_INVALID                                              0
#define NV_PAPU_GPHDRV_VLD_VALID                                                1
        NV_PAPU_GPGSCNT                             = 0x0000ff18,
#define NV_PAPU_GPGSCNT_GSCNT                                                   0x00ffffff
#define NV_PAPU_GPGSCNT_GSCNT__BITSHIFT                                         0
        NV_PAPU_GPDCFG                              = 0x0000ff5c,
#define NV_PAPU_GPDCFG_GPDASTRT                                                 0x00000001
#define NV_PAPU_GPDCFG_GPDASTRT__BITSHIFT                                       0
#define NV_PAPU_GPDCFG_GPDASTRT_DISABLED                                        0
#define NV_PAPU_GPDCFG_GPDASTRT_ENABLED                                         1
#define NV_PAPU_GPDCFG_GPDARDY                                                  0x00000002
#define NV_PAPU_GPDCFG_GPDARDY__BITSHIFT                                        1
#define NV_PAPU_GPDCFG_GPDARDY_DISABLED                                         0
#define NV_PAPU_GPDCFG_GPDARDY_ENABLED                                          1
#define NV_PAPU_GPDCFG_GPDIOCR                                                  0x00000004
#define NV_PAPU_GPDCFG_GPDIOCR__BITSHIFT                                        2
#define NV_PAPU_GPDCFG_GPDIOCR_DISABLED                                         0
#define NV_PAPU_GPDCFG_GPDIOCR_ENABLED                                          1
#define NV_PAPU_GPDCFG_GPDEOLR                                                  0x00000008
#define NV_PAPU_GPDCFG_GPDEOLR__BITSHIFT                                        3
#define NV_PAPU_GPDCFG_GPDEOLR_DISABLED                                         0
#define NV_PAPU_GPDCFG_GPDEOLR_ENABLED                                          1
#define NV_PAPU_GPDCFG_GPDERRR                                                  0x00000010
#define NV_PAPU_GPDCFG_GPDERRR__BITSHIFT                                        4
#define NV_PAPU_GPDCFG_GPDERRR_DISABLED                                         0
#define NV_PAPU_GPDCFG_GPDERRR_ENABLED                                          1
        NV_PAPU_GPDCTL                              = 0x0000ff58,
#define NV_PAPU_GPDCTL_GPDACT                                                   0x00000007
#define NV_PAPU_GPDCTL_GPDACT__BITSHIFT                                         0
#define NV_PAPU_GPDCTL_GPDACT_NOP                                               0
#define NV_PAPU_GPDCTL_GPDACT_START                                             1
#define NV_PAPU_GPDCTL_GPDACT_STOP                                              2
#define NV_PAPU_GPDCTL_GPDACT_FREEZE                                            3
#define NV_PAPU_GPDCTL_GPDACT_UNFREEZE                                          4
#define NV_PAPU_GPDCTL_GPDFRZ                                                   0x00000008
#define NV_PAPU_GPDCTL_GPDFRZ__BITSHIFT                                         3
#define NV_PAPU_GPDCTL_GPDFRZ_NOT_FROZE                                         0
#define NV_PAPU_GPDCTL_GPDFRZ_FROZE                                             1
#define NV_PAPU_GPDCTL_GPDRUN                                                   0x00000010
#define NV_PAPU_GPDCTL_GPDRUN__BITSHIFT                                         4
#define NV_PAPU_GPDCTL_GPDRUN_NOT_RUN                                           0
#define NV_PAPU_GPDCTL_GPDRUN_RUNNING                                           1
#define NV_PAPU_GPDCTL_GPDSTP                                                   0x00000020
#define NV_PAPU_GPDCTL_GPDSTP__BITSHIFT                                         5
#define NV_PAPU_GPDCTL_GPDSTP_NOT_STOP                                          0
#define NV_PAPU_GPDCTL_GPDFRZ_STOPPED                                           1
        NV_PAPU_GPDSPT                              = 0x0000ff54,
#define NV_PAPU_GPDSPT_GPDSBPT                                                  0x00003fff
#define NV_PAPU_GPDSPT_GPDSBPT__BITSHIFT                                        0
#define NV_PAPU_GPDSPT_GPDSBPT_INIT                                             0
#define NV_PAPU_GPDSPT_GPDEOL                                                   0x00004000
#define NV_PAPU_GPDSPT_GPDEOL__BITSHIFT                                         14
#define NV_PAPU_GPDSPT_GPDEOL_EOL                                               1
#define NV_PAPU_GPDSPT_GPDEOL_NOT_EOL                                           0
        NV_PAPU_GPNSPT                              = 0x0000ff50,
#define NV_PAPU_GPNSPT_GPNSBPT                                                  0x00003fff
#define NV_PAPU_GPNSPT_GPNSBPT__BITSHIFT                                        0
#define NV_PAPU_GPNSPT_GPNSBPT_INIT                                             0
#define NV_PAPU_GPNSPT_GPNEOL                                                   0x00004000
#define NV_PAPU_GPNSPT_GPNEOL__BITSHIFT                                         14
#define NV_PAPU_GPNSPT_GPNEOL_NOT_EOL                                           0
#define NV_PAPU_GPNSPT_GPNEOL_EOL                                               1
        NV_PAPU_GPDBASE_0                           = 0x0000ff30,
        NV_PAPU_GPDBASE_1                           = 0x0000ff38,
        NV_PAPU_GPDBASE_2                           = 0x0000ff40,
        NV_PAPU_GPDBASE_3                           = 0x0000ff48,
#define NV_PAPU_GPDBASE_BASE                                                    0x00ffffff
#define NV_PAPU_GPDBASE_BASE__BITSHIFT                                          0
        NV_PAPU_GPDSIZE_0                           = 0x0000ff34,
        NV_PAPU_GPDSIZE_1                           = 0x0000ff3c,
        NV_PAPU_GPDSIZE_2                           = 0x0000ff44,
        NV_PAPU_GPDSIZE_3                           = 0x0000ff4c,
#define NV_PAPU_GPDSIZE_SIZE                                                    0x00ffffff
#define NV_PAPU_GPDSIZE_SIZE__BITSHIFT                                          0
        NV_PAPU_GPDDEBUG                            = 0x0000ff2c,
#define NV_PAPU_GPDDEBUG_PBADBLK                                                0x00003fff
#define NV_PAPU_GPDDEBUG_PBADBLK__BITSHIFT                                      0
#define NV_PAPU_GPDDEBUG_ILLSMBUFID                                             0x00004000
#define NV_PAPU_GPDDEBUG_ILLSMBUFID__BITSHIFT                                   14
#define NV_PAPU_GPDDEBUG_ILLSMBUFID_NOT_ILL                                     0
#define NV_PAPU_GPDDEBUG_ILLSMBUFID_ILL                                         1
#define NV_PAPU_GPDDEBUG_ILLSMDATAFMT                                           0x00008000
#define NV_PAPU_GPDDEBUG_ILLSMDATAFMT__BITSHIFT                                 15
#define NV_PAPU_GPDDEBUG_ILLSMDATAFMT_NOT_ILL                                   0
#define NV_PAPU_GPDDEBUG_ILLSMDATAFMT_ILL                                       1
#define NV_PAPU_GPDDEBUG_ILLCOUNT                                               0x00010000
#define NV_PAPU_GPDDEBUG_ILLCOUNT__BITSHIFT                                     16
#define NV_PAPU_GPDDEBUG_ILLCOUNT_NOT_ILL                                       0
#define NV_PAPU_GPDDEBUG_ILLCOUNT_ILL                                           1
#define NV_PAPU_GPDDEBUG_ILLDSPADDR                                             0x00020000
#define NV_PAPU_GPDDEBUG_ILLDSPADDR__BITSHIFT                                   17
#define NV_PAPU_GPDDEBUG_ILLDSPADDR_NOT_ILL                                     0
#define NV_PAPU_GPDDEBUG_ILLDSPADDR_ILL                                         1
};

static const char *const apu_papu_gp_name[] = {
    NAMER(NV_PAPU_GPRST),
#if 0
NV_PAPU_GPRST_GPRST                                                     0x00000001
NV_PAPU_GPRST_GPRST__BITSHIFT                                           0
NV_PAPU_GPRST_GPRST_ENABLED                                             0
NV_PAPU_GPRST_GPRST_DISABLED                                            1
NV_PAPU_GPRST_GPDSPRST                                                  0x00000002
NV_PAPU_GPRST_GPDSPRST__BITSHIFT                                        1
NV_PAPU_GPRST_GPDSPRST_ENABLED                                          0
NV_PAPU_GPRST_GPDSPRST_DISABLED                                         1
NV_PAPU_GPRST_GPNMI                                                     0x00000004
NV_PAPU_GPRST_GPNMI__BITSHIFT                                           2
NV_PAPU_GPRST_GPNMI_DISABLED                                            0
NV_PAPU_GPRST_GPNMI_ENABLED                                             1
NV_PAPU_GPRST_GPABORT                                                   0x00000008
NV_PAPU_GPRST_GPABORT__BITSHIFT                                         3
NV_PAPU_GPRST_GPABORT_DISABLED                                          0
NV_PAPU_GPRST_GPABORT_ENABLED                                           1
#endif
    NAMER(NV_PAPU_GPCTLG),
#if 0
NV_PAPU_GPCTLG_GPSTYP                                                   0x00000001
NV_PAPU_GPCTLG_GPSTYP__BITSHIFT                                         0
NV_PAPU_GPCTLG_GPSTYP_NON_ISO                                           0
NV_PAPU_GPCTLG_GPSTYP_ISO                                               1
NV_PAPU_GPCTLG_SPARE                                                    0x00fffffe
NV_PAPU_GPCTLG_SPARE__BITSHIFT                                          1
NV_PAPU_GPCTLG_SPARE_INIT                                               0
#endif
    NAMER(NV_PAPU_GPCTLOUT),
#if 0
NV_PAPU_GPCTLOUT_GD0FMT                                                 0x00000003
NV_PAPU_GPCTLOUT_GD0FMT__BITSHIFT                                       0
NV_PAPU_GPCTLOUT_GD0FMT_MONO                                            0
NV_PAPU_GPCTLOUT_GD0FMT_STEREO                                          1
NV_PAPU_GPCTLOUT_GD0FMT_QUAD                                            2
NV_PAPU_GPCTLOUT_GD0FMT_HEX                                             3
NV_PAPU_GPCTLOUT_GP0TYP                                                 0x00000004
NV_PAPU_GPCTLOUT_GP0TYP__BITSHIFT                                       2
NV_PAPU_GPCTLOUT_GP0TYP_NON_ISO                                         0
NV_PAPU_GPCTLOUT_GP0TYP_ISO                                             1
NV_PAPU_GPCTLOUT_GD0SIZ                                                 0x00000018
NV_PAPU_GPCTLOUT_GD0SIZ__BITSHIFT                                       3
NV_PAPU_GPCTLOUT_GD0SIZ_8                                               0
NV_PAPU_GPCTLOUT_GD0SIZ_16                                              1
NV_PAPU_GPCTLOUT_GD0SIZ_32                                              2
NV_PAPU_GPCTLOUT_GD1FMT                                                 0x00000060
NV_PAPU_GPCTLOUT_GD1FMT__BITSHIFT                                       5
NV_PAPU_GPCTLOUT_GD1FMT_MONO                                            0
NV_PAPU_GPCTLOUT_GD1FMT_STEREO                                          1
NV_PAPU_GPCTLOUT_GD1FMT_QUAD                                            2
NV_PAPU_GPCTLOUT_GD1FMT_HEX                                             3
NV_PAPU_GPCTLOUT_GP1TYP                                                 0x00000080
NV_PAPU_GPCTLOUT_GP1TYP__BITSHIFT                                       7
NV_PAPU_GPCTLOUT_GP1TYP_NON_ISO                                         0
NV_PAPU_GPCTLOUT_GP1TYP_ISO                                             1
NV_PAPU_GPCTLOUT_GD1SIZ                                                 0x00000300
NV_PAPU_GPCTLOUT_GD1SIZ__BITSHIFT                                       8
NV_PAPU_GPCTLOUT_GD1SIZ_8                                               0
NV_PAPU_GPCTLOUT_GD1SIZ_16                                              1
NV_PAPU_GPCTLOUT_GD1SIZ_32                                              2
NV_PAPU_GPCTLOUT_GD2FMT                                                 0x00000c00
NV_PAPU_GPCTLOUT_GD2FMT__BITSHIFT                                       10
NV_PAPU_GPCTLOUT_GD2FMT_MONO                                            0
NV_PAPU_GPCTLOUT_GD2FMT_STEREO                                          1
NV_PAPU_GPCTLOUT_GD2FMT_QUAD                                            2
NV_PAPU_GPCTLOUT_GD2FMT_HEX                                             3
NV_PAPU_GPCTLOUT_GP2TYP                                                 0x00001000
NV_PAPU_GPCTLOUT_GP2TYP__BITSHIFT                                       12
NV_PAPU_GPCTLOUT_GP2TYP_NON_ISO                                         0
NV_PAPU_GPCTLOUT_GP2TYP_ISO                                             1
NV_PAPU_GPCTLOUT_GD2SIZ                                                 0x00006000
NV_PAPU_GPCTLOUT_GD2SIZ__BITSHIFT                                       13
NV_PAPU_GPCTLOUT_GD2SIZ_8                                               0
NV_PAPU_GPCTLOUT_GD2SIZ_16                                              1
NV_PAPU_GPCTLOUT_GD2SIZ_32                                              2
NV_PAPU_GPCTLOUT_GD3FMT                                                 0x00018000
NV_PAPU_GPCTLOUT_GD3FMT__BITSHIFT                                       15
NV_PAPU_GPCTLOUT_GD3FMT_MONO                                            0
NV_PAPU_GPCTLOUT_GD3FMT_STEREO                                          1
NV_PAPU_GPCTLOUT_GD3FMT_QUAD                                            2
NV_PAPU_GPCTLOUT_GD3FMT_HEX                                             3
NV_PAPU_GPCTLOUT_GP3TYP                                                 0x00020000
NV_PAPU_GPCTLOUT_GP3TYP__BITSHIFT                                       17
NV_PAPU_GPCTLOUT_GP3TYP_NON_ISO                                         0
NV_PAPU_GPCTLOUT_GP3TYP_ISO                                             1
NV_PAPU_GPCTLOUT_GD3SIZ                                                 0x000c0000
NV_PAPU_GPCTLOUT_GD3SIZ__BITSHIFT                                       18
NV_PAPU_GPCTLOUT_GD3SIZ_8                                               0
NV_PAPU_GPCTLOUT_GD3SIZ_16                                              1
NV_PAPU_GPCTLOUT_GD3SIZ_32                                              2
NV_PAPU_GPCTLOUT_SPARE                                                  0x00f00000
NV_PAPU_GPCTLOUT_SPARE__BITSHIFT                                        20
NV_PAPU_GPCTLOUT_SPARE_INIT                                             0
#endif
    NAMER(NV_PAPU_GPCTLIN),
#if 0
NV_PAPU_GPCTLIN_GDI0FMT                                                 0x00000003
NV_PAPU_GPCTLIN_GDI0FMT__BITSHIFT                                       0
NV_PAPU_GPCTLIN_GDI0FMT_MONO                                            0
NV_PAPU_GPCTLIN_GDI0FMT_STEREO                                          1
NV_PAPU_GPCTLIN_GDI0FMT_QUAD                                            2
NV_PAPU_GPCTLIN_GDI0FMT_HEX                                             3
NV_PAPU_GPCTLIN_GPI0TYP                                                 0x00000004
NV_PAPU_GPCTLIN_GPI0TYP__BITSHIFT                                       2
NV_PAPU_GPCTLIN_GPI0TYP_NON_ISO                                         0
NV_PAPU_GPCTLIN_GPI0TYP_ISO                                             1
NV_PAPU_GPCTLIN_GDI0SIZ                                                 0x00000018
NV_PAPU_GPCTLIN_GDI0SIZ__BITSHIFT                                       3
NV_PAPU_GPCTLIN_GDI0SIZ_8                                               0
NV_PAPU_GPCTLIN_GDI0SIZ_16                                              1
NV_PAPU_GPCTLIN_GDI0SIZ_32                                              2
NV_PAPU_GPCTLIN_GDI1FMT                                                 0x00000060
NV_PAPU_GPCTLIN_GDI1FMT__BITSHIFT                                       5
NV_PAPU_GPCTLIN_GDI1FMT_MONO                                            0
NV_PAPU_GPCTLIN_GDI1FMT_STEREO                                          1
NV_PAPU_GPCTLIN_GDI1FMT_QUAD                                            2
NV_PAPU_GPCTLIN_GDI1FMT_HEX                                             3
NV_PAPU_GPCTLIN_GPI1TYP                                                 0x00000080
NV_PAPU_GPCTLIN_GPI1TYP__BITSHIFT                                       7
NV_PAPU_GPCTLIN_GPI1TYP_NON_ISO                                         0
NV_PAPU_GPCTLIN_GPI1TYP_ISO                                             1
NV_PAPU_GPCTLIN_GDI1SIZ                                                 0x00000300
NV_PAPU_GPCTLIN_GDI1SIZ__BITSHIFT                                       8
NV_PAPU_GPCTLIN_GDI1SIZ_8                                               0
NV_PAPU_GPCTLIN_GDI1SIZ_16                                              1
NV_PAPU_GPCTLIN_GDI1SIZ_32                                              2
NV_PAPU_GPCTLIN_SPARE                                                   0x00fffc00
NV_PAPU_GPCTLIN_SPARE__BITSHIFT                                         10
NV_PAPU_GPCTLIN_SPARE_INIT                                              0
#endif
    NAMER(NV_PAPU_GPMBO),
#if 0
NV_PAPU_GPMBO_VALUE                                                     0x00ffffff
NV_PAPU_GPMBO_VALUE__BITSHIFT                                           0
#endif
    NAMER(NV_PAPU_GPMBI),
#if 0
NV_PAPU_GPMBI_VALUE                                                     0x00ffffff
NV_PAPU_GPMBI_VALUE__BITSHIFT                                           0
#endif
    NAMER(NV_PAPU_GPGET),
#if 0
NV_PAPU_GPGET_VALUE                                                     0x00000ffc
NV_PAPU_GPGET_VALUE__BITSHIFT                                           2
#endif
    NAMER(NV_PAPU_GPPUT),
#if 0
NV_PAPU_GPPUT_VALUE                                                     0x00000ffc
NV_PAPU_GPPUT_VALUE__BITSHIFT                                           2
#endif
    NAMER(NV_PAPU_GPIDRDY),
#if 0
NV_PAPU_GPIDRDY_GPSETIDLE                                               0x00000001
NV_PAPU_GPIDRDY_GPSETIDLE__BITSHIFT                                     0
NV_PAPU_GPIDRDY_GPSETIDLE_NOT_SET                                       0
NV_PAPU_GPIDRDY_GPSETIDLE_SET                                           1
NV_PAPU_GPIDRDY_GPSTOPPED                                               0x00000002
NV_PAPU_GPIDRDY_GPSTOPPED__BITSHIFT                                     1
NV_PAPU_GPIDRDY_GPSTOPPED_NOT_SET                                       0
NV_PAPU_GPIDRDY_GPSTOPPED_SET                                           1
NV_PAPU_GPIDRDY_GPSTOP_ENABLE                                           0x00000004
NV_PAPU_GPIDRDY_GPSTOP_ENABLE__BITSHIFT                                 2
NV_PAPU_GPIDRDY_GPSTOP_NOT_ENABLED                                      0
NV_PAPU_GPIDRDY_GPSTOP_ENABLED                                          1
NV_PAPU_GPIDRDY_GPSETNINT                                               0x00000008
NV_PAPU_GPIDRDY_GPSETNINT__BITSHIFT                                     3
NV_PAPU_GPIDRDY_GPSETNINT_NOT_SET                                       0
NV_PAPU_GPIDRDY_GPSETNINT_SET                                           1
#endif
    NAMER(NV_PAPU_GPT0CFG),
#if 0
NV_PAPU_GPT0CFG_GPT0RUN                                                 0x00000001
NV_PAPU_GPT0CFG_GPT0RUN__BITSHIFT                                       0
NV_PAPU_GPT0CFG_GPT0RUN_IDLE                                            0
NV_PAPU_GPT0CFG_GPT0RUN_RUN                                             1
NV_PAPU_GPT0CFG_GPT0DSTP                                                0x00000002
NV_PAPU_GPT0CFG_GPT0DSTP__BITSHIFT                                      1
NV_PAPU_GPT0CFG_GPT0DSTP_RUN                                            0
NV_PAPU_GPT0CFG_GPT0DSTP_HOLD                                           1
#endif
    NAMER(NV_PAPU_GPT1CFG),
#if 0
NV_PAPU_GPT1CFG_GPT1RUN                                                 0x00000001
NV_PAPU_GPT1CFG_GPT1RUN__BITSHIFT                                       0
NV_PAPU_GPT1CFG_GPT1RUN_IDLE                                            0
NV_PAPU_GPT1CFG_GPT1RUN_RUN                                             1
NV_PAPU_GPT1CFG_GPT1DSTP                                                0x00000002
NV_PAPU_GPT1CFG_GPT1DSTP__BITSHIFT                                      1
NV_PAPU_GPT1CFG_GPT1DSTP_RUN                                            0
NV_PAPU_GPT1CFG_GPT1DSTP_HOLD                                           1
#endif
    NAMER(NV_PAPU_GPT0CTL),
#if 0
NV_PAPU_GPT0CTL_GPT0ACT                                                 0x00000003
NV_PAPU_GPT0CTL_GPT0ACT__BITSHIFT                                       0
NV_PAPU_GPT0CTL_GPT0ACT_NOP                                             0
NV_PAPU_GPT0CTL_GPT0ACT_STZERO                                          1
NV_PAPU_GPT0CTL_GPT0ACT_STCUR                                           2
NV_PAPU_GPT0CTL_GPT0ACT_STOP                                            3
NV_PAPU_GPT0CTL_GPT0ST                                                  0x00000004
NV_PAPU_GPT0CTL_GPT0ST__BITSHIFT                                        2
NV_PAPU_GPT0CTL_GPT0ST_STOP                                             0
NV_PAPU_GPT0CTL_GPT0ST_RUN                                              1
#endif
    NAMER(NV_PAPU_GPT1CTL),
#if 0
NV_PAPU_GPT1CTL_GPT1ACT                                                 0x00000003
NV_PAPU_GPT1CTL_GPT1ACT__BITSHIFT                                       0
NV_PAPU_GPT1CTL_GPT1ACT_NOP                                             0
NV_PAPU_GPT1CTL_GPT1ACT_STZERO                                          1
NV_PAPU_GPT1CTL_GPT1ACT_STCUR                                           2
NV_PAPU_GPT1CTL_GPT1ACT_STOP                                            3
NV_PAPU_GPT1CTL_GPT1ST                                                  0x00000004
NV_PAPU_GPT1CTL_GPT1ST__BITSHIFT                                        2
NV_PAPU_GPT1CTL_GPT1ST_STOP                                             0
NV_PAPU_GPT1CTL_GPT1ST_RUN                                              1
#endif
    NAMER(NV_PAPU_GPT0TCNT),
#if 0
NV_PAPU_GPT0TCNT_VALUE                                                  0x00ffffff
NV_PAPU_GPT0TCNT_VALUE__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_GPT1TCNT),
#if 0
NV_PAPU_GPT1TCNT_VALUE                                                  0x00ffffff
NV_PAPU_GPT1TCNT_VALUE__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_GPT0CCNT),
#if 0
NV_PAPU_GPT0CCNT_VALUE                                                  0x00ffffff
NV_PAPU_GPT0CCNT_VALUE__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_GPT1CCNT),
#if 0
NV_PAPU_GPT1CCNT_VALUE                                                  0x00ffffff
NV_PAPU_GPT1CCNT_VALUE__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_GPISTS),
#if 0
NV_PAPU_GPISTS_GPSTRTSTS                                                0x00000001
NV_PAPU_GPISTS_GPSTRTSTS__BITSHIFT                                      0
NV_PAPU_GPISTS_GPSTRTSTS_NO_INTR                                        0
NV_PAPU_GPISTS_GPSTRTSTS_INTR                                           1
NV_PAPU_GPISTS_GPSTRTSTS_CLR                                            1
NV_PAPU_GPISTS_GPABRTSTS                                                0x00000002
NV_PAPU_GPISTS_GPABRTSTS__BITSHIFT                                      1
NV_PAPU_GPISTS_GPABRTSTS_NO_INTR                                        0
NV_PAPU_GPISTS_GPABRTSTS_INTR                                           1
NV_PAPU_GPISTS_GPABRTSTS_CLR                                            1
NV_PAPU_GPISTS_GPPUTSTS                                                 0x00000004
NV_PAPU_GPISTS_GPPUTSTS__BITSHIFT                                       2
NV_PAPU_GPISTS_GPPUTSTS_NO_INTR                                         0
NV_PAPU_GPISTS_GPPUTSTS_INTR                                            1
NV_PAPU_GPISTS_GPPUTSTS_CLR                                             1
NV_PAPU_GPISTS_GPMBISTS                                                 0x00000008
NV_PAPU_GPISTS_GPMBISTS__BITSHIFT                                       3
NV_PAPU_GPISTS_GPMBISTS_NO_INTR                                         0
NV_PAPU_GPISTS_GPMBISTS_INTR                                            1
NV_PAPU_GPISTS_GPMBISTS_CLR                                             1
NV_PAPU_GPISTS_GPD1STS                                                  0x00000010
NV_PAPU_GPISTS_GPD1STS__BITSHIFT                                        4
NV_PAPU_GPISTS_GPD1STS_NO_INTR                                          0
NV_PAPU_GPISTS_GPD1STS_INTR                                             1
NV_PAPU_GPISTS_GPD1STS_CLR                                              1
NV_PAPU_GPISTS_GPD2STS                                                  0x00000020
NV_PAPU_GPISTS_GPD2STS__BITSHIFT                                        5
NV_PAPU_GPISTS_GPD2STS_NO_INTR                                          0
NV_PAPU_GPISTS_GPD2STS_INTR                                             1
NV_PAPU_GPISTS_GPD2STS_CLR                                              1
NV_PAPU_GPISTS_GPD3STS                                                  0x00000040
NV_PAPU_GPISTS_GPD3STS__BITSHIFT                                        6
NV_PAPU_GPISTS_GPD3STS_NO_INTR                                          0
NV_PAPU_GPISTS_GPD3STS_INTR                                             1
NV_PAPU_GPISTS_GPD3STS_CLR                                              1
NV_PAPU_GPISTS_GPDEOLSTS                                                0x00000080
NV_PAPU_GPISTS_GPDEOLSTS__BITSHIFT                                      7
NV_PAPU_GPISTS_GPDEOLSTS_NO_INTR                                        0
NV_PAPU_GPISTS_GPDEOLSTS_INTR                                           1
NV_PAPU_GPISTS_GPDEOLSTS_CLR                                            1
NV_PAPU_GPISTS_GPT0STS                                                  0x00000100
NV_PAPU_GPISTS_GPT0STS__BITSHIFT                                        8
NV_PAPU_GPISTS_GPT0STS_NO_INTR                                          0
NV_PAPU_GPISTS_GPT0STS_INTR                                             1
NV_PAPU_GPISTS_GPT0STS_CLR                                              1
NV_PAPU_GPISTS_GPT1STS                                                  0x00000200
NV_PAPU_GPISTS_GPT1STS__BITSHIFT                                        9
NV_PAPU_GPISTS_GPT1STS_NO_INTR                                          0
NV_PAPU_GPISTS_GPT1STS_INTR                                             1
NV_PAPU_GPISTS_GPT1STS_CLR                                              1
NV_PAPU_GPISTS_GPDERRSTS                                                0x00000400
NV_PAPU_GPISTS_GPDERRSTS__BITSHIFT                                      10
NV_PAPU_GPISTS_GPDERRSTS_NO_INTR                                        0
NV_PAPU_GPISTS_GPDERRSTS_INTR                                           1
NV_PAPU_GPISTS_GPDERRSTS_CLR                                            1
#endif
    NAMER(NV_PAPU_GPHDRV_0),
    NAMER(NV_PAPU_GPHDRV_1),
    NAMER(NV_PAPU_GPHDRV_2),
    NAMER(NV_PAPU_GPHDRV_3),
    NAMER(NV_PAPU_GPHDRV_4),
    NAMER(NV_PAPU_GPHDRV_5),
    NAMER(NV_PAPU_GPHDRV_6),
    NAMER(NV_PAPU_GPHDRV_7),
    NAMER(NV_PAPU_GPHDRV_8),
    NAMER(NV_PAPU_GPHDRV_9),
    NAMER(NV_PAPU_GPHDRV_10),
    NAMER(NV_PAPU_GPHDRV_11),
    NAMER(NV_PAPU_GPHDRV_12),
    NAMER(NV_PAPU_GPHDRV_13),
    NAMER(NV_PAPU_GPHDRV_14),
    NAMER(NV_PAPU_GPHDRV_15),
    NAMER(NV_PAPU_GPHDRV_16),
    NAMER(NV_PAPU_GPHDRV_17),
    NAMER(NV_PAPU_GPHDRV_18),
    NAMER(NV_PAPU_GPHDRV_19),
    NAMER(NV_PAPU_GPHDRV_20),
    NAMER(NV_PAPU_GPHDRV_21),
    NAMER(NV_PAPU_GPHDRV_22),
    NAMER(NV_PAPU_GPHDRV_23),
    NAMER(NV_PAPU_GPHDRV_24),
    NAMER(NV_PAPU_GPHDRV_25),
    NAMER(NV_PAPU_GPHDRV_26),
    NAMER(NV_PAPU_GPHDRV_27),
    NAMER(NV_PAPU_GPHDRV_28),
    NAMER(NV_PAPU_GPHDRV_29),
    NAMER(NV_PAPU_GPHDRV_30),
    NAMER(NV_PAPU_GPHDRV_31),
#if 0
NV_PAPU_GPHDRV_MHR                                                      0x00000007
NV_PAPU_GPHDRV_MHR__BITSHIFT                                            0
NV_PAPU_GPHDRV_VLD                                                      0x00000008
NV_PAPU_GPHDRV_VLD__BITSHIFT                                            3
NV_PAPU_GPHDRV_VLD_INVALID                                              0
NV_PAPU_GPHDRV_VLD_VALID                                                1
#endif
    NAMER(NV_PAPU_GPGSCNT),
#if 0
NV_PAPU_GPGSCNT_GSCNT                                                   0x00ffffff
NV_PAPU_GPGSCNT_GSCNT__BITSHIFT                                         0
#endif
    NAMER(NV_PAPU_GPDCFG),
#if 0
NV_PAPU_GPDCFG_GPDASTRT                                                 0x00000001
NV_PAPU_GPDCFG_GPDASTRT__BITSHIFT                                       0
NV_PAPU_GPDCFG_GPDASTRT_DISABLED                                        0
NV_PAPU_GPDCFG_GPDASTRT_ENABLED                                         1
NV_PAPU_GPDCFG_GPDARDY                                                  0x00000002
NV_PAPU_GPDCFG_GPDARDY__BITSHIFT                                        1
NV_PAPU_GPDCFG_GPDARDY_DISABLED                                         0
NV_PAPU_GPDCFG_GPDARDY_ENABLED                                          1
NV_PAPU_GPDCFG_GPDIOCR                                                  0x00000004
NV_PAPU_GPDCFG_GPDIOCR__BITSHIFT                                        2
NV_PAPU_GPDCFG_GPDIOCR_DISABLED                                         0
NV_PAPU_GPDCFG_GPDIOCR_ENABLED                                          1
NV_PAPU_GPDCFG_GPDEOLR                                                  0x00000008
NV_PAPU_GPDCFG_GPDEOLR__BITSHIFT                                        3
NV_PAPU_GPDCFG_GPDEOLR_DISABLED                                         0
NV_PAPU_GPDCFG_GPDEOLR_ENABLED                                          1
NV_PAPU_GPDCFG_GPDERRR                                                  0x00000010
NV_PAPU_GPDCFG_GPDERRR__BITSHIFT                                        4
NV_PAPU_GPDCFG_GPDERRR_DISABLED                                         0
NV_PAPU_GPDCFG_GPDERRR_ENABLED                                          1
#endif
    NAMER(NV_PAPU_GPDCTL),
#if 0
NV_PAPU_GPDCTL_GPDACT                                                   0x00000007
NV_PAPU_GPDCTL_GPDACT__BITSHIFT                                         0
NV_PAPU_GPDCTL_GPDACT_NOP                                               0
NV_PAPU_GPDCTL_GPDACT_START                                             1
NV_PAPU_GPDCTL_GPDACT_STOP                                              2
NV_PAPU_GPDCTL_GPDACT_FREEZE                                            3
NV_PAPU_GPDCTL_GPDACT_UNFREEZE                                          4
NV_PAPU_GPDCTL_GPDFRZ                                                   0x00000008
NV_PAPU_GPDCTL_GPDFRZ__BITSHIFT                                         3
NV_PAPU_GPDCTL_GPDFRZ_NOT_FROZE                                         0
NV_PAPU_GPDCTL_GPDFRZ_FROZE                                             1
NV_PAPU_GPDCTL_GPDRUN                                                   0x00000010
NV_PAPU_GPDCTL_GPDRUN__BITSHIFT                                         4
NV_PAPU_GPDCTL_GPDRUN_NOT_RUN                                           0
NV_PAPU_GPDCTL_GPDRUN_RUNNING                                           1
NV_PAPU_GPDCTL_GPDSTP                                                   0x00000020
NV_PAPU_GPDCTL_GPDSTP__BITSHIFT                                         5
NV_PAPU_GPDCTL_GPDSTP_NOT_STOP                                          0
NV_PAPU_GPDCTL_GPDFRZ_STOPPED                                           1
#endif
    NAMER(NV_PAPU_GPDSPT),
#if 0
NV_PAPU_GPDSPT_GPDSBPT                                                  0x00003fff
NV_PAPU_GPDSPT_GPDSBPT__BITSHIFT                                        0
NV_PAPU_GPDSPT_GPDSBPT_INIT                                             0
NV_PAPU_GPDSPT_GPDEOL                                                   0x00004000
NV_PAPU_GPDSPT_GPDEOL__BITSHIFT                                         14
NV_PAPU_GPDSPT_GPDEOL_EOL                                               1
NV_PAPU_GPDSPT_GPDEOL_NOT_EOL                                           0
#endif
    NAMER(NV_PAPU_GPNSPT),
#if 0
NV_PAPU_GPNSPT_GPNSBPT                                                  0x00003fff
NV_PAPU_GPNSPT_GPNSBPT__BITSHIFT                                        0
NV_PAPU_GPNSPT_GPNSBPT_INIT                                             0
NV_PAPU_GPNSPT_GPNEOL                                                   0x00004000
NV_PAPU_GPNSPT_GPNEOL__BITSHIFT                                         14
NV_PAPU_GPNSPT_GPNEOL_NOT_EOL                                           0
NV_PAPU_GPNSPT_GPNEOL_EOL                                               1
#endif
    NAMER(NV_PAPU_GPDBASE_0),
    NAMER(NV_PAPU_GPDBASE_1),
    NAMER(NV_PAPU_GPDBASE_2),
    NAMER(NV_PAPU_GPDBASE_3),
#if 0
NV_PAPU_GPDBASE_BASE                                                    0x00ffffff
NV_PAPU_GPDBASE_BASE__BITSHIFT                                          0
#endif
    NAMER(NV_PAPU_GPDSIZE_0),
    NAMER(NV_PAPU_GPDSIZE_1),
    NAMER(NV_PAPU_GPDSIZE_2),
    NAMER(NV_PAPU_GPDSIZE_3),
#if 0
NV_PAPU_GPDSIZE_SIZE                                                    0x00ffffff
NV_PAPU_GPDSIZE_SIZE__BITSHIFT                                          0
#endif
    NAMER(NV_PAPU_GPDDEBUG),
#if 0
NV_PAPU_GPDDEBUG_PBADBLK                                                0x00003fff
NV_PAPU_GPDDEBUG_PBADBLK__BITSHIFT                                      0
NV_PAPU_GPDDEBUG_ILLSMBUFID                                             0x00004000
NV_PAPU_GPDDEBUG_ILLSMBUFID__BITSHIFT                                   14
NV_PAPU_GPDDEBUG_ILLSMBUFID_NOT_ILL                                     0
NV_PAPU_GPDDEBUG_ILLSMBUFID_ILL                                         1
NV_PAPU_GPDDEBUG_ILLSMDATAFMT                                           0x00008000
NV_PAPU_GPDDEBUG_ILLSMDATAFMT__BITSHIFT                                 15
NV_PAPU_GPDDEBUG_ILLSMDATAFMT_NOT_ILL                                   0
NV_PAPU_GPDDEBUG_ILLSMDATAFMT_ILL                                       1
NV_PAPU_GPDDEBUG_ILLCOUNT                                               0x00010000
NV_PAPU_GPDDEBUG_ILLCOUNT__BITSHIFT                                     16
NV_PAPU_GPDDEBUG_ILLCOUNT_NOT_ILL                                       0
NV_PAPU_GPDDEBUG_ILLCOUNT_ILL                                           1
NV_PAPU_GPDDEBUG_ILLDSPADDR                                             0x00020000
NV_PAPU_GPDDEBUG_ILLDSPADDR__BITSHIFT                                   17
NV_PAPU_GPDDEBUG_ILLDSPADDR_NOT_ILL                                     0
NV_PAPU_GPDDEBUG_ILLDSPADDR_ILL                                         1
#endif
};

