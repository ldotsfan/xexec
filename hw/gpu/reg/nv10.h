
enum {
        NV10_CONTEXT_SURFACES_2D                    = 0x00000062,
        NV_062_NV10_CONTEXT_SURFACES_2D             = 0x00000000,
        NV_062_NOP                                  = 0x00000100,
        NV_062_PM_TRIGGER                           = 0x00000140,
        NV_062_NOTIFY                               = 0x00000104,
        NV_062_WAIT_FOR_IDLE                        = 0x00000108,
        NV_062_SET_CONTEXT_DMA_NOTIFY               = 0x00000180,
        NV_062_SET_CONTEXT_DMA_IMAGE_SOURCE         = 0x00000184,
        NV_062_SET_CONTEXT_DMA_IMAGE_DESTIN         = 0x00000188,
        NV_062_COLOR_FORMAT                         = 0x00000300,
#define NV_062_COLOR_FORMAT_LE_Y8                       1
#define NV_062_COLOR_FORMAT_LE_X1R5G5B5_Z1R5G5B5        2
#define NV_062_COLOR_FORMAT_LE_X1R5G5B5_O1R5G5B5        3
#define NV_062_COLOR_FORMAT_LE_R5G6B5                   4
#define NV_062_COLOR_FORMAT_LE_Y16                      5
#define NV_062_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8        6
#define NV_062_COLOR_FORMAT_LE_X8R8G8B8_O8R8G8B8        7
#define NV_062_COLOR_FORMAT_LE_X1A7R8G8B8_Z1A7R8G8B8    8
#define NV_062_COLOR_FORMAT_LE_X1A7R8G8B8_O1A7R8G8B8    9
#define NV_062_COLOR_FORMAT_LE_A8R8G8B8                 10
#define NV_062_COLOR_FORMAT_LE_Y32                      11
        NV_062_PITCH                                = 0x00000304,
#define NV_062_PITCH_SOURCE                             0x0000ffff
#define NV_062_PITCH_SOURCE__BITSHIFT                   0
#define NV_062_PITCH_DESTIN                             0xffff0000
#define NV_062_PITCH_DESTIN__BITSHIFT                   16
        NV_062_OFFSET_SOURCE                        = 0x00000308,
        NV_062_OFFSET_DESTIN                        = 0x0000030c,
};

static const char *const nv2a_062_name[] = {
    NAMER(NV_062_NV10_CONTEXT_SURFACES_2D),
    NAMER(NV_062_NOP),
    NAMER(NV_062_PM_TRIGGER),
    NAMER(NV_062_NOTIFY),
    NAMER(NV_062_WAIT_FOR_IDLE),
    NAMER(NV_062_SET_CONTEXT_DMA_NOTIFY),
    NAMER(NV_062_SET_CONTEXT_DMA_IMAGE_SOURCE),
    NAMER(NV_062_SET_CONTEXT_DMA_IMAGE_DESTIN),
    NAMER(NV_062_COLOR_FORMAT),
    NAMER(NV_062_PITCH),
    NAMER(NV_062_OFFSET_SOURCE),
    NAMER(NV_062_OFFSET_DESTIN),
};

static const char *const nv2a_062_color_format_name[] = {
    NAME(NV_062_COLOR_FORMAT_LE_Y8),
    NAME(NV_062_COLOR_FORMAT_LE_X1R5G5B5_Z1R5G5B5),
    NAME(NV_062_COLOR_FORMAT_LE_X1R5G5B5_O1R5G5B5),
    NAME(NV_062_COLOR_FORMAT_LE_R5G6B5),
    NAME(NV_062_COLOR_FORMAT_LE_Y16),
    NAME(NV_062_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8),
    NAME(NV_062_COLOR_FORMAT_LE_X8R8G8B8_O8R8G8B8),
    NAME(NV_062_COLOR_FORMAT_LE_X1A7R8G8B8_Z1A7R8G8B8),
    NAME(NV_062_COLOR_FORMAT_LE_X1A7R8G8B8_O1A7R8G8B8),
    NAME(NV_062_COLOR_FORMAT_LE_A8R8G8B8),
    NAME(NV_062_COLOR_FORMAT_LE_Y32),
};

