
enum {
        NV_PTIMER_INTR_0                            = 0x00000100,
#define NV_PTIMER_INTR_0_ALARM                                                  0x00000001
#define NV_PTIMER_INTR_0_ALARM__BITSHIFT                                        0
#define NV_PTIMER_INTR_0_ALARM_NOT_PENDING                                      0x00000000
#define NV_PTIMER_INTR_0_ALARM_PENDING                                          0x00000001
#define NV_PTIMER_INTR_0_ALARM_RESET                                            0x00000001
        NV_PTIMER_INTR_EN_0                         = 0x00000140,
#define NV_PTIMER_INTR_EN_0_ALARM                                               0x00000001
#define NV_PTIMER_INTR_EN_0_ALARM__BITSHIFT                                     0
#define NV_PTIMER_INTR_EN_0_ALARM_DISABLED                                      0x00000000
#define NV_PTIMER_INTR_EN_0_ALARM_ENABLED                                       0x00000001
        NV_PTIMER_NUMERATOR                         = 0x00000200,
#define NV_PTIMER_NUMERATOR_VALUE                                               0x0000ffff
#define NV_PTIMER_NUMERATOR_VALUE__BITSHIFT                                     0
#define NV_PTIMER_NUMERATOR_VALUE_0                                             0x00000000
        NV_PTIMER_DENOMINATOR                       = 0x00000210,
#define NV_PTIMER_DENOMINATOR_VALUE                                             0x0000ffff
#define NV_PTIMER_DENOMINATOR_VALUE__BITSHIFT                                   0
#define NV_PTIMER_DENOMINATOR_VALUE_0                                           0x00000000
        NV_PTIMER_TIME_0                            = 0x00000400,
#define NV_PTIMER_TIME_0_NSEC                                                   0xffffffe0
#define NV_PTIMER_TIME_0_NSEC__BITSHIFT                                         5
        NV_PTIMER_TIME_1                            = 0x00000410,
#define NV_PTIMER_TIME_1_NSEC                                                   0x1fffffff
#define NV_PTIMER_TIME_1_NSEC__BITSHIFT                                         0
        NV_PTIMER_ALARM_0                           = 0x00000420,
#define NV_PTIMER_ALARM_0_NSEC                                                  0xffffffe0
#define NV_PTIMER_ALARM_0_NSEC__BITSHIFT                                        5
};

static const char *const nv2a_ptimer_name[] = {
    NAMER(NV_PTIMER_INTR_0),
    NAMER(NV_PTIMER_INTR_EN_0),
    NAMER(NV_PTIMER_NUMERATOR),
    NAMER(NV_PTIMER_DENOMINATOR),
    NAMER(NV_PTIMER_TIME_0),
    NAMER(NV_PTIMER_TIME_1),
    NAMER(NV_PTIMER_ALARM_0),
};

