
enum {
        NV_PAPU_EPRST                               = 0x0000fffc,
#define NV_PAPU_EPRST_EPRST                                                     0x00000001
#define NV_PAPU_EPRST_EPRST__BITSHIFT                                           0
#define NV_PAPU_EPRST_EPRST_ENABLED                                             0
#define NV_PAPU_EPRST_EPRST_DISABLED                                            1
#define NV_PAPU_EPRST_EPDSPRST                                                  0x00000002
#define NV_PAPU_EPRST_EPDSPRST__BITSHIFT                                        1
#define NV_PAPU_EPRST_EPDSPRST_ENABLED                                          0
#define NV_PAPU_EPRST_EPDSPRST_DISABLED                                         1
#define NV_PAPU_EPRST_EPNMI                                                     0x00000004
#define NV_PAPU_EPRST_EPNMI__BITSHIFT                                           2
#define NV_PAPU_EPRST_EPNMI_DISABLED                                            0
#define NV_PAPU_EPRST_EPNMI_ENABLED                                             1
#define NV_PAPU_EPRST_EPABORT                                                   0x00000008
#define NV_PAPU_EPRST_EPABORT__BITSHIFT                                         3
#define NV_PAPU_EPRST_EPABORT_DISABLED                                          0
#define NV_PAPU_EPRST_EPABORT_ENABLED                                           1
        NV_PAPU_EPCTLG                              = 0x0000ff28,
#define NV_PAPU_EPCTLG_EPSTYP                                                   0x00000001
#define NV_PAPU_EPCTLG_EPSTYP__BITSHIFT                                         0
#define NV_PAPU_EPCTLG_EPSTYP_NON_ISO                                           0
#define NV_PAPU_EPCTLG_EPSTYP_ISO                                               1
#define NV_PAPU_EPCTLG_SPARE                                                    0x00fffffe
#define NV_PAPU_EPCTLG_SPARE__BITSHIFT                                          1
#define NV_PAPU_EPCTLG_SPARE_INIT                                               0
        NV_PAPU_EPCTLOUT                            = 0x0000ff20,
#define NV_PAPU_EPCTLOUT_ED0FMT                                                 0x00000003
#define NV_PAPU_EPCTLOUT_ED0FMT__BITSHIFT                                       0
#define NV_PAPU_EPCTLOUT_ED0FMT_MONO                                            0
#define NV_PAPU_EPCTLOUT_ED0FMT_STEREO                                          1
#define NV_PAPU_EPCTLOUT_ED0FMT_QUAD                                            2
#define NV_PAPU_EPCTLOUT_ED0FMT_HEX                                             3
#define NV_PAPU_EPCTLOUT_EP0TYP                                                 0x00000004
#define NV_PAPU_EPCTLOUT_EP0TYP__BITSHIFT                                       2
#define NV_PAPU_EPCTLOUT_EP0TYP_NON_ISO                                         0
#define NV_PAPU_EPCTLOUT_EP0TYP_ISO                                             1
#define NV_PAPU_EPCTLOUT_ED0SIZ                                                 0x00000018
#define NV_PAPU_EPCTLOUT_ED0SIZ__BITSHIFT                                       3
#define NV_PAPU_EPCTLOUT_ED0SIZ_8                                               0
#define NV_PAPU_EPCTLOUT_ED0SIZ_16                                              1
#define NV_PAPU_EPCTLOUT_ED0SIZ_32                                              2
#define NV_PAPU_EPCTLOUT_ED1FMT                                                 0x00000060
#define NV_PAPU_EPCTLOUT_ED1FMT__BITSHIFT                                       5
#define NV_PAPU_EPCTLOUT_ED1FMT_MONO                                            0
#define NV_PAPU_EPCTLOUT_ED1FMT_STEREO                                          1
#define NV_PAPU_EPCTLOUT_ED1FMT_QUAD                                            2
#define NV_PAPU_EPCTLOUT_ED1FMT_HEX                                             3
#define NV_PAPU_EPCTLOUT_EP1TYP                                                 0x00000080
#define NV_PAPU_EPCTLOUT_EP1TYP__BITSHIFT                                       7
#define NV_PAPU_EPCTLOUT_EP1TYP_NON_ISO                                         0
#define NV_PAPU_EPCTLOUT_EP1TYP_ISO                                             1
#define NV_PAPU_EPCTLOUT_ED1SIZ                                                 0x00000300
#define NV_PAPU_EPCTLOUT_ED1SIZ__BITSHIFT                                       8
#define NV_PAPU_EPCTLOUT_ED1SIZ_8                                               0
#define NV_PAPU_EPCTLOUT_ED1SIZ_16                                              1
#define NV_PAPU_EPCTLOUT_ED1SIZ_32                                              2
#define NV_PAPU_EPCTLOUT_ED2FMT                                                 0x00000c00
#define NV_PAPU_EPCTLOUT_ED2FMT__BITSHIFT                                       10
#define NV_PAPU_EPCTLOUT_ED2FMT_MONO                                            0
#define NV_PAPU_EPCTLOUT_ED2FMT_STEREO                                          1
#define NV_PAPU_EPCTLOUT_ED2FMT_QUAD                                            2
#define NV_PAPU_EPCTLOUT_ED2FMT_HEX                                             3
#define NV_PAPU_EPCTLOUT_EP2TYP                                                 0x00001000
#define NV_PAPU_EPCTLOUT_EP2TYP__BITSHIFT                                       12
#define NV_PAPU_EPCTLOUT_EP2TYP_NON_ISO                                         0
#define NV_PAPU_EPCTLOUT_EP2TYP_ISO                                             1
#define NV_PAPU_EPCTLOUT_ED2SIZ                                                 0x00006000
#define NV_PAPU_EPCTLOUT_ED2SIZ__BITSHIFT                                       13
#define NV_PAPU_EPCTLOUT_ED2SIZ_8                                               0
#define NV_PAPU_EPCTLOUT_ED2SIZ_16                                              1
#define NV_PAPU_EPCTLOUT_ED2SIZ_32                                              2
#define NV_PAPU_EPCTLOUT_ED3FMT                                                 0x00018000
#define NV_PAPU_EPCTLOUT_ED3FMT__BITSHIFT                                       15
#define NV_PAPU_EPCTLOUT_ED3FMT_MONO                                            0
#define NV_PAPU_EPCTLOUT_ED3FMT_STEREO                                          1
#define NV_PAPU_EPCTLOUT_ED3FMT_QUAD                                            2
#define NV_PAPU_EPCTLOUT_ED3FMT_HEX                                             3
#define NV_PAPU_EPCTLOUT_EP3TYP                                                 0x00020000
#define NV_PAPU_EPCTLOUT_EP3TYP__BITSHIFT                                       17
#define NV_PAPU_EPCTLOUT_EP3TYP_NON_ISO                                         0
#define NV_PAPU_EPCTLOUT_EP3TYP_ISO                                             1
#define NV_PAPU_EPCTLOUT_ED3SIZ                                                 0x000c0000
#define NV_PAPU_EPCTLOUT_ED3SIZ__BITSHIFT                                       18
#define NV_PAPU_EPCTLOUT_ED3SIZ_8                                               0
#define NV_PAPU_EPCTLOUT_ED3SIZ_16                                              1
#define NV_PAPU_EPCTLOUT_ED3SIZ_32                                              2
#define NV_PAPU_EPCTLOUT_SPARE                                                  0x00f00000
#define NV_PAPU_EPCTLOUT_SPARE__BITSHIFT                                        20
#define NV_PAPU_EPCTLOUT_SPARE_INIT                                             0
        NV_PAPU_EPCTLIN                             = 0x0000ff24,
#define NV_PAPU_EPCTLIN_EDI0FMT                                                 0x00000003
#define NV_PAPU_EPCTLIN_EDI0FMT__BITSHIFT                                       0
#define NV_PAPU_EPCTLIN_EDI0FMT_MONO                                            0
#define NV_PAPU_EPCTLIN_EDI0FMT_STEREO                                          1
#define NV_PAPU_EPCTLIN_EDI0FMT_QUAD                                            2
#define NV_PAPU_EPCTLIN_EDI0FMT_HEX                                             3
#define NV_PAPU_EPCTLIN_EPI0TYP                                                 0x00000004
#define NV_PAPU_EPCTLIN_EPI0TYP__BITSHIFT                                       2
#define NV_PAPU_EPCTLIN_EPI0TYP_NON_ISO                                         0
#define NV_PAPU_EPCTLIN_EPI0TYP_ISO                                             1
#define NV_PAPU_EPCTLIN_EDI0SIZ                                                 0x00000018
#define NV_PAPU_EPCTLIN_EDI0SIZ__BITSHIFT                                       3
#define NV_PAPU_EPCTLIN_EDI0SIZ_8                                               0
#define NV_PAPU_EPCTLIN_EDI0SIZ_16                                              1
#define NV_PAPU_EPCTLIN_EDI0SIZ_32                                              2
#define NV_PAPU_EPCTLIN_EDI1FMT                                                 0x00000060
#define NV_PAPU_EPCTLIN_EDI1FMT__BITSHIFT                                       5
#define NV_PAPU_EPCTLIN_EDI1FMT_MONO                                            0
#define NV_PAPU_EPCTLIN_EDI1FMT_STEREO                                          1
#define NV_PAPU_EPCTLIN_EDI1FMT_QUAD                                            2
#define NV_PAPU_EPCTLIN_EDI1FMT_HEX                                             3
#define NV_PAPU_EPCTLIN_EPI1TYP                                                 0x00000080
#define NV_PAPU_EPCTLIN_EPI1TYP__BITSHIFT                                       7
#define NV_PAPU_EPCTLIN_EPI1TYP_NON_ISO                                         0
#define NV_PAPU_EPCTLIN_EPI1TYP_ISO                                             1
#define NV_PAPU_EPCTLIN_EDI1SIZ                                                 0x00000300
#define NV_PAPU_EPCTLIN_EDI1SIZ__BITSHIFT                                       8
#define NV_PAPU_EPCTLIN_EDI1SIZ_8                                               0
#define NV_PAPU_EPCTLIN_EDI1SIZ_16                                              1
#define NV_PAPU_EPCTLIN_EDI1SIZ_32                                              2
#define NV_PAPU_EPCTLIN_SPARE                                                   0x00fffc00
#define NV_PAPU_EPCTLIN_SPARE__BITSHIFT                                         10
#define NV_PAPU_EPCTLIN_SPARE_INIT                                              0
        NV_PAPU_EPMBO                               = 0x0000ff08,
#define NV_PAPU_EPMBO_VALUE                                                     0x00ffffff
#define NV_PAPU_EPMBO_VALUE__BITSHIFT                                           0
        NV_PAPU_EPMBI                               = 0x0000ff0c,
#define NV_PAPU_EPMBI_VALUE                                                     0x00ffffff
#define NV_PAPU_EPMBI_VALUE__BITSHIFT                                           0
        NV_PAPU_EPGET                               = 0x0000ff00,
#define NV_PAPU_EPGET_VALUE                                                     0x00000ffc
#define NV_PAPU_EPGET_VALUE__BITSHIFT                                           2
        NV_PAPU_EPPUT                               = 0x0000ff04,
#define NV_PAPU_EPPUT_VALUE                                                     0x00000ffc
#define NV_PAPU_EPPUT_VALUE__BITSHIFT                                           2
        NV_PAPU_EPIDRDY                             = 0x0000ff10,
#define NV_PAPU_EPIDRDY_EPSETIDLE                                               0x00000001
#define NV_PAPU_EPIDRDY_EPSETIDLE__BITSHIFT                                     0
#define NV_PAPU_EPIDRDY_EPSETIDLE_NOT_SET                                       0
#define NV_PAPU_EPIDRDY_EPSETIDLE_SET                                           1
#define NV_PAPU_EPIDRDY_EPSTOPPED                                               0x00000002
#define NV_PAPU_EPIDRDY_EPSTOPPED__BITSHIFT                                     1
#define NV_PAPU_EPIDRDY_EPSTOPPED_NOT_SET                                       0
#define NV_PAPU_EPIDRDY_EPSTOPPED_SET                                           1
#define NV_PAPU_EPIDRDY_EPSTOP_ENABLE                                           0x00000004
#define NV_PAPU_EPIDRDY_EPSTOP_ENABLE__BITSHIFT                                 2
#define NV_PAPU_EPIDRDY_EPSTOP_NOT_ENABLED                                      0
#define NV_PAPU_EPIDRDY_EPSTOP_ENABLED                                          1
#define NV_PAPU_EPIDRDY_EPSETNINT                                               0x00000008
#define NV_PAPU_EPIDRDY_EPSETNINT__BITSHIFT                                     3
#define NV_PAPU_EPIDRDY_EPSETNINT_NOT_SET                                       0
#define NV_PAPU_EPIDRDY_EPSETNINT_SET                                           1
        NV_PAPU_EPT0CFG                             = 0x0000fec0,
#define NV_PAPU_EPT0CFG_EPT0RUN                                                 0x00000001
#define NV_PAPU_EPT0CFG_EPT0RUN__BITSHIFT                                       0
#define NV_PAPU_EPT0CFG_EPT0RUN_IDLE                                            0
#define NV_PAPU_EPT0CFG_EPT0RUN_RUN                                             1
#define NV_PAPU_EPT0CFG_EPT0DSTP                                                0x00000002
#define NV_PAPU_EPT0CFG_EPT0DSTP__BITSHIFT                                      1
#define NV_PAPU_EPT0CFG_EPT0DSTP_RUN                                            0
#define NV_PAPU_EPT0CFG_EPT0DSTP_HOLD                                           1
        NV_PAPU_EPT1CFG                             = 0x0000fed0,
#define NV_PAPU_EPT1CFG_EPT1RUN                                                 0x00000001
#define NV_PAPU_EPT1CFG_EPT1RUN__BITSHIFT                                       0
#define NV_PAPU_EPT1CFG_EPT1RUN_IDLE                                            0
#define NV_PAPU_EPT1CFG_EPT1RUN_RUN                                             1
#define NV_PAPU_EPT1CFG_EPT1DSTP                                                0x00000002
#define NV_PAPU_EPT1CFG_EPT1DSTP__BITSHIFT                                      1
#define NV_PAPU_EPT1CFG_EPT1DSTP_RUN                                            0
#define NV_PAPU_EPT1CFG_EPT1DSTP_HOLD                                           1
        NV_PAPU_EPT0CTL                             = 0x0000fec4,
#define NV_PAPU_EPT0CTL_EPT0ACT                                                 0x00000003
#define NV_PAPU_EPT0CTL_EPT0ACT__BITSHIFT                                       0
#define NV_PAPU_EPT0CTL_EPT0ACT_NOP                                             0
#define NV_PAPU_EPT0CTL_EPT0ACT_STZERO                                          1
#define NV_PAPU_EPT0CTL_EPT0ACT_STCUR                                           2
#define NV_PAPU_EPT0CTL_EPT0ACT_STOP                                            3
#define NV_PAPU_EPT0CTL_EPT0ST                                                  0x00000004
#define NV_PAPU_EPT0CTL_EPT0ST__BITSHIFT                                        2
#define NV_PAPU_EPT0CTL_EPT0ST_STOP                                             0
#define NV_PAPU_EPT0CTL_EPT0ST_RUN                                              1
        NV_PAPU_EPT1CTL                             = 0x0000fed4,
#define NV_PAPU_EPT1CTL_EPT1ACT                                                 0x00000003
#define NV_PAPU_EPT1CTL_EPT1ACT__BITSHIFT                                       0
#define NV_PAPU_EPT1CTL_EPT1ACT_NOP                                             0
#define NV_PAPU_EPT1CTL_EPT1ACT_STZERO                                          1
#define NV_PAPU_EPT1CTL_EPT1ACT_STCUR                                           2
#define NV_PAPU_EPT1CTL_EPT1ACT_STOP                                            3
#define NV_PAPU_EPT1CTL_EPT1ST                                                  0x00000004
#define NV_PAPU_EPT1CTL_EPT1ST__BITSHIFT                                        2
#define NV_PAPU_EPT1CTL_EPT1ST_STOP                                             0
#define NV_PAPU_EPT1CTL_EPT1ST_RUN                                              1
        NV_PAPU_EPT0TCNT                            = 0x0000fec8,
#define NV_PAPU_EPT0TCNT_VALUE                                                  0x00ffffff
#define NV_PAPU_EPT0TCNT_VALUE__BITSHIFT                                        0
        NV_PAPU_EPT1TCNT                            = 0x0000fed8,
#define NV_PAPU_EPT1TCNT_VALUE                                                  0x00ffffff
#define NV_PAPU_EPT1TCNT_VALUE__BITSHIFT                                        0
        NV_PAPU_EPT0CCNT                            = 0x0000fecc,
#define NV_PAPU_EPT0CCNT_VALUE                                                  0x00ffffff
#define NV_PAPU_EPT0CCNT_VALUE__BITSHIFT                                        0
        NV_PAPU_EPT1CCNT                            = 0x0000fedc,
#define NV_PAPU_EPT1CCNT_VALUE                                                  0x00ffffff
#define NV_PAPU_EPT1CCNT_VALUE__BITSHIFT                                        0
        NV_PAPU_EPISTS                              = 0x0000ff14,
#define NV_PAPU_EPISTS_EPSTRTSTS                                                0x00000001
#define NV_PAPU_EPISTS_EPSTRTSTS__BITSHIFT                                      0
#define NV_PAPU_EPISTS_EPSTRTSTS_NO_INTR                                        0
#define NV_PAPU_EPISTS_EPSTRTSTS_INTR                                           1
#define NV_PAPU_EPISTS_EPSTRTSTS_CLR                                            1
#define NV_PAPU_EPISTS_EPABRTSTS                                                0x00000002
#define NV_PAPU_EPISTS_EPABRTSTS__BITSHIFT                                      1
#define NV_PAPU_EPISTS_EPABRTSTS_NO_INTR                                        0
#define NV_PAPU_EPISTS_EPABRTSTS_INTR                                           1
#define NV_PAPU_EPISTS_EPABRTSTS_CLR                                            1
#define NV_PAPU_EPISTS_EPPUTSTS                                                 0x00000004
#define NV_PAPU_EPISTS_EPPUTSTS__BITSHIFT                                       2
#define NV_PAPU_EPISTS_EPPUTSTS_NO_INTR                                         0
#define NV_PAPU_EPISTS_EPPUTSTS_INTR                                            1
#define NV_PAPU_EPISTS_EPPUTSTS_CLR                                             1
#define NV_PAPU_EPISTS_EPMBISTS                                                 0x00000008
#define NV_PAPU_EPISTS_EPMBISTS__BITSHIFT                                       3
#define NV_PAPU_EPISTS_EPMBISTS_NO_INTR                                         0
#define NV_PAPU_EPISTS_EPMBISTS_INTR                                            1
#define NV_PAPU_EPISTS_EPMBISTS_CLR                                             1
#define NV_PAPU_EPISTS_EPD1STS                                                  0x00000010
#define NV_PAPU_EPISTS_EPD1STS__BITSHIFT                                        4
#define NV_PAPU_EPISTS_EPD1STS_NO_INTR                                          0
#define NV_PAPU_EPISTS_EPD1STS_INTR                                             1
#define NV_PAPU_EPISTS_EPD1STS_CLR                                              1
#define NV_PAPU_EPISTS_EPD2STS                                                  0x00000020
#define NV_PAPU_EPISTS_EPD2STS__BITSHIFT                                        5
#define NV_PAPU_EPISTS_EPD2STS_NO_INTR                                          0
#define NV_PAPU_EPISTS_EPD2STS_INTR                                             1
#define NV_PAPU_EPISTS_EPD2STS_CLR                                              1
#define NV_PAPU_EPISTS_EPD3STS                                                  0x00000040
#define NV_PAPU_EPISTS_EPD3STS__BITSHIFT                                        6
#define NV_PAPU_EPISTS_EPD3STS_NO_INTR                                          0
#define NV_PAPU_EPISTS_EPD3STS_INTR                                             1
#define NV_PAPU_EPISTS_EPD3STS_CLR                                              1
#define NV_PAPU_EPISTS_EPDEOLSTS                                                0x00000080
#define NV_PAPU_EPISTS_EPDEOLSTS__BITSHIFT                                      7
#define NV_PAPU_EPISTS_EPDEOLSTS_NO_INTR                                        0
#define NV_PAPU_EPISTS_EPDEOLSTS_INTR                                           1
#define NV_PAPU_EPISTS_EPDEOLSTS_CLR                                            1
#define NV_PAPU_EPISTS_EPT0STS                                                  0x00000100
#define NV_PAPU_EPISTS_EPT0STS__BITSHIFT                                        8
#define NV_PAPU_EPISTS_EPT0STS_NO_INTR                                          0
#define NV_PAPU_EPISTS_EPT0STS_INTR                                             1
#define NV_PAPU_EPISTS_EPT0STS_CLR                                              1
#define NV_PAPU_EPISTS_EPT1STS                                                  0x00000200
#define NV_PAPU_EPISTS_EPT1STS__BITSHIFT                                        9
#define NV_PAPU_EPISTS_EPT1STS_NO_INTR                                          0
#define NV_PAPU_EPISTS_EPT1STS_INTR                                             1
#define NV_PAPU_EPISTS_EPT1STS_CLR                                              1
#define NV_PAPU_EPISTS_EPDERRSTS                                                0x00000400
#define NV_PAPU_EPISTS_EPDERRSTS__BITSHIFT                                      10
#define NV_PAPU_EPISTS_EPDERRSTS_NO_INTR                                        0
#define NV_PAPU_EPISTS_EPDERRSTS_INTR                                           1
#define NV_PAPU_EPISTS_EPDERRSTS_CLR                                            1
        NV_PAPU_EPGSCNT                             = 0x0000ff18,
#define NV_PAPU_EPGSCNT_GSCNT                                                   0x00ffffff
#define NV_PAPU_EPGSCNT_GSCNT__BITSHIFT                                         0
        NV_PAPU_EPDCFG                              = 0x0000ff5c,
#define NV_PAPU_EPDCFG_EPDASTRT                                                 0x00000001
#define NV_PAPU_EPDCFG_EPDASTRT__BITSHIFT                                       0
#define NV_PAPU_EPDCFG_EPDASTRT_DISABLED                                        0
#define NV_PAPU_EPDCFG_EPDASTRT_ENABLED                                         1
#define NV_PAPU_EPDCFG_EPDARDY                                                  0x00000002
#define NV_PAPU_EPDCFG_EPDARDY__BITSHIFT                                        1
#define NV_PAPU_EPDCFG_EPDARDY_DISABLED                                         0
#define NV_PAPU_EPDCFG_EPDARDY_ENABLED                                          1
#define NV_PAPU_EPDCFG_EPDIOCR                                                  0x00000004
#define NV_PAPU_EPDCFG_EPDIOCR__BITSHIFT                                        2
#define NV_PAPU_EPDCFG_EPDIOCR_DISABLED                                         0
#define NV_PAPU_EPDCFG_EPDIOCR_ENABLED                                          1
#define NV_PAPU_EPDCFG_EPDEOLR                                                  0x00000008
#define NV_PAPU_EPDCFG_EPDEOLR__BITSHIFT                                        3
#define NV_PAPU_EPDCFG_EPDEOLR_DISABLED                                         0
#define NV_PAPU_EPDCFG_EPDEOLR_ENABLED                                          1
#define NV_PAPU_EPDCFG_EPDERRR                                                  0x00000010
#define NV_PAPU_EPDCFG_EPDERRR__BITSHIFT                                        4
#define NV_PAPU_EPDCFG_EPDERRR_DISABLED                                         0
#define NV_PAPU_EPDCFG_EPDERRR_ENABLED                                          1
        NV_PAPU_EPDCTL                              = 0x0000ff58,
#define NV_PAPU_EPDCTL_EPDACT                                                   0x00000007
#define NV_PAPU_EPDCTL_EPDACT__BITSHIFT                                         0
#define NV_PAPU_EPDCTL_EPDACT_NOP                                               0
#define NV_PAPU_EPDCTL_EPDACT_START                                             1
#define NV_PAPU_EPDCTL_EPDACT_STOP                                              2
#define NV_PAPU_EPDCTL_EPDACT_FREEZE                                            3
#define NV_PAPU_EPDCTL_EPDACT_UNFREEZE                                          4
#define NV_PAPU_EPDCTL_EPDFRZ                                                   0x00000008
#define NV_PAPU_EPDCTL_EPDFRZ__BITSHIFT                                         3
#define NV_PAPU_EPDCTL_EPDFRZ_NOT_FROZE                                         0
#define NV_PAPU_EPDCTL_EPDFRZ_FROZE                                             1
#define NV_PAPU_EPDCTL_EPDRUN                                                   0x00000010
#define NV_PAPU_EPDCTL_EPDRUN__BITSHIFT                                         4
#define NV_PAPU_EPDCTL_EPDRUN_NOT_RUN                                           0
#define NV_PAPU_EPDCTL_EPDRUN_RUNNING                                           1
#define NV_PAPU_EPDCTL_EPDSTP                                                   0x00000020
#define NV_PAPU_EPDCTL_EPDSTP__BITSHIFT                                         5
#define NV_PAPU_EPDCTL_EPDSTP_NOT_STOP                                          0
#define NV_PAPU_EPDCTL_EPDFRZ_STOPPED                                           1
        NV_PAPU_EPDSPT                              = 0x0000ff54,
#define NV_PAPU_EPDSPT_EPDSBPT                                                  0x00003fff
#define NV_PAPU_EPDSPT_EPDSBPT__BITSHIFT                                        0
#define NV_PAPU_EPDSPT_EPDSBPT_INIT                                             0
#define NV_PAPU_EPDSPT_EPDEOL                                                   0x00004000
#define NV_PAPU_EPDSPT_EPDEOL__BITSHIFT                                         14
#define NV_PAPU_EPDSPT_EPDEOL_EOL                                               1
#define NV_PAPU_EPDSPT_EPDEOL_NOT_EOL                                           0
        NV_PAPU_EPNSPT                              = 0x0000ff50,
#define NV_PAPU_EPNSPT_EPNSBPT                                                  0x00003fff
#define NV_PAPU_EPNSPT_EPNSBPT__BITSHIFT                                        0
#define NV_PAPU_EPNSPT_EPNSBPT_INIT                                             0
#define NV_PAPU_EPNSPT_EPNEOL                                                   0x00004000
#define NV_PAPU_EPNSPT_EPNEOL__BITSHIFT                                         14
#define NV_PAPU_EPNSPT_EPNEOL_EOL                                               1
#define NV_PAPU_EPNSPT_EPNEOL_NOT_EOL                                           0
        NV_PAPU_EPDBASE_0                           = 0x0000ff30,
        NV_PAPU_EPDBASE_1                           = 0x0000ff38,
        NV_PAPU_EPDBASE_2                           = 0x0000ff40,
        NV_PAPU_EPDBASE_3                           = 0x0000ff48,
#define NV_PAPU_EPDBASE_BASE                                                    0x00ffffff
#define NV_PAPU_EPDBASE_BASE__BITSHIFT                                          0
        NV_PAPU_EPDSIZE_0                           = 0x0000ff34,
        NV_PAPU_EPDSIZE_1                           = 0x0000ff3c,
        NV_PAPU_EPDSIZE_2                           = 0x0000ff44,
        NV_PAPU_EPDSIZE_3                           = 0x0000ff4c,
#define NV_PAPU_EPDSIZE_SIZE                                                    0x00ffffff
#define NV_PAPU_EPDSIZE_SIZE__BITSHIFT                                          0
        NV_PAPU_EPDDEBUG                            = 0x0000ff2c,
#define NV_PAPU_EPDDEBUG_PBADBLK                                                0x00003fff
#define NV_PAPU_EPDDEBUG_PBADBLK__BITSHIFT                                      0
#define NV_PAPU_EPDDEBUG_ILLSMBUFID                                             0x00004000
#define NV_PAPU_EPDDEBUG_ILLSMBUFID__BITSHIFT                                   14
#define NV_PAPU_EPDDEBUG_ILLSMBUFID_NOT_ILL                                     0
#define NV_PAPU_EPDDEBUG_ILLSMBUFID_ILL                                         1
#define NV_PAPU_EPDDEBUG_ILLSMDATAFMT                                           0x00008000
#define NV_PAPU_EPDDEBUG_ILLSMDATAFMT__BITSHIFT                                 15
#define NV_PAPU_EPDDEBUG_ILLSMDATAFMT_NOT_ILL                                   0
#define NV_PAPU_EPDDEBUG_ILLSMDATAFMT_ILL                                       1
#define NV_PAPU_EPDDEBUG_ILLCOUNT                                               0x00010000
#define NV_PAPU_EPDDEBUG_ILLCOUNT__BITSHIFT                                     16
#define NV_PAPU_EPDDEBUG_ILLCOUNT_NOT_ILL                                       0
#define NV_PAPU_EPDDEBUG_ILLCOUNT_ILL                                           1
};

static const char *const apu_papu_ep_name[] = {
    NAMER(NV_PAPU_EPRST),
#if 0
NV_PAPU_EPRST_EPRST                                                     0x00000001
NV_PAPU_EPRST_EPRST__BITSHIFT                                           0
NV_PAPU_EPRST_EPRST_ENABLED                                             0
NV_PAPU_EPRST_EPRST_DISABLED                                            1
NV_PAPU_EPRST_EPDSPRST                                                  0x00000002
NV_PAPU_EPRST_EPDSPRST__BITSHIFT                                        1
NV_PAPU_EPRST_EPDSPRST_ENABLED                                          0
NV_PAPU_EPRST_EPDSPRST_DISABLED                                         1
NV_PAPU_EPRST_EPNMI                                                     0x00000004
NV_PAPU_EPRST_EPNMI__BITSHIFT                                           2
NV_PAPU_EPRST_EPNMI_DISABLED                                            0
NV_PAPU_EPRST_EPNMI_ENABLED                                             1
NV_PAPU_EPRST_EPABORT                                                   0x00000008
NV_PAPU_EPRST_EPABORT__BITSHIFT                                         3
NV_PAPU_EPRST_EPABORT_DISABLED                                          0
NV_PAPU_EPRST_EPABORT_ENABLED                                           1
#endif
    NAMER(NV_PAPU_EPCTLG),
#if 0
NV_PAPU_EPCTLG_EPSTYP                                                   0x00000001
NV_PAPU_EPCTLG_EPSTYP__BITSHIFT                                         0
NV_PAPU_EPCTLG_EPSTYP_NON_ISO                                           0
NV_PAPU_EPCTLG_EPSTYP_ISO                                               1
NV_PAPU_EPCTLG_SPARE                                                    0x00fffffe
NV_PAPU_EPCTLG_SPARE__BITSHIFT                                          1
NV_PAPU_EPCTLG_SPARE_INIT                                               0
#endif
    NAMER(NV_PAPU_EPCTLOUT),
#if 0
NV_PAPU_EPCTLOUT_ED0FMT                                                 0x00000003
NV_PAPU_EPCTLOUT_ED0FMT__BITSHIFT                                       0
NV_PAPU_EPCTLOUT_ED0FMT_MONO                                            0
NV_PAPU_EPCTLOUT_ED0FMT_STEREO                                          1
NV_PAPU_EPCTLOUT_ED0FMT_QUAD                                            2
NV_PAPU_EPCTLOUT_ED0FMT_HEX                                             3
NV_PAPU_EPCTLOUT_EP0TYP                                                 0x00000004
NV_PAPU_EPCTLOUT_EP0TYP__BITSHIFT                                       2
NV_PAPU_EPCTLOUT_EP0TYP_NON_ISO                                         0
NV_PAPU_EPCTLOUT_EP0TYP_ISO                                             1
NV_PAPU_EPCTLOUT_ED0SIZ                                                 0x00000018
NV_PAPU_EPCTLOUT_ED0SIZ__BITSHIFT                                       3
NV_PAPU_EPCTLOUT_ED0SIZ_8                                               0
NV_PAPU_EPCTLOUT_ED0SIZ_16                                              1
NV_PAPU_EPCTLOUT_ED0SIZ_32                                              2
NV_PAPU_EPCTLOUT_ED1FMT                                                 0x00000060
NV_PAPU_EPCTLOUT_ED1FMT__BITSHIFT                                       5
NV_PAPU_EPCTLOUT_ED1FMT_MONO                                            0
NV_PAPU_EPCTLOUT_ED1FMT_STEREO                                          1
NV_PAPU_EPCTLOUT_ED1FMT_QUAD                                            2
NV_PAPU_EPCTLOUT_ED1FMT_HEX                                             3
NV_PAPU_EPCTLOUT_EP1TYP                                                 0x00000080
NV_PAPU_EPCTLOUT_EP1TYP__BITSHIFT                                       7
NV_PAPU_EPCTLOUT_EP1TYP_NON_ISO                                         0
NV_PAPU_EPCTLOUT_EP1TYP_ISO                                             1
NV_PAPU_EPCTLOUT_ED1SIZ                                                 0x00000300
NV_PAPU_EPCTLOUT_ED1SIZ__BITSHIFT                                       8
NV_PAPU_EPCTLOUT_ED1SIZ_8                                               0
NV_PAPU_EPCTLOUT_ED1SIZ_16                                              1
NV_PAPU_EPCTLOUT_ED1SIZ_32                                              2
NV_PAPU_EPCTLOUT_ED2FMT                                                 0x00000c00
NV_PAPU_EPCTLOUT_ED2FMT__BITSHIFT                                       10
NV_PAPU_EPCTLOUT_ED2FMT_MONO                                            0
NV_PAPU_EPCTLOUT_ED2FMT_STEREO                                          1
NV_PAPU_EPCTLOUT_ED2FMT_QUAD                                            2
NV_PAPU_EPCTLOUT_ED2FMT_HEX                                             3
NV_PAPU_EPCTLOUT_EP2TYP                                                 0x00001000
NV_PAPU_EPCTLOUT_EP2TYP__BITSHIFT                                       12
NV_PAPU_EPCTLOUT_EP2TYP_NON_ISO                                         0
NV_PAPU_EPCTLOUT_EP2TYP_ISO                                             1
NV_PAPU_EPCTLOUT_ED2SIZ                                                 0x00006000
NV_PAPU_EPCTLOUT_ED2SIZ__BITSHIFT                                       13
NV_PAPU_EPCTLOUT_ED2SIZ_8                                               0
NV_PAPU_EPCTLOUT_ED2SIZ_16                                              1
NV_PAPU_EPCTLOUT_ED2SIZ_32                                              2
NV_PAPU_EPCTLOUT_ED3FMT                                                 0x00018000
NV_PAPU_EPCTLOUT_ED3FMT__BITSHIFT                                       15
NV_PAPU_EPCTLOUT_ED3FMT_MONO                                            0
NV_PAPU_EPCTLOUT_ED3FMT_STEREO                                          1
NV_PAPU_EPCTLOUT_ED3FMT_QUAD                                            2
NV_PAPU_EPCTLOUT_ED3FMT_HEX                                             3
NV_PAPU_EPCTLOUT_EP3TYP                                                 0x00020000
NV_PAPU_EPCTLOUT_EP3TYP__BITSHIFT                                       17
NV_PAPU_EPCTLOUT_EP3TYP_NON_ISO                                         0
NV_PAPU_EPCTLOUT_EP3TYP_ISO                                             1
NV_PAPU_EPCTLOUT_ED3SIZ                                                 0x000c0000
NV_PAPU_EPCTLOUT_ED3SIZ__BITSHIFT                                       18
NV_PAPU_EPCTLOUT_ED3SIZ_8                                               0
NV_PAPU_EPCTLOUT_ED3SIZ_16                                              1
NV_PAPU_EPCTLOUT_ED3SIZ_32                                              2
NV_PAPU_EPCTLOUT_SPARE                                                  0x00f00000
NV_PAPU_EPCTLOUT_SPARE__BITSHIFT                                        20
NV_PAPU_EPCTLOUT_SPARE_INIT                                             0
#endif
    NAMER(NV_PAPU_EPCTLIN),
#if 0
NV_PAPU_EPCTLIN_EDI0FMT                                                 0x00000003
NV_PAPU_EPCTLIN_EDI0FMT__BITSHIFT                                       0
NV_PAPU_EPCTLIN_EDI0FMT_MONO                                            0
NV_PAPU_EPCTLIN_EDI0FMT_STEREO                                          1
NV_PAPU_EPCTLIN_EDI0FMT_QUAD                                            2
NV_PAPU_EPCTLIN_EDI0FMT_HEX                                             3
NV_PAPU_EPCTLIN_EPI0TYP                                                 0x00000004
NV_PAPU_EPCTLIN_EPI0TYP__BITSHIFT                                       2
NV_PAPU_EPCTLIN_EPI0TYP_NON_ISO                                         0
NV_PAPU_EPCTLIN_EPI0TYP_ISO                                             1
NV_PAPU_EPCTLIN_EDI0SIZ                                                 0x00000018
NV_PAPU_EPCTLIN_EDI0SIZ__BITSHIFT                                       3
NV_PAPU_EPCTLIN_EDI0SIZ_8                                               0
NV_PAPU_EPCTLIN_EDI0SIZ_16                                              1
NV_PAPU_EPCTLIN_EDI0SIZ_32                                              2
NV_PAPU_EPCTLIN_EDI1FMT                                                 0x00000060
NV_PAPU_EPCTLIN_EDI1FMT__BITSHIFT                                       5
NV_PAPU_EPCTLIN_EDI1FMT_MONO                                            0
NV_PAPU_EPCTLIN_EDI1FMT_STEREO                                          1
NV_PAPU_EPCTLIN_EDI1FMT_QUAD                                            2
NV_PAPU_EPCTLIN_EDI1FMT_HEX                                             3
NV_PAPU_EPCTLIN_EPI1TYP                                                 0x00000080
NV_PAPU_EPCTLIN_EPI1TYP__BITSHIFT                                       7
NV_PAPU_EPCTLIN_EPI1TYP_NON_ISO                                         0
NV_PAPU_EPCTLIN_EPI1TYP_ISO                                             1
NV_PAPU_EPCTLIN_EDI1SIZ                                                 0x00000300
NV_PAPU_EPCTLIN_EDI1SIZ__BITSHIFT                                       8
NV_PAPU_EPCTLIN_EDI1SIZ_8                                               0
NV_PAPU_EPCTLIN_EDI1SIZ_16                                              1
NV_PAPU_EPCTLIN_EDI1SIZ_32                                              2
NV_PAPU_EPCTLIN_SPARE                                                   0x00fffc00
NV_PAPU_EPCTLIN_SPARE__BITSHIFT                                         10
NV_PAPU_EPCTLIN_SPARE_INIT                                              0
#endif
    NAMER(NV_PAPU_EPMBO),
#if 0
NV_PAPU_EPMBO_VALUE                                                     0x00ffffff
NV_PAPU_EPMBO_VALUE__BITSHIFT                                           0
#endif
    NAMER(NV_PAPU_EPMBI),
#if 0
NV_PAPU_EPMBI_VALUE                                                     0x00ffffff
NV_PAPU_EPMBI_VALUE__BITSHIFT                                           0
#endif
    NAMER(NV_PAPU_EPGET),
#if 0
NV_PAPU_EPGET_VALUE                                                     0x00000ffc
NV_PAPU_EPGET_VALUE__BITSHIFT                                           2
#endif
    NAMER(NV_PAPU_EPPUT),
#if 0
NV_PAPU_EPPUT_VALUE                                                     0x00000ffc
NV_PAPU_EPPUT_VALUE__BITSHIFT                                           2
#endif
    NAMER(NV_PAPU_EPIDRDY),
#if 0
NV_PAPU_EPIDRDY_EPSETIDLE                                               0x00000001
NV_PAPU_EPIDRDY_EPSETIDLE__BITSHIFT                                     0
NV_PAPU_EPIDRDY_EPSETIDLE_NOT_SET                                       0
NV_PAPU_EPIDRDY_EPSETIDLE_SET                                           1
NV_PAPU_EPIDRDY_EPSTOPPED                                               0x00000002
NV_PAPU_EPIDRDY_EPSTOPPED__BITSHIFT                                     1
NV_PAPU_EPIDRDY_EPSTOPPED_NOT_SET                                       0
NV_PAPU_EPIDRDY_EPSTOPPED_SET                                           1
NV_PAPU_EPIDRDY_EPSTOP_ENABLE                                           0x00000004
NV_PAPU_EPIDRDY_EPSTOP_ENABLE__BITSHIFT                                 2
NV_PAPU_EPIDRDY_EPSTOP_NOT_ENABLED                                      0
NV_PAPU_EPIDRDY_EPSTOP_ENABLED                                          1
NV_PAPU_EPIDRDY_EPSETNINT                                               0x00000008
NV_PAPU_EPIDRDY_EPSETNINT__BITSHIFT                                     3
NV_PAPU_EPIDRDY_EPSETNINT_NOT_SET                                       0
NV_PAPU_EPIDRDY_EPSETNINT_SET                                           1
#endif
    NAMER(NV_PAPU_EPT0CFG),
#if 0
NV_PAPU_EPT0CFG_EPT0RUN                                                 0x00000001
NV_PAPU_EPT0CFG_EPT0RUN__BITSHIFT                                       0
NV_PAPU_EPT0CFG_EPT0RUN_IDLE                                            0
NV_PAPU_EPT0CFG_EPT0RUN_RUN                                             1
NV_PAPU_EPT0CFG_EPT0DSTP                                                0x00000002
NV_PAPU_EPT0CFG_EPT0DSTP__BITSHIFT                                      1
NV_PAPU_EPT0CFG_EPT0DSTP_RUN                                            0
NV_PAPU_EPT0CFG_EPT0DSTP_HOLD                                           1
#endif
    NAMER(NV_PAPU_EPT1CFG),
#if 0
NV_PAPU_EPT1CFG_EPT1RUN                                                 0x00000001
NV_PAPU_EPT1CFG_EPT1RUN__BITSHIFT                                       0
NV_PAPU_EPT1CFG_EPT1RUN_IDLE                                            0
NV_PAPU_EPT1CFG_EPT1RUN_RUN                                             1
NV_PAPU_EPT1CFG_EPT1DSTP                                                0x00000002
NV_PAPU_EPT1CFG_EPT1DSTP__BITSHIFT                                      1
NV_PAPU_EPT1CFG_EPT1DSTP_RUN                                            0
NV_PAPU_EPT1CFG_EPT1DSTP_HOLD                                           1
#endif
    NAMER(NV_PAPU_EPT0CTL),
#if 0
NV_PAPU_EPT0CTL_EPT0ACT                                                 0x00000003
NV_PAPU_EPT0CTL_EPT0ACT__BITSHIFT                                       0
NV_PAPU_EPT0CTL_EPT0ACT_NOP                                             0
NV_PAPU_EPT0CTL_EPT0ACT_STZERO                                          1
NV_PAPU_EPT0CTL_EPT0ACT_STCUR                                           2
NV_PAPU_EPT0CTL_EPT0ACT_STOP                                            3
NV_PAPU_EPT0CTL_EPT0ST                                                  0x00000004
NV_PAPU_EPT0CTL_EPT0ST__BITSHIFT                                        2
NV_PAPU_EPT0CTL_EPT0ST_STOP                                             0
NV_PAPU_EPT0CTL_EPT0ST_RUN                                              1
#endif
    NAMER(NV_PAPU_EPT1CTL),
#if 0
NV_PAPU_EPT1CTL_EPT1ACT                                                 0x00000003
NV_PAPU_EPT1CTL_EPT1ACT__BITSHIFT                                       0
NV_PAPU_EPT1CTL_EPT1ACT_NOP                                             0
NV_PAPU_EPT1CTL_EPT1ACT_STZERO                                          1
NV_PAPU_EPT1CTL_EPT1ACT_STCUR                                           2
NV_PAPU_EPT1CTL_EPT1ACT_STOP                                            3
NV_PAPU_EPT1CTL_EPT1ST                                                  0x00000004
NV_PAPU_EPT1CTL_EPT1ST__BITSHIFT                                        2
NV_PAPU_EPT1CTL_EPT1ST_STOP                                             0
NV_PAPU_EPT1CTL_EPT1ST_RUN                                              1
#endif
    NAMER(NV_PAPU_EPT0TCNT),
#if 0
NV_PAPU_EPT0TCNT_VALUE                                                  0x00ffffff
NV_PAPU_EPT0TCNT_VALUE__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_EPT1TCNT),
#if 0
NV_PAPU_EPT1TCNT_VALUE                                                  0x00ffffff
NV_PAPU_EPT1TCNT_VALUE__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_EPT0CCNT),
#if 0
NV_PAPU_EPT0CCNT_VALUE                                                  0x00ffffff
NV_PAPU_EPT0CCNT_VALUE__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_EPT1CCNT),
#if 0
NV_PAPU_EPT1CCNT_VALUE                                                  0x00ffffff
NV_PAPU_EPT1CCNT_VALUE__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_EPISTS),
#if 0
NV_PAPU_EPISTS_EPSTRTSTS                                                0x00000001
NV_PAPU_EPISTS_EPSTRTSTS__BITSHIFT                                      0
NV_PAPU_EPISTS_EPSTRTSTS_NO_INTR                                        0
NV_PAPU_EPISTS_EPSTRTSTS_INTR                                           1
NV_PAPU_EPISTS_EPSTRTSTS_CLR                                            1
NV_PAPU_EPISTS_EPABRTSTS                                                0x00000002
NV_PAPU_EPISTS_EPABRTSTS__BITSHIFT                                      1
NV_PAPU_EPISTS_EPABRTSTS_NO_INTR                                        0
NV_PAPU_EPISTS_EPABRTSTS_INTR                                           1
NV_PAPU_EPISTS_EPABRTSTS_CLR                                            1
NV_PAPU_EPISTS_EPPUTSTS                                                 0x00000004
NV_PAPU_EPISTS_EPPUTSTS__BITSHIFT                                       2
NV_PAPU_EPISTS_EPPUTSTS_NO_INTR                                         0
NV_PAPU_EPISTS_EPPUTSTS_INTR                                            1
NV_PAPU_EPISTS_EPPUTSTS_CLR                                             1
NV_PAPU_EPISTS_EPMBISTS                                                 0x00000008
NV_PAPU_EPISTS_EPMBISTS__BITSHIFT                                       3
NV_PAPU_EPISTS_EPMBISTS_NO_INTR                                         0
NV_PAPU_EPISTS_EPMBISTS_INTR                                            1
NV_PAPU_EPISTS_EPMBISTS_CLR                                             1
NV_PAPU_EPISTS_EPD1STS                                                  0x00000010
NV_PAPU_EPISTS_EPD1STS__BITSHIFT                                        4
NV_PAPU_EPISTS_EPD1STS_NO_INTR                                          0
NV_PAPU_EPISTS_EPD1STS_INTR                                             1
NV_PAPU_EPISTS_EPD1STS_CLR                                              1
NV_PAPU_EPISTS_EPD2STS                                                  0x00000020
NV_PAPU_EPISTS_EPD2STS__BITSHIFT                                        5
NV_PAPU_EPISTS_EPD2STS_NO_INTR                                          0
NV_PAPU_EPISTS_EPD2STS_INTR                                             1
NV_PAPU_EPISTS_EPD2STS_CLR                                              1
NV_PAPU_EPISTS_EPD3STS                                                  0x00000040
NV_PAPU_EPISTS_EPD3STS__BITSHIFT                                        6
NV_PAPU_EPISTS_EPD3STS_NO_INTR                                          0
NV_PAPU_EPISTS_EPD3STS_INTR                                             1
NV_PAPU_EPISTS_EPD3STS_CLR                                              1
NV_PAPU_EPISTS_EPDEOLSTS                                                0x00000080
NV_PAPU_EPISTS_EPDEOLSTS__BITSHIFT                                      7
NV_PAPU_EPISTS_EPDEOLSTS_NO_INTR                                        0
NV_PAPU_EPISTS_EPDEOLSTS_INTR                                           1
NV_PAPU_EPISTS_EPDEOLSTS_CLR                                            1
NV_PAPU_EPISTS_EPT0STS                                                  0x00000100
NV_PAPU_EPISTS_EPT0STS__BITSHIFT                                        8
NV_PAPU_EPISTS_EPT0STS_NO_INTR                                          0
NV_PAPU_EPISTS_EPT0STS_INTR                                             1
NV_PAPU_EPISTS_EPT0STS_CLR                                              1
NV_PAPU_EPISTS_EPT1STS                                                  0x00000200
NV_PAPU_EPISTS_EPT1STS__BITSHIFT                                        9
NV_PAPU_EPISTS_EPT1STS_NO_INTR                                          0
NV_PAPU_EPISTS_EPT1STS_INTR                                             1
NV_PAPU_EPISTS_EPT1STS_CLR                                              1
NV_PAPU_EPISTS_EPDERRSTS                                                0x00000400
NV_PAPU_EPISTS_EPDERRSTS__BITSHIFT                                      10
NV_PAPU_EPISTS_EPDERRSTS_NO_INTR                                        0
NV_PAPU_EPISTS_EPDERRSTS_INTR                                           1
NV_PAPU_EPISTS_EPDERRSTS_CLR                                            1
#endif
    NAMER(NV_PAPU_EPGSCNT),
#if 0
NV_PAPU_EPGSCNT_GSCNT                                                   0x00ffffff
NV_PAPU_EPGSCNT_GSCNT__BITSHIFT                                         0
#endif
    NAMER(NV_PAPU_EPDCFG),
#if 0
NV_PAPU_EPDCFG_EPDASTRT                                                 0x00000001
NV_PAPU_EPDCFG_EPDASTRT__BITSHIFT                                       0
NV_PAPU_EPDCFG_EPDASTRT_DISABLED                                        0
NV_PAPU_EPDCFG_EPDASTRT_ENABLED                                         1
NV_PAPU_EPDCFG_EPDARDY                                                  0x00000002
NV_PAPU_EPDCFG_EPDARDY__BITSHIFT                                        1
NV_PAPU_EPDCFG_EPDARDY_DISABLED                                         0
NV_PAPU_EPDCFG_EPDARDY_ENABLED                                          1
NV_PAPU_EPDCFG_EPDIOCR                                                  0x00000004
NV_PAPU_EPDCFG_EPDIOCR__BITSHIFT                                        2
NV_PAPU_EPDCFG_EPDIOCR_DISABLED                                         0
NV_PAPU_EPDCFG_EPDIOCR_ENABLED                                          1
NV_PAPU_EPDCFG_EPDEOLR                                                  0x00000008
NV_PAPU_EPDCFG_EPDEOLR__BITSHIFT                                        3
NV_PAPU_EPDCFG_EPDEOLR_DISABLED                                         0
NV_PAPU_EPDCFG_EPDEOLR_ENABLED                                          1
NV_PAPU_EPDCFG_EPDERRR                                                  0x00000010
NV_PAPU_EPDCFG_EPDERRR__BITSHIFT                                        4
NV_PAPU_EPDCFG_EPDERRR_DISABLED                                         0
NV_PAPU_EPDCFG_EPDERRR_ENABLED                                          1
#endif
    NAMER(NV_PAPU_EPDCTL),
#if 0
NV_PAPU_EPDCTL_EPDACT                                                   0x00000007
NV_PAPU_EPDCTL_EPDACT__BITSHIFT                                         0
NV_PAPU_EPDCTL_EPDACT_NOP                                               0
NV_PAPU_EPDCTL_EPDACT_START                                             1
NV_PAPU_EPDCTL_EPDACT_STOP                                              2
NV_PAPU_EPDCTL_EPDACT_FREEZE                                            3
NV_PAPU_EPDCTL_EPDACT_UNFREEZE                                          4
NV_PAPU_EPDCTL_EPDFRZ                                                   0x00000008
NV_PAPU_EPDCTL_EPDFRZ__BITSHIFT                                         3
NV_PAPU_EPDCTL_EPDFRZ_NOT_FROZE                                         0
NV_PAPU_EPDCTL_EPDFRZ_FROZE                                             1
NV_PAPU_EPDCTL_EPDRUN                                                   0x00000010
NV_PAPU_EPDCTL_EPDRUN__BITSHIFT                                         4
NV_PAPU_EPDCTL_EPDRUN_NOT_RUN                                           0
NV_PAPU_EPDCTL_EPDRUN_RUNNING                                           1
NV_PAPU_EPDCTL_EPDSTP                                                   0x00000020
NV_PAPU_EPDCTL_EPDSTP__BITSHIFT                                         5
NV_PAPU_EPDCTL_EPDSTP_NOT_STOP                                          0
NV_PAPU_EPDCTL_EPDFRZ_STOPPED                                           1
#endif
    NAMER(NV_PAPU_EPDSPT),
#if 0
NV_PAPU_EPDSPT_EPDSBPT                                                  0x00003fff
NV_PAPU_EPDSPT_EPDSBPT__BITSHIFT                                        0
NV_PAPU_EPDSPT_EPDSBPT_INIT                                             0
NV_PAPU_EPDSPT_EPDEOL                                                   0x00004000
NV_PAPU_EPDSPT_EPDEOL__BITSHIFT                                         14
NV_PAPU_EPDSPT_EPDEOL_EOL                                               1
NV_PAPU_EPDSPT_EPDEOL_NOT_EOL                                           0
#endif
    NAMER(NV_PAPU_EPNSPT),
#if 0
NV_PAPU_EPNSPT_EPNSBPT                                                  0x00003fff
NV_PAPU_EPNSPT_EPNSBPT__BITSHIFT                                        0
NV_PAPU_EPNSPT_EPNSBPT_INIT                                             0
NV_PAPU_EPNSPT_EPNEOL                                                   0x00004000
NV_PAPU_EPNSPT_EPNEOL__BITSHIFT                                         14
NV_PAPU_EPNSPT_EPNEOL_EOL                                               1
NV_PAPU_EPNSPT_EPNEOL_NOT_EOL                                           0
#endif
    NAMER(NV_PAPU_EPDBASE_0),
    NAMER(NV_PAPU_EPDBASE_1),
    NAMER(NV_PAPU_EPDBASE_2),
    NAMER(NV_PAPU_EPDBASE_3),
#if 0
NV_PAPU_EPDBASE_BASE                                                    0x00ffffff
NV_PAPU_EPDBASE_BASE__BITSHIFT                                          0
#endif
    NAMER(NV_PAPU_EPDSIZE_0),
    NAMER(NV_PAPU_EPDSIZE_1),
    NAMER(NV_PAPU_EPDSIZE_2),
    NAMER(NV_PAPU_EPDSIZE_3),
#if 0
NV_PAPU_EPDSIZE_SIZE                                                    0x00ffffff
NV_PAPU_EPDSIZE_SIZE__BITSHIFT                                          0
#endif
    NAMER(NV_PAPU_EPDDEBUG),
#if 0
NV_PAPU_EPDDEBUG_PBADBLK                                                0x00003fff
NV_PAPU_EPDDEBUG_PBADBLK__BITSHIFT                                      0
NV_PAPU_EPDDEBUG_ILLSMBUFID                                             0x00004000
NV_PAPU_EPDDEBUG_ILLSMBUFID__BITSHIFT                                   14
NV_PAPU_EPDDEBUG_ILLSMBUFID_NOT_ILL                                     0
NV_PAPU_EPDDEBUG_ILLSMBUFID_ILL                                         1
NV_PAPU_EPDDEBUG_ILLSMDATAFMT                                           0x00008000
NV_PAPU_EPDDEBUG_ILLSMDATAFMT__BITSHIFT                                 15
NV_PAPU_EPDDEBUG_ILLSMDATAFMT_NOT_ILL                                   0
NV_PAPU_EPDDEBUG_ILLSMDATAFMT_ILL                                       1
NV_PAPU_EPDDEBUG_ILLCOUNT                                               0x00010000
NV_PAPU_EPDDEBUG_ILLCOUNT__BITSHIFT                                     16
NV_PAPU_EPDDEBUG_ILLCOUNT_NOT_ILL                                       0
NV_PAPU_EPDDEBUG_ILLCOUNT_ILL                                           1
#endif
};

