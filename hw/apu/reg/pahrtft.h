
enum {
        NV_PAHRTFT_HRIR8A_0                         = 0x00000000,
        NV_PAHRTFT_HRIR8A_1                         = 0x00000004,
        NV_PAHRTFT_HRIR8A_2                         = 0x00000008,
        NV_PAHRTFT_HRIR8A_3                         = 0x0000000c,
        NV_PAHRTFT_HRIR8A_4                         = 0x00000010,
        NV_PAHRTFT_HRIR8A_5                         = 0x00000014,
        NV_PAHRTFT_HRIR8A_6                         = 0x00000018,
        NV_PAHRTFT_HRIR8A_7                         = 0x0000001c,
        NV_PAHRTFT_HRIR8A_8                         = 0x00000020,
        NV_PAHRTFT_HRIR8A_9                         = 0x00000024,
        NV_PAHRTFT_HRIR8A_10                        = 0x00000028,
        NV_PAHRTFT_HRIR8A_11                        = 0x0000002c,
        NV_PAHRTFT_HRIR8A_12                        = 0x00000030,
        NV_PAHRTFT_HRIR8A_13                        = 0x00000034,
        NV_PAHRTFT_HRIR8A_14                        = 0x00000038,
#define NV_PAHRTFT_HRIR8A_LEFT0                                                 0x000000ff
#define NV_PAHRTFT_HRIR8A_LEFT0__BITSHIFT                                       0
#define NV_PAHRTFT_HRIR8A_RIGHT0                                                0x0000ff00
#define NV_PAHRTFT_HRIR8A_RIGHT0__BITSHIFT                                      8
#define NV_PAHRTFT_HRIR8A_LEFT1                                                 0x00ff0000
#define NV_PAHRTFT_HRIR8A_LEFT1__BITSHIFT                                       16
#define NV_PAHRTFT_HRIR8A_RIGHT1                                                0xff000000
#define NV_PAHRTFT_HRIR8A_RIGHT1__BITSHIFT                                      24
        NV_PAHRTFT_HRIR8X                           = 0x0000003c,
#define NV_PAHRTFT_HRIR8X_LEFT30                                                0x000000ff
#define NV_PAHRTFT_HRIR8X_LEFT30__BITSHIFT                                      0
#define NV_PAHRTFT_HRIR8X_RIGHT30                                               0x0000ff00
#define NV_PAHRTFT_HRIR8X_RIGHT30__BITSHIFT                                     8
#define NV_PAHRTFT_HRIR8X_ITD                                                   0xffff0000
#define NV_PAHRTFT_HRIR8X_ITD__BITSHIFT                                         16
};

static const char *const apu_pahrtft_name[] = {
    NAMER(NV_PAHRTFT_HRIR8A_0),
    NAMER(NV_PAHRTFT_HRIR8A_1),
    NAMER(NV_PAHRTFT_HRIR8A_2),
    NAMER(NV_PAHRTFT_HRIR8A_3),
    NAMER(NV_PAHRTFT_HRIR8A_4),
    NAMER(NV_PAHRTFT_HRIR8A_5),
    NAMER(NV_PAHRTFT_HRIR8A_6),
    NAMER(NV_PAHRTFT_HRIR8A_7),
    NAMER(NV_PAHRTFT_HRIR8A_8),
    NAMER(NV_PAHRTFT_HRIR8A_9),
    NAMER(NV_PAHRTFT_HRIR8A_10),
    NAMER(NV_PAHRTFT_HRIR8A_11),
    NAMER(NV_PAHRTFT_HRIR8A_12),
    NAMER(NV_PAHRTFT_HRIR8A_13),
    NAMER(NV_PAHRTFT_HRIR8A_14),
#if 0
NV_PAHRTFT_HRIR8A_LEFT0                                                 0x000000ff
NV_PAHRTFT_HRIR8A_LEFT0__BITSHIFT                                       0
NV_PAHRTFT_HRIR8A_RIGHT0                                                0x0000ff00
NV_PAHRTFT_HRIR8A_RIGHT0__BITSHIFT                                      8
NV_PAHRTFT_HRIR8A_LEFT1                                                 0x00ff0000
NV_PAHRTFT_HRIR8A_LEFT1__BITSHIFT                                       16
NV_PAHRTFT_HRIR8A_RIGHT1                                                0xff000000
NV_PAHRTFT_HRIR8A_RIGHT1__BITSHIFT                                      24
#endif
    NAMER(NV_PAHRTFT_HRIR8X),
#if 0
NV_PAHRTFT_HRIR8X_LEFT30                                                0x000000ff
NV_PAHRTFT_HRIR8X_LEFT30__BITSHIFT                                      0
NV_PAHRTFT_HRIR8X_RIGHT30                                               0x0000ff00
NV_PAHRTFT_HRIR8X_RIGHT30__BITSHIFT                                     8
NV_PAHRTFT_HRIR8X_ITD                                                   0xffff0000
NV_PAHRTFT_HRIR8X_ITD__BITSHIFT                                         16
#endif
};

