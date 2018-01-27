
enum {
#define NV_PAPU_BITS                                                            0x0007ffff
#define NV_PAPU_BITS__BITSHIFT                                                  0
#define NV_PAPU_W                                                               19
#define NV_PAPU_SAVED_BITS                                                      0x0007fffc
#define NV_PAPU_SAVED_BITS__BITSHIFT                                            2
#define NV_PAPU_SAVED_W                                                         17
#define NV_PAPU_SUBSPACE_BITS                                                   0x00070000
#define NV_PAPU_SUBSPACE_BITS__BITSHIFT                                         16
#define NV_PAPU_SUBSPACE_PRIV                                                   0
#define NV_PAPU_SUBSPACE_NV1BA0                                                 2
#define NV_PAPU_SUBSPACE_GP                                                     3
#define NV_PAPU_SUBSPACE_NV1BXX                                                 4
#define NV_PAPU_SUBSPACE_EP                                                     5
#define NV_PAPU_PRIV_BITS                                                       0x0000ffff
#define NV_PAPU_PRIV_BITS__BITSHIFT                                             0
#define NV_PAPU_PRIV_SAVED_BITS                                                 0x0000fffc
#define NV_PAPU_PRIV_SAVED_BITS__BITSHIFT                                       2
#define NV_PAPU_PRIV_SAVED_W                                                    14
#define NV_PAPU_PRIV_SUBUNIT_BITS                                               0x0000f000
#define NV_PAPU_PRIV_SUBUNIT_BITS__BITSHIFT                                     12
#define NV_PAPU_PRIV_SUBUNIT_CFG                                                0
#define NV_PAPU_PRIV_SUBUNIT_FE                                                 1
#define NV_PAPU_PRIV_SUBUNIT_SE                                                 2
#define NV_PAPU_PRIV_SUBUNIT_SE_GP                                              3
#define NV_PAPU_PRIV_SUBUNIT_SE_EP                                              4
#define NV_PAPU_NV1BA0_BITS                                                     0x0000ffff
#define NV_PAPU_NV1BA0_BITS__BITSHIFT                                           0
        NV_PAPU_ISTS                                = 0x00001000,
#define NV_PAPU_ISTS_GINTSTS                                                    0x00000001
#define NV_PAPU_ISTS_GINTSTS__BITSHIFT                                          0
#define NV_PAPU_ISTS_GINTSTS_NO_INTR                                            0
#define NV_PAPU_ISTS_GINTSTS_INTR                                               1
#define NV_PAPU_ISTS_DWINTSTS                                                   0x00000002
#define NV_PAPU_ISTS_DWINTSTS__BITSHIFT                                         1
#define NV_PAPU_ISTS_DWINTSTS_NO_INTR                                           0
#define NV_PAPU_ISTS_DWINTSTS_INTR                                              1
#define NV_PAPU_ISTS_DWINTSTS_CLR                                               1
#define NV_PAPU_ISTS_RTWSTS                                                     0x00000004
#define NV_PAPU_ISTS_RTWSTS__BITSHIFT                                           2
#define NV_PAPU_ISTS_RTWSTS_NO_INTR                                             0
#define NV_PAPU_ISTS_RTWSTS_INTR                                                1
#define NV_PAPU_ISTS_DPINTSTS                                                   0x00000008
#define NV_PAPU_ISTS_DPINTSTS__BITSHIFT                                         3
#define NV_PAPU_ISTS_DPINTSTS_NO_INTR                                           0
#define NV_PAPU_ISTS_DPINTSTS_INTR                                              1
#define NV_PAPU_ISTS_DPINTSTS_CLR                                               1
#define NV_PAPU_ISTS_FETINTSTS                                                  0x00000010
#define NV_PAPU_ISTS_FETINTSTS__BITSHIFT                                        4
#define NV_PAPU_ISTS_FETINTSTS_NO_INTR                                          0
#define NV_PAPU_ISTS_FETINTSTS_INTR                                             1
#define NV_PAPU_ISTS_FETINTSTS_CLR                                              1
#define NV_PAPU_ISTS_FENINTSTS                                                  0x00000020
#define NV_PAPU_ISTS_FENINTSTS__BITSHIFT                                        5
#define NV_PAPU_ISTS_FENINTSTS_NO_INTR                                          0
#define NV_PAPU_ISTS_FENINTSTS_INTR                                             1
#define NV_PAPU_ISTS_FENINTSTS_CLR                                              1
#define NV_PAPU_ISTS_FEVINTSTS                                                  0x00000040
#define NV_PAPU_ISTS_FEVINTSTS__BITSHIFT                                        6
#define NV_PAPU_ISTS_FEVINTSTS_NO_INTR                                          0
#define NV_PAPU_ISTS_FEVINTSTS_INTR                                             1
#define NV_PAPU_ISTS_FEVINTSTS_CLR                                              1
#define NV_PAPU_ISTS_FEOINTSTS                                                  0x00000080
#define NV_PAPU_ISTS_FEOINTSTS__BITSHIFT                                        7
#define NV_PAPU_ISTS_FEOINTSTS_NO_INTR                                          0
#define NV_PAPU_ISTS_FEOINTSTS_INTR                                             1
#define NV_PAPU_ISTS_FEOINTSTS_CLR                                              1
#define NV_PAPU_ISTS_GPMINTSTS                                                  0x00000100
#define NV_PAPU_ISTS_GPMINTSTS__BITSHIFT                                        8
#define NV_PAPU_ISTS_GPMINTSTS_NO_INTR                                          0
#define NV_PAPU_ISTS_GPMINTSTS_INTR                                             1
#define NV_PAPU_ISTS_GPMINTSTS_CLR                                              1
#define NV_PAPU_ISTS_GPNINTSTS                                                  0x00000200
#define NV_PAPU_ISTS_GPNINTSTS__BITSHIFT                                        9
#define NV_PAPU_ISTS_GPNINTSTS_NO_INTR                                          0
#define NV_PAPU_ISTS_GPNINTSTS_INTR                                             1
#define NV_PAPU_ISTS_GPNINTSTS_CLR                                              1
#define NV_PAPU_ISTS_EPMINTSTS                                                  0x00000400
#define NV_PAPU_ISTS_EPMINTSTS__BITSHIFT                                        10
#define NV_PAPU_ISTS_EPMINTSTS_NO_INTR                                          0
#define NV_PAPU_ISTS_EPMINTSTS_INTR                                             1
#define NV_PAPU_ISTS_EPMINTSTS_CLR                                              1
#define NV_PAPU_ISTS_EPNINTSTS                                                  0x00000800
#define NV_PAPU_ISTS_EPNINTSTS__BITSHIFT                                        11
#define NV_PAPU_ISTS_EPNINTSTS_NO_INTR                                          0
#define NV_PAPU_ISTS_EPNINTSTS_INTR                                             1
#define NV_PAPU_ISTS_EPNINTSTS_CLR                                              1
#define NV_PAPU_ISTS_BITS                                                       0x00000fff
#define NV_PAPU_ISTS_BITS__BITSHIFT                                             0
#define NV_PAPU_ISTS_MOD_BITS                                                   0x00000ffe
#define NV_PAPU_ISTS_MOD_BITS__BITSHIFT                                         1
#define NV_PAPU_ISTS_MOD_UPPER_BITS                                             0x00000ff8
#define NV_PAPU_ISTS_MOD_UPPER_BITS__BITSHIFT                                   3
#define NV_PAPU_ISTS_MOD_LOWER_BITS                                             0x00000002
#define NV_PAPU_ISTS_MOD_LOWER_BITS__BITSHIFT                                   1
        NV_PAPU_IEN                                 = 0x00001004,
#define NV_PAPU_IEN_GINTEN                                                      0x00000001
#define NV_PAPU_IEN_GINTEN__BITSHIFT                                            0
#define NV_PAPU_IEN_GINTEN_DISABLED                                             0
#define NV_PAPU_IEN_GINTEN_ENABLED                                              1
#define NV_PAPU_IEN_DWINTEN                                                     0x00000002
#define NV_PAPU_IEN_DWINTEN__BITSHIFT                                           1
#define NV_PAPU_IEN_DWINTEN_DISABLED                                            0
#define NV_PAPU_IEN_DWINTEN_ENABLED                                             1
#define NV_PAPU_IEN_DPINTEN                                                     0x00000008
#define NV_PAPU_IEN_DPINTEN__BITSHIFT                                           3
#define NV_PAPU_IEN_DPINTEN_DISABLED                                            0
#define NV_PAPU_IEN_DPINTEN_ENABLED                                             1
#define NV_PAPU_IEN_FETINTEN                                                    0x00000010
#define NV_PAPU_IEN_FETINTEN__BITSHIFT                                          4
#define NV_PAPU_IEN_FETINTEN_DISABLED                                           0
#define NV_PAPU_IEN_FETINTEN_ENABLED                                            1
#define NV_PAPU_IEN_FENINTEN                                                    0x00000020
#define NV_PAPU_IEN_FENINTEN__BITSHIFT                                          5
#define NV_PAPU_IEN_FENINTEN_DISABLED                                           0
#define NV_PAPU_IEN_FENINTEN_ENABLED                                            1
#define NV_PAPU_IEN_FEVINTEN                                                    0x00000040
#define NV_PAPU_IEN_FEVINTEN__BITSHIFT                                          6
#define NV_PAPU_IEN_FEVINTEN_DISABLED                                           0
#define NV_PAPU_IEN_FEVINTEN_ENABLED                                            1
#define NV_PAPU_IEN_FEOINTEN                                                    0x00000080
#define NV_PAPU_IEN_FEOINTEN__BITSHIFT                                          7
#define NV_PAPU_IEN_FEOINTEN_DISABLED                                           0
#define NV_PAPU_IEN_FEOINTEN_ENABLED                                            1
#define NV_PAPU_IEN_GPMINTEN                                                    0x00000100
#define NV_PAPU_IEN_GPMINTEN__BITSHIFT                                          8
#define NV_PAPU_IEN_GPMINTEN_DISABLED                                           0
#define NV_PAPU_IEN_GPMINTEN_ENABLED                                            1
#define NV_PAPU_IEN_GPNINTEN                                                    0x00000200
#define NV_PAPU_IEN_GPNINTEN__BITSHIFT                                          9
#define NV_PAPU_IEN_GPNINTEN_DISABLED                                           0
#define NV_PAPU_IEN_GPNINTEN_ENABLED                                            1
#define NV_PAPU_IEN_EPMINTEN                                                    0x00000400
#define NV_PAPU_IEN_EPMINTEN__BITSHIFT                                          10
#define NV_PAPU_IEN_EPMINTEN_DISABLED                                           0
#define NV_PAPU_IEN_EPMINTEN_ENABLED                                            1
#define NV_PAPU_IEN_EPNINTEN                                                    0x00000800
#define NV_PAPU_IEN_EPNINTEN__BITSHIFT                                          11
#define NV_PAPU_IEN_EPNINTEN_DISABLED                                           0
#define NV_PAPU_IEN_EPNINTEN_ENABLED                                            1
#define NV_PAPU_IEN_BITS                                                        0x00000fff
#define NV_PAPU_IEN_BITS__BITSHIFT                                              0
#define NV_PAPU_IEN_SETTABLE_MASK                                               0x00000ffb
        NV_PAPU_ISET                                = 0x00001008,
#define NV_PAPU_ISET_DWINTSET                                                   0x00000002
#define NV_PAPU_ISET_DWINTSET__BITSHIFT                                         1
#define NV_PAPU_ISET_DWINTSET_NO_INTR                                           0
#define NV_PAPU_ISET_DWINTSET_INTR                                              1
#define NV_PAPU_ISET_DPINTSET                                                   0x00000008
#define NV_PAPU_ISET_DPINTSET__BITSHIFT                                         3
#define NV_PAPU_ISET_DPINTSET_NO_INTR                                           0
#define NV_PAPU_ISET_DPINTSET_INTR                                              1
#define NV_PAPU_ISET_FETINTSET                                                  0x00000010
#define NV_PAPU_ISET_FETINTSET__BITSHIFT                                        4
#define NV_PAPU_ISET_FETINTSET_NO_INTR                                          0
#define NV_PAPU_ISET_FETINTSET_INTR                                             1
#define NV_PAPU_ISET_FENINTSET                                                  0x00000020
#define NV_PAPU_ISET_FENINTSET__BITSHIFT                                        5
#define NV_PAPU_ISET_FENINTSET_NO_INTR                                          0
#define NV_PAPU_ISET_FENINTSET_INTR                                             1
#define NV_PAPU_ISET_FEVINTSET                                                  0x00000040
#define NV_PAPU_ISET_FEVINTSET__BITSHIFT                                        6
#define NV_PAPU_ISET_FEVINTSET_NO_INTR                                          0
#define NV_PAPU_ISET_FEVINTSET_INTR                                             1
#define NV_PAPU_ISET_FEOINTSET                                                  0x00000080
#define NV_PAPU_ISET_FEOINTSET__BITSHIFT                                        7
#define NV_PAPU_ISET_FEOINTSET_NO_INTR                                          0
#define NV_PAPU_ISET_FEOINTSET_INTR                                             1
#define NV_PAPU_ISET_GPMINTSET                                                  0x00000100
#define NV_PAPU_ISET_GPMINTSET__BITSHIFT                                        8
#define NV_PAPU_ISET_GPMINTSET_NO_INTR                                          0
#define NV_PAPU_ISET_GPMINTSET_INTR                                             1
#define NV_PAPU_ISET_GPNINTSET                                                  0x00000200
#define NV_PAPU_ISET_GPNINTSET__BITSHIFT                                        9
#define NV_PAPU_ISET_GPNINTSET_NO_INTR                                          0
#define NV_PAPU_ISET_GPNINTSET_INTR                                             1
#define NV_PAPU_ISET_EPMINTSET                                                  0x00000400
#define NV_PAPU_ISET_EPMINTSET__BITSHIFT                                        10
#define NV_PAPU_ISET_EPMINTSET_NO_INTR                                          0
#define NV_PAPU_ISET_EPMINTSET_INTR                                             1
#define NV_PAPU_ISET_EPNINTSET                                                  0x00000800
#define NV_PAPU_ISET_EPNINTSET__BITSHIFT                                        11
#define NV_PAPU_ISET_EPNINTSET_NO_INTR                                          0
#define NV_PAPU_ISET_EPNINTSET_INTR                                             1
#define NV_PAPU_ISET_BITS                                                       0x00000ffe
#define NV_PAPU_ISET_BITS__BITSHIFT                                             1
#define NV_PAPU_PRIV_FEMAJ_BITS                                                 0x00000f00
#define NV_PAPU_PRIV_FEMAJ_BITS__BITSHIFT                                       8
#define NV_PAPU_PRIV_FEMAJ_INTR                                                 0
#define NV_PAPU_PRIV_FEMAJ_METH                                                 1
#define NV_PAPU_PRIV_FEMAJ_PERF                                                 2
#define NV_PAPU_PRIV_FEMAJ_INTERNAL                                             3
#define NV_PAPU_PRIV_FEMAJ_UMETHFIFO                                            4
#define NV_PAPU_PRIV_FEMIN_BITS                                                 0x000000ff
#define NV_PAPU_PRIV_FEMIN_BITS__BITSHIFT                                       0
        NV_PAPU_FECTL                               = 0x00001100,
#define NV_PAPU_FECTL_FENTYP                                                    0x00000001
#define NV_PAPU_FECTL_FENTYP__BITSHIFT                                          0
#define NV_PAPU_FECTL_FENTYP_NON_ISO                                            0
#define NV_PAPU_FECTL_FENTYP_ISO                                                1
#define NV_PAPU_FECTL_FEMRTYP                                                   0x00000002
#define NV_PAPU_FECTL_FEMRTYP__BITSHIFT                                         1
#define NV_PAPU_FECTL_FEMRTYP_NON_ISO                                           0
#define NV_PAPU_FECTL_FEMRTYP_ISO                                               1
#define NV_PAPU_FECTL_FEMWTYP                                                   0x00000004
#define NV_PAPU_FECTL_FEMWTYP__BITSHIFT                                         2
#define NV_PAPU_FECTL_FEMWTYP_NON_ISO                                           0
#define NV_PAPU_FECTL_FEMWTYP_ISO                                               1
#define NV_PAPU_FECTL_FENINT                                                    0x00000008
#define NV_PAPU_FECTL_FENINT__BITSHIFT                                          3
#define NV_PAPU_FECTL_FENINT_DISABLED                                           0
#define NV_PAPU_FECTL_FENINT_ENABLED                                            1
#define NV_PAPU_FECTL_FEMETHMODE                                                0x000000e0
#define NV_PAPU_FECTL_FEMETHMODE__BITSHIFT                                      5
#define NV_PAPU_FECTL_FEMETHMODE_FREE_RUNNING                                   0
#define NV_PAPU_FECTL_FEMETHMODE_HALTED                                         4
#define NV_PAPU_FECTL_FEMETHMODE_TRAPPED                                        7
#define NV_PAPU_FECTL_FETRAPREASON                                              0x00000f00
#define NV_PAPU_FECTL_FETRAPREASON__BITSHIFT                                    8
#define NV_PAPU_FECTL_FETRAPREASON_NONE                                         0
#define NV_PAPU_FECTL_FETRAPREASON_NOT_IMPLEMENTED                              1
#define NV_PAPU_FECTL_FETRAPREASON_METHOD_UNKNOWN                               2
#define NV_PAPU_FECTL_FETRAPREASON_BAD_ARGUMENT                                 3
#define NV_PAPU_FECTL_FETRAPREASON_CURRENT_NOT_SET                              4
#define NV_PAPU_FECTL_FETRAPREASON_ANTECEDENT_NOT_SET                           5
#define NV_PAPU_FECTL_FETRAPREASON_BAD_DATA_TYPE                                7
#define NV_PAPU_FECTL_FETRAPREASON_BAD_LIST_POINTER                             8
#define NV_PAPU_FECTL_FETRAPREASON_CURRENT_VOICE_NOT_3D                         9
#define NV_PAPU_FECTL_FETRAPREASON_CTXPA_INVALID                                10
#define NV_PAPU_FECTL_FETRAPREASON_REQUESTED                                    15
#define NV_PAPU_FECTL_FEPIOCLASS                                                0x00001000
#define NV_PAPU_FECTL_FEPIOCLASS__BITSHIFT                                      12
#define NV_PAPU_FECTL_FEPIOCLASS_NON_ISO                                        0
#define NV_PAPU_FECTL_FEPIOCLASS_ISO                                            1
#define NV_PAPU_FECTL_FEMETH_PRIVLOCK                                           0x00002000
#define NV_PAPU_FECTL_FEMETH_PRIVLOCK__BITSHIFT                                 13
#define NV_PAPU_FECTL_FEMETH_PRIVLOCK_NOT_DISABLED                              0
#define NV_PAPU_FECTL_FEMETH_PRIVLOCK_DISABLED                                  1
#define NV_PAPU_FECTL_FECVLOCK                                                  0x00004000
#define NV_PAPU_FECTL_FECVLOCK__BITSHIFT                                        14
#define NV_PAPU_FECTL_FECVLOCK_UNLOCKED                                         0
#define NV_PAPU_FECTL_FECVLOCK_LOCKED                                           1
#define NV_PAPU_FECTL_FEMETHORIGIN                                              0x00008000
#define NV_PAPU_FECTL_FEMETHORIGIN__BITSHIFT                                    15
#define NV_PAPU_FECTL_FEMETHORIGIN_USER                                         0
#define NV_PAPU_FECTL_FEMETHORIGIN_INTERNAL                                     1
#define NV_PAPU_FECTL_FESESSLCTXPA_VALID                                        0x00010000
#define NV_PAPU_FECTL_FESESSLCTXPA_VALID__BITSHIFT                              16
#define NV_PAPU_FECTL_FESESSLCTXPA_VALID_NO                                     0
#define NV_PAPU_FECTL_FESESSLCTXPA_VALID_YES                                    1
#define NV_PAPU_FECTL_FESESGECTXPA_VALID                                        0x00020000
#define NV_PAPU_FECTL_FESESGECTXPA_VALID__BITSHIFT                              17
#define NV_PAPU_FECTL_FESESGECTXPA_VALID_NO                                     0
#define NV_PAPU_FECTL_FESESGECTXPA_VALID_YES                                    1
#define NV_PAPU_FECTL_FEGPSGECTXPA_VALID                                        0x00040000
#define NV_PAPU_FECTL_FEGPSGECTXPA_VALID__BITSHIFT                              18
#define NV_PAPU_FECTL_FEGPSGECTXPA_VALID_NO                                     0
#define NV_PAPU_FECTL_FEGPSGECTXPA_VALID_YES                                    1
#define NV_PAPU_FECTL_BITS                                                      0x0007ffff
#define NV_PAPU_FECTL_BITS__BITSHIFT                                            0
#define NV_PAPU_FECTL_WR_BITS                                                   0x00007fff
#define NV_PAPU_FECTL_WR_BITS__BITSHIFT                                         0
        NV_PAPU_FEMAXV                              = 0x00001104,
#define NV_PAPU_FEMAXV_VALUE                                                    0x0000ffff
#define NV_PAPU_FEMAXV_VALUE__BITSHIFT                                          0
#define NV_PAPU_FEMAXV_INIT                                                     0
#define NV_PAPU_FEMAXV_BITS                                                     0x0000ffff
#define NV_PAPU_FEMAXV_BITS__BITSHIFT                                           0
        NV_PAPU_FEMAXTV                             = 0x00001108,
#define NV_PAPU_FEMAXTV_VALUE                                                   0x0000ffff
#define NV_PAPU_FEMAXTV_VALUE__BITSHIFT                                         0
#define NV_PAPU_FEMAXTV_INIT                                                    0
#define NV_PAPU_FEMAXTV_BITS                                                    0x0000ffff
#define NV_PAPU_FEMAXTV_BITS__BITSHIFT                                          0
        NV_PAPU_FEAMAXV                             = 0x0000110c,
#define NV_PAPU_FEAMAXV_VALUE                                                   0x0000ffff
#define NV_PAPU_FEAMAXV_VALUE__BITSHIFT                                         0
#define NV_PAPU_FEAMAXV_UNLIMITED                                               0x0000ffff
#define NV_PAPU_FEAMAXV_BITS                                                    0x0000ffff
#define NV_PAPU_FEAMAXV_BITS__BITSHIFT                                          0
        NV_PAPU_FECV                                = 0x00001110,
#define NV_PAPU_FECV_VALUE                                                      0x0000ffff
#define NV_PAPU_FECV_VALUE__BITSHIFT                                            0
#define NV_PAPU_FECV_VALUE_NULL                                                 0x0000ffff
#define NV_PAPU_FECV_BITS                                                       0x0000ffff
#define NV_PAPU_FECV_BITS__BITSHIFT                                             0
        NV_PAPU_FECVDT                              = 0x00001114,
#define NV_PAPU_FECVDT_DATA_TYPE                                                0x00000003
#define NV_PAPU_FECVDT_DATA_TYPE__BITSHIFT                                      0
#define NV_PAPU_FECVDT_DATA_TYPE_BUFFER                                         0
#define NV_PAPU_FECVDT_DATA_TYPE_STREAM                                         1
#define NV_PAPU_FECVDT_DATA_TYPE_UNCACHED                                       2
#define NV_PAPU_FECVDT_CONTAINER_SIZE                                           0x0000000c
#define NV_PAPU_FECVDT_CONTAINER_SIZE__BITSHIFT                                 2
#define NV_PAPU_FECVDT_CONTAINER_SIZE_B8                                        0
#define NV_PAPU_FECVDT_CONTAINER_SIZE_B16                                       1
#define NV_PAPU_FECVDT_CONTAINER_SIZE_ADPCM                                     2
#define NV_PAPU_FECVDT_CONTAINER_SIZE_B32                                       3
#define NV_PAPU_FECVDT_BITS                                                     0x0000000f
#define NV_PAPU_FECVDT_BITS__BITSHIFT                                           0
        NV_PAPU_FEAV                                = 0x00001118,
#define NV_PAPU_FEAV_VALUE                                                      0x0000ffff
#define NV_PAPU_FEAV_VALUE__BITSHIFT                                            0
#define NV_PAPU_FEAV_VALUE_NULL                                                 0x0000ffff
#define NV_PAPU_FEAV_LST                                                        0x00030000
#define NV_PAPU_FEAV_LST__BITSHIFT                                              16
#define NV_PAPU_FEAV_LST_INHERIT                                                0
#define NV_PAPU_FEAV_LST_2D_TOP                                                 1
#define NV_PAPU_FEAV_LST_3D_TOP                                                 2
#define NV_PAPU_FEAV_LST_MP_TOP                                                 3
#define NV_PAPU_FEAV_BITS                                                       0x0003ffff
#define NV_PAPU_FEAV_BITS__BITSHIFT                                             0
        NV_PAPU_FEMAXHT                             = 0x0000111c,
#define NV_PAPU_FEMAXHT_VALUE                                                   0x0000ffff
#define NV_PAPU_FEMAXHT_VALUE__BITSHIFT                                         0
#define NV_PAPU_FEMAXHT_INIT                                                    0
#define NV_PAPU_FEMAXHT_BITS                                                    0x0000ffff
#define NV_PAPU_FEMAXHT_BITS__BITSHIFT                                          0
        NV_PAPU_FECHT                               = 0x00001120,
#define NV_PAPU_FECHT_VALUE                                                     0x0000ffff
#define NV_PAPU_FECHT_VALUE__BITSHIFT                                           0
#define NV_PAPU_FECHT_VALUE_NULL                                                0x0000ffff
#define NV_PAPU_FECHT_BITS                                                      0x0000ffff
#define NV_PAPU_FECHT_BITS__BITSHIFT                                            0
        NV_PAPU_FEMAXSESSL                          = 0x00001124,
#define NV_PAPU_FEMAXSESSL_VALUE                                                0x0000ffff
#define NV_PAPU_FEMAXSESSL_VALUE__BITSHIFT                                      0
#define NV_PAPU_FEMAXSESSL_INIT                                                 0
#define NV_PAPU_FEMAXSESSL_BITS                                                 0x0000ffff
#define NV_PAPU_FEMAXSESSL_BITS__BITSHIFT                                       0
        NV_PAPU_FECSESSL                            = 0x00001128,
#define NV_PAPU_FECSESSL_VALUE                                                  0x0000ffff
#define NV_PAPU_FECSESSL_VALUE__BITSHIFT                                        0
#define NV_PAPU_FECSESSL_VALUE_NULL                                             0x0000ffff
#define NV_PAPU_FECSESSL_BITS                                                   0x0000ffff
#define NV_PAPU_FECSESSL_BITS__BITSHIFT                                         0
        NV_PAPU_FESESSLCTXPA                        = 0x0000112c,
#define NV_PAPU_FESESSLCTXPA_VALUE                                              0xfffff000
#define NV_PAPU_FESESSLCTXPA_VALUE__BITSHIFT                                    12
        NV_PAPU_FESESSLMAXOFF                       = 0x00001130,
#define NV_PAPU_FESESSLMAXOFF_VALUE                                             0xfffff000
#define NV_PAPU_FESESSLMAXOFF_VALUE__BITSHIFT                                   12
        NV_PAPU_FESESSLPENDOFF                      = 0x00001134,
        NV_PAPU_FEMAXSESGE                          = 0x00001138,
#define NV_PAPU_FEMAXSESGE_VALUE                                                0x0000ffff
#define NV_PAPU_FEMAXSESGE_VALUE__BITSHIFT                                      0
#define NV_PAPU_FEMAXSESGE_INIT                                                 0
#define NV_PAPU_FEMAXSESGE_BITS                                                 0x0000ffff
#define NV_PAPU_FEMAXSESGE_BITS__BITSHIFT                                       0
        NV_PAPU_FECSESGE                            = 0x0000113c,
#define NV_PAPU_FECSESGE_VALUE                                                  0x0000ffff
#define NV_PAPU_FECSESGE_VALUE__BITSHIFT                                        0
#define NV_PAPU_FECSESGE_VALUE_NULL                                             0x0000ffff
#define NV_PAPU_FECSESGE_BITS                                                   0x0000ffff
#define NV_PAPU_FECSESGE_BITS__BITSHIFT                                         0
        NV_PAPU_FESESGECTXPA                        = 0x00001140,
#define NV_PAPU_FESESGECTXPA_VALUE                                              0xfffff000
#define NV_PAPU_FESESGECTXPA_VALUE__BITSHIFT                                    12
        NV_PAPU_FESESGEMAXOFF                       = 0x00001144,
#define NV_PAPU_FESESGEMAXOFF_VALUE                                             0xfffff000
#define NV_PAPU_FESESGEMAXOFF_VALUE__BITSHIFT                                   12
        NV_PAPU_FEMAXGPSGE                          = 0x00001148,
#define NV_PAPU_FEMAXGPSGE_VALUE                                                0x0000ffff
#define NV_PAPU_FEMAXGPSGE_VALUE__BITSHIFT                                      0
#define NV_PAPU_FEMAXGPSGE_INIT                                                 0
#define NV_PAPU_FEMAXGPSGE_BITS                                                 0x0000ffff
#define NV_PAPU_FEMAXGPSGE_BITS__BITSHIFT                                       0
        NV_PAPU_FECGPSGE                            = 0x0000114c,
#define NV_PAPU_FECGPSGE_VALUE                                                  0x0000ffff
#define NV_PAPU_FECGPSGE_VALUE__BITSHIFT                                        0
#define NV_PAPU_FECGPSGE_VALUE_NULL                                             0x0000ffff
#define NV_PAPU_FECGPSGE_BITS                                                   0x0000ffff
#define NV_PAPU_FECGPSGE_BITS__BITSHIFT                                         0
        NV_PAPU_FEGPSGECTXPA                        = 0x00001150,
#define NV_PAPU_FEGPSGECTXPA_VALUE                                              0xfffff000
#define NV_PAPU_FEGPSGECTXPA_VALUE__BITSHIFT                                    12
        NV_PAPU_FEGPSGEMAXOFF                       = 0x00001154,
#define NV_PAPU_FEGPSGEMAXOFF_VALUE                                             0xfffff000
#define NV_PAPU_FEGPSGEMAXOFF_VALUE__BITSHIFT                                   12
        NV_PAPU_FEMAXMB                             = 0x00001158,
#define NV_PAPU_FEMAXMB_VALUE                                                   0x0000003f
#define NV_PAPU_FEMAXMB_VALUE__BITSHIFT                                         0
#define NV_PAPU_FEMAXMB_BITS                                                    0x0000003f
#define NV_PAPU_FEMAXMB_BITS__BITSHIFT                                          0
        NV_PAPU_FENADDR                             = 0x0000115c,
        NV_PAPU_AVGBW                               = 0x00001200,
#define NV_PAPU_AVGBW_VALUE                                                     0x001fffff
#define NV_PAPU_AVGBW_VALUE__BITSHIFT                                           0
#define NV_PAPU_AVGBW_BITS                                                      0x001fffff
#define NV_PAPU_AVGBW_BITS__BITSHIFT                                            0
        NV_PAPU_WCBW                                = 0x00001204,
#define NV_PAPU_WCBW_VALUE                                                      0x001fffff
#define NV_PAPU_WCBW_VALUE__BITSHIFT                                            0
#define NV_PAPU_WCBW_BITS                                                       0x001fffff
#define NV_PAPU_WCBW_BITS__BITSHIFT                                             0
        NV_PAPU_CURBW                               = 0x00001208,
#define NV_PAPU_CURBW_VALUE                                                     0x001fffff
#define NV_PAPU_CURBW_VALUE__BITSHIFT                                           0
#define NV_PAPU_CURBW_BITS                                                      0x001fffff
#define NV_PAPU_CURBW_BITS__BITSHIFT                                            0
        NV_PAPU_CNTBW                               = 0x0000120c,
#define NV_PAPU_CNTBW_VALUE                                                     0x000fffff
#define NV_PAPU_CNTBW_VALUE__BITSHIFT                                           0
#define NV_PAPU_CNTBW_BITS                                                      0x000fffff
#define NV_PAPU_CNTBW_BITS__BITSHIFT                                            0
        NV_PAPU_AVGLAT                              = 0x00001210,
#define NV_PAPU_AVGLAT_VALUE                                                    0x00003fff
#define NV_PAPU_AVGLAT_VALUE__BITSHIFT                                          0
#define NV_PAPU_AVGLAT_BITS                                                     0x00003fff
#define NV_PAPU_AVGLAT_BITS__BITSHIFT                                           0
        NV_PAPU_WCLAT                               = 0x00001214,
#define NV_PAPU_WCLAT_VALUE                                                     0x00003fff
#define NV_PAPU_WCLAT_VALUE__BITSHIFT                                           0
#define NV_PAPU_WCLAT_BITS                                                      0x00003fff
#define NV_PAPU_WCLAT_BITS__BITSHIFT                                            0
        NV_PAPU_CURLAT                              = 0x00001218,
#define NV_PAPU_CURLAT_VALUE                                                    0x00ffffff
#define NV_PAPU_CURLAT_VALUE__BITSHIFT                                          0
#define NV_PAPU_CURLAT_BITS                                                     0x00ffffff
#define NV_PAPU_CURLAT_BITS__BITSHIFT                                           0
#define NV_PAPU_CURLAT_AVGLAT_BITS                                              0x00fffc00
#define NV_PAPU_CURLAT_AVGLAT_BITS__BITSHIFT                                    10
        NV_PAPU_CNTLAT                              = 0x0000121c,
#define NV_PAPU_CNTLAT_VALUE                                                    0x000003ff
#define NV_PAPU_CNTLAT_VALUE__BITSHIFT                                          0
#define NV_PAPU_CNTLAT_VALUE_MAX                                                0x000003ff
#define NV_PAPU_CNTLAT_BITS                                                     0x000003ff
#define NV_PAPU_CNTLAT_BITS__BITSHIFT                                           0
        NV_PAPU_FEDECMETH                           = 0x00001300,
#define NV_PAPU_FEDECMETH_METH                                                  0x0000ffff
#define NV_PAPU_FEDECMETH_METH__BITSHIFT                                        0
#define NV_PAPU_FEDECMETH_PHASE                                                 0x00ff0000
#define NV_PAPU_FEDECMETH_PHASE__BITSHIFT                                       16
#define NV_PAPU_FEDECMETH_PHASE_LAST                                            0x01000000
#define NV_PAPU_FEDECMETH_PHASE_LAST__BITSHIFT                                  24
#define NV_PAPU_FEDECMETH_PHASE_LAST_FALSE                                      0
#define NV_PAPU_FEDECMETH_PHASE_LAST_TRUE                                       1
#define NV_PAPU_FEDECMETH_VALID                                                 0x02000000
#define NV_PAPU_FEDECMETH_VALID__BITSHIFT                                       25
#define NV_PAPU_FEDECMETH_VALID_FALSE                                           0
#define NV_PAPU_FEDECMETH_VALID_TRUE                                            1
#define NV_PAPU_FEDECMETH_BITS                                                  0x03ffffff
#define NV_PAPU_FEDECMETH_BITS__BITSHIFT                                        0
        NV_PAPU_FEDECPARAM                          = 0x00001304,
        NV_PAPU_FEDECIMMED                          = 0x00001308,
        NV_PAPU_FESCRATCH                           = 0x0000130c,
        NV_PAPU_FESCRATCH2                          = 0x00001310,
        NV_PAPU_FESCRATCH3                          = 0x00001314,
        NV_PAPU_FESCRATCH4                          = 0x00001318,
        NV_PAPU_FEMEMADDR                           = 0x00001324,
        NV_PAPU_FEMEMSZ                             = 0x00001328,
#define NV_PAPU_FEMEMSZ_VALUE                                                   0x0000003f
#define NV_PAPU_FEMEMSZ_VALUE__BITSHIFT                                         0
#define NV_PAPU_FEMEMSZ_BITS                                                    0x0000003f
#define NV_PAPU_FEMEMSZ_BITS__BITSHIFT                                          0
        NV_PAPU_FEMEMISO                            = 0x0000132c,
#define NV_PAPU_FEMEMISO_WR                                                     0x00000001
#define NV_PAPU_FEMEMISO_WR__BITSHIFT                                           0
#define NV_PAPU_FEMEMISO_WR_NON_ISO                                             0
#define NV_PAPU_FEMEMISO_WR_ISO                                                 1
#define NV_PAPU_FEMEMISO_RD                                                     0x00000002
#define NV_PAPU_FEMEMISO_RD__BITSHIFT                                           1
#define NV_PAPU_FEMEMISO_RD_NON_ISO                                             0
#define NV_PAPU_FEMEMISO_RD_ISO                                                 1
#define NV_PAPU_FEMEMISO_BITS                                                   0x00000003
#define NV_PAPU_FEMEMISO_BITS__BITSHIFT                                         0
        NV_PAPU_FEMEMWE                             = 0x00001330,
#define NV_PAPU_FEMEMWE_VALUE                                                   0x0000000f
#define NV_PAPU_FEMEMWE_VALUE__BITSHIFT                                         0
#define NV_PAPU_FEMEMWE_VALUE_ALL                                               15
#define NV_PAPU_FEMEMWE_BITS                                                    0x0000000f
#define NV_PAPU_FEMEMWE_BITS__BITSHIFT                                          0
        NV_PAPU_FEMEMDATA                           = 0x00001334,
        NV_PAPU_FESEIDLELINKS                       = 0x00001338,
#define NV_PAPU_FESEIDLELINKS_PREV                                              0x0000ffff
#define NV_PAPU_FESEIDLELINKS_PREV__BITSHIFT                                    0
#define NV_PAPU_FESEIDLELINKS_PREV_NULL                                         0x0000ffff
#define NV_PAPU_FESEIDLELINKS_NEXT                                              0xffff0000
#define NV_PAPU_FESEIDLELINKS_NEXT__BITSHIFT                                    16
#define NV_PAPU_FESEIDLELINKS_NEXT_NULL                                         0x0000ffff
        NV_PAPU_FESENOTIFY                          = 0x0000133c,
#define NV_PAPU_FESENOTIFY_VOICE                                                0x0000ffff
#define NV_PAPU_FESENOTIFY_VOICE__BITSHIFT                                      0
#define NV_PAPU_FESENOTIFY_SSLA_COMPLETE                                        0x00010000
#define NV_PAPU_FESENOTIFY_SSLA_COMPLETE__BITSHIFT                              16
#define NV_PAPU_FESENOTIFY_SSLA_COMPLETE_NO                                     0
#define NV_PAPU_FESENOTIFY_SSLA_COMPLETE_YES                                    1
#define NV_PAPU_FESENOTIFY_SSLB_COMPLETE                                        0x00020000
#define NV_PAPU_FESENOTIFY_SSLB_COMPLETE__BITSHIFT                              17
#define NV_PAPU_FESENOTIFY_SSLB_COMPLETE_NO                                     0
#define NV_PAPU_FESENOTIFY_SSLB_COMPLETE_YES                                    1
#define NV_PAPU_FESENOTIFY_ENVELOPE_ACTIVE                                      0x00040000
#define NV_PAPU_FESENOTIFY_ENVELOPE_ACTIVE__BITSHIFT                            18
#define NV_PAPU_FESENOTIFY_ENVELOPE_ACTIVE_NO                                   0
#define NV_PAPU_FESENOTIFY_ENVELOPE_ACTIVE_YES                                  1
#define NV_PAPU_FESENOTIFY_SAMPLES_ACTIVE                                       0x00080000
#define NV_PAPU_FESENOTIFY_SAMPLES_ACTIVE__BITSHIFT                             19
#define NV_PAPU_FESENOTIFY_SAMPLES_ACTIVE_NO                                    0
#define NV_PAPU_FESENOTIFY_SAMPLES_ACTIVE_YES                                   1
#define NV_PAPU_FESENOTIFY_INTERRUPT_ENABLE                                     0x00100000
#define NV_PAPU_FESENOTIFY_INTERRUPT_ENABLE__BITSHIFT                           20
#define NV_PAPU_FESENOTIFY_INTERRUPT_ENABLE_NO                                  0
#define NV_PAPU_FESENOTIFY_INTERRUPT_ENABLE_YES                                 1
#define NV_PAPU_FESENOTIFY_BITS                                                 0x001fffff
#define NV_PAPU_FESENOTIFY_BITS__BITSHIFT                                       0
        NV_PAPU_FEUFIFOCTL                          = 0x00001340,
#define NV_PAPU_FEUFIFOCTL_COUNT                                                0x0000003f
#define NV_PAPU_FEUFIFOCTL_COUNT__BITSHIFT                                      0
#define NV_PAPU_FEUFIFOCTL_HEAD                                                 0x00001f00
#define NV_PAPU_FEUFIFOCTL_HEAD__BITSHIFT                                       8
#define NV_PAPU_FEUFIFOCTL_HEAD_LSB                                             0x00000100
#define NV_PAPU_FEUFIFOCTL_HEAD_LSB__BITSHIFT                                   8
#define NV_PAPU_FEUFIFOCTL_TAIL                                                 0x001f0000
#define NV_PAPU_FEUFIFOCTL_TAIL__BITSHIFT                                       16
#define NV_PAPU_FEUFIFOCTL_BITS                                                 0x001fffff
#define NV_PAPU_FEUFIFOCTL_BITS__BITSHIFT                                       0
        NV_PAPU_FEUFIFOMETH_0                       = 0x00001400,
        NV_PAPU_FEUFIFOMETH_1                       = 0x00001408,
        NV_PAPU_FEUFIFOMETH_2                       = 0x00001410,
        NV_PAPU_FEUFIFOMETH_3                       = 0x00001418,
        NV_PAPU_FEUFIFOMETH_4                       = 0x00001420,
        NV_PAPU_FEUFIFOMETH_5                       = 0x00001428,
        NV_PAPU_FEUFIFOMETH_6                       = 0x00001430,
        NV_PAPU_FEUFIFOMETH_7                       = 0x00001438,
        NV_PAPU_FEUFIFOMETH_8                       = 0x00001440,
        NV_PAPU_FEUFIFOMETH_9                       = 0x00001448,
        NV_PAPU_FEUFIFOMETH_10                      = 0x00001450,
        NV_PAPU_FEUFIFOMETH_11                      = 0x00001458,
        NV_PAPU_FEUFIFOMETH_12                      = 0x00001460,
        NV_PAPU_FEUFIFOMETH_13                      = 0x00001468,
        NV_PAPU_FEUFIFOMETH_14                      = 0x00001470,
        NV_PAPU_FEUFIFOMETH_15                      = 0x00001478,
        NV_PAPU_FEUFIFOMETH_16                      = 0x00001480,
        NV_PAPU_FEUFIFOMETH_17                      = 0x00001488,
        NV_PAPU_FEUFIFOMETH_18                      = 0x00001490,
        NV_PAPU_FEUFIFOMETH_19                      = 0x00001498,
        NV_PAPU_FEUFIFOMETH_20                      = 0x000014a0,
        NV_PAPU_FEUFIFOMETH_21                      = 0x000014a8,
        NV_PAPU_FEUFIFOMETH_22                      = 0x000014b0,
        NV_PAPU_FEUFIFOMETH_23                      = 0x000014b8,
        NV_PAPU_FEUFIFOMETH_24                      = 0x000014c0,
        NV_PAPU_FEUFIFOMETH_25                      = 0x000014c8,
        NV_PAPU_FEUFIFOMETH_26                      = 0x000014d0,
        NV_PAPU_FEUFIFOMETH_27                      = 0x000014d8,
        NV_PAPU_FEUFIFOMETH_28                      = 0x000014e0,
        NV_PAPU_FEUFIFOMETH_29                      = 0x000014e8,
        NV_PAPU_FEUFIFOMETH_30                      = 0x000014f0,
        NV_PAPU_FEUFIFOMETH_31                      = 0x000014f8,
#define NV_PAPU_FEUFIFOMETH_ADDR_BITS                                           0x000000ff
#define NV_PAPU_FEUFIFOMETH_ADDR_BITS__BITSHIFT                                 0
#define NV_PAPU_FEUFIFOMETH_ADDR_SAVED_BITS                                     0x000000fc
#define NV_PAPU_FEUFIFOMETH_ADDR_SAVED_BITS__BITSHIFT                           2
        NV_PAPU_FEUFIFOPARAM_0                      = 0x00001404,
        NV_PAPU_FEUFIFOPARAM_1                      = 0x0000140c,
        NV_PAPU_FEUFIFOPARAM_2                      = 0x00001414,
        NV_PAPU_FEUFIFOPARAM_3                      = 0x0000141c,
        NV_PAPU_FEUFIFOPARAM_4                      = 0x00001424,
        NV_PAPU_FEUFIFOPARAM_5                      = 0x0000142c,
        NV_PAPU_FEUFIFOPARAM_6                      = 0x00001434,
        NV_PAPU_FEUFIFOPARAM_7                      = 0x0000143c,
        NV_PAPU_FEUFIFOPARAM_8                      = 0x00001444,
        NV_PAPU_FEUFIFOPARAM_9                      = 0x0000144c,
        NV_PAPU_FEUFIFOPARAM_10                     = 0x00001454,
        NV_PAPU_FEUFIFOPARAM_11                     = 0x0000145c,
        NV_PAPU_FEUFIFOPARAM_12                     = 0x00001464,
        NV_PAPU_FEUFIFOPARAM_13                     = 0x0000146c,
        NV_PAPU_FEUFIFOPARAM_14                     = 0x00001474,
        NV_PAPU_FEUFIFOPARAM_15                     = 0x0000147c,
        NV_PAPU_FEUFIFOPARAM_16                     = 0x00001484,
        NV_PAPU_FEUFIFOPARAM_17                     = 0x0000148c,
        NV_PAPU_FEUFIFOPARAM_18                     = 0x00001494,
        NV_PAPU_FEUFIFOPARAM_19                     = 0x0000149c,
        NV_PAPU_FEUFIFOPARAM_20                     = 0x000014a4,
        NV_PAPU_FEUFIFOPARAM_21                     = 0x000014ac,
        NV_PAPU_FEUFIFOPARAM_22                     = 0x000014b4,
        NV_PAPU_FEUFIFOPARAM_23                     = 0x000014bc,
        NV_PAPU_FEUFIFOPARAM_24                     = 0x000014c4,
        NV_PAPU_FEUFIFOPARAM_25                     = 0x000014cc,
        NV_PAPU_FEUFIFOPARAM_26                     = 0x000014d4,
        NV_PAPU_FEUFIFOPARAM_27                     = 0x000014dc,
        NV_PAPU_FEUFIFOPARAM_28                     = 0x000014e4,
        NV_PAPU_FEUFIFOPARAM_29                     = 0x000014ec,
        NV_PAPU_FEUFIFOPARAM_30                     = 0x000014f4,
        NV_PAPU_FEUFIFOPARAM_31                     = 0x000014fc,
        NV_PAPU_FETFORCE0                           = 0x00001500,
#define NV_PAPU_FETFORCE0_NOP                                                   0x00000001
#define NV_PAPU_FETFORCE0_NOP__BITSHIFT                                         0
#define NV_PAPU_FETFORCE0_SYNCHRONIZE                                           0x00000002
#define NV_PAPU_FETFORCE0_SYNCHRONIZE__BITSHIFT                                 1
#define NV_PAPU_FETFORCE0_SET_TIME                                              0x00000004
#define NV_PAPU_FETFORCE0_SET_TIME__BITSHIFT                                    2
#define NV_PAPU_FETFORCE0_SET_MODE                                              0x00000008
#define NV_PAPU_FETFORCE0_SET_MODE__BITSHIFT                                    3
#define NV_PAPU_FETFORCE0_SET_CURRENT_VOICE                                     0x00000010
#define NV_PAPU_FETFORCE0_SET_CURRENT_VOICE__BITSHIFT                           4
#define NV_PAPU_FETFORCE0_SET_ANTECEDENT_VOICE                                  0x00000020
#define NV_PAPU_FETFORCE0_SET_ANTECEDENT_VOICE__BITSHIFT                        5
#define NV_PAPU_FETFORCE0_VOICE_ON                                              0x00000040
#define NV_PAPU_FETFORCE0_VOICE_ON__BITSHIFT                                    6
#define NV_PAPU_FETFORCE0_VOICE_OFF                                             0x00000080
#define NV_PAPU_FETFORCE0_VOICE_OFF__BITSHIFT                                   7
#define NV_PAPU_FETFORCE0_VOICE_RELEASE                                         0x00000100
#define NV_PAPU_FETFORCE0_VOICE_RELEASE__BITSHIFT                               8
#define NV_PAPU_FETFORCE0_VOICE_PAUSE                                           0x00000200
#define NV_PAPU_FETFORCE0_VOICE_PAUSE__BITSHIFT                                 9
#define NV_PAPU_FETFORCE0_GET_VOICE_POSITION                                    0x00000400
#define NV_PAPU_FETFORCE0_GET_VOICE_POSITION__BITSHIFT                          10
#define NV_PAPU_FETFORCE0_SET_SUBMIX_HEADROOM                                   0x00000800
#define NV_PAPU_FETFORCE0_SET_SUBMIX_HEADROOM__BITSHIFT                         11
#define NV_PAPU_FETFORCE0_SET_HRTF_SUBMIXES                                     0x00001000
#define NV_PAPU_FETFORCE0_SET_HRTF_SUBMIXES__BITSHIFT                           12
#define NV_PAPU_FETFORCE0_SET_HRTF_HEADROOM                                     0x00002000
#define NV_PAPU_FETFORCE0_SET_HRTF_HEADROOM__BITSHIFT                           13
#define NV_PAPU_FETFORCE0_SET_VOLUME_TRACKING                                   0x00004000
#define NV_PAPU_FETFORCE0_SET_VOLUME_TRACKING__BITSHIFT                         14
#define NV_PAPU_FETFORCE0_SET_PITCH_TRACKING                                    0x00008000
#define NV_PAPU_FETFORCE0_SET_PITCH_TRACKING__BITSHIFT                          15
#define NV_PAPU_FETFORCE0_SET_HRTF_TRACKING                                     0x00010000
#define NV_PAPU_FETFORCE0_SET_HRTF_TRACKING__BITSHIFT                           16
#define NV_PAPU_FETFORCE0_SET_ITD_TRACKING                                      0x00020000
#define NV_PAPU_FETFORCE0_SET_ITD_TRACKING__BITSHIFT                            17
#define NV_PAPU_FETFORCE0_SET_FILTER_TRACKING                                   0x00040000
#define NV_PAPU_FETFORCE0_SET_FILTER_TRACKING__BITSHIFT                         18
#define NV_PAPU_FETFORCE0_VOICE_LOCK                                            0x00080000
#define NV_PAPU_FETFORCE0_VOICE_LOCK__BITSHIFT                                  19
#define NV_PAPU_FETFORCE0_SET_VOICE_CFG_VBIN                                    0x00100000
#define NV_PAPU_FETFORCE0_SET_VOICE_CFG_VBIN__BITSHIFT                          20
#define NV_PAPU_FETFORCE0_SET_VOICE_CFG_FMT                                     0x00200000
#define NV_PAPU_FETFORCE0_SET_VOICE_CFG_FMT__BITSHIFT                           21
#define NV_PAPU_FETFORCE0_SET_VOICE_CFG_OTHER                                   0x00400000
#define NV_PAPU_FETFORCE0_SET_VOICE_CFG_OTHER__BITSHIFT                         22
#define NV_PAPU_FETFORCE0_SET_VOICE_TAR_HRTF                                    0x00800000
#define NV_PAPU_FETFORCE0_SET_VOICE_TAR_HRTF__BITSHIFT                          23
#define NV_PAPU_FETFORCE0_SET_VOICE_PAR_LFODLY                                  0x01000000
#define NV_PAPU_FETFORCE0_SET_VOICE_PAR_LFODLY__BITSHIFT                        24
#define NV_PAPU_FETFORCE0_SET_VOICE_SSL                                         0x02000000
#define NV_PAPU_FETFORCE0_SET_VOICE_SSL__BITSHIFT                               25
#define NV_PAPU_FETFORCE0_SET_VOICE_CFG_BUF                                     0x04000000
#define NV_PAPU_FETFORCE0_SET_VOICE_CFG_BUF__BITSHIFT                           26
#define NV_PAPU_FETFORCE0_SET_VOICE_TAR_VOL                                     0x08000000
#define NV_PAPU_FETFORCE0_SET_VOICE_TAR_VOL__BITSHIFT                           27
#define NV_PAPU_FETFORCE0_SET_VOICE_LFO                                         0x10000000
#define NV_PAPU_FETFORCE0_SET_VOICE_LFO__BITSHIFT                               28
#define NV_PAPU_FETFORCE0_SET_VOICE_TAR_FC                                      0x20000000
#define NV_PAPU_FETFORCE0_SET_VOICE_TAR_FC__BITSHIFT                            29
#define NV_PAPU_FETFORCE0_SET_VOICE_TAR_PITCH                                   0x40000000
#define NV_PAPU_FETFORCE0_SET_VOICE_TAR_PITCH__BITSHIFT                         30
#define NV_PAPU_FETFORCE0_BITS                                                  0x7fffffff
#define NV_PAPU_FETFORCE0_BITS__BITSHIFT                                        0
        NV_PAPU_FETFORCE1                           = 0x00001504,
#define NV_PAPU_FETFORCE1_SET_CURRENT_HRTF_ENTRY                                0x00000001
#define NV_PAPU_FETFORCE1_SET_CURRENT_HRTF_ENTRY__BITSHIFT                      0
#define NV_PAPU_FETFORCE1_SET_HRIR                                              0x00000002
#define NV_PAPU_FETFORCE1_SET_HRIR__BITSHIFT                                    1
#define NV_PAPU_FETFORCE1_SET_HRIR_X                                            0x00000004
#define NV_PAPU_FETFORCE1_SET_HRIR_X__BITSHIFT                                  2
#define NV_PAPU_FETFORCE1_SET_CURRENT_SSL_CONTEXT_DMA                           0x00000008
#define NV_PAPU_FETFORCE1_SET_CURRENT_SSL_CONTEXT_DMA__BITSHIFT                 3
#define NV_PAPU_FETFORCE1_SET_CURRENT_SSL                                       0x00000010
#define NV_PAPU_FETFORCE1_SET_CURRENT_SSL__BITSHIFT                             4
#define NV_PAPU_FETFORCE1_SET_SSL_SEGMENT_OFFSET                                0x00000020
#define NV_PAPU_FETFORCE1_SET_SSL_SEGMENT_OFFSET__BITSHIFT                      5
#define NV_PAPU_FETFORCE1_SET_SSL_SEGMENT_LENGTH                                0x00000040
#define NV_PAPU_FETFORCE1_SET_SSL_SEGMENT_LENGTH__BITSHIFT                      6
#define NV_PAPU_FETFORCE1_SET_CURRENT_INBUF_SGE_CONTEXT_DMA                     0x00000080
#define NV_PAPU_FETFORCE1_SET_CURRENT_INBUF_SGE_CONTEXT_DMA__BITSHIFT           7
#define NV_PAPU_FETFORCE1_SET_CURRENT_INBUF_SGE                                 0x00000100
#define NV_PAPU_FETFORCE1_SET_CURRENT_INBUF_SGE__BITSHIFT                       8
#define NV_PAPU_FETFORCE1_SET_CURRENT_INBUF_SGE_OFFSET                          0x00000200
#define NV_PAPU_FETFORCE1_SET_CURRENT_INBUF_SGE_OFFSET__BITSHIFT                9
#define NV_PAPU_FETFORCE1_SET_CURRENT_BUFFER_SGE_CONTEXT_DMA                    0x00000400
#define NV_PAPU_FETFORCE1_SET_CURRENT_BUFFER_SGE_CONTEXT_DMA__BITSHIFT          10
#define NV_PAPU_FETFORCE1_SET_CURRENT_OUTBUF_SGE                                0x00000800
#define NV_PAPU_FETFORCE1_SET_CURRENT_OUTBUF_SGE__BITSHIFT                      11
#define NV_PAPU_FETFORCE1_SET_OUTBUF_BA                                         0x00001000
#define NV_PAPU_FETFORCE1_SET_OUTBUF_BA__BITSHIFT                               12
#define NV_PAPU_FETFORCE1_SET_OUTBUF_LEN                                        0x00002000
#define NV_PAPU_FETFORCE1_SET_OUTBUF_LEN__BITSHIFT                              13
#define NV_PAPU_FETFORCE1_SET_CURRENT_OUTBUF_SGE_OFFSET                         0x00004000
#define NV_PAPU_FETFORCE1_SET_CURRENT_OUTBUF_SGE_OFFSET__BITSHIFT               14
#define NV_PAPU_FETFORCE1_SE2FE_IDLE_VOICE                                      0x00008000
#define NV_PAPU_FETFORCE1_SE2FE_IDLE_VOICE__BITSHIFT                            15
#define NV_PAPU_FETFORCE1_SE2FE_NOTIFY                                          0x00010000
#define NV_PAPU_FETFORCE1_SE2FE_NOTIFY__BITSHIFT                                16
#define NV_PAPU_FETFORCE1_SE2FE_RTINTR                                          0x00020000
#define NV_PAPU_FETFORCE1_SE2FE_RTINTR__BITSHIFT                                17
#define NV_PAPU_FETFORCE1_UNKNOWN                                               0x00040000
#define NV_PAPU_FETFORCE1_UNKNOWN__BITSHIFT                                     18
#define NV_PAPU_FETFORCE1_BITS                                                  0x0007ffff
#define NV_PAPU_FETFORCE1_BITS__BITSHIFT                                        0
        NV_PAPU_FETIGNORE0                          = 0x00001508,
#define NV_PAPU_FETIGNORE0_NOP                                                  0x00000001
#define NV_PAPU_FETIGNORE0_NOP__BITSHIFT                                        0
#define NV_PAPU_FETIGNORE0_SYNCHRONIZE                                          0x00000002
#define NV_PAPU_FETIGNORE0_SYNCHRONIZE__BITSHIFT                                1
#define NV_PAPU_FETIGNORE0_SET_TIME                                             0x00000004
#define NV_PAPU_FETIGNORE0_SET_TIME__BITSHIFT                                   2
#define NV_PAPU_FETIGNORE0_SET_MODE                                             0x00000008
#define NV_PAPU_FETIGNORE0_SET_MODE__BITSHIFT                                   3
#define NV_PAPU_FETIGNORE0_SET_CURRENT_VOICE                                    0x00000010
#define NV_PAPU_FETIGNORE0_SET_CURRENT_VOICE__BITSHIFT                          4
#define NV_PAPU_FETIGNORE0_SET_ANTECEDENT_VOICE                                 0x00000020
#define NV_PAPU_FETIGNORE0_SET_ANTECEDENT_VOICE__BITSHIFT                       5
#define NV_PAPU_FETIGNORE0_VOICE_ON                                             0x00000040
#define NV_PAPU_FETIGNORE0_VOICE_ON__BITSHIFT                                   6
#define NV_PAPU_FETIGNORE0_VOICE_OFF                                            0x00000080
#define NV_PAPU_FETIGNORE0_VOICE_OFF__BITSHIFT                                  7
#define NV_PAPU_FETIGNORE0_VOICE_RELEASE                                        0x00000100
#define NV_PAPU_FETIGNORE0_VOICE_RELEASE__BITSHIFT                              8
#define NV_PAPU_FETIGNORE0_VOICE_PAUSE                                          0x00000200
#define NV_PAPU_FETIGNORE0_VOICE_PAUSE__BITSHIFT                                9
#define NV_PAPU_FETIGNORE0_GET_VOICE_POSITION                                   0x00000400
#define NV_PAPU_FETIGNORE0_GET_VOICE_POSITION__BITSHIFT                         10
#define NV_PAPU_FETIGNORE0_SET_SUBMIX_HEADROOM                                  0x00000800
#define NV_PAPU_FETIGNORE0_SET_SUBMIX_HEADROOM__BITSHIFT                        11
#define NV_PAPU_FETIGNORE0_SET_HRTF_SUBMIXES                                    0x00001000
#define NV_PAPU_FETIGNORE0_SET_HRTF_SUBMIXES__BITSHIFT                          12
#define NV_PAPU_FETIGNORE0_SET_HRTF_HEADROOM                                    0x00002000
#define NV_PAPU_FETIGNORE0_SET_HRTF_HEADROOM__BITSHIFT                          13
#define NV_PAPU_FETIGNORE0_SET_VOLUME_TRACKING                                  0x00004000
#define NV_PAPU_FETIGNORE0_SET_VOLUME_TRACKING__BITSHIFT                        14
#define NV_PAPU_FETIGNORE0_SET_PITCH_TRACKING                                   0x00008000
#define NV_PAPU_FETIGNORE0_SET_PITCH_TRACKING__BITSHIFT                         15
#define NV_PAPU_FETIGNORE0_SET_HRTF_TRACKING                                    0x00010000
#define NV_PAPU_FETIGNORE0_SET_HRTF_TRACKING__BITSHIFT                          16
#define NV_PAPU_FETIGNORE0_SET_ITD_TRACKING                                     0x00020000
#define NV_PAPU_FETIGNORE0_SET_ITD_TRACKING__BITSHIFT                           17
#define NV_PAPU_FETIGNORE0_SET_FILTER_TRACKING                                  0x00040000
#define NV_PAPU_FETIGNORE0_SET_FILTER_TRACKING__BITSHIFT                        18
#define NV_PAPU_FETIGNORE0_VOICE_LOCK                                           0x00080000
#define NV_PAPU_FETIGNORE0_VOICE_LOCK__BITSHIFT                                 19
#define NV_PAPU_FETIGNORE0_SET_VOICE_CFG_VBIN                                   0x00100000
#define NV_PAPU_FETIGNORE0_SET_VOICE_CFG_VBIN__BITSHIFT                         20
#define NV_PAPU_FETIGNORE0_SET_VOICE_CFG_FMT                                    0x00200000
#define NV_PAPU_FETIGNORE0_SET_VOICE_CFG_FMT__BITSHIFT                          21
#define NV_PAPU_FETIGNORE0_SET_VOICE_CFG_OTHER                                  0x00400000
#define NV_PAPU_FETIGNORE0_SET_VOICE_CFG_OTHER__BITSHIFT                        22
#define NV_PAPU_FETIGNORE0_SET_VOICE_TAR_HRTF                                   0x00800000
#define NV_PAPU_FETIGNORE0_SET_VOICE_TAR_HRTF__BITSHIFT                         23
#define NV_PAPU_FETIGNORE0_SET_VOICE_PAR_LFODLY                                 0x01000000
#define NV_PAPU_FETIGNORE0_SET_VOICE_PAR_LFODLY__BITSHIFT                       24
#define NV_PAPU_FETIGNORE0_SET_VOICE_SSL                                        0x02000000
#define NV_PAPU_FETIGNORE0_SET_VOICE_SSL__BITSHIFT                              25
#define NV_PAPU_FETIGNORE0_SET_VOICE_CFG_BUF                                    0x04000000
#define NV_PAPU_FETIGNORE0_SET_VOICE_CFG_BUF__BITSHIFT                          26
#define NV_PAPU_FETIGNORE0_SET_VOICE_TAR_VOL                                    0x08000000
#define NV_PAPU_FETIGNORE0_SET_VOICE_TAR_VOL__BITSHIFT                          27
#define NV_PAPU_FETIGNORE0_SET_VOICE_LFO                                        0x10000000
#define NV_PAPU_FETIGNORE0_SET_VOICE_LFO__BITSHIFT                              28
#define NV_PAPU_FETIGNORE0_SET_VOICE_TAR_FC                                     0x20000000
#define NV_PAPU_FETIGNORE0_SET_VOICE_TAR_FC__BITSHIFT                           29
#define NV_PAPU_FETIGNORE0_SET_VOICE_TAR_PITCH                                  0x40000000
#define NV_PAPU_FETIGNORE0_SET_VOICE_TAR_PITCH__BITSHIFT                        30
#define NV_PAPU_FETIGNORE0_BITS                                                 0x7fffffff
#define NV_PAPU_FETIGNORE0_BITS__BITSHIFT                                       0
        NV_PAPU_FETIGNORE1                          = 0x0000150c,
#define NV_PAPU_FETIGNORE1_SET_CURRENT_HRTF_ENTRY                               0x00000001
#define NV_PAPU_FETIGNORE1_SET_CURRENT_HRTF_ENTRY__BITSHIFT                     0
#define NV_PAPU_FETIGNORE1_SET_HRIR                                             0x00000002
#define NV_PAPU_FETIGNORE1_SET_HRIR__BITSHIFT                                   1
#define NV_PAPU_FETIGNORE1_SET_HRIR_X                                           0x00000004
#define NV_PAPU_FETIGNORE1_SET_HRIR_X__BITSHIFT                                 2
#define NV_PAPU_FETIGNORE1_SET_CURRENT_SSL_CONTEXT_DMA                          0x00000008
#define NV_PAPU_FETIGNORE1_SET_CURRENT_SSL_CONTEXT_DMA__BITSHIFT                3
#define NV_PAPU_FETIGNORE1_SET_CURRENT_SSL                                      0x00000010
#define NV_PAPU_FETIGNORE1_SET_CURRENT_SSL__BITSHIFT                            4
#define NV_PAPU_FETIGNORE1_SET_SSL_SEGMENT_OFFSET                               0x00000020
#define NV_PAPU_FETIGNORE1_SET_SSL_SEGMENT_OFFSET__BITSHIFT                     5
#define NV_PAPU_FETIGNORE1_SET_SSL_SEGMENT_LENGTH                               0x00000040
#define NV_PAPU_FETIGNORE1_SET_SSL_SEGMENT_LENGTH__BITSHIFT                     6
#define NV_PAPU_FETIGNORE1_SET_CURRENT_INBUF_SGE_CONTEXT_DMA                    0x00000080
#define NV_PAPU_FETIGNORE1_SET_CURRENT_INBUF_SGE_CONTEXT_DMA__BITSHIFT          7
#define NV_PAPU_FETIGNORE1_SET_CURRENT_INBUF_SGE                                0x00000100
#define NV_PAPU_FETIGNORE1_SET_CURRENT_INBUF_SGE__BITSHIFT                      8
#define NV_PAPU_FETIGNORE1_SET_CURRENT_INBUF_SGE_OFFSET                         0x00000200
#define NV_PAPU_FETIGNORE1_SET_CURRENT_INBUF_SGE_OFFSET__BITSHIFT               9
#define NV_PAPU_FETIGNORE1_SET_CURRENT_BUFFER_SGE_CONTEXT_DMA                   0x00000400
#define NV_PAPU_FETIGNORE1_SET_CURRENT_BUFFER_SGE_CONTEXT_DMA__BITSHIFT         10
#define NV_PAPU_FETIGNORE1_SET_CURRENT_OUTBUF_SGE                               0x00000800
#define NV_PAPU_FETIGNORE1_SET_CURRENT_OUTBUF_SGE__BITSHIFT                     11
#define NV_PAPU_FETIGNORE1_SET_OUTBUF_BA                                        0x00001000
#define NV_PAPU_FETIGNORE1_SET_OUTBUF_BA__BITSHIFT                              12
#define NV_PAPU_FETIGNORE1_SET_OUTBUF_LEN                                       0x00002000
#define NV_PAPU_FETIGNORE1_SET_OUTBUF_LEN__BITSHIFT                             13
#define NV_PAPU_FETIGNORE1_SET_CURRENT_OUTBUF_SGE_OFFSET                        0x00004000
#define NV_PAPU_FETIGNORE1_SET_CURRENT_OUTBUF_SGE_OFFSET__BITSHIFT              14
#define NV_PAPU_FETIGNORE1_SE2FE_IDLE_VOICE                                     0x00008000
#define NV_PAPU_FETIGNORE1_SE2FE_IDLE_VOICE__BITSHIFT                           15
#define NV_PAPU_FETIGNORE1_UNKNOWN                                              0x00040000
#define NV_PAPU_FETIGNORE1_UNKNOWN__BITSHIFT                                    18
#define NV_PAPU_FETIGNORE1_TRAPREASON_NOT_IMPLEMENTED                           0x00400000
#define NV_PAPU_FETIGNORE1_TRAPREASON_NOT_IMPLEMENTED__BITSHIFT                 22
#define NV_PAPU_FETIGNORE1_TRAPREASON_METHOD_UNKNOWN                            0x00800000
#define NV_PAPU_FETIGNORE1_TRAPREASON_METHOD_UNKNOWN__BITSHIFT                  23
#define NV_PAPU_FETIGNORE1_TRAPREASON_BAD_ARGUMENT                              0x01000000
#define NV_PAPU_FETIGNORE1_TRAPREASON_BAD_ARGUMENT__BITSHIFT                    24
#define NV_PAPU_FETIGNORE1_TRAPREASON_CURRENT_NOT_SET                           0x02000000
#define NV_PAPU_FETIGNORE1_TRAPREASON_CURRENT_NOT_SET__BITSHIFT                 25
#define NV_PAPU_FETIGNORE1_TRAPREASON_ANTECEDENT_NOT_SET                        0x04000000
#define NV_PAPU_FETIGNORE1_TRAPREASON_ANTECEDENT_NOT_SET__BITSHIFT              26
#define NV_PAPU_FETIGNORE1_TRAPREASON_BAD_DATA_TYPE                             0x10000000
#define NV_PAPU_FETIGNORE1_TRAPREASON_BAD_DATA_TYPE__BITSHIFT                   28
#define NV_PAPU_FETIGNORE1_TRAPREASON_BAD_LIST_POINTER                          0x20000000
#define NV_PAPU_FETIGNORE1_TRAPREASON_BAD_LIST_POINTER__BITSHIFT                29
#define NV_PAPU_FETIGNORE1_TRAPREASON_CURRENT_VOICE_NOT_3D                      0x40000000
#define NV_PAPU_FETIGNORE1_TRAPREASON_CURRENT_VOICE_NOT_3D__BITSHIFT            30
#define NV_PAPU_FETIGNORE1_TRAPREASON_CTXPA_INVALID                             0x80000000
#define NV_PAPU_FETIGNORE1_TRAPREASON_CTXPA_INVALID__BITSHIFT                   31
        NV_PAPU_FEPRIVLOCK                          = 0x00001510,
#define NV_PAPU_FEPRIVLOCK_VALUE                                                0x00000001
#define NV_PAPU_FEPRIVLOCK_VALUE__BITSHIFT                                      0
#define NV_PAPU_FEPRIVLOCK_VALUE_UNLOCKED                                       0
#define NV_PAPU_FEPRIVLOCK_VALUE_LOCKED                                         1
#define NV_PAPU_FEPRIVLOCK_BITS                                                 0x00000001
#define NV_PAPU_FEPRIVLOCK_BITS__BITSHIFT                                       0
        NV_PAPU_SECTL                               = 0x00002000,
#define NV_PAPU_SECTL_SEPRTYP                                                   0x00000001
#define NV_PAPU_SECTL_SEPRTYP__BITSHIFT                                         0
#define NV_PAPU_SECTL_SEPRTYP_NON_ISO                                           0
#define NV_PAPU_SECTL_SEPRTYP_ISO                                               1
#define NV_PAPU_SECTL_SEPWTYP                                                   0x00000002
#define NV_PAPU_SECTL_SEPWTYP__BITSHIFT                                         1
#define NV_PAPU_SECTL_SEPWTYP_NON_ISO                                           0
#define NV_PAPU_SECTL_SEPWTYP_ISO                                               1
#define NV_PAPU_SECTL_SESRTYP                                                   0x00000004
#define NV_PAPU_SECTL_SESRTYP__BITSHIFT                                         2
#define NV_PAPU_SECTL_SESRTYP_NON_ISO                                           0
#define NV_PAPU_SECTL_SESRTYP_ISO                                               1
#define NV_PAPU_SECTL_XCNTMODE                                                  0x00000018
#define NV_PAPU_SECTL_XCNTMODE__BITSHIFT                                        3
#define NV_PAPU_SECTL_XCNTMODE_OFF                                              0
#define NV_PAPU_SECTL_XCNTMODE_AC_SYNC                                          1
#define NV_PAPU_SECTL_XCNTMODE_SW                                               2
#define NV_PAPU_SECTL_XCNTMODE_FREE_RUNNING                                     3
        NV_PAPU_STID                                = 0x00002004,
#define NV_PAPU_STID_SESTART                                                    0x00000001
#define NV_PAPU_STID_SESTART__BITSHIFT                                          0
#define NV_PAPU_STID_SESTART_NOT_STARTING                                       0
#define NV_PAPU_STID_SESTART_STARTING                                           1
#define NV_PAPU_STID_SEIDLE                                                     0x00000002
#define NV_PAPU_STID_SEIDLE__BITSHIFT                                           1
#define NV_PAPU_STID_SEIDLE_IDLE                                                1
#define NV_PAPU_STID_SEIDLE_NOT_IDLE                                            0
#define NV_PAPU_STID_VPSTART                                                    0x00000004
#define NV_PAPU_STID_VPSTART__BITSHIFT                                          2
#define NV_PAPU_STID_VPSTART_NOT_STARTING                                       0
#define NV_PAPU_STID_VPSTART_STARTING                                           1
#define NV_PAPU_STID_VPIDLE                                                     0x00000008
#define NV_PAPU_STID_VPIDLE__BITSHIFT                                           3
#define NV_PAPU_STID_VPIDLE_IDLE                                                1
#define NV_PAPU_STID_VPIDLE_NOT_IDLE                                            0
#define NV_PAPU_STID_GPSTART                                                    0x00000010
#define NV_PAPU_STID_GPSTART__BITSHIFT                                          4
#define NV_PAPU_STID_GPSTART_NOT_STARTING                                       0
#define NV_PAPU_STID_GPSTART_STARTING                                           1
#define NV_PAPU_STID_GPIDLE                                                     0x00000020
#define NV_PAPU_STID_GPIDLE__BITSHIFT                                           5
#define NV_PAPU_STID_GPIDLE_IDLE                                                1
#define NV_PAPU_STID_GPIDLE_NOT_IDLE                                            0
#define NV_PAPU_STID_EPSTART                                                    0x00000040
#define NV_PAPU_STID_EPSTART__BITSHIFT                                          6
#define NV_PAPU_STID_EPSTART_NOT_STARTING                                       0
#define NV_PAPU_STID_EPSTART_STARTING                                           1
#define NV_PAPU_STID_EPIDLE                                                     0x00000080
#define NV_PAPU_STID_EPIDLE__BITSHIFT                                           7
#define NV_PAPU_STID_EPIDLE_IDLE                                                1
#define NV_PAPU_STID_EPIDLE_NOT_IDLE                                            0
        NV_PAPU_IGSCNT                              = 0x00002008,
#define NV_PAPU_IGSCNT_INIT                                                     0
        NV_PAPU_XGSCNT                              = 0x0000200c,
#define NV_PAPU_XGSCNT_INIT                                                     0xffffffff
        NV_PAPU_DGSCNT                              = 0x00002010,
#define NV_PAPU_DGSCNT_INIT                                                     0
        NV_PAPU_WGSCNT                              = 0x00002014,
#define NV_PAPU_WGSCNT_INIT                                                     0
        NV_PAPU_RGSCNT                              = 0x00002018,
#define NV_PAPU_RGSCNT_INIT                                                     0
        NV_PAPU_PGSCNT                              = 0x0000201c,
#define NV_PAPU_PGSCNT_INIT                                                     0
        NV_PAPU_EGSCNT                              = 0x00002020,
#define NV_PAPU_EGSCNT_INIT                                                     0
        NV_PAPU_DEGSCNT                             = 0x00002024,
#define NV_PAPU_DEGSCNT_INIT                                                    0
        NV_PAPU_ECNTSTP                             = 0x00002028,
#define NV_PAPU_ECNTSTP_VALUE                                                   0x0000ffff
#define NV_PAPU_ECNTSTP_VALUE__BITSHIFT                                         0
        NV_PAPU_VPVADDR                             = 0x0000202c,
#define NV_PAPU_VPVADDR_VALUE                                                   0xffffc000
#define NV_PAPU_VPVADDR_VALUE__BITSHIFT                                         14
        NV_PAPU_VPSGEADDR                           = 0x00002030,
#define NV_PAPU_VPSGEADDR_VALUE                                                 0xffffc000
#define NV_PAPU_VPSGEADDR_VALUE__BITSHIFT                                       14
        NV_PAPU_VPSSLADDR                           = 0x00002034,
#define NV_PAPU_VPSSLADDR_VALUE                                                 0xffffc000
#define NV_PAPU_VPSSLADDR_VALUE__BITSHIFT                                       14
        NV_PAPU_VPHTADDR                            = 0x00002038,
#define NV_PAPU_VPHTADDR_VALUE                                                  0xffffc000
#define NV_PAPU_VPHTADDR_VALUE__BITSHIFT                                        14
        NV_PAPU_VPHCADDR                            = 0x0000203c,
#define NV_PAPU_VPHCADDR_VALUE                                                  0xffffc000
#define NV_PAPU_VPHCADDR_VALUE__BITSHIFT                                        14
        NV_PAPU_GPSADDR                             = 0x00002040,
#define NV_PAPU_GPSADDR_VALUE                                                   0xffffc000
#define NV_PAPU_GPSADDR_VALUE__BITSHIFT                                         14
        NV_PAPU_GPFADDR                             = 0x00002044,
#define NV_PAPU_GPFADDR_VALUE                                                   0xffffc000
#define NV_PAPU_GPFADDR_VALUE__BITSHIFT                                         14
        NV_PAPU_EPSADDR                             = 0x00002048,
#define NV_PAPU_EPSADDR_VALUE                                                   0xffffc000
#define NV_PAPU_EPSADDR_VALUE__BITSHIFT                                         14
        NV_PAPU_EPFADDR                             = 0x0000204c,
#define NV_PAPU_EPFADDR_VALUE                                                   0xffffc000
#define NV_PAPU_EPFADDR_VALUE__BITSHIFT                                         14
        NV_PAPU_VLOCK                               = 0x00002050,
#define NV_PAPU_VLOCK_VOICE                                                     0x0000ffff
#define NV_PAPU_VLOCK_VOICE__BITSHIFT                                           0
#define NV_PAPU_VLOCK_REQ                                                       0x00010000
#define NV_PAPU_VLOCK_REQ__BITSHIFT                                             16
#define NV_PAPU_VLOCK_REQ_LOCK                                                  1
#define NV_PAPU_VLOCK_REQ_UNLOCK                                                0
#define NV_PAPU_VLOCK_STATUS                                                    0x00020000
#define NV_PAPU_VLOCK_STATUS__BITSHIFT                                          17
#define NV_PAPU_VLOCK_STATUS_NOT_IN_PROGRESS                                    0
#define NV_PAPU_VLOCK_STATUS_IN_PROGRESS                                        1
        NV_PAPU_TVL2D                               = 0x00002054,
#define NV_PAPU_TVL2D_VALUE                                                     0x0000ffff
#define NV_PAPU_TVL2D_VALUE__BITSHIFT                                           0
#define NV_PAPU_TVL2D_VALUE_NULL                                                0x0000ffff
        NV_PAPU_CVL2D                               = 0x00002058,
#define NV_PAPU_CVL2D_VALUE                                                     0x0000ffff
#define NV_PAPU_CVL2D_VALUE__BITSHIFT                                           0
#define NV_PAPU_CVL2D_VALUE_NULL                                                0x0000ffff
        NV_PAPU_NVL2D                               = 0x0000205c,
#define NV_PAPU_NVL2D_VALUE                                                     0x0000ffff
#define NV_PAPU_NVL2D_VALUE__BITSHIFT                                           0
#define NV_PAPU_NVL2D_VALUE_NULL                                                0x0000ffff
        NV_PAPU_TVL3D                               = 0x00002060,
#define NV_PAPU_TVL3D_VALUE                                                     0x0000ffff
#define NV_PAPU_TVL3D_VALUE__BITSHIFT                                           0
#define NV_PAPU_TVL3D_VALUE_NULL                                                0x0000ffff
        NV_PAPU_CVL3D                               = 0x00002064,
#define NV_PAPU_CVL3D_VALUE                                                     0x0000ffff
#define NV_PAPU_CVL3D_VALUE__BITSHIFT                                           0
#define NV_PAPU_CVL3D_VALUE_NULL                                                0x0000ffff
        NV_PAPU_NVL3D                               = 0x00002068,
#define NV_PAPU_NVL3D_VALUE                                                     0x0000ffff
#define NV_PAPU_NVL3D_VALUE__BITSHIFT                                           0
#define NV_PAPU_NVL3D_VALUE_NULL                                                0x0000ffff
        NV_PAPU_TVLMP                               = 0x0000206c,
#define NV_PAPU_TVLMP_VALUE                                                     0x0000ffff
#define NV_PAPU_TVLMP_VALUE__BITSHIFT                                           0
#define NV_PAPU_TVLMP_VALUE_NULL                                                0x0000ffff
        NV_PAPU_CVLMP                               = 0x00002070,
#define NV_PAPU_CVLMP_VALUE                                                     0x0000ffff
#define NV_PAPU_CVLMP_VALUE__BITSHIFT                                           0
#define NV_PAPU_CVLMP_VALUE_NULL                                                0x0000ffff
        NV_PAPU_NVLMP                               = 0x00002074,
#define NV_PAPU_NVLMP_VALUE                                                     0x0000ffff
#define NV_PAPU_NVLMP_VALUE__BITSHIFT                                           0
#define NV_PAPU_NVLMP_VALUE_NULL                                                0x0000ffff
        NV_PAPU_ALPHAV                              = 0x00002078,
#define NV_PAPU_ALPHAV_VALUE                                                    0x00000fff
#define NV_PAPU_ALPHAV_VALUE__BITSHIFT                                          0
        NV_PAPU_ALPHAP                              = 0x0000207c,
#define NV_PAPU_ALPHAP_VALUE                                                    0x00000fff
#define NV_PAPU_ALPHAP_VALUE__BITSHIFT                                          0
        NV_PAPU_ALPHAF                              = 0x00002080,
#define NV_PAPU_ALPHAF_VALUE                                                    0x00000fff
#define NV_PAPU_ALPHAF_VALUE__BITSHIFT                                          0
        NV_PAPU_ALPHAI                              = 0x00002084,
#define NV_PAPU_ALPHAI_VALUE                                                    0x00000fff
#define NV_PAPU_ALPHAI_VALUE__BITSHIFT                                          0
        NV_PAPU_ALPHAH                              = 0x00002088,
#define NV_PAPU_ALPHAH_VALUE                                                    0x00000fff
#define NV_PAPU_ALPHAH_VALUE__BITSHIFT                                          0
        NV_PAPU_MIXHRA                              = 0x0000208c,
#define NV_PAPU_MIXHRA_MHR0                                                     0x00000007
#define NV_PAPU_MIXHRA_MHR0__BITSHIFT                                           0
#define NV_PAPU_MIXHRA_MHR0_INIT                                                0
#define NV_PAPU_MIXHRA_MHR1                                                     0x00000700
#define NV_PAPU_MIXHRA_MHR1__BITSHIFT                                           8
#define NV_PAPU_MIXHRA_MHR1_INIT                                                0
#define NV_PAPU_MIXHRA_MHR2                                                     0x00070000
#define NV_PAPU_MIXHRA_MHR2__BITSHIFT                                           16
#define NV_PAPU_MIXHRA_MHR2_INIT                                                0
#define NV_PAPU_MIXHRA_MHR3                                                     0x07000000
#define NV_PAPU_MIXHRA_MHR3__BITSHIFT                                           24
#define NV_PAPU_MIXHRA_MHR3_INIT                                                0
        NV_PAPU_MIXHRB                              = 0x00002090,
#define NV_PAPU_MIXHRB_MHR4                                                     0x00000007
#define NV_PAPU_MIXHRB_MHR4__BITSHIFT                                           0
#define NV_PAPU_MIXHRB_MHR4_INIT                                                0
#define NV_PAPU_MIXHRB_MHR5                                                     0x00000700
#define NV_PAPU_MIXHRB_MHR5__BITSHIFT                                           8
#define NV_PAPU_MIXHRB_MHR5_INIT                                                0
#define NV_PAPU_MIXHRB_MHR6                                                     0x00070000
#define NV_PAPU_MIXHRB_MHR6__BITSHIFT                                           16
#define NV_PAPU_MIXHRB_MHR6_INIT                                                0
#define NV_PAPU_MIXHRB_MHR7                                                     0x07000000
#define NV_PAPU_MIXHRB_MHR7__BITSHIFT                                           24
#define NV_PAPU_MIXHRB_MHR7_INIT                                                0
        NV_PAPU_MIXHRC                              = 0x00002094,
#define NV_PAPU_MIXHRC_MHR8                                                     0x00000007
#define NV_PAPU_MIXHRC_MHR8__BITSHIFT                                           0
#define NV_PAPU_MIXHRC_MHR8_INIT                                                0
#define NV_PAPU_MIXHRC_MHR9                                                     0x00000700
#define NV_PAPU_MIXHRC_MHR9__BITSHIFT                                           8
#define NV_PAPU_MIXHRC_MHR9_INIT                                                0
#define NV_PAPU_MIXHRC_MHR10                                                    0x00070000
#define NV_PAPU_MIXHRC_MHR10__BITSHIFT                                          16
#define NV_PAPU_MIXHRC_MHR10_INIT                                               0
#define NV_PAPU_MIXHRC_MHR11                                                    0x07000000
#define NV_PAPU_MIXHRC_MHR11__BITSHIFT                                          24
#define NV_PAPU_MIXHRC_MHR11_INIT                                               0
        NV_PAPU_MIXHRD                              = 0x00002098,
#define NV_PAPU_MIXHRD_MHR12                                                    0x00000007
#define NV_PAPU_MIXHRD_MHR12__BITSHIFT                                          0
#define NV_PAPU_MIXHRD_MHR12_INIT                                               0
#define NV_PAPU_MIXHRD_MHR13                                                    0x00000700
#define NV_PAPU_MIXHRD_MHR13__BITSHIFT                                          8
#define NV_PAPU_MIXHRD_MHR13_INIT                                               0
#define NV_PAPU_MIXHRD_MHR14                                                    0x00070000
#define NV_PAPU_MIXHRD_MHR14__BITSHIFT                                          16
#define NV_PAPU_MIXHRD_MHR14_INIT                                               0
#define NV_PAPU_MIXHRD_MHR15                                                    0x07000000
#define NV_PAPU_MIXHRD_MHR15__BITSHIFT                                          24
#define NV_PAPU_MIXHRD_MHR15_INIT                                               0
        NV_PAPU_MIXHRE                              = 0x0000209c,
#define NV_PAPU_MIXHRE_MHR16                                                    0x00000007
#define NV_PAPU_MIXHRE_MHR16__BITSHIFT                                          0
#define NV_PAPU_MIXHRE_MHR16_INIT                                               0
#define NV_PAPU_MIXHRE_MHR17                                                    0x00000700
#define NV_PAPU_MIXHRE_MHR17__BITSHIFT                                          8
#define NV_PAPU_MIXHRE_MHR17_INIT                                               0
#define NV_PAPU_MIXHRE_MHR18                                                    0x00070000
#define NV_PAPU_MIXHRE_MHR18__BITSHIFT                                          16
#define NV_PAPU_MIXHRE_MHR18_INIT                                               0
#define NV_PAPU_MIXHRE_MHR19                                                    0x07000000
#define NV_PAPU_MIXHRE_MHR19__BITSHIFT                                          24
        NV_PAPU_MIXHRF                              = 0x000020a0,
#define NV_PAPU_MIXHRF_MHR19_INIT                                               0
#define NV_PAPU_MIXHRF_MHR20                                                    0x00000007
#define NV_PAPU_MIXHRF_MHR20__BITSHIFT                                          0
#define NV_PAPU_MIXHRF_MHR20_INIT                                               0
#define NV_PAPU_MIXHRF_MHR21                                                    0x00000700
#define NV_PAPU_MIXHRF_MHR21__BITSHIFT                                          8
#define NV_PAPU_MIXHRF_MHR21_INIT                                               0
#define NV_PAPU_MIXHRF_MHR22                                                    0x00070000
#define NV_PAPU_MIXHRF_MHR22__BITSHIFT                                          16
#define NV_PAPU_MIXHRF_MHR22_INIT                                               0
#define NV_PAPU_MIXHRF_MHR23                                                    0x07000000
#define NV_PAPU_MIXHRF_MHR23__BITSHIFT                                          24
#define NV_PAPU_MIXHRF_MHR23_INIT                                               0
        NV_PAPU_MIXHRG                              = 0x000020a4,
#define NV_PAPU_MIXHRG_MHR24                                                    0x00000007
#define NV_PAPU_MIXHRG_MHR24__BITSHIFT                                          0
#define NV_PAPU_MIXHRG_MHR24_INIT                                               0
#define NV_PAPU_MIXHRG_MHR25                                                    0x00000700
#define NV_PAPU_MIXHRG_MHR25__BITSHIFT                                          8
#define NV_PAPU_MIXHRG_MHR25_INIT                                               0
#define NV_PAPU_MIXHRG_MHR26                                                    0x00070000
#define NV_PAPU_MIXHRG_MHR26__BITSHIFT                                          16
#define NV_PAPU_MIXHRG_MHR26_INIT                                               0
#define NV_PAPU_MIXHRG_MHR27                                                    0x07000000
#define NV_PAPU_MIXHRG_MHR27__BITSHIFT                                          24
#define NV_PAPU_MIXHRG_MHR27_INIT                                               0
        NV_PAPU_MIXHRH                              = 0x000020a8,
#define NV_PAPU_MIXHRH_MHR28                                                    0x00000007
#define NV_PAPU_MIXHRH_MHR28__BITSHIFT                                          0
#define NV_PAPU_MIXHRH_MHR28_INIT                                               0
#define NV_PAPU_MIXHRH_MHR29                                                    0x00000700
#define NV_PAPU_MIXHRH_MHR29__BITSHIFT                                          8
#define NV_PAPU_MIXHRH_MHR29_INIT                                               0
#define NV_PAPU_MIXHRH_MHR30                                                    0x00070000
#define NV_PAPU_MIXHRH_MHR30__BITSHIFT                                          16
#define NV_PAPU_MIXHRH_MHR30_INIT                                               0
#define NV_PAPU_MIXHRH_MHR31                                                    0x07000000
#define NV_PAPU_MIXHRH_MHR31__BITSHIFT                                          24
#define NV_PAPU_MIXHRH_MHR31_INIT                                               0
        NV_PAPU_MIX3D                               = 0x000020ac,
#define NV_PAPU_MIX3D_SMX3DFL                                                   0x0000001f
#define NV_PAPU_MIX3D_SMX3DFL__BITSHIFT                                         0
#define NV_PAPU_MIX3D_SMX3DFL_INIT                                              0
#define NV_PAPU_MIX3D_SMX3DFR                                                   0x00001f00
#define NV_PAPU_MIX3D_SMX3DFR__BITSHIFT                                         8
#define NV_PAPU_MIX3D_SMX3DFR_INIT                                              0
#define NV_PAPU_MIX3D_SMX3DRL                                                   0x001f0000
#define NV_PAPU_MIX3D_SMX3DRL__BITSHIFT                                         16
#define NV_PAPU_MIX3D_SMX3DRL_INIT                                              0
#define NV_PAPU_MIX3D_SMX3DRR                                                   0x1f000000
#define NV_PAPU_MIX3D_SMX3DRR__BITSHIFT                                         24
#define NV_PAPU_MIX3D_SMX3DRR_INIT                                              0
        NV_PAPU_TDHR                                = 0x000020b0,
#define NV_PAPU_TDHR_TDHR                                                       0x00000007
#define NV_PAPU_TDHR_TDHR__BITSHIFT                                             0
#define NV_PAPU_TDHR_TDHR_INIT                                                  0
        NV_PAPU_AVGSESTI                            = 0x000020b4,
        NV_PAPU_WCSESTI                             = 0x000020b8,
        NV_PAPU_CURSESTI                            = 0x000020bc,
        NV_PAPU_CNTSESTI                            = 0x000020c0,
#define NV_PAPU_CNTSESTI_VALUE                                                  0x000000ff
#define NV_PAPU_CNTSESTI_VALUE__BITSHIFT                                        0
        NV_PAPU_AVGGPSTI                            = 0x000020c4,
        NV_PAPU_WCGPSTI                             = 0x000020c8,
        NV_PAPU_CURGPSTI                            = 0x000020cc,
        NV_PAPU_CNTGPSTI                            = 0x000020d0,
#define NV_PAPU_CNTGPSTI_VALUE                                                  0x000000ff
#define NV_PAPU_CNTGPSTI_VALUE__BITSHIFT                                        0
        NV_PAPU_GPSMAXSGE                           = 0x000020d4,
#define NV_PAPU_GPSMAXSGE_VALUE                                                 0x0000ffff
#define NV_PAPU_GPSMAXSGE_VALUE__BITSHIFT                                       0
#define NV_PAPU_GPSMAXSGE_INIT                                                  0
#define NV_PAPU_GPSMAXSGE_BITS                                                  0x0000ffff
#define NV_PAPU_GPSMAXSGE_BITS__BITSHIFT                                        0
        NV_PAPU_GPFMAXSGE                           = 0x000020d8,
#define NV_PAPU_GPFMAXSGE_VALUE                                                 0x0000ffff
#define NV_PAPU_GPFMAXSGE_VALUE__BITSHIFT                                       0
#define NV_PAPU_GPFMAXSGE_INIT                                                  0
#define NV_PAPU_GPFMAXSGE_BITS                                                  0x0000ffff
#define NV_PAPU_GPFMAXSGE_BITS__BITSHIFT                                        0
        NV_PAPU_EPSMAXSGE                           = 0x000020dc,
#define NV_PAPU_EPSMAXSGE_VALUE                                                 0x0000ffff
#define NV_PAPU_EPSMAXSGE_VALUE__BITSHIFT                                       0
#define NV_PAPU_EPSMAXSGE_INIT                                                  0
#define NV_PAPU_EPSMAXSGE_BITS                                                  0x0000ffff
#define NV_PAPU_EPSMAXSGE_BITS__BITSHIFT                                        0
        NV_PAPU_EPFMAXSGE                           = 0x000020e0,
#define NV_PAPU_EPFMAXSGE_VALUE                                                 0x0000ffff
#define NV_PAPU_EPFMAXSGE_VALUE__BITSHIFT                                       0
#define NV_PAPU_EPFMAXSGE_INIT                                                  0
#define NV_PAPU_EPFMAXSGE_BITS                                                  0x0000ffff
#define NV_PAPU_EPFMAXSGE_BITS__BITSHIFT                                        0
        NV_PAPU_GPNADDR                             = 0x00003004,
#define NV_PAPU_GPNADDR_VALUE                                                   0xfffff000
#define NV_PAPU_GPNADDR_VALUE__BITSHIFT                                         12
        NV_PAPU_GPCADDR                             = 0x00003014,
#define NV_PAPU_GPCADDR_VALUE                                                   0xfffff000
#define NV_PAPU_GPCADDR_VALUE__BITSHIFT                                         12
        NV_PAPU_GPOFBASE0                           = 0x00003024,
#define NV_PAPU_GPOFBASE0_VALUE                                                 0x00ffff00
#define NV_PAPU_GPOFBASE0_VALUE__BITSHIFT                                       8
        NV_PAPU_GPOFBASE1                           = 0x00003034,
#define NV_PAPU_GPOFBASE1_VALUE                                                 0x00ffff00
#define NV_PAPU_GPOFBASE1_VALUE__BITSHIFT                                       8
        NV_PAPU_GPOFBASE2                           = 0x00003044,
#define NV_PAPU_GPOFBASE2_VALUE                                                 0x00ffff00
#define NV_PAPU_GPOFBASE2_VALUE__BITSHIFT                                       8
        NV_PAPU_GPOFBASE3                           = 0x00003054,
#define NV_PAPU_GPOFBASE3_VALUE                                                 0x00ffff00
#define NV_PAPU_GPOFBASE3_VALUE__BITSHIFT                                       8
        NV_PAPU_GPOFEND0                            = 0x00003028,
#define NV_PAPU_GPOFEND0_VALUE                                                  0x00ffff00
#define NV_PAPU_GPOFEND0_VALUE__BITSHIFT                                        8
        NV_PAPU_GPOFEND1                            = 0x00003038,
#define NV_PAPU_GPOFEND1_VALUE                                                  0x00ffff00
#define NV_PAPU_GPOFEND1_VALUE__BITSHIFT                                        8
        NV_PAPU_GPOFEND2                            = 0x00003048,
#define NV_PAPU_GPOFEND2_VALUE                                                  0x00ffff00
#define NV_PAPU_GPOFEND2_VALUE__BITSHIFT                                        8
        NV_PAPU_GPOFEND3                            = 0x00003058,
#define NV_PAPU_GPOFEND3_VALUE                                                  0x00ffff00
#define NV_PAPU_GPOFEND3_VALUE__BITSHIFT                                        8
        NV_PAPU_GPOFCUR0                            = 0x0000302c,
#define NV_PAPU_GPOFCUR0_VALUE                                                  0x00fffffc
#define NV_PAPU_GPOFCUR0_VALUE__BITSHIFT                                        2
        NV_PAPU_GPOFCUR1                            = 0x0000303c,
#define NV_PAPU_GPOFCUR1_VALUE                                                  0x00fffffc
#define NV_PAPU_GPOFCUR1_VALUE__BITSHIFT                                        2
        NV_PAPU_GPOFCUR2                            = 0x0000304c,
#define NV_PAPU_GPOFCUR2_VALUE                                                  0x00fffffc
#define NV_PAPU_GPOFCUR2_VALUE__BITSHIFT                                        2
        NV_PAPU_GPOFCUR3                            = 0x0000305c,
#define NV_PAPU_GPOFCUR3_VALUE                                                  0x00fffffc
#define NV_PAPU_GPOFCUR3_VALUE__BITSHIFT                                        2
        NV_PAPU_GPIFBASE0                           = 0x00003064,
#define NV_PAPU_GPIFBASE0_VALUE                                                 0x00ffff00
#define NV_PAPU_GPIFBASE0_VALUE__BITSHIFT                                       8
        NV_PAPU_GPIFBASE1                           = 0x00003074,
#define NV_PAPU_GPIFBASE1_VALUE                                                 0x00ffff00
#define NV_PAPU_GPIFBASE1_VALUE__BITSHIFT                                       8
        NV_PAPU_GPIFEND0                            = 0x00003068,
#define NV_PAPU_GPIFEND0_VALUE                                                  0x00ffff00
#define NV_PAPU_GPIFEND0_VALUE__BITSHIFT                                        8
        NV_PAPU_GPIFEND1                            = 0x00003078,
#define NV_PAPU_GPIFEND1_VALUE                                                  0x00ffff00
#define NV_PAPU_GPIFEND1_VALUE__BITSHIFT                                        8
        NV_PAPU_GPIFCUR0                            = 0x0000306c,
#define NV_PAPU_GPIFCUR0_VALUE                                                  0x00fffffc
#define NV_PAPU_GPIFCUR0_VALUE__BITSHIFT                                        2
        NV_PAPU_GPIFCUR1                            = 0x0000307c,
#define NV_PAPU_GPIFCUR1_VALUE                                                  0x00fffffc
#define NV_PAPU_GPIFCUR1_VALUE__BITSHIFT                                        2
        NV_PAPU_EPNADDR                             = 0x00004004,
#define NV_PAPU_EPNADDR_VALUE                                                   0xfffff000
#define NV_PAPU_EPNADDR_VALUE__BITSHIFT                                         12
        NV_PAPU_EPCADDR                             = 0x00004014,
#define NV_PAPU_EPCADDR_VALUE                                                   0xfffff000
#define NV_PAPU_EPCADDR_VALUE__BITSHIFT                                         12
        NV_PAPU_EPOFBASE0                           = 0x00004024,
#define NV_PAPU_EPOFBASE0_VALUE                                                 0x00ffff00
#define NV_PAPU_EPOFBASE0_VALUE__BITSHIFT                                       8
        NV_PAPU_EPOFBASE1                           = 0x00004034,
#define NV_PAPU_EPOFBASE1_VALUE                                                 0x00ffff00
#define NV_PAPU_EPOFBASE1_VALUE__BITSHIFT                                       8
        NV_PAPU_EPOFBASE2                           = 0x00004044,
#define NV_PAPU_EPOFBASE2_VALUE                                                 0x00ffff00
#define NV_PAPU_EPOFBASE2_VALUE__BITSHIFT                                       8
        NV_PAPU_EPOFBASE3                           = 0x00004054,
#define NV_PAPU_EPOFBASE3_VALUE                                                 0x00ffff00
#define NV_PAPU_EPOFBASE3_VALUE__BITSHIFT                                       8
        NV_PAPU_EPOFEND0                            = 0x00004028,
#define NV_PAPU_EPOFEND0_VALUE                                                  0x00ffff00
#define NV_PAPU_EPOFEND0_VALUE__BITSHIFT                                        8
        NV_PAPU_EPOFEND1                            = 0x00004038,
#define NV_PAPU_EPOFEND1_VALUE                                                  0x00ffff00
#define NV_PAPU_EPOFEND1_VALUE__BITSHIFT                                        8
        NV_PAPU_EPOFEND2                            = 0x00004048,
#define NV_PAPU_EPOFEND2_VALUE                                                  0x00ffff00
#define NV_PAPU_EPOFEND2_VALUE__BITSHIFT                                        8
        NV_PAPU_EPOFEND3                            = 0x00004058,
#define NV_PAPU_EPOFEND3_VALUE                                                  0x00ffff00
#define NV_PAPU_EPOFEND3_VALUE__BITSHIFT                                        8
        NV_PAPU_EPOFCUR0                            = 0x0000402c,
#define NV_PAPU_EPOFCUR0_VALUE                                                  0x00fffffc
#define NV_PAPU_EPOFCUR0_VALUE__BITSHIFT                                        2
        NV_PAPU_EPOFCUR1                            = 0x0000403c,
#define NV_PAPU_EPOFCUR1_VALUE                                                  0x00fffffc
#define NV_PAPU_EPOFCUR1_VALUE__BITSHIFT                                        2
        NV_PAPU_EPOFCUR2                            = 0x0000404c,
#define NV_PAPU_EPOFCUR2_VALUE                                                  0x00fffffc
#define NV_PAPU_EPOFCUR2_VALUE__BITSHIFT                                        2
        NV_PAPU_EPOFCUR3                            = 0x0000405c,
#define NV_PAPU_EPOFCUR3_VALUE                                                  0x00fffffc
#define NV_PAPU_EPOFCUR3_VALUE__BITSHIFT                                        2
        NV_PAPU_EPIFBASE0                           = 0x00004064,
#define NV_PAPU_EPIFBASE0_VALUE                                                 0x00ffff00
#define NV_PAPU_EPIFBASE0_VALUE__BITSHIFT                                       8
        NV_PAPU_EPIFBASE1                           = 0x00004074,
#define NV_PAPU_EPIFBASE1_VALUE                                                 0x00ffff00
#define NV_PAPU_EPIFBASE1_VALUE__BITSHIFT                                       8
        NV_PAPU_EPIFEND0                            = 0x00004068,
#define NV_PAPU_EPIFEND0_VALUE                                                  0x00ffff00
#define NV_PAPU_EPIFEND0_VALUE__BITSHIFT                                        8
        NV_PAPU_EPIFEND1                            = 0x00004078,
#define NV_PAPU_EPIFEND1_VALUE                                                  0x00ffff00
#define NV_PAPU_EPIFEND1_VALUE__BITSHIFT                                        8
        NV_PAPU_EPIFCUR0                            = 0x0000406c,
#define NV_PAPU_EPIFCUR0_VALUE                                                  0x00fffffc
#define NV_PAPU_EPIFCUR0_VALUE__BITSHIFT                                        2
        NV_PAPU_EPIFCUR1                            = 0x0000407c,
#define NV_PAPU_EPIFCUR1_VALUE                                                  0x00fffffc
#define NV_PAPU_EPIFCUR1_VALUE__BITSHIFT                                        2
};

static const char *const apu_papu_name[] = {
#if 0
NV_PAPU_BITS                                                            0x0007ffff
NV_PAPU_BITS__BITSHIFT                                                  0
NV_PAPU_W                                                               19
NV_PAPU_SAVED_BITS                                                      0x0007fffc
NV_PAPU_SAVED_BITS__BITSHIFT                                            2
NV_PAPU_SAVED_W                                                         17
NV_PAPU_SUBSPACE_BITS                                                   0x00070000
NV_PAPU_SUBSPACE_BITS__BITSHIFT                                         16
NV_PAPU_SUBSPACE_PRIV                                                   0
NV_PAPU_SUBSPACE_NV1BA0                                                 2
NV_PAPU_SUBSPACE_GP                                                     3
NV_PAPU_SUBSPACE_NV1BXX                                                 4
NV_PAPU_SUBSPACE_EP                                                     5
NV_PAPU_PRIV_BITS                                                       0x0000ffff
NV_PAPU_PRIV_BITS__BITSHIFT                                             0
NV_PAPU_PRIV_SAVED_BITS                                                 0x0000fffc
NV_PAPU_PRIV_SAVED_BITS__BITSHIFT                                       2
NV_PAPU_PRIV_SAVED_W                                                    14
NV_PAPU_PRIV_SUBUNIT_BITS                                               0x0000f000
NV_PAPU_PRIV_SUBUNIT_BITS__BITSHIFT                                     12
NV_PAPU_PRIV_SUBUNIT_CFG                                                0
NV_PAPU_PRIV_SUBUNIT_FE                                                 1
NV_PAPU_PRIV_SUBUNIT_SE                                                 2
NV_PAPU_PRIV_SUBUNIT_SE_GP                                              3
NV_PAPU_PRIV_SUBUNIT_SE_EP                                              4
NV_PAPU_NV1BA0_BITS                                                     0x0000ffff
NV_PAPU_NV1BA0_BITS__BITSHIFT                                           0
#endif
    NAMER(NV_PAPU_ISTS),
#if 0
NV_PAPU_ISTS_GINTSTS                                                    0x00000001
NV_PAPU_ISTS_GINTSTS__BITSHIFT                                          0
NV_PAPU_ISTS_GINTSTS_NO_INTR                                            0
NV_PAPU_ISTS_GINTSTS_INTR                                               1
NV_PAPU_ISTS_DWINTSTS                                                   0x00000002
NV_PAPU_ISTS_DWINTSTS__BITSHIFT                                         1
NV_PAPU_ISTS_DWINTSTS_NO_INTR                                           0
NV_PAPU_ISTS_DWINTSTS_INTR                                              1
NV_PAPU_ISTS_DWINTSTS_CLR                                               1
NV_PAPU_ISTS_RTWSTS                                                     0x00000004
NV_PAPU_ISTS_RTWSTS__BITSHIFT                                           2
NV_PAPU_ISTS_RTWSTS_NO_INTR                                             0
NV_PAPU_ISTS_RTWSTS_INTR                                                1
NV_PAPU_ISTS_DPINTSTS                                                   0x00000008
NV_PAPU_ISTS_DPINTSTS__BITSHIFT                                         3
NV_PAPU_ISTS_DPINTSTS_NO_INTR                                           0
NV_PAPU_ISTS_DPINTSTS_INTR                                              1
NV_PAPU_ISTS_DPINTSTS_CLR                                               1
NV_PAPU_ISTS_FETINTSTS                                                  0x00000010
NV_PAPU_ISTS_FETINTSTS__BITSHIFT                                        4
NV_PAPU_ISTS_FETINTSTS_NO_INTR                                          0
NV_PAPU_ISTS_FETINTSTS_INTR                                             1
NV_PAPU_ISTS_FETINTSTS_CLR                                              1
NV_PAPU_ISTS_FENINTSTS                                                  0x00000020
NV_PAPU_ISTS_FENINTSTS__BITSHIFT                                        5
NV_PAPU_ISTS_FENINTSTS_NO_INTR                                          0
NV_PAPU_ISTS_FENINTSTS_INTR                                             1
NV_PAPU_ISTS_FENINTSTS_CLR                                              1
NV_PAPU_ISTS_FEVINTSTS                                                  0x00000040
NV_PAPU_ISTS_FEVINTSTS__BITSHIFT                                        6
NV_PAPU_ISTS_FEVINTSTS_NO_INTR                                          0
NV_PAPU_ISTS_FEVINTSTS_INTR                                             1
NV_PAPU_ISTS_FEVINTSTS_CLR                                              1
NV_PAPU_ISTS_FEOINTSTS                                                  0x00000080
NV_PAPU_ISTS_FEOINTSTS__BITSHIFT                                        7
NV_PAPU_ISTS_FEOINTSTS_NO_INTR                                          0
NV_PAPU_ISTS_FEOINTSTS_INTR                                             1
NV_PAPU_ISTS_FEOINTSTS_CLR                                              1
NV_PAPU_ISTS_GPMINTSTS                                                  0x00000100
NV_PAPU_ISTS_GPMINTSTS__BITSHIFT                                        8
NV_PAPU_ISTS_GPMINTSTS_NO_INTR                                          0
NV_PAPU_ISTS_GPMINTSTS_INTR                                             1
NV_PAPU_ISTS_GPMINTSTS_CLR                                              1
NV_PAPU_ISTS_GPNINTSTS                                                  0x00000200
NV_PAPU_ISTS_GPNINTSTS__BITSHIFT                                        9
NV_PAPU_ISTS_GPNINTSTS_NO_INTR                                          0
NV_PAPU_ISTS_GPNINTSTS_INTR                                             1
NV_PAPU_ISTS_GPNINTSTS_CLR                                              1
NV_PAPU_ISTS_EPMINTSTS                                                  0x00000400
NV_PAPU_ISTS_EPMINTSTS__BITSHIFT                                        10
NV_PAPU_ISTS_EPMINTSTS_NO_INTR                                          0
NV_PAPU_ISTS_EPMINTSTS_INTR                                             1
NV_PAPU_ISTS_EPMINTSTS_CLR                                              1
NV_PAPU_ISTS_EPNINTSTS                                                  0x00000800
NV_PAPU_ISTS_EPNINTSTS__BITSHIFT                                        11
NV_PAPU_ISTS_EPNINTSTS_NO_INTR                                          0
NV_PAPU_ISTS_EPNINTSTS_INTR                                             1
NV_PAPU_ISTS_EPNINTSTS_CLR                                              1
NV_PAPU_ISTS_BITS                                                       0x00000fff
NV_PAPU_ISTS_BITS__BITSHIFT                                             0
NV_PAPU_ISTS_MOD_BITS                                                   0x00000ffe
NV_PAPU_ISTS_MOD_BITS__BITSHIFT                                         1
NV_PAPU_ISTS_MOD_UPPER_BITS                                             0x00000ff8
NV_PAPU_ISTS_MOD_UPPER_BITS__BITSHIFT                                   3
NV_PAPU_ISTS_MOD_LOWER_BITS                                             0x00000002
NV_PAPU_ISTS_MOD_LOWER_BITS__BITSHIFT                                   1
#endif
    NAMER(NV_PAPU_IEN),
#if 0
NV_PAPU_IEN_GINTEN                                                      0x00000001
NV_PAPU_IEN_GINTEN__BITSHIFT                                            0
NV_PAPU_IEN_GINTEN_DISABLED                                             0
NV_PAPU_IEN_GINTEN_ENABLED                                              1
NV_PAPU_IEN_DWINTEN                                                     0x00000002
NV_PAPU_IEN_DWINTEN__BITSHIFT                                           1
NV_PAPU_IEN_DWINTEN_DISABLED                                            0
NV_PAPU_IEN_DWINTEN_ENABLED                                             1
NV_PAPU_IEN_DPINTEN                                                     0x00000008
NV_PAPU_IEN_DPINTEN__BITSHIFT                                           3
NV_PAPU_IEN_DPINTEN_DISABLED                                            0
NV_PAPU_IEN_DPINTEN_ENABLED                                             1
NV_PAPU_IEN_FETINTEN                                                    0x00000010
NV_PAPU_IEN_FETINTEN__BITSHIFT                                          4
NV_PAPU_IEN_FETINTEN_DISABLED                                           0
NV_PAPU_IEN_FETINTEN_ENABLED                                            1
NV_PAPU_IEN_FENINTEN                                                    0x00000020
NV_PAPU_IEN_FENINTEN__BITSHIFT                                          5
NV_PAPU_IEN_FENINTEN_DISABLED                                           0
NV_PAPU_IEN_FENINTEN_ENABLED                                            1
NV_PAPU_IEN_FEVINTEN                                                    0x00000040
NV_PAPU_IEN_FEVINTEN__BITSHIFT                                          6
NV_PAPU_IEN_FEVINTEN_DISABLED                                           0
NV_PAPU_IEN_FEVINTEN_ENABLED                                            1
NV_PAPU_IEN_FEOINTEN                                                    0x00000080
NV_PAPU_IEN_FEOINTEN__BITSHIFT                                          7
NV_PAPU_IEN_FEOINTEN_DISABLED                                           0
NV_PAPU_IEN_FEOINTEN_ENABLED                                            1
NV_PAPU_IEN_GPMINTEN                                                    0x00000100
NV_PAPU_IEN_GPMINTEN__BITSHIFT                                          8
NV_PAPU_IEN_GPMINTEN_DISABLED                                           0
NV_PAPU_IEN_GPMINTEN_ENABLED                                            1
NV_PAPU_IEN_GPNINTEN                                                    0x00000200
NV_PAPU_IEN_GPNINTEN__BITSHIFT                                          9
NV_PAPU_IEN_GPNINTEN_DISABLED                                           0
NV_PAPU_IEN_GPNINTEN_ENABLED                                            1
NV_PAPU_IEN_EPMINTEN                                                    0x00000400
NV_PAPU_IEN_EPMINTEN__BITSHIFT                                          10
NV_PAPU_IEN_EPMINTEN_DISABLED                                           0
NV_PAPU_IEN_EPMINTEN_ENABLED                                            1
NV_PAPU_IEN_EPNINTEN                                                    0x00000800
NV_PAPU_IEN_EPNINTEN__BITSHIFT                                          11
NV_PAPU_IEN_EPNINTEN_DISABLED                                           0
NV_PAPU_IEN_EPNINTEN_ENABLED                                            1
NV_PAPU_IEN_BITS                                                        0x00000fff
NV_PAPU_IEN_BITS__BITSHIFT                                              0
NV_PAPU_IEN_SETTABLE_MASK                                               0x00000ffb
#endif
    NAMER(NV_PAPU_ISET),
#if 0
NV_PAPU_ISET_DWINTSET                                                   0x00000002
NV_PAPU_ISET_DWINTSET__BITSHIFT                                         1
NV_PAPU_ISET_DWINTSET_NO_INTR                                           0
NV_PAPU_ISET_DWINTSET_INTR                                              1
NV_PAPU_ISET_DPINTSET                                                   0x00000008
NV_PAPU_ISET_DPINTSET__BITSHIFT                                         3
NV_PAPU_ISET_DPINTSET_NO_INTR                                           0
NV_PAPU_ISET_DPINTSET_INTR                                              1
NV_PAPU_ISET_FETINTSET                                                  0x00000010
NV_PAPU_ISET_FETINTSET__BITSHIFT                                        4
NV_PAPU_ISET_FETINTSET_NO_INTR                                          0
NV_PAPU_ISET_FETINTSET_INTR                                             1
NV_PAPU_ISET_FENINTSET                                                  0x00000020
NV_PAPU_ISET_FENINTSET__BITSHIFT                                        5
NV_PAPU_ISET_FENINTSET_NO_INTR                                          0
NV_PAPU_ISET_FENINTSET_INTR                                             1
NV_PAPU_ISET_FEVINTSET                                                  0x00000040
NV_PAPU_ISET_FEVINTSET__BITSHIFT                                        6
NV_PAPU_ISET_FEVINTSET_NO_INTR                                          0
NV_PAPU_ISET_FEVINTSET_INTR                                             1
NV_PAPU_ISET_FEOINTSET                                                  0x00000080
NV_PAPU_ISET_FEOINTSET__BITSHIFT                                        7
NV_PAPU_ISET_FEOINTSET_NO_INTR                                          0
NV_PAPU_ISET_FEOINTSET_INTR                                             1
NV_PAPU_ISET_GPMINTSET                                                  0x00000100
NV_PAPU_ISET_GPMINTSET__BITSHIFT                                        8
NV_PAPU_ISET_GPMINTSET_NO_INTR                                          0
NV_PAPU_ISET_GPMINTSET_INTR                                             1
NV_PAPU_ISET_GPNINTSET                                                  0x00000200
NV_PAPU_ISET_GPNINTSET__BITSHIFT                                        9
NV_PAPU_ISET_GPNINTSET_NO_INTR                                          0
NV_PAPU_ISET_GPNINTSET_INTR                                             1
NV_PAPU_ISET_EPMINTSET                                                  0x00000400
NV_PAPU_ISET_EPMINTSET__BITSHIFT                                        10
NV_PAPU_ISET_EPMINTSET_NO_INTR                                          0
NV_PAPU_ISET_EPMINTSET_INTR                                             1
NV_PAPU_ISET_EPNINTSET                                                  0x00000800
NV_PAPU_ISET_EPNINTSET__BITSHIFT                                        11
NV_PAPU_ISET_EPNINTSET_NO_INTR                                          0
NV_PAPU_ISET_EPNINTSET_INTR                                             1
NV_PAPU_ISET_BITS                                                       0x00000ffe
NV_PAPU_ISET_BITS__BITSHIFT                                             1
NV_PAPU_PRIV_FEMAJ_BITS                                                 0x00000f00
NV_PAPU_PRIV_FEMAJ_BITS__BITSHIFT                                       8
NV_PAPU_PRIV_FEMAJ_INTR                                                 0
NV_PAPU_PRIV_FEMAJ_METH                                                 1
NV_PAPU_PRIV_FEMAJ_PERF                                                 2
NV_PAPU_PRIV_FEMAJ_INTERNAL                                             3
NV_PAPU_PRIV_FEMAJ_UMETHFIFO                                            4
NV_PAPU_PRIV_FEMIN_BITS                                                 0x000000ff
NV_PAPU_PRIV_FEMIN_BITS__BITSHIFT                                       0
#endif
    NAMER(NV_PAPU_FECTL),
#if 0
NV_PAPU_FECTL_FENTYP                                                    0x00000001
NV_PAPU_FECTL_FENTYP__BITSHIFT                                          0
NV_PAPU_FECTL_FENTYP_NON_ISO                                            0
NV_PAPU_FECTL_FENTYP_ISO                                                1
NV_PAPU_FECTL_FEMRTYP                                                   0x00000002
NV_PAPU_FECTL_FEMRTYP__BITSHIFT                                         1
NV_PAPU_FECTL_FEMRTYP_NON_ISO                                           0
NV_PAPU_FECTL_FEMRTYP_ISO                                               1
NV_PAPU_FECTL_FEMWTYP                                                   0x00000004
NV_PAPU_FECTL_FEMWTYP__BITSHIFT                                         2
NV_PAPU_FECTL_FEMWTYP_NON_ISO                                           0
NV_PAPU_FECTL_FEMWTYP_ISO                                               1
NV_PAPU_FECTL_FENINT                                                    0x00000008
NV_PAPU_FECTL_FENINT__BITSHIFT                                          3
NV_PAPU_FECTL_FENINT_DISABLED                                           0
NV_PAPU_FECTL_FENINT_ENABLED                                            1
NV_PAPU_FECTL_FEMETHMODE                                                0x000000e0
NV_PAPU_FECTL_FEMETHMODE__BITSHIFT                                      5
NV_PAPU_FECTL_FEMETHMODE_FREE_RUNNING                                   0
NV_PAPU_FECTL_FEMETHMODE_HALTED                                         4
NV_PAPU_FECTL_FEMETHMODE_TRAPPED                                        7
NV_PAPU_FECTL_FETRAPREASON                                              0x00000f00
NV_PAPU_FECTL_FETRAPREASON__BITSHIFT                                    8
NV_PAPU_FECTL_FETRAPREASON_NONE                                         0
NV_PAPU_FECTL_FETRAPREASON_NOT_IMPLEMENTED                              1
NV_PAPU_FECTL_FETRAPREASON_METHOD_UNKNOWN                               2
NV_PAPU_FECTL_FETRAPREASON_BAD_ARGUMENT                                 3
NV_PAPU_FECTL_FETRAPREASON_CURRENT_NOT_SET                              4
NV_PAPU_FECTL_FETRAPREASON_ANTECEDENT_NOT_SET                           5
NV_PAPU_FECTL_FETRAPREASON_BAD_DATA_TYPE                                7
NV_PAPU_FECTL_FETRAPREASON_BAD_LIST_POINTER                             8
NV_PAPU_FECTL_FETRAPREASON_CURRENT_VOICE_NOT_3D                         9
NV_PAPU_FECTL_FETRAPREASON_CTXPA_INVALID                                10
NV_PAPU_FECTL_FETRAPREASON_REQUESTED                                    15
NV_PAPU_FECTL_FEPIOCLASS                                                0x00001000
NV_PAPU_FECTL_FEPIOCLASS__BITSHIFT                                      12
NV_PAPU_FECTL_FEPIOCLASS_NON_ISO                                        0
NV_PAPU_FECTL_FEPIOCLASS_ISO                                            1
NV_PAPU_FECTL_FEMETH_PRIVLOCK                                           0x00002000
NV_PAPU_FECTL_FEMETH_PRIVLOCK__BITSHIFT                                 13
NV_PAPU_FECTL_FEMETH_PRIVLOCK_NOT_DISABLED                              0
NV_PAPU_FECTL_FEMETH_PRIVLOCK_DISABLED                                  1
NV_PAPU_FECTL_FECVLOCK                                                  0x00004000
NV_PAPU_FECTL_FECVLOCK__BITSHIFT                                        14
NV_PAPU_FECTL_FECVLOCK_UNLOCKED                                         0
NV_PAPU_FECTL_FECVLOCK_LOCKED                                           1
NV_PAPU_FECTL_FEMETHORIGIN                                              0x00008000
NV_PAPU_FECTL_FEMETHORIGIN__BITSHIFT                                    15
NV_PAPU_FECTL_FEMETHORIGIN_USER                                         0
NV_PAPU_FECTL_FEMETHORIGIN_INTERNAL                                     1
NV_PAPU_FECTL_FESESSLCTXPA_VALID                                        0x00010000
NV_PAPU_FECTL_FESESSLCTXPA_VALID__BITSHIFT                              16
NV_PAPU_FECTL_FESESSLCTXPA_VALID_NO                                     0
NV_PAPU_FECTL_FESESSLCTXPA_VALID_YES                                    1
NV_PAPU_FECTL_FESESGECTXPA_VALID                                        0x00020000
NV_PAPU_FECTL_FESESGECTXPA_VALID__BITSHIFT                              17
NV_PAPU_FECTL_FESESGECTXPA_VALID_NO                                     0
NV_PAPU_FECTL_FESESGECTXPA_VALID_YES                                    1
NV_PAPU_FECTL_FEGPSGECTXPA_VALID                                        0x00040000
NV_PAPU_FECTL_FEGPSGECTXPA_VALID__BITSHIFT                              18
NV_PAPU_FECTL_FEGPSGECTXPA_VALID_NO                                     0
NV_PAPU_FECTL_FEGPSGECTXPA_VALID_YES                                    1
NV_PAPU_FECTL_BITS                                                      0x0007ffff
NV_PAPU_FECTL_BITS__BITSHIFT                                            0
NV_PAPU_FECTL_WR_BITS                                                   0x00007fff
NV_PAPU_FECTL_WR_BITS__BITSHIFT                                         0
#endif
    NAMER(NV_PAPU_FEMAXV),
#if 0
NV_PAPU_FEMAXV_VALUE                                                    0x0000ffff
NV_PAPU_FEMAXV_VALUE__BITSHIFT                                          0
NV_PAPU_FEMAXV_INIT                                                     0
NV_PAPU_FEMAXV_BITS                                                     0x0000ffff
NV_PAPU_FEMAXV_BITS__BITSHIFT                                           0
#endif
    NAMER(NV_PAPU_FEMAXTV),
#if 0
NV_PAPU_FEMAXTV_VALUE                                                   0x0000ffff
NV_PAPU_FEMAXTV_VALUE__BITSHIFT                                         0
NV_PAPU_FEMAXTV_INIT                                                    0
NV_PAPU_FEMAXTV_BITS                                                    0x0000ffff
NV_PAPU_FEMAXTV_BITS__BITSHIFT                                          0
#endif
    NAMER(NV_PAPU_FEAMAXV),
#if 0
NV_PAPU_FEAMAXV_VALUE                                                   0x0000ffff
NV_PAPU_FEAMAXV_VALUE__BITSHIFT                                         0
NV_PAPU_FEAMAXV_UNLIMITED                                               0x0000ffff
NV_PAPU_FEAMAXV_BITS                                                    0x0000ffff
NV_PAPU_FEAMAXV_BITS__BITSHIFT                                          0
#endif
    NAMER(NV_PAPU_FECV),
#if 0
NV_PAPU_FECV_VALUE                                                      0x0000ffff
NV_PAPU_FECV_VALUE__BITSHIFT                                            0
NV_PAPU_FECV_VALUE_NULL                                                 0x0000ffff
NV_PAPU_FECV_BITS                                                       0x0000ffff
NV_PAPU_FECV_BITS__BITSHIFT                                             0
#endif
    NAMER(NV_PAPU_FECVDT),
#if 0
NV_PAPU_FECVDT_DATA_TYPE                                                0x00000003
NV_PAPU_FECVDT_DATA_TYPE__BITSHIFT                                      0
NV_PAPU_FECVDT_DATA_TYPE_BUFFER                                         0
NV_PAPU_FECVDT_DATA_TYPE_STREAM                                         1
NV_PAPU_FECVDT_DATA_TYPE_UNCACHED                                       2
NV_PAPU_FECVDT_CONTAINER_SIZE                                           0x0000000c
NV_PAPU_FECVDT_CONTAINER_SIZE__BITSHIFT                                 2
NV_PAPU_FECVDT_CONTAINER_SIZE_B8                                        0
NV_PAPU_FECVDT_CONTAINER_SIZE_B16                                       1
NV_PAPU_FECVDT_CONTAINER_SIZE_ADPCM                                     2
NV_PAPU_FECVDT_CONTAINER_SIZE_B32                                       3
NV_PAPU_FECVDT_BITS                                                     0x0000000f
NV_PAPU_FECVDT_BITS__BITSHIFT                                           0
#endif
    NAMER(NV_PAPU_FEAV),
#if 0
NV_PAPU_FEAV_VALUE                                                      0x0000ffff
NV_PAPU_FEAV_VALUE__BITSHIFT                                            0
NV_PAPU_FEAV_VALUE_NULL                                                 0x0000ffff
NV_PAPU_FEAV_LST                                                        0x00030000
NV_PAPU_FEAV_LST__BITSHIFT                                              16
NV_PAPU_FEAV_LST_INHERIT                                                0
NV_PAPU_FEAV_LST_2D_TOP                                                 1
NV_PAPU_FEAV_LST_3D_TOP                                                 2
NV_PAPU_FEAV_LST_MP_TOP                                                 3
NV_PAPU_FEAV_BITS                                                       0x0003ffff
NV_PAPU_FEAV_BITS__BITSHIFT                                             0
#endif
    NAMER(NV_PAPU_FEMAXHT),
#if 0
NV_PAPU_FEMAXHT_VALUE                                                   0x0000ffff
NV_PAPU_FEMAXHT_VALUE__BITSHIFT                                         0
NV_PAPU_FEMAXHT_INIT                                                    0
NV_PAPU_FEMAXHT_BITS                                                    0x0000ffff
NV_PAPU_FEMAXHT_BITS__BITSHIFT                                          0
#endif
    NAMER(NV_PAPU_FECHT),
#if 0
NV_PAPU_FECHT_VALUE                                                     0x0000ffff
NV_PAPU_FECHT_VALUE__BITSHIFT                                           0
NV_PAPU_FECHT_VALUE_NULL                                                0x0000ffff
NV_PAPU_FECHT_BITS                                                      0x0000ffff
NV_PAPU_FECHT_BITS__BITSHIFT                                            0
#endif
    NAMER(NV_PAPU_FEMAXSESSL),
#if 0
NV_PAPU_FEMAXSESSL_VALUE                                                0x0000ffff
NV_PAPU_FEMAXSESSL_VALUE__BITSHIFT                                      0
NV_PAPU_FEMAXSESSL_INIT                                                 0
NV_PAPU_FEMAXSESSL_BITS                                                 0x0000ffff
NV_PAPU_FEMAXSESSL_BITS__BITSHIFT                                       0
#endif
    NAMER(NV_PAPU_FECSESSL),
#if 0
NV_PAPU_FECSESSL_VALUE                                                  0x0000ffff
NV_PAPU_FECSESSL_VALUE__BITSHIFT                                        0
NV_PAPU_FECSESSL_VALUE_NULL                                             0x0000ffff
NV_PAPU_FECSESSL_BITS                                                   0x0000ffff
NV_PAPU_FECSESSL_BITS__BITSHIFT                                         0
#endif
    NAMER(NV_PAPU_FESESSLCTXPA),
#if 0
NV_PAPU_FESESSLCTXPA_VALUE                                              0xfffff000
NV_PAPU_FESESSLCTXPA_VALUE__BITSHIFT                                    12
#endif
    NAMER(NV_PAPU_FESESSLMAXOFF),
#if 0
NV_PAPU_FESESSLMAXOFF_VALUE                                             0xfffff000
NV_PAPU_FESESSLMAXOFF_VALUE__BITSHIFT                                   12
#endif
    NAMER(NV_PAPU_FESESSLPENDOFF),
    NAMER(NV_PAPU_FEMAXSESGE),
#if 0
NV_PAPU_FEMAXSESGE_VALUE                                                0x0000ffff
NV_PAPU_FEMAXSESGE_VALUE__BITSHIFT                                      0
NV_PAPU_FEMAXSESGE_INIT                                                 0
NV_PAPU_FEMAXSESGE_BITS                                                 0x0000ffff
NV_PAPU_FEMAXSESGE_BITS__BITSHIFT                                       0
#endif
    NAMER(NV_PAPU_FECSESGE),
#if 0
NV_PAPU_FECSESGE_VALUE                                                  0x0000ffff
NV_PAPU_FECSESGE_VALUE__BITSHIFT                                        0
NV_PAPU_FECSESGE_VALUE_NULL                                             0x0000ffff
NV_PAPU_FECSESGE_BITS                                                   0x0000ffff
NV_PAPU_FECSESGE_BITS__BITSHIFT                                         0
#endif
    NAMER(NV_PAPU_FESESGECTXPA),
#if 0
NV_PAPU_FESESGECTXPA_VALUE                                              0xfffff000
NV_PAPU_FESESGECTXPA_VALUE__BITSHIFT                                    12
#endif
    NAMER(NV_PAPU_FESESGEMAXOFF),
#if 0
NV_PAPU_FESESGEMAXOFF_VALUE                                             0xfffff000
NV_PAPU_FESESGEMAXOFF_VALUE__BITSHIFT                                   12
#endif
    NAMER(NV_PAPU_FEMAXGPSGE),
#if 0
NV_PAPU_FEMAXGPSGE_VALUE                                                0x0000ffff
NV_PAPU_FEMAXGPSGE_VALUE__BITSHIFT                                      0
NV_PAPU_FEMAXGPSGE_INIT                                                 0
NV_PAPU_FEMAXGPSGE_BITS                                                 0x0000ffff
NV_PAPU_FEMAXGPSGE_BITS__BITSHIFT                                       0
#endif
    NAMER(NV_PAPU_FECGPSGE),
#if 0
NV_PAPU_FECGPSGE_VALUE                                                  0x0000ffff
NV_PAPU_FECGPSGE_VALUE__BITSHIFT                                        0
NV_PAPU_FECGPSGE_VALUE_NULL                                             0x0000ffff
NV_PAPU_FECGPSGE_BITS                                                   0x0000ffff
NV_PAPU_FECGPSGE_BITS__BITSHIFT                                         0
#endif
    NAMER(NV_PAPU_FEGPSGECTXPA),
#if 0
NV_PAPU_FEGPSGECTXPA_VALUE                                              0xfffff000
NV_PAPU_FEGPSGECTXPA_VALUE__BITSHIFT                                    12
#endif
    NAMER(NV_PAPU_FEGPSGEMAXOFF),
#if 0
NV_PAPU_FEGPSGEMAXOFF_VALUE                                             0xfffff000
NV_PAPU_FEGPSGEMAXOFF_VALUE__BITSHIFT                                   12
#endif
    NAMER(NV_PAPU_FEMAXMB),
#if 0
NV_PAPU_FEMAXMB_VALUE                                                   0x0000003f
NV_PAPU_FEMAXMB_VALUE__BITSHIFT                                         0
NV_PAPU_FEMAXMB_BITS                                                    0x0000003f
NV_PAPU_FEMAXMB_BITS__BITSHIFT                                          0
#endif
    NAMER(NV_PAPU_FENADDR),
    NAMER(NV_PAPU_AVGBW),
#if 0
NV_PAPU_AVGBW_VALUE                                                     0x001fffff
NV_PAPU_AVGBW_VALUE__BITSHIFT                                           0
NV_PAPU_AVGBW_BITS                                                      0x001fffff
NV_PAPU_AVGBW_BITS__BITSHIFT                                            0
#endif
    NAMER(NV_PAPU_WCBW),
#if 0
NV_PAPU_WCBW_VALUE                                                      0x001fffff
NV_PAPU_WCBW_VALUE__BITSHIFT                                            0
NV_PAPU_WCBW_BITS                                                       0x001fffff
NV_PAPU_WCBW_BITS__BITSHIFT                                             0
#endif
    NAMER(NV_PAPU_CURBW),
#if 0
NV_PAPU_CURBW_VALUE                                                     0x001fffff
NV_PAPU_CURBW_VALUE__BITSHIFT                                           0
NV_PAPU_CURBW_BITS                                                      0x001fffff
NV_PAPU_CURBW_BITS__BITSHIFT                                            0
#endif
    NAMER(NV_PAPU_CNTBW),
#if 0
NV_PAPU_CNTBW_VALUE                                                     0x000fffff
NV_PAPU_CNTBW_VALUE__BITSHIFT                                           0
NV_PAPU_CNTBW_BITS                                                      0x000fffff
NV_PAPU_CNTBW_BITS__BITSHIFT                                            0
#endif
    NAMER(NV_PAPU_AVGLAT),
#if 0
NV_PAPU_AVGLAT_VALUE                                                    0x00003fff
NV_PAPU_AVGLAT_VALUE__BITSHIFT                                          0
NV_PAPU_AVGLAT_BITS                                                     0x00003fff
NV_PAPU_AVGLAT_BITS__BITSHIFT                                           0
#endif
    NAMER(NV_PAPU_WCLAT),
#if 0
NV_PAPU_WCLAT_VALUE                                                     0x00003fff
NV_PAPU_WCLAT_VALUE__BITSHIFT                                           0
NV_PAPU_WCLAT_BITS                                                      0x00003fff
NV_PAPU_WCLAT_BITS__BITSHIFT                                            0
#endif
    NAMER(NV_PAPU_CURLAT),
#if 0
NV_PAPU_CURLAT_VALUE                                                    0x00ffffff
NV_PAPU_CURLAT_VALUE__BITSHIFT                                          0
NV_PAPU_CURLAT_BITS                                                     0x00ffffff
NV_PAPU_CURLAT_BITS__BITSHIFT                                           0
NV_PAPU_CURLAT_AVGLAT_BITS                                              0x00fffc00
NV_PAPU_CURLAT_AVGLAT_BITS__BITSHIFT                                    10
#endif
    NAMER(NV_PAPU_CNTLAT),
#if 0
NV_PAPU_CNTLAT_VALUE                                                    0x000003ff
NV_PAPU_CNTLAT_VALUE__BITSHIFT                                          0
NV_PAPU_CNTLAT_VALUE_MAX                                                0x000003ff
NV_PAPU_CNTLAT_BITS                                                     0x000003ff
NV_PAPU_CNTLAT_BITS__BITSHIFT                                           0
#endif
    NAMER(NV_PAPU_FEDECMETH),
#if 0
NV_PAPU_FEDECMETH_METH                                                  0x0000ffff
NV_PAPU_FEDECMETH_METH__BITSHIFT                                        0
NV_PAPU_FEDECMETH_PHASE                                                 0x00ff0000
NV_PAPU_FEDECMETH_PHASE__BITSHIFT                                       16
NV_PAPU_FEDECMETH_PHASE_LAST                                            0x01000000
NV_PAPU_FEDECMETH_PHASE_LAST__BITSHIFT                                  24
NV_PAPU_FEDECMETH_PHASE_LAST_FALSE                                      0
NV_PAPU_FEDECMETH_PHASE_LAST_TRUE                                       1
NV_PAPU_FEDECMETH_VALID                                                 0x02000000
NV_PAPU_FEDECMETH_VALID__BITSHIFT                                       25
NV_PAPU_FEDECMETH_VALID_FALSE                                           0
NV_PAPU_FEDECMETH_VALID_TRUE                                            1
NV_PAPU_FEDECMETH_BITS                                                  0x03ffffff
NV_PAPU_FEDECMETH_BITS__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_FEDECPARAM),
    NAMER(NV_PAPU_FEDECIMMED),
    NAMER(NV_PAPU_FESCRATCH),
    NAMER(NV_PAPU_FESCRATCH2),
    NAMER(NV_PAPU_FESCRATCH3),
    NAMER(NV_PAPU_FESCRATCH4),
    NAMER(NV_PAPU_FEMEMADDR),
    NAMER(NV_PAPU_FEMEMSZ),
#if 0
NV_PAPU_FEMEMSZ_VALUE                                                   0x0000003f
NV_PAPU_FEMEMSZ_VALUE__BITSHIFT                                         0
NV_PAPU_FEMEMSZ_BITS                                                    0x0000003f
NV_PAPU_FEMEMSZ_BITS__BITSHIFT                                          0
#endif
    NAMER(NV_PAPU_FEMEMISO),
#if 0
NV_PAPU_FEMEMISO_WR                                                     0x00000001
NV_PAPU_FEMEMISO_WR__BITSHIFT                                           0
NV_PAPU_FEMEMISO_WR_NON_ISO                                             0
NV_PAPU_FEMEMISO_WR_ISO                                                 1
NV_PAPU_FEMEMISO_RD                                                     0x00000002
NV_PAPU_FEMEMISO_RD__BITSHIFT                                           1
NV_PAPU_FEMEMISO_RD_NON_ISO                                             0
NV_PAPU_FEMEMISO_RD_ISO                                                 1
NV_PAPU_FEMEMISO_BITS                                                   0x00000003
NV_PAPU_FEMEMISO_BITS__BITSHIFT                                         0
#endif
    NAMER(NV_PAPU_FEMEMWE),
#if 0
NV_PAPU_FEMEMWE_VALUE                                                   0x0000000f
NV_PAPU_FEMEMWE_VALUE__BITSHIFT                                         0
NV_PAPU_FEMEMWE_VALUE_ALL                                               15
NV_PAPU_FEMEMWE_BITS                                                    0x0000000f
NV_PAPU_FEMEMWE_BITS__BITSHIFT                                          0
#endif
    NAMER(NV_PAPU_FEMEMDATA),
    NAMER(NV_PAPU_FESEIDLELINKS),
#if 0
NV_PAPU_FESEIDLELINKS_PREV                                              0x0000ffff
NV_PAPU_FESEIDLELINKS_PREV__BITSHIFT                                    0
NV_PAPU_FESEIDLELINKS_PREV_NULL                                         0x0000ffff
NV_PAPU_FESEIDLELINKS_NEXT                                              0xffff0000
NV_PAPU_FESEIDLELINKS_NEXT__BITSHIFT                                    16
NV_PAPU_FESEIDLELINKS_NEXT_NULL                                         0x0000ffff
#endif
    NAMER(NV_PAPU_FESENOTIFY),
#if 0
NV_PAPU_FESENOTIFY_VOICE                                                0x0000ffff
NV_PAPU_FESENOTIFY_VOICE__BITSHIFT                                      0
NV_PAPU_FESENOTIFY_SSLA_COMPLETE                                        0x00010000
NV_PAPU_FESENOTIFY_SSLA_COMPLETE__BITSHIFT                              16
NV_PAPU_FESENOTIFY_SSLA_COMPLETE_NO                                     0
NV_PAPU_FESENOTIFY_SSLA_COMPLETE_YES                                    1
NV_PAPU_FESENOTIFY_SSLB_COMPLETE                                        0x00020000
NV_PAPU_FESENOTIFY_SSLB_COMPLETE__BITSHIFT                              17
NV_PAPU_FESENOTIFY_SSLB_COMPLETE_NO                                     0
NV_PAPU_FESENOTIFY_SSLB_COMPLETE_YES                                    1
NV_PAPU_FESENOTIFY_ENVELOPE_ACTIVE                                      0x00040000
NV_PAPU_FESENOTIFY_ENVELOPE_ACTIVE__BITSHIFT                            18
NV_PAPU_FESENOTIFY_ENVELOPE_ACTIVE_NO                                   0
NV_PAPU_FESENOTIFY_ENVELOPE_ACTIVE_YES                                  1
NV_PAPU_FESENOTIFY_SAMPLES_ACTIVE                                       0x00080000
NV_PAPU_FESENOTIFY_SAMPLES_ACTIVE__BITSHIFT                             19
NV_PAPU_FESENOTIFY_SAMPLES_ACTIVE_NO                                    0
NV_PAPU_FESENOTIFY_SAMPLES_ACTIVE_YES                                   1
NV_PAPU_FESENOTIFY_INTERRUPT_ENABLE                                     0x00100000
NV_PAPU_FESENOTIFY_INTERRUPT_ENABLE__BITSHIFT                           20
NV_PAPU_FESENOTIFY_INTERRUPT_ENABLE_NO                                  0
NV_PAPU_FESENOTIFY_INTERRUPT_ENABLE_YES                                 1
NV_PAPU_FESENOTIFY_BITS                                                 0x001fffff
NV_PAPU_FESENOTIFY_BITS__BITSHIFT                                       0
#endif
    NAMER(NV_PAPU_FEUFIFOCTL),
#if 0
NV_PAPU_FEUFIFOCTL_COUNT                                                0x0000003f
NV_PAPU_FEUFIFOCTL_COUNT__BITSHIFT                                      0
NV_PAPU_FEUFIFOCTL_HEAD                                                 0x00001f00
NV_PAPU_FEUFIFOCTL_HEAD__BITSHIFT                                       8
NV_PAPU_FEUFIFOCTL_HEAD_LSB                                             0x00000100
NV_PAPU_FEUFIFOCTL_HEAD_LSB__BITSHIFT                                   8
NV_PAPU_FEUFIFOCTL_TAIL                                                 0x001f0000
NV_PAPU_FEUFIFOCTL_TAIL__BITSHIFT                                       16
NV_PAPU_FEUFIFOCTL_BITS                                                 0x001fffff
NV_PAPU_FEUFIFOCTL_BITS__BITSHIFT                                       0
#endif
    NAMER(NV_PAPU_FEUFIFOMETH_0),
    NAMER(NV_PAPU_FEUFIFOMETH_1),
    NAMER(NV_PAPU_FEUFIFOMETH_2),
    NAMER(NV_PAPU_FEUFIFOMETH_3),
    NAMER(NV_PAPU_FEUFIFOMETH_4),
    NAMER(NV_PAPU_FEUFIFOMETH_5),
    NAMER(NV_PAPU_FEUFIFOMETH_6),
    NAMER(NV_PAPU_FEUFIFOMETH_7),
    NAMER(NV_PAPU_FEUFIFOMETH_8),
    NAMER(NV_PAPU_FEUFIFOMETH_9),
    NAMER(NV_PAPU_FEUFIFOMETH_10),
    NAMER(NV_PAPU_FEUFIFOMETH_11),
    NAMER(NV_PAPU_FEUFIFOMETH_12),
    NAMER(NV_PAPU_FEUFIFOMETH_13),
    NAMER(NV_PAPU_FEUFIFOMETH_14),
    NAMER(NV_PAPU_FEUFIFOMETH_15),
    NAMER(NV_PAPU_FEUFIFOMETH_16),
    NAMER(NV_PAPU_FEUFIFOMETH_17),
    NAMER(NV_PAPU_FEUFIFOMETH_18),
    NAMER(NV_PAPU_FEUFIFOMETH_19),
    NAMER(NV_PAPU_FEUFIFOMETH_20),
    NAMER(NV_PAPU_FEUFIFOMETH_21),
    NAMER(NV_PAPU_FEUFIFOMETH_22),
    NAMER(NV_PAPU_FEUFIFOMETH_23),
    NAMER(NV_PAPU_FEUFIFOMETH_24),
    NAMER(NV_PAPU_FEUFIFOMETH_25),
    NAMER(NV_PAPU_FEUFIFOMETH_26),
    NAMER(NV_PAPU_FEUFIFOMETH_27),
    NAMER(NV_PAPU_FEUFIFOMETH_28),
    NAMER(NV_PAPU_FEUFIFOMETH_29),
    NAMER(NV_PAPU_FEUFIFOMETH_30),
    NAMER(NV_PAPU_FEUFIFOMETH_31),
#if 0
NV_PAPU_FEUFIFOMETH_ADDR_BITS                                           0x000000ff
NV_PAPU_FEUFIFOMETH_ADDR_BITS__BITSHIFT                                 0
NV_PAPU_FEUFIFOMETH_ADDR_SAVED_BITS                                     0x000000fc
NV_PAPU_FEUFIFOMETH_ADDR_SAVED_BITS__BITSHIFT                           2
#endif
    NAMER(NV_PAPU_FEUFIFOPARAM_0),
    NAMER(NV_PAPU_FEUFIFOPARAM_1),
    NAMER(NV_PAPU_FEUFIFOPARAM_2),
    NAMER(NV_PAPU_FEUFIFOPARAM_3),
    NAMER(NV_PAPU_FEUFIFOPARAM_4),
    NAMER(NV_PAPU_FEUFIFOPARAM_5),
    NAMER(NV_PAPU_FEUFIFOPARAM_6),
    NAMER(NV_PAPU_FEUFIFOPARAM_7),
    NAMER(NV_PAPU_FEUFIFOPARAM_8),
    NAMER(NV_PAPU_FEUFIFOPARAM_9),
    NAMER(NV_PAPU_FEUFIFOPARAM_10),
    NAMER(NV_PAPU_FEUFIFOPARAM_11),
    NAMER(NV_PAPU_FEUFIFOPARAM_12),
    NAMER(NV_PAPU_FEUFIFOPARAM_13),
    NAMER(NV_PAPU_FEUFIFOPARAM_14),
    NAMER(NV_PAPU_FEUFIFOPARAM_15),
    NAMER(NV_PAPU_FEUFIFOPARAM_16),
    NAMER(NV_PAPU_FEUFIFOPARAM_17),
    NAMER(NV_PAPU_FEUFIFOPARAM_18),
    NAMER(NV_PAPU_FEUFIFOPARAM_19),
    NAMER(NV_PAPU_FEUFIFOPARAM_20),
    NAMER(NV_PAPU_FEUFIFOPARAM_21),
    NAMER(NV_PAPU_FEUFIFOPARAM_22),
    NAMER(NV_PAPU_FEUFIFOPARAM_23),
    NAMER(NV_PAPU_FEUFIFOPARAM_24),
    NAMER(NV_PAPU_FEUFIFOPARAM_25),
    NAMER(NV_PAPU_FEUFIFOPARAM_26),
    NAMER(NV_PAPU_FEUFIFOPARAM_27),
    NAMER(NV_PAPU_FEUFIFOPARAM_28),
    NAMER(NV_PAPU_FEUFIFOPARAM_29),
    NAMER(NV_PAPU_FEUFIFOPARAM_30),
    NAMER(NV_PAPU_FEUFIFOPARAM_31),
    NAMER(NV_PAPU_FETFORCE0),
#if 0
NV_PAPU_FETFORCE0_NOP                                                   0x00000001
NV_PAPU_FETFORCE0_NOP__BITSHIFT                                         0
NV_PAPU_FETFORCE0_SYNCHRONIZE                                           0x00000002
NV_PAPU_FETFORCE0_SYNCHRONIZE__BITSHIFT                                 1
NV_PAPU_FETFORCE0_SET_TIME                                              0x00000004
NV_PAPU_FETFORCE0_SET_TIME__BITSHIFT                                    2
NV_PAPU_FETFORCE0_SET_MODE                                              0x00000008
NV_PAPU_FETFORCE0_SET_MODE__BITSHIFT                                    3
NV_PAPU_FETFORCE0_SET_CURRENT_VOICE                                     0x00000010
NV_PAPU_FETFORCE0_SET_CURRENT_VOICE__BITSHIFT                           4
NV_PAPU_FETFORCE0_SET_ANTECEDENT_VOICE                                  0x00000020
NV_PAPU_FETFORCE0_SET_ANTECEDENT_VOICE__BITSHIFT                        5
NV_PAPU_FETFORCE0_VOICE_ON                                              0x00000040
NV_PAPU_FETFORCE0_VOICE_ON__BITSHIFT                                    6
NV_PAPU_FETFORCE0_VOICE_OFF                                             0x00000080
NV_PAPU_FETFORCE0_VOICE_OFF__BITSHIFT                                   7
NV_PAPU_FETFORCE0_VOICE_RELEASE                                         0x00000100
NV_PAPU_FETFORCE0_VOICE_RELEASE__BITSHIFT                               8
NV_PAPU_FETFORCE0_VOICE_PAUSE                                           0x00000200
NV_PAPU_FETFORCE0_VOICE_PAUSE__BITSHIFT                                 9
NV_PAPU_FETFORCE0_GET_VOICE_POSITION                                    0x00000400
NV_PAPU_FETFORCE0_GET_VOICE_POSITION__BITSHIFT                          10
NV_PAPU_FETFORCE0_SET_SUBMIX_HEADROOM                                   0x00000800
NV_PAPU_FETFORCE0_SET_SUBMIX_HEADROOM__BITSHIFT                         11
NV_PAPU_FETFORCE0_SET_HRTF_SUBMIXES                                     0x00001000
NV_PAPU_FETFORCE0_SET_HRTF_SUBMIXES__BITSHIFT                           12
NV_PAPU_FETFORCE0_SET_HRTF_HEADROOM                                     0x00002000
NV_PAPU_FETFORCE0_SET_HRTF_HEADROOM__BITSHIFT                           13
NV_PAPU_FETFORCE0_SET_VOLUME_TRACKING                                   0x00004000
NV_PAPU_FETFORCE0_SET_VOLUME_TRACKING__BITSHIFT                         14
NV_PAPU_FETFORCE0_SET_PITCH_TRACKING                                    0x00008000
NV_PAPU_FETFORCE0_SET_PITCH_TRACKING__BITSHIFT                          15
NV_PAPU_FETFORCE0_SET_HRTF_TRACKING                                     0x00010000
NV_PAPU_FETFORCE0_SET_HRTF_TRACKING__BITSHIFT                           16
NV_PAPU_FETFORCE0_SET_ITD_TRACKING                                      0x00020000
NV_PAPU_FETFORCE0_SET_ITD_TRACKING__BITSHIFT                            17
NV_PAPU_FETFORCE0_SET_FILTER_TRACKING                                   0x00040000
NV_PAPU_FETFORCE0_SET_FILTER_TRACKING__BITSHIFT                         18
NV_PAPU_FETFORCE0_VOICE_LOCK                                            0x00080000
NV_PAPU_FETFORCE0_VOICE_LOCK__BITSHIFT                                  19
NV_PAPU_FETFORCE0_SET_VOICE_CFG_VBIN                                    0x00100000
NV_PAPU_FETFORCE0_SET_VOICE_CFG_VBIN__BITSHIFT                          20
NV_PAPU_FETFORCE0_SET_VOICE_CFG_FMT                                     0x00200000
NV_PAPU_FETFORCE0_SET_VOICE_CFG_FMT__BITSHIFT                           21
NV_PAPU_FETFORCE0_SET_VOICE_CFG_OTHER                                   0x00400000
NV_PAPU_FETFORCE0_SET_VOICE_CFG_OTHER__BITSHIFT                         22
NV_PAPU_FETFORCE0_SET_VOICE_TAR_HRTF                                    0x00800000
NV_PAPU_FETFORCE0_SET_VOICE_TAR_HRTF__BITSHIFT                          23
NV_PAPU_FETFORCE0_SET_VOICE_PAR_LFODLY                                  0x01000000
NV_PAPU_FETFORCE0_SET_VOICE_PAR_LFODLY__BITSHIFT                        24
NV_PAPU_FETFORCE0_SET_VOICE_SSL                                         0x02000000
NV_PAPU_FETFORCE0_SET_VOICE_SSL__BITSHIFT                               25
NV_PAPU_FETFORCE0_SET_VOICE_CFG_BUF                                     0x04000000
NV_PAPU_FETFORCE0_SET_VOICE_CFG_BUF__BITSHIFT                           26
NV_PAPU_FETFORCE0_SET_VOICE_TAR_VOL                                     0x08000000
NV_PAPU_FETFORCE0_SET_VOICE_TAR_VOL__BITSHIFT                           27
NV_PAPU_FETFORCE0_SET_VOICE_LFO                                         0x10000000
NV_PAPU_FETFORCE0_SET_VOICE_LFO__BITSHIFT                               28
NV_PAPU_FETFORCE0_SET_VOICE_TAR_FC                                      0x20000000
NV_PAPU_FETFORCE0_SET_VOICE_TAR_FC__BITSHIFT                            29
NV_PAPU_FETFORCE0_SET_VOICE_TAR_PITCH                                   0x40000000
NV_PAPU_FETFORCE0_SET_VOICE_TAR_PITCH__BITSHIFT                         30
NV_PAPU_FETFORCE0_BITS                                                  0x7fffffff
NV_PAPU_FETFORCE0_BITS__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_FETFORCE1),
#if 0
NV_PAPU_FETFORCE1_SET_CURRENT_HRTF_ENTRY                                0x00000001
NV_PAPU_FETFORCE1_SET_CURRENT_HRTF_ENTRY__BITSHIFT                      0
NV_PAPU_FETFORCE1_SET_HRIR                                              0x00000002
NV_PAPU_FETFORCE1_SET_HRIR__BITSHIFT                                    1
NV_PAPU_FETFORCE1_SET_HRIR_X                                            0x00000004
NV_PAPU_FETFORCE1_SET_HRIR_X__BITSHIFT                                  2
NV_PAPU_FETFORCE1_SET_CURRENT_SSL_CONTEXT_DMA                           0x00000008
NV_PAPU_FETFORCE1_SET_CURRENT_SSL_CONTEXT_DMA__BITSHIFT                 3
NV_PAPU_FETFORCE1_SET_CURRENT_SSL                                       0x00000010
NV_PAPU_FETFORCE1_SET_CURRENT_SSL__BITSHIFT                             4
NV_PAPU_FETFORCE1_SET_SSL_SEGMENT_OFFSET                                0x00000020
NV_PAPU_FETFORCE1_SET_SSL_SEGMENT_OFFSET__BITSHIFT                      5
NV_PAPU_FETFORCE1_SET_SSL_SEGMENT_LENGTH                                0x00000040
NV_PAPU_FETFORCE1_SET_SSL_SEGMENT_LENGTH__BITSHIFT                      6
NV_PAPU_FETFORCE1_SET_CURRENT_INBUF_SGE_CONTEXT_DMA                     0x00000080
NV_PAPU_FETFORCE1_SET_CURRENT_INBUF_SGE_CONTEXT_DMA__BITSHIFT           7
NV_PAPU_FETFORCE1_SET_CURRENT_INBUF_SGE                                 0x00000100
NV_PAPU_FETFORCE1_SET_CURRENT_INBUF_SGE__BITSHIFT                       8
NV_PAPU_FETFORCE1_SET_CURRENT_INBUF_SGE_OFFSET                          0x00000200
NV_PAPU_FETFORCE1_SET_CURRENT_INBUF_SGE_OFFSET__BITSHIFT                9
NV_PAPU_FETFORCE1_SET_CURRENT_BUFFER_SGE_CONTEXT_DMA                    0x00000400
NV_PAPU_FETFORCE1_SET_CURRENT_BUFFER_SGE_CONTEXT_DMA__BITSHIFT          10
NV_PAPU_FETFORCE1_SET_CURRENT_OUTBUF_SGE                                0x00000800
NV_PAPU_FETFORCE1_SET_CURRENT_OUTBUF_SGE__BITSHIFT                      11
NV_PAPU_FETFORCE1_SET_OUTBUF_BA                                         0x00001000
NV_PAPU_FETFORCE1_SET_OUTBUF_BA__BITSHIFT                               12
NV_PAPU_FETFORCE1_SET_OUTBUF_LEN                                        0x00002000
NV_PAPU_FETFORCE1_SET_OUTBUF_LEN__BITSHIFT                              13
NV_PAPU_FETFORCE1_SET_CURRENT_OUTBUF_SGE_OFFSET                         0x00004000
NV_PAPU_FETFORCE1_SET_CURRENT_OUTBUF_SGE_OFFSET__BITSHIFT               14
NV_PAPU_FETFORCE1_SE2FE_IDLE_VOICE                                      0x00008000
NV_PAPU_FETFORCE1_SE2FE_IDLE_VOICE__BITSHIFT                            15
NV_PAPU_FETFORCE1_SE2FE_NOTIFY                                          0x00010000
NV_PAPU_FETFORCE1_SE2FE_NOTIFY__BITSHIFT                                16
NV_PAPU_FETFORCE1_SE2FE_RTINTR                                          0x00020000
NV_PAPU_FETFORCE1_SE2FE_RTINTR__BITSHIFT                                17
NV_PAPU_FETFORCE1_UNKNOWN                                               0x00040000
NV_PAPU_FETFORCE1_UNKNOWN__BITSHIFT                                     18
NV_PAPU_FETFORCE1_BITS                                                  0x0007ffff
NV_PAPU_FETFORCE1_BITS__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_FETIGNORE0),
#if 0
NV_PAPU_FETIGNORE0_NOP                                                  0x00000001
NV_PAPU_FETIGNORE0_NOP__BITSHIFT                                        0
NV_PAPU_FETIGNORE0_SYNCHRONIZE                                          0x00000002
NV_PAPU_FETIGNORE0_SYNCHRONIZE__BITSHIFT                                1
NV_PAPU_FETIGNORE0_SET_TIME                                             0x00000004
NV_PAPU_FETIGNORE0_SET_TIME__BITSHIFT                                   2
NV_PAPU_FETIGNORE0_SET_MODE                                             0x00000008
NV_PAPU_FETIGNORE0_SET_MODE__BITSHIFT                                   3
NV_PAPU_FETIGNORE0_SET_CURRENT_VOICE                                    0x00000010
NV_PAPU_FETIGNORE0_SET_CURRENT_VOICE__BITSHIFT                          4
NV_PAPU_FETIGNORE0_SET_ANTECEDENT_VOICE                                 0x00000020
NV_PAPU_FETIGNORE0_SET_ANTECEDENT_VOICE__BITSHIFT                       5
NV_PAPU_FETIGNORE0_VOICE_ON                                             0x00000040
NV_PAPU_FETIGNORE0_VOICE_ON__BITSHIFT                                   6
NV_PAPU_FETIGNORE0_VOICE_OFF                                            0x00000080
NV_PAPU_FETIGNORE0_VOICE_OFF__BITSHIFT                                  7
NV_PAPU_FETIGNORE0_VOICE_RELEASE                                        0x00000100
NV_PAPU_FETIGNORE0_VOICE_RELEASE__BITSHIFT                              8
NV_PAPU_FETIGNORE0_VOICE_PAUSE                                          0x00000200
NV_PAPU_FETIGNORE0_VOICE_PAUSE__BITSHIFT                                9
NV_PAPU_FETIGNORE0_GET_VOICE_POSITION                                   0x00000400
NV_PAPU_FETIGNORE0_GET_VOICE_POSITION__BITSHIFT                         10
NV_PAPU_FETIGNORE0_SET_SUBMIX_HEADROOM                                  0x00000800
NV_PAPU_FETIGNORE0_SET_SUBMIX_HEADROOM__BITSHIFT                        11
NV_PAPU_FETIGNORE0_SET_HRTF_SUBMIXES                                    0x00001000
NV_PAPU_FETIGNORE0_SET_HRTF_SUBMIXES__BITSHIFT                          12
NV_PAPU_FETIGNORE0_SET_HRTF_HEADROOM                                    0x00002000
NV_PAPU_FETIGNORE0_SET_HRTF_HEADROOM__BITSHIFT                          13
NV_PAPU_FETIGNORE0_SET_VOLUME_TRACKING                                  0x00004000
NV_PAPU_FETIGNORE0_SET_VOLUME_TRACKING__BITSHIFT                        14
NV_PAPU_FETIGNORE0_SET_PITCH_TRACKING                                   0x00008000
NV_PAPU_FETIGNORE0_SET_PITCH_TRACKING__BITSHIFT                         15
NV_PAPU_FETIGNORE0_SET_HRTF_TRACKING                                    0x00010000
NV_PAPU_FETIGNORE0_SET_HRTF_TRACKING__BITSHIFT                          16
NV_PAPU_FETIGNORE0_SET_ITD_TRACKING                                     0x00020000
NV_PAPU_FETIGNORE0_SET_ITD_TRACKING__BITSHIFT                           17
NV_PAPU_FETIGNORE0_SET_FILTER_TRACKING                                  0x00040000
NV_PAPU_FETIGNORE0_SET_FILTER_TRACKING__BITSHIFT                        18
NV_PAPU_FETIGNORE0_VOICE_LOCK                                           0x00080000
NV_PAPU_FETIGNORE0_VOICE_LOCK__BITSHIFT                                 19
NV_PAPU_FETIGNORE0_SET_VOICE_CFG_VBIN                                   0x00100000
NV_PAPU_FETIGNORE0_SET_VOICE_CFG_VBIN__BITSHIFT                         20
NV_PAPU_FETIGNORE0_SET_VOICE_CFG_FMT                                    0x00200000
NV_PAPU_FETIGNORE0_SET_VOICE_CFG_FMT__BITSHIFT                          21
NV_PAPU_FETIGNORE0_SET_VOICE_CFG_OTHER                                  0x00400000
NV_PAPU_FETIGNORE0_SET_VOICE_CFG_OTHER__BITSHIFT                        22
NV_PAPU_FETIGNORE0_SET_VOICE_TAR_HRTF                                   0x00800000
NV_PAPU_FETIGNORE0_SET_VOICE_TAR_HRTF__BITSHIFT                         23
NV_PAPU_FETIGNORE0_SET_VOICE_PAR_LFODLY                                 0x01000000
NV_PAPU_FETIGNORE0_SET_VOICE_PAR_LFODLY__BITSHIFT                       24
NV_PAPU_FETIGNORE0_SET_VOICE_SSL                                        0x02000000
NV_PAPU_FETIGNORE0_SET_VOICE_SSL__BITSHIFT                              25
NV_PAPU_FETIGNORE0_SET_VOICE_CFG_BUF                                    0x04000000
NV_PAPU_FETIGNORE0_SET_VOICE_CFG_BUF__BITSHIFT                          26
NV_PAPU_FETIGNORE0_SET_VOICE_TAR_VOL                                    0x08000000
NV_PAPU_FETIGNORE0_SET_VOICE_TAR_VOL__BITSHIFT                          27
NV_PAPU_FETIGNORE0_SET_VOICE_LFO                                        0x10000000
NV_PAPU_FETIGNORE0_SET_VOICE_LFO__BITSHIFT                              28
NV_PAPU_FETIGNORE0_SET_VOICE_TAR_FC                                     0x20000000
NV_PAPU_FETIGNORE0_SET_VOICE_TAR_FC__BITSHIFT                           29
NV_PAPU_FETIGNORE0_SET_VOICE_TAR_PITCH                                  0x40000000
NV_PAPU_FETIGNORE0_SET_VOICE_TAR_PITCH__BITSHIFT                        30
NV_PAPU_FETIGNORE0_BITS                                                 0x7fffffff
NV_PAPU_FETIGNORE0_BITS__BITSHIFT                                       0
#endif
    NAMER(NV_PAPU_FETIGNORE1),
#if 0
NV_PAPU_FETIGNORE1_SET_CURRENT_HRTF_ENTRY                               0x00000001
NV_PAPU_FETIGNORE1_SET_CURRENT_HRTF_ENTRY__BITSHIFT                     0
NV_PAPU_FETIGNORE1_SET_HRIR                                             0x00000002
NV_PAPU_FETIGNORE1_SET_HRIR__BITSHIFT                                   1
NV_PAPU_FETIGNORE1_SET_HRIR_X                                           0x00000004
NV_PAPU_FETIGNORE1_SET_HRIR_X__BITSHIFT                                 2
NV_PAPU_FETIGNORE1_SET_CURRENT_SSL_CONTEXT_DMA                          0x00000008
NV_PAPU_FETIGNORE1_SET_CURRENT_SSL_CONTEXT_DMA__BITSHIFT                3
NV_PAPU_FETIGNORE1_SET_CURRENT_SSL                                      0x00000010
NV_PAPU_FETIGNORE1_SET_CURRENT_SSL__BITSHIFT                            4
NV_PAPU_FETIGNORE1_SET_SSL_SEGMENT_OFFSET                               0x00000020
NV_PAPU_FETIGNORE1_SET_SSL_SEGMENT_OFFSET__BITSHIFT                     5
NV_PAPU_FETIGNORE1_SET_SSL_SEGMENT_LENGTH                               0x00000040
NV_PAPU_FETIGNORE1_SET_SSL_SEGMENT_LENGTH__BITSHIFT                     6
NV_PAPU_FETIGNORE1_SET_CURRENT_INBUF_SGE_CONTEXT_DMA                    0x00000080
NV_PAPU_FETIGNORE1_SET_CURRENT_INBUF_SGE_CONTEXT_DMA__BITSHIFT          7
NV_PAPU_FETIGNORE1_SET_CURRENT_INBUF_SGE                                0x00000100
NV_PAPU_FETIGNORE1_SET_CURRENT_INBUF_SGE__BITSHIFT                      8
NV_PAPU_FETIGNORE1_SET_CURRENT_INBUF_SGE_OFFSET                         0x00000200
NV_PAPU_FETIGNORE1_SET_CURRENT_INBUF_SGE_OFFSET__BITSHIFT               9
NV_PAPU_FETIGNORE1_SET_CURRENT_BUFFER_SGE_CONTEXT_DMA                   0x00000400
NV_PAPU_FETIGNORE1_SET_CURRENT_BUFFER_SGE_CONTEXT_DMA__BITSHIFT         10
NV_PAPU_FETIGNORE1_SET_CURRENT_OUTBUF_SGE                               0x00000800
NV_PAPU_FETIGNORE1_SET_CURRENT_OUTBUF_SGE__BITSHIFT                     11
NV_PAPU_FETIGNORE1_SET_OUTBUF_BA                                        0x00001000
NV_PAPU_FETIGNORE1_SET_OUTBUF_BA__BITSHIFT                              12
NV_PAPU_FETIGNORE1_SET_OUTBUF_LEN                                       0x00002000
NV_PAPU_FETIGNORE1_SET_OUTBUF_LEN__BITSHIFT                             13
NV_PAPU_FETIGNORE1_SET_CURRENT_OUTBUF_SGE_OFFSET                        0x00004000
NV_PAPU_FETIGNORE1_SET_CURRENT_OUTBUF_SGE_OFFSET__BITSHIFT              14
NV_PAPU_FETIGNORE1_SE2FE_IDLE_VOICE                                     0x00008000
NV_PAPU_FETIGNORE1_SE2FE_IDLE_VOICE__BITSHIFT                           15
NV_PAPU_FETIGNORE1_UNKNOWN                                              0x00040000
NV_PAPU_FETIGNORE1_UNKNOWN__BITSHIFT                                    18
NV_PAPU_FETIGNORE1_TRAPREASON_NOT_IMPLEMENTED                           0x00400000
NV_PAPU_FETIGNORE1_TRAPREASON_NOT_IMPLEMENTED__BITSHIFT                 22
NV_PAPU_FETIGNORE1_TRAPREASON_METHOD_UNKNOWN                            0x00800000
NV_PAPU_FETIGNORE1_TRAPREASON_METHOD_UNKNOWN__BITSHIFT                  23
NV_PAPU_FETIGNORE1_TRAPREASON_BAD_ARGUMENT                              0x01000000
NV_PAPU_FETIGNORE1_TRAPREASON_BAD_ARGUMENT__BITSHIFT                    24
NV_PAPU_FETIGNORE1_TRAPREASON_CURRENT_NOT_SET                           0x02000000
NV_PAPU_FETIGNORE1_TRAPREASON_CURRENT_NOT_SET__BITSHIFT                 25
NV_PAPU_FETIGNORE1_TRAPREASON_ANTECEDENT_NOT_SET                        0x04000000
NV_PAPU_FETIGNORE1_TRAPREASON_ANTECEDENT_NOT_SET__BITSHIFT              26
NV_PAPU_FETIGNORE1_TRAPREASON_BAD_DATA_TYPE                             0x10000000
NV_PAPU_FETIGNORE1_TRAPREASON_BAD_DATA_TYPE__BITSHIFT                   28
NV_PAPU_FETIGNORE1_TRAPREASON_BAD_LIST_POINTER                          0x20000000
NV_PAPU_FETIGNORE1_TRAPREASON_BAD_LIST_POINTER__BITSHIFT                29
NV_PAPU_FETIGNORE1_TRAPREASON_CURRENT_VOICE_NOT_3D                      0x40000000
NV_PAPU_FETIGNORE1_TRAPREASON_CURRENT_VOICE_NOT_3D__BITSHIFT            30
NV_PAPU_FETIGNORE1_TRAPREASON_CTXPA_INVALID                             0x80000000
NV_PAPU_FETIGNORE1_TRAPREASON_CTXPA_INVALID__BITSHIFT                   31
#endif
    NAMER(NV_PAPU_FEPRIVLOCK),
#if 0
NV_PAPU_FEPRIVLOCK_VALUE                                                0x00000001
NV_PAPU_FEPRIVLOCK_VALUE__BITSHIFT                                      0
NV_PAPU_FEPRIVLOCK_VALUE_UNLOCKED                                       0
NV_PAPU_FEPRIVLOCK_VALUE_LOCKED                                         1
NV_PAPU_FEPRIVLOCK_BITS                                                 0x00000001
NV_PAPU_FEPRIVLOCK_BITS__BITSHIFT                                       0
#endif
    NAMER(NV_PAPU_SECTL),
#if 0
NV_PAPU_SECTL_SEPRTYP                                                   0x00000001
NV_PAPU_SECTL_SEPRTYP__BITSHIFT                                         0
NV_PAPU_SECTL_SEPRTYP_NON_ISO                                           0
NV_PAPU_SECTL_SEPRTYP_ISO                                               1
NV_PAPU_SECTL_SEPWTYP                                                   0x00000002
NV_PAPU_SECTL_SEPWTYP__BITSHIFT                                         1
NV_PAPU_SECTL_SEPWTYP_NON_ISO                                           0
NV_PAPU_SECTL_SEPWTYP_ISO                                               1
NV_PAPU_SECTL_SESRTYP                                                   0x00000004
NV_PAPU_SECTL_SESRTYP__BITSHIFT                                         2
NV_PAPU_SECTL_SESRTYP_NON_ISO                                           0
NV_PAPU_SECTL_SESRTYP_ISO                                               1
NV_PAPU_SECTL_XCNTMODE                                                  0x00000018
NV_PAPU_SECTL_XCNTMODE__BITSHIFT                                        3
NV_PAPU_SECTL_XCNTMODE_OFF                                              0
NV_PAPU_SECTL_XCNTMODE_AC_SYNC                                          1
NV_PAPU_SECTL_XCNTMODE_SW                                               2
NV_PAPU_SECTL_XCNTMODE_FREE_RUNNING                                     3
#endif
    NAMER(NV_PAPU_STID),
#if 0
NV_PAPU_STID_SESTART                                                    0x00000001
NV_PAPU_STID_SESTART__BITSHIFT                                          0
NV_PAPU_STID_SESTART_NOT_STARTING                                       0
NV_PAPU_STID_SESTART_STARTING                                           1
NV_PAPU_STID_SEIDLE                                                     0x00000002
NV_PAPU_STID_SEIDLE__BITSHIFT                                           1
NV_PAPU_STID_SEIDLE_IDLE                                                1
NV_PAPU_STID_SEIDLE_NOT_IDLE                                            0
NV_PAPU_STID_VPSTART                                                    0x00000004
NV_PAPU_STID_VPSTART__BITSHIFT                                          2
NV_PAPU_STID_VPSTART_NOT_STARTING                                       0
NV_PAPU_STID_VPSTART_STARTING                                           1
NV_PAPU_STID_VPIDLE                                                     0x00000008
NV_PAPU_STID_VPIDLE__BITSHIFT                                           3
NV_PAPU_STID_VPIDLE_IDLE                                                1
NV_PAPU_STID_VPIDLE_NOT_IDLE                                            0
NV_PAPU_STID_GPSTART                                                    0x00000010
NV_PAPU_STID_GPSTART__BITSHIFT                                          4
NV_PAPU_STID_GPSTART_NOT_STARTING                                       0
NV_PAPU_STID_GPSTART_STARTING                                           1
NV_PAPU_STID_GPIDLE                                                     0x00000020
NV_PAPU_STID_GPIDLE__BITSHIFT                                           5
NV_PAPU_STID_GPIDLE_IDLE                                                1
NV_PAPU_STID_GPIDLE_NOT_IDLE                                            0
NV_PAPU_STID_EPSTART                                                    0x00000040
NV_PAPU_STID_EPSTART__BITSHIFT                                          6
NV_PAPU_STID_EPSTART_NOT_STARTING                                       0
NV_PAPU_STID_EPSTART_STARTING                                           1
NV_PAPU_STID_EPIDLE                                                     0x00000080
NV_PAPU_STID_EPIDLE__BITSHIFT                                           7
NV_PAPU_STID_EPIDLE_IDLE                                                1
NV_PAPU_STID_EPIDLE_NOT_IDLE                                            0
#endif
    NAMER(NV_PAPU_IGSCNT),
#if 0
NV_PAPU_IGSCNT_INIT                                                     0
#endif
    NAMER(NV_PAPU_XGSCNT),
#if 0
NV_PAPU_XGSCNT_INIT                                                     0xffffffff
#endif
    NAMER(NV_PAPU_DGSCNT),
#if 0
NV_PAPU_DGSCNT_INIT                                                     0
#endif
    NAMER(NV_PAPU_WGSCNT),
#if 0
NV_PAPU_WGSCNT_INIT                                                     0
#endif
    NAMER(NV_PAPU_RGSCNT),
#if 0
NV_PAPU_RGSCNT_INIT                                                     0
#endif
    NAMER(NV_PAPU_PGSCNT),
#if 0
NV_PAPU_PGSCNT_INIT                                                     0
#endif
    NAMER(NV_PAPU_EGSCNT),
#if 0
NV_PAPU_EGSCNT_INIT                                                     0
#endif
    NAMER(NV_PAPU_DEGSCNT),
#if 0
NV_PAPU_DEGSCNT_INIT                                                    0
#endif
    NAMER(NV_PAPU_ECNTSTP),
#if 0
NV_PAPU_ECNTSTP_VALUE                                                   0x0000ffff
NV_PAPU_ECNTSTP_VALUE__BITSHIFT                                         0
#endif
    NAMER(NV_PAPU_VPVADDR),
#if 0
NV_PAPU_VPVADDR_VALUE                                                   0xffffc000
NV_PAPU_VPVADDR_VALUE__BITSHIFT                                         14
#endif
    NAMER(NV_PAPU_VPSGEADDR),
#if 0
NV_PAPU_VPSGEADDR_VALUE                                                 0xffffc000
NV_PAPU_VPSGEADDR_VALUE__BITSHIFT                                       14
#endif
    NAMER(NV_PAPU_VPSSLADDR),
#if 0
NV_PAPU_VPSSLADDR_VALUE                                                 0xffffc000
NV_PAPU_VPSSLADDR_VALUE__BITSHIFT                                       14
#endif
    NAMER(NV_PAPU_VPHTADDR),
#if 0
NV_PAPU_VPHTADDR_VALUE                                                  0xffffc000
NV_PAPU_VPHTADDR_VALUE__BITSHIFT                                        14
#endif
    NAMER(NV_PAPU_VPHCADDR),
#if 0
NV_PAPU_VPHCADDR_VALUE                                                  0xffffc000
NV_PAPU_VPHCADDR_VALUE__BITSHIFT                                        14
#endif
    NAMER(NV_PAPU_GPSADDR),
#if 0
NV_PAPU_GPSADDR_VALUE                                                   0xffffc000
NV_PAPU_GPSADDR_VALUE__BITSHIFT                                         14
#endif
    NAMER(NV_PAPU_GPFADDR),
#if 0
NV_PAPU_GPFADDR_VALUE                                                   0xffffc000
NV_PAPU_GPFADDR_VALUE__BITSHIFT                                         14
#endif
    NAMER(NV_PAPU_EPSADDR),
#if 0
NV_PAPU_EPSADDR_VALUE                                                   0xffffc000
NV_PAPU_EPSADDR_VALUE__BITSHIFT                                         14
#endif
    NAMER(NV_PAPU_EPFADDR),
#if 0
NV_PAPU_EPFADDR_VALUE                                                   0xffffc000
NV_PAPU_EPFADDR_VALUE__BITSHIFT                                         14
#endif
    NAMER(NV_PAPU_VLOCK),
#if 0
NV_PAPU_VLOCK_VOICE                                                     0x0000ffff
NV_PAPU_VLOCK_VOICE__BITSHIFT                                           0
NV_PAPU_VLOCK_REQ                                                       0x00010000
NV_PAPU_VLOCK_REQ__BITSHIFT                                             16
NV_PAPU_VLOCK_REQ_LOCK                                                  1
NV_PAPU_VLOCK_REQ_UNLOCK                                                0
NV_PAPU_VLOCK_STATUS                                                    0x00020000
NV_PAPU_VLOCK_STATUS__BITSHIFT                                          17
NV_PAPU_VLOCK_STATUS_NOT_IN_PROGRESS                                    0
NV_PAPU_VLOCK_STATUS_IN_PROGRESS                                        1
#endif
    NAMER(NV_PAPU_TVL2D),
#if 0
NV_PAPU_TVL2D_VALUE                                                     0x0000ffff
NV_PAPU_TVL2D_VALUE__BITSHIFT                                           0
NV_PAPU_TVL2D_VALUE_NULL                                                0x0000ffff
#endif
    NAMER(NV_PAPU_CVL2D),
#if 0
NV_PAPU_CVL2D_VALUE                                                     0x0000ffff
NV_PAPU_CVL2D_VALUE__BITSHIFT                                           0
NV_PAPU_CVL2D_VALUE_NULL                                                0x0000ffff
#endif
    NAMER(NV_PAPU_NVL2D),
#if 0
NV_PAPU_NVL2D_VALUE                                                     0x0000ffff
NV_PAPU_NVL2D_VALUE__BITSHIFT                                           0
NV_PAPU_NVL2D_VALUE_NULL                                                0x0000ffff
#endif
    NAMER(NV_PAPU_TVL3D),
#if 0
NV_PAPU_TVL3D_VALUE                                                     0x0000ffff
NV_PAPU_TVL3D_VALUE__BITSHIFT                                           0
NV_PAPU_TVL3D_VALUE_NULL                                                0x0000ffff
#endif
    NAMER(NV_PAPU_CVL3D),
#if 0
NV_PAPU_CVL3D_VALUE                                                     0x0000ffff
NV_PAPU_CVL3D_VALUE__BITSHIFT                                           0
NV_PAPU_CVL3D_VALUE_NULL                                                0x0000ffff
#endif
    NAMER(NV_PAPU_NVL3D),
#if 0
NV_PAPU_NVL3D_VALUE                                                     0x0000ffff
NV_PAPU_NVL3D_VALUE__BITSHIFT                                           0
NV_PAPU_NVL3D_VALUE_NULL                                                0x0000ffff
#endif
    NAMER(NV_PAPU_TVLMP),
#if 0
NV_PAPU_TVLMP_VALUE                                                     0x0000ffff
NV_PAPU_TVLMP_VALUE__BITSHIFT                                           0
NV_PAPU_TVLMP_VALUE_NULL                                                0x0000ffff
#endif
    NAMER(NV_PAPU_CVLMP),
#if 0
NV_PAPU_CVLMP_VALUE                                                     0x0000ffff
NV_PAPU_CVLMP_VALUE__BITSHIFT                                           0
NV_PAPU_CVLMP_VALUE_NULL                                                0x0000ffff
#endif
    NAMER(NV_PAPU_NVLMP),
#if 0
NV_PAPU_NVLMP_VALUE                                                     0x0000ffff
NV_PAPU_NVLMP_VALUE__BITSHIFT                                           0
NV_PAPU_NVLMP_VALUE_NULL                                                0x0000ffff
#endif
    NAMER(NV_PAPU_ALPHAV),
#if 0
NV_PAPU_ALPHAV_VALUE                                                    0x00000fff
NV_PAPU_ALPHAV_VALUE__BITSHIFT                                          0
#endif
    NAMER(NV_PAPU_ALPHAP),
#if 0
NV_PAPU_ALPHAP_VALUE                                                    0x00000fff
NV_PAPU_ALPHAP_VALUE__BITSHIFT                                          0
#endif
    NAMER(NV_PAPU_ALPHAF),
#if 0
NV_PAPU_ALPHAF_VALUE                                                    0x00000fff
NV_PAPU_ALPHAF_VALUE__BITSHIFT                                          0
#endif
    NAMER(NV_PAPU_ALPHAI),
#if 0
NV_PAPU_ALPHAI_VALUE                                                    0x00000fff
NV_PAPU_ALPHAI_VALUE__BITSHIFT                                          0
#endif
    NAMER(NV_PAPU_ALPHAH),
#if 0
NV_PAPU_ALPHAH_VALUE                                                    0x00000fff
NV_PAPU_ALPHAH_VALUE__BITSHIFT                                          0
#endif
    NAMER(NV_PAPU_MIXHRA),
#if 0
NV_PAPU_MIXHRA_MHR0                                                     0x00000007
NV_PAPU_MIXHRA_MHR0__BITSHIFT                                           0
NV_PAPU_MIXHRA_MHR0_INIT                                                0
NV_PAPU_MIXHRA_MHR1                                                     0x00000700
NV_PAPU_MIXHRA_MHR1__BITSHIFT                                           8
NV_PAPU_MIXHRA_MHR1_INIT                                                0
NV_PAPU_MIXHRA_MHR2                                                     0x00070000
NV_PAPU_MIXHRA_MHR2__BITSHIFT                                           16
NV_PAPU_MIXHRA_MHR2_INIT                                                0
NV_PAPU_MIXHRA_MHR3                                                     0x07000000
NV_PAPU_MIXHRA_MHR3__BITSHIFT                                           24
NV_PAPU_MIXHRA_MHR3_INIT                                                0
#endif
    NAMER(NV_PAPU_MIXHRB),
#if 0
NV_PAPU_MIXHRB_MHR4                                                     0x00000007
NV_PAPU_MIXHRB_MHR4__BITSHIFT                                           0
NV_PAPU_MIXHRB_MHR4_INIT                                                0
NV_PAPU_MIXHRB_MHR5                                                     0x00000700
NV_PAPU_MIXHRB_MHR5__BITSHIFT                                           8
NV_PAPU_MIXHRB_MHR5_INIT                                                0
NV_PAPU_MIXHRB_MHR6                                                     0x00070000
NV_PAPU_MIXHRB_MHR6__BITSHIFT                                           16
NV_PAPU_MIXHRB_MHR6_INIT                                                0
NV_PAPU_MIXHRB_MHR7                                                     0x07000000
NV_PAPU_MIXHRB_MHR7__BITSHIFT                                           24
NV_PAPU_MIXHRB_MHR7_INIT                                                0
#endif
    NAMER(NV_PAPU_MIXHRC),
#if 0
NV_PAPU_MIXHRC_MHR8                                                     0x00000007
NV_PAPU_MIXHRC_MHR8__BITSHIFT                                           0
NV_PAPU_MIXHRC_MHR8_INIT                                                0
NV_PAPU_MIXHRC_MHR9                                                     0x00000700
NV_PAPU_MIXHRC_MHR9__BITSHIFT                                           8
NV_PAPU_MIXHRC_MHR9_INIT                                                0
NV_PAPU_MIXHRC_MHR10                                                    0x00070000
NV_PAPU_MIXHRC_MHR10__BITSHIFT                                          16
NV_PAPU_MIXHRC_MHR10_INIT                                               0
NV_PAPU_MIXHRC_MHR11                                                    0x07000000
NV_PAPU_MIXHRC_MHR11__BITSHIFT                                          24
NV_PAPU_MIXHRC_MHR11_INIT                                               0
#endif
    NAMER(NV_PAPU_MIXHRD),
#if 0
NV_PAPU_MIXHRD_MHR12                                                    0x00000007
NV_PAPU_MIXHRD_MHR12__BITSHIFT                                          0
NV_PAPU_MIXHRD_MHR12_INIT                                               0
NV_PAPU_MIXHRD_MHR13                                                    0x00000700
NV_PAPU_MIXHRD_MHR13__BITSHIFT                                          8
NV_PAPU_MIXHRD_MHR13_INIT                                               0
NV_PAPU_MIXHRD_MHR14                                                    0x00070000
NV_PAPU_MIXHRD_MHR14__BITSHIFT                                          16
NV_PAPU_MIXHRD_MHR14_INIT                                               0
NV_PAPU_MIXHRD_MHR15                                                    0x07000000
NV_PAPU_MIXHRD_MHR15__BITSHIFT                                          24
NV_PAPU_MIXHRD_MHR15_INIT                                               0
#endif
    NAMER(NV_PAPU_MIXHRE),
#if 0
NV_PAPU_MIXHRE_MHR16                                                    0x00000007
NV_PAPU_MIXHRE_MHR16__BITSHIFT                                          0
NV_PAPU_MIXHRE_MHR16_INIT                                               0
NV_PAPU_MIXHRE_MHR17                                                    0x00000700
NV_PAPU_MIXHRE_MHR17__BITSHIFT                                          8
NV_PAPU_MIXHRE_MHR17_INIT                                               0
NV_PAPU_MIXHRE_MHR18                                                    0x00070000
NV_PAPU_MIXHRE_MHR18__BITSHIFT                                          16
NV_PAPU_MIXHRE_MHR18_INIT                                               0
NV_PAPU_MIXHRE_MHR19                                                    0x07000000
NV_PAPU_MIXHRE_MHR19__BITSHIFT                                          24
#endif
    NAMER(NV_PAPU_MIXHRF),
#if 0
NV_PAPU_MIXHRF_MHR19_INIT                                               0
NV_PAPU_MIXHRF_MHR20                                                    0x00000007
NV_PAPU_MIXHRF_MHR20__BITSHIFT                                          0
NV_PAPU_MIXHRF_MHR20_INIT                                               0
NV_PAPU_MIXHRF_MHR21                                                    0x00000700
NV_PAPU_MIXHRF_MHR21__BITSHIFT                                          8
NV_PAPU_MIXHRF_MHR21_INIT                                               0
NV_PAPU_MIXHRF_MHR22                                                    0x00070000
NV_PAPU_MIXHRF_MHR22__BITSHIFT                                          16
NV_PAPU_MIXHRF_MHR22_INIT                                               0
NV_PAPU_MIXHRF_MHR23                                                    0x07000000
NV_PAPU_MIXHRF_MHR23__BITSHIFT                                          24
NV_PAPU_MIXHRF_MHR23_INIT                                               0
#endif
    NAMER(NV_PAPU_MIXHRG),
#if 0
NV_PAPU_MIXHRG_MHR24                                                    0x00000007
NV_PAPU_MIXHRG_MHR24__BITSHIFT                                          0
NV_PAPU_MIXHRG_MHR24_INIT                                               0
NV_PAPU_MIXHRG_MHR25                                                    0x00000700
NV_PAPU_MIXHRG_MHR25__BITSHIFT                                          8
NV_PAPU_MIXHRG_MHR25_INIT                                               0
NV_PAPU_MIXHRG_MHR26                                                    0x00070000
NV_PAPU_MIXHRG_MHR26__BITSHIFT                                          16
NV_PAPU_MIXHRG_MHR26_INIT                                               0
NV_PAPU_MIXHRG_MHR27                                                    0x07000000
NV_PAPU_MIXHRG_MHR27__BITSHIFT                                          24
NV_PAPU_MIXHRG_MHR27_INIT                                               0
#endif
    NAMER(NV_PAPU_MIXHRH),
#if 0
NV_PAPU_MIXHRH_MHR28                                                    0x00000007
NV_PAPU_MIXHRH_MHR28__BITSHIFT                                          0
NV_PAPU_MIXHRH_MHR28_INIT                                               0
NV_PAPU_MIXHRH_MHR29                                                    0x00000700
NV_PAPU_MIXHRH_MHR29__BITSHIFT                                          8
NV_PAPU_MIXHRH_MHR29_INIT                                               0
NV_PAPU_MIXHRH_MHR30                                                    0x00070000
NV_PAPU_MIXHRH_MHR30__BITSHIFT                                          16
NV_PAPU_MIXHRH_MHR30_INIT                                               0
NV_PAPU_MIXHRH_MHR31                                                    0x07000000
NV_PAPU_MIXHRH_MHR31__BITSHIFT                                          24
NV_PAPU_MIXHRH_MHR31_INIT                                               0
#endif
    NAMER(NV_PAPU_MIX3D),
#if 0
NV_PAPU_MIX3D_SMX3DFL                                                   0x0000001f
NV_PAPU_MIX3D_SMX3DFL__BITSHIFT                                         0
NV_PAPU_MIX3D_SMX3DFL_INIT                                              0
NV_PAPU_MIX3D_SMX3DFR                                                   0x00001f00
NV_PAPU_MIX3D_SMX3DFR__BITSHIFT                                         8
NV_PAPU_MIX3D_SMX3DFR_INIT                                              0
NV_PAPU_MIX3D_SMX3DRL                                                   0x001f0000
NV_PAPU_MIX3D_SMX3DRL__BITSHIFT                                         16
NV_PAPU_MIX3D_SMX3DRL_INIT                                              0
NV_PAPU_MIX3D_SMX3DRR                                                   0x1f000000
NV_PAPU_MIX3D_SMX3DRR__BITSHIFT                                         24
NV_PAPU_MIX3D_SMX3DRR_INIT                                              0
#endif
    NAMER(NV_PAPU_TDHR),
#if 0
NV_PAPU_TDHR_TDHR                                                       0x00000007
NV_PAPU_TDHR_TDHR__BITSHIFT                                             0
NV_PAPU_TDHR_TDHR_INIT                                                  0
#endif
    NAMER(NV_PAPU_AVGSESTI),
    NAMER(NV_PAPU_WCSESTI),
    NAMER(NV_PAPU_CURSESTI),
    NAMER(NV_PAPU_CNTSESTI),
#if 0
NV_PAPU_CNTSESTI_VALUE                                                  0x000000ff
NV_PAPU_CNTSESTI_VALUE__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_AVGGPSTI),
    NAMER(NV_PAPU_WCGPSTI),
    NAMER(NV_PAPU_CURGPSTI),
    NAMER(NV_PAPU_CNTGPSTI),
#if 0
NV_PAPU_CNTGPSTI_VALUE                                                  0x000000ff
NV_PAPU_CNTGPSTI_VALUE__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_GPSMAXSGE),
#if 0
NV_PAPU_GPSMAXSGE_VALUE                                                 0x0000ffff
NV_PAPU_GPSMAXSGE_VALUE__BITSHIFT                                       0
NV_PAPU_GPSMAXSGE_INIT                                                  0
NV_PAPU_GPSMAXSGE_BITS                                                  0x0000ffff
NV_PAPU_GPSMAXSGE_BITS__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_GPFMAXSGE),
#if 0
NV_PAPU_GPFMAXSGE_VALUE                                                 0x0000ffff
NV_PAPU_GPFMAXSGE_VALUE__BITSHIFT                                       0
NV_PAPU_GPFMAXSGE_INIT                                                  0
NV_PAPU_GPFMAXSGE_BITS                                                  0x0000ffff
NV_PAPU_GPFMAXSGE_BITS__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_EPSMAXSGE),
#if 0
NV_PAPU_EPSMAXSGE_VALUE                                                 0x0000ffff
NV_PAPU_EPSMAXSGE_VALUE__BITSHIFT                                       0
NV_PAPU_EPSMAXSGE_INIT                                                  0
NV_PAPU_EPSMAXSGE_BITS                                                  0x0000ffff
NV_PAPU_EPSMAXSGE_BITS__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_EPFMAXSGE),
#if 0
NV_PAPU_EPFMAXSGE_VALUE                                                 0x0000ffff
NV_PAPU_EPFMAXSGE_VALUE__BITSHIFT                                       0
NV_PAPU_EPFMAXSGE_INIT                                                  0
NV_PAPU_EPFMAXSGE_BITS                                                  0x0000ffff
NV_PAPU_EPFMAXSGE_BITS__BITSHIFT                                        0
#endif
    NAMER(NV_PAPU_GPNADDR),
#if 0
NV_PAPU_GPNADDR_VALUE                                                   0xfffff000
NV_PAPU_GPNADDR_VALUE__BITSHIFT                                         12
#endif
    NAMER(NV_PAPU_GPCADDR),
#if 0
NV_PAPU_GPCADDR_VALUE                                                   0xfffff000
NV_PAPU_GPCADDR_VALUE__BITSHIFT                                         12
#endif
    NAMER(NV_PAPU_GPOFBASE0),
#if 0
NV_PAPU_GPOFBASE0_VALUE                                                 0x00ffff00
NV_PAPU_GPOFBASE0_VALUE__BITSHIFT                                       8
#endif
    NAMER(NV_PAPU_GPOFBASE1),
#if 0
NV_PAPU_GPOFBASE1_VALUE                                                 0x00ffff00
NV_PAPU_GPOFBASE1_VALUE__BITSHIFT                                       8
#endif
    NAMER(NV_PAPU_GPOFBASE2),
#if 0
NV_PAPU_GPOFBASE2_VALUE                                                 0x00ffff00
NV_PAPU_GPOFBASE2_VALUE__BITSHIFT                                       8
#endif
    NAMER(NV_PAPU_GPOFBASE3),
#if 0
NV_PAPU_GPOFBASE3_VALUE                                                 0x00ffff00
NV_PAPU_GPOFBASE3_VALUE__BITSHIFT                                       8
#endif
    NAMER(NV_PAPU_GPOFEND0),
#if 0
NV_PAPU_GPOFEND0_VALUE                                                  0x00ffff00
NV_PAPU_GPOFEND0_VALUE__BITSHIFT                                        8
#endif
    NAMER(NV_PAPU_GPOFEND1),
#if 0
NV_PAPU_GPOFEND1_VALUE                                                  0x00ffff00
NV_PAPU_GPOFEND1_VALUE__BITSHIFT                                        8
#endif
    NAMER(NV_PAPU_GPOFEND2),
#if 0
NV_PAPU_GPOFEND2_VALUE                                                  0x00ffff00
NV_PAPU_GPOFEND2_VALUE__BITSHIFT                                        8
#endif
    NAMER(NV_PAPU_GPOFEND3),
#if 0
NV_PAPU_GPOFEND3_VALUE                                                  0x00ffff00
NV_PAPU_GPOFEND3_VALUE__BITSHIFT                                        8
#endif
    NAMER(NV_PAPU_GPOFCUR0),
#if 0
NV_PAPU_GPOFCUR0_VALUE                                                  0x00fffffc
NV_PAPU_GPOFCUR0_VALUE__BITSHIFT                                        2
#endif
    NAMER(NV_PAPU_GPOFCUR1),
#if 0
NV_PAPU_GPOFCUR1_VALUE                                                  0x00fffffc
NV_PAPU_GPOFCUR1_VALUE__BITSHIFT                                        2
#endif
    NAMER(NV_PAPU_GPOFCUR2),
#if 0
NV_PAPU_GPOFCUR2_VALUE                                                  0x00fffffc
NV_PAPU_GPOFCUR2_VALUE__BITSHIFT                                        2
#endif
    NAMER(NV_PAPU_GPOFCUR3),
#if 0
NV_PAPU_GPOFCUR3_VALUE                                                  0x00fffffc
NV_PAPU_GPOFCUR3_VALUE__BITSHIFT                                        2
#endif
    NAMER(NV_PAPU_GPIFBASE0),
#if 0
NV_PAPU_GPIFBASE0_VALUE                                                 0x00ffff00
NV_PAPU_GPIFBASE0_VALUE__BITSHIFT                                       8
#endif
    NAMER(NV_PAPU_GPIFBASE1),
#if 0
NV_PAPU_GPIFBASE1_VALUE                                                 0x00ffff00
NV_PAPU_GPIFBASE1_VALUE__BITSHIFT                                       8
#endif
    NAMER(NV_PAPU_GPIFEND0),
#if 0
NV_PAPU_GPIFEND0_VALUE                                                  0x00ffff00
NV_PAPU_GPIFEND0_VALUE__BITSHIFT                                        8
#endif
    NAMER(NV_PAPU_GPIFEND1),
#if 0
NV_PAPU_GPIFEND1_VALUE                                                  0x00ffff00
NV_PAPU_GPIFEND1_VALUE__BITSHIFT                                        8
#endif
    NAMER(NV_PAPU_GPIFCUR0),
#if 0
NV_PAPU_GPIFCUR0_VALUE                                                  0x00fffffc
NV_PAPU_GPIFCUR0_VALUE__BITSHIFT                                        2
#endif
    NAMER(NV_PAPU_GPIFCUR1),
#if 0
NV_PAPU_GPIFCUR1_VALUE                                                  0x00fffffc
NV_PAPU_GPIFCUR1_VALUE__BITSHIFT                                        2
#endif
    NAMER(NV_PAPU_EPNADDR),
#if 0
NV_PAPU_EPNADDR_VALUE                                                   0xfffff000
NV_PAPU_EPNADDR_VALUE__BITSHIFT                                         12
#endif
    NAMER(NV_PAPU_EPCADDR),
#if 0
NV_PAPU_EPCADDR_VALUE                                                   0xfffff000
NV_PAPU_EPCADDR_VALUE__BITSHIFT                                         12
#endif
    NAMER(NV_PAPU_EPOFBASE0),
#if 0
NV_PAPU_EPOFBASE0_VALUE                                                 0x00ffff00
NV_PAPU_EPOFBASE0_VALUE__BITSHIFT                                       8
#endif
    NAMER(NV_PAPU_EPOFBASE1),
#if 0
NV_PAPU_EPOFBASE1_VALUE                                                 0x00ffff00
NV_PAPU_EPOFBASE1_VALUE__BITSHIFT                                       8
#endif
    NAMER(NV_PAPU_EPOFBASE2),
#if 0
NV_PAPU_EPOFBASE2_VALUE                                                 0x00ffff00
NV_PAPU_EPOFBASE2_VALUE__BITSHIFT                                       8
#endif
    NAMER(NV_PAPU_EPOFBASE3),
#if 0
NV_PAPU_EPOFBASE3_VALUE                                                 0x00ffff00
NV_PAPU_EPOFBASE3_VALUE__BITSHIFT                                       8
#endif
    NAMER(NV_PAPU_EPOFEND0),
#if 0
NV_PAPU_EPOFEND0_VALUE                                                  0x00ffff00
NV_PAPU_EPOFEND0_VALUE__BITSHIFT                                        8
#endif
    NAMER(NV_PAPU_EPOFEND1),
#if 0
NV_PAPU_EPOFEND1_VALUE                                                  0x00ffff00
NV_PAPU_EPOFEND1_VALUE__BITSHIFT                                        8
#endif
    NAMER(NV_PAPU_EPOFEND2),
#if 0
NV_PAPU_EPOFEND2_VALUE                                                  0x00ffff00
NV_PAPU_EPOFEND2_VALUE__BITSHIFT                                        8
#endif
    NAMER(NV_PAPU_EPOFEND3),
#if 0
NV_PAPU_EPOFEND3_VALUE                                                  0x00ffff00
NV_PAPU_EPOFEND3_VALUE__BITSHIFT                                        8
#endif
    NAMER(NV_PAPU_EPOFCUR0),
#if 0
NV_PAPU_EPOFCUR0_VALUE                                                  0x00fffffc
NV_PAPU_EPOFCUR0_VALUE__BITSHIFT                                        2
#endif
    NAMER(NV_PAPU_EPOFCUR1),
#if 0
NV_PAPU_EPOFCUR1_VALUE                                                  0x00fffffc
NV_PAPU_EPOFCUR1_VALUE__BITSHIFT                                        2
#endif
    NAMER(NV_PAPU_EPOFCUR2),
#if 0
NV_PAPU_EPOFCUR2_VALUE                                                  0x00fffffc
NV_PAPU_EPOFCUR2_VALUE__BITSHIFT                                        2
#endif
    NAMER(NV_PAPU_EPOFCUR3),
#if 0
NV_PAPU_EPOFCUR3_VALUE                                                  0x00fffffc
NV_PAPU_EPOFCUR3_VALUE__BITSHIFT                                        2
#endif
    NAMER(NV_PAPU_EPIFBASE0),
#if 0
NV_PAPU_EPIFBASE0_VALUE                                                 0x00ffff00
NV_PAPU_EPIFBASE0_VALUE__BITSHIFT                                       8
#endif
    NAMER(NV_PAPU_EPIFBASE1),
#if 0
NV_PAPU_EPIFBASE1_VALUE                                                 0x00ffff00
NV_PAPU_EPIFBASE1_VALUE__BITSHIFT                                       8
#endif
    NAMER(NV_PAPU_EPIFEND0),
#if 0
NV_PAPU_EPIFEND0_VALUE                                                  0x00ffff00
NV_PAPU_EPIFEND0_VALUE__BITSHIFT                                        8
#endif
    NAMER(NV_PAPU_EPIFEND1),
#if 0
NV_PAPU_EPIFEND1_VALUE                                                  0x00ffff00
NV_PAPU_EPIFEND1_VALUE__BITSHIFT                                        8
#endif
    NAMER(NV_PAPU_EPIFCUR0),
#if 0
NV_PAPU_EPIFCUR0_VALUE                                                  0x00fffffc
NV_PAPU_EPIFCUR0_VALUE__BITSHIFT                                        2
#endif
    NAMER(NV_PAPU_EPIFCUR1),
#if 0
NV_PAPU_EPIFCUR1_VALUE                                                  0x00fffffc
NV_PAPU_EPIFCUR1_VALUE__BITSHIFT                                        2
#endif
};

