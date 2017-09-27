
#include <windef.h>
#include <winbase.h>

#define SYNC_THREADS        1   /* debugging */

#if SYNC_THREADS
static pthread_mutex_t      xboxkrnl_mutex_thread = PTHREAD_MUTEX_INITIALIZER;
# define THREAD_LOCK        pthread_mutex_lock(&xboxkrnl_mutex_thread)
# define THREAD_UNLOCK      pthread_mutex_unlock(&xboxkrnl_mutex_thread)
#else
# define THREAD_LOCK
# define THREAD_UNLOCK
#endif

#define NAME(x)             [x] = #x

//#define STUB(x)             debug("stub: %s = %p", #x, (x))
//#define DUMP(x)             debug("dump: %s = %p", #x, (x))
#define VAR_IN              0
#define VAR_OUT             1
#define STUB                2
#define DUMP                3

#define DEBUG(x,y)          DEBUG3(x,y,0,0)
/* value name w/o flags: z = const *char[] name in relation to y; f = 0 */
#define DEBUG2(x,y,z)       DEBUG3(x,y,z,0)
/* value name w/ flags:  z == const *char[] index masking y;      f = 1 */
#define DEBUG3(x,y,z,f) do { \
            unsigned long long v = 0; \
            if (sizeof(typeof((y))) == 8) memcpy(&v, &(y), 8); \
            else memcpy(&v, &(y), sizeof(typeof((y)))); \
            char *c = NULL; \
            unsigned t = (unsigned)(z); \
            if (t) t = sizeof((z))/sizeof(char *); \
            if (!f && t) { /* set c w/o flags */ \
                if (t) t = (v < t); \
                if (t) c = ((char **)(z))[v]; \
            } else if (t) { /* set c w/ flags */ \
                char *n; \
                size_t l, len; \
                typeof(t) i; \
                for (l = 0, i = 0; i <= t; ++i) { \
                    if (i == t) { \
                        if (l) c[l] = 0; \
                        t = !!c; \
                        break; \
                    } \
                    if (l) ++l; \
                    if ((n = ((char **)(z))[i]) && v & i) { \
                        len = strlen(n); \
                        c = realloc(c, l + len + 2); \
                        strncpy(&c[l], n, len + 2); \
                        l += len; \
                        c[l] = '|'; \
                    } \
                } \
            } \
            unsigned s = xboxkrnl_stack * 4; \
            char *st = (s)?malloc(s+1):NULL; \
            if (st) memset(st, ' ', s), st[s] = 0; \
            debug("%s%s: *%s = 0x%llx (%llu)%s%s%s", \
                (st) ? st : "", \
                ((x) > 1) ? (((x) == STUB)?"stub":"dump") : ((x)?" out":"  in"), \
                #y, \
                v, \
                v, \
                (c) ? " (" : "", \
                (c) ?    c : "", \
                (c) ?  ")" : ""); \
            if (f && c) free(c); \
        } while (0)

//#define ENTER debug("enter: --> %s()", __func__)
//#define LEAVE debug("leave: %s() -->", __func__)
#define ENTER2(x,y,z) do { \
            if (z) THREAD_LOCK; \
            debug("enter%s%s%s: --> %s()", \
                (y) ? " (" : "", \
                (y) ?  (y) : "", \
                (y) ?  ")" : "", \
                __func__); \
            if (++(x) & 0x80000000) (x) = 0; \
        } while (0)
#define LEAVE2(x,y,z) do { \
            if (--(x) & 0x80000000) (x) = 0; \
            debug("leave%s%s%s: <-- %s()", \
                (y) ? " (" : "", \
                (y) ?  (y) : "", \
                (y) ?  ")" : "", \
                __func__); \
            if (z) THREAD_UNLOCK; \
        } while (0)

#define INT3                __asm__ __volatile__("int3")
#define STDCALL             __attribute__((__stdcall__))
#define FASTCALL            __attribute__((__fastcall__))

#define ENTER               ENTER2(xboxkrnl_stack, NULL, 1)
#define LEAVE               LEAVE2(xboxkrnl_stack, NULL, 1)
static int                  xboxkrnl_stack = 0;

#define ENTER_DPC           ENTER2(xboxkrnl_dpc_stack, "dpc", 0)
#define LEAVE_DPC           LEAVE2(xboxkrnl_dpc_stack, "dpc", 0)
#define DPC_SIGNAL          pthread_cond_signal(xboxkrnl_dpc_cond)
static int                  xboxkrnl_dpc_stack = 0;
static pthread_t *          xboxkrnl_dpc_thread = NULL;
static pthread_mutexattr_t *xboxkrnl_dpc_mattr = NULL;
static pthread_mutex_t *    xboxkrnl_dpc_mutex = NULL;
static pthread_cond_t *     xboxkrnl_dpc_cond = NULL;

typedef struct _listentry {
    struct _listentry *     Flink;
    struct _listentry *     Blink;
} xboxkrnl_listentry;

#define XBOXKRNL_LISTENTRY_INIT(x) \
        (x)->Flink = (x)->Blink = (x)
#define XBOXKRNL_LISTENTRY_EMPTY(x) \
        ((x)->Flink == (x))
#define XBOXKRNL_LISTENTRY_INSERTHEAD(x,y) \
        (y)->Flink = (x)->Flink, \
        (y)->Blink = (x), \
        (x)->Flink->Blink = (y), \
        (x)->Flink = (y)
#define XBOXKRNL_LISTENTRY_INSERTTAIL(x,y) \
        (y)->Flink = (x), \
        (y)->Blink = (x)->Blink, \
        (x)->Blink->Flink = (y), \
        (x)->Blink = (y)
#define XBOXKRNL_LISTENTRY_REMOVEENTRY(x) \
        if ((x)->Flink) \
            (x)->Blink->Flink = (x)->Flink, \
            (x)->Flink->Blink = (x)->Blink
#define XBOXKRNL_LISTENTRY_REMOVEHEAD(x) \
        XBOXKRNL_LISTENTRY_REMOVEENTRY((x)->Flink)
#define XBOXKRNL_LISTENTRY_REMOVETAIL(x) \
        XBOXKRNL_LISTENTRY_REMOVEENTRY((x)->Blink)

typedef struct {
    int16_t                 Type;
    uint8_t                 Number;
    uint8_t                 Importance;
    xboxkrnl_listentry *    DpcListEntry;
    void *                  DeferredRoutine;
    void *                  DeferredContext;
    void *                  SystemArgument1;
    void *                  SystemArgument2;
    void *                  Lock;
} xboxkrnl_dpc;

static xboxkrnl_dpc *       xboxkrnl_dpc_list = NULL;
static size_t               xboxkrnl_dpc_list_sz = 0;

#define ENTER_EVENT         ENTER2(xboxkrnl_event_stack, "event", 0)
#define LEAVE_EVENT         LEAVE2(xboxkrnl_event_stack, "event", 0)
#define EVENT_SIGNAL        pthread_cond_signal(xboxkrnl_event_cond)
static int                  xboxkrnl_event_stack = 0;
static pthread_t *          xboxkrnl_event_thread = NULL;
static pthread_mutexattr_t *xboxkrnl_event_mattr = NULL;
static pthread_mutex_t *    xboxkrnl_event_mutex = NULL;
static pthread_cond_t *     xboxkrnl_event_cond = NULL;

typedef struct {
    struct _dispatcherheader {
        uint8_t             Type;
        uint8_t             Absolute;
        uint8_t             Size;
        uint8_t             Inserted;
        int32_t             SignalState;
        xboxkrnl_listentry  WaitListHead;
    }                       Header;
    uint64_t                DueTime;
    xboxkrnl_listentry      TimerListEntry;
    xboxkrnl_dpc *          Dpc;
    int32_t                 Period;
} xboxkrnl_timer;

typedef struct {
    uint32_t                bus;
    uint32_t                device;
    uint32_t                function;
    uint32_t                irq;

    char *                  name;

    uint16_t                ioreg_base_0;
    uint16_t                ioreg_size_0;
    uint16_t                ioreg_base_1;
    uint16_t                ioreg_size_1;
    uint16_t                ioreg_base_2;
    uint16_t                ioreg_size_2;
    uint16_t                ioreg_base_3;
    uint16_t                ioreg_size_3;
    uint16_t                ioreg_base_4;
    uint16_t                ioreg_size_4;

    uint32_t                memreg_base_0;
    uint32_t                memreg_size_0;
    uint32_t                memreg_base_1;
    uint32_t                memreg_size_1;
    uint32_t                memreg_base_2;
    uint32_t                memreg_size_2;
    uint32_t                memreg_base_3;
    uint32_t                memreg_size_3;
    uint32_t                memreg_base_4;
    uint32_t                memreg_size_4;
} xpci;

enum {
    XPCICFG_HOSTBRIDGE = 0,
    XPCICFG_LPCBRIDGE,
    XPCICFG_SMBUS,
    XPCICFG_USB0,
    XPCICFG_USB1,
    XPCICFG_NIC,
    XPCICFG_APU,
    XPCICFG_ACI,
    XPCICFG_IDE,
    XPCICFG_AGPBRIDGE,
    XPCICFG_GPU,
    XPCICFG_INTERNAL
};

static const xpci const xpcicfg[] = {
    {
        0, 0, 0, -1,                    /* bus, device, function, irq, name */
        "HOSTBRIDGE",
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   /* ioreg 0-4 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0    /* memreg 0-4 */
    },
    {
        0, 1, 0, -1,
        "LPCBRIDGE",
        0x8000, 0x0100, 0, 0, 0, 0, 0, 0, 0, 0,
             0,      0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 1, 1, -1,
        "SMBUS",
        0, 0, 0xc000, 0x0010, 0xc200, 0x0020, 0, 0, 0, 0,
        0, 0,      0,      0,      0,      0, 0, 0, 0, 0
    },
    {
        0, 2, 0, 1,
        "USB0",
                 0,          0, 0, 0, 0, 0, 0, 0, 0, 0,
        0xfed00000, 0x00001000, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 3, 0, 9,
        "USB1",
                 0,          0, 0, 0, 0, 0, 0, 0, 0, 0,
        0xfed08000, 0x00001000, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 4, 0, 4,
        "NIC",
                 0,          0, 0xe000, 0x0008, 0, 0, 0, 0, 0, 0,
        0xfef00000, 0x00000400,      0,      0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 5, 0, 5,
        "APU",
                 0,          0, 0, 0, 0, 0, 0, 0, 0, 0,
        0xfe800000, 0x00080000, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 6, 0, 6,
        "ACI",
        0xd000, 0x0100, 0xd200, 0x0080,          0,          0, 0, 0, 0, 0,
             0,      0,      0,      0, 0xfec00000, 0x00001000, 0, 0, 0, 0
    },
    {
        0, 9, 0, 14,
        "IDE",
        0, 0, 0, 0, 0, 0, 0, 0, 0xff60, 0x0010,
        0, 0, 0, 0, 0, 0, 0, 0,      0,      0
    },
    {
        0, 30, 0, -1,
        "AGPBRIDGE",
        0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,  0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        1, 0, 0, 3,
        "GPU",
                 0,          0, 0, 0, 0, 0, 0, 0, 0, 0,
        0xfd000000, 0x01000000, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 0, 0, -1,
        NULL,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
};

#if 0
#define XBOXKRNL_FLASH_BASE         0xfffc0000 /* 256 kB rom */
//#define XBOXKRNL_FLASH_BASE         0xfff00000 /* 1 MB rom */
#define XBOXKRNL_FLASH_SIZE         (0xffffffff - XBOXKRNL_FLASH_BASE)

#define LD_SECTION(x,y) char x[y] __attribute__((section("." #x),used))
LD_SECTION(xboxkrnl_flash, XBOXKRNL_FLASH_SIZE - 0x2000); // could disable ASLR for this to work but affects the whole OS; most likely not needed; use `setarch -R` ?
#undef LD_SECTION
#endif

typedef struct {
    void *                  BaseAddress;
    void *                  AllocationBase;
    int32_t                 AllocationProtect;
    size_t                  RegionSize;
    int32_t                 State;
    int32_t                 Protect;
    int32_t                 Type;
} xboxkrnl_meminfo;

typedef struct {
    uint32_t                Length;
    void *                  RootDirectory;
    char *                  ObjectName;
    uint32_t                Attributes;
    void *                  SecurityDescriptor;
    void *                  SecurityQualityOfService;
} xboxkrnl_objattr;

typedef struct {
    union {
        int                 Status;
        void *              Pointer;
    };
    uint32_t                Information;
} xboxkrnl_iostatusblk;

/* enums & names */
enum XC_VALUE_INDEX {
    XC_TIMEZONE_BIAS                = 0,
    XC_TZ_STD_NAME,
    XC_TZ_STD_DATE,
    XC_TZ_STD_BIAS,
    XC_TZ_DLT_NAME,
    XC_TZ_DLT_DATE,
    XC_TZ_DLT_BIAS,
    XC_LANGUAGE,
    XC_VIDEO_FLAGS,
    XC_AUDIO_FLAGS,
    XC_PARENTAL_CONTROL_GAMES,
    XC_PARENTAL_CONTROL_PASSWORD,
    XC_PARENTAL_CONTROL_MOVIES,
    XC_ONLINE_IP_ADDRESS,
    XC_ONLINE_DNS_ADDRESS,
    XC_ONLINE_DEFAULT_GATEWAY_ADDRESS,
    XC_ONLINE_SUBNET_ADDRESS,
    XC_MISC_FLAGS,
    XC_DVD_REGION,

    /* end of user configurations */
    XC_MAX_OS                       = 0xff,

    /* start of factory settings */
    XC_FACTORY_START_INDEX,
    XC_FACTORY_SERIAL_NUMBER = XC_FACTORY_START_INDEX,
    XC_FACTORY_ETHERNET_ADDR,
    XC_FACTORY_ONLINE_KEY,
    XC_FACTORY_AV_REGION,
    XC_FACTORY_GAME_REGION,

    /* end of factory settings */
    XC_MAX_FACTORY                  = 0x1ff,

    /* special value to access the encryption section of the EEPROM
       or to access the entire EEPROM at once */
    XC_ENCRYPTED_SECTION            = 0xfffe,
    XC_MAX_ALL                      = 0xffff
};

static const char *const xboxkrnl_xc_value_name[] = {
    NAME(XC_TIMEZONE_BIAS),
    NAME(XC_TZ_STD_NAME),
    NAME(XC_TZ_STD_DATE),
    NAME(XC_TZ_STD_BIAS),
    NAME(XC_TZ_DLT_NAME),
    NAME(XC_TZ_DLT_DATE),
    NAME(XC_TZ_DLT_BIAS),
    NAME(XC_LANGUAGE),
    NAME(XC_VIDEO_FLAGS),
    NAME(XC_AUDIO_FLAGS),
    NAME(XC_PARENTAL_CONTROL_GAMES),
    NAME(XC_PARENTAL_CONTROL_PASSWORD),
    NAME(XC_PARENTAL_CONTROL_MOVIES),
    NAME(XC_ONLINE_IP_ADDRESS),
    NAME(XC_ONLINE_DNS_ADDRESS),
    NAME(XC_ONLINE_DEFAULT_GATEWAY_ADDRESS),
    NAME(XC_ONLINE_SUBNET_ADDRESS),
    NAME(XC_MISC_FLAGS),
    NAME(XC_DVD_REGION),
    NAME(XC_MAX_OS),
//    NAME(XC_FACTORY_START_INDEX),
    NAME(XC_FACTORY_SERIAL_NUMBER),
    NAME(XC_FACTORY_ETHERNET_ADDR),
    NAME(XC_FACTORY_ONLINE_KEY),
    NAME(XC_FACTORY_AV_REGION),
    NAME(XC_FACTORY_GAME_REGION),
    NAME(XC_MAX_FACTORY),
    NAME(XC_ENCRYPTED_SECTION),
    NAME(XC_MAX_ALL),
};

enum XC_LANGUAGE {
    XC_LANGUAGE_UNKNOWN             = 0,
    XC_LANGUAGE_ENGLISH             = 1,
    XC_LANGUAGE_JAPANESE            = 2,
    XC_LANGUAGE_GERMAN              = 3,
    XC_LANGUAGE_FRENCH              = 4,
    XC_LANGUAGE_SPANISH             = 5,
    XC_LANGUAGE_ITALIAN             = 6,
};

static const char *const xboxkrnl_xc_language_name[] = {
    NAME(XC_LANGUAGE_UNKNOWN),
    NAME(XC_LANGUAGE_ENGLISH),
    NAME(XC_LANGUAGE_JAPANESE),
    NAME(XC_LANGUAGE_GERMAN),
    NAME(XC_LANGUAGE_FRENCH),
    NAME(XC_LANGUAGE_SPANISH),
    NAME(XC_LANGUAGE_ITALIAN),
};

enum XC_VIDEO_FLAGS {
    XC_VIDEO_FLAGS_WIDESCREEN       = 0x00000001,
    XC_VIDEO_FLAGS_HDTV_720p        = 0x00000002,
    XC_VIDEO_FLAGS_HDTV_1080i       = 0x00000004,
    XC_VIDEO_FLAGS_HDTV_480p        = 0x00000008,
    XC_VIDEO_FLAGS_LETTERBOX        = 0x00000010,
    XC_VIDEO_FLAGS_PAL_60Hz         = 0x00000040,
};

static const char *const xboxkrnl_xc_video_flags_name[] = {
    NAME(XC_VIDEO_FLAGS_WIDESCREEN),
    NAME(XC_VIDEO_FLAGS_HDTV_720p),
    NAME(XC_VIDEO_FLAGS_HDTV_1080i),
    NAME(XC_VIDEO_FLAGS_HDTV_480p),
    NAME(XC_VIDEO_FLAGS_LETTERBOX),
    NAME(XC_VIDEO_FLAGS_PAL_60Hz),
};

#define XC_AUDIO_FLAGS_BASIC(c)      ((c) & 0x0000ffff)
#define XC_AUDIO_FLAGS_ENCODED(c)    ((c) & 0xffff0000)
#define XC_AUDIO_FLAGS_COMBINED(b,e) (XC_AUDIO_FLAGS_BASIC(b) | XC_AUDIO_FLAGS_ENCODED(e))

enum XC_AUDIO_FLAGS {
    /* basic */
    XC_AUDIO_FLAGS_STEREO           = 0x00000000,
    XC_AUDIO_FLAGS_MONO             = 0x00000001,
    XC_AUDIO_FLAGS_SURROUND         = 0x00000002,
    /* encoded */
    XC_AUDIO_FLAGS_ENABLE_AC3       = 0x00010000,
    XC_AUDIO_FLAGS_ENABLE_DTS       = 0x00020000,
};

static const char *const xboxkrnl_xc_audio_flags_name[] = {
    NAME(XC_AUDIO_FLAGS_STEREO),
    NAME(XC_AUDIO_FLAGS_MONO),
    NAME(XC_AUDIO_FLAGS_SURROUND),
    NAME(XC_AUDIO_FLAGS_ENABLE_AC3),
    NAME(XC_AUDIO_FLAGS_ENABLE_DTS),
};

enum XC_PARENTAL_CONTROL {
    XC_PC_ESRB_ALL                  = 0,
    XC_PC_ESRB_ADULT                = 1,
    XC_PC_ESRB_MATURE               = 2,
    XC_PC_ESRB_TEEN                 = 3,
    XC_PC_ESRB_EVERYONE             = 4,
    XC_PC_ESRB_KIDS_TO_ADULTS       = 5,
    XC_PC_ESRB_EARLY_CHILDHOOD      = 6,
};

static const char *const xboxkrnl_xc_parental_control_name[] = {
    NAME(XC_PC_ESRB_ALL),
    NAME(XC_PC_ESRB_ADULT),
    NAME(XC_PC_ESRB_MATURE),
    NAME(XC_PC_ESRB_TEEN),
    NAME(XC_PC_ESRB_EVERYONE),
    NAME(XC_PC_ESRB_KIDS_TO_ADULTS),
    NAME(XC_PC_ESRB_EARLY_CHILDHOOD),
};

enum XC_MISC_FLAGS {
    XC_MISC_FLAG_AUTOPOWERDOWN      = 0x00000001,
    XC_MISC_FLAG_DONT_USE_DST       = 0x00000002,
};

static const char *const xboxkrnl_xc_misc_flags_name[] = {
    NAME(XC_MISC_FLAG_AUTOPOWERDOWN),
    NAME(XC_MISC_FLAG_DONT_USE_DST),
};

enum XC_FACTORY_AV_REGION {
    XC_VIDEO_STANDARD_NTSC_M        = 1,
    XC_VIDEO_STANDARD_NTSC_J        = 2,
    XC_VIDEO_STANDARD_PAL_I         = 3,
    XC_VIDEO_STANDARD_PAL_M         = 4,
};

static const char *const xboxkrnl_xc_factory_av_region_name[] = {
    NAME(XC_VIDEO_STANDARD_NTSC_M),
    NAME(XC_VIDEO_STANDARD_NTSC_J),
    NAME(XC_VIDEO_STANDARD_PAL_I),
    NAME(XC_VIDEO_STANDARD_PAL_M),
};

enum XC_FACTORY_GAME_REGION {
    XC_GAME_REGION_NA               = 0x00000001,
    XC_GAME_REGION_JAPAN            = 0x00000002,
    XC_GAME_REGION_RESTOFWORLD      = 0x00000004,
    XC_GAME_REGION_MANUFACTURING    = 0x80000000,
};

static const char *const xboxkrnl_xc_factory_game_region_name[] = {
    NAME(XC_GAME_REGION_NA),
    NAME(XC_GAME_REGION_JAPAN),
    NAME(XC_GAME_REGION_RESTOFWORLD),
//    NAME(XC_GAME_REGION_MANUFACTURING),
};

enum XC_AV_PACK {
    XC_AV_PACK_SCART                = 0,
    XC_AV_PACK_HDTV                 = 1,
    XC_AV_PACK_VGA                  = 2,
    XC_AV_PACK_RFU                  = 3,
    XC_AV_PACK_SVIDEO               = 4,
    XC_AV_PACK_STANDARD             = 6,
    XC_AV_PACK_NONE                 = 7,
};

static const char *const xboxkrnl_xc_av_pack_name[] = {
    NAME(XC_AV_PACK_SCART),
    NAME(XC_AV_PACK_HDTV),
    NAME(XC_AV_PACK_VGA),
    NAME(XC_AV_PACK_RFU),
    NAME(XC_AV_PACK_SVIDEO),
    NAME(XC_AV_PACK_STANDARD),
    NAME(XC_AV_PACK_NONE),
};

static const unsigned xboxkrnl_os[] = {
    [XC_LANGUAGE]                   = XC_LANGUAGE_ENGLISH,
    [XC_VIDEO_FLAGS]                = XC_VIDEO_FLAGS_LETTERBOX,
    [XC_AUDIO_FLAGS]                = XC_AUDIO_FLAGS_STEREO,
    [XC_PARENTAL_CONTROL_GAMES]     = XC_PC_ESRB_ALL, // Zapper queries this.
    [XC_PARENTAL_CONTROL_MOVIES]    = XC_PC_ESRB_ALL, // Xbox Dashboard queries this.
    [XC_MISC_FLAGS]                 = 0,
    [XC_FACTORY_AV_REGION]          = XC_VIDEO_STANDARD_NTSC_M,
    [XC_FACTORY_GAME_REGION]        = /*XC_GAME_REGION_MANUFACTURING | */XC_GAME_REGION_NA,
    [XC_MAX_OS]                     = 0,
};

static const char *const xboxkrnl_os_name[] = {
    NAME(XC_LANGUAGE),
    NAME(XC_VIDEO_FLAGS),
    NAME(XC_AUDIO_FLAGS),
    NAME(XC_PARENTAL_CONTROL_GAMES),
    NAME(XC_PARENTAL_CONTROL_MOVIES),
    NAME(XC_MISC_FLAGS),
    NAME(XC_FACTORY_AV_REGION),
    NAME(XC_FACTORY_GAME_REGION),
    NAME(XC_MAX_OS),
};

/* suppress warnings */
#if 1
void
xboxkrnl_nullsub() {
    (void)xboxkrnl_xc_value_name;
    (void)xboxkrnl_xc_language_name;
    (void)xboxkrnl_xc_video_flags_name;
    (void)xboxkrnl_xc_audio_flags_name;
    (void)xboxkrnl_xc_parental_control_name;
    (void)xboxkrnl_xc_misc_flags_name;
    (void)xboxkrnl_xc_factory_av_region_name;
    (void)xboxkrnl_xc_factory_game_region_name;
    (void)xboxkrnl_xc_av_pack_name;
    (void)xboxkrnl_os_name;
}
#endif

/* initialization */
#if 0
int
xboxkrnl_init(void **x) {
    int ret = 1;
    ENTER;

    if (x) {
        
    }

    LEAVE;
    return ret;
}

void
xboxkrnl_release(void *x) {
    ENTER;

    if (x) {
        
    }

    LEAVE;
}
#endif
void *
xboxkrnl_worker(void *arg) {
    if (!arg) {
        ENTER;

#define XBOXKRNL_THREAD(x,y) do { \
            xboxkrnl_ ## x ## _thread = calloc(1, sizeof(*xboxkrnl_ ## x ## _thread)); \
            xboxkrnl_ ## x ## _mattr = calloc(1, sizeof(*xboxkrnl_ ## x ## _mattr)); \
            xboxkrnl_ ## x ## _mutex = calloc(1, sizeof(*xboxkrnl_ ## x ## _mutex)); \
            xboxkrnl_ ## x ## _cond = calloc(1, sizeof(*xboxkrnl_ ## x ## _cond)); \
            \
            DEBUG(DUMP, xboxkrnl_ ## x ## _thread); \
            DEBUG(DUMP, xboxkrnl_ ## x ## _mattr); \
            DEBUG(DUMP, xboxkrnl_ ## x ## _mutex); \
            DEBUG(DUMP, xboxkrnl_ ## x ## _cond); \
            \
            pthread_mutexattr_init(xboxkrnl_ ## x ## _mattr); \
            pthread_mutexattr_settype(xboxkrnl_ ## x ## _mattr, PTHREAD_MUTEX_RECURSIVE); \
            pthread_mutex_init(xboxkrnl_ ## x ## _mutex, xboxkrnl_ ## x ## _mattr); \
            pthread_cond_init(xboxkrnl_ ## x ## _cond, NULL); \
            \
            debug("    " "/* starting " #x " thread */"); \
            \
            pthread_create(xboxkrnl_ ## x ## _thread, NULL, xboxkrnl_worker, (y)); \
        } while (0)

        XBOXKRNL_THREAD(dpc, (void *)1);
        XBOXKRNL_THREAD(event, (void *)2);

#undef XBOXKRNL_THREAD

        LEAVE;
    } else if (arg == (void *)1) {
        ENTER_DPC;

        for (;;) {
            pthread_mutex_lock(xboxkrnl_dpc_mutex);

            debug("    " "/* waiting for deferred procedure routines */");

            pthread_cond_wait(xboxkrnl_dpc_cond, xboxkrnl_dpc_mutex);

            pthread_mutex_unlock(xboxkrnl_dpc_mutex);
        }

        LEAVE_DPC;
        pthread_exit(NULL);
    } else if (arg == (void *)2) {
        ENTER_EVENT;

        for (;;) {
            pthread_mutex_lock(xboxkrnl_event_mutex);

            debug("    " "/* waiting for event routines */");

            pthread_cond_wait(xboxkrnl_event_cond, xboxkrnl_event_mutex);

            pthread_mutex_unlock(xboxkrnl_event_mutex);
        }

        LEAVE_EVENT;
        pthread_exit(NULL);
    }

    return NULL;
}

int
xboxkrnl_init_hw(void) {
    const uint32_t mem[] = {
        xpcicfg[XPCICFG_USB0].memreg_base_0,
        xpcicfg[XPCICFG_USB0].memreg_size_0,
        0
    };
    void *ptr;
    size_t sz;
    size_t i;

    for (i = 0; i < sizeof(mem) / sizeof(*mem) && mem[i];) {
        ptr = (void *)mem[i++];
        sz = mem[i++];
        if (mmap(ptr,
                sz,
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_FIXED | MAP_NORESERVE | MAP_ANONYMOUS,
                -1,
                0) != ptr) {
            fprintf(stderr, "error: could not mmap hardware emulation @ address %p size %p\n", ptr, (void *)sz);
            return 1;
        }
    }

    xboxkrnl_worker(NULL);

    return 0;
}

/* prototypes */
STDCALL uint8_t
xboxkrnl_KeSetTimerEx( /* 150 (0x096) */
        /* in */       xboxkrnl_timer *Timer,
        /* in */       int64_t DueTime,
        /* in (opt) */ int32_t Period,
        /* in (opt) */ xboxkrnl_dpc *Dpc);
STDCALL int
xboxkrnl_PsCreateSystemThreadEx( /* 255 (0x0ff) */
        /* out */       void **ThreadHandle,
        /* in */        size_t ThreadExtensionSize,
        /* in */        size_t KernelStackSize,
        /* in */        size_t TlsDataSize,
        /* out (opt) */ void **ThreadId,
        /* in */        void *StartRoutine,
        /* in */        void *StartContext,
        /* in */        int CreateSuspended,
        /* in */        int DebuggerThread,
        /* in (opt) */  void *SystemRoutine);

/* wrappers */
STDCALL void
xboxkrnl_AvGetSavedDataAddress() { /* 001 (0x001) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_AvSendTVEncoderOption() { /* 002 (0x002) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_AvSetDisplayMode() { /* 003 (0x003) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_AvSetSavedDataAddress() { /* 004 (0x004) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_DbgBreakPoint() { /* 005 (0x005) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_DbgBreakPointWithStatus() { /* 006 (0x006) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_DbgPrint() { /* 008 (0x008) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_DbgPrompt() { /* 010 (0x00a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ExAcquireReadWriteLockExclusive() { /* 012 (0x00c) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ExAcquireReadWriteLockShared() { /* 013 (0x00d) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void *
xboxkrnl_ExAllocatePool( /* 014 (0x00e) */
        /* in */ size_t NumberOfBytes) {
    void *ret;
    ENTER;
    DEBUG(VAR_IN, NumberOfBytes);

    if (!(ret = malloc(NumberOfBytes)) && NumberOfBytes) {
        debug("error: out of memory", 0);
        INT3;
    }
    DEBUG(DUMP, ret);

    LEAVE;
    return ret;
}
STDCALL void
xboxkrnl_ExAllocatePoolWithTag() { /* 015 (0x00f) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ExEventObjectType() { /* 016 (0x010) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ExFreePool( /* 017 (0x011) */
        /* in */ void *P) {
    ENTER;
    DEBUG(VAR_IN, P);

    free(P);

    LEAVE;
}
STDCALL void
xboxkrnl_ExInitializeReadWriteLock() { /* 018 (0x012) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ExInterlockedAddLargeInteger() { /* 019 (0x013) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_ExInterlockedAddLargeStatistic() { /* 020 (0x014) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_ExInterlockedCompareExchange64() { /* 021 (0x015) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ExMutantObjectType() { /* 022 (0x016) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ExQueryPoolBlockSize() { /* 023 (0x017) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL int
xboxkrnl_ExQueryNonVolatileSetting( /* 024 (0x018) */
        /* in */  unsigned ValueIndex,
        /* out */ unsigned *Type,
        /* out */ void *Value,
        /* in */  unsigned ValueLength,
        /* out */ unsigned *ResultLength) {
    unsigned val = 0;
    int ret = 0;
    ENTER;
    DEBUG2(VAR_IN, ValueIndex, xboxkrnl_os_name);
    DEBUG(VAR_OUT, Type);
    DEBUG(VAR_OUT, Value);
    DEBUG(VAR_IN,  ValueLength);
    DEBUG(VAR_OUT, ResultLength);

    switch (ValueIndex) {
    case XC_LANGUAGE:
    case XC_VIDEO_FLAGS:
    case XC_AUDIO_FLAGS:
    case XC_PARENTAL_CONTROL_GAMES:
    case XC_PARENTAL_CONTROL_MOVIES:
    case XC_MISC_FLAGS:
    case XC_FACTORY_AV_REGION:
    case XC_FACTORY_GAME_REGION:
        if (ValueLength < sizeof(val)) {
            ret = 1;
            break;
        }
        if (ResultLength) *ResultLength = sizeof(val);
        *Type = REG_DWORD;
        val = xboxkrnl_os[ValueIndex];
        *(typeof(val) *)Value = val;
        DEBUG(DUMP, val);
        break;
    case XC_ENCRYPTED_SECTION:
    case XC_MAX_ALL:
        if (ValueIndex == XC_MAX_ALL || ValueIndex == XC_ENCRYPTED_SECTION) {
#if 0
            val = (ValueIndex == XC_MAX_ALL)
                    ? 256 /* EEPROM_TOTAL_MEMORY_SIZE */
                    : 48; /* EEPROM_ENCRYPTED_SECTION_SIZE */
            if (ValueLength < val) {
                ret = STATUS_BUFFER_TOO_SMALL;
                break;
            }
            if (ResultLength) *ResultLength = val;
            *Type = REG_BINARY;
            //TODO eeprom copy from offset 0 length val
#else
            ret = 1;INT3;
#endif
        }
        break;
    default:
        ret = 1;INT3;
        break;
    }

    LEAVE;
    return ret;
}
STDCALL void
xboxkrnl_ExReadWriteRefurbInfo() { /* 025 (0x019) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ExRaiseException() { /* 026 (0x01a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ExRaiseStatus() { /* 027 (0x01b) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ExReleaseReadWriteLock() { /* 028 (0x01c) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ExSaveNonVolatileSetting() { /* 029 (0x01d) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ExSemaphoreObjectType() { /* 030 (0x01e) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ExTimerObjectType() { /* 031 (0x01f) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ExfInterlockedInsertHeadList() { /* 032 (0x020) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_ExfInterlockedInsertTailList() { /* 033 (0x021) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_ExfInterlockedRemoveHeadList() { /* 034 (0x022) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_FscGetCacheSize() { /* 035 (0x023) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_FscInvalidateIdleBlocks() { /* 036 (0x024) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_FscSetCacheSize() { /* 037 (0x025) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_HalBootSMCVideoMode() { /* 356 (0x164) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_HalClearSoftwareInterrupt() { /* 038 (0x026) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_HalDisableSystemInterrupt() { /* 039 (0x027) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_HalDiskCachePartitionCount() { /* 040 (0x028) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_HalDiskModelNumber() { /* 041 (0x029) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_HalDiskSerialNumber() { /* 042 (0x02a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_HalEnableSecureTrayEject() { /* 365 (0x16d) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_HalEnableSystemInterrupt() { /* 043 (0x02b) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL unsigned
xboxkrnl_HalGetInterruptVector( /* 044 (0x02c) */
        /* in */  unsigned BusInterruptLevel,
        /* out */ void *Irql) {
    int i;
    ENTER;
    DEBUG(STUB,BusInterruptLevel);DEBUG(STUB,Irql);

    for (i = 0; xpcicfg[i].name; ++i) {
        if (xpcicfg[i].irq == BusInterruptLevel) {
            debug("    name: '%s'", xpcicfg[i].name);//TODO
            break;
        }
    }

    LEAVE;
    return 0;
}
STDCALL void
xboxkrnl_HalInitiateShutdown() { /* 360 (0x168) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_HalIsResetOrShutdownPending() { /* 358 (0x166) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_HalReadSMBusValue() { /* 045 (0x02d) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_HalReadSMCTrayState() { /* 009 (0x009) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_HalReadWritePCISpace() { /* 046 (0x02e) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_HalRegisterShutdownNotification( /* 047 (0x02f) */
        /* in */ void *ShutdownRegistration,
        /* in */ int Register) {
    ENTER;
    DEBUG(STUB,ShutdownRegistration);DEBUG(STUB,Register);

    LEAVE;
}
FASTCALL void
xboxkrnl_HalRequestSoftwareInterrupt() { /* 048 (0x030) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_HalReturnToFirmware() { /* 049 (0x031) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_HalWriteSMBusValue() { /* 050 (0x032) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_HalWriteSMCScratchRegister() { /* 366 (0x16e) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IdexChannelObject() { /* 357 (0x165) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_InterlockedCompareExchange() { /* 051 (0x033) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_InterlockedDecrement() { /* 052 (0x034) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_InterlockedIncrement() { /* 053 (0x035) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_InterlockedExchange() { /* 054 (0x036) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_InterlockedExchangeAdd() { /* 055 (0x037) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_InterlockedFlushSList() { /* 056 (0x038) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_InterlockedPopEntrySList() { /* 057 (0x039) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_InterlockedPushEntrySList() { /* 058 (0x03a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoAllocateIrp() { /* 059 (0x03b) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoBuildAsynchronousFsdRequest() { /* 060 (0x03c) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoBuildDeviceIoControlRequest() { /* 061 (0x03d) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoBuildSynchronousFsdRequest() { /* 062 (0x03e) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoCheckShareAccess() { /* 063 (0x03f) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoCompletionObjectType() { /* 064 (0x040) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoCreateDevice() { /* 065 (0x041) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoCreateFile() { /* 066 (0x042) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoCreateSymbolicLink() { /* 067 (0x043) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoDeleteDevice() { /* 068 (0x044) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoDeleteSymbolicLink() { /* 069 (0x045) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoDeviceObjectType() { /* 070 (0x046) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoDismountVolume() { /* 090 (0x05a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoDismountVolumeByName() { /* 091 (0x05b) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoFileObjectType() { /* 071 (0x047) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoFreeIrp() { /* 072 (0x048) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoInitializeIrp() { /* 073 (0x049) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoInvalidDeviceRequest() { /* 074 (0x04a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoMarkIrpMustComplete() { /* 359 (0x167) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoQueryFileInformation() { /* 075 (0x04b) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoQueryVolumeInformation() { /* 076 (0x04c) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoQueueThreadIrp() { /* 077 (0x04d) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoRemoveShareAccess() { /* 078 (0x04e) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoSetIoCompletion() { /* 079 (0x04f) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoSetShareAccess() { /* 080 (0x050) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoStartNextPacket() { /* 081 (0x051) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoStartNextPacketByKey() { /* 082 (0x052) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoStartPacket() { /* 083 (0x053) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoSynchronousDeviceIoControlRequest() { /* 084 (0x054) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_IoSynchronousFsdRequest() { /* 085 (0x055) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_IofCallDriver() { /* 086 (0x056) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_IofCompleteRequest() { /* 087 (0x057) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KdDebuggerEnabled() { /* 088 (0x058) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KdDebuggerNotPresent() { /* 089 (0x059) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeAlertResumeThread() { /* 092 (0x05c) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeAlertThread() { /* 093 (0x05d) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeBoostPriorityThread() { /* 094 (0x05e) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeBugCheck() { /* 095 (0x05f) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeBugCheckEx() { /* 096 (0x060) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeCancelTimer() { /* 097 (0x061) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL int
xboxkrnl_KeConnectInterrupt( /* 098 (0x062) */
        /* in */ void *Interrupt) {
    int ret = 0;
    ENTER;
    DEBUG(STUB,Interrupt);

    LEAVE;
    return ret;
}
STDCALL int
xboxkrnl_KeDelayExecutionThread( /* 099 (0x063) */
        /* in */ char WaitMode,
        /* in */ int Alertable,
        /* in */ void *Interval) {
    struct timespec tp;
    int64_t nsec;
    ENTER;
    DEBUG(VAR_IN, WaitMode);
    DEBUG(VAR_IN, Alertable);
    DEBUG(VAR_IN, Interval);

    nsec = *(typeof(nsec) *)Interval * 100;

    if (nsec < 0) {
        nsec = -nsec;
        tp.tv_sec = nsec / 1000000000;
        tp.tv_nsec = nsec % 1000000000;
        DEBUG(DUMP, tp.tv_sec);
        DEBUG(DUMP, tp.tv_nsec);
        nanosleep(&tp, NULL);
    } else if (nsec > 0) {
        INT3;
    }

    LEAVE;
    return 0;
}
STDCALL void
xboxkrnl_KeDisconnectInterrupt() { /* 100 (0x064) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeEnterCriticalRegion() { /* 101 (0x065) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeGetCurrentIrql() { /* 103 (0x067) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeGetCurrentThread() { /* 104 (0x068) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeInitializeApc() { /* 105 (0x069) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeInitializeDeviceQueue() { /* 106 (0x06a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeInitializeDpc( /* 107 (0x06b) */
        /* in */ xboxkrnl_dpc *Dpc,
        /* in */ void *DeferredRoutine,
        /* in */ void *DeferredContext) {
    ENTER;
    DEBUG(VAR_IN, Dpc);
    DEBUG(VAR_IN, DeferredRoutine);
    DEBUG(VAR_IN, DeferredContext);

    memset(Dpc, 0, sizeof(*Dpc));

    Dpc->Type = /* DpcObject */ 19;
    Dpc->DeferredRoutine = DeferredRoutine;
    Dpc->DeferredContext = DeferredContext;

    LEAVE;
}
STDCALL void
xboxkrnl_KeInitializeEvent() { /* 108 (0x06c) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeInitializeInterrupt( /* 109 (0x06d) */
        /* in */ void *Interrupt,
        /* in */ void *ServiceRoutine,
        /* in */ void *ServiceContext,
        /* in */ unsigned Vector,
        /* in */ unsigned char Irql,
        /* in */ int InterruptMode,
        /* in */ int ShareVector) {
    ENTER;
    DEBUG(STUB,Interrupt);DEBUG(STUB,ServiceRoutine);DEBUG(STUB,ServiceContext);DEBUG(STUB,Vector);DEBUG(STUB,Irql);DEBUG(STUB,InterruptMode);DEBUG(STUB,ShareVector);

    LEAVE;
}
STDCALL void
xboxkrnl_KeInitializeMutant() { /* 110 (0x06e) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeInitializeQueue() { /* 111 (0x06f) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeInitializeSemaphore() { /* 112 (0x070) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeInitializeTimerEx( /* 113 (0x071) */
        /* in */ xboxkrnl_timer *Timer,
        /* in */ int Type) {
    ENTER;
    DEBUG(VAR_IN, Timer);
    DEBUG(VAR_IN, Type);

    memset(Timer, 0, sizeof(*Timer));

    Timer->Header.Type = /* TimerNotificationObject */ 8 + Type;
    Timer->Header.Size = sizeof(*Timer) / 4;
    XBOXKRNL_LISTENTRY_INIT(&Timer->Header.WaitListHead);

    LEAVE;
}
STDCALL void
xboxkrnl_KeInsertByKeyDeviceQueue() { /* 114 (0x072) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeInsertDeviceQueue() { /* 115 (0x073) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeInsertHeadQueue() { /* 116 (0x074) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeInsertQueue() { /* 117 (0x075) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeInsertQueueApc() { /* 118 (0x076) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeInsertQueueDpc() { /* 119 (0x077) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeInterruptTime() { /* 120 (0x078) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeIsExecutingDpc() { /* 121 (0x079) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeLeaveCriticalRegion() { /* 122 (0x07a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KePulseEvent() { /* 123 (0x07b) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeQueryBasePriorityThread() { /* 124 (0x07c) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeQueryInterruptTime() { /* 125 (0x07d) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeQueryPerformanceCounter() { /* 126 (0x07e) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeQueryPerformanceFrequency() { /* 127 (0x07f) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeQuerySystemTime() { /* 128 (0x080) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL unsigned char
xboxkrnl_KeRaiseIrqlToDpcLevel(void) { /* 129 (0x081) */
    unsigned char ret = 0;
    ENTER;

    DEBUG(DUMP, ret);

    LEAVE;
    return ret;
}
STDCALL void
xboxkrnl_KeRaiseIrqlToSynchLevel() { /* 130 (0x082) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeReleaseMutant() { /* 131 (0x083) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeReleaseSemaphore() { /* 132 (0x084) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeRemoveByKeyDeviceQueue() { /* 133 (0x085) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeRemoveDeviceQueue() { /* 134 (0x086) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeRemoveEntryDeviceQueue() { /* 135 (0x087) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeRemoveQueue() { /* 136 (0x088) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeRemoveQueueDpc() { /* 137 (0x089) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeResetEvent() { /* 138 (0x08a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeRestoreFloatingPointState() { /* 139 (0x08b) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeResumeThread() { /* 140 (0x08c) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeRundownQueue() { /* 141 (0x08d) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeSaveFloatingPointState() { /* 142 (0x08e) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeSetBasePriorityThread() { /* 143 (0x08f) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeSetDisableBoostThread() { /* 144 (0x090) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeSetEvent() { /* 145 (0x091) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeSetEventBoostPriority() { /* 146 (0x092) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeSetPriorityProcess() { /* 147 (0x093) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeSetPriorityThread() { /* 148 (0x094) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL uint8_t
xboxkrnl_KeSetTimer( /* 149 (0x095) */
        /* in */       xboxkrnl_timer *Timer,
        /* in */       int64_t DueTime,
        /* in (opt) */ xboxkrnl_dpc *Dpc) {
    return xboxkrnl_KeSetTimerEx(Timer, DueTime, 0, Dpc);
}
STDCALL uint8_t
xboxkrnl_KeSetTimerEx( /* 150 (0x096) */
        /* in */       xboxkrnl_timer *Timer,
        /* in */       int64_t DueTime,
        /* in (opt) */ int32_t Period,
        /* in (opt) */ xboxkrnl_dpc *Dpc) {
    int ret = 0;
    ENTER;
    DEBUG(VAR_IN, Timer);
    DEBUG(VAR_IN, DueTime);
    DEBUG(VAR_IN, Period);
    DEBUG(VAR_IN, Dpc);

    if (Timer->Header.Inserted) {
        Timer->Header.Inserted = 0;
        XBOXKRNL_LISTENTRY_REMOVEENTRY(&Timer->TimerListEntry);
    }
    Timer->Header.SignalState = 0;
    Timer->Dpc = Dpc;
    Timer->Period = Period;

//    INT3;//TODO

    LEAVE;
    return ret;
}
STDCALL void
xboxkrnl_KeStallExecutionProcessor( /* 151 (0x097) */
        /* in */ unsigned MicroSeconds) {
    ENTER;
    DEBUG(VAR_IN, MicroSeconds);

    usleep(MicroSeconds);

    LEAVE;
}
STDCALL void
xboxkrnl_KeSuspendThread() { /* 152 (0x098) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeSynchronizeExecution() { /* 153 (0x099) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeSystemTime() { /* 154 (0x09a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeTestAlertThread() { /* 155 (0x09b) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeTickCount() { /* 156 (0x09c) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeTimeIncrement() { /* 157 (0x09d) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeWaitForMultipleObjects() { /* 158 (0x09e) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_KeWaitForSingleObject() { /* 159 (0x09f) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_KfRaiseIrql() { /* 160 (0x0a0) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_KfLowerIrql( /* 161 (0x0a1) */
        /* in */ unsigned char NewIrql) {
    ENTER;
    DEBUG(STUB,NewIrql);

    LEAVE;
}
STDCALL void
xboxkrnl_KiBugCheckData() { /* 162 (0x0a2) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_KiUnlockDispatcherDatabase() { /* 163 (0x0a3) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_LaunchDataPage() { /* 164 (0x0a4) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void *
xboxkrnl_MmAllocateContiguousMemory( /* 165 (0x0a5) */
        /* in */ size_t NumberOfBytes) {
    void *ret;
    ENTER;
    DEBUG(VAR_IN, NumberOfBytes);

    if (!(ret = malloc(NumberOfBytes)) && NumberOfBytes) {
        debug("error: out of memory", 0);
        INT3;
    }
    DEBUG(DUMP, ret);

    LEAVE;
    return ret;
}
STDCALL void
xboxkrnl_MmAllocateContiguousMemoryEx() { /* 166 (0x0a6) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmAllocateSystemMemory() { /* 167 (0x0a7) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmClaimGpuInstanceMemory() { /* 168 (0x0a8) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmCreateKernelStack() { /* 169 (0x0a9) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmDeleteKernelStack() { /* 170 (0x0aa) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmFreeContiguousMemory() { /* 171 (0x0ab) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmFreeSystemMemory() { /* 172 (0x0ac) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void *
xboxkrnl_MmGetPhysicalAddress( /* 173 (0x0ad) */
        /* in */ void *BaseAddress) {
    ENTER;
    DEBUG(VAR_IN, BaseAddress);

    DEBUG(DUMP, BaseAddress);

    LEAVE;
    return BaseAddress;
}
STDCALL void
xboxkrnl_MmGlobalData() { /* 102 (0x066) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmIsAddressValid() { /* 174 (0x0ae) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmLockUnlockBufferPages( /* 175 (0x0af) */
        /* in */ void *BaseAddress,
        /* in */ size_t NumberOfBytes,
        /* in */ int UnlockPages) {
    ENTER;
    DEBUG(STUB,BaseAddress);DEBUG(STUB,NumberOfBytes);DEBUG(STUB,UnlockPages);

    LEAVE;
}
STDCALL void
xboxkrnl_MmLockUnlockPhysicalPage() { /* 176 (0x0b0) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmMapIoSpace() { /* 177 (0x0b1) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmPersistContiguousMemory() { /* 178 (0x0b2) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmQueryAddressProtect() { /* 179 (0x0b3) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmQueryAllocationSize() { /* 180 (0x0b4) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmQueryStatistics() { /* 181 (0x0b5) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmSetAddressProtect() { /* 182 (0x0b6) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmUnmapIoSpace() { /* 183 (0x0b7) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL int
xboxkrnl_NtAllocateVirtualMemory( /* 184 (0x0b8) */
        /* i/o */ void **BaseAddress,
        /* in */  uint32_t *ZeroBits,
        /* i/o */ size_t *RegionSize,
        /* in */  uint32_t AllocationType,
        /* in */  uint32_t Protect) {
    int flag = 0;
    int ret = 0;
    ENTER;
    DEBUG(VAR_IN, BaseAddress);
    DEBUG(VAR_IN, ZeroBits);
    DEBUG(VAR_IN, RegionSize);
    DEBUG(VAR_IN, AllocationType);
    DEBUG(VAR_IN, Protect);
    DEBUG(VAR_OUT, *BaseAddress);
    DEBUG(VAR_OUT, *RegionSize);

    if (Protect & /* PAGE_NOCACHE */ 0x200) flag |= MAP_NORESERVE;

    if ((*BaseAddress = mmap(*BaseAddress,
            *RegionSize,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS | flag,
            -1,
            0)) == MAP_FAILED) {
        *BaseAddress = NULL;
        ret = -1;
        INT3;
    }

    DEBUG(DUMP, *BaseAddress);
    DEBUG(DUMP, *RegionSize);
    LEAVE;
    return ret;
}
STDCALL void
xboxkrnl_NtCancelTimer() { /* 185 (0x0b9) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtClearEvent() { /* 186 (0x0ba) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtClose() { /* 187 (0x0bb) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtCreateDirectoryObject() { /* 188 (0x0bc) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtCreateEvent() { /* 189 (0x0bd) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtCreateFile() { /* 190 (0x0be) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtCreateIoCompletion() { /* 191 (0x0bf) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtCreateMutant() { /* 192 (0x0c0) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtCreateSemaphore() { /* 193 (0x0c1) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtCreateTimer() { /* 194 (0x0c2) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtDeleteFile() { /* 195 (0x0c3) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtDeviceIoControlFile() { /* 196 (0x0c4) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtDuplicateObject() { /* 197 (0x0c5) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtFlushBuffersFile() { /* 198 (0x0c6) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtFreeVirtualMemory() { /* 199 (0x0c7) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtFsControlFile() { /* 200 (0x0c8) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtOpenDirectoryObject() { /* 201 (0x0c9) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL int
xboxkrnl_NtOpenFile( /* 202 (0x0ca) */
        /* out */ void **FileHandle,
        /* in */  uint32_t DesiredAccess,
        /* in */  xboxkrnl_objattr *ObjectAttributes,
        /* out */ xboxkrnl_iostatusblk *IoStatusBlock,
        /* in */  uint32_t ShareAccess,
        /* in */  uint32_t OpenOptions) {
    int ret = 0;
    ENTER;
    DEBUG(VAR_OUT, FileHandle);
    DEBUG(VAR_IN, DesiredAccess);
    DEBUG(VAR_IN, ObjectAttributes);
    DEBUG(VAR_OUT, IoStatusBlock);
    DEBUG(VAR_IN, ShareAccess);
    DEBUG(VAR_IN, OpenOptions);
#if 0
typedef struct {
    uint32_t                Length;
    void *                  RootDirectory;
    char *                  ObjectName;
    uint32_t                Attributes;
    void *                  SecurityDescriptor;
    void *                  SecurityQualityOfService;
} xboxkrnl_objattr;

typedef struct {
    union {
        int                 Status;
        void *              Pointer;
    };
    uint32_t                Information;
} xboxkrnl_iostatusblk;
#endif
    DEBUG(DUMP, ObjectAttributes->Length);
    DEBUG(DUMP, ObjectAttributes->RootDirectory);
    DEBUG(DUMP, ObjectAttributes->ObjectName);
    DEBUG(DUMP, ObjectAttributes->Attributes);
    DEBUG(DUMP, ObjectAttributes->SecurityDescriptor);
    DEBUG(DUMP, ObjectAttributes->SecurityQualityOfService);
    DEBUG(DUMP, IoStatusBlock->Status);
    DEBUG(DUMP, IoStatusBlock->Information);

    if (DesiredAccess & /* SYNCHRONIZE */ 0x100000 &&
        OpenOptions & /* FILE_OPEN_FOR_FREE_SPACE_QUERY */ 0x800000) {
        
    }

    INT3;
    LEAVE;
    return ret;
}
STDCALL void
xboxkrnl_NtOpenSymbolicLinkObject() { /* 203 (0x0cb) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtProtectVirtualMemory() { /* 204 (0x0cc) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtPulseEvent() { /* 205 (0x0cd) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtQueueApcThread() { /* 206 (0x0ce) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtQueryDirectoryFile() { /* 207 (0x0cf) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtQueryDirectoryObject() { /* 208 (0x0d0) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtQueryEvent() { /* 209 (0x0d1) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtQueryFullAttributesFile() { /* 210 (0x0d2) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtQueryInformationFile() { /* 211 (0x0d3) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtQueryIoCompletion() { /* 212 (0x0d4) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtQueryMutant() { /* 213 (0x0d5) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtQuerySemaphore() { /* 214 (0x0d6) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtQuerySymbolicLinkObject() { /* 215 (0x0d7) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtQueryTimer() { /* 216 (0x0d8) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL int
xboxkrnl_NtQueryVirtualMemory( /* 217 (0x0d9) */
        /* in */ void *BaseAddress,
        /* out */ xboxkrnl_meminfo *MemoryInformation) {
    int ret = -1;
    ENTER;
    DEBUG(VAR_IN, BaseAddress);
    DEBUG(VAR_OUT, MemoryInformation);

#if 0
typedef struct {
    void *                  BaseAddress;
    void *                  AllocationBase;
    int32_t                 AllocationProtect;
    size_t                  RegionSize;
    int32_t                 State;
    int32_t                 Protect;
    int32_t                 Type;
} xboxkrnl_meminfo;
#define SECTION_QUERY       0x0001
#define SECTION_MAP_WRITE   0x0002
#define SECTION_MAP_READ    0x0004
#define SECTION_MAP_EXECUTE 0x0008
#define SECTION_EXTEND_SIZE 0x0010

#define SECTION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SECTION_QUERY|\
                            SECTION_MAP_WRITE |      \
                            SECTION_MAP_READ |       \
                            SECTION_MAP_EXECUTE |    \
                            SECTION_EXTEND_SIZE)
#define PAGE_NOACCESS          0x01
#define PAGE_READONLY          0x02
#define PAGE_READWRITE         0x04
#define PAGE_WRITECOPY         0x08
#define PAGE_EXECUTE           0x10
#define PAGE_EXECUTE_READ      0x20
#define PAGE_EXECUTE_READWRITE 0x40
#define PAGE_EXECUTE_WRITECOPY 0x80
#define PAGE_GUARD            0x100
#define PAGE_NOCACHE          0x200
#define PAGE_WRITECOMBINE     0x400
#define PAGE_VIDEO              0x0
#define MEM_COMMIT           0x1000
#define MEM_RESERVE          0x2000
#define MEM_DECOMMIT         0x4000
#define MEM_RELEASE          0x8000
#define MEM_FREE            0x10000
#define MEM_PRIVATE         0x20000
#define MEM_RESET           0x80000
#define MEM_TOP_DOWN       0x100000
#define MEM_NOZERO         0x800000
#define MEM_4MB_PAGES    0x80000000  
#endif

#if 0
    MemoryInformation->BaseAddress = BaseAddress;
    MemoryInformation->AllocationBase = BaseAddress;
    MemoryInformation->AllocationProtect
    MemoryInformation->RegionSize
    MemoryInformation->State
    MemoryInformation->Protect
    MemoryInformation->Type
//#if 0
			Buffer->AllocationBase = BaseAddress;
			Buffer->AllocationProtect = PAGE_READONLY;
			Buffer->RegionSize = 64 * 1024;             // size, in bytes, of the region beginning at the base address in which all pages have identical attributes
			Buffer->State = 4096;                       // MEM_DECOMMIT | PAGE_EXECUTE_WRITECOPY etc
			Buffer->Protect = PAGE_READONLY;            // One of the flags listed for the AllocationProtect member is specified
			Buffer->Type = 262144;                      // Specifies the type of pages in the region. (MEM_IMAGE, MEM_MAPPED or MEM_PRIVATE)
#endif
    INT3;//TODO

    LEAVE;
    return ret;
}
STDCALL void
xboxkrnl_NtQueryVolumeInformationFile() { /* 218 (0x0da) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtReadFile() { /* 219 (0x0db) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtReadFileScatter() { /* 220 (0x0dc) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtReleaseMutant() { /* 221 (0x0dd) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtReleaseSemaphore() { /* 222 (0x0de) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtRemoveIoCompletion() { /* 223 (0x0df) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtResumeThread() { /* 224 (0x0e0) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtSetEvent() { /* 225 (0x0e1) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtSetInformationFile() { /* 226 (0x0e2) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtSetIoCompletion() { /* 227 (0x0e3) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtSetSystemTime() { /* 228 (0x0e4) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtSetTimerEx() { /* 229 (0x0e5) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtSignalAndWaitForSingleObjectEx() { /* 230 (0x0e6) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtSuspendThread() { /* 231 (0x0e7) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtUserIoApcDispatcher() { /* 232 (0x0e8) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtWaitForSingleObject() { /* 233 (0x0e9) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtWaitForSingleObjectEx() { /* 234 (0x0ea) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtWaitForMultipleObjectsEx() { /* 235 (0x0eb) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtWriteFile() { /* 236 (0x0ec) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtWriteFileGather() { /* 237 (0x0ed) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_NtYieldExecution() { /* 238 (0x0ee) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ObCreateObject() { /* 239 (0x0ef) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ObDirectoryObjectType() { /* 240 (0x0f0) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ObInsertObject() { /* 241 (0x0f1) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ObMakeTemporaryObject() { /* 242 (0x0f2) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ObOpenObjectByName() { /* 243 (0x0f3) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ObOpenObjectByPointer() { /* 244 (0x0f4) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ObpObjectHandleTable() { /* 245 (0x0f5) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ObReferenceObjectByHandle() { /* 246 (0x0f6) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ObReferenceObjectByName() { /* 247 (0x0f7) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ObReferenceObjectByPointer() { /* 248 (0x0f8) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_ObSymbolicLinkObjectType() { /* 249 (0x0f9) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_ObfDereferenceObject() { /* 250 (0x0fa) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_ObfReferenceObject() { /* 251 (0x0fb) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_PhyGetLinkState() { /* 252 (0x0fc) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_PhyInitialize() { /* 253 (0x0fd) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL int
xboxkrnl_PsCreateSystemThread( /* 254 (0x0fe) */
        /* out */       void *ThreadHandle,
        /* out (opt) */ void **ThreadId,
        /* in */        void *StartRoutine,
        /* in */        void *StartContext,
        /* in */        int DebuggerThread) {
    int ret;
    ENTER;

    INT3;
    ret = xboxkrnl_PsCreateSystemThreadEx(
            ThreadHandle,
            0,
            12288, /* KERNEL_STACK_SIZE */
            0,
            ThreadId,
            StartRoutine,
            StartContext,
            0,
            DebuggerThread,
            NULL);

    LEAVE;
    return ret;
}
STDCALL int
xboxkrnl_PsCreateSystemThreadEx( /* 255 (0x0ff) */
        /* out */       void **ThreadHandle,
        /* in */        size_t ThreadExtensionSize,
        /* in */        size_t KernelStackSize,
        /* in */        size_t TlsDataSize,
        /* out (opt) */ void **ThreadId,
        /* in */        void *StartRoutine,
        /* in */        void *StartContext,
        /* in */        int CreateSuspended,
        /* in */        int DebuggerThread,
        /* in (opt) */  void *SystemRoutine) {
    ENTER;
    DEBUG(VAR_OUT, ThreadHandle);
    DEBUG(VAR_IN,  ThreadExtensionSize);
    DEBUG(VAR_IN,  KernelStackSize);
    DEBUG(VAR_IN,  TlsDataSize);
    DEBUG(VAR_OUT, ThreadId);
    DEBUG(VAR_IN,  StartRoutine);
    DEBUG(VAR_IN,  StartContext);
    DEBUG(VAR_IN,  CreateSuspended);
    DEBUG(VAR_IN,  DebuggerThread);
    DEBUG(VAR_IN,  SystemRoutine);

    debug("    " "/* protected calling thread is system kernel thread */", 0);
    LEAVE;
    ((void (*)(void *))StartRoutine)(StartContext);

    return 0;
}
STDCALL void
xboxkrnl_PsQueryStatistics() { /* 256 (0x100) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_PsSetCreateThreadNotifyRoutine() { /* 257 (0x101) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_PsTerminateSystemThread() { /* 258 (0x102) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_PsThreadObjectType() { /* 259 (0x103) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlAnsiStringToUnicodeString() { /* 260 (0x104) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlAppendStringToString() { /* 261 (0x105) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlAppendUnicodeStringToString() { /* 262 (0x106) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlAppendUnicodeToString() { /* 263 (0x107) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlAssert() { /* 264 (0x108) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlCaptureContext() { /* 265 (0x109) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlCaptureStackBackTrace() { /* 266 (0x10a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlCharToInteger() { /* 267 (0x10b) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlCompareMemory() { /* 268 (0x10c) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlCompareMemoryUlong() { /* 269 (0x10d) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlCompareString() { /* 270 (0x10e) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlCompareUnicodeString() { /* 271 (0x10f) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlCopyString() { /* 272 (0x110) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlCopyUnicodeString() { /* 273 (0x111) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlCreateUnicodeString() { /* 274 (0x112) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlDowncaseUnicodeChar() { /* 275 (0x113) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlDowncaseUnicodeString() { /* 276 (0x114) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlEnterCriticalSection() { /* 277 (0x115) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlEnterCriticalSectionAndRegion() { /* 278 (0x116) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlEqualString() { /* 279 (0x117) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlEqualUnicodeString() { /* 280 (0x118) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlExtendedIntegerMultiply() { /* 281 (0x119) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlExtendedLargeIntegerDivide() { /* 282 (0x11a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlExtendedMagicDivide() { /* 283 (0x11b) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlFillMemory() { /* 284 (0x11c) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlFillMemoryUlong() { /* 285 (0x11d) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlFreeAnsiString() { /* 286 (0x11e) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlFreeUnicodeString() { /* 287 (0x11f) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlGetCallersAddress() { /* 288 (0x120) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlInitAnsiString() { /* 289 (0x121) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlInitUnicodeString() { /* 290 (0x122) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlInitializeCriticalSection( /* 291 (0x123) */
        /* i/o */ pthread_mutex_t **CriticalSection) {
    ENTER;
    DEBUG(VAR_IN, CriticalSection);
    DEBUG(DUMP, *CriticalSection);

    if (!(*CriticalSection = calloc(1, sizeof(**CriticalSection)))) INT3;
    pthread_mutex_init(*CriticalSection, NULL);

    DEBUG(VAR_OUT, *CriticalSection);
    LEAVE;
}
STDCALL void
xboxkrnl_RtlIntegerToChar() { /* 292 (0x124) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlIntegerToUnicodeString() { /* 293 (0x125) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlLeaveCriticalSection() { /* 294 (0x126) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlLeaveCriticalSectionAndRegion() { /* 295 (0x127) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlLowerChar() { /* 296 (0x128) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlMapGenericMask() { /* 297 (0x129) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlMoveMemory() { /* 298 (0x12a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlMultiByteToUnicodeN() { /* 299 (0x12b) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlMultiByteToUnicodeSize() { /* 300 (0x12c) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL int
xboxkrnl_RtlNtStatusToDosError( /* 301 (0x12d) */
        /* in */ int Status) {
    int ret;
    ENTER;
    DEBUG(VAR_IN, Status);

    ret = !!Status;
    DEBUG(DUMP, ret);

    LEAVE;
    return ret;
}
STDCALL void
xboxkrnl_RtlRaiseException() { /* 302 (0x12e) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlRaiseStatus() { /* 303 (0x12f) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlRip() { /* 352 (0x160) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlSnprintf() { /* 361 (0x169) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlSprintf() { /* 362 (0x16a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlTimeFieldsToTime() { /* 304 (0x130) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlTimeToTimeFields() { /* 305 (0x131) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlTryEnterCriticalSection() { /* 306 (0x132) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_RtlUlongByteSwap() { /* 307 (0x133) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlUnicodeStringToAnsiString() { /* 308 (0x134) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlUnicodeStringToInteger() { /* 309 (0x135) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlUnicodeToMultiByteN() { /* 310 (0x136) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlUnicodeToMultiByteSize() { /* 311 (0x137) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlUnwind() { /* 312 (0x138) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlUpcaseUnicodeChar() { /* 313 (0x139) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlUpcaseUnicodeString() { /* 314 (0x13a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlUpcaseUnicodeToMultiByteN() { /* 315 (0x13b) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlUpperChar() { /* 316 (0x13c) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlUpperString() { /* 317 (0x13d) */
    ENTER;

    INT3;
    

    LEAVE;
}
FASTCALL void
xboxkrnl_RtlUshortByteSwap() { /* 318 (0x13e) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlVsnprintf() { /* 363 (0x16b) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlVsprintf() { /* 364 (0x16c) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlWalkFrameChain() { /* 319 (0x13f) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_RtlZeroMemory() { /* 320 (0x140) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XboxAlternateSignatureKeys() { /* 354 (0x162) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XboxEEPROMKey() { /* 321 (0x141) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XboxHardwareInfo() { /* 322 (0x142) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XboxHDKey() { /* 323 (0x143) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XboxLANKey() { /* 353 (0x161) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XboxKrnlVersion() { /* 324 (0x144) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XboxSignatureKey() { /* 325 (0x145) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XeImageFileName() { /* 326 (0x146) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XeLoadSection() { /* 327 (0x147) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XePublicKeyData() { /* 355 (0x163) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XeUnloadSection() { /* 328 (0x148) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_READ_PORT_BUFFER_UCHAR() { /* 329 (0x149) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_READ_PORT_BUFFER_USHORT() { /* 330 (0x14a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_READ_PORT_BUFFER_ULONG() { /* 331 (0x14b) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_WRITE_PORT_BUFFER_UCHAR() { /* 332 (0x14c) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_WRITE_PORT_BUFFER_USHORT() { /* 333 (0x14d) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_WRITE_PORT_BUFFER_ULONG() { /* 334 (0x14e) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcSHAInit() { /* 335 (0x14f) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcSHAUpdate() { /* 336 (0x150) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcSHAFinal() { /* 337 (0x151) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcRC4Key() { /* 338 (0x152) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcRC4Crypt() { /* 339 (0x153) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcHMAC() { /* 340 (0x154) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcPKEncPublic() { /* 341 (0x155) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcPKDecPrivate() { /* 342 (0x156) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcPKGetKeyLen() { /* 343 (0x157) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcVerifyPKCS1Signature() { /* 344 (0x158) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcModExp() { /* 345 (0x159) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcDESKeyParity() { /* 346 (0x15a) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcKeyTable() { /* 347 (0x15b) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcBlockCrypt() { /* 348 (0x15c) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcBlockCryptCBC() { /* 349 (0x15d) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcCryptService() { /* 350 (0x15e) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_XcUpdateCrypto() { /* 351 (0x15f) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_DbgLoadImageSymbols() { /* 007 (0x007) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_DbgUnLoadImageSymbols() { /* 011 (0x00b) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmDbgAllocateMemory() { /* 374 (0x176) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmDbgFreeMemory() { /* 375 (0x177) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmDbgQueryAvailablePages() { /* 376 (0x178) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmDbgReleaseAddress() { /* 377 (0x179) */
    ENTER;

    INT3;
    

    LEAVE;
}
STDCALL void
xboxkrnl_MmDbgWriteCheck() { /* 378 (0x17a) */
    ENTER;

    INT3;
    

    LEAVE;
}

static const void *xboxkrnl_thunk[] = {
    /* 001 */ [0x001] = &xboxkrnl_AvGetSavedDataAddress,
    /* 002 */ [0x002] = &xboxkrnl_AvSendTVEncoderOption,
    /* 003 */ [0x003] = &xboxkrnl_AvSetDisplayMode,
    /* 004 */ [0x004] = &xboxkrnl_AvSetSavedDataAddress,
    /* 005 */ [0x005] = &xboxkrnl_DbgBreakPoint,
    /* 006 */ [0x006] = &xboxkrnl_DbgBreakPointWithStatus,
    /* 008 */ [0x008] = &xboxkrnl_DbgPrint,
    /* 010 */ [0x00a] = &xboxkrnl_DbgPrompt,
    /* 012 */ [0x00c] = &xboxkrnl_ExAcquireReadWriteLockExclusive,
    /* 013 */ [0x00d] = &xboxkrnl_ExAcquireReadWriteLockShared,
    /* 014 */ [0x00e] = &xboxkrnl_ExAllocatePool,
    /* 015 */ [0x00f] = &xboxkrnl_ExAllocatePoolWithTag,
    /* 016 */ [0x010] = &xboxkrnl_ExEventObjectType,
    /* 017 */ [0x011] = &xboxkrnl_ExFreePool,
    /* 018 */ [0x012] = &xboxkrnl_ExInitializeReadWriteLock,
    /* 019 */ [0x013] = &xboxkrnl_ExInterlockedAddLargeInteger,
    /* 020 */ [0x014] = &xboxkrnl_ExInterlockedAddLargeStatistic, /* fastcall */
    /* 021 */ [0x015] = &xboxkrnl_ExInterlockedCompareExchange64, /* fastcall */
    /* 022 */ [0x016] = &xboxkrnl_ExMutantObjectType,
    /* 023 */ [0x017] = &xboxkrnl_ExQueryPoolBlockSize,
    /* 024 */ [0x018] = &xboxkrnl_ExQueryNonVolatileSetting,
    /* 025 */ [0x019] = &xboxkrnl_ExReadWriteRefurbInfo,
    /* 026 */ [0x01a] = &xboxkrnl_ExRaiseException,
    /* 027 */ [0x01b] = &xboxkrnl_ExRaiseStatus,
    /* 028 */ [0x01c] = &xboxkrnl_ExReleaseReadWriteLock,
    /* 029 */ [0x01d] = &xboxkrnl_ExSaveNonVolatileSetting,
    /* 030 */ [0x01e] = &xboxkrnl_ExSemaphoreObjectType,
    /* 031 */ [0x01f] = &xboxkrnl_ExTimerObjectType,
    /* 032 */ [0x020] = &xboxkrnl_ExfInterlockedInsertHeadList,
    /* 033 */ [0x021] = &xboxkrnl_ExfInterlockedInsertTailList, /* fastcall */
    /* 034 */ [0x022] = &xboxkrnl_ExfInterlockedRemoveHeadList, /* fastcall */
    /* 035 */ [0x023] = &xboxkrnl_FscGetCacheSize,
    /* 036 */ [0x024] = &xboxkrnl_FscInvalidateIdleBlocks,
    /* 037 */ [0x025] = &xboxkrnl_FscSetCacheSize,
    /* 356 */ [0x164] = &xboxkrnl_HalBootSMCVideoMode,
    /* 038 */ [0x026] = &xboxkrnl_HalClearSoftwareInterrupt, /* fastcall */
    /* 039 */ [0x027] = &xboxkrnl_HalDisableSystemInterrupt,
    /* 040 */ [0x028] = &xboxkrnl_HalDiskCachePartitionCount,
    /* 041 */ [0x029] = &xboxkrnl_HalDiskModelNumber,
    /* 042 */ [0x02a] = &xboxkrnl_HalDiskSerialNumber,
    /* 365 */ [0x16d] = &xboxkrnl_HalEnableSecureTrayEject,
    /* 043 */ [0x02b] = &xboxkrnl_HalEnableSystemInterrupt,
    /* 044 */ [0x02c] = &xboxkrnl_HalGetInterruptVector,
    /* 360 */ [0x168] = &xboxkrnl_HalInitiateShutdown,
    /* 358 */ [0x166] = &xboxkrnl_HalIsResetOrShutdownPending,
    /* 045 */ [0x02d] = &xboxkrnl_HalReadSMBusValue,
    /* 009 */ [0x009] = &xboxkrnl_HalReadSMCTrayState,
    /* 046 */ [0x02e] = &xboxkrnl_HalReadWritePCISpace,
    /* 047 */ [0x02f] = &xboxkrnl_HalRegisterShutdownNotification,
    /* 048 */ [0x030] = &xboxkrnl_HalRequestSoftwareInterrupt, /* fastcall */
    /* 049 */ [0x031] = &xboxkrnl_HalReturnToFirmware,
    /* 050 */ [0x032] = &xboxkrnl_HalWriteSMBusValue,
    /* 366 */ [0x16e] = &xboxkrnl_HalWriteSMCScratchRegister,
    /* 357 */ [0x165] = &xboxkrnl_IdexChannelObject,
    /* 051 */ [0x033] = &xboxkrnl_InterlockedCompareExchange, /* fastcall */
    /* 052 */ [0x034] = &xboxkrnl_InterlockedDecrement, /* fastcall */
    /* 053 */ [0x035] = &xboxkrnl_InterlockedIncrement, /* fastcall */
    /* 054 */ [0x036] = &xboxkrnl_InterlockedExchange, /* fastcall */
    /* 055 */ [0x037] = &xboxkrnl_InterlockedExchangeAdd, /* fastcall */
    /* 056 */ [0x038] = &xboxkrnl_InterlockedFlushSList, /* fastcall */
    /* 057 */ [0x039] = &xboxkrnl_InterlockedPopEntrySList, /* fastcall */
    /* 058 */ [0x03a] = &xboxkrnl_InterlockedPushEntrySList, /* fastcall */
    /* 059 */ [0x03b] = &xboxkrnl_IoAllocateIrp,
    /* 060 */ [0x03c] = &xboxkrnl_IoBuildAsynchronousFsdRequest,
    /* 061 */ [0x03d] = &xboxkrnl_IoBuildDeviceIoControlRequest,
    /* 062 */ [0x03e] = &xboxkrnl_IoBuildSynchronousFsdRequest,
    /* 063 */ [0x03f] = &xboxkrnl_IoCheckShareAccess,
    /* 064 */ [0x040] = &xboxkrnl_IoCompletionObjectType,
    /* 065 */ [0x041] = &xboxkrnl_IoCreateDevice,
    /* 066 */ [0x042] = &xboxkrnl_IoCreateFile,
    /* 067 */ [0x043] = &xboxkrnl_IoCreateSymbolicLink,
    /* 068 */ [0x044] = &xboxkrnl_IoDeleteDevice,
    /* 069 */ [0x045] = &xboxkrnl_IoDeleteSymbolicLink,
    /* 070 */ [0x046] = &xboxkrnl_IoDeviceObjectType,
    /* 090 */ [0x05a] = &xboxkrnl_IoDismountVolume,
    /* 091 */ [0x05b] = &xboxkrnl_IoDismountVolumeByName,
    /* 071 */ [0x047] = &xboxkrnl_IoFileObjectType,
    /* 072 */ [0x048] = &xboxkrnl_IoFreeIrp,
    /* 073 */ [0x049] = &xboxkrnl_IoInitializeIrp,
    /* 074 */ [0x04a] = &xboxkrnl_IoInvalidDeviceRequest,
    /* 359 */ [0x167] = &xboxkrnl_IoMarkIrpMustComplete,
    /* 075 */ [0x04b] = &xboxkrnl_IoQueryFileInformation,
    /* 076 */ [0x04c] = &xboxkrnl_IoQueryVolumeInformation,
    /* 077 */ [0x04d] = &xboxkrnl_IoQueueThreadIrp,
    /* 078 */ [0x04e] = &xboxkrnl_IoRemoveShareAccess,
    /* 079 */ [0x04f] = &xboxkrnl_IoSetIoCompletion,
    /* 080 */ [0x050] = &xboxkrnl_IoSetShareAccess,
    /* 081 */ [0x051] = &xboxkrnl_IoStartNextPacket,
    /* 082 */ [0x052] = &xboxkrnl_IoStartNextPacketByKey,
    /* 083 */ [0x053] = &xboxkrnl_IoStartPacket,
    /* 084 */ [0x054] = &xboxkrnl_IoSynchronousDeviceIoControlRequest,
    /* 085 */ [0x055] = &xboxkrnl_IoSynchronousFsdRequest,
    /* 086 */ [0x056] = &xboxkrnl_IofCallDriver, /* fastcall */
    /* 087 */ [0x057] = &xboxkrnl_IofCompleteRequest, /* fastcall */
    /* 088 */ [0x058] = &xboxkrnl_KdDebuggerEnabled,
    /* 089 */ [0x059] = &xboxkrnl_KdDebuggerNotPresent,
    /* 092 */ [0x05c] = &xboxkrnl_KeAlertResumeThread,
    /* 093 */ [0x05d] = &xboxkrnl_KeAlertThread,
    /* 094 */ [0x05e] = &xboxkrnl_KeBoostPriorityThread,
    /* 095 */ [0x05f] = &xboxkrnl_KeBugCheck,
    /* 096 */ [0x060] = &xboxkrnl_KeBugCheckEx,
    /* 097 */ [0x061] = &xboxkrnl_KeCancelTimer,
    /* 098 */ [0x062] = &xboxkrnl_KeConnectInterrupt,
    /* 099 */ [0x063] = &xboxkrnl_KeDelayExecutionThread,
    /* 100 */ [0x064] = &xboxkrnl_KeDisconnectInterrupt,
    /* 101 */ [0x065] = &xboxkrnl_KeEnterCriticalRegion,
    /* 103 */ [0x067] = &xboxkrnl_KeGetCurrentIrql,
    /* 104 */ [0x068] = &xboxkrnl_KeGetCurrentThread,
    /* 105 */ [0x069] = &xboxkrnl_KeInitializeApc,
    /* 106 */ [0x06a] = &xboxkrnl_KeInitializeDeviceQueue,
    /* 107 */ [0x06b] = &xboxkrnl_KeInitializeDpc,
    /* 108 */ [0x06c] = &xboxkrnl_KeInitializeEvent,
    /* 109 */ [0x06d] = &xboxkrnl_KeInitializeInterrupt,
    /* 110 */ [0x06e] = &xboxkrnl_KeInitializeMutant,
    /* 111 */ [0x06f] = &xboxkrnl_KeInitializeQueue,
    /* 112 */ [0x070] = &xboxkrnl_KeInitializeSemaphore,
    /* 113 */ [0x071] = &xboxkrnl_KeInitializeTimerEx,
    /* 114 */ [0x072] = &xboxkrnl_KeInsertByKeyDeviceQueue,
    /* 115 */ [0x073] = &xboxkrnl_KeInsertDeviceQueue,
    /* 116 */ [0x074] = &xboxkrnl_KeInsertHeadQueue,
    /* 117 */ [0x075] = &xboxkrnl_KeInsertQueue,
    /* 118 */ [0x076] = &xboxkrnl_KeInsertQueueApc,
    /* 119 */ [0x077] = &xboxkrnl_KeInsertQueueDpc,
    /* 120 */ [0x078] = &xboxkrnl_KeInterruptTime,
    /* 121 */ [0x079] = &xboxkrnl_KeIsExecutingDpc,
    /* 122 */ [0x07a] = &xboxkrnl_KeLeaveCriticalRegion,
    /* 123 */ [0x07b] = &xboxkrnl_KePulseEvent,
    /* 124 */ [0x07c] = &xboxkrnl_KeQueryBasePriorityThread,
    /* 125 */ [0x07d] = &xboxkrnl_KeQueryInterruptTime,
    /* 126 */ [0x07e] = &xboxkrnl_KeQueryPerformanceCounter,
    /* 127 */ [0x07f] = &xboxkrnl_KeQueryPerformanceFrequency,
    /* 128 */ [0x080] = &xboxkrnl_KeQuerySystemTime,
    /* 129 */ [0x081] = &xboxkrnl_KeRaiseIrqlToDpcLevel,
    /* 130 */ [0x082] = &xboxkrnl_KeRaiseIrqlToSynchLevel,
    /* 131 */ [0x083] = &xboxkrnl_KeReleaseMutant,
    /* 132 */ [0x084] = &xboxkrnl_KeReleaseSemaphore,
    /* 133 */ [0x085] = &xboxkrnl_KeRemoveByKeyDeviceQueue,
    /* 134 */ [0x086] = &xboxkrnl_KeRemoveDeviceQueue,
    /* 135 */ [0x087] = &xboxkrnl_KeRemoveEntryDeviceQueue,
    /* 136 */ [0x088] = &xboxkrnl_KeRemoveQueue,
    /* 137 */ [0x089] = &xboxkrnl_KeRemoveQueueDpc,
    /* 138 */ [0x08a] = &xboxkrnl_KeResetEvent,
    /* 139 */ [0x08b] = &xboxkrnl_KeRestoreFloatingPointState,
    /* 140 */ [0x08c] = &xboxkrnl_KeResumeThread,
    /* 141 */ [0x08d] = &xboxkrnl_KeRundownQueue,
    /* 142 */ [0x08e] = &xboxkrnl_KeSaveFloatingPointState,
    /* 143 */ [0x08f] = &xboxkrnl_KeSetBasePriorityThread,
    /* 144 */ [0x090] = &xboxkrnl_KeSetDisableBoostThread,
    /* 145 */ [0x091] = &xboxkrnl_KeSetEvent,
    /* 146 */ [0x092] = &xboxkrnl_KeSetEventBoostPriority,
    /* 147 */ [0x093] = &xboxkrnl_KeSetPriorityProcess,
    /* 148 */ [0x094] = &xboxkrnl_KeSetPriorityThread,
    /* 149 */ [0x095] = &xboxkrnl_KeSetTimer,
    /* 150 */ [0x096] = &xboxkrnl_KeSetTimerEx,
    /* 151 */ [0x097] = &xboxkrnl_KeStallExecutionProcessor,
    /* 152 */ [0x098] = &xboxkrnl_KeSuspendThread,
    /* 153 */ [0x099] = &xboxkrnl_KeSynchronizeExecution,
    /* 154 */ [0x09a] = &xboxkrnl_KeSystemTime,
    /* 155 */ [0x09b] = &xboxkrnl_KeTestAlertThread,
    /* 156 */ [0x09c] = &xboxkrnl_KeTickCount,
    /* 157 */ [0x09d] = &xboxkrnl_KeTimeIncrement,
    /* 158 */ [0x09e] = &xboxkrnl_KeWaitForMultipleObjects,
    /* 159 */ [0x09f] = &xboxkrnl_KeWaitForSingleObject,
    /* 160 */ [0x0a0] = &xboxkrnl_KfRaiseIrql, /* fastcall */
    /* 161 */ [0x0a1] = &xboxkrnl_KfLowerIrql, /* fastcall */
    /* 162 */ [0x0a2] = &xboxkrnl_KiBugCheckData,
    /* 163 */ [0x0a3] = &xboxkrnl_KiUnlockDispatcherDatabase, /* fastcall */
    /* 164 */ [0x0a4] = &xboxkrnl_LaunchDataPage,
    /* 165 */ [0x0a5] = &xboxkrnl_MmAllocateContiguousMemory,
    /* 166 */ [0x0a6] = &xboxkrnl_MmAllocateContiguousMemoryEx,
    /* 167 */ [0x0a7] = &xboxkrnl_MmAllocateSystemMemory,
    /* 168 */ [0x0a8] = &xboxkrnl_MmClaimGpuInstanceMemory,
    /* 169 */ [0x0a9] = &xboxkrnl_MmCreateKernelStack,
    /* 170 */ [0x0aa] = &xboxkrnl_MmDeleteKernelStack,
    /* 171 */ [0x0ab] = &xboxkrnl_MmFreeContiguousMemory,
    /* 172 */ [0x0ac] = &xboxkrnl_MmFreeSystemMemory,
    /* 173 */ [0x0ad] = &xboxkrnl_MmGetPhysicalAddress,
    /* 102 */ [0x066] = &xboxkrnl_MmGlobalData,
    /* 174 */ [0x0ae] = &xboxkrnl_MmIsAddressValid,
    /* 175 */ [0x0af] = &xboxkrnl_MmLockUnlockBufferPages,
    /* 176 */ [0x0b0] = &xboxkrnl_MmLockUnlockPhysicalPage,
    /* 177 */ [0x0b1] = &xboxkrnl_MmMapIoSpace,
    /* 178 */ [0x0b2] = &xboxkrnl_MmPersistContiguousMemory,
    /* 179 */ [0x0b3] = &xboxkrnl_MmQueryAddressProtect,
    /* 180 */ [0x0b4] = &xboxkrnl_MmQueryAllocationSize,
    /* 181 */ [0x0b5] = &xboxkrnl_MmQueryStatistics,
    /* 182 */ [0x0b6] = &xboxkrnl_MmSetAddressProtect,
    /* 183 */ [0x0b7] = &xboxkrnl_MmUnmapIoSpace,
    /* 184 */ [0x0b8] = &xboxkrnl_NtAllocateVirtualMemory,
    /* 185 */ [0x0b9] = &xboxkrnl_NtCancelTimer,
    /* 186 */ [0x0ba] = &xboxkrnl_NtClearEvent,
    /* 187 */ [0x0bb] = &xboxkrnl_NtClose,
    /* 188 */ [0x0bc] = &xboxkrnl_NtCreateDirectoryObject,
    /* 189 */ [0x0bd] = &xboxkrnl_NtCreateEvent,
    /* 190 */ [0x0be] = &xboxkrnl_NtCreateFile,
    /* 191 */ [0x0bf] = &xboxkrnl_NtCreateIoCompletion,
    /* 192 */ [0x0c0] = &xboxkrnl_NtCreateMutant,
    /* 193 */ [0x0c1] = &xboxkrnl_NtCreateSemaphore,
    /* 194 */ [0x0c2] = &xboxkrnl_NtCreateTimer,
    /* 195 */ [0x0c3] = &xboxkrnl_NtDeleteFile,
    /* 196 */ [0x0c4] = &xboxkrnl_NtDeviceIoControlFile,
    /* 197 */ [0x0c5] = &xboxkrnl_NtDuplicateObject,
    /* 198 */ [0x0c6] = &xboxkrnl_NtFlushBuffersFile,
    /* 199 */ [0x0c7] = &xboxkrnl_NtFreeVirtualMemory,
    /* 200 */ [0x0c8] = &xboxkrnl_NtFsControlFile,
    /* 201 */ [0x0c9] = &xboxkrnl_NtOpenDirectoryObject,
    /* 202 */ [0x0ca] = &xboxkrnl_NtOpenFile,
    /* 203 */ [0x0cb] = &xboxkrnl_NtOpenSymbolicLinkObject,
    /* 204 */ [0x0cc] = &xboxkrnl_NtProtectVirtualMemory,
    /* 205 */ [0x0cd] = &xboxkrnl_NtPulseEvent,
    /* 206 */ [0x0ce] = &xboxkrnl_NtQueueApcThread,
    /* 207 */ [0x0cf] = &xboxkrnl_NtQueryDirectoryFile,
    /* 208 */ [0x0d0] = &xboxkrnl_NtQueryDirectoryObject,
    /* 209 */ [0x0d1] = &xboxkrnl_NtQueryEvent,
    /* 210 */ [0x0d2] = &xboxkrnl_NtQueryFullAttributesFile,
    /* 211 */ [0x0d3] = &xboxkrnl_NtQueryInformationFile,
    /* 212 */ [0x0d4] = &xboxkrnl_NtQueryIoCompletion,
    /* 213 */ [0x0d5] = &xboxkrnl_NtQueryMutant,
    /* 214 */ [0x0d6] = &xboxkrnl_NtQuerySemaphore,
    /* 215 */ [0x0d7] = &xboxkrnl_NtQuerySymbolicLinkObject,
    /* 216 */ [0x0d8] = &xboxkrnl_NtQueryTimer,
    /* 217 */ [0x0d9] = &xboxkrnl_NtQueryVirtualMemory,
    /* 218 */ [0x0da] = &xboxkrnl_NtQueryVolumeInformationFile,
    /* 219 */ [0x0db] = &xboxkrnl_NtReadFile,
    /* 220 */ [0x0dc] = &xboxkrnl_NtReadFileScatter,
    /* 221 */ [0x0dd] = &xboxkrnl_NtReleaseMutant,
    /* 222 */ [0x0de] = &xboxkrnl_NtReleaseSemaphore,
    /* 223 */ [0x0df] = &xboxkrnl_NtRemoveIoCompletion,
    /* 224 */ [0x0e0] = &xboxkrnl_NtResumeThread,
    /* 225 */ [0x0e1] = &xboxkrnl_NtSetEvent,
    /* 226 */ [0x0e2] = &xboxkrnl_NtSetInformationFile,
    /* 227 */ [0x0e3] = &xboxkrnl_NtSetIoCompletion,
    /* 228 */ [0x0e4] = &xboxkrnl_NtSetSystemTime,
    /* 229 */ [0x0e5] = &xboxkrnl_NtSetTimerEx,
    /* 230 */ [0x0e6] = &xboxkrnl_NtSignalAndWaitForSingleObjectEx,
    /* 231 */ [0x0e7] = &xboxkrnl_NtSuspendThread,
    /* 232 */ [0x0e8] = &xboxkrnl_NtUserIoApcDispatcher,
    /* 233 */ [0x0e9] = &xboxkrnl_NtWaitForSingleObject,
    /* 234 */ [0x0ea] = &xboxkrnl_NtWaitForSingleObjectEx,
    /* 235 */ [0x0eb] = &xboxkrnl_NtWaitForMultipleObjectsEx,
    /* 236 */ [0x0ec] = &xboxkrnl_NtWriteFile,
    /* 237 */ [0x0ed] = &xboxkrnl_NtWriteFileGather,
    /* 238 */ [0x0ee] = &xboxkrnl_NtYieldExecution,
    /* 239 */ [0x0ef] = &xboxkrnl_ObCreateObject,
    /* 240 */ [0x0f0] = &xboxkrnl_ObDirectoryObjectType,
    /* 241 */ [0x0f1] = &xboxkrnl_ObInsertObject,
    /* 242 */ [0x0f2] = &xboxkrnl_ObMakeTemporaryObject,
    /* 243 */ [0x0f3] = &xboxkrnl_ObOpenObjectByName,
    /* 244 */ [0x0f4] = &xboxkrnl_ObOpenObjectByPointer,
    /* 245 */ [0x0f5] = &xboxkrnl_ObpObjectHandleTable,
    /* 246 */ [0x0f6] = &xboxkrnl_ObReferenceObjectByHandle,
    /* 247 */ [0x0f7] = &xboxkrnl_ObReferenceObjectByName,
    /* 248 */ [0x0f8] = &xboxkrnl_ObReferenceObjectByPointer,
    /* 249 */ [0x0f9] = &xboxkrnl_ObSymbolicLinkObjectType,
    /* 250 */ [0x0fa] = &xboxkrnl_ObfDereferenceObject, /* fastcall */
    /* 251 */ [0x0fb] = &xboxkrnl_ObfReferenceObject, /* fastcall */
    /* 252 */ [0x0fc] = &xboxkrnl_PhyGetLinkState,
    /* 253 */ [0x0fd] = &xboxkrnl_PhyInitialize,
    /* 254 */ [0x0fe] = &xboxkrnl_PsCreateSystemThread,
    /* 255 */ [0x0ff] = &xboxkrnl_PsCreateSystemThreadEx,
    /* 256 */ [0x100] = &xboxkrnl_PsQueryStatistics,
    /* 257 */ [0x101] = &xboxkrnl_PsSetCreateThreadNotifyRoutine,
    /* 258 */ [0x102] = &xboxkrnl_PsTerminateSystemThread,
    /* 259 */ [0x103] = &xboxkrnl_PsThreadObjectType,
    /* 260 */ [0x104] = &xboxkrnl_RtlAnsiStringToUnicodeString,
    /* 261 */ [0x105] = &xboxkrnl_RtlAppendStringToString,
    /* 262 */ [0x106] = &xboxkrnl_RtlAppendUnicodeStringToString,
    /* 263 */ [0x107] = &xboxkrnl_RtlAppendUnicodeToString,
    /* 264 */ [0x108] = &xboxkrnl_RtlAssert,
    /* 265 */ [0x109] = &xboxkrnl_RtlCaptureContext,
    /* 266 */ [0x10a] = &xboxkrnl_RtlCaptureStackBackTrace,
    /* 267 */ [0x10b] = &xboxkrnl_RtlCharToInteger,
    /* 268 */ [0x10c] = &xboxkrnl_RtlCompareMemory,
    /* 269 */ [0x10d] = &xboxkrnl_RtlCompareMemoryUlong,
    /* 270 */ [0x10e] = &xboxkrnl_RtlCompareString,
    /* 271 */ [0x10f] = &xboxkrnl_RtlCompareUnicodeString,
    /* 272 */ [0x110] = &xboxkrnl_RtlCopyString,
    /* 273 */ [0x111] = &xboxkrnl_RtlCopyUnicodeString,
    /* 274 */ [0x112] = &xboxkrnl_RtlCreateUnicodeString,
    /* 275 */ [0x113] = &xboxkrnl_RtlDowncaseUnicodeChar,
    /* 276 */ [0x114] = &xboxkrnl_RtlDowncaseUnicodeString,
    /* 277 */ [0x115] = &xboxkrnl_RtlEnterCriticalSection,
    /* 278 */ [0x116] = &xboxkrnl_RtlEnterCriticalSectionAndRegion,
    /* 279 */ [0x117] = &xboxkrnl_RtlEqualString,
    /* 280 */ [0x118] = &xboxkrnl_RtlEqualUnicodeString,
    /* 281 */ [0x119] = &xboxkrnl_RtlExtendedIntegerMultiply,
    /* 282 */ [0x11a] = &xboxkrnl_RtlExtendedLargeIntegerDivide,
    /* 283 */ [0x11b] = &xboxkrnl_RtlExtendedMagicDivide,
    /* 284 */ [0x11c] = &xboxkrnl_RtlFillMemory,
    /* 285 */ [0x11d] = &xboxkrnl_RtlFillMemoryUlong,
    /* 286 */ [0x11e] = &xboxkrnl_RtlFreeAnsiString,
    /* 287 */ [0x11f] = &xboxkrnl_RtlFreeUnicodeString,
    /* 288 */ [0x120] = &xboxkrnl_RtlGetCallersAddress,
    /* 289 */ [0x121] = &xboxkrnl_RtlInitAnsiString,
    /* 290 */ [0x122] = &xboxkrnl_RtlInitUnicodeString,
    /* 291 */ [0x123] = &xboxkrnl_RtlInitializeCriticalSection,
    /* 292 */ [0x124] = &xboxkrnl_RtlIntegerToChar,
    /* 293 */ [0x125] = &xboxkrnl_RtlIntegerToUnicodeString,
    /* 294 */ [0x126] = &xboxkrnl_RtlLeaveCriticalSection,
    /* 295 */ [0x127] = &xboxkrnl_RtlLeaveCriticalSectionAndRegion,
    /* 296 */ [0x128] = &xboxkrnl_RtlLowerChar,
    /* 297 */ [0x129] = &xboxkrnl_RtlMapGenericMask,
    /* 298 */ [0x12a] = &xboxkrnl_RtlMoveMemory,
    /* 299 */ [0x12b] = &xboxkrnl_RtlMultiByteToUnicodeN,
    /* 300 */ [0x12c] = &xboxkrnl_RtlMultiByteToUnicodeSize,
    /* 301 */ [0x12d] = &xboxkrnl_RtlNtStatusToDosError,
    /* 302 */ [0x12e] = &xboxkrnl_RtlRaiseException,
    /* 303 */ [0x12f] = &xboxkrnl_RtlRaiseStatus,
    /* 352 */ [0x160] = &xboxkrnl_RtlRip,
    /* 361 */ [0x169] = &xboxkrnl_RtlSnprintf,
    /* 362 */ [0x16a] = &xboxkrnl_RtlSprintf,
    /* 304 */ [0x130] = &xboxkrnl_RtlTimeFieldsToTime,
    /* 305 */ [0x131] = &xboxkrnl_RtlTimeToTimeFields,
    /* 306 */ [0x132] = &xboxkrnl_RtlTryEnterCriticalSection,
    /* 307 */ [0x133] = &xboxkrnl_RtlUlongByteSwap, /* fastcall */
    /* 308 */ [0x134] = &xboxkrnl_RtlUnicodeStringToAnsiString,
    /* 309 */ [0x135] = &xboxkrnl_RtlUnicodeStringToInteger,
    /* 310 */ [0x136] = &xboxkrnl_RtlUnicodeToMultiByteN,
    /* 311 */ [0x137] = &xboxkrnl_RtlUnicodeToMultiByteSize,
    /* 312 */ [0x138] = &xboxkrnl_RtlUnwind,
    /* 313 */ [0x139] = &xboxkrnl_RtlUpcaseUnicodeChar,
    /* 314 */ [0x13a] = &xboxkrnl_RtlUpcaseUnicodeString,
    /* 315 */ [0x13b] = &xboxkrnl_RtlUpcaseUnicodeToMultiByteN,
    /* 316 */ [0x13c] = &xboxkrnl_RtlUpperChar,
    /* 317 */ [0x13d] = &xboxkrnl_RtlUpperString,
    /* 318 */ [0x13e] = &xboxkrnl_RtlUshortByteSwap, /* fastcall */
    /* 363 */ [0x16b] = &xboxkrnl_RtlVsnprintf,
    /* 364 */ [0x16c] = &xboxkrnl_RtlVsprintf,
    /* 319 */ [0x13f] = &xboxkrnl_RtlWalkFrameChain,
    /* 320 */ [0x140] = &xboxkrnl_RtlZeroMemory,
    /* 354 */ [0x162] = &xboxkrnl_XboxAlternateSignatureKeys,
    /* 321 */ [0x141] = &xboxkrnl_XboxEEPROMKey,
    /* 322 */ [0x142] = &xboxkrnl_XboxHardwareInfo,
    /* 323 */ [0x143] = &xboxkrnl_XboxHDKey,
    /* 353 */ [0x161] = &xboxkrnl_XboxLANKey,
    /* 324 */ [0x144] = &xboxkrnl_XboxKrnlVersion,
    /* 325 */ [0x145] = &xboxkrnl_XboxSignatureKey,
    /* 326 */ [0x146] = &xboxkrnl_XeImageFileName,
    /* 327 */ [0x147] = &xboxkrnl_XeLoadSection,
    /* 355 */ [0x163] = &xboxkrnl_XePublicKeyData,
    /* 328 */ [0x148] = &xboxkrnl_XeUnloadSection,
    /* 329 */ [0x149] = &xboxkrnl_READ_PORT_BUFFER_UCHAR,
    /* 330 */ [0x14a] = &xboxkrnl_READ_PORT_BUFFER_USHORT,
    /* 331 */ [0x14b] = &xboxkrnl_READ_PORT_BUFFER_ULONG,
    /* 332 */ [0x14c] = &xboxkrnl_WRITE_PORT_BUFFER_UCHAR,
    /* 333 */ [0x14d] = &xboxkrnl_WRITE_PORT_BUFFER_USHORT,
    /* 334 */ [0x14e] = &xboxkrnl_WRITE_PORT_BUFFER_ULONG,
    /* 335 */ [0x14f] = &xboxkrnl_XcSHAInit,
    /* 336 */ [0x150] = &xboxkrnl_XcSHAUpdate,
    /* 337 */ [0x151] = &xboxkrnl_XcSHAFinal,
    /* 338 */ [0x152] = &xboxkrnl_XcRC4Key,
    /* 339 */ [0x153] = &xboxkrnl_XcRC4Crypt,
    /* 340 */ [0x154] = &xboxkrnl_XcHMAC,
    /* 341 */ [0x155] = &xboxkrnl_XcPKEncPublic,
    /* 342 */ [0x156] = &xboxkrnl_XcPKDecPrivate,
    /* 343 */ [0x157] = &xboxkrnl_XcPKGetKeyLen,
    /* 344 */ [0x158] = &xboxkrnl_XcVerifyPKCS1Signature,
    /* 345 */ [0x159] = &xboxkrnl_XcModExp,
    /* 346 */ [0x15a] = &xboxkrnl_XcDESKeyParity,
    /* 347 */ [0x15b] = &xboxkrnl_XcKeyTable,
    /* 348 */ [0x15c] = &xboxkrnl_XcBlockCrypt,
    /* 349 */ [0x15d] = &xboxkrnl_XcBlockCryptCBC,
    /* 350 */ [0x15e] = &xboxkrnl_XcCryptService,
    /* 351 */ [0x15f] = &xboxkrnl_XcUpdateCrypto,
    /* devkit */
    /* 007 */ [0x007] = &xboxkrnl_DbgLoadImageSymbols,
    /* 011 */ [0x00b] = &xboxkrnl_DbgUnLoadImageSymbols,
    /* 374 */ [0x176] = &xboxkrnl_MmDbgAllocateMemory,
    /* 375 */ [0x177] = &xboxkrnl_MmDbgFreeMemory,
    /* 376 */ [0x178] = &xboxkrnl_MmDbgQueryAvailablePages,
    /* 377 */ [0x179] = &xboxkrnl_MmDbgReleaseAddress,
    /* 378 */ [0x17a] = &xboxkrnl_MmDbgWriteCheck,
};

static const char *const xboxkrnl_thunk_name[] = {
    /* 001 */ [0x001] = "AvGetSavedDataAddress",
    /* 002 */ [0x002] = "AvSendTVEncoderOption",
    /* 003 */ [0x003] = "AvSetDisplayMode",
    /* 004 */ [0x004] = "AvSetSavedDataAddress",
    /* 005 */ [0x005] = "DbgBreakPoint",
    /* 006 */ [0x006] = "DbgBreakPointWithStatus",
    /* 008 */ [0x008] = "DbgPrint",
    /* 010 */ [0x00a] = "DbgPrompt",
    /* 012 */ [0x00c] = "ExAcquireReadWriteLockExclusive",
    /* 013 */ [0x00d] = "ExAcquireReadWriteLockShared",
    /* 014 */ [0x00e] = "ExAllocatePool",
    /* 015 */ [0x00f] = "ExAllocatePoolWithTag",
    /* 016 */ [0x010] = "ExEventObjectType",
    /* 017 */ [0x011] = "ExFreePool",
    /* 018 */ [0x012] = "ExInitializeReadWriteLock",
    /* 019 */ [0x013] = "ExInterlockedAddLargeInteger",
    /* 020 */ [0x014] = "ExInterlockedAddLargeStatistic",
    /* 021 */ [0x015] = "ExInterlockedCompareExchange64",
    /* 022 */ [0x016] = "ExMutantObjectType",
    /* 023 */ [0x017] = "ExQueryPoolBlockSize",
    /* 024 */ [0x018] = "ExQueryNonVolatileSetting",
    /* 025 */ [0x019] = "ExReadWriteRefurbInfo",
    /* 026 */ [0x01a] = "ExRaiseException",
    /* 027 */ [0x01b] = "ExRaiseStatus",
    /* 028 */ [0x01c] = "ExReleaseReadWriteLock",
    /* 029 */ [0x01d] = "ExSaveNonVolatileSetting",
    /* 030 */ [0x01e] = "ExSemaphoreObjectType",
    /* 031 */ [0x01f] = "ExTimerObjectType",
    /* 032 */ [0x020] = "ExfInterlockedInsertHeadList",
    /* 033 */ [0x021] = "ExfInterlockedInsertTailList",
    /* 034 */ [0x022] = "ExfInterlockedRemoveHeadList",
    /* 035 */ [0x023] = "FscGetCacheSize",
    /* 036 */ [0x024] = "FscInvalidateIdleBlocks",
    /* 037 */ [0x025] = "FscSetCacheSize",
    /* 356 */ [0x164] = "HalBootSMCVideoMode",
    /* 038 */ [0x026] = "HalClearSoftwareInterrupt",
    /* 039 */ [0x027] = "HalDisableSystemInterrupt",
    /* 040 */ [0x028] = "HalDiskCachePartitionCount",
    /* 041 */ [0x029] = "HalDiskModelNumber",
    /* 042 */ [0x02a] = "HalDiskSerialNumber",
    /* 365 */ [0x16d] = "HalEnableSecureTrayEject",
    /* 043 */ [0x02b] = "HalEnableSystemInterrupt",
    /* 044 */ [0x02c] = "HalGetInterruptVector",
    /* 360 */ [0x168] = "HalInitiateShutdown",
    /* 358 */ [0x166] = "HalIsResetOrShutdownPending",
    /* 045 */ [0x02d] = "HalReadSMBusValue",
    /* 009 */ [0x009] = "HalReadSMCTrayState",
    /* 046 */ [0x02e] = "HalReadWritePCISpace",
    /* 047 */ [0x02f] = "HalRegisterShutdownNotification",
    /* 048 */ [0x030] = "HalRequestSoftwareInterrupt",
    /* 049 */ [0x031] = "HalReturnToFirmware",
    /* 050 */ [0x032] = "HalWriteSMBusValue",
    /* 366 */ [0x16e] = "HalWriteSMCScratchRegister",
    /* 357 */ [0x165] = "IdexChannelObject",
    /* 051 */ [0x033] = "InterlockedCompareExchange",
    /* 052 */ [0x034] = "InterlockedDecrement",
    /* 053 */ [0x035] = "InterlockedIncrement",
    /* 054 */ [0x036] = "InterlockedExchange",
    /* 055 */ [0x037] = "InterlockedExchangeAdd",
    /* 056 */ [0x038] = "InterlockedFlushSList",
    /* 057 */ [0x039] = "InterlockedPopEntrySList",
    /* 058 */ [0x03a] = "InterlockedPushEntrySList",
    /* 059 */ [0x03b] = "IoAllocateIrp",
    /* 060 */ [0x03c] = "IoBuildAsynchronousFsdRequest",
    /* 061 */ [0x03d] = "IoBuildDeviceIoControlRequest",
    /* 062 */ [0x03e] = "IoBuildSynchronousFsdRequest",
    /* 063 */ [0x03f] = "IoCheckShareAccess",
    /* 064 */ [0x040] = "IoCompletionObjectType",
    /* 065 */ [0x041] = "IoCreateDevice",
    /* 066 */ [0x042] = "IoCreateFile",
    /* 067 */ [0x043] = "IoCreateSymbolicLink",
    /* 068 */ [0x044] = "IoDeleteDevice",
    /* 069 */ [0x045] = "IoDeleteSymbolicLink",
    /* 070 */ [0x046] = "IoDeviceObjectType",
    /* 090 */ [0x05a] = "IoDismountVolume",
    /* 091 */ [0x05b] = "IoDismountVolumeByName",
    /* 071 */ [0x047] = "IoFileObjectType",
    /* 072 */ [0x048] = "IoFreeIrp",
    /* 073 */ [0x049] = "IoInitializeIrp",
    /* 074 */ [0x04a] = "IoInvalidDeviceRequest",
    /* 359 */ [0x167] = "IoMarkIrpMustComplete",
    /* 075 */ [0x04b] = "IoQueryFileInformation",
    /* 076 */ [0x04c] = "IoQueryVolumeInformation",
    /* 077 */ [0x04d] = "IoQueueThreadIrp",
    /* 078 */ [0x04e] = "IoRemoveShareAccess",
    /* 079 */ [0x04f] = "IoSetIoCompletion",
    /* 080 */ [0x050] = "IoSetShareAccess",
    /* 081 */ [0x051] = "IoStartNextPacket",
    /* 082 */ [0x052] = "IoStartNextPacketByKey",
    /* 083 */ [0x053] = "IoStartPacket",
    /* 084 */ [0x054] = "IoSynchronousDeviceIoControlRequest",
    /* 085 */ [0x055] = "IoSynchronousFsdRequest",
    /* 086 */ [0x056] = "IofCallDriver",
    /* 087 */ [0x057] = "IofCompleteRequest",
    /* 088 */ [0x058] = "KdDebuggerEnabled",
    /* 089 */ [0x059] = "KdDebuggerNotPresent",
    /* 092 */ [0x05c] = "KeAlertResumeThread",
    /* 093 */ [0x05d] = "KeAlertThread",
    /* 094 */ [0x05e] = "KeBoostPriorityThread",
    /* 095 */ [0x05f] = "KeBugCheck",
    /* 096 */ [0x060] = "KeBugCheckEx",
    /* 097 */ [0x061] = "KeCancelTimer",
    /* 098 */ [0x062] = "KeConnectInterrupt",
    /* 099 */ [0x063] = "KeDelayExecutionThread",
    /* 100 */ [0x064] = "KeDisconnectInterrupt",
    /* 101 */ [0x065] = "KeEnterCriticalRegion",
    /* 103 */ [0x067] = "KeGetCurrentIrql",
    /* 104 */ [0x068] = "KeGetCurrentThread",
    /* 105 */ [0x069] = "KeInitializeApc",
    /* 106 */ [0x06a] = "KeInitializeDeviceQueue",
    /* 107 */ [0x06b] = "KeInitializeDpc",
    /* 108 */ [0x06c] = "KeInitializeEvent",
    /* 109 */ [0x06d] = "KeInitializeInterrupt",
    /* 110 */ [0x06e] = "KeInitializeMutant",
    /* 111 */ [0x06f] = "KeInitializeQueue",
    /* 112 */ [0x070] = "KeInitializeSemaphore",
    /* 113 */ [0x071] = "KeInitializeTimerEx",
    /* 114 */ [0x072] = "KeInsertByKeyDeviceQueue",
    /* 115 */ [0x073] = "KeInsertDeviceQueue",
    /* 116 */ [0x074] = "KeInsertHeadQueue",
    /* 117 */ [0x075] = "KeInsertQueue",
    /* 118 */ [0x076] = "KeInsertQueueApc",
    /* 119 */ [0x077] = "KeInsertQueueDpc",
    /* 120 */ [0x078] = "KeInterruptTime",
    /* 121 */ [0x079] = "KeIsExecutingDpc",
    /* 122 */ [0x07a] = "KeLeaveCriticalRegion",
    /* 123 */ [0x07b] = "KePulseEvent",
    /* 124 */ [0x07c] = "KeQueryBasePriorityThread",
    /* 125 */ [0x07d] = "KeQueryInterruptTime",
    /* 126 */ [0x07e] = "KeQueryPerformanceCounter",
    /* 127 */ [0x07f] = "KeQueryPerformanceFrequency",
    /* 128 */ [0x080] = "KeQuerySystemTime",
    /* 129 */ [0x081] = "KeRaiseIrqlToDpcLevel",
    /* 130 */ [0x082] = "KeRaiseIrqlToSynchLevel",
    /* 131 */ [0x083] = "KeReleaseMutant",
    /* 132 */ [0x084] = "KeReleaseSemaphore",
    /* 133 */ [0x085] = "KeRemoveByKeyDeviceQueue",
    /* 134 */ [0x086] = "KeRemoveDeviceQueue",
    /* 135 */ [0x087] = "KeRemoveEntryDeviceQueue",
    /* 136 */ [0x088] = "KeRemoveQueue",
    /* 137 */ [0x089] = "KeRemoveQueueDpc",
    /* 138 */ [0x08a] = "KeResetEvent",
    /* 139 */ [0x08b] = "KeRestoreFloatingPointState",
    /* 140 */ [0x08c] = "KeResumeThread",
    /* 141 */ [0x08d] = "KeRundownQueue",
    /* 142 */ [0x08e] = "KeSaveFloatingPointState",
    /* 143 */ [0x08f] = "KeSetBasePriorityThread",
    /* 144 */ [0x090] = "KeSetDisableBoostThread",
    /* 145 */ [0x091] = "KeSetEvent",
    /* 146 */ [0x092] = "KeSetEventBoostPriority",
    /* 147 */ [0x093] = "KeSetPriorityProcess",
    /* 148 */ [0x094] = "KeSetPriorityThread",
    /* 149 */ [0x095] = "KeSetTimer",
    /* 150 */ [0x096] = "KeSetTimerEx",
    /* 151 */ [0x097] = "KeStallExecutionProcessor",
    /* 152 */ [0x098] = "KeSuspendThread",
    /* 153 */ [0x099] = "KeSynchronizeExecution",
    /* 154 */ [0x09a] = "KeSystemTime",
    /* 155 */ [0x09b] = "KeTestAlertThread",
    /* 156 */ [0x09c] = "KeTickCount",
    /* 157 */ [0x09d] = "KeTimeIncrement",
    /* 158 */ [0x09e] = "KeWaitForMultipleObjects",
    /* 159 */ [0x09f] = "KeWaitForSingleObject",
    /* 160 */ [0x0a0] = "KfRaiseIrql",
    /* 161 */ [0x0a1] = "KfLowerIrql",
    /* 162 */ [0x0a2] = "KiBugCheckData",
    /* 163 */ [0x0a3] = "KiUnlockDispatcherDatabase",
    /* 164 */ [0x0a4] = "LaunchDataPage",
    /* 165 */ [0x0a5] = "MmAllocateContiguousMemory",
    /* 166 */ [0x0a6] = "MmAllocateContiguousMemoryEx",
    /* 167 */ [0x0a7] = "MmAllocateSystemMemory",
    /* 168 */ [0x0a8] = "MmClaimGpuInstanceMemory",
    /* 169 */ [0x0a9] = "MmCreateKernelStack",
    /* 170 */ [0x0aa] = "MmDeleteKernelStack",
    /* 171 */ [0x0ab] = "MmFreeContiguousMemory",
    /* 172 */ [0x0ac] = "MmFreeSystemMemory",
    /* 173 */ [0x0ad] = "MmGetPhysicalAddress",
    /* 102 */ [0x066] = "MmGlobalData",
    /* 174 */ [0x0ae] = "MmIsAddressValid",
    /* 175 */ [0x0af] = "MmLockUnlockBufferPages",
    /* 176 */ [0x0b0] = "MmLockUnlockPhysicalPage",
    /* 177 */ [0x0b1] = "MmMapIoSpace",
    /* 178 */ [0x0b2] = "MmPersistContiguousMemory",
    /* 179 */ [0x0b3] = "MmQueryAddressProtect",
    /* 180 */ [0x0b4] = "MmQueryAllocationSize",
    /* 181 */ [0x0b5] = "MmQueryStatistics",
    /* 182 */ [0x0b6] = "MmSetAddressProtect",
    /* 183 */ [0x0b7] = "MmUnmapIoSpace",
    /* 184 */ [0x0b8] = "NtAllocateVirtualMemory",
    /* 185 */ [0x0b9] = "NtCancelTimer",
    /* 186 */ [0x0ba] = "NtClearEvent",
    /* 187 */ [0x0bb] = "NtClose",
    /* 188 */ [0x0bc] = "NtCreateDirectoryObject",
    /* 189 */ [0x0bd] = "NtCreateEvent",
    /* 190 */ [0x0be] = "NtCreateFile",
    /* 191 */ [0x0bf] = "NtCreateIoCompletion",
    /* 192 */ [0x0c0] = "NtCreateMutant",
    /* 193 */ [0x0c1] = "NtCreateSemaphore",
    /* 194 */ [0x0c2] = "NtCreateTimer",
    /* 195 */ [0x0c3] = "NtDeleteFile",
    /* 196 */ [0x0c4] = "NtDeviceIoControlFile",
    /* 197 */ [0x0c5] = "NtDuplicateObject",
    /* 198 */ [0x0c6] = "NtFlushBuffersFile",
    /* 199 */ [0x0c7] = "NtFreeVirtualMemory",
    /* 200 */ [0x0c8] = "NtFsControlFile",
    /* 201 */ [0x0c9] = "NtOpenDirectoryObject",
    /* 202 */ [0x0ca] = "NtOpenFile",
    /* 203 */ [0x0cb] = "NtOpenSymbolicLinkObject",
    /* 204 */ [0x0cc] = "NtProtectVirtualMemory",
    /* 205 */ [0x0cd] = "NtPulseEvent",
    /* 206 */ [0x0ce] = "NtQueueApcThread",
    /* 207 */ [0x0cf] = "NtQueryDirectoryFile",
    /* 208 */ [0x0d0] = "NtQueryDirectoryObject",
    /* 209 */ [0x0d1] = "NtQueryEvent",
    /* 210 */ [0x0d2] = "NtQueryFullAttributesFile",
    /* 211 */ [0x0d3] = "NtQueryInformationFile",
    /* 212 */ [0x0d4] = "NtQueryIoCompletion",
    /* 213 */ [0x0d5] = "NtQueryMutant",
    /* 214 */ [0x0d6] = "NtQuerySemaphore",
    /* 215 */ [0x0d7] = "NtQuerySymbolicLinkObject",
    /* 216 */ [0x0d8] = "NtQueryTimer",
    /* 217 */ [0x0d9] = "NtQueryVirtualMemory",
    /* 218 */ [0x0da] = "NtQueryVolumeInformationFile",
    /* 219 */ [0x0db] = "NtReadFile",
    /* 220 */ [0x0dc] = "NtReadFileScatter",
    /* 221 */ [0x0dd] = "NtReleaseMutant",
    /* 222 */ [0x0de] = "NtReleaseSemaphore",
    /* 223 */ [0x0df] = "NtRemoveIoCompletion",
    /* 224 */ [0x0e0] = "NtResumeThread",
    /* 225 */ [0x0e1] = "NtSetEvent",
    /* 226 */ [0x0e2] = "NtSetInformationFile",
    /* 227 */ [0x0e3] = "NtSetIoCompletion",
    /* 228 */ [0x0e4] = "NtSetSystemTime",
    /* 229 */ [0x0e5] = "NtSetTimerEx",
    /* 230 */ [0x0e6] = "NtSignalAndWaitForSingleObjectEx",
    /* 231 */ [0x0e7] = "NtSuspendThread",
    /* 232 */ [0x0e8] = "NtUserIoApcDispatcher",
    /* 233 */ [0x0e9] = "NtWaitForSingleObject",
    /* 234 */ [0x0ea] = "NtWaitForSingleObjectEx",
    /* 235 */ [0x0eb] = "NtWaitForMultipleObjectsEx",
    /* 236 */ [0x0ec] = "NtWriteFile",
    /* 237 */ [0x0ed] = "NtWriteFileGather",
    /* 238 */ [0x0ee] = "NtYieldExecution",
    /* 239 */ [0x0ef] = "ObCreateObject",
    /* 240 */ [0x0f0] = "ObDirectoryObjectType",
    /* 241 */ [0x0f1] = "ObInsertObject",
    /* 242 */ [0x0f2] = "ObMakeTemporaryObject",
    /* 243 */ [0x0f3] = "ObOpenObjectByName",
    /* 244 */ [0x0f4] = "ObOpenObjectByPointer",
    /* 245 */ [0x0f5] = "ObpObjectHandleTable",
    /* 246 */ [0x0f6] = "ObReferenceObjectByHandle",
    /* 247 */ [0x0f7] = "ObReferenceObjectByName",
    /* 248 */ [0x0f8] = "ObReferenceObjectByPointer",
    /* 249 */ [0x0f9] = "ObSymbolicLinkObjectType",
    /* 250 */ [0x0fa] = "ObfDereferenceObject",
    /* 251 */ [0x0fb] = "ObfReferenceObject",
    /* 252 */ [0x0fc] = "PhyGetLinkState",
    /* 253 */ [0x0fd] = "PhyInitialize",
    /* 254 */ [0x0fe] = "PsCreateSystemThread",
    /* 255 */ [0x0ff] = "PsCreateSystemThreadEx",
    /* 256 */ [0x100] = "PsQueryStatistics",
    /* 257 */ [0x101] = "PsSetCreateThreadNotifyRoutine",
    /* 258 */ [0x102] = "PsTerminateSystemThread",
    /* 259 */ [0x103] = "PsThreadObjectType",
    /* 260 */ [0x104] = "RtlAnsiStringToUnicodeString",
    /* 261 */ [0x105] = "RtlAppendStringToString",
    /* 262 */ [0x106] = "RtlAppendUnicodeStringToString",
    /* 263 */ [0x107] = "RtlAppendUnicodeToString",
    /* 264 */ [0x108] = "RtlAssert",
    /* 265 */ [0x109] = "RtlCaptureContext",
    /* 266 */ [0x10a] = "RtlCaptureStackBackTrace",
    /* 267 */ [0x10b] = "RtlCharToInteger",
    /* 268 */ [0x10c] = "RtlCompareMemory",
    /* 269 */ [0x10d] = "RtlCompareMemoryUlong",
    /* 270 */ [0x10e] = "RtlCompareString",
    /* 271 */ [0x10f] = "RtlCompareUnicodeString",
    /* 272 */ [0x110] = "RtlCopyString",
    /* 273 */ [0x111] = "RtlCopyUnicodeString",
    /* 274 */ [0x112] = "RtlCreateUnicodeString",
    /* 275 */ [0x113] = "RtlDowncaseUnicodeChar",
    /* 276 */ [0x114] = "RtlDowncaseUnicodeString",
    /* 277 */ [0x115] = "RtlEnterCriticalSection",
    /* 278 */ [0x116] = "RtlEnterCriticalSectionAndRegion",
    /* 279 */ [0x117] = "RtlEqualString",
    /* 280 */ [0x118] = "RtlEqualUnicodeString",
    /* 281 */ [0x119] = "RtlExtendedIntegerMultiply",
    /* 282 */ [0x11a] = "RtlExtendedLargeIntegerDivide",
    /* 283 */ [0x11b] = "RtlExtendedMagicDivide",
    /* 284 */ [0x11c] = "RtlFillMemory",
    /* 285 */ [0x11d] = "RtlFillMemoryUlong",
    /* 286 */ [0x11e] = "RtlFreeAnsiString",
    /* 287 */ [0x11f] = "RtlFreeUnicodeString",
    /* 288 */ [0x120] = "RtlGetCallersAddress",
    /* 289 */ [0x121] = "RtlInitAnsiString",
    /* 290 */ [0x122] = "RtlInitUnicodeString",
    /* 291 */ [0x123] = "RtlInitializeCriticalSection",
    /* 292 */ [0x124] = "RtlIntegerToChar",
    /* 293 */ [0x125] = "RtlIntegerToUnicodeString",
    /* 294 */ [0x126] = "RtlLeaveCriticalSection",
    /* 295 */ [0x127] = "RtlLeaveCriticalSectionAndRegion",
    /* 296 */ [0x128] = "RtlLowerChar",
    /* 297 */ [0x129] = "RtlMapGenericMask",
    /* 298 */ [0x12a] = "RtlMoveMemory",
    /* 299 */ [0x12b] = "RtlMultiByteToUnicodeN",
    /* 300 */ [0x12c] = "RtlMultiByteToUnicodeSize",
    /* 301 */ [0x12d] = "RtlNtStatusToDosError",
    /* 302 */ [0x12e] = "RtlRaiseException",
    /* 303 */ [0x12f] = "RtlRaiseStatus",
    /* 352 */ [0x160] = "RtlRip",
    /* 361 */ [0x169] = "RtlSnprintf",
    /* 362 */ [0x16a] = "RtlSprintf",
    /* 304 */ [0x130] = "RtlTimeFieldsToTime",
    /* 305 */ [0x131] = "RtlTimeToTimeFields",
    /* 306 */ [0x132] = "RtlTryEnterCriticalSection",
    /* 307 */ [0x133] = "RtlUlongByteSwap",
    /* 308 */ [0x134] = "RtlUnicodeStringToAnsiString",
    /* 309 */ [0x135] = "RtlUnicodeStringToInteger",
    /* 310 */ [0x136] = "RtlUnicodeToMultiByteN",
    /* 311 */ [0x137] = "RtlUnicodeToMultiByteSize",
    /* 312 */ [0x138] = "RtlUnwind",
    /* 313 */ [0x139] = "RtlUpcaseUnicodeChar",
    /* 314 */ [0x13a] = "RtlUpcaseUnicodeString",
    /* 315 */ [0x13b] = "RtlUpcaseUnicodeToMultiByteN",
    /* 316 */ [0x13c] = "RtlUpperChar",
    /* 317 */ [0x13d] = "RtlUpperString",
    /* 318 */ [0x13e] = "RtlUshortByteSwap",
    /* 363 */ [0x16b] = "RtlVsnprintf",
    /* 364 */ [0x16c] = "RtlVsprintf",
    /* 319 */ [0x13f] = "RtlWalkFrameChain",
    /* 320 */ [0x140] = "RtlZeroMemory",
    /* 354 */ [0x162] = "XboxAlternateSignatureKeys",
    /* 321 */ [0x141] = "XboxEEPROMKey",
    /* 322 */ [0x142] = "XboxHardwareInfo",
    /* 323 */ [0x143] = "XboxHDKey",
    /* 353 */ [0x161] = "XboxLANKey",
    /* 324 */ [0x144] = "XboxKrnlVersion",
    /* 325 */ [0x145] = "XboxSignatureKey",
    /* 326 */ [0x146] = "XeImageFileName",
    /* 327 */ [0x147] = "XeLoadSection",
    /* 355 */ [0x163] = "XePublicKeyData",
    /* 328 */ [0x148] = "XeUnloadSection",
    /* 329 */ [0x149] = "READ_PORT_BUFFER_UCHAR",
    /* 330 */ [0x14a] = "READ_PORT_BUFFER_USHORT",
    /* 331 */ [0x14b] = "READ_PORT_BUFFER_ULONG",
    /* 332 */ [0x14c] = "WRITE_PORT_BUFFER_UCHAR",
    /* 333 */ [0x14d] = "WRITE_PORT_BUFFER_USHORT",
    /* 334 */ [0x14e] = "WRITE_PORT_BUFFER_ULONG",
    /* 335 */ [0x14f] = "XcSHAInit",
    /* 336 */ [0x150] = "XcSHAUpdate",
    /* 337 */ [0x151] = "XcSHAFinal",
    /* 338 */ [0x152] = "XcRC4Key",
    /* 339 */ [0x153] = "XcRC4Crypt",
    /* 340 */ [0x154] = "XcHMAC",
    /* 341 */ [0x155] = "XcPKEncPublic",
    /* 342 */ [0x156] = "XcPKDecPrivate",
    /* 343 */ [0x157] = "XcPKGetKeyLen",
    /* 344 */ [0x158] = "XcVerifyPKCS1Signature",
    /* 345 */ [0x159] = "XcModExp",
    /* 346 */ [0x15a] = "XcDESKeyParity",
    /* 347 */ [0x15b] = "XcKeyTable",
    /* 348 */ [0x15c] = "XcBlockCrypt",
    /* 349 */ [0x15d] = "XcBlockCryptCBC",
    /* 350 */ [0x15e] = "XcCryptService",
    /* 351 */ [0x15f] = "XcUpdateCrypto",
    /* devkit */
    /* 007 */ [0x007] = "DbgLoadImageSymbols",
    /* 011 */ [0x00b] = "DbgUnLoadImageSymbols",
    /* 374 */ [0x176] = "MmDbgAllocateMemory",
    /* 375 */ [0x177] = "MmDbgFreeMemory",
    /* 376 */ [0x178] = "MmDbgQueryAvailablePages",
    /* 377 */ [0x179] = "MmDbgReleaseAddress",
    /* 378 */ [0x17a] = "MmDbgWriteCheck",
};

#undef ENTER
#undef LEAVE

