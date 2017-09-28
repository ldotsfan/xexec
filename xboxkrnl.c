
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <windef.h>
#include <winbase.h>

#define SYNC_THREADS        1   /* debugging */

#if SYNC_THREADS
static pthread_mutex_t *    xboxkrnl_thread_mutex = NULL;
static pthread_mutexattr_t *xboxkrnl_thread_mattr = NULL;
# define THREAD_LOCK        pthread_mutex_lock(xboxkrnl_thread_mutex)
# define THREAD_UNLOCK      pthread_mutex_unlock(xboxkrnl_thread_mutex)
#else
# define THREAD_LOCK
# define THREAD_UNLOCK
#endif

#define NAME(x)             [x] = #x
#define NAMEB(x,y)          [x] = #y

#define VAR_IN              0
#define VAR_OUT             1
#define STUB                2
#define DUMP                3
#define STRING              4

#define STACK_LEVEL_SPACING(x,y) do { \
            size_t __le = ((x) > 0) ? (x) * 4 : 0; \
            char *__st = (__le) ? malloc(__le + 1) : NULL; \
            if (__st) memset(__st, ' ', __le), __st[__le] = 0; \
            else __st = strdup(""); \
            *(y) = __st; \
        } while (0)

#define VARDUMP(x,y)        VARDUMP4(x,y,0,0)
#define VARDUMPD(x,y)       if (y) VARDUMP4(x,*y,0,0)
/* value name w/o flags: z = const *char[] name in relation to y; f = 0 */
#define VARDUMP2(x,y,z)     VARDUMP4(x,y,z,0)
/* value name w/ flags:  z == const *char[] index masking y;      f = 1 */
#define VARDUMP3(x,y,z)     VARDUMP4(x,y,z,1)
#define VARDUMP4(x,y,z,f) do { \
            unsigned long long __v = 0; \
            if (sizeof(typeof((y))) == 8) memcpy(&__v, &(y), 8); \
            else memcpy(&__v, &(y), sizeof(typeof((y)))); \
            char *__c = NULL; \
            unsigned __t = (unsigned)(z); \
            if (__t) __t = sizeof((z))/sizeof(char *); \
            if (!(f) && __t) { /* set c w/o flags */ \
                if (__t) __t = (__v < __t); \
                if (__t) __c = ((char **)(z))[__v]; \
            } else if (__t) { /* set c w/ flags */ \
                char *__n; \
                size_t __l, __len; \
                typeof(__t) __i; \
                for (__l = 0, __i = 0; __i <= __t; ++__i) { \
                    if (__i >= __t) { \
                        if (__l) __c[__l] = 0; \
                        __t = !!__c; \
                        break; \
                    } \
                    if ((__n = ((char **)(z))[__i]) && __v & (1 << __i)) { \
                        if (__l) ++__l; \
                        __len = strlen(__n); \
                        __c = realloc(__c, __l + __len + 2); \
                        strncpy(&__c[__l], __n, __len + 2); \
                        __l += __len; \
                        __c[__l] = '|'; \
                    } \
                } \
            } \
            char *__s; \
            STACK_LEVEL_SPACING(xboxkrnl_stack, &__s); \
            if ((x) == STRING) { \
                debug("%svar:  str: *%s = '%s' [0x%llx] (%llu)", \
                    __s, \
                    #y, \
                    (y), \
                    __v, \
                    __v); \
            } else { \
                debug("%svar: %s: *%s = 0x%llx (%llu)%s%s%s", \
                    __s, \
                    ((x) > 1) ? (((x) == STUB)?"stub":"dump") : ((x)?" out":"  in"), \
                    #y, \
                    __v, \
                    __v, \
                    (__c) ? " (" : "", \
                    (__c) ?  __c : "", \
                    (__c) ?  ")" : ""); \
            } \
            free(__s); \
            if ((f) && __c) free(__c); \
        } while (0)

static void
xboxkrnl_enter(int8_t *stack, const char *func, const char *prefix, int lock) {
    char *s;

    if (lock) THREAD_LOCK;

    STACK_LEVEL_SPACING(*stack, &s);
    debug("%senter%s%s%s: --> %s()",
        s,
        (prefix) ?   " (" : "",
        (prefix) ? prefix : "",
        (prefix) ?    ")" : "",
        func);
    free(s);

    if (++*stack & 0xc0) *stack = 0;
}

static void
xboxkrnl_leave(int8_t *stack, const char *func, const char *prefix, int lock) {
    char *s;

    if (--*stack & 0xc0) *stack = 0;

    STACK_LEVEL_SPACING(*stack, &s);
    debug("%sleave%s%s%s: <-- %s()",
        s,
        (prefix) ?   " (" : "",
        (prefix) ? prefix : "",
        (prefix) ?    ")" : "",
        func);
    free(s);

    if (lock) THREAD_UNLOCK;
}

static void
xboxkrnl_print(int8_t stack, const char *format, ...) {
    va_list args;
    char buf[4096];
    size_t len;
    char *s;
    int ret;

    if (!dbg) return;

    STACK_LEVEL_SPACING(stack, &s);
    ret = sizeof(buf) - 1;
    len = strlen(s);
    strncpy(buf, s, ret);
    free(s);

    if (len >= (typeof(len))ret) return;

    va_start(args, format);
    ret = vsnprintf(&buf[len], ret - len, format, args);
    va_end(args);

    if (ret > 0) {
        ret += len;
        buf[ret] = 0;
        debug(buf, 0);
    }
}

#define INT3                __asm__ __volatile__("int3")
#define STDCALL             __attribute__((__stdcall__))
#define FASTCALL            __attribute__((__fastcall__))

static void *               xboxkrnl_mem_contiguous = NULL;

#define ENTER               xboxkrnl_enter(&xboxkrnl_stack, __func__, NULL, 1)
#define LEAVE               xboxkrnl_leave(&xboxkrnl_stack, __func__, NULL, 1)
#define PRINT(x,...)        xboxkrnl_print(xboxkrnl_stack, (x), __VA_ARGS__)
static int8_t               xboxkrnl_stack = 0;

#define ENTER_DPC           xboxkrnl_enter(&xboxkrnl_dpc_stack, __func__, "dpc", 0)
#define LEAVE_DPC           xboxkrnl_leave(&xboxkrnl_dpc_stack, __func__, "dpc", 0)
#define PRINT_DPC(x,...)    xboxkrnl_print(xboxkrnl_dpc_stack, (x), __VA_ARGS__)
static int8_t               xboxkrnl_dpc_stack = 0;
static pthread_t *          xboxkrnl_dpc_thread = NULL;
static pthread_mutexattr_t *xboxkrnl_dpc_mattr = NULL;
static pthread_mutex_t *    xboxkrnl_dpc_mutex = NULL;
static pthread_cond_t *     xboxkrnl_dpc_cond = NULL;
#define DPC_SIGNAL          pthread_cond_signal(xboxkrnl_dpc_cond)

/* winnt */
typedef struct _listentry {
    struct _listentry *     Flink;
    struct _listentry *     Blink;
} PACKED xboxkrnl_listentry;

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
} PACKED xboxkrnl_dpc;
#if 0
static xboxkrnl_dpc *       xboxkrnl_dpc_list = NULL;
static size_t               xboxkrnl_dpc_list_sz = 0;
#endif
#define ENTER_EVENT         xboxkrnl_enter(&xboxkrnl_event_stack, __func__, "event", 0)
#define LEAVE_EVENT         xboxkrnl_leave(&xboxkrnl_event_stack, __func__, "event", 0)
#define PRINT_EVENT(x,...)  xboxkrnl_print(xboxkrnl_event_stack, (x), __VA_ARGS__)
static int8_t               xboxkrnl_event_stack = 0;
static pthread_t *          xboxkrnl_event_thread = NULL;
static pthread_mutexattr_t *xboxkrnl_event_mattr = NULL;
static pthread_mutex_t *    xboxkrnl_event_mutex = NULL;
static pthread_cond_t *     xboxkrnl_event_cond = NULL;
#define EVENT_SIGNAL        pthread_cond_signal(xboxkrnl_event_cond)

typedef struct {
    struct _dispatcherheader {
        uint8_t             Type;
        uint8_t             Absolute;
        uint8_t             Size;
        uint8_t             Inserted;
        int32_t             SignalState;
        xboxkrnl_listentry  WaitListHead;
    } PACKED                Header;
    uint64_t                DueTime;
    xboxkrnl_listentry      TimerListEntry;
    xboxkrnl_dpc *          Dpc;
    int32_t                 Period;
} PACKED xboxkrnl_timer;

/* pci */
struct xpci {
    const uint8_t           bus;
    const uint8_t           device;
    const uint8_t           function;
    const uint8_t           irq;

    const char *            name;

    const uint16_t          ioreg_base_0;
    const uint16_t          ioreg_size_0;
    const uint16_t          ioreg_base_1;
    const uint16_t          ioreg_size_1;
    const uint16_t          ioreg_base_2;
    const uint16_t          ioreg_size_2;
    const uint16_t          ioreg_base_3;
    const uint16_t          ioreg_size_3;
    const uint16_t          ioreg_base_4;
    const uint16_t          ioreg_size_4;

    const uint32_t          memreg_base;
    const uint32_t          memreg_size;

    void *                  memreg;

    void *                  irq_kinterrupt;
    uint32_t                irq_connected;
    uint32_t                irq_level;
    void *                  irq_isr_routine;
    void *                  irq_isr_context;
};

enum {
    XPCI_HOSTBRIDGE =  0,
    XPCI_LPCBRIDGE, /* 1 */
    XPCI_SMBUS,     /* 2 */
    XPCI_USB0,      /* 3 */
    XPCI_USB1,      /* 4 */
    XPCI_NIC,       /* 5 */
    XPCI_APU,       /* 6 */
    XPCI_ACI,       /* 7 */
    XPCI_IDE,       /* 8 */
    XPCI_AGPBRIDGE, /* 9 */
    XPCI_GPU,       /* 10 */
    XPCI_INTERNAL   /* 11 */
};

static struct xpci xpci[] = {
    {
        0, 0, 0, -1,                    /* bus, device, function, irq */
        "HOSTBRIDGE",                   /* name */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   /* ioreg 0-4 */
        0, 0,                           /* memreg */
        NULL,                           /* mapped memreg */
        NULL, 0, 0, NULL, NULL          /* irq connection */
    },
    {
        0, 1, 0, -1,
        "LPCBRIDGE",
        0x8000, 0x0100, 0, 0, 0, 0, 0, 0, 0, 0,
             0,      0,
        NULL,
        NULL, 0, 0, NULL, NULL
    },
    {
        0, 1, 1, -1,
        "SMBUS",
        0, 0, 0xc000, 0x0010, 0xc200, 0x0020, 0, 0, 0, 0,
        0, 0,
        NULL,
        NULL, 0, 0, NULL, NULL
    },
    {
        0, 2, 0, 1,
        "USB0",
                 0,          0, 0, 0, 0, 0, 0, 0, 0, 0,
        0xfed00000, 0x00001000,
        NULL,
        NULL, 0, 0, NULL, NULL
    },
    {
        0, 3, 0, 9,
        "USB1",
                 0,          0, 0, 0, 0, 0, 0, 0, 0, 0,
        0xfed08000, 0x00001000,
        NULL,
        NULL, 0, 0, NULL, NULL
    },
    {
        0, 4, 0, 4,
        "NIC (NVNet)",
                 0,          0, 0xe000, 0x0008, 0, 0, 0, 0, 0, 0,
        0xfef00000, 0x00000400,
        NULL,
        NULL, 0, 0, NULL, NULL
    },
    {
        0, 5, 0, 5,
        "APU",
                 0,          0, 0, 0, 0, 0, 0, 0, 0, 0,
        0xfe800000, 0x00080000,
        NULL,
        NULL, 0, 0, NULL, NULL
    },
    {
        0, 6, 0, 6,
        "ACI (AC97)",
            0xd000,     0x0100, 0xd200, 0x0080, 0, 0, 0, 0, 0, 0,
        0xfec00000, 0x00001000,
        NULL,
        NULL, 0, 0, NULL, NULL
    },
    {
        0, 9, 0, 14,
        "IDE",
        0, 0, 0, 0, 0, 0, 0, 0, 0xff60, 0x0010,
        0, 0,
        NULL,
        NULL, 0, 0, NULL, NULL
    },
    {
        0, 30, 0, -1,
        "AGPBRIDGE",
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0,
        NULL,
        NULL, 0, 0, NULL, NULL
    },
    {
        1, 0, 0, 3,
        "GPU (NV2A)",
                 0,          0, 0, 0, 0, 0, 0, 0, 0, 0,
        0xfd000000, 0x01000000,
        NULL,
        NULL, 0, 0, NULL, NULL
    },
    {
        0, 0, 0, -1,
        NULL,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0,
        NULL,
        NULL, 0, 0, NULL, NULL
    }
};

/* gpu */
#include "nv2a.c"

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
} PACKED xboxkrnl_meminfo;

typedef struct {
    uint16_t                Length;
    uint16_t                MaximumLength;
    char *                  Buffer;
} PACKED xboxkrnl_string;

typedef struct {
    void *                  RootDirectory;
    xboxkrnl_string *       ObjectName;
    uint32_t                Attributes;
} PACKED xboxkrnl_objattr;

typedef struct {
    int                     Status;
    uint32_t                Information;
} PACKED xboxkrnl_iostatus;

enum XBOXKRNL_FILE_IO_STATUS {
    XBOXKRNL_FILE_SUPERSEDED                =  0,
    XBOXKRNL_FILE_OPENED,                   /* 1 */
    XBOXKRNL_FILE_CREATED,                  /* 2 */
    XBOXKRNL_FILE_OVERWRITTEN,              /* 3 */
    XBOXKRNL_FILE_EXISTS,                   /* 4 */
    XBOXKRNL_FILE_DOES_NOT_EXIST,           /* 5 */
};

static const char *const xboxkrnl_file_io_status_name[] = {
    NAME(XBOXKRNL_FILE_SUPERSEDED),
    NAME(XBOXKRNL_FILE_OPENED),
    NAME(XBOXKRNL_FILE_CREATED),
    NAME(XBOXKRNL_FILE_OVERWRITTEN),
    NAME(XBOXKRNL_FILE_EXISTS),
    NAME(XBOXKRNL_FILE_DOES_NOT_EXIST),
};

enum XBOXKRNL_FILE_OPEN_CREATE {
    XBOXKRNL_FILE_DIRECTORY_FILE            = 1 << 0,   /* 0x1 */
    XBOXKRNL_FILE_WRITE_THROUGH             = 1 << 1,   /* 0x2 */
    XBOXKRNL_FILE_SEQUENTIAL_ONLY           = 1 << 2,   /* 0x4 */
    XBOXKRNL_FILE_NO_INTERMEDIATE_BUFFERING = 1 << 3,   /* 0x8 */
    XBOXKRNL_FILE_SYNCHRONOUS_IO_ALERT      = 1 << 4,   /* 0x10 */
    XBOXKRNL_FILE_SYNCHRONOUS_IO_NONALERT   = 1 << 5,   /* 0x20 */
    XBOXKRNL_FILE_NON_DIRECTORY_FILE        = 1 << 6,   /* 0x40 */
    XBOXKRNL_FILE_CREATE_TREE_CONNECTION    = 1 << 7,   /* 0x80 */
    XBOXKRNL_FILE_COMPLETE_IF_OPLOCKED      = 1 << 8,   /* 0x100 */
    XBOXKRNL_FILE_NO_EA_KNOWLEDGE           = 1 << 9,   /* 0x200 */
    XBOXKRNL_FILE_OPEN_FOR_RECOVERY         = 1 << 10,  /* 0x400 */
    XBOXKRNL_FILE_RANDOM_ACCESS             = 1 << 11,  /* 0x800 */
    XBOXKRNL_FILE_DELETE_ON_CLOSE           = 1 << 12,  /* 0x1000 */
    XBOXKRNL_FILE_OPEN_BY_FILE_ID           = 1 << 13,  /* 0x2000 */
    XBOXKRNL_FILE_OPEN_FOR_BACKUP_INTENT    = 1 << 14,  /* 0x4000 */
    XBOXKRNL_FILE_NO_COMPRESSION            = 1 << 15,  /* 0x8000 */
    XBOXKRNL_FILE_RESERVE_OPFILTER          = 1 << 20,  /* 0x100000 */
    XBOXKRNL_FILE_OPEN_REPARSE_POINT        = 1 << 21,  /* 0x200000 */
    XBOXKRNL_FILE_OPEN_NO_RECALL            = 1 << 22,  /* 0x400000 */
    XBOXKRNL_FILE_OPEN_FOR_FREE_SPACE_QUERY = 1 << 23,  /* 0x800000 */
    XBOXKRNL_FILE_COPY_STRUCTURED_STORAGE   = 0x41,
    XBOXKRNL_FILE_STRUCTURED_STORAGE        = 0x441,
    XBOXKRNL_FILE_VALID_OPTION_FLAGS        = 0xffffff,
    XBOXKRNL_FILE_VALID_SET_FLAGS           = 0x36,
};

static const char *const xboxkrnl_file_open_create_name[] = {
    NAMEB(0,  XBOXKRNL_FILE_DIRECTORY_FILE),
    NAMEB(1,  XBOXKRNL_FILE_WRITE_THROUGH),
    NAMEB(2,  XBOXKRNL_FILE_SEQUENTIAL_ONLY),
    NAMEB(3,  XBOXKRNL_FILE_NO_INTERMEDIATE_BUFFERING),
    NAMEB(4,  XBOXKRNL_FILE_SYNCHRONOUS_IO_ALERT),
    NAMEB(5,  XBOXKRNL_FILE_SYNCHRONOUS_IO_NONALERT),
    NAMEB(6,  XBOXKRNL_FILE_NON_DIRECTORY_FILE),
    NAMEB(7,  XBOXKRNL_FILE_CREATE_TREE_CONNECTION),
    NAMEB(8,  XBOXKRNL_FILE_COMPLETE_IF_OPLOCKED),
    NAMEB(9,  XBOXKRNL_FILE_NO_EA_KNOWLEDGE),
    NAMEB(10, XBOXKRNL_FILE_OPEN_FOR_RECOVERY),
    NAMEB(11, XBOXKRNL_FILE_RANDOM_ACCESS),
    NAMEB(12, XBOXKRNL_FILE_DELETE_ON_CLOSE),
    NAMEB(13, XBOXKRNL_FILE_OPEN_BY_FILE_ID),
    NAMEB(14, XBOXKRNL_FILE_OPEN_FOR_BACKUP_INTENT),
    NAMEB(15, XBOXKRNL_FILE_NO_COMPRESSION),
    NAMEB(20, XBOXKRNL_FILE_RESERVE_OPFILTER),
    NAMEB(21, XBOXKRNL_FILE_OPEN_REPARSE_POINT),
    NAMEB(22, XBOXKRNL_FILE_OPEN_NO_RECALL),
    NAMEB(23, XBOXKRNL_FILE_OPEN_FOR_FREE_SPACE_QUERY),
};

enum XBOXKRNL_FILE_ACCESS_MASK {
    XBOXKRNL_FILE_READ_DATA                 = 1 << 0,   /* 0x1 */   /* file & pipe */
//  XBOXKRNL_FILE_LIST_DIRECTORY            = 1 << 0,   /* 0x1 */   /* directory */
    XBOXKRNL_FILE_WRITE_DATA                = 1 << 1,   /* 0x2 */   /* file & pipe */
//  XBOXKRNL_FILE_ADD_FILE                  = 1 << 1,   /* 0x2 */   /* directory */
    XBOXKRNL_FILE_APPEND_DATA               = 1 << 2,   /* 0x4 */   /* file */
//  XBOXKRNL_FILE_ADD_SUBDIRECTORY          = 1 << 2,   /* 0x4 */   /* directory */
//  XBOXKRNL_FILE_CREATE_PIPE_INSTANCE      = 1 << 2,   /* 0x4 */   /* pipe */
    XBOXKRNL_FILE_READ_EA                   = 1 << 3,   /* 0x8 */   /* file & directory */
    XBOXKRNL_FILE_WRITE_EA                  = 1 << 4,   /* 0x10 */  /* file & directory */
    XBOXKRNL_FILE_EXECUTE                   = 1 << 5,   /* 0x20 */  /* file */
//  XBOXKRNL_FILE_TRAVERSE                  = 1 << 5,   /* 0x20 */  /* directory */
    XBOXKRNL_FILE_DELETE_CHILD              = 1 << 6,   /* 0x40 */  /* directory */
    XBOXKRNL_FILE_READ_ATTRIBUTES           = 1 << 7,   /* 0x80 */  /* all */
    XBOXKRNL_FILE_WRITE_ATTRIBUTES          = 1 << 8,   /* 0x100 */ /* all */
    XBOXKRNL_DELETE                         = 1 << 16,  /* 0x10000 */
    XBOXKRNL_READ_CONTROL                   = 1 << 17,  /* 0x20000 */
    XBOXKRNL_WRITE_DAC                      = 1 << 18,  /* 0x40000 */
    XBOXKRNL_WRITE_OWNER                    = 1 << 19,  /* 0x80000 */
    XBOXKRNL_SYNCHRONIZE                    = 1 << 20,  /* 0x100000 */
    XBOXKRNL_GENERIC_READ                   = 1 << 31,  /* 0x80000000 */
    XBOXKRNL_GENERIC_WRITE                  = 1 << 30,  /* 0x40000000 */
    XBOXKRNL_GENERIC_EXECUTE                = 1 << 29,  /* 0x20000000 */
    XBOXKRNL_GENERIC_ALL                    = 1 << 28,  /* 0x10000000 */
};

static const char *const xboxkrnl_file_access_mask_name[] = {
    NAMEB(0,  XBOXKRNL_FILE_READ_DATA),
//  NAMEB(0,  XBOXKRNL_FILE_LIST_DIRECTORY),
    NAMEB(1,  XBOXKRNL_FILE_WRITE_DATA),
//  NAMEB(1,  XBOXKRNL_FILE_ADD_FILE),
    NAMEB(2,  XBOXKRNL_FILE_APPEND_DATA),
//  NAMEB(2,  XBOXKRNL_FILE_ADD_SUBDIRECTORY),
//  NAMEB(2,  XBOXKRNL_FILE_CREATE_PIPE_INSTANCE),
    NAMEB(3,  XBOXKRNL_FILE_READ_EA),
    NAMEB(4,  XBOXKRNL_FILE_WRITE_EA),
    NAMEB(5,  XBOXKRNL_FILE_EXECUTE),
//  NAMEB(5,  XBOXKRNL_FILE_TRAVERSE),
    NAMEB(6,  XBOXKRNL_FILE_DELETE_CHILD),
    NAMEB(7,  XBOXKRNL_FILE_READ_ATTRIBUTES),
    NAMEB(8,  XBOXKRNL_FILE_WRITE_ATTRIBUTES),
    NAMEB(16, XBOXKRNL_DELETE),
    NAMEB(17, XBOXKRNL_READ_CONTROL),
    NAMEB(18, XBOXKRNL_WRITE_DAC),
    NAMEB(19, XBOXKRNL_WRITE_OWNER),
    NAMEB(20, XBOXKRNL_SYNCHRONIZE),
    NAMEB(31, XBOXKRNL_GENERIC_READ),
    NAMEB(30, XBOXKRNL_GENERIC_WRITE),
    NAMEB(29, XBOXKRNL_GENERIC_EXECUTE),
    NAMEB(28, XBOXKRNL_GENERIC_ALL),
};

enum XBOXKRNL_FILE_CREATE_DISPOSITION {
    XBOXKRNL_FILE_SUPERSEDE                 =  0,
    XBOXKRNL_FILE_OPEN,                     /* 1 */
    XBOXKRNL_FILE_CREATE,                   /* 2 */
    XBOXKRNL_FILE_OPEN_IF,                  /* 3 */
    XBOXKRNL_FILE_OVERWRITE,                /* 4 */
    XBOXKRNL_FILE_OVERWRITE_IF,             /* 5 */
};

static const char *const xboxkrnl_file_create_disposition_name[] = {
    NAME(XBOXKRNL_FILE_SUPERSEDE),
    NAME(XBOXKRNL_FILE_OPEN),
    NAME(XBOXKRNL_FILE_CREATE),
    NAME(XBOXKRNL_FILE_OPEN_IF),
    NAME(XBOXKRNL_FILE_OVERWRITE),
    NAME(XBOXKRNL_FILE_OVERWRITE_IF),
};

typedef struct {
    char                    path[4096];
    int                     fd;
    DIR *                   dir;
} xboxkrnl_file;

/* enums */
enum XC_VALUE_INDEX {
    XC_TIMEZONE_BIAS                        = 0,
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
    XC_MAX_OS                               = 0xff,

    /* start of factory settings */
    XC_FACTORY_START_INDEX,
    XC_FACTORY_SERIAL_NUMBER                = XC_FACTORY_START_INDEX,
    XC_FACTORY_ETHERNET_ADDR,
    XC_FACTORY_ONLINE_KEY,
    XC_FACTORY_AV_REGION,
    XC_FACTORY_GAME_REGION,

    /* end of factory settings */
    XC_MAX_FACTORY                          = 0x1ff,

    /* special value to access the encryption section of the EEPROM
       or to access the entire EEPROM at once */
    XC_ENCRYPTED_SECTION                    = 0xfffe,
    XC_MAX_ALL                              = 0xffff,
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
//  NAME(XC_FACTORY_START_INDEX),
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
    XC_LANGUAGE_UNKNOWN                     = 0,
    XC_LANGUAGE_ENGLISH                     = 1,
    XC_LANGUAGE_JAPANESE                    = 2,
    XC_LANGUAGE_GERMAN                      = 3,
    XC_LANGUAGE_FRENCH                      = 4,
    XC_LANGUAGE_SPANISH                     = 5,
    XC_LANGUAGE_ITALIAN                     = 6,
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
    XC_VIDEO_FLAGS_WIDESCREEN               = 1 << 0,   /* 0x1 */
    XC_VIDEO_FLAGS_HDTV_720p                = 1 << 1,   /* 0x2 */
    XC_VIDEO_FLAGS_HDTV_1080i               = 1 << 2,   /* 0x4 */
    XC_VIDEO_FLAGS_HDTV_480p                = 1 << 3,   /* 0x8 */
    XC_VIDEO_FLAGS_LETTERBOX                = 1 << 4,   /* 0x10 */
    XC_VIDEO_FLAGS_PAL_60Hz                 = 1 << 6,   /* 0x40 */
};

static const char *const xboxkrnl_xc_video_flags_name[] = {
    NAMEB(0, XC_VIDEO_FLAGS_WIDESCREEN),
    NAMEB(1, XC_VIDEO_FLAGS_HDTV_720p),
    NAMEB(2, XC_VIDEO_FLAGS_HDTV_1080i),
    NAMEB(3, XC_VIDEO_FLAGS_HDTV_480p),
    NAMEB(4, XC_VIDEO_FLAGS_LETTERBOX),
    NAMEB(6, XC_VIDEO_FLAGS_PAL_60Hz),
};

#define XC_AUDIO_FLAGS_BASIC(c)             ((c) & 0x0000ffff)
#define XC_AUDIO_FLAGS_ENCODED(c)           ((c) & 0xffff0000)
#define XC_AUDIO_FLAGS_COMBINED(b,e)        (XC_AUDIO_FLAGS_BASIC(b) | XC_AUDIO_FLAGS_ENCODED(e))

enum XC_AUDIO_FLAGS {
    /* basic */
    XC_AUDIO_FLAGS_STEREO                   = 0 << 0,   /* 0x0 */
    XC_AUDIO_FLAGS_MONO                     = 1 << 0,   /* 0x1 */
    XC_AUDIO_FLAGS_SURROUND                 = 1 << 1,   /* 0x2 */
    /* encoded */
    XC_AUDIO_FLAGS_ENABLE_AC3               = 1 << 16,  /* 0x10000 */
    XC_AUDIO_FLAGS_ENABLE_DTS               = 1 << 17,  /* 0x20000 */
};

static const char *const xboxkrnl_xc_audio_flags_name[] = {
    NAME(XC_AUDIO_FLAGS_STEREO),
    NAME(XC_AUDIO_FLAGS_MONO),
    NAME(XC_AUDIO_FLAGS_SURROUND),
    NAME(XC_AUDIO_FLAGS_ENABLE_AC3),
    NAME(XC_AUDIO_FLAGS_ENABLE_DTS),
};

enum XC_PARENTAL_CONTROL {
    XC_PC_ESRB_ALL                          = 0,
    XC_PC_ESRB_ADULT                        = 1,
    XC_PC_ESRB_MATURE                       = 2,
    XC_PC_ESRB_TEEN                         = 3,
    XC_PC_ESRB_EVERYONE                     = 4,
    XC_PC_ESRB_KIDS_TO_ADULTS               = 5,
    XC_PC_ESRB_EARLY_CHILDHOOD              = 6,
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
    XC_MISC_FLAG_AUTOPOWERDOWN              = 1 << 0,   /* 0x1 */
    XC_MISC_FLAG_DONT_USE_DST               = 1 << 1,   /* 0x2 */
};

static const char *const xboxkrnl_xc_misc_flags_name[] = {
    NAMEB(0, XC_MISC_FLAG_AUTOPOWERDOWN),
    NAMEB(1, XC_MISC_FLAG_DONT_USE_DST),
};

enum XC_FACTORY_AV_REGION {
    XC_VIDEO_STANDARD_NTSC_M                = 1,
    XC_VIDEO_STANDARD_NTSC_J                = 2,
    XC_VIDEO_STANDARD_PAL_I                 = 3,
    XC_VIDEO_STANDARD_PAL_M                 = 4,
};

static const char *const xboxkrnl_xc_factory_av_region_name[] = {
    NAME(XC_VIDEO_STANDARD_NTSC_M),
    NAME(XC_VIDEO_STANDARD_NTSC_J),
    NAME(XC_VIDEO_STANDARD_PAL_I),
    NAME(XC_VIDEO_STANDARD_PAL_M),
};

enum XC_FACTORY_GAME_REGION {
    XC_GAME_REGION_NA                       = 1 << 0,   /* 0x1 */
    XC_GAME_REGION_JAPAN                    = 1 << 1,   /* 0x2 */
    XC_GAME_REGION_RESTOFWORLD              = 1 << 2,   /* 0x4 */
    XC_GAME_REGION_MANUFACTURING            = 1 << 31,  /* 0x80000000 */
};

static const char *const xboxkrnl_xc_factory_game_region_name[] = {
    NAMEB(0,  XC_GAME_REGION_NA),
    NAMEB(1,  XC_GAME_REGION_JAPAN),
    NAMEB(2,  XC_GAME_REGION_RESTOFWORLD),
    NAMEB(31, XC_GAME_REGION_MANUFACTURING),
};

enum XC_AV_PACK {
    XC_AV_PACK_SCART                        = 0,
    XC_AV_PACK_HDTV                         = 1,
    XC_AV_PACK_VGA                          = 2,
    XC_AV_PACK_RFU                          = 3,
    XC_AV_PACK_SVIDEO                       = 4,
    XC_AV_PACK_STANDARD                     = 6,
    XC_AV_PACK_NONE                         = 7,
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

static const uint32_t xboxkrnl_os[] = {
    [XC_LANGUAGE]                           = XC_LANGUAGE_ENGLISH,
    [XC_VIDEO_FLAGS]                        = XC_VIDEO_FLAGS_LETTERBOX,
    [XC_AUDIO_FLAGS]                        = XC_AUDIO_FLAGS_STEREO,
    [XC_PARENTAL_CONTROL_GAMES]             = XC_PC_ESRB_ALL, // Zapper queries this.
    [XC_PARENTAL_CONTROL_MOVIES]            = XC_PC_ESRB_ALL, // Xbox Dashboard queries this.
    [XC_MISC_FLAGS]                         = 0,
    [XC_FACTORY_AV_REGION]                  = XC_VIDEO_STANDARD_NTSC_M,
    [XC_FACTORY_GAME_REGION]                = /*XC_GAME_REGION_MANUFACTURING | */XC_GAME_REGION_NA,
    [XC_MAX_OS]                             = 0,
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
#if 0
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
static void *
xboxkrnl_worker(void *arg) {
    if (!arg) {
        ENTER;

#define XBOXKRNL_THREAD(x,y) do { \
            xboxkrnl_ ## x ## _thread = calloc(1, sizeof(*xboxkrnl_ ## x ## _thread)); \
            xboxkrnl_ ## x ## _mattr = calloc(1, sizeof(*xboxkrnl_ ## x ## _mattr)); \
            xboxkrnl_ ## x ## _mutex = calloc(1, sizeof(*xboxkrnl_ ## x ## _mutex)); \
            xboxkrnl_ ## x ## _cond = calloc(1, sizeof(*xboxkrnl_ ## x ## _cond)); \
            \
            VARDUMP(DUMP, xboxkrnl_ ## x ## _thread); \
            VARDUMP(DUMP, xboxkrnl_ ## x ## _mattr); \
            VARDUMP(DUMP, xboxkrnl_ ## x ## _mutex); \
            VARDUMP(DUMP, xboxkrnl_ ## x ## _cond); \
            \
            pthread_mutexattr_init(xboxkrnl_ ## x ## _mattr); \
            pthread_mutexattr_settype(xboxkrnl_ ## x ## _mattr, PTHREAD_MUTEX_RECURSIVE); \
            pthread_mutex_init(xboxkrnl_ ## x ## _mutex, xboxkrnl_ ## x ## _mattr); \
            pthread_cond_init(xboxkrnl_ ## x ## _cond, NULL); \
            \
            PRINT("/* starting " #x " thread */", 0); \
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

            PRINT_DPC("/* waiting for deferred procedure routines */", 0);

            pthread_cond_wait(xboxkrnl_dpc_cond, xboxkrnl_dpc_mutex);

            pthread_mutex_unlock(xboxkrnl_dpc_mutex);
        }

        LEAVE_DPC;
        pthread_exit(NULL);
    } else if (arg == (void *)2) {
        ENTER_EVENT;

        for (;;) {
            pthread_mutex_lock(xboxkrnl_event_mutex);

            PRINT_EVENT("/* waiting for event routines */", 0);

            pthread_cond_wait(xboxkrnl_event_cond, xboxkrnl_event_mutex);

            pthread_mutex_unlock(xboxkrnl_event_mutex);
        }

        LEAVE_EVENT;
        pthread_exit(NULL);
    }

    return NULL;
}

static int
xboxkrnl_init_hw(void) {
    register void *ptr;
    register size_t sz;
    register size_t i;
    ENTER;

    for (i = 0; i < sizeof(xpci) / sizeof(*xpci) && xpci[i].name; ++i) {
        if ((ptr = (typeof(ptr))xpci[i].memreg_base)) {
            sz = xpci[i].memreg_size;
            PRINT("mmap(): '%s': reserve: address 0x%.08x size 0x%.08x (%lu)", xpci[i].name, ptr, sz, sz);
            if (mmap(ptr,
                    sz,
                    PROT_NONE,
                    MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED | MAP_NORESERVE,
                    -1,
                    0) != ptr) {
                PRINT("error: mmap(): failed to reserve emulated hardware: address %p size %p (%lu): '%s'",
                    ptr,
                    sz,
                    sz,
                    strerror(errno));
                LEAVE;
                return 1;
            }
        }
    }
    for (i = 0; i < sizeof(xpci) / sizeof(*xpci) && xpci[i].name; ++i) {
        if ((ptr = (typeof(ptr))xpci[i].memreg_base)) {
            sz = xpci[i].memreg_size;
            if ((ptr = mmap(NULL,
                    sz,
                    PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
                    -1,
                    0)) == MAP_FAILED) {
                PRINT("error: mmap(): failed to allocate emulated hardware: size %p (%lu): '%s'",
                    sz,
                    sz,
                    strerror(errno));
                LEAVE;
                return 1;
            }
            xpci[i].memreg = ptr;
            PRINT("mmap(): '%s': mapped to address: %p", xpci[i].name, ptr);
        }
    }

    nv2a_init();

    LEAVE;
    return 0;
}

void
xboxkrnl_destroy(void/* *x*/) {
    if (!xboxkrnl_thread_mutex) return;

    pthread_mutex_destroy(xboxkrnl_thread_mutex);
    xboxkrnl_thread_mutex = NULL;
    pthread_mutexattr_destroy(xboxkrnl_thread_mattr);
    xboxkrnl_thread_mattr = NULL;
}

int
xboxkrnl_init(void/* **x*/) {
    int ret = 1;

    if (xboxkrnl_thread_mutex) return ret;

    xboxkrnl_thread_mutex = calloc(1, sizeof(*xboxkrnl_thread_mutex));
    xboxkrnl_thread_mattr = calloc(1, sizeof(*xboxkrnl_thread_mattr));
    pthread_mutexattr_init(xboxkrnl_thread_mattr);
    pthread_mutexattr_settype(xboxkrnl_thread_mattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(xboxkrnl_thread_mutex, xboxkrnl_thread_mattr);

    ENTER;

    ret = xboxkrnl_init_hw();
    if (!ret) xboxkrnl_worker(NULL);

    VARDUMP(DUMP, ret);
    LEAVE;
    if (ret) xboxkrnl_destroy();
    return ret;
}

/* helpers */
#define XBOXKRNL_PATH_CDROM     "/Device/CdRom0"
#define XBOXKRNL_PATH_HARDDRIVE "/Device/Harddisk0/partition0"
#define XBOXKRNL_PATH_DRIVE_E   "/Device/Harddisk0/partition1"
#define XBOXKRNL_PATH_SYMLINK_D "/\?\?/D:"
#define XBOXKRNL_PATH_SYMLINK_T "/\?\?/T:"
#define XBOXKRNL_PATH_SYMLINK_U "/\?\?/U:"

static char *
xboxkrnl_path_winnt_to_unix(const char *path) {
    const char *prefix = NULL;
    char buf[4096];
    char *tmp;
    size_t len;
    size_t i;
    char *ret = NULL;

    tmp = strdup(path);
    len = strlen(path);

    for (i = 0; i < len; ++i) if (tmp[i] == '\\') tmp[i] = '/';

    do {

#define XBOXKRNL_PATH_PREFIX(x,y) \
        if (!strncasecmp(tmp, (x), (i = sizeof((x)) - 1))) { \
            prefix = (y); \
            break; \
        }

        XBOXKRNL_PATH_PREFIX(XBOXKRNL_PATH_CDROM, "D");

        XBOXKRNL_PATH_PREFIX(XBOXKRNL_PATH_HARDDRIVE, "");

        XBOXKRNL_PATH_PREFIX(XBOXKRNL_PATH_DRIVE_E, "E");

        XBOXKRNL_PATH_PREFIX(XBOXKRNL_PATH_SYMLINK_D, "D");

        XBOXKRNL_PATH_PREFIX(XBOXKRNL_PATH_SYMLINK_T, "T");

        XBOXKRNL_PATH_PREFIX(XBOXKRNL_PATH_SYMLINK_U, "U");

#undef XBOXKRNL_PATH_PREFIX

    } while (0);

    if (prefix) {
        len -= i;
        memmove(tmp, &tmp[i], len);
        tmp[len] = 0;
        i = sizeof(buf) - 1;
        buf[i] = 0;
        snprintf(buf, i, "%s%s", prefix, tmp);
        free(tmp);
        ret = strdup(buf);
    } else {
        ret = tmp;
    }

    return ret;
}

/* prototypes */
static STDCALL uint8_t
xboxkrnl_KeSetTimerEx( /* 150 (0x096) */
        /* in */       xboxkrnl_timer *Timer,
        /* in */       int64_t DueTime,
        /* in (opt) */ int32_t Period,
        /* in (opt) */ xboxkrnl_dpc *Dpc);
static STDCALL int
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
static STDCALL void
xboxkrnl_AvGetSavedDataAddress() { /* 001 (0x001) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_AvSendTVEncoderOption( /* 002 (0x002) */
        /* in */  void *RegisterBase,
        /* in */  uint32_t Option,
        /* in */  uint32_t Param,
        /* out */ uint32_t *Result) {
    ENTER;
    VARDUMP(VAR_IN,   RegisterBase);
    VARDUMP(VAR_IN,   Option);
    VARDUMP(VAR_IN,   Param);
    VARDUMP(VAR_OUT,  Result);
    VARDUMPD(VAR_OUT, Result);

    switch (Option) {
    case /* AV_QUERY_AV_CAPABILITIES */ 6:
        *Result  = XC_AV_PACK_HDTV;
        *Result |= xboxkrnl_os[XC_FACTORY_AV_REGION] << /* AV_STANDARD_BIT_SHIFT */ 8;
        *Result |= /* AV_FLAGS_60Hz */ 0x00400000;
        break;
    case /* AV_OPTION_FLICKER_FILTER */ 11:
    case /* AV_OPTION_ENABLE_LUMA_FILTER */ 14:
        break;
    default:
        INT3;
        break;
    }

    VARDUMPD(DUMP, Result);
    LEAVE;
}
static STDCALL void
xboxkrnl_AvSetDisplayMode() { /* 003 (0x003) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_AvSetSavedDataAddress() { /* 004 (0x004) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_DbgBreakPoint() { /* 005 (0x005) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_DbgBreakPointWithStatus() { /* 006 (0x006) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_DbgPrint() { /* 008 (0x008) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_DbgPrompt() { /* 010 (0x00a) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_ExAcquireReadWriteLockExclusive() { /* 012 (0x00c) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_ExAcquireReadWriteLockShared() { /* 013 (0x00d) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void *
xboxkrnl_ExAllocatePool( /* 014 (0x00e) */
        /* in */ size_t NumberOfBytes) {
    void *ret;
    ENTER;
    VARDUMP(VAR_IN, NumberOfBytes);

    if (!(ret = malloc(NumberOfBytes)) && NumberOfBytes) INT3;

    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
}
static STDCALL void
xboxkrnl_ExAllocatePoolWithTag() { /* 015 (0x00f) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_ExEventObjectType() { /* 016 (0x010) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
typedef struct _OBJECT_TYPE {
    OB_ALLOCATE_METHOD AllocateProcedure;
    OB_FREE_METHOD FreeProcedure;
    OB_CLOSE_METHOD CloseProcedure;
    OB_DELETE_METHOD DeleteProcedure;
    OB_PARSE_METHOD ParseProcedure;
    PVOID DefaultObject;
    ULONG PoolTag;
} OBJECT_TYPE, *POBJECT_TYPE;
#endif
static void *
xboxkrnl_ExEventObjectType = (void *)0xdeadbeef; /* 016 (0x010) */
#endif
static STDCALL void
xboxkrnl_ExFreePool( /* 017 (0x011) */
        /* in */ void *P) {
    ENTER;
    VARDUMP(VAR_IN, P);

    free(P);

    LEAVE;
}
static STDCALL void
xboxkrnl_ExInitializeReadWriteLock() { /* 018 (0x012) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_ExInterlockedAddLargeInteger() { /* 019 (0x013) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_ExInterlockedAddLargeStatistic() { /* 020 (0x014) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_ExInterlockedCompareExchange64() { /* 021 (0x015) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_ExMutantObjectType() { /* 022 (0x016) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
typedef struct _OBJECT_TYPE {
    OB_ALLOCATE_METHOD AllocateProcedure;
    OB_FREE_METHOD FreeProcedure;
    OB_CLOSE_METHOD CloseProcedure;
    OB_DELETE_METHOD DeleteProcedure;
    OB_PARSE_METHOD ParseProcedure;
    PVOID DefaultObject;
    ULONG PoolTag;
} OBJECT_TYPE, *POBJECT_TYPE;
#endif
static void *
xboxkrnl_ExMutantObjectType = (void *)0xdeadbeef; /* 022 (0x016) */
#endif
static STDCALL void
xboxkrnl_ExQueryPoolBlockSize() { /* 023 (0x017) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL int
xboxkrnl_ExQueryNonVolatileSetting( /* 024 (0x018) */
        /* in */  uint32_t ValueIndex,
        /* out */ uint32_t *Type,
        /* out */ void *Value,
        /* in */  uint32_t ValueLength,
        /* out */ uint32_t *ResultLength) {
    uint32_t val = 0;
    int ret = 0;
    ENTER;
    VARDUMP2(VAR_IN, ValueIndex, xboxkrnl_os_name);
    VARDUMP(VAR_OUT, Type);
    VARDUMP(VAR_OUT, Value);
    VARDUMP(VAR_IN,  ValueLength);
    VARDUMP(VAR_OUT, ResultLength);

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
        *Type = REG_DWORD;//FIXME
        val = xboxkrnl_os[ValueIndex];
        *(typeof(val) *)Value = val;
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
            ret = 1;
            INT3;
#endif
        }
        break;
    default:
        ret = 1;
        INT3;
        break;
    }
    switch (ValueIndex) {
    case XC_LANGUAGE:
        VARDUMP2(DUMP, val, xboxkrnl_xc_language_name);
        break;
    case XC_VIDEO_FLAGS:
        VARDUMP3(DUMP, val, xboxkrnl_xc_video_flags_name);
        break;
    case XC_AUDIO_FLAGS:
        VARDUMP2(DUMP, val, xboxkrnl_xc_audio_flags_name);
        break;
    case XC_PARENTAL_CONTROL_GAMES:
    case XC_PARENTAL_CONTROL_MOVIES:
        VARDUMP2(DUMP, val, xboxkrnl_xc_parental_control_name);
        break;
    case XC_MISC_FLAGS:
        VARDUMP3(DUMP, val, xboxkrnl_xc_misc_flags_name);
        break;
    case XC_FACTORY_AV_REGION:
        VARDUMP2(DUMP, val, xboxkrnl_xc_factory_av_region_name);
        break;
    case XC_FACTORY_GAME_REGION:
        VARDUMP3(DUMP, val, xboxkrnl_xc_factory_game_region_name);
        break;
    default:
        VARDUMP(DUMP, val);
        break;
    }

    LEAVE;
    return ret;
}
static STDCALL void
xboxkrnl_ExReadWriteRefurbInfo() { /* 025 (0x019) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_ExRaiseException() { /* 026 (0x01a) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_ExRaiseStatus() { /* 027 (0x01b) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_ExReleaseReadWriteLock() { /* 028 (0x01c) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_ExSaveNonVolatileSetting() { /* 029 (0x01d) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_ExSemaphoreObjectType() { /* 030 (0x01e) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
typedef struct _OBJECT_TYPE {
    OB_ALLOCATE_METHOD AllocateProcedure;
    OB_FREE_METHOD FreeProcedure;
    OB_CLOSE_METHOD CloseProcedure;
    OB_DELETE_METHOD DeleteProcedure;
    OB_PARSE_METHOD ParseProcedure;
    PVOID DefaultObject;
    ULONG PoolTag;
} OBJECT_TYPE, *POBJECT_TYPE;
#endif
static void *
xboxkrnl_ExSemaphoreObjectType = (void *)0xdeadbeef; /* 030 (0x01e) */
#endif
#if 0
static STDCALL void
xboxkrnl_ExTimerObjectType() { /* 031 (0x01f) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
typedef struct _OBJECT_TYPE {
    OB_ALLOCATE_METHOD AllocateProcedure;
    OB_FREE_METHOD FreeProcedure;
    OB_CLOSE_METHOD CloseProcedure;
    OB_DELETE_METHOD DeleteProcedure;
    OB_PARSE_METHOD ParseProcedure;
    PVOID DefaultObject;
    ULONG PoolTag;
} OBJECT_TYPE, *POBJECT_TYPE;
#endif
static void *
xboxkrnl_ExTimerObjectType = (void *)0xdeadbeef; /* 031 (0x01f) */
#endif
static STDCALL void
xboxkrnl_ExfInterlockedInsertHeadList() { /* 032 (0x020) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_ExfInterlockedInsertTailList() { /* 033 (0x021) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_ExfInterlockedRemoveHeadList() { /* 034 (0x022) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_FscGetCacheSize() { /* 035 (0x023) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_FscInvalidateIdleBlocks() { /* 036 (0x024) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_FscSetCacheSize() { /* 037 (0x025) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_HalBootSMCVideoMode() { /* 356 (0x164) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
static uint32_t
xboxkrnl_HalBootSMCVideoMode = 0; /* 356 (0x164) */
#endif
static FASTCALL void
xboxkrnl_HalClearSoftwareInterrupt() { /* 038 (0x026) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_HalDisableSystemInterrupt() { /* 039 (0x027) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_HalDiskCachePartitionCount() { /* 040 (0x028) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
static uint32_t
xboxkrnl_HalDiskCachePartitionCount = 0; /* 040 (0x028) */
#endif
#if 0
static STDCALL void
xboxkrnl_HalDiskModelNumber() { /* 041 (0x029) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
static xboxkrnl_string
xboxkrnl_HalDiskModelNumber = { }; /* 041 (0x029) */
#endif
#if 0
static STDCALL void
xboxkrnl_HalDiskSerialNumber() { /* 042 (0x02a) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
static xboxkrnl_string
xboxkrnl_HalDiskSerialNumber = { }; /* 042 (0x02a) */
#endif
static STDCALL void
xboxkrnl_HalEnableSecureTrayEject() { /* 365 (0x16d) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_HalEnableSystemInterrupt() { /* 043 (0x02b) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL uint32_t
xboxkrnl_HalGetInterruptVector( /* 044 (0x02c) */
        /* in */  uint32_t BusInterruptLevel,
        /* out */ uint8_t *Irql) {
    size_t i;
    ENTER;
    VARDUMP(VAR_IN,  BusInterruptLevel);
    VARDUMP(VAR_OUT, Irql);
    VARDUMP(VAR_OUT, *Irql);

    for (i = 0; xpci[i].name; ++i) {
        if (xpci[i].irq == BusInterruptLevel) {
            *Irql = xpci[i].irq;
            PRINT("name: '%s'", xpci[i].name);
            break;
        }
    }

    VARDUMP(DUMP, *Irql);
    LEAVE;
    return 0;
}
static STDCALL void
xboxkrnl_HalInitiateShutdown() { /* 360 (0x168) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_HalIsResetOrShutdownPending() { /* 358 (0x166) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_HalReadSMBusValue() { /* 045 (0x02d) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_HalReadSMCTrayState() { /* 009 (0x009) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_HalReadWritePCISpace( /* 046 (0x02e) */
        /* in */ uint32_t BusNumber,
        /* in */ uint32_t SlotNumber,
        /* in */ uint32_t RegisterNumber,
        /* in */ void *Buffer,
        /* in */ uint32_t Length,
        /* in */ uint8_t WritePCISpace) {
    ENTER;
    VARDUMP(VAR_IN, BusNumber);
    VARDUMP(VAR_IN, SlotNumber);
    VARDUMP(VAR_IN, RegisterNumber);
    VARDUMP(VAR_IN, Buffer);
    VARDUMP(VAR_IN, Length);
    VARDUMP(VAR_IN, WritePCISpace);

    //TODO interrupt mutex

    LEAVE;
}
static STDCALL void
xboxkrnl_HalRegisterShutdownNotification( /* 047 (0x02f) */
        /* in */ void *ShutdownRegistration,
        /* in */ int Register) {
    ENTER;
    VARDUMP(STUB,ShutdownRegistration);VARDUMP(STUB,Register);

    LEAVE;
}
static FASTCALL void
xboxkrnl_HalRequestSoftwareInterrupt() { /* 048 (0x030) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_HalReturnToFirmware() { /* 049 (0x031) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_HalWriteSMBusValue() { /* 050 (0x032) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_HalWriteSMCScratchRegister() { /* 366 (0x16e) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_IdexChannelObject() { /* 357 (0x165) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
typedef struct _IDE_CHANNEL_OBJECT {
    PIDE_INTERRUPT_ROUTINE InterruptRoutine;
    PIDE_FINISHIO_ROUTINE FinishIoRoutine;
    PIDE_POLL_RESET_COMPLETE_ROUTINE PollResetCompleteRoutine;
    PIDE_TIMEOUT_EXPIRED_ROUTINE TimeoutExpiredRoutine;
    PIDE_START_PACKET_ROUTINE StartPacketRoutine;
    PIDE_START_NEXT_PACKET_ROUTINE StartNextPacketRoutine;
    KIRQL InterruptIrql;
    BOOLEAN ExpectingBusMasterInterrupt;
    BOOLEAN StartPacketBusy;
    BOOLEAN StartPacketRequested;
    UCHAR Timeout;
    UCHAR IoRetries;
    UCHAR MaximumIoRetries;
    PIRP CurrentIrp;
    KDEVICE_QUEUE DeviceQueue;
    ULONG PhysicalRegionDescriptorTablePhysical;
    KDPC TimerDpc;
    KDPC FinishDpc;
    KTIMER Timer;
    KINTERRUPT InterruptObject;
} IDE_CHANNEL_OBJECT, *PIDE_CHANNEL_OBJECT;
#endif
static void *
xboxkrnl_IdexChannelObject = (void *)0xdeadbeef; /* 357 (0x165) */
#endif
static FASTCALL void
xboxkrnl_InterlockedCompareExchange() { /* 051 (0x033) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_InterlockedDecrement() { /* 052 (0x034) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_InterlockedIncrement() { /* 053 (0x035) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_InterlockedExchange() { /* 054 (0x036) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_InterlockedExchangeAdd() { /* 055 (0x037) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_InterlockedFlushSList() { /* 056 (0x038) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_InterlockedPopEntrySList() { /* 057 (0x039) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_InterlockedPushEntrySList() { /* 058 (0x03a) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoAllocateIrp() { /* 059 (0x03b) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoBuildAsynchronousFsdRequest() { /* 060 (0x03c) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoBuildDeviceIoControlRequest() { /* 061 (0x03d) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoBuildSynchronousFsdRequest() { /* 062 (0x03e) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoCheckShareAccess() { /* 063 (0x03f) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_IoCompletionObjectType() { /* 064 (0x040) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
typedef struct _OBJECT_TYPE {
    OB_ALLOCATE_METHOD AllocateProcedure;
    OB_FREE_METHOD FreeProcedure;
    OB_CLOSE_METHOD CloseProcedure;
    OB_DELETE_METHOD DeleteProcedure;
    OB_PARSE_METHOD ParseProcedure;
    PVOID DefaultObject;
    ULONG PoolTag;
} OBJECT_TYPE, *POBJECT_TYPE;
#endif
static void *
xboxkrnl_IoCompletionObjectType = (void *)0xdeadbeef; /* 064 (0x040) */
#endif
static STDCALL void
xboxkrnl_IoCreateDevice() { /* 065 (0x041) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoCreateFile() { /* 066 (0x042) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL int
xboxkrnl_IoCreateSymbolicLink( /* 067 (0x043) */
        /* in */ xboxkrnl_string *SymbolicLinkName,
        /* in */ xboxkrnl_string *DeviceName) {
    struct stat st;
    char *src;
    char *dest;
    int ret = -1;
    ENTER;
    VARDUMP(VAR_IN, SymbolicLinkName);
    VARDUMP(STRING, SymbolicLinkName->Buffer);
    VARDUMP(VAR_IN, DeviceName);
    VARDUMP(STRING, DeviceName->Buffer);

    src = xboxkrnl_path_winnt_to_unix(DeviceName->Buffer);
    VARDUMP(STRING, src);
    dest = xboxkrnl_path_winnt_to_unix(SymbolicLinkName->Buffer);
    VARDUMP(STRING, dest);

    if (strlen(src) == strlen(dest) && !strncmp(src, dest, strlen(src) + 1)) {
        ret = 0;
    } else {
        if (!lstat(dest, &st) && S_ISLNK(st.st_mode) && unlink(dest) < 0) {
            PRINT("error: unlink(): '%s'", strerror(errno));
            INT3;
        }
        if (symlink(src, dest) < 0) {
            PRINT("error: symlink(): '%s'", strerror(errno));
            INT3;
        } else {
            ret = 0;
        }
    }

    free(src);
    free(dest);

    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
}
static STDCALL void
xboxkrnl_IoDeleteDevice() { /* 068 (0x044) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoDeleteSymbolicLink() { /* 069 (0x045) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_IoDeviceObjectType() { /* 070 (0x046) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
typedef struct _OBJECT_TYPE {
    OB_ALLOCATE_METHOD AllocateProcedure;
    OB_FREE_METHOD FreeProcedure;
    OB_CLOSE_METHOD CloseProcedure;
    OB_DELETE_METHOD DeleteProcedure;
    OB_PARSE_METHOD ParseProcedure;
    PVOID DefaultObject;
    ULONG PoolTag;
} OBJECT_TYPE, *POBJECT_TYPE;
#endif
static void *
xboxkrnl_IoDeviceObjectType = (void *)0xdeadbeef; /* 070 (0x046) */
#endif
static STDCALL void
xboxkrnl_IoDismountVolume() { /* 090 (0x05a) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoDismountVolumeByName() { /* 091 (0x05b) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_IoFileObjectType() { /* 071 (0x047) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
typedef struct _OBJECT_TYPE {
    OB_ALLOCATE_METHOD AllocateProcedure;
    OB_FREE_METHOD FreeProcedure;
    OB_CLOSE_METHOD CloseProcedure;
    OB_DELETE_METHOD DeleteProcedure;
    OB_PARSE_METHOD ParseProcedure;
    PVOID DefaultObject;
    ULONG PoolTag;
} OBJECT_TYPE, *POBJECT_TYPE;
#endif
static void *
xboxkrnl_IoFileObjectType = (void *)0xdeadbeef; /* 071 (0x047) */
#endif
static STDCALL void
xboxkrnl_IoFreeIrp() { /* 072 (0x048) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoInitializeIrp() { /* 073 (0x049) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoInvalidDeviceRequest() { /* 074 (0x04a) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoMarkIrpMustComplete() { /* 359 (0x167) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoQueryFileInformation() { /* 075 (0x04b) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoQueryVolumeInformation() { /* 076 (0x04c) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoQueueThreadIrp() { /* 077 (0x04d) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoRemoveShareAccess() { /* 078 (0x04e) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoSetIoCompletion() { /* 079 (0x04f) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoSetShareAccess() { /* 080 (0x050) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoStartNextPacket() { /* 081 (0x051) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoStartNextPacketByKey() { /* 082 (0x052) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoStartPacket() { /* 083 (0x053) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoSynchronousDeviceIoControlRequest() { /* 084 (0x054) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_IoSynchronousFsdRequest() { /* 085 (0x055) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_IofCallDriver() { /* 086 (0x056) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_IofCompleteRequest() { /* 087 (0x057) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_KdDebuggerEnabled() { /* 088 (0x058) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
static uint8_t
xboxkrnl_KdDebuggerEnabled = 0; /* 088 (0x058) */
#endif
#if 0
static STDCALL void
xboxkrnl_KdDebuggerNotPresent() { /* 089 (0x059) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
static uint8_t
xboxkrnl_KdDebuggerNotPresent = 0; /* 089 (0x059) */
#endif
static STDCALL void
xboxkrnl_KeAlertResumeThread() { /* 092 (0x05c) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeAlertThread() { /* 093 (0x05d) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeBoostPriorityThread() { /* 094 (0x05e) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeBugCheck() { /* 095 (0x05f) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeBugCheckEx() { /* 096 (0x060) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeCancelTimer() { /* 097 (0x061) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL uint8_t
xboxkrnl_KeConnectInterrupt( /* 098 (0x062) */
        /* in */ void *Interrupt) {
    size_t i;
    uint8_t ret = 0;
    ENTER;
    VARDUMP(VAR_IN, Interrupt);

    for (i = 0; xpci[i].name; ++i) {
        if (xpci[i].irq_kinterrupt == Interrupt) {
            PRINT("name: '%s'", xpci[i].name);
            xpci[i].irq_connected = 1;
            ret = 1;
            break;
        }
    }

    VARDUMP(DUMP, ret);
    if (!ret) INT3;
    LEAVE;
    return ret;
}
static STDCALL int
xboxkrnl_KeDelayExecutionThread( /* 099 (0x063) */
        /* in */ char WaitMode,
        /* in */ int Alertable,
        /* in */ int64_t *Interval) {
    struct timespec tp;
    int64_t nsec;
    ENTER;
    VARDUMP(VAR_IN, WaitMode);
    VARDUMP(VAR_IN, Alertable);
    VARDUMP(VAR_IN, Interval);

    nsec = *Interval * 100;

    if (nsec < 0) {
        nsec = -nsec;
        tp.tv_sec = nsec / 1000000000;
        tp.tv_nsec = nsec % 1000000000;
        VARDUMP(DUMP, tp.tv_sec);
        VARDUMP(DUMP, tp.tv_nsec);
        nanosleep(&tp, NULL);
    } else if (nsec > 0) {
        INT3;
    }

    LEAVE;
    return 0;
}
static STDCALL void
xboxkrnl_KeDisconnectInterrupt() { /* 100 (0x064) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeEnterCriticalRegion() { /* 101 (0x065) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeGetCurrentIrql() { /* 103 (0x067) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeGetCurrentThread() { /* 104 (0x068) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeInitializeApc() { /* 105 (0x069) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeInitializeDeviceQueue() { /* 106 (0x06a) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeInitializeDpc( /* 107 (0x06b) */
        /* in */ xboxkrnl_dpc *Dpc,
        /* in */ void *DeferredRoutine,
        /* in */ void *DeferredContext) {
    ENTER;
    VARDUMP(VAR_IN, Dpc);
    VARDUMP(VAR_IN, DeferredRoutine);
    VARDUMP(VAR_IN, DeferredContext);

    memset(Dpc, 0, sizeof(*Dpc));

    Dpc->Type = /* DpcObject */ 19;
    Dpc->DeferredRoutine = DeferredRoutine;
    Dpc->DeferredContext = DeferredContext;

    LEAVE;
}
static STDCALL void
xboxkrnl_KeInitializeEvent() { /* 108 (0x06c) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeInitializeInterrupt( /* 109 (0x06d) */
        /* in */ void *Interrupt,
        /* in */ void *ServiceRoutine,
        /* in */ void *ServiceContext,
        /* in */ uint32_t Vector,
        /* in */ uint8_t Irql,
        /* in */ int32_t InterruptMode,
        /* in */ int32_t ShareVector) {
    size_t i;
    ENTER;
    VARDUMP(VAR_IN, Interrupt);
    VARDUMP(VAR_IN, ServiceRoutine);
    VARDUMP(VAR_IN, ServiceContext);
    VARDUMP(VAR_IN, Vector);
    VARDUMP(VAR_IN, Irql);
    VARDUMP(VAR_IN, InterruptMode);
    VARDUMP(VAR_IN, ShareVector);

    for (i = 0; xpci[i].name; ++i) {
        if (xpci[i].irq == Irql) {
            PRINT("name: '%s'", xpci[i].name);
            xpci[i].irq_kinterrupt  = Interrupt;
            xpci[i].irq_connected   = 0;
            xpci[i].irq_level       = 0;
            xpci[i].irq_isr_routine = ServiceRoutine;
            xpci[i].irq_isr_context = ServiceContext;
            break;
        }
    }

    LEAVE;
}
static STDCALL void
xboxkrnl_KeInitializeMutant() { /* 110 (0x06e) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeInitializeQueue() { /* 111 (0x06f) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeInitializeSemaphore() { /* 112 (0x070) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeInitializeTimerEx( /* 113 (0x071) */
        /* in */ xboxkrnl_timer *Timer,
        /* in */ int Type) {
    ENTER;
    VARDUMP(VAR_IN, Timer);
    VARDUMP(VAR_IN, Type);

    memset(Timer, 0, sizeof(*Timer));

    Timer->Header.Type = /* TimerNotificationObject */ 8 + Type;
    Timer->Header.Size = sizeof(*Timer) / 4;
    XBOXKRNL_LISTENTRY_INIT(&Timer->Header.WaitListHead);

    LEAVE;
}
static STDCALL void
xboxkrnl_KeInsertByKeyDeviceQueue() { /* 114 (0x072) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeInsertDeviceQueue() { /* 115 (0x073) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeInsertHeadQueue() { /* 116 (0x074) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeInsertQueue() { /* 117 (0x075) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeInsertQueueApc() { /* 118 (0x076) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeInsertQueueDpc() { /* 119 (0x077) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_KeInterruptTime() { /* 120 (0x078) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
typedef struct _KSYSTEM_TIME {
    ULONG LowPart;
    LONG High1Time;
    LONG High2Time;
} KSYSTEM_TIME, *PKSYSTEM_TIME;
#endif
static void *
xboxkrnl_KeInterruptTime = (void *)0xdeadbeef; /* 120 (0x078) */
#endif
static STDCALL void
xboxkrnl_KeIsExecutingDpc() { /* 121 (0x079) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeLeaveCriticalRegion() { /* 122 (0x07a) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KePulseEvent() { /* 123 (0x07b) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeQueryBasePriorityThread() { /* 124 (0x07c) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeQueryInterruptTime() { /* 125 (0x07d) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeQueryPerformanceCounter() { /* 126 (0x07e) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeQueryPerformanceFrequency() { /* 127 (0x07f) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeQuerySystemTime() { /* 128 (0x080) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL uint8_t
xboxkrnl_KeRaiseIrqlToDpcLevel(void) { /* 129 (0x081) */
    ENTER;

    LEAVE;
    return 0;
}
static STDCALL void
xboxkrnl_KeRaiseIrqlToSynchLevel() { /* 130 (0x082) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeReleaseMutant() { /* 131 (0x083) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeReleaseSemaphore() { /* 132 (0x084) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeRemoveByKeyDeviceQueue() { /* 133 (0x085) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeRemoveDeviceQueue() { /* 134 (0x086) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeRemoveEntryDeviceQueue() { /* 135 (0x087) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeRemoveQueue() { /* 136 (0x088) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeRemoveQueueDpc() { /* 137 (0x089) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeResetEvent() { /* 138 (0x08a) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeRestoreFloatingPointState() { /* 139 (0x08b) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeResumeThread() { /* 140 (0x08c) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeRundownQueue() { /* 141 (0x08d) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeSaveFloatingPointState() { /* 142 (0x08e) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeSetBasePriorityThread() { /* 143 (0x08f) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeSetDisableBoostThread() { /* 144 (0x090) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeSetEvent() { /* 145 (0x091) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeSetEventBoostPriority() { /* 146 (0x092) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeSetPriorityProcess() { /* 147 (0x093) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeSetPriorityThread() { /* 148 (0x094) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL uint8_t
xboxkrnl_KeSetTimer( /* 149 (0x095) */
        /* in */       xboxkrnl_timer *Timer,
        /* in */       int64_t DueTime,
        /* in (opt) */ xboxkrnl_dpc *Dpc) {
    return xboxkrnl_KeSetTimerEx(Timer, DueTime, 0, Dpc);
}
static STDCALL uint8_t
xboxkrnl_KeSetTimerEx( /* 150 (0x096) */
        /* in */       xboxkrnl_timer *Timer,
        /* in */       int64_t DueTime,
        /* in (opt) */ int32_t Period,
        /* in (opt) */ xboxkrnl_dpc *Dpc) {
    int ret = 0;
    ENTER;
    VARDUMP(VAR_IN, Timer);
    VARDUMP(VAR_IN, DueTime);
    VARDUMP(VAR_IN, Period);
    VARDUMP(VAR_IN, Dpc);

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
static STDCALL void
xboxkrnl_KeStallExecutionProcessor( /* 151 (0x097) */
        /* in */ uint32_t MicroSeconds) {
    ENTER;
    VARDUMP(VAR_IN, MicroSeconds);

    usleep(MicroSeconds);

    LEAVE;
}
static STDCALL void
xboxkrnl_KeSuspendThread() { /* 152 (0x098) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeSynchronizeExecution() { /* 153 (0x099) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_KeSystemTime() { /* 154 (0x09a) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
typedef struct _KSYSTEM_TIME {
    ULONG LowPart;
    LONG High1Time;
    LONG High2Time;
} KSYSTEM_TIME, *PKSYSTEM_TIME;
#endif
static void *
xboxkrnl_KeSystemTime = (void *)0xdeadbeef; /* 154 (0x09a) */
#endif
static STDCALL void
xboxkrnl_KeTestAlertThread() { /* 155 (0x09b) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_KeTickCount() { /* 156 (0x09c) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
static uint32_t
xboxkrnl_KeTickCount = 0; /* 156 (0x09c) */
#endif
#if 0
static STDCALL void
xboxkrnl_KeTimeIncrement() { /* 157 (0x09d) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
static uint32_t
xboxkrnl_KeTimeIncrement = 0; /* 157 (0x09d) */
#endif
static STDCALL void
xboxkrnl_KeWaitForMultipleObjects() { /* 158 (0x09e) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_KeWaitForSingleObject() { /* 159 (0x09f) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_KfRaiseIrql() { /* 160 (0x0a0) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_KfLowerIrql( /* 161 (0x0a1) */
        /* in */ uint8_t NewIrql) {
    ENTER;
    VARDUMP(VAR_IN, NewIrql);

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_KiBugCheckData() { /* 162 (0x0a2) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
static uint32_t *
xboxkrnl_KiBugCheckData[5] = { }; /* 162 (0x0a2) */
#endif
static FASTCALL void
xboxkrnl_KiUnlockDispatcherDatabase() { /* 163 (0x0a3) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_LaunchDataPage() { /* 164 (0x0a4) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
#define MAX_LAUNCH_DATA_SIZE 3072

#define LDT_LAUNCH_DASHBOARD 1
#define LDT_NONE             0xFFFFFFFF

#define LDF_HAS_BEEN_READ    0x00000001

typedef struct _LAUNCH_DATA_HEADER
{
    ULONG dwLaunchDataType;
    ULONG dwTitleId;
    CHAR  szLaunchPath[520];
    ULONG dwFlags;
} LAUNCH_DATA_HEADER, *PLAUNCH_DATA_HEADER;

typedef struct _LAUNCH_DATA_PAGE
{
    LAUNCH_DATA_HEADER Header;
    UCHAR Pad[PAGE_SIZE - MAX_LAUNCH_DATA_SIZE - sizeof(LAUNCH_DATA_HEADER)];
    UCHAR LaunchData[MAX_LAUNCH_DATA_SIZE];
} LAUNCH_DATA_PAGE, *PLAUNCH_DATA_PAGE;
#endif
static void *
xboxkrnl_LaunchDataPage = (void *)0xdeadbeef; /* 164 (0x0a4) */
#endif
static STDCALL void *
xboxkrnl_MmAllocateContiguousMemory( /* 165 (0x0a5) */
        /* in */ size_t NumberOfBytes) {
    size_t tmp;
    void *ret;
    ENTER;
    VARDUMP(VAR_IN, NumberOfBytes);

    VARDUMP(DUMP, xboxkrnl_mem_contiguous);

    if ((ret = mmap(xboxkrnl_mem_contiguous,
            NumberOfBytes,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
            -1,
            0)) == MAP_FAILED) INT3;

    if ((tmp = NumberOfBytes & PAGEMASK)) tmp = NumberOfBytes + PAGESIZE - tmp;
    else tmp = NumberOfBytes;

    xboxkrnl_mem_contiguous += tmp;

    VARDUMP(DUMP, xboxkrnl_mem_contiguous);
    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
}
static STDCALL void *
xboxkrnl_MmAllocateContiguousMemoryEx( /* 166 (0x0a6) */
        /* in */ size_t NumberOfBytes,
        /* in */ uint32_t LowestAcceptableAddress,
        /* in */ uint32_t HighestAcceptableAddress,
        /* in */ uint32_t Alignment,
        /* in */ uint32_t Protect) {
    size_t tmp;
    void *ret;
    ENTER;
    VARDUMP(VAR_IN, NumberOfBytes);
    VARDUMP(VAR_IN, LowestAcceptableAddress);
    VARDUMP(VAR_IN, HighestAcceptableAddress);
    VARDUMP(VAR_IN, Alignment);
    VARDUMP(VAR_IN, Protect);

    VARDUMP(DUMP, xboxkrnl_mem_contiguous);

    if ((ret = mmap(xboxkrnl_mem_contiguous,
            NumberOfBytes,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
            -1,
            0)) == MAP_FAILED) INT3;

    if ((tmp = NumberOfBytes & PAGEMASK)) tmp = NumberOfBytes + PAGESIZE - tmp;
    else tmp = NumberOfBytes;

    xboxkrnl_mem_contiguous += tmp;

    VARDUMP(DUMP, xboxkrnl_mem_contiguous);
    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
}
static STDCALL void
xboxkrnl_MmAllocateSystemMemory() { /* 167 (0x0a7) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void *
xboxkrnl_MmClaimGpuInstanceMemory( /* 168 (0x0a8) */
        /* in */  size_t NumberOfBytes,
        /* out */ size_t *NumberOfPaddingBytes) {
    void *ret = NULL;
    size_t tmp;
    ENTER;
    VARDUMP(VAR_IN,  NumberOfBytes);
    VARDUMP(VAR_OUT, NumberOfPaddingBytes);
    VARDUMP(VAR_OUT, *NumberOfPaddingBytes);

    if (NumberOfBytes == ~0U) {
        ret = xboxkrnl_gpuinstmem + xboxkrnl_gpuinstsz;
    } else {
        if (xboxkrnl_gpuinstmem) munmap(xboxkrnl_gpuinstmem, xboxkrnl_gpuinstsz);
        if ((tmp = NumberOfBytes & PAGEMASK)) NumberOfBytes += PAGESIZE - tmp;
        if ((ret = mmap(NULL,
                NumberOfBytes,
                0,//PROT_READ | PROT_WRITE,//TODO
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
                -1,
                0)) == MAP_FAILED) INT3;
        xboxkrnl_gpuinstmem = ret;
        xboxkrnl_gpuinstsz = NumberOfBytes;
        ret += NumberOfBytes;
    }

    *NumberOfPaddingBytes = 0;

    VARDUMP(DUMP, xboxkrnl_gpuinstmem);
    VARDUMP(DUMP, xboxkrnl_gpuinstsz);
    VARDUMP(DUMP, NumberOfBytes);
    VARDUMP(DUMP, *NumberOfPaddingBytes);
    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
}
static STDCALL void
xboxkrnl_MmCreateKernelStack() { /* 169 (0x0a9) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_MmDeleteKernelStack() { /* 170 (0x0aa) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_MmFreeContiguousMemory() { /* 171 (0x0ab) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_MmFreeSystemMemory() { /* 172 (0x0ac) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void *
xboxkrnl_MmGetPhysicalAddress( /* 173 (0x0ad) */
        /* in */ void *BaseAddress) {
    ENTER;
    VARDUMP(VAR_IN, BaseAddress);

    LEAVE;
    return BaseAddress;
}
#if 0
static STDCALL void
xboxkrnl_MmGlobalData() { /* 102 (0x066) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
typedef struct _MMGLOBALDATA {
    PMMPFNREGION RetailPfnRegion;
    PMMPTERANGE SystemPteRange;
    PULONG AvailablePages;
    PFN_COUNT *AllocatedPagesByUsage;
    PRTL_CRITICAL_SECTION AddressSpaceLock;
    PMMADDRESS_NODE *VadRoot;
    PMMADDRESS_NODE *VadHint;
    PMMADDRESS_NODE *VadFreeHint;
} MMGLOBALDATA, *PMMGLOBALDATA;
#endif
static void *
xboxkrnl_MmGlobalData = (void *)0xdeadbeef; /* 102 (0x066) */
#endif
static STDCALL void
xboxkrnl_MmIsAddressValid() { /* 174 (0x0ae) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_MmLockUnlockBufferPages( /* 175 (0x0af) */
        /* in */ void *BaseAddress,
        /* in */ size_t NumberOfBytes,
        /* in */ int UnlockPages) {
    ENTER;
    VARDUMP(STUB,BaseAddress);VARDUMP(STUB,NumberOfBytes);VARDUMP(STUB,UnlockPages);

    LEAVE;
}
static STDCALL void
xboxkrnl_MmLockUnlockPhysicalPage() { /* 176 (0x0b0) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_MmMapIoSpace() { /* 177 (0x0b1) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_MmPersistContiguousMemory() { /* 178 (0x0b2) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_MmQueryAddressProtect() { /* 179 (0x0b3) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_MmQueryAllocationSize() { /* 180 (0x0b4) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_MmQueryStatistics() { /* 181 (0x0b5) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_MmSetAddressProtect() { /* 182 (0x0b6) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_MmUnmapIoSpace() { /* 183 (0x0b7) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL int
xboxkrnl_NtAllocateVirtualMemory( /* 184 (0x0b8) */
        /* i/o */      void **BaseAddress,
        /* in (opt) */ uint32_t *ZeroBits,
        /* i/o */      size_t *RegionSize,
        /* in */       uint32_t AllocationType,
        /* in */       uint32_t Protect) {
    int flag = 0;
    int ret = -1;
    ENTER;
    VARDUMP(VAR_IN,  BaseAddress);
    VARDUMP(VAR_OUT, *BaseAddress);
    VARDUMP(VAR_IN,  ZeroBits);
    VARDUMPD(VAR_IN, ZeroBits);
    VARDUMP(VAR_IN,  RegionSize);
    VARDUMP(VAR_OUT, *RegionSize);
    VARDUMP(VAR_IN,  AllocationType);
    VARDUMP(VAR_IN,  Protect);

    if (AllocationType & /* MEM_RESERVE */ 0x2000) {
        if (Protect & /* PAGE_NOCACHE */ 0x200) flag |= MAP_NORESERVE;
        if ((*BaseAddress = mmap(*BaseAddress,
                *RegionSize,
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS | flag,
                -1,
                0)) == MAP_FAILED) {
            *BaseAddress = NULL;
        } else {
            ret = 0;
        }
    } else if (AllocationType & /* MEM_COMMIT */ 0x1000) {
        ret = 0;
    }

    if (ret) INT3;

    VARDUMP(DUMP, *BaseAddress);
    VARDUMP(DUMP, *RegionSize);
    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
}
static STDCALL void
xboxkrnl_NtCancelTimer() { /* 185 (0x0b9) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtClearEvent() { /* 186 (0x0ba) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL int
xboxkrnl_NtClose( /* 187 (0x0bb) */
        /* in */ xboxkrnl_file *Handle) {
    ENTER;
    VARDUMP(VAR_IN, Handle);

    if (Handle) {
        VARDUMP(VAR_IN, Handle->fd);
        VARDUMP(VAR_IN, Handle->dir);
        if (Handle->fd >= 0) close(Handle->fd);
        if (Handle->dir) closedir(Handle->dir);
        free(Handle);
    }

    LEAVE;
    return 0;
}
static STDCALL void
xboxkrnl_NtCreateDirectoryObject() { /* 188 (0x0bc) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtCreateEvent() { /* 189 (0x0bd) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL int
xboxkrnl_NtCreateFile( /* 190 (0x0be) */
        /* out */      xboxkrnl_file **FileHandle,
        /* in */       uint32_t DesiredAccess,
        /* in */       xboxkrnl_objattr *ObjectAttributes,
        /* out */      xboxkrnl_iostatus *IoStatusBlock,
        /* in (opt) */ int64_t *AllocationSize,
        /* in */       uint32_t FileAttributes,
        /* in */       uint32_t ShareAccess,
        /* in */       uint32_t CreateDisposition,
        /* in */       uint32_t CreateOptions) {
    xboxkrnl_file f;
    struct stat st;
    char *path;
//    int ci = 0;
    int tmp;
    int ret = -1;
    ENTER;
    VARDUMP(VAR_OUT, FileHandle);
    VARDUMP3(VAR_IN, DesiredAccess, xboxkrnl_file_access_mask_name);
    VARDUMP(VAR_IN,  ObjectAttributes);
    VARDUMP(VAR_IN,  ObjectAttributes->RootDirectory);
    VARDUMP(VAR_IN,  ObjectAttributes->ObjectName);
    VARDUMP(STRING,  ObjectAttributes->ObjectName->Buffer);
    VARDUMP(VAR_IN,  ObjectAttributes->Attributes);
    VARDUMP(VAR_OUT, IoStatusBlock);
    VARDUMP(VAR_OUT, IoStatusBlock->Status);
    VARDUMP(VAR_OUT, IoStatusBlock->Information);
    VARDUMP(VAR_IN,  AllocationSize);
    VARDUMP(VAR_IN,  FileAttributes);
    VARDUMP(VAR_IN,  ShareAccess);
    VARDUMP2(VAR_IN, CreateDisposition, xboxkrnl_file_create_disposition_name);
    VARDUMP3(VAR_IN, CreateOptions, xboxkrnl_file_open_create_name);

    memset(&f, 0, sizeof(f));
    f.fd = -1;

    path = xboxkrnl_path_winnt_to_unix(ObjectAttributes->ObjectName->Buffer);
    VARDUMP(STRING, path);
    tmp = sizeof(f.path) - 1;
    f.path[tmp] = 0;
    strncpy(f.path, path, tmp);
    free(path);
    path = f.path;
#if 0
    ci = !!(ObjectAttributes->Attributes & /* OBJ_CASE_INSENSITIVE */ 0x40);

#if 0
CreateDisposition  Action if file exists             Action if file does not exist
FILE_SUPERSEDE     Replace the file.                 Create the file.
FILE_CREATE        Return an error.                  Create the file.
FILE_OPEN          Open the file.                    Return an error.
FILE_OPEN_IF       Open the file.                    Create the file.
FILE_OVERWRITE     Open the file, and overwrite it.  Return an error.
FILE_OVERWRITE_IF  Open the file, and overwrite it.  Create the file.
#endif

enum XBOXKRNL_FILE_CREATE_DISPOSITION {
    XBOXKRNL_FILE_SUPERSEDE                 =  0,
    XBOXKRNL_FILE_OPEN,                     /* 1 */
    XBOXKRNL_FILE_CREATE,                   /* 2 */
    XBOXKRNL_FILE_OPEN_IF,                  /* 3 */
    XBOXKRNL_FILE_OVERWRITE,                /* 4 */
    XBOXKRNL_FILE_OVERWRITE_IF              /* 5 */
};
#endif
    IoStatusBlock->Information = XBOXKRNL_FILE_DOES_NOT_EXIST;

    do {
        if ((tmp = stat(path, &st)) < 0 &&
            (CreateDisposition == XBOXKRNL_FILE_OPEN ||
            CreateDisposition == XBOXKRNL_FILE_OVERWRITE ||
            errno != ENOENT)) {
            PRINT("error: stat(): '%s'", strerror(errno));
            break;
        }
        if (!tmp &&
            CreateDisposition == XBOXKRNL_FILE_CREATE) {
            IoStatusBlock->Information = XBOXKRNL_FILE_EXISTS;
            PRINT("error: file exists", 0);
            break;
        }
        if (!tmp &&
            CreateOptions & XBOXKRNL_FILE_DIRECTORY_FILE &&
            !S_ISDIR(st.st_mode)) {
            PRINT("error: not a directory", 0);
            break;
        }
        if (tmp &&
            CreateOptions & XBOXKRNL_FILE_DIRECTORY_FILE &&
            mkdir(path, 0755) < 0) {
            PRINT("error: mkdir(): '%s'", strerror(errno));
            break;
        }
        if (CreateOptions & XBOXKRNL_FILE_DIRECTORY_FILE &&
            !(f.dir = opendir(path))) {
            PRINT("error: opendir(): '%s'", strerror(errno));
            break;
        } else if (!(CreateOptions & XBOXKRNL_FILE_DIRECTORY_FILE)) {
            tmp = O_RDWR | O_CREAT;
            switch (CreateDisposition) {
            case XBOXKRNL_FILE_OVERWRITE:
            case XBOXKRNL_FILE_OVERWRITE_IF:
                tmp |= O_TRUNC;
                break;
            }
            if ((f.fd = open(path, tmp, 0644)) < 0) {
                PRINT("error: open(): '%s'", strerror(errno));
                break;
            }
        }
    } while ((ret = 0));

    if (!(IoStatusBlock->Status = ret)) {
        switch (CreateDisposition) {
        case XBOXKRNL_FILE_SUPERSEDE:
            IoStatusBlock->Information = XBOXKRNL_FILE_SUPERSEDED;
            break;
        case XBOXKRNL_FILE_OPEN:
        case XBOXKRNL_FILE_OPEN_IF:
            IoStatusBlock->Information = XBOXKRNL_FILE_OPENED;
            break;
        case XBOXKRNL_FILE_CREATE:
            IoStatusBlock->Information = XBOXKRNL_FILE_CREATED;
            break;
        case XBOXKRNL_FILE_OVERWRITE:
        case XBOXKRNL_FILE_OVERWRITE_IF:
            IoStatusBlock->Information = XBOXKRNL_FILE_OVERWRITTEN;
            break;
        }
        if ((*FileHandle = malloc(sizeof(**FileHandle)))) {
            **FileHandle = f;
            VARDUMP(DUMP, *FileHandle);
            VARDUMP(DUMP, (*FileHandle)->fd);
            VARDUMP(DUMP, (*FileHandle)->dir);
        } else {
            INT3;
        }
    }

    VARDUMP(DUMP,  IoStatusBlock->Status);
    VARDUMP2(DUMP, IoStatusBlock->Information, xboxkrnl_file_io_status_name);
    VARDUMP(DUMP,  ret);
    LEAVE;
    return ret;
}
static STDCALL void
xboxkrnl_NtCreateIoCompletion() { /* 191 (0x0bf) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtCreateMutant() { /* 192 (0x0c0) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtCreateSemaphore() { /* 193 (0x0c1) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtCreateTimer() { /* 194 (0x0c2) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtDeleteFile() { /* 195 (0x0c3) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtDeviceIoControlFile() { /* 196 (0x0c4) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtDuplicateObject() { /* 197 (0x0c5) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtFlushBuffersFile() { /* 198 (0x0c6) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtFreeVirtualMemory() { /* 199 (0x0c7) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtFsControlFile() { /* 200 (0x0c8) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtOpenDirectoryObject() { /* 201 (0x0c9) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL int
xboxkrnl_NtOpenFile( /* 202 (0x0ca) */
        /* out */ xboxkrnl_file **FileHandle,
        /* in */  uint32_t DesiredAccess,
        /* in */  xboxkrnl_objattr *ObjectAttributes,
        /* out */ xboxkrnl_iostatus *IoStatusBlock,
        /* in */  uint32_t ShareAccess,
        /* in */  uint32_t OpenOptions) {
    char *path;
    int ret = -1;
    ENTER;
    VARDUMP(VAR_OUT, FileHandle);
    VARDUMP3(VAR_IN, DesiredAccess, xboxkrnl_file_access_mask_name);
    VARDUMP(VAR_IN,  ObjectAttributes);
    VARDUMP(VAR_IN,  ObjectAttributes->RootDirectory);
    VARDUMP(VAR_IN,  ObjectAttributes->ObjectName);
    VARDUMP(STRING,  ObjectAttributes->ObjectName->Buffer);
    VARDUMP(VAR_IN,  ObjectAttributes->Attributes);
    VARDUMP(VAR_OUT, IoStatusBlock);
    VARDUMP(VAR_OUT, IoStatusBlock->Status);
    VARDUMP(VAR_OUT, IoStatusBlock->Information);
    VARDUMP(VAR_IN,  ShareAccess);
    VARDUMP3(VAR_IN, OpenOptions, xboxkrnl_file_open_create_name);

    path = xboxkrnl_path_winnt_to_unix(ObjectAttributes->ObjectName->Buffer);
    VARDUMP(STRING, path);

    if (OpenOptions & XBOXKRNL_FILE_OPEN_FOR_FREE_SPACE_QUERY /* 0x800000 */) {
        *FileHandle = NULL;
        VARDUMP(DUMP, *FileHandle);
        ret = 0;
    }

    free(path);

    if (!(IoStatusBlock->Status = ret)) IoStatusBlock->Information = XBOXKRNL_FILE_OPENED;
    else INT3;

    VARDUMP(DUMP,  IoStatusBlock->Status);
    VARDUMP2(DUMP, IoStatusBlock->Information, xboxkrnl_file_io_status_name);
    VARDUMP(DUMP,  ret);
    LEAVE;
    return ret;
}
static STDCALL void
xboxkrnl_NtOpenSymbolicLinkObject() { /* 203 (0x0cb) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtProtectVirtualMemory() { /* 204 (0x0cc) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtPulseEvent() { /* 205 (0x0cd) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtQueueApcThread() { /* 206 (0x0ce) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtQueryDirectoryFile() { /* 207 (0x0cf) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtQueryDirectoryObject() { /* 208 (0x0d0) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtQueryEvent() { /* 209 (0x0d1) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtQueryFullAttributesFile() { /* 210 (0x0d2) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL int
xboxkrnl_NtQueryInformationFile( /* 211 (0x0d3) */
        /* in */  xboxkrnl_file *FileHandle,
        /* out */ xboxkrnl_iostatus *IoStatusBlock,
        /* out */ void *FileInformation,
        /* in */  uint32_t Length,
        /* in */  int FileInformationClass) {
    struct {
        int64_t     CreationTime;
        int64_t     LastAccessTime;
        int64_t     LastWriteTime;
        int64_t     ChangeTime;
        int64_t     AllocationSize;
        int64_t     EndOfFile;
        uint32_t    FileAttributes;
    } PACKED *FileNetworkOpenInformation;//FIXME
    struct stat st;
    int ret = -1;
    ENTER;
    VARDUMP(VAR_IN,  FileHandle);
    VARDUMP(VAR_OUT, IoStatusBlock);
    VARDUMP(VAR_OUT, IoStatusBlock->Status);
    VARDUMP(VAR_OUT, IoStatusBlock->Information);
    VARDUMP(VAR_OUT, FileInformation);
    VARDUMP(VAR_IN,  Length);
    VARDUMP(VAR_IN,  FileInformationClass);

    IoStatusBlock->Information = 0;

    do {
        if (FileInformationClass == /* FileNetworkOpenInformation */ 34) {
            FileNetworkOpenInformation = FileInformation;
            if (fstat(FileHandle->fd, &st) < 0) {
                PRINT("error: fstat(): '%s'", strerror(errno));
                break;
            }
            FileNetworkOpenInformation->CreationTime    = st.st_ctime;
            FileNetworkOpenInformation->LastAccessTime  = st.st_atime;
            FileNetworkOpenInformation->LastWriteTime   = st.st_mtime;
            FileNetworkOpenInformation->ChangeTime      = st.st_mtime;
            FileNetworkOpenInformation->AllocationSize  = st.st_size;
            FileNetworkOpenInformation->EndOfFile       = st.st_size;
            FileNetworkOpenInformation->FileAttributes  = 0;
            VARDUMP(DUMP, FileNetworkOpenInformation->CreationTime);
            VARDUMP(DUMP, FileNetworkOpenInformation->LastAccessTime);
            VARDUMP(DUMP, FileNetworkOpenInformation->LastWriteTime);
            VARDUMP(DUMP, FileNetworkOpenInformation->ChangeTime);
            VARDUMP(DUMP, FileNetworkOpenInformation->AllocationSize);
            VARDUMP(DUMP, FileNetworkOpenInformation->EndOfFile);
            VARDUMP(DUMP, FileNetworkOpenInformation->FileAttributes);
            IoStatusBlock->Information = sizeof(*FileNetworkOpenInformation);
            ret = 0;
            break;
        }
    } while (0);

    if ((IoStatusBlock->Status = ret)) INT3;

    VARDUMP(DUMP, IoStatusBlock->Status);
    VARDUMP(DUMP, IoStatusBlock->Information);
    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
}
static STDCALL void
xboxkrnl_NtQueryIoCompletion() { /* 212 (0x0d4) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtQueryMutant() { /* 213 (0x0d5) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtQuerySemaphore() { /* 214 (0x0d6) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtQuerySymbolicLinkObject() { /* 215 (0x0d7) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtQueryTimer() { /* 216 (0x0d8) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL int
xboxkrnl_NtQueryVirtualMemory( /* 217 (0x0d9) */
        /* in */  void *BaseAddress,
        /* out */ xboxkrnl_meminfo *MemoryInformation) {
    int ret = -1;
    ENTER;
    VARDUMP(VAR_IN,  BaseAddress);
    VARDUMP(VAR_OUT, MemoryInformation);

#if 0
typedef struct {
    void *                  BaseAddress;
    void *                  AllocationBase;
    int32_t                 AllocationProtect;
    size_t                  RegionSize;
    int32_t                 State;
    int32_t                 Protect;
    int32_t                 Type;
} PACKED xboxkrnl_meminfo;
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
static STDCALL int
xboxkrnl_NtQueryVolumeInformationFile( /* 218 (0x0da) */
        /* in */  void *FileHandle,
        /* out */ xboxkrnl_iostatus *IoStatusBlock,
        /* out */ void *FsInformation,
        /* in */  uint32_t Length,
        /* in */  int FsInformationClass) {
    struct {
        int64_t     TotalAllocationUnits;
        int64_t     AvailableAllocationUnits;
        uint32_t    SectorsPerAllocationUnit;
        uint32_t    BytesPerSector;
    } PACKED *FileFsSizeInformation;//FIXME
    size_t sz;
    int ret = -1;
    ENTER;
    VARDUMP(VAR_IN,  FileHandle);
    VARDUMP(VAR_OUT, IoStatusBlock);
    VARDUMP(VAR_OUT, IoStatusBlock->Status);
    VARDUMP(VAR_OUT, IoStatusBlock->Information);
    VARDUMP(VAR_OUT, FsInformation);
    VARDUMP(VAR_IN,  Length);
    VARDUMP(VAR_IN,  FsInformationClass);

    IoStatusBlock->Information = 0;

    if (FsInformationClass == /* FileFsSizeInformation */ 3) {
        FileFsSizeInformation = FsInformation;
        FileFsSizeInformation->SectorsPerAllocationUnit = 32;
        FileFsSizeInformation->BytesPerSector = 512;
        VARDUMP(DUMP, FileFsSizeInformation->SectorsPerAllocationUnit);
        VARDUMP(DUMP, FileFsSizeInformation->BytesPerSector);
        sz = sizeof(*FileFsSizeInformation);
        ret = 0;
    }

    if (!(IoStatusBlock->Status = ret)) IoStatusBlock->Information = sz;
    else INT3;

    VARDUMP(DUMP, IoStatusBlock->Status);
    VARDUMP(DUMP, IoStatusBlock->Information);
    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
}
static STDCALL void
xboxkrnl_NtReadFile() { /* 219 (0x0db) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtReadFileScatter() { /* 220 (0x0dc) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtReleaseMutant() { /* 221 (0x0dd) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtReleaseSemaphore() { /* 222 (0x0de) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtRemoveIoCompletion() { /* 223 (0x0df) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtResumeThread() { /* 224 (0x0e0) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtSetEvent() { /* 225 (0x0e1) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtSetInformationFile() { /* 226 (0x0e2) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtSetIoCompletion() { /* 227 (0x0e3) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtSetSystemTime() { /* 228 (0x0e4) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtSetTimerEx() { /* 229 (0x0e5) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtSignalAndWaitForSingleObjectEx() { /* 230 (0x0e6) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtSuspendThread() { /* 231 (0x0e7) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtUserIoApcDispatcher() { /* 232 (0x0e8) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtWaitForSingleObject() { /* 233 (0x0e9) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtWaitForSingleObjectEx() { /* 234 (0x0ea) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtWaitForMultipleObjectsEx() { /* 235 (0x0eb) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL int
xboxkrnl_NtWriteFile( /* 236 (0x0ec) */
        /* in */       xboxkrnl_file *FileHandle,
        /* in (opt) */ void *Event,
        /* in (opt) */ void *ApcRoutine,
        /* in (opt) */ void *ApcContext,
        /* out */      xboxkrnl_iostatus *IoStatusBlock,
        /* in */       void *Buffer,
        /* in */       uint32_t Length,
        /* in (opt) */ int64_t *ByteOffset) {
    ssize_t wr;
    int ret = -1;
    ENTER;
    VARDUMP(VAR_IN,  FileHandle);
    VARDUMP(VAR_IN,  Event);
    VARDUMP(VAR_IN,  ApcRoutine);
    VARDUMP(VAR_IN,  ApcContext);
    VARDUMP(VAR_OUT, IoStatusBlock);
    VARDUMP(VAR_OUT, IoStatusBlock->Status);
    VARDUMP(VAR_OUT, IoStatusBlock->Information);
    VARDUMP(VAR_IN,  Buffer);
    VARDUMP(VAR_IN,  Length);
    VARDUMP(VAR_IN,  ByteOffset);

    IoStatusBlock->Information = 0;

    do {
        if (ByteOffset) INT3;
        if ((wr = write(FileHandle->fd, Buffer, Length)) != (typeof(wr))Length) {
            PRINT("error: write(): '%s'", (wr < 0) ? strerror(errno) : "short write");
            break;
        }
    } while ((ret = 0));

    if (!(IoStatusBlock->Status = ret)) IoStatusBlock->Information = wr;
    else INT3;

    VARDUMP(DUMP, IoStatusBlock->Status);
    VARDUMP(DUMP, IoStatusBlock->Information);
    VARDUMP(DUMP, ret);
INT3;//XXX
    LEAVE;
    return ret;
}
static STDCALL void
xboxkrnl_NtWriteFileGather() { /* 237 (0x0ed) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_NtYieldExecution() { /* 238 (0x0ee) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_ObCreateObject() { /* 239 (0x0ef) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_ObDirectoryObjectType() { /* 240 (0x0f0) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
typedef struct _OBJECT_TYPE {
    OB_ALLOCATE_METHOD AllocateProcedure;
    OB_FREE_METHOD FreeProcedure;
    OB_CLOSE_METHOD CloseProcedure;
    OB_DELETE_METHOD DeleteProcedure;
    OB_PARSE_METHOD ParseProcedure;
    PVOID DefaultObject;
    ULONG PoolTag;
} OBJECT_TYPE, *POBJECT_TYPE;
#endif
static void *
xboxkrnl_ObDirectoryObjectType = (void *)0xdeadbeef; /* 240 (0x0f0) */
#endif
static STDCALL void
xboxkrnl_ObInsertObject() { /* 241 (0x0f1) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_ObMakeTemporaryObject() { /* 242 (0x0f2) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_ObOpenObjectByName() { /* 243 (0x0f3) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_ObOpenObjectByPointer() { /* 244 (0x0f4) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_ObpObjectHandleTable() { /* 245 (0x0f5) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
#define OB_HANDLES_PER_TABLE_SHIFT      6
#define OB_HANDLES_PER_TABLE            (1 << OB_HANDLES_PER_TABLE_SHIFT)
#define OB_TABLES_PER_SEGMENT           8
#define OB_HANDLES_PER_SEGMENT          (OB_TABLES_PER_SEGMENT * OB_HANDLES_PER_TABLE)

typedef struct _OBJECT_HANDLE_TABLE {
    LONG HandleCount;
    LONG_PTR FirstFreeTableEntry;
    HANDLE NextHandleNeedingPool;
    PVOID **RootTable;
    PVOID *BuiltinRootTable[OB_TABLES_PER_SEGMENT];
} OBJECT_HANDLE_TABLE, *POBJECT_HANDLE_TABLE;
#endif
static void *
xboxkrnl_ObpObjectHandleTable = (void *)0xdeadbeef; /* 245 (0x0f5) */
#endif
static STDCALL void
xboxkrnl_ObReferenceObjectByHandle() { /* 246 (0x0f6) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_ObReferenceObjectByName() { /* 247 (0x0f7) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_ObReferenceObjectByPointer() { /* 248 (0x0f8) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_ObSymbolicLinkObjectType() { /* 249 (0x0f9) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
typedef struct _OBJECT_TYPE {
    OB_ALLOCATE_METHOD AllocateProcedure;
    OB_FREE_METHOD FreeProcedure;
    OB_CLOSE_METHOD CloseProcedure;
    OB_DELETE_METHOD DeleteProcedure;
    OB_PARSE_METHOD ParseProcedure;
    PVOID DefaultObject;
    ULONG PoolTag;
} OBJECT_TYPE, *POBJECT_TYPE;
#endif
static void *
xboxkrnl_ObSymbolicLinkObjectType = (void *)0xdeadbeef; /* 249 (0x0f9) */
#endif
static FASTCALL void
xboxkrnl_ObfDereferenceObject() { /* 250 (0x0fa) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_ObfReferenceObject() { /* 251 (0x0fb) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_PhyGetLinkState() { /* 252 (0x0fc) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_PhyInitialize() { /* 253 (0x0fd) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL int
xboxkrnl_PsCreateSystemThread( /* 254 (0x0fe) */
        /* out */       void *ThreadHandle,
        /* out (opt) */ void **ThreadId,
        /* in */        void *StartRoutine,
        /* in */        void *StartContext,
        /* in */        int DebuggerThread) {
    return xboxkrnl_PsCreateSystemThreadEx(
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
}
static STDCALL int
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
    VARDUMP(VAR_OUT, ThreadHandle);
    VARDUMP(VAR_IN,  ThreadExtensionSize);
    VARDUMP(VAR_IN,  KernelStackSize);
    VARDUMP(VAR_IN,  TlsDataSize);
    VARDUMP(VAR_OUT, ThreadId);
    VARDUMP(VAR_IN,  StartRoutine);
    VARDUMP(VAR_IN,  StartContext);
    VARDUMP(VAR_IN,  CreateSuspended);
    VARDUMP(VAR_IN,  DebuggerThread);
    VARDUMP(VAR_IN,  SystemRoutine);

    PRINT("/* protected calling thread is system kernel thread */", 0);
    LEAVE;
    ((void (*)(void *))StartRoutine)(StartContext);

    return 0;
}
static STDCALL void
xboxkrnl_PsQueryStatistics() { /* 256 (0x100) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_PsSetCreateThreadNotifyRoutine() { /* 257 (0x101) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_PsTerminateSystemThread() { /* 258 (0x102) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_PsThreadObjectType() { /* 259 (0x103) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
typedef struct _OBJECT_TYPE {
    OB_ALLOCATE_METHOD AllocateProcedure;
    OB_FREE_METHOD FreeProcedure;
    OB_CLOSE_METHOD CloseProcedure;
    OB_DELETE_METHOD DeleteProcedure;
    OB_PARSE_METHOD ParseProcedure;
    PVOID DefaultObject;
    ULONG PoolTag;
} OBJECT_TYPE, *POBJECT_TYPE;
#endif
static void *
xboxkrnl_PsThreadObjectType = (void *)0xdeadbeef; /* 259 (0x103) */
#endif
static STDCALL void
xboxkrnl_RtlAnsiStringToUnicodeString() { /* 260 (0x104) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlAppendStringToString() { /* 261 (0x105) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlAppendUnicodeStringToString() { /* 262 (0x106) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlAppendUnicodeToString() { /* 263 (0x107) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlAssert() { /* 264 (0x108) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlCaptureContext() { /* 265 (0x109) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlCaptureStackBackTrace() { /* 266 (0x10a) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlCharToInteger() { /* 267 (0x10b) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlCompareMemory() { /* 268 (0x10c) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlCompareMemoryUlong() { /* 269 (0x10d) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlCompareString() { /* 270 (0x10e) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlCompareUnicodeString() { /* 271 (0x10f) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlCopyString() { /* 272 (0x110) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlCopyUnicodeString() { /* 273 (0x111) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlCreateUnicodeString() { /* 274 (0x112) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlDowncaseUnicodeChar() { /* 275 (0x113) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlDowncaseUnicodeString() { /* 276 (0x114) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlEnterCriticalSection( /* 277 (0x115) */
        /* in */ pthread_mutex_t **CriticalSection) {
    ENTER;
    VARDUMP(VAR_IN, CriticalSection);
    VARDUMP(VAR_IN, *CriticalSection);

    pthread_mutex_lock(*CriticalSection);

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlEnterCriticalSectionAndRegion() { /* 278 (0x116) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL uint8_t
xboxkrnl_RtlEqualString( /* 279 (0x117) */
        /* in */ xboxkrnl_string *String1,
        /* in */ xboxkrnl_string *String2,
        /* in */ uint8_t CaseInSensitive) {
    size_t len;
    uint8_t ret = 0;
    ENTER;
    VARDUMP(VAR_IN, String1);
    VARDUMP(STRING, String1->Buffer);
    VARDUMP(VAR_IN, String2);
    VARDUMP(STRING, String2->Buffer);
    VARDUMP(VAR_IN, CaseInSensitive);

    len = strlen(String1->Buffer) + 1;

    if ((CaseInSensitive && !strncasecmp(String1->Buffer, String2->Buffer, len)) ||
        (!CaseInSensitive && !strncmp(String1->Buffer, String2->Buffer, len))) ret = 1;

    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
}
static STDCALL void
xboxkrnl_RtlEqualUnicodeString() { /* 280 (0x118) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlExtendedIntegerMultiply() { /* 281 (0x119) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlExtendedLargeIntegerDivide() { /* 282 (0x11a) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlExtendedMagicDivide() { /* 283 (0x11b) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlFillMemory() { /* 284 (0x11c) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlFillMemoryUlong() { /* 285 (0x11d) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlFreeAnsiString() { /* 286 (0x11e) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlFreeUnicodeString() { /* 287 (0x11f) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlGetCallersAddress() { /* 288 (0x120) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlInitAnsiString( /* 289 (0x121) */
        /* out */ xboxkrnl_string *DestinationString,
        /* in */  const char *SourceString) {
    ENTER;
    VARDUMP(VAR_OUT, DestinationString);
    VARDUMP(VAR_OUT, DestinationString->Length);
    VARDUMP(VAR_OUT, DestinationString->MaximumLength);
    VARDUMP(VAR_OUT, DestinationString->Buffer);
    VARDUMP(VAR_IN,  SourceString);
    VARDUMP(STRING,  SourceString);

    DestinationString->Length = strlen(SourceString);
    DestinationString->MaximumLength = DestinationString->Length + 1;
    DestinationString->Buffer = strdup(SourceString);

    VARDUMP(DUMP,   DestinationString->Length);
    VARDUMP(DUMP,   DestinationString->MaximumLength);
    VARDUMP(DUMP,   DestinationString->Buffer);
    VARDUMP(STRING, DestinationString->Buffer);
    LEAVE;
}
static STDCALL void
xboxkrnl_RtlInitUnicodeString() { /* 290 (0x122) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlInitializeCriticalSection( /* 291 (0x123) */
        /* i/o */ pthread_mutex_t **CriticalSection) {
    ENTER;
    VARDUMP(VAR_IN,  CriticalSection);
    VARDUMP(VAR_OUT, *CriticalSection);

    if (!(*CriticalSection = calloc(1, sizeof(**CriticalSection)))) INT3;
    pthread_mutex_init(*CriticalSection, NULL);

    VARDUMP(DUMP, *CriticalSection);
    LEAVE;
}
static STDCALL void
xboxkrnl_RtlIntegerToChar() { /* 292 (0x124) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlIntegerToUnicodeString() { /* 293 (0x125) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlLeaveCriticalSection( /* 294 (0x126) */
        /* in */ pthread_mutex_t **CriticalSection) {
    ENTER;
    VARDUMP(VAR_IN, CriticalSection);
    VARDUMP(VAR_IN, *CriticalSection);

    pthread_mutex_unlock(*CriticalSection);

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlLeaveCriticalSectionAndRegion() { /* 295 (0x127) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlLowerChar() { /* 296 (0x128) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlMapGenericMask() { /* 297 (0x129) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlMoveMemory() { /* 298 (0x12a) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlMultiByteToUnicodeN() { /* 299 (0x12b) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlMultiByteToUnicodeSize() { /* 300 (0x12c) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL int
xboxkrnl_RtlNtStatusToDosError( /* 301 (0x12d) */
        /* in */ int Status) {
    int ret;
    ENTER;
    VARDUMP(VAR_IN, Status);

    ret = !!Status;

    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
}
static STDCALL void
xboxkrnl_RtlRaiseException() { /* 302 (0x12e) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlRaiseStatus() { /* 303 (0x12f) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlRip() { /* 352 (0x160) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlSnprintf() { /* 361 (0x169) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlSprintf() { /* 362 (0x16a) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlTimeFieldsToTime() { /* 304 (0x130) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlTimeToTimeFields() { /* 305 (0x131) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlTryEnterCriticalSection() { /* 306 (0x132) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_RtlUlongByteSwap() { /* 307 (0x133) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlUnicodeStringToAnsiString() { /* 308 (0x134) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlUnicodeStringToInteger() { /* 309 (0x135) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlUnicodeToMultiByteN() { /* 310 (0x136) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlUnicodeToMultiByteSize() { /* 311 (0x137) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlUnwind() { /* 312 (0x138) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlUpcaseUnicodeChar() { /* 313 (0x139) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlUpcaseUnicodeString() { /* 314 (0x13a) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlUpcaseUnicodeToMultiByteN() { /* 315 (0x13b) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlUpperChar() { /* 316 (0x13c) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlUpperString() { /* 317 (0x13d) */
    ENTER;

    INT3;
    

    LEAVE;
}
static FASTCALL void
xboxkrnl_RtlUshortByteSwap() { /* 318 (0x13e) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlVsnprintf() { /* 363 (0x16b) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlVsprintf() { /* 364 (0x16c) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlWalkFrameChain() { /* 319 (0x13f) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_RtlZeroMemory() { /* 320 (0x140) */
    ENTER;

    INT3;
    

    LEAVE;
}
#if 0
static STDCALL void
xboxkrnl_XboxAlternateSignatureKeys() { /* 354 (0x162) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
static uint8_t
xboxkrnl_XboxAlternateSignatureKeys[16][16] = { }; /* 354 (0x162) */
#endif
#if 0
static STDCALL void
xboxkrnl_XboxEEPROMKey() { /* 321 (0x141) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
static uint8_t
xboxkrnl_XboxEEPROMKey[16] = { }; /* 321 (0x141) */
#endif
#if 0
static STDCALL void
xboxkrnl_XboxHardwareInfo() { /* 322 (0x142) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
#define XBOX_HW_FLAG_INTERNAL_USB_HUB   0x00000001
#define XBOX_HW_FLAG_DEVKIT_KERNEL      0x00000002
#define XBOX_HW_FLAG_ARCADE             0x00000008

typedef struct _XBOX_HARDWARE_INFO {
    ULONG Flags;
    UCHAR GpuRevision;
    UCHAR McpRevision;
    UCHAR reserved[2];
} XBOX_HARDWARE_INFO;
#endif
static struct {
    uint32_t Flags;
    uint8_t  GpuRevision;
    uint8_t  McpRevision;
    uint8_t  reserved[2];
} xboxkrnl_XboxHardwareInfo = { }; /* 322 (0x142) */
#endif
#if 0
static STDCALL void
xboxkrnl_XboxHDKey() { /* 323 (0x143) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
static uint8_t
xboxkrnl_XboxHDKey[16] = { }; /* 323 (0x143) */
#endif
#if 0
static STDCALL void
xboxkrnl_XboxLANKey() { /* 353 (0x161) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
static uint8_t
xboxkrnl_XboxLANKey[16] = { }; /* 353 (0x161) */
#endif
#if 0
static STDCALL void
xboxkrnl_XboxKrnlVersion() { /* 324 (0x144) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
#if 0
typedef struct _XBOX_KRNL_VERSION {
    USHORT Major;
    USHORT Minor;
    USHORT Build;
    USHORT Qfe;
} XBOX_KRNL_VERSION, *PXBOX_KRNL_VERSION;
#endif
static void *
xboxkrnl_XboxKrnlVersion = (void *)0xdeadbeef; /* 324 (0x144) */
#endif
#if 0
static STDCALL void
xboxkrnl_XboxSignatureKey() { /* 325 (0x145) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
static uint8_t
xboxkrnl_XboxSignatureKey[16] = { }; /* 325 (0x145) */
#endif
#if 0
static STDCALL void
xboxkrnl_XeImageFileName() { /* 326 (0x146) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
static xboxkrnl_string
xboxkrnl_XeImageFileName = { }; /* 326 (0x146) */
#endif
static STDCALL int
xboxkrnl_XeLoadSection( /* 327 (0x147) */
        /* in */ void *Section) {
    ENTER;
    VARDUMP(STUB, Section);

    LEAVE;
    return 0;
}
#if 0
static STDCALL void
xboxkrnl_XePublicKeyData() { /* 355 (0x163) */
    ENTER;

    INT3;
    

    LEAVE;
}
#else
static uint8_t
xboxkrnl_XePublicKeyData[284] = { }; /* 355 (0x163) */
#endif
static STDCALL int
xboxkrnl_XeUnloadSection( /* 328 (0x148) */
        /* in */ void *Section) {
    ENTER;
    VARDUMP(STUB, Section);

    LEAVE;
    return 0;
}
static STDCALL void
xboxkrnl_READ_PORT_BUFFER_UCHAR() { /* 329 (0x149) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_READ_PORT_BUFFER_USHORT() { /* 330 (0x14a) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_READ_PORT_BUFFER_ULONG() { /* 331 (0x14b) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_WRITE_PORT_BUFFER_UCHAR() { /* 332 (0x14c) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_WRITE_PORT_BUFFER_USHORT() { /* 333 (0x14d) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_WRITE_PORT_BUFFER_ULONG() { /* 334 (0x14e) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcSHAInit() { /* 335 (0x14f) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcSHAUpdate() { /* 336 (0x150) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcSHAFinal() { /* 337 (0x151) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcRC4Key() { /* 338 (0x152) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcRC4Crypt() { /* 339 (0x153) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcHMAC() { /* 340 (0x154) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcPKEncPublic() { /* 341 (0x155) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcPKDecPrivate() { /* 342 (0x156) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcPKGetKeyLen() { /* 343 (0x157) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcVerifyPKCS1Signature() { /* 344 (0x158) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcModExp() { /* 345 (0x159) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcDESKeyParity() { /* 346 (0x15a) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcKeyTable() { /* 347 (0x15b) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcBlockCrypt() { /* 348 (0x15c) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcBlockCryptCBC() { /* 349 (0x15d) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcCryptService() { /* 350 (0x15e) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_XcUpdateCrypto() { /* 351 (0x15f) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_DbgLoadImageSymbols() { /* 007 (0x007) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_DbgUnLoadImageSymbols() { /* 011 (0x00b) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_MmDbgAllocateMemory() { /* 374 (0x176) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_MmDbgFreeMemory() { /* 375 (0x177) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_MmDbgQueryAvailablePages() { /* 376 (0x178) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_MmDbgReleaseAddress() { /* 377 (0x179) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL void
xboxkrnl_MmDbgWriteCheck() { /* 378 (0x17a) */
    ENTER;

    INT3;
    

    LEAVE;
}

static const void *const xboxkrnl_thunk[] = {
    /* 001 */ [0x001] = &xboxkrnl_AvGetSavedDataAddress,                /* stdcall */
    /* 002 */ [0x002] = &xboxkrnl_AvSendTVEncoderOption,                /* stdcall */
    /* 003 */ [0x003] = &xboxkrnl_AvSetDisplayMode,                     /* stdcall */
    /* 004 */ [0x004] = &xboxkrnl_AvSetSavedDataAddress,                /* stdcall */
    /* 005 */ [0x005] = &xboxkrnl_DbgBreakPoint,                        /* stdcall */
    /* 006 */ [0x006] = &xboxkrnl_DbgBreakPointWithStatus,              /* stdcall */
    /* 008 */ [0x008] = &xboxkrnl_DbgPrint,                             /* stdcall */
    /* 010 */ [0x00a] = &xboxkrnl_DbgPrompt,                            /* stdcall */
    /* 012 */ [0x00c] = &xboxkrnl_ExAcquireReadWriteLockExclusive,      /* stdcall */
    /* 013 */ [0x00d] = &xboxkrnl_ExAcquireReadWriteLockShared,         /* stdcall */
    /* 014 */ [0x00e] = &xboxkrnl_ExAllocatePool,                       /* stdcall */
    /* 015 */ [0x00f] = &xboxkrnl_ExAllocatePoolWithTag,                /* stdcall */
#if 0
    /* 016 */ [0x010] = &xboxkrnl_ExEventObjectType,                    /* variable */
#endif
    /* 017 */ [0x011] = &xboxkrnl_ExFreePool,                           /* stdcall */
    /* 018 */ [0x012] = &xboxkrnl_ExInitializeReadWriteLock,            /* stdcall */
    /* 019 */ [0x013] = &xboxkrnl_ExInterlockedAddLargeInteger,         /* stdcall */
    /* 020 */ [0x014] = &xboxkrnl_ExInterlockedAddLargeStatistic,       /* fastcall */
    /* 021 */ [0x015] = &xboxkrnl_ExInterlockedCompareExchange64,       /* fastcall */
#if 0
    /* 022 */ [0x016] = &xboxkrnl_ExMutantObjectType,                   /* variable */
#endif
    /* 023 */ [0x017] = &xboxkrnl_ExQueryPoolBlockSize,                 /* stdcall */
    /* 024 */ [0x018] = &xboxkrnl_ExQueryNonVolatileSetting,            /* stdcall */
    /* 025 */ [0x019] = &xboxkrnl_ExReadWriteRefurbInfo,                /* stdcall */
    /* 026 */ [0x01a] = &xboxkrnl_ExRaiseException,                     /* stdcall */
    /* 027 */ [0x01b] = &xboxkrnl_ExRaiseStatus,                        /* stdcall */
    /* 028 */ [0x01c] = &xboxkrnl_ExReleaseReadWriteLock,               /* stdcall */
    /* 029 */ [0x01d] = &xboxkrnl_ExSaveNonVolatileSetting,             /* stdcall */
#if 0
    /* 030 */ [0x01e] = &xboxkrnl_ExSemaphoreObjectType,                /* variable */
#endif
#if 0
    /* 031 */ [0x01f] = &xboxkrnl_ExTimerObjectType,                    /* variable */
#endif
    /* 032 */ [0x020] = &xboxkrnl_ExfInterlockedInsertHeadList,         /* stdcall */
    /* 033 */ [0x021] = &xboxkrnl_ExfInterlockedInsertTailList,         /* fastcall */
    /* 034 */ [0x022] = &xboxkrnl_ExfInterlockedRemoveHeadList,         /* fastcall */
    /* 035 */ [0x023] = &xboxkrnl_FscGetCacheSize,                      /* stdcall */
    /* 036 */ [0x024] = &xboxkrnl_FscInvalidateIdleBlocks,              /* stdcall */
    /* 037 */ [0x025] = &xboxkrnl_FscSetCacheSize,                      /* stdcall */
#if 0
    /* 356 */ [0x164] = &xboxkrnl_HalBootSMCVideoMode,                  /* variable */
#endif
    /* 038 */ [0x026] = &xboxkrnl_HalClearSoftwareInterrupt,            /* fastcall */
    /* 039 */ [0x027] = &xboxkrnl_HalDisableSystemInterrupt,            /* stdcall */
#if 0
    /* 040 */ [0x028] = &xboxkrnl_HalDiskCachePartitionCount,           /* variable */
#endif
#if 0
    /* 041 */ [0x029] = &xboxkrnl_HalDiskModelNumber,                   /* variable */
#endif
#if 0
    /* 042 */ [0x02a] = &xboxkrnl_HalDiskSerialNumber,                  /* variable */
#endif
    /* 365 */ [0x16d] = &xboxkrnl_HalEnableSecureTrayEject,             /* stdcall */
    /* 043 */ [0x02b] = &xboxkrnl_HalEnableSystemInterrupt,             /* stdcall */
    /* 044 */ [0x02c] = &xboxkrnl_HalGetInterruptVector,                /* stdcall */
    /* 360 */ [0x168] = &xboxkrnl_HalInitiateShutdown,                  /* stdcall */
    /* 358 */ [0x166] = &xboxkrnl_HalIsResetOrShutdownPending,          /* stdcall */
    /* 045 */ [0x02d] = &xboxkrnl_HalReadSMBusValue,                    /* stdcall */
    /* 009 */ [0x009] = &xboxkrnl_HalReadSMCTrayState,                  /* stdcall */
    /* 046 */ [0x02e] = &xboxkrnl_HalReadWritePCISpace,                 /* stdcall */
    /* 047 */ [0x02f] = &xboxkrnl_HalRegisterShutdownNotification,      /* stdcall */
    /* 048 */ [0x030] = &xboxkrnl_HalRequestSoftwareInterrupt,          /* fastcall */
    /* 049 */ [0x031] = &xboxkrnl_HalReturnToFirmware,                  /* stdcall */
    /* 050 */ [0x032] = &xboxkrnl_HalWriteSMBusValue,                   /* stdcall */
    /* 366 */ [0x16e] = &xboxkrnl_HalWriteSMCScratchRegister,           /* stdcall */
#if 0
    /* 357 */ [0x165] = &xboxkrnl_IdexChannelObject,                    /* variable */
#endif
    /* 051 */ [0x033] = &xboxkrnl_InterlockedCompareExchange,           /* fastcall */
    /* 052 */ [0x034] = &xboxkrnl_InterlockedDecrement,                 /* fastcall */
    /* 053 */ [0x035] = &xboxkrnl_InterlockedIncrement,                 /* fastcall */
    /* 054 */ [0x036] = &xboxkrnl_InterlockedExchange,                  /* fastcall */
    /* 055 */ [0x037] = &xboxkrnl_InterlockedExchangeAdd,               /* fastcall */
    /* 056 */ [0x038] = &xboxkrnl_InterlockedFlushSList,                /* fastcall */
    /* 057 */ [0x039] = &xboxkrnl_InterlockedPopEntrySList,             /* fastcall */
    /* 058 */ [0x03a] = &xboxkrnl_InterlockedPushEntrySList,            /* fastcall */
    /* 059 */ [0x03b] = &xboxkrnl_IoAllocateIrp,                        /* stdcall */
    /* 060 */ [0x03c] = &xboxkrnl_IoBuildAsynchronousFsdRequest,        /* stdcall */
    /* 061 */ [0x03d] = &xboxkrnl_IoBuildDeviceIoControlRequest,        /* stdcall */
    /* 062 */ [0x03e] = &xboxkrnl_IoBuildSynchronousFsdRequest,         /* stdcall */
    /* 063 */ [0x03f] = &xboxkrnl_IoCheckShareAccess,                   /* stdcall */
#if 0
    /* 064 */ [0x040] = &xboxkrnl_IoCompletionObjectType,               /* variable */
#endif
    /* 065 */ [0x041] = &xboxkrnl_IoCreateDevice,                       /* stdcall */
    /* 066 */ [0x042] = &xboxkrnl_IoCreateFile,                         /* stdcall */
    /* 067 */ [0x043] = &xboxkrnl_IoCreateSymbolicLink,                 /* stdcall */
    /* 068 */ [0x044] = &xboxkrnl_IoDeleteDevice,                       /* stdcall */
    /* 069 */ [0x045] = &xboxkrnl_IoDeleteSymbolicLink,                 /* stdcall */
#if 0
    /* 070 */ [0x046] = &xboxkrnl_IoDeviceObjectType,                   /* variable */
#endif
    /* 090 */ [0x05a] = &xboxkrnl_IoDismountVolume,                     /* stdcall */
    /* 091 */ [0x05b] = &xboxkrnl_IoDismountVolumeByName,               /* stdcall */
#if 0
    /* 071 */ [0x047] = &xboxkrnl_IoFileObjectType,                     /* variable */
#endif
    /* 072 */ [0x048] = &xboxkrnl_IoFreeIrp,                            /* stdcall */
    /* 073 */ [0x049] = &xboxkrnl_IoInitializeIrp,                      /* stdcall */
    /* 074 */ [0x04a] = &xboxkrnl_IoInvalidDeviceRequest,               /* stdcall */
    /* 359 */ [0x167] = &xboxkrnl_IoMarkIrpMustComplete,                /* stdcall */
    /* 075 */ [0x04b] = &xboxkrnl_IoQueryFileInformation,               /* stdcall */
    /* 076 */ [0x04c] = &xboxkrnl_IoQueryVolumeInformation,             /* stdcall */
    /* 077 */ [0x04d] = &xboxkrnl_IoQueueThreadIrp,                     /* stdcall */
    /* 078 */ [0x04e] = &xboxkrnl_IoRemoveShareAccess,                  /* stdcall */
    /* 079 */ [0x04f] = &xboxkrnl_IoSetIoCompletion,                    /* stdcall */
    /* 080 */ [0x050] = &xboxkrnl_IoSetShareAccess,                     /* stdcall */
    /* 081 */ [0x051] = &xboxkrnl_IoStartNextPacket,                    /* stdcall */
    /* 082 */ [0x052] = &xboxkrnl_IoStartNextPacketByKey,               /* stdcall */
    /* 083 */ [0x053] = &xboxkrnl_IoStartPacket,                        /* stdcall */
    /* 084 */ [0x054] = &xboxkrnl_IoSynchronousDeviceIoControlRequest,  /* stdcall */
    /* 085 */ [0x055] = &xboxkrnl_IoSynchronousFsdRequest,              /* stdcall */
    /* 086 */ [0x056] = &xboxkrnl_IofCallDriver,                        /* fastcall */
    /* 087 */ [0x057] = &xboxkrnl_IofCompleteRequest,                   /* fastcall */
#if 0
    /* 088 */ [0x058] = &xboxkrnl_KdDebuggerEnabled,                    /* variable */
#endif
#if 0
    /* 089 */ [0x059] = &xboxkrnl_KdDebuggerNotPresent,                 /* variable */
#endif
    /* 092 */ [0x05c] = &xboxkrnl_KeAlertResumeThread,                  /* stdcall */
    /* 093 */ [0x05d] = &xboxkrnl_KeAlertThread,                        /* stdcall */
    /* 094 */ [0x05e] = &xboxkrnl_KeBoostPriorityThread,                /* stdcall */
    /* 095 */ [0x05f] = &xboxkrnl_KeBugCheck,                           /* stdcall */
    /* 096 */ [0x060] = &xboxkrnl_KeBugCheckEx,                         /* stdcall */
    /* 097 */ [0x061] = &xboxkrnl_KeCancelTimer,                        /* stdcall */
    /* 098 */ [0x062] = &xboxkrnl_KeConnectInterrupt,                   /* stdcall */
    /* 099 */ [0x063] = &xboxkrnl_KeDelayExecutionThread,               /* stdcall */
    /* 100 */ [0x064] = &xboxkrnl_KeDisconnectInterrupt,                /* stdcall */
    /* 101 */ [0x065] = &xboxkrnl_KeEnterCriticalRegion,                /* stdcall */
    /* 103 */ [0x067] = &xboxkrnl_KeGetCurrentIrql,                     /* stdcall */
    /* 104 */ [0x068] = &xboxkrnl_KeGetCurrentThread,                   /* stdcall */
    /* 105 */ [0x069] = &xboxkrnl_KeInitializeApc,                      /* stdcall */
    /* 106 */ [0x06a] = &xboxkrnl_KeInitializeDeviceQueue,              /* stdcall */
    /* 107 */ [0x06b] = &xboxkrnl_KeInitializeDpc,                      /* stdcall */
    /* 108 */ [0x06c] = &xboxkrnl_KeInitializeEvent,                    /* stdcall */
    /* 109 */ [0x06d] = &xboxkrnl_KeInitializeInterrupt,                /* stdcall */
    /* 110 */ [0x06e] = &xboxkrnl_KeInitializeMutant,                   /* stdcall */
    /* 111 */ [0x06f] = &xboxkrnl_KeInitializeQueue,                    /* stdcall */
    /* 112 */ [0x070] = &xboxkrnl_KeInitializeSemaphore,                /* stdcall */
    /* 113 */ [0x071] = &xboxkrnl_KeInitializeTimerEx,                  /* stdcall */
    /* 114 */ [0x072] = &xboxkrnl_KeInsertByKeyDeviceQueue,             /* stdcall */
    /* 115 */ [0x073] = &xboxkrnl_KeInsertDeviceQueue,                  /* stdcall */
    /* 116 */ [0x074] = &xboxkrnl_KeInsertHeadQueue,                    /* stdcall */
    /* 117 */ [0x075] = &xboxkrnl_KeInsertQueue,                        /* stdcall */
    /* 118 */ [0x076] = &xboxkrnl_KeInsertQueueApc,                     /* stdcall */
    /* 119 */ [0x077] = &xboxkrnl_KeInsertQueueDpc,                     /* stdcall */
#if 0
    /* 120 */ [0x078] = &xboxkrnl_KeInterruptTime,                      /* variable */
#endif
    /* 121 */ [0x079] = &xboxkrnl_KeIsExecutingDpc,                     /* stdcall */
    /* 122 */ [0x07a] = &xboxkrnl_KeLeaveCriticalRegion,                /* stdcall */
    /* 123 */ [0x07b] = &xboxkrnl_KePulseEvent,                         /* stdcall */
    /* 124 */ [0x07c] = &xboxkrnl_KeQueryBasePriorityThread,            /* stdcall */
    /* 125 */ [0x07d] = &xboxkrnl_KeQueryInterruptTime,                 /* stdcall */
    /* 126 */ [0x07e] = &xboxkrnl_KeQueryPerformanceCounter,            /* stdcall */
    /* 127 */ [0x07f] = &xboxkrnl_KeQueryPerformanceFrequency,          /* stdcall */
    /* 128 */ [0x080] = &xboxkrnl_KeQuerySystemTime,                    /* stdcall */
    /* 129 */ [0x081] = &xboxkrnl_KeRaiseIrqlToDpcLevel,                /* stdcall */
    /* 130 */ [0x082] = &xboxkrnl_KeRaiseIrqlToSynchLevel,              /* stdcall */
    /* 131 */ [0x083] = &xboxkrnl_KeReleaseMutant,                      /* stdcall */
    /* 132 */ [0x084] = &xboxkrnl_KeReleaseSemaphore,                   /* stdcall */
    /* 133 */ [0x085] = &xboxkrnl_KeRemoveByKeyDeviceQueue,             /* stdcall */
    /* 134 */ [0x086] = &xboxkrnl_KeRemoveDeviceQueue,                  /* stdcall */
    /* 135 */ [0x087] = &xboxkrnl_KeRemoveEntryDeviceQueue,             /* stdcall */
    /* 136 */ [0x088] = &xboxkrnl_KeRemoveQueue,                        /* stdcall */
    /* 137 */ [0x089] = &xboxkrnl_KeRemoveQueueDpc,                     /* stdcall */
    /* 138 */ [0x08a] = &xboxkrnl_KeResetEvent,                         /* stdcall */
    /* 139 */ [0x08b] = &xboxkrnl_KeRestoreFloatingPointState,          /* stdcall */
    /* 140 */ [0x08c] = &xboxkrnl_KeResumeThread,                       /* stdcall */
    /* 141 */ [0x08d] = &xboxkrnl_KeRundownQueue,                       /* stdcall */
    /* 142 */ [0x08e] = &xboxkrnl_KeSaveFloatingPointState,             /* stdcall */
    /* 143 */ [0x08f] = &xboxkrnl_KeSetBasePriorityThread,              /* stdcall */
    /* 144 */ [0x090] = &xboxkrnl_KeSetDisableBoostThread,              /* stdcall */
    /* 145 */ [0x091] = &xboxkrnl_KeSetEvent,                           /* stdcall */
    /* 146 */ [0x092] = &xboxkrnl_KeSetEventBoostPriority,              /* stdcall */
    /* 147 */ [0x093] = &xboxkrnl_KeSetPriorityProcess,                 /* stdcall */
    /* 148 */ [0x094] = &xboxkrnl_KeSetPriorityThread,                  /* stdcall */
    /* 149 */ [0x095] = &xboxkrnl_KeSetTimer,                           /* stdcall */
    /* 150 */ [0x096] = &xboxkrnl_KeSetTimerEx,                         /* stdcall */
    /* 151 */ [0x097] = &xboxkrnl_KeStallExecutionProcessor,            /* stdcall */
    /* 152 */ [0x098] = &xboxkrnl_KeSuspendThread,                      /* stdcall */
    /* 153 */ [0x099] = &xboxkrnl_KeSynchronizeExecution,               /* stdcall */
#if 0
    /* 154 */ [0x09a] = &xboxkrnl_KeSystemTime,                         /* variable */
#endif
    /* 155 */ [0x09b] = &xboxkrnl_KeTestAlertThread,                    /* stdcall */
#if 0
    /* 156 */ [0x09c] = &xboxkrnl_KeTickCount,                          /* variable */
#endif
#if 0
    /* 157 */ [0x09d] = &xboxkrnl_KeTimeIncrement,                      /* variable */
#endif
    /* 158 */ [0x09e] = &xboxkrnl_KeWaitForMultipleObjects,             /* stdcall */
    /* 159 */ [0x09f] = &xboxkrnl_KeWaitForSingleObject,                /* stdcall */
    /* 160 */ [0x0a0] = &xboxkrnl_KfRaiseIrql,                          /* fastcall */
    /* 161 */ [0x0a1] = &xboxkrnl_KfLowerIrql,                          /* fastcall */
#if 0
    /* 162 */ [0x0a2] = &xboxkrnl_KiBugCheckData,                       /* variable */
#endif
    /* 163 */ [0x0a3] = &xboxkrnl_KiUnlockDispatcherDatabase,           /* fastcall */
#if 0
    /* 164 */ [0x0a4] = &xboxkrnl_LaunchDataPage,                       /* variable */
#endif
    /* 165 */ [0x0a5] = &xboxkrnl_MmAllocateContiguousMemory,           /* stdcall */
    /* 166 */ [0x0a6] = &xboxkrnl_MmAllocateContiguousMemoryEx,         /* stdcall */
    /* 167 */ [0x0a7] = &xboxkrnl_MmAllocateSystemMemory,               /* stdcall */
    /* 168 */ [0x0a8] = &xboxkrnl_MmClaimGpuInstanceMemory,             /* stdcall */
    /* 169 */ [0x0a9] = &xboxkrnl_MmCreateKernelStack,                  /* stdcall */
    /* 170 */ [0x0aa] = &xboxkrnl_MmDeleteKernelStack,                  /* stdcall */
    /* 171 */ [0x0ab] = &xboxkrnl_MmFreeContiguousMemory,               /* stdcall */
    /* 172 */ [0x0ac] = &xboxkrnl_MmFreeSystemMemory,                   /* stdcall */
    /* 173 */ [0x0ad] = &xboxkrnl_MmGetPhysicalAddress,                 /* stdcall */
#if 0
    /* 102 */ [0x066] = &xboxkrnl_MmGlobalData,                         /* variable */
#endif
    /* 174 */ [0x0ae] = &xboxkrnl_MmIsAddressValid,                     /* stdcall */
    /* 175 */ [0x0af] = &xboxkrnl_MmLockUnlockBufferPages,              /* stdcall */
    /* 176 */ [0x0b0] = &xboxkrnl_MmLockUnlockPhysicalPage,             /* stdcall */
    /* 177 */ [0x0b1] = &xboxkrnl_MmMapIoSpace,                         /* stdcall */
    /* 178 */ [0x0b2] = &xboxkrnl_MmPersistContiguousMemory,            /* stdcall */
    /* 179 */ [0x0b3] = &xboxkrnl_MmQueryAddressProtect,                /* stdcall */
    /* 180 */ [0x0b4] = &xboxkrnl_MmQueryAllocationSize,                /* stdcall */
    /* 181 */ [0x0b5] = &xboxkrnl_MmQueryStatistics,                    /* stdcall */
    /* 182 */ [0x0b6] = &xboxkrnl_MmSetAddressProtect,                  /* stdcall */
    /* 183 */ [0x0b7] = &xboxkrnl_MmUnmapIoSpace,                       /* stdcall */
    /* 184 */ [0x0b8] = &xboxkrnl_NtAllocateVirtualMemory,              /* stdcall */
    /* 185 */ [0x0b9] = &xboxkrnl_NtCancelTimer,                        /* stdcall */
    /* 186 */ [0x0ba] = &xboxkrnl_NtClearEvent,                         /* stdcall */
    /* 187 */ [0x0bb] = &xboxkrnl_NtClose,                              /* stdcall */
    /* 188 */ [0x0bc] = &xboxkrnl_NtCreateDirectoryObject,              /* stdcall */
    /* 189 */ [0x0bd] = &xboxkrnl_NtCreateEvent,                        /* stdcall */
    /* 190 */ [0x0be] = &xboxkrnl_NtCreateFile,                         /* stdcall */
    /* 191 */ [0x0bf] = &xboxkrnl_NtCreateIoCompletion,                 /* stdcall */
    /* 192 */ [0x0c0] = &xboxkrnl_NtCreateMutant,                       /* stdcall */
    /* 193 */ [0x0c1] = &xboxkrnl_NtCreateSemaphore,                    /* stdcall */
    /* 194 */ [0x0c2] = &xboxkrnl_NtCreateTimer,                        /* stdcall */
    /* 195 */ [0x0c3] = &xboxkrnl_NtDeleteFile,                         /* stdcall */
    /* 196 */ [0x0c4] = &xboxkrnl_NtDeviceIoControlFile,                /* stdcall */
    /* 197 */ [0x0c5] = &xboxkrnl_NtDuplicateObject,                    /* stdcall */
    /* 198 */ [0x0c6] = &xboxkrnl_NtFlushBuffersFile,                   /* stdcall */
    /* 199 */ [0x0c7] = &xboxkrnl_NtFreeVirtualMemory,                  /* stdcall */
    /* 200 */ [0x0c8] = &xboxkrnl_NtFsControlFile,                      /* stdcall */
    /* 201 */ [0x0c9] = &xboxkrnl_NtOpenDirectoryObject,                /* stdcall */
    /* 202 */ [0x0ca] = &xboxkrnl_NtOpenFile,                           /* stdcall */
    /* 203 */ [0x0cb] = &xboxkrnl_NtOpenSymbolicLinkObject,             /* stdcall */
    /* 204 */ [0x0cc] = &xboxkrnl_NtProtectVirtualMemory,               /* stdcall */
    /* 205 */ [0x0cd] = &xboxkrnl_NtPulseEvent,                         /* stdcall */
    /* 206 */ [0x0ce] = &xboxkrnl_NtQueueApcThread,                     /* stdcall */
    /* 207 */ [0x0cf] = &xboxkrnl_NtQueryDirectoryFile,                 /* stdcall */
    /* 208 */ [0x0d0] = &xboxkrnl_NtQueryDirectoryObject,               /* stdcall */
    /* 209 */ [0x0d1] = &xboxkrnl_NtQueryEvent,                         /* stdcall */
    /* 210 */ [0x0d2] = &xboxkrnl_NtQueryFullAttributesFile,            /* stdcall */
    /* 211 */ [0x0d3] = &xboxkrnl_NtQueryInformationFile,               /* stdcall */
    /* 212 */ [0x0d4] = &xboxkrnl_NtQueryIoCompletion,                  /* stdcall */
    /* 213 */ [0x0d5] = &xboxkrnl_NtQueryMutant,                        /* stdcall */
    /* 214 */ [0x0d6] = &xboxkrnl_NtQuerySemaphore,                     /* stdcall */
    /* 215 */ [0x0d7] = &xboxkrnl_NtQuerySymbolicLinkObject,            /* stdcall */
    /* 216 */ [0x0d8] = &xboxkrnl_NtQueryTimer,                         /* stdcall */
    /* 217 */ [0x0d9] = &xboxkrnl_NtQueryVirtualMemory,                 /* stdcall */
    /* 218 */ [0x0da] = &xboxkrnl_NtQueryVolumeInformationFile,         /* stdcall */
    /* 219 */ [0x0db] = &xboxkrnl_NtReadFile,                           /* stdcall */
    /* 220 */ [0x0dc] = &xboxkrnl_NtReadFileScatter,                    /* stdcall */
    /* 221 */ [0x0dd] = &xboxkrnl_NtReleaseMutant,                      /* stdcall */
    /* 222 */ [0x0de] = &xboxkrnl_NtReleaseSemaphore,                   /* stdcall */
    /* 223 */ [0x0df] = &xboxkrnl_NtRemoveIoCompletion,                 /* stdcall */
    /* 224 */ [0x0e0] = &xboxkrnl_NtResumeThread,                       /* stdcall */
    /* 225 */ [0x0e1] = &xboxkrnl_NtSetEvent,                           /* stdcall */
    /* 226 */ [0x0e2] = &xboxkrnl_NtSetInformationFile,                 /* stdcall */
    /* 227 */ [0x0e3] = &xboxkrnl_NtSetIoCompletion,                    /* stdcall */
    /* 228 */ [0x0e4] = &xboxkrnl_NtSetSystemTime,                      /* stdcall */
    /* 229 */ [0x0e5] = &xboxkrnl_NtSetTimerEx,                         /* stdcall */
    /* 230 */ [0x0e6] = &xboxkrnl_NtSignalAndWaitForSingleObjectEx,     /* stdcall */
    /* 231 */ [0x0e7] = &xboxkrnl_NtSuspendThread,                      /* stdcall */
    /* 232 */ [0x0e8] = &xboxkrnl_NtUserIoApcDispatcher,                /* stdcall */
    /* 233 */ [0x0e9] = &xboxkrnl_NtWaitForSingleObject,                /* stdcall */
    /* 234 */ [0x0ea] = &xboxkrnl_NtWaitForSingleObjectEx,              /* stdcall */
    /* 235 */ [0x0eb] = &xboxkrnl_NtWaitForMultipleObjectsEx,           /* stdcall */
    /* 236 */ [0x0ec] = &xboxkrnl_NtWriteFile,                          /* stdcall */
    /* 237 */ [0x0ed] = &xboxkrnl_NtWriteFileGather,                    /* stdcall */
    /* 238 */ [0x0ee] = &xboxkrnl_NtYieldExecution,                     /* stdcall */
    /* 239 */ [0x0ef] = &xboxkrnl_ObCreateObject,                       /* stdcall */
#if 0
    /* 240 */ [0x0f0] = &xboxkrnl_ObDirectoryObjectType,                /* variable */
#endif
    /* 241 */ [0x0f1] = &xboxkrnl_ObInsertObject,                       /* stdcall */
    /* 242 */ [0x0f2] = &xboxkrnl_ObMakeTemporaryObject,                /* stdcall */
    /* 243 */ [0x0f3] = &xboxkrnl_ObOpenObjectByName,                   /* stdcall */
    /* 244 */ [0x0f4] = &xboxkrnl_ObOpenObjectByPointer,                /* stdcall */
#if 0
    /* 245 */ [0x0f5] = &xboxkrnl_ObpObjectHandleTable,                 /* variable */
#endif
    /* 246 */ [0x0f6] = &xboxkrnl_ObReferenceObjectByHandle,            /* stdcall */
    /* 247 */ [0x0f7] = &xboxkrnl_ObReferenceObjectByName,              /* stdcall */
    /* 248 */ [0x0f8] = &xboxkrnl_ObReferenceObjectByPointer,           /* stdcall */
#if 0
    /* 249 */ [0x0f9] = &xboxkrnl_ObSymbolicLinkObjectType,             /* variable */
#endif
    /* 250 */ [0x0fa] = &xboxkrnl_ObfDereferenceObject,                 /* fastcall */
    /* 251 */ [0x0fb] = &xboxkrnl_ObfReferenceObject,                   /* fastcall */
    /* 252 */ [0x0fc] = &xboxkrnl_PhyGetLinkState,                      /* stdcall */
    /* 253 */ [0x0fd] = &xboxkrnl_PhyInitialize,                        /* stdcall */
    /* 254 */ [0x0fe] = &xboxkrnl_PsCreateSystemThread,                 /* stdcall */
    /* 255 */ [0x0ff] = &xboxkrnl_PsCreateSystemThreadEx,               /* stdcall */
    /* 256 */ [0x100] = &xboxkrnl_PsQueryStatistics,                    /* stdcall */
    /* 257 */ [0x101] = &xboxkrnl_PsSetCreateThreadNotifyRoutine,       /* stdcall */
    /* 258 */ [0x102] = &xboxkrnl_PsTerminateSystemThread,              /* stdcall */
#if 0
    /* 259 */ [0x103] = &xboxkrnl_PsThreadObjectType,                   /* variable */
#endif
    /* 260 */ [0x104] = &xboxkrnl_RtlAnsiStringToUnicodeString,         /* stdcall */
    /* 261 */ [0x105] = &xboxkrnl_RtlAppendStringToString,              /* stdcall */
    /* 262 */ [0x106] = &xboxkrnl_RtlAppendUnicodeStringToString,       /* stdcall */
    /* 263 */ [0x107] = &xboxkrnl_RtlAppendUnicodeToString,             /* stdcall */
    /* 264 */ [0x108] = &xboxkrnl_RtlAssert,                            /* stdcall */
    /* 265 */ [0x109] = &xboxkrnl_RtlCaptureContext,                    /* stdcall */
    /* 266 */ [0x10a] = &xboxkrnl_RtlCaptureStackBackTrace,             /* stdcall */
    /* 267 */ [0x10b] = &xboxkrnl_RtlCharToInteger,                     /* stdcall */
    /* 268 */ [0x10c] = &xboxkrnl_RtlCompareMemory,                     /* stdcall */
    /* 269 */ [0x10d] = &xboxkrnl_RtlCompareMemoryUlong,                /* stdcall */
    /* 270 */ [0x10e] = &xboxkrnl_RtlCompareString,                     /* stdcall */
    /* 271 */ [0x10f] = &xboxkrnl_RtlCompareUnicodeString,              /* stdcall */
    /* 272 */ [0x110] = &xboxkrnl_RtlCopyString,                        /* stdcall */
    /* 273 */ [0x111] = &xboxkrnl_RtlCopyUnicodeString,                 /* stdcall */
    /* 274 */ [0x112] = &xboxkrnl_RtlCreateUnicodeString,               /* stdcall */
    /* 275 */ [0x113] = &xboxkrnl_RtlDowncaseUnicodeChar,               /* stdcall */
    /* 276 */ [0x114] = &xboxkrnl_RtlDowncaseUnicodeString,             /* stdcall */
    /* 277 */ [0x115] = &xboxkrnl_RtlEnterCriticalSection,              /* stdcall */
    /* 278 */ [0x116] = &xboxkrnl_RtlEnterCriticalSectionAndRegion,     /* stdcall */
    /* 279 */ [0x117] = &xboxkrnl_RtlEqualString,                       /* stdcall */
    /* 280 */ [0x118] = &xboxkrnl_RtlEqualUnicodeString,                /* stdcall */
    /* 281 */ [0x119] = &xboxkrnl_RtlExtendedIntegerMultiply,           /* stdcall */
    /* 282 */ [0x11a] = &xboxkrnl_RtlExtendedLargeIntegerDivide,        /* stdcall */
    /* 283 */ [0x11b] = &xboxkrnl_RtlExtendedMagicDivide,               /* stdcall */
    /* 284 */ [0x11c] = &xboxkrnl_RtlFillMemory,                        /* stdcall */
    /* 285 */ [0x11d] = &xboxkrnl_RtlFillMemoryUlong,                   /* stdcall */
    /* 286 */ [0x11e] = &xboxkrnl_RtlFreeAnsiString,                    /* stdcall */
    /* 287 */ [0x11f] = &xboxkrnl_RtlFreeUnicodeString,                 /* stdcall */
    /* 288 */ [0x120] = &xboxkrnl_RtlGetCallersAddress,                 /* stdcall */
    /* 289 */ [0x121] = &xboxkrnl_RtlInitAnsiString,                    /* stdcall */
    /* 290 */ [0x122] = &xboxkrnl_RtlInitUnicodeString,                 /* stdcall */
    /* 291 */ [0x123] = &xboxkrnl_RtlInitializeCriticalSection,         /* stdcall */
    /* 292 */ [0x124] = &xboxkrnl_RtlIntegerToChar,                     /* stdcall */
    /* 293 */ [0x125] = &xboxkrnl_RtlIntegerToUnicodeString,            /* stdcall */
    /* 294 */ [0x126] = &xboxkrnl_RtlLeaveCriticalSection,              /* stdcall */
    /* 295 */ [0x127] = &xboxkrnl_RtlLeaveCriticalSectionAndRegion,     /* stdcall */
    /* 296 */ [0x128] = &xboxkrnl_RtlLowerChar,                         /* stdcall */
    /* 297 */ [0x129] = &xboxkrnl_RtlMapGenericMask,                    /* stdcall */
    /* 298 */ [0x12a] = &xboxkrnl_RtlMoveMemory,                        /* stdcall */
    /* 299 */ [0x12b] = &xboxkrnl_RtlMultiByteToUnicodeN,               /* stdcall */
    /* 300 */ [0x12c] = &xboxkrnl_RtlMultiByteToUnicodeSize,            /* stdcall */
    /* 301 */ [0x12d] = &xboxkrnl_RtlNtStatusToDosError,                /* stdcall */
    /* 302 */ [0x12e] = &xboxkrnl_RtlRaiseException,                    /* stdcall */
    /* 303 */ [0x12f] = &xboxkrnl_RtlRaiseStatus,                       /* stdcall */
    /* 352 */ [0x160] = &xboxkrnl_RtlRip,                               /* stdcall */
    /* 361 */ [0x169] = &xboxkrnl_RtlSnprintf,                          /* stdcall */
    /* 362 */ [0x16a] = &xboxkrnl_RtlSprintf,                           /* stdcall */
    /* 304 */ [0x130] = &xboxkrnl_RtlTimeFieldsToTime,                  /* stdcall */
    /* 305 */ [0x131] = &xboxkrnl_RtlTimeToTimeFields,                  /* stdcall */
    /* 306 */ [0x132] = &xboxkrnl_RtlTryEnterCriticalSection,           /* stdcall */
    /* 307 */ [0x133] = &xboxkrnl_RtlUlongByteSwap,                     /* fastcall */
    /* 308 */ [0x134] = &xboxkrnl_RtlUnicodeStringToAnsiString,         /* stdcall */
    /* 309 */ [0x135] = &xboxkrnl_RtlUnicodeStringToInteger,            /* stdcall */
    /* 310 */ [0x136] = &xboxkrnl_RtlUnicodeToMultiByteN,               /* stdcall */
    /* 311 */ [0x137] = &xboxkrnl_RtlUnicodeToMultiByteSize,            /* stdcall */
    /* 312 */ [0x138] = &xboxkrnl_RtlUnwind,                            /* stdcall */
    /* 313 */ [0x139] = &xboxkrnl_RtlUpcaseUnicodeChar,                 /* stdcall */
    /* 314 */ [0x13a] = &xboxkrnl_RtlUpcaseUnicodeString,               /* stdcall */
    /* 315 */ [0x13b] = &xboxkrnl_RtlUpcaseUnicodeToMultiByteN,         /* stdcall */
    /* 316 */ [0x13c] = &xboxkrnl_RtlUpperChar,                         /* stdcall */
    /* 317 */ [0x13d] = &xboxkrnl_RtlUpperString,                       /* stdcall */
    /* 318 */ [0x13e] = &xboxkrnl_RtlUshortByteSwap,                    /* fastcall */
    /* 363 */ [0x16b] = &xboxkrnl_RtlVsnprintf,                         /* stdcall */
    /* 364 */ [0x16c] = &xboxkrnl_RtlVsprintf,                          /* stdcall */
    /* 319 */ [0x13f] = &xboxkrnl_RtlWalkFrameChain,                    /* stdcall */
    /* 320 */ [0x140] = &xboxkrnl_RtlZeroMemory,                        /* stdcall */
#if 0
    /* 354 */ [0x162] = &xboxkrnl_XboxAlternateSignatureKeys,           /* variable */
#endif
#if 0
    /* 321 */ [0x141] = &xboxkrnl_XboxEEPROMKey,                        /* variable */
#endif
#if 1
    /* 322 */ [0x142] = &xboxkrnl_XboxHardwareInfo,                     /* variable */
#endif
#if 0
    /* 323 */ [0x143] = &xboxkrnl_XboxHDKey,                            /* variable */
#endif
#if 0
    /* 353 */ [0x161] = &xboxkrnl_XboxLANKey,                           /* variable */
#endif
#if 0
    /* 324 */ [0x144] = &xboxkrnl_XboxKrnlVersion,                      /* variable */
#endif
#if 0
    /* 325 */ [0x145] = &xboxkrnl_XboxSignatureKey,                     /* variable */
#endif
#if 0
    /* 326 */ [0x146] = &xboxkrnl_XeImageFileName,                      /* variable */
#endif
    /* 327 */ [0x147] = &xboxkrnl_XeLoadSection,                        /* stdcall */
#if 0
    /* 355 */ [0x163] = &xboxkrnl_XePublicKeyData,                      /* variable */
#endif
    /* 328 */ [0x148] = &xboxkrnl_XeUnloadSection,                      /* stdcall */
    /* 329 */ [0x149] = &xboxkrnl_READ_PORT_BUFFER_UCHAR,               /* stdcall */
    /* 330 */ [0x14a] = &xboxkrnl_READ_PORT_BUFFER_USHORT,              /* stdcall */
    /* 331 */ [0x14b] = &xboxkrnl_READ_PORT_BUFFER_ULONG,               /* stdcall */
    /* 332 */ [0x14c] = &xboxkrnl_WRITE_PORT_BUFFER_UCHAR,              /* stdcall */
    /* 333 */ [0x14d] = &xboxkrnl_WRITE_PORT_BUFFER_USHORT,             /* stdcall */
    /* 334 */ [0x14e] = &xboxkrnl_WRITE_PORT_BUFFER_ULONG,              /* stdcall */
    /* 335 */ [0x14f] = &xboxkrnl_XcSHAInit,                            /* stdcall */
    /* 336 */ [0x150] = &xboxkrnl_XcSHAUpdate,                          /* stdcall */
    /* 337 */ [0x151] = &xboxkrnl_XcSHAFinal,                           /* stdcall */
    /* 338 */ [0x152] = &xboxkrnl_XcRC4Key,                             /* stdcall */
    /* 339 */ [0x153] = &xboxkrnl_XcRC4Crypt,                           /* stdcall */
    /* 340 */ [0x154] = &xboxkrnl_XcHMAC,                               /* stdcall */
    /* 341 */ [0x155] = &xboxkrnl_XcPKEncPublic,                        /* stdcall */
    /* 342 */ [0x156] = &xboxkrnl_XcPKDecPrivate,                       /* stdcall */
    /* 343 */ [0x157] = &xboxkrnl_XcPKGetKeyLen,                        /* stdcall */
    /* 344 */ [0x158] = &xboxkrnl_XcVerifyPKCS1Signature,               /* stdcall */
    /* 345 */ [0x159] = &xboxkrnl_XcModExp,                             /* stdcall */
    /* 346 */ [0x15a] = &xboxkrnl_XcDESKeyParity,                       /* stdcall */
    /* 347 */ [0x15b] = &xboxkrnl_XcKeyTable,                           /* stdcall */
    /* 348 */ [0x15c] = &xboxkrnl_XcBlockCrypt,                         /* stdcall */
    /* 349 */ [0x15d] = &xboxkrnl_XcBlockCryptCBC,                      /* stdcall */
    /* 350 */ [0x15e] = &xboxkrnl_XcCryptService,                       /* stdcall */
    /* 351 */ [0x15f] = &xboxkrnl_XcUpdateCrypto,                       /* stdcall */
    /* devkit */
    /* 007 */ [0x007] = &xboxkrnl_DbgLoadImageSymbols,                  /* stdcall */
    /* 011 */ [0x00b] = &xboxkrnl_DbgUnLoadImageSymbols,                /* stdcall */
    /* 374 */ [0x176] = &xboxkrnl_MmDbgAllocateMemory,                  /* stdcall */
    /* 375 */ [0x177] = &xboxkrnl_MmDbgFreeMemory,                      /* stdcall */
    /* 376 */ [0x178] = &xboxkrnl_MmDbgQueryAvailablePages,             /* stdcall */
    /* 377 */ [0x179] = &xboxkrnl_MmDbgReleaseAddress,                  /* stdcall */
    /* 378 */ [0x17a] = &xboxkrnl_MmDbgWriteCheck,                      /* stdcall */
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

static size_t
xboxkrnl_thunk_resolve(register const void **k) {
    register size_t i;
    register uint32_t t;
    ENTER;

    PRINT("/* resolving kernel thunk table ordinals @ %p */", k);

    for (i = 0; k[i]; ++i) {
        t = ((typeof(t) *)k)[i] & 0x1ff;
        k[i] = xboxkrnl_thunk[t];
        PRINT("thunk: %03u (0x%03x): *%p <- %p %s()",
            t,
            t,
            &k[i],
            k[i],
            xboxkrnl_thunk_name[t]);
    }

    PRINT("/* resolved %zu ordinals in kernel thunk table */", i);

    LEAVE;
    return i;
}

