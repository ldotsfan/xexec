
enum {
    ACI_NABM_PI_INDEX = 0x00,   /* pcm in */
    ACI_NABM_PO_INDEX = 0x10,   /* pcm out */
    ACI_NABM_MC_INDEX = 0x20,   /* mic in */
    ACI_NABM_SO_INDEX = 0x70,   /* spdif out */
};

static const uint32_t const aci_nabm_index[] = {
    ACI_NABM_PI_INDEX,
    ACI_NABM_PO_INDEX,
    ACI_NABM_MC_INDEX,
    ACI_NABM_SO_INDEX,
};

enum {
#define ACI_NABM_REGS(x) \
        ACI_NABM_ ## x ## _BDBAR    = (ACI_NABM_ ## x ## _INDEX), \
        ACI_NABM_ ## x ## _CIV      = (ACI_NABM_ ## x ## _INDEX) + 0x4, \
        ACI_NABM_ ## x ## _LVI      = (ACI_NABM_ ## x ## _INDEX) + 0x5, \
        ACI_NABM_ ## x ## _SR       = (ACI_NABM_ ## x ## _INDEX) + 0x6, \
        ACI_NABM_ ## x ## _PICB     = (ACI_NABM_ ## x ## _INDEX) + 0x8, \
        ACI_NABM_ ## x ## _PIV      = (ACI_NABM_ ## x ## _INDEX) + 0xa, \
        ACI_NABM_ ## x ## _CR       = (ACI_NABM_ ## x ## _INDEX) + 0xb
#define ACI_NABM_BD_BUP                 0x40000000  /* buffer underrun policy */
#define ACI_NABM_BD_IOC                 0x80000000  /* interrupt on completion */
#define ACI_NABM_SR_MASK                0x0000001f
#define ACI_NABM_SR_DCH                 0x00000001
#define ACI_NABM_SR_CELV                0x00000002
#define ACI_NABM_SR_LVBCI               0x00000004
#define ACI_NABM_SR_BCIS                0x00000008
#define ACI_NABM_SR_FIFOE               0x00000010
#define ACI_NABM_SR_RO_MASK             (ACI_NABM_SR_DCH | \
                                        ACI_NABM_SR_CELV)
#define ACI_NABM_SR_WCLEAR_MASK         (ACI_NABM_SR_LVBCI | \
                                        ACI_NABM_SR_BCIS | \
                                        ACI_NABM_SR_FIFOE)
#define ACI_NABM_CR_MASK                0x0000001f
#define ACI_NABM_CR_RPBM                0x00000001
#define ACI_NABM_CR_RR                  0x00000002
#define ACI_NABM_CR_LVBIE               0x00000004
#define ACI_NABM_CR_FEIE                0x00000008
#define ACI_NABM_CR_IOCE                0x00000010
#define ACI_NABM_CR_NCLEAR_MASK         (ACI_NABM_CR_LVBIE | \
                                        ACI_NABM_CR_FEIE | \
                                        ACI_NABM_CR_IOCE)
        ACI_NABM_REGS(PI),
        ACI_NABM_REGS(PO),
        ACI_NABM_REGS(MC),
        ACI_NABM_GLOB_CNT           = 0x0000002c,
#define ACI_NABM_GLOB_CNT_MASK          0x0000003f
#define ACI_NABM_GLOB_CNT_CR            0x00000002
#define ACI_NABM_GLOB_CNT_WR            0x00000004
        ACI_NABM_GLOB_STA           = 0x00000030,
#define ACI_NABM_GLOB_STA_MASK          0x0003ffff
#define ACI_NABM_GLOB_STA_GSCI          0x00000001
#define ACI_NABM_GLOB_STA_MIINT         0x00000002
#define ACI_NABM_GLOB_STA_MOINT         0x00000004
#define ACI_NABM_GLOB_STA_RSRVD         0x00000018
#define ACI_NABM_GLOB_STA_PIINT         0x00000020
#define ACI_NABM_GLOB_STA_POINT         0x00000040
#define ACI_NABM_GLOB_STA_MINT          0x00000080
#define ACI_NABM_GLOB_STA_S0CR          0x00000100
#define ACI_NABM_GLOB_STA_S1CR          0x00000200
#define ACI_NABM_GLOB_STA_S0R1          0x00000400
#define ACI_NABM_GLOB_STA_S1R1          0x00000800
#define ACI_NABM_GLOB_STA_B1S12         0x00001000
#define ACI_NABM_GLOB_STA_B2S12         0x00002000
#define ACI_NABM_GLOB_STA_B3S12         0x00004000
#define ACI_NABM_GLOB_STA_RCS           0x00008000
#define ACI_NABM_GLOB_STA_AD3           0x00010000
#define ACI_NABM_GLOB_STA_MD3           0x00020000
#define ACI_NABM_GLOB_STA_RO_MASK       (ACI_NABM_GLOB_STA_MIINT | \
                                        ACI_NABM_GLOB_STA_MOINT | \
                                        ACI_NABM_GLOB_STA_RSRVD | \
                                        ACI_NABM_GLOB_STA_PIINT | \
                                        ACI_NABM_GLOB_STA_POINT | \
                                        ACI_NABM_GLOB_STA_MINT | \
                                        ACI_NABM_GLOB_STA_S0CR | \
                                        ACI_NABM_GLOB_STA_S1CR | \
                                        ACI_NABM_GLOB_STA_B1S12 | \
                                        ACI_NABM_GLOB_STA_B2S12 | \
                                        ACI_NABM_GLOB_STA_B3S12)
#define ACI_NABM_GLOB_STA_WCLEAR_MASK   (ACI_NABM_GLOB_STA_GSCI | \
                                        ACI_NABM_GLOB_STA_S0R1 | \
                                        ACI_NABM_GLOB_STA_S1R1 | \
                                        ACI_NABM_GLOB_STA_RCS)
        ACI_NABM_CAS                = 0x00000034,   /* codec access semaphore register */
        ACI_NABM_REGS(SO),
#undef ACI_NABM_REGS
};

typedef struct {
    uint32_t bdbar; /*  0 */    /* buffer descriptor list base address register */
    uint8_t  civ;   /*  4 */    /* current index value */
    uint8_t  lvi;   /*  5 */    /* last valid index */
    uint16_t sr;    /*  6 */    /* status register */
    uint16_t picb;  /*  8 */    /* position in current buffer */
    uint8_t  piv;   /* 10 */    /* prefetched index value */
    uint8_t  cr;    /* 11 */    /* control register */
    uint32_t pad;   /* 12 */
} PACKED aci_nabm;

static const char *const aci_nabm_name[] = {
#define ACI_NABM_REGS_NAME(x) \
    NAME(ACI_NABM_ ## x ## _BDBAR), \
    NAME(ACI_NABM_ ## x ## _CIV), \
    NAME(ACI_NABM_ ## x ## _LVI), \
    NAME(ACI_NABM_ ## x ## _SR), \
    NAME(ACI_NABM_ ## x ## _PICB), \
    NAME(ACI_NABM_ ## x ## _PIV), \
    NAME(ACI_NABM_ ## x ## _CR)
    ACI_NABM_REGS_NAME(PI),
    ACI_NABM_REGS_NAME(PO),
    ACI_NABM_REGS_NAME(MC),
    NAME(ACI_NABM_GLOB_CNT),
    NAME(ACI_NABM_GLOB_STA),
    NAME(ACI_NABM_CAS),
    ACI_NABM_REGS_NAME(SO),
#undef ACI_NABM_REGS_NAME
};

