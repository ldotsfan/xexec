
enum {
        NV_PAHRTFC_HRIR12A_0                        = 0x00000000,
        NV_PAHRTFC_HRIR12A_1                        = 0x0000000c,
        NV_PAHRTFC_HRIR12A_2                        = 0x00000018,
        NV_PAHRTFC_HRIR12A_3                        = 0x00000024,
        NV_PAHRTFC_HRIR12A_4                        = 0x00000030,
        NV_PAHRTFC_HRIR12A_5                        = 0x0000003c,
        NV_PAHRTFC_HRIR12A_6                        = 0x00000048,
#define NV_PAHRTFC_HRIR12A_LEFT0                                                0x00000fff
#define NV_PAHRTFC_HRIR12A_LEFT0__BITSHIFT                                      0
#define NV_PAHRTFC_HRIR12A_RIGHT0                                               0x00fff000
#define NV_PAHRTFC_HRIR12A_RIGHT0__BITSHIFT                                     12
#define NV_PAHRTFC_HRIR12A_LEFT1_B7_0                                           0xff000000
#define NV_PAHRTFC_HRIR12A_LEFT1_B7_0__BITSHIFT                                 24
        NV_PAHRTFC_HRIR12B_0                        = 0x00000004,
        NV_PAHRTFC_HRIR12B_1                        = 0x00000010,
        NV_PAHRTFC_HRIR12B_2                        = 0x0000001c,
        NV_PAHRTFC_HRIR12B_3                        = 0x00000028,
        NV_PAHRTFC_HRIR12B_4                        = 0x00000034,
        NV_PAHRTFC_HRIR12B_5                        = 0x00000040,
        NV_PAHRTFC_HRIR12B_6                        = 0x0000004c,
#define NV_PAHRTFC_HRIR12B_LEFT2                                                0x00000fff
#define NV_PAHRTFC_HRIR12B_LEFT2__BITSHIFT                                      0
#define NV_PAHRTFC_HRIR12B_RIGHT2                                               0x00fff000
#define NV_PAHRTFC_HRIR12B_RIGHT2__BITSHIFT                                     12
#define NV_PAHRTFC_HRIR12B_RIGHT1_B11_8                                         0x0f000000
#define NV_PAHRTFC_HRIR12B_RIGHT1_B11_8__BITSHIFT                               24
#define NV_PAHRTFC_HRIR12B_LEFT1_B11_8                                          0xf0000000
#define NV_PAHRTFC_HRIR12B_LEFT1_B11_8__BITSHIFT                                28
        NV_PAHRTFC_HRIR12C_0                        = 0x00000008,
        NV_PAHRTFC_HRIR12C_1                        = 0x00000014,
        NV_PAHRTFC_HRIR12C_2                        = 0x00000020,
        NV_PAHRTFC_HRIR12C_3                        = 0x0000002c,
        NV_PAHRTFC_HRIR12C_4                        = 0x00000038,
        NV_PAHRTFC_HRIR12C_5                        = 0x00000044,
        NV_PAHRTFC_HRIR12C_6                        = 0x00000050,
#define NV_PAHRTFC_HRIR12C_LEFT3                                                0x00000fff
#define NV_PAHRTFC_HRIR12C_LEFT3__BITSHIFT                                      0
#define NV_PAHRTFC_HRIR12C_RIGHT3                                               0x00fff000
#define NV_PAHRTFC_HRIR12C_RIGHT3__BITSHIFT                                     12
#define NV_PAHRTFC_HRIR12C_RIGHT1_B7_0                                          0xff000000
#define NV_PAHRTFC_HRIR12C_RIGHT1_B7_0__BITSHIFT                                24
        NV_PAHRTFC_HRIR12D                          = 0x00000054,
#define NV_PAHRTFC_HRIR12D_LEFT28                                               0x00000fff
#define NV_PAHRTFC_HRIR12D_LEFT28__BITSHIFT                                     0
#define NV_PAHRTFC_HRIR12D_RIGHT28                                              0x00fff000
#define NV_PAHRTFC_HRIR12D_RIGHT28__BITSHIFT                                    12
#define NV_PAHRTFC_HRIR12D_LEFT29_B7_0                                          0xff000000
#define NV_PAHRTFC_HRIR12D_LEFT29_B7_0__BITSHIFT                                24
        NV_PAHRTFC_HRIR12E                          = 0x00000058,
#define NV_PAHRTFC_HRIR12E_LEFT30                                               0x00000fff
#define NV_PAHRTFC_HRIR12E_LEFT30__BITSHIFT                                     0
#define NV_PAHRTFC_HRIR12E_RIGHT30                                              0x00fff000
#define NV_PAHRTFC_HRIR12E_RIGHT30__BITSHIFT                                    12
#define NV_PAHRTFC_HRIR12E_RIGHT29_B11_8                                        0x0f000000
#define NV_PAHRTFC_HRIR12E_RIGHT29_B11_8__BITSHIFT                              24
#define NV_PAHRTFC_HRIR12E_LEFT29_B11_8                                         0xf0000000
#define NV_PAHRTFC_HRIR12E_LEFT29_B11_8__BITSHIFT                               28
        NV_PAHRTFC_HRIR12X                          = 0x0000005c,
#define NV_PAHRTFC_HRIR12X_ITD                                                  0x0000ffff
#define NV_PAHRTFC_HRIR12X_ITD__BITSHIFT                                        0
#define NV_PAHRTFC_HRIR12X_RIGHT29_B7_0                                         0xff000000
#define NV_PAHRTFC_HRIR12X_RIGHT29_B7_0__BITSHIFT                               24
};

static const char *const apu_pahrtfc_name[] = {
    NAMER(NV_PAHRTFC_HRIR12A_0),
    NAMER(NV_PAHRTFC_HRIR12A_1),
    NAMER(NV_PAHRTFC_HRIR12A_2),
    NAMER(NV_PAHRTFC_HRIR12A_3),
    NAMER(NV_PAHRTFC_HRIR12A_4),
    NAMER(NV_PAHRTFC_HRIR12A_5),
    NAMER(NV_PAHRTFC_HRIR12A_6),
#if 0
NV_PAHRTFC_HRIR12A_LEFT0                                                0x00000fff
NV_PAHRTFC_HRIR12A_LEFT0__BITSHIFT                                      0
NV_PAHRTFC_HRIR12A_RIGHT0                                               0x00fff000
NV_PAHRTFC_HRIR12A_RIGHT0__BITSHIFT                                     12
NV_PAHRTFC_HRIR12A_LEFT1_B7_0                                           0xff000000
NV_PAHRTFC_HRIR12A_LEFT1_B7_0__BITSHIFT                                 24
#endif
    NAMER(NV_PAHRTFC_HRIR12B_0),
    NAMER(NV_PAHRTFC_HRIR12B_1),
    NAMER(NV_PAHRTFC_HRIR12B_2),
    NAMER(NV_PAHRTFC_HRIR12B_3),
    NAMER(NV_PAHRTFC_HRIR12B_4),
    NAMER(NV_PAHRTFC_HRIR12B_5),
    NAMER(NV_PAHRTFC_HRIR12B_6),
#if 0
NV_PAHRTFC_HRIR12B_LEFT2                                                0x00000fff
NV_PAHRTFC_HRIR12B_LEFT2__BITSHIFT                                      0
NV_PAHRTFC_HRIR12B_RIGHT2                                               0x00fff000
NV_PAHRTFC_HRIR12B_RIGHT2__BITSHIFT                                     12
NV_PAHRTFC_HRIR12B_RIGHT1_B11_8                                         0x0f000000
NV_PAHRTFC_HRIR12B_RIGHT1_B11_8__BITSHIFT                               24
NV_PAHRTFC_HRIR12B_LEFT1_B11_8                                          0xf0000000
NV_PAHRTFC_HRIR12B_LEFT1_B11_8__BITSHIFT                                28
#endif
    NAMER(NV_PAHRTFC_HRIR12C_0),
    NAMER(NV_PAHRTFC_HRIR12C_1),
    NAMER(NV_PAHRTFC_HRIR12C_2),
    NAMER(NV_PAHRTFC_HRIR12C_3),
    NAMER(NV_PAHRTFC_HRIR12C_4),
    NAMER(NV_PAHRTFC_HRIR12C_5),
    NAMER(NV_PAHRTFC_HRIR12C_6),
#if 0
NV_PAHRTFC_HRIR12C_LEFT3                                                0x00000fff
NV_PAHRTFC_HRIR12C_LEFT3__BITSHIFT                                      0
NV_PAHRTFC_HRIR12C_RIGHT3                                               0x00fff000
NV_PAHRTFC_HRIR12C_RIGHT3__BITSHIFT                                     12
NV_PAHRTFC_HRIR12C_RIGHT1_B7_0                                          0xff000000
NV_PAHRTFC_HRIR12C_RIGHT1_B7_0__BITSHIFT                                24
#endif
    NAMER(NV_PAHRTFC_HRIR12D),
#if 0
NV_PAHRTFC_HRIR12D_LEFT28                                               0x00000fff
NV_PAHRTFC_HRIR12D_LEFT28__BITSHIFT                                     0
NV_PAHRTFC_HRIR12D_RIGHT28                                              0x00fff000
NV_PAHRTFC_HRIR12D_RIGHT28__BITSHIFT                                    12
NV_PAHRTFC_HRIR12D_LEFT29_B7_0                                          0xff000000
NV_PAHRTFC_HRIR12D_LEFT29_B7_0__BITSHIFT                                24
#endif
    NAMER(NV_PAHRTFC_HRIR12E),
#if 0
NV_PAHRTFC_HRIR12E_LEFT30                                               0x00000fff
NV_PAHRTFC_HRIR12E_LEFT30__BITSHIFT                                     0
NV_PAHRTFC_HRIR12E_RIGHT30                                              0x00fff000
NV_PAHRTFC_HRIR12E_RIGHT30__BITSHIFT                                    12
NV_PAHRTFC_HRIR12E_RIGHT29_B11_8                                        0x0f000000
NV_PAHRTFC_HRIR12E_RIGHT29_B11_8__BITSHIFT                              24
NV_PAHRTFC_HRIR12E_LEFT29_B11_8                                         0xf0000000
NV_PAHRTFC_HRIR12E_LEFT29_B11_8__BITSHIFT                               28
#endif
    NAMER(NV_PAHRTFC_HRIR12X),
#if 0
NV_PAHRTFC_HRIR12X_ITD                                                  0x0000ffff
NV_PAHRTFC_HRIR12X_ITD__BITSHIFT                                        0
NV_PAHRTFC_HRIR12X_RIGHT29_B7_0                                         0xff000000
NV_PAHRTFC_HRIR12X_RIGHT29_B7_0__BITSHIFT                               24
#endif
};

