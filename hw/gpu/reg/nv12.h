
enum {
        NV12_IMAGE_BLIT                             = 0x0000009f,
        NV_09F_NV12_IMAGE_BLIT                      = 0x00000000,
        NV_09F_NOP                                  = 0x00000100,
        NV_09F_NOTIFY                               = 0x00000104,
        NV_09F_WAIT_FOR_IDLE                        = 0x00000108,
        NV_09F_WAIT_FOR_CRTC                        = 0x0000010c,
        NV_09F_SET_CONTEXT_DMA_NOTIFY               = 0x00000180,
        NV_09F_SET_CONTEXT_COLOR_KEY                = 0x00000184,
        NV_09F_SET_CONTEXT_CLIP_RECTANGLE           = 0x00000188,
        NV_09F_SET_CONTEXT_PATTERN                  = 0x0000018c,
        NV_09F_SET_CONTEXT_ROP                      = 0x00000190,
        NV_09F_SET_CONTEXT_BETA1                    = 0x00000194,
        NV_09F_SET_CONTEXT_BETA4                    = 0x00000198,
        NV_09F_SET_CONTEXT_SURFACE                  = 0x0000019c,
        NV_09F_SET_OPERATION                        = 0x000002fc,
#define NV_09F_SET_OPERATION_MODE_SRCCOPY_AND           0
#define NV_09F_SET_OPERATION_MODE_ROP_AND               1
#define NV_09F_SET_OPERATION_MODE_BLEND_AND             2
#define NV_09F_SET_OPERATION_MODE_SRCCOPY               3
#define NV_09F_SET_OPERATION_MODE_SRCCOPY_PREMULT       4
#define NV_09F_SET_OPERATION_MODE_BLEND_PREMULT         5
        NV_09F_POINT_IN                             = 0x00000300,
#define NV_09F_POINT_IN_X                               0x0000ffff
#define NV_09F_POINT_IN_X__BITSHIFT                     0
#define NV_09F_POINT_IN_Y                               0xffff0000
#define NV_09F_POINT_IN_Y__BITSHIFT                     16
        NV_09F_POINT_OUT                            = 0x00000304,
#define NV_09F_POINT_OUT_X                              0x0000ffff
#define NV_09F_POINT_OUT_X__BITSHIFT                    0
#define NV_09F_POINT_OUT_Y                              0xffff0000
#define NV_09F_POINT_OUT_Y__BITSHIFT                    16
        NV_09F_SIZE                                 = 0x00000308,
#define NV_09F_SIZE_WIDTH                               0x0000ffff
#define NV_09F_SIZE_WIDTH__BITSHIFT                     0
#define NV_09F_SIZE_HEIGHT                              0xffff0000
#define NV_09F_SIZE_HEIGHT__BITSHIFT                    16
};

static const char *const nv2a_09f_name[] = {
    NAMER(NV_09F_NV12_IMAGE_BLIT),
    NAMER(NV_09F_NOP),
    NAMER(NV_09F_NOTIFY),
    NAMER(NV_09F_WAIT_FOR_IDLE),
    NAMER(NV_09F_WAIT_FOR_CRTC),
    NAMER(NV_09F_SET_CONTEXT_DMA_NOTIFY),
    NAMER(NV_09F_SET_CONTEXT_COLOR_KEY),
    NAMER(NV_09F_SET_CONTEXT_CLIP_RECTANGLE),
    NAMER(NV_09F_SET_CONTEXT_PATTERN),
    NAMER(NV_09F_SET_CONTEXT_ROP),
    NAMER(NV_09F_SET_CONTEXT_BETA1),
    NAMER(NV_09F_SET_CONTEXT_BETA4),
    NAMER(NV_09F_SET_CONTEXT_SURFACE),
    NAMER(NV_09F_SET_OPERATION),
    NAMER(NV_09F_POINT_IN),
    NAMER(NV_09F_POINT_OUT),
    NAMER(NV_09F_SIZE),
};

static const char *const nv2a_09f_set_operation_name[] = {
    NAME(NV_09F_SET_OPERATION_MODE_SRCCOPY_AND),
    NAME(NV_09F_SET_OPERATION_MODE_ROP_AND),
    NAME(NV_09F_SET_OPERATION_MODE_BLEND_AND),
    NAME(NV_09F_SET_OPERATION_MODE_SRCCOPY),
    NAME(NV_09F_SET_OPERATION_MODE_SRCCOPY_PREMULT),
    NAME(NV_09F_SET_OPERATION_MODE_BLEND_PREMULT),
};

