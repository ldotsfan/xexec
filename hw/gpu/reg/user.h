
enum {
#define NV_USER_REG                                                             0x0000ffff
#define NV_USER_REG__BITSHIFT                                                   0
#define NV_USER_CHID                                                            0xffff0000
#define NV_USER_CHID__BITSHIFT                                                  16
        NV_USER_OBJECT                              = 0x00000000,
#define NV_USER_OBJECT__OFFSET(x,y)                                             (((x) << 16) + ((y) << 13)) /* x = channel, y = subchannel */
        NV_USER_DMA_PUT                             = 0x00000040,
#define NV_USER_DMA_PUT_OFFSET                                                  0xfffffffc
#define NV_USER_DMA_PUT_OFFSET__BITSHIFT                                        0
        NV_USER_DMA_GET                             = 0x00000044,
#define NV_USER_DMA_GET_OFFSET                                                  0xfffffffc
#define NV_USER_DMA_GET_OFFSET__BITSHIFT                                        0
        NV_USER_REF                                 = 0x00000048,
};

static const char *const nv2a_user_name[] = {
    NAMER(NV_USER_DMA_PUT),
    NAMER(NV_USER_DMA_GET),
    NAMER(NV_USER_REF),
};

