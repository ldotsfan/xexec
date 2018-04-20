/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  Copyright (c) 2017 Michael Saga. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <execinfo.h>
#include <iconv.h>

#include "sw/int128.h"

#define REG08(x)            *(uint8_t  *)((x))
#define REG16(x)            *(uint16_t *)((x))
#define REG32(x)            *(uint32_t *)((x))

#define ARRAY_SIZE(x)       (sizeof(x) / sizeof(*x))
#define ARRAY(x,y,z)        if ((z) < ARRAY_SIZE(y)) (x) = y[(z)]

#define MIN(x,y)            ((x) < (y) ? (x) : (y))
#define MAX(x,y)            ((x) > (y) ? (x) : (y))

#define NAME(x)             [x]   = #x
#define NAMEB(x,y)          [x]   = #y
#define NAMER(x)            [x/4] = #x

#define STRDUMP(x)          VARDUMP5(STRING,x,0,0,0,0)
#define STRDUMP2(x,y)       if (x) VARDUMP5(STRING,x->y,0,0,0,0)
#define STRDUMPW(x)         VARDUMP5(WSTRING,x,0,0,0,0)
#define STRDUMPN(x,y)       VARDUMP5(STRING,x,0,0,0,y)
#define STRDUMPR(x)         VARDUMP5(STRING,&x,0,0,0,sizeof(x))
#define VARDUMP(x,y)        VARDUMP5(x,y,0,0,0,0)
#define VARDUMPN(x,n,y)     xboxkrnl_vardump(xboxkrnl_stack,x,(uint64_t)y,n,0,0,0,0)
#define VARDUMPD(x,y)       if (y) VARDUMP5(x,*y,0,0,0,0)
#define VARDUMPD2(x,y,z)    if (y) VARDUMP5(x,y->z,0,0,0,0)
/* value name w/o bit mask: z = const *char[] name in relation to y; f = 0 */
#define VARDUMP2(x,y,z)     VARDUMP5(x,y,z,ARRAY_SIZE(z),0,0)
#define VARDUMPN2(x,n,y,z)  xboxkrnl_vardump(xboxkrnl_stack,x,(uint64_t)y,n,z,ARRAY_SIZE(z),0,0)
/* value name w/ bit mask:  z == const *char[] index bit masking y;  f = 1 */
#define VARDUMP3(x,y,z)     VARDUMP5(x,y,z,ARRAY_SIZE(z),1,0)
#define VARDUMPN3(x,n,y,z)  xboxkrnl_vardump(xboxkrnl_stack,x,(uint64_t)y,n,z,ARRAY_SIZE(z),1,0)
/* register name:           z = const *char[] name in relation to y; f = 0, r = register size */
#define VARDUMP4(x,y,z)     VARDUMP5(x,y,z,ARRAY_SIZE(z),0,4)
#define VARDUMP5(x,y,z,l,f,r) xboxkrnl_vardump(xboxkrnl_stack,x,(uint64_t)y,#y,z,l,f,r)

enum XBOXKRNL_VARDUMP_TYPE {
    VAR_IN,
    VAR_OUT,
    STUB,
    DUMP,
    STRING,
    WSTRING,
    HEX,
};

static const char *const xboxkrnl_vardump_type_name[] = {
    [VAR_IN]    = "  in",
    [VAR_OUT]   = " out",
    [STUB]      = "stub",
    [DUMP]      = "dump",
    [STRING]    = " str",
    [WSTRING]   = "wstr",
    [HEX]       = " hex",
};

static iconv_t              xboxkrnl_wc_c = (iconv_t)-1;
static iconv_t              xboxkrnl_c_wc = (iconv_t)-1;

static int
xboxkrnl_wchar_to_char(const void *in, char **out, size_t *outlen) {
    register const uint16_t *w;
    register char *b;
    char *buf;
    size_t len;
    size_t inlen;
    register int ret = 0;

    if (xboxkrnl_wc_c == (iconv_t)-1) return ret;

    for (inlen = 0, w = in; *w; ++inlen, ++w);
    if (inlen && (b = malloc(inlen + 1))) {
        b[inlen] = 0;
        buf      = b;
        len      = inlen;
        inlen   *= sizeof(*w);
        if (iconv(xboxkrnl_wc_c, (char **)&in, &inlen, &buf, &len) != (typeof(len))-1) {
            *out = b;
            if (outlen) *outlen = buf - b;
            ret = 1;
        } else {
            free(b);
        }
    }

    return ret;
}

static char *
xboxkrnl_stack_indent(int8_t stack) {
    register size_t sz;
    register char *ret;

    sz  = (stack > 0) ? stack * 4 : 0;
    ret = (sz) ? malloc(sz + 1) : NULL;
    if (ret) memset(ret, ' ', sz), ret[sz] = 0;
    else ret = strdup("");

    return ret;
}

static void
xboxkrnl_vardump(
        int8_t stack,
        int type,
        uint64_t v,
        const char *vname,
        const char *const name[],
        size_t nsz,
        int mask,
        size_t sz) {
    register char *c = NULL;
    register char *s;
    char *a;

    if (!dbg) return;

    if (v >> 32 == ~0UL) v &= ~0UL; /* unwanted sign extension from 32-bit to 64-bit */
    a = *(char **)&v;

    if (nsz) {
        if (!mask) {
            register size_t r = v;
            if (sz) r /= sz;
            if (r < nsz) c = (typeof(c))name[r];
        } else {
            register size_t l, i, len;
            register char *n;
            for (l = 0, i = 0; i <= nsz; ++i) {
                if (i >= nsz) {
                    if (l) c[l] = 0;
                    break;
                }
                if ((n = (typeof(n))name[i]) && v & (1 << i)) {
                    if (l) ++l;
                    len = strlen(n);
                    c = realloc(c, l + len + 2);
                    memcpy(&c[l], n, len);
                    l += len;
                    c[l] = '|';
                }
            }
        }
    }

    s = xboxkrnl_stack_indent(stack);
    if (type == STRING || type == WSTRING) {
        size_t l;
        register int ret = (type == WSTRING && xboxkrnl_wchar_to_char(a, &a, &l));
        if (!ret) l = (sz) ? sz : strlen(a);
        debug("%svar: %s: *%s = '%.*s' (%zu) [0x%llx]",
            s,
            xboxkrnl_vardump_type_name[type],
            vname,
            l,
            a,
            l,
            v);
        if (ret) free(a);
    } else {
        debug("%svar: %s: *%s = 0x%llx (%llu)%s%s%s",
            s,
            xboxkrnl_vardump_type_name[type],
            vname,
            v,
            v,
            (c) ? " (" : "",
            (c) ?    c : "",
            (c) ?  ")" : "");
    }
    free(s);

    if (mask && c) free(c);
}

static void
xboxkrnl_hexdump(int8_t stack, const void *in, uint16_t inlen, const char *var) {
    register const uint8_t *d;
    register char *h;
    register int i;
    char buf[4096];
    char b[67];

    if (!dbg || !inlen) return;

    h = xboxkrnl_stack_indent(stack);
    i = snprintf(buf, sizeof(buf), "%svar: %s: ", h, xboxkrnl_vardump_type_name[HEX]);
    free(h);

    if (i <= 0) return;

    if (var) debug("%sdump: *%s = %p | size: %hu (0x%hx)", buf, var, in, inlen, inlen);
    else debug("%sdump: %p | size: %hu (0x%hx)", buf, in, inlen, inlen);
    debug("%s            0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f  |0123456789abcdef|", buf);

    memset(b, ' ', 66), b[66] = 0;
    for (d = in, h = b; inlen; h = b) {
        for (i = 0; i < 16; ++i) {
            if (inlen) {
                *h++ = "0123456789abcdef"[*d >> 4];
                *h++ = "0123456789abcdef"[*d & 15];
                ++h;
                b[50 + i] = (*d >= ' ' && *d < 0x7f) ? *d : '.';
                ++d;
                --inlen;
            } else {
                *h++ = ' ';
                *h++ = ' ';
                ++h;
                b[50 + i] = 0;
            }
        }
        if ((i = ((void *)d - 16 - in)) & 15) i &= ~15, i += 16;
        debug("%s[0x%.04x]:  %s", buf, i, b);
    }
}

static void *
xboxkrnl_backtrace(void) {
    void *bt[4];

    return (backtrace(bt, 4) == 4) ? bt[3] : NULL;
}

static void
xboxkrnl_enter(int8_t *stack, const char *func, const char *prefix) {
    register void *bt = xboxkrnl_backtrace();
    register char *s;

    s = xboxkrnl_stack_indent(*stack);
    debug("%senter%s%s%s: [%p] --> %s()",
        s,
        (prefix) ?   " (" : "",
        (prefix) ? prefix : "",
        (prefix) ?    ")" : "",
        (bt) ? bt - 6 : bt,
        func);
    free(s);

    if (++*stack & 0xc0) *stack = 0;
}

static void
xboxkrnl_leave(int8_t *stack, const char *func, const char *prefix) {
    register void *bt = xboxkrnl_backtrace();
    register char *s;

    if (--*stack & 0xc0) *stack = 0;

    s = xboxkrnl_stack_indent(*stack);
    debug("%sleave%s%s%s: [%p] <-- %s()",
        s,
        (prefix) ?   " (" : "",
        (prefix) ? prefix : "",
        (prefix) ?    ")" : "",
        bt,
        func);
    free(s);
}

static void
xboxkrnl_print(int8_t stack, const char *format, ...) {
    register char *s;
    register size_t len;
    register size_t n;
    register int ret;
    char buf[4096];
    va_list args;

    if (!dbg) return;

    s   = xboxkrnl_stack_indent(stack);
    n   = sizeof(buf) - 1;
    len = strlen(s);
    strncpy(buf, s, n);
    free(s);

    if (len >= n) return;

    va_start(args, format);
    ret = vsnprintf(&buf[len], n - len, format, args);
    va_end(args);

    if (ret > 0) {
        ret += len;
        buf[ret] = 0;
        debug("%.*s", ret, buf);
    }
}

#define INT3                __asm__ __volatile__("int3")
#define STDCALL             __attribute__((__stdcall__))
#define FASTCALL            __attribute__((__fastcall__))

static void *               xboxkrnl_mem_contiguous = NULL;

#define ENTER               xboxkrnl_enter(&xboxkrnl_stack, __func__, NULL)
#define LEAVE               xboxkrnl_leave(&xboxkrnl_stack, __func__, NULL)
#define PRINT(x,...)        xboxkrnl_print(xboxkrnl_stack, (x), __VA_ARGS__)
#define HEXDUMP(x)          xboxkrnl_hexdump(xboxkrnl_stack, (x), sizeof(x), #x)
#define HEXDUMPN(x,y)       xboxkrnl_hexdump(xboxkrnl_stack, (x), (y), #x)
static int8_t               xboxkrnl_stack = 0;

#define ENTER_DPC           xboxkrnl_enter(&xboxkrnl_dpc_stack, __func__, "dpc")
#define LEAVE_DPC           xboxkrnl_leave(&xboxkrnl_dpc_stack, __func__, "dpc")
#define PRINT_DPC(x,...)    xboxkrnl_print(xboxkrnl_dpc_stack, (x), __VA_ARGS__)
#define HEXDUMP_DPC(x)      xboxkrnl_hexdump(xboxkrnl_dpc_stack, (x), sizeof(x), #x)
#define HEXDUMPN_DPC(x,y)   xboxkrnl_hexdump(xboxkrnl_dpc_stack, (x), (y), #x)
static int8_t               xboxkrnl_dpc_stack = 0;
static pthread_t *          xboxkrnl_dpc_thread = NULL;
static pthread_mutexattr_t *xboxkrnl_dpc_mattr = NULL;
static pthread_mutex_t *    xboxkrnl_dpc_mutex = NULL;
static pthread_cond_t *     xboxkrnl_dpc_cond = NULL;
#define DPC_LOCK            pthread_mutex_lock(xboxkrnl_dpc_mutex)
#define DPC_UNLOCK          pthread_mutex_unlock(xboxkrnl_dpc_mutex)
#define DPC_WAIT            pthread_cond_wait(xboxkrnl_dpc_cond, xboxkrnl_dpc_mutex)
#define DPC_SIGNAL          DPC_LOCK, \
                            pthread_cond_broadcast(xboxkrnl_dpc_cond), \
                            DPC_UNLOCK

static void                 (*xboxkrnl_entry)(void) = NULL;

static pthread_mutex_t      xboxkrnl_tsc_mutex = PTHREAD_MUTEX_INITIALIZER;
static int                  xboxkrnl_tsc_intercept = 0;
static uint128_t            xboxkrnl_tsc_hz = uint128_0;
static uint128_t            xboxkrnl_tsc_scaler = uint128_0;
#define TSC_LOCK            pthread_mutex_lock(&xboxkrnl_tsc_mutex)
#define TSC_UNLOCK          pthread_mutex_unlock(&xboxkrnl_tsc_mutex)

int
xboxkrnl_tsc_on(void) {
    return (xboxkrnl_tsc_intercept) ? prctl(PR_SET_TSC, PR_TSC_ENABLE) : -1;
}

int
xboxkrnl_tsc_off(void) {
    return (xboxkrnl_tsc_intercept) ? prctl(PR_SET_TSC, PR_TSC_SIGSEGV) : -1;
}

int
xboxkrnl_tsc_init(void) {
    char buf[4096];
    uint32_t mhz;
    uint32_t khz;
    uint128_t host;
    uint128_t freq;
    register int fd = -1;
    register int tmp;
    register char *x;
    register char *y;
    register int ret = 1;
    ENTER;

    do {
        xboxkrnl_tsc_intercept = 1;
        if (xboxkrnl_tsc_off() < 0) {
            PRINT("error: failed to intercept TSC register: prctl(): '%s'", strerror(errno));
            break;
        }
        xboxkrnl_tsc_on();
        if ((fd = open("/proc/cpuinfo", O_RDONLY)) < 0) {
            PRINT("error: failed to query TSC frequency: open(): '%s'", strerror(errno));
            break;
        }
        if ((tmp = read(fd, buf, sizeof(buf))) <= 0) {
            PRINT("error: failed to query TSC frequency: read(): '%s'", (!tmp) ? "empty read" : strerror(errno));
            break;
        }
        if (!(x = strstr(buf, "cpu MHz")) ||
            !(y = strchr(x, '\n')) ||
            (*y = 0) ||
            !(x = strrchr(x, ' ')) ||
            sscanf(x + 1, "%u.%03u", &mhz, &khz) != 2) {
            PRINT("error: failed to query TSC frequency: string not found", 0);
            break;
        }
        PRINT("/* TSC frequency is %u.%.03u MHz */", mhz, khz);
        if (mhz < 733 || (mhz == 733 && khz <= 333)) break;
        uint128_set(xboxkrnl_tsc_hz, mhz * 1000000 + khz * 1000);
        VARDUMP(DUMP, xboxkrnl_tsc_hz.lo);
    } while ((ret = 0));

    if (fd >= 0) close(fd);

    if (ret) {
        xboxkrnl_tsc_intercept = 0;
        PRINT("/* TSC scaling is disabled */", 0);
    } else {
        xboxkrnl_tsc_intercept = 1;
        PRINT("/* TSC scaling is enabled */", 0);
        uint128_set(host, 1000000000);
        uint128_set(freq, 733333333);
        host = uint128_mul(&host, &xboxkrnl_tsc_hz);
        uint128_divmod(&host, &freq, &xboxkrnl_tsc_scaler, NULL);
        PRINT("/* TSC scaling factor: %llu.%.09llu per 733 MHz tick */",
            xboxkrnl_tsc_scaler.lo / 1000000000,
            xboxkrnl_tsc_scaler.lo % 1000000000);
    }

    LEAVE;
    return ret;
}

void
xboxkrnl_tsc(uint32_t *lo, uint32_t *hi, uint64_t *v, int xbox) {
    uint128_t host;
    uint128_t tick;
    register uint32_t a;
    register uint32_t d;

    TSC_LOCK;

    xboxkrnl_tsc_on();
    __asm__ __volatile__("rdtsc" : "=a" (a), "=d" (d));
    xboxkrnl_tsc_off();

    if (xbox && xboxkrnl_tsc_scaler.lo) {
        uint128_set(host, 1000000000);
        uint128_set(tick, ((uint64_t)d << 32) | a);
        host = uint128_mul(&host, &tick);
        uint128_divmod(&host, &xboxkrnl_tsc_scaler, &tick, NULL);
        if (tick.hi) {
            fprintf(stderr, "xboxkrnl: FATAL: TSC scaler overflow\n");
            _exit(-1);
        }
        if (lo) *lo = tick.lo & 0xffffffff;
        if (hi) *hi = tick.lo >> 32;
        if (v)  *v  = tick.lo;
    } else {
        if (lo) *lo = a;
        if (hi) *hi = d;
        if (v)  *v  = ((uint64_t)d << 32) | a;
    }

    TSC_UNLOCK;
}

void
xboxkrnl_clock_local(struct timeval *tv) {
    TSC_LOCK;

    if (xboxkrnl_entry) xboxkrnl_tsc_on();
    gettimeofday(tv, NULL);
    if (xboxkrnl_entry) xboxkrnl_tsc_off();

    TSC_UNLOCK;
}

void
xboxkrnl_clock_wall(struct timespec *tp) {
    TSC_LOCK;

    if (xboxkrnl_entry) xboxkrnl_tsc_on();
    clock_gettime(CLOCK_MONOTONIC, tp);
    if (xboxkrnl_entry) xboxkrnl_tsc_off();

    TSC_UNLOCK;
}

void *
xboxkrnl_entry_thread(void *arg) {
    if (arg) {
        xboxkrnl_entry = arg;
        xboxkrnl_tsc_off();
        xboxkrnl_entry();
    }

    pthread_exit(NULL);
    return NULL;
}

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
    uint8_t                 Inserted;
    uint8_t                 Padding;
    xboxkrnl_listentry *    DpcListEntry;
    void *                  DeferredRoutine;
    void *                  DeferredContext;
    void *                  SystemArgument1;
    void *                  SystemArgument2;
} PACKED xboxkrnl_dpc;

static xboxkrnl_dpc **      xboxkrnl_dpc_list = NULL;
static size_t               xboxkrnl_dpc_list_sz = 0;
static pthread_mutex_t      xboxkrnl_dpc_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#define DPC_LIST_LOCK       pthread_mutex_lock(&xboxkrnl_dpc_list_mutex)
#define DPC_LIST_UNLOCK     pthread_mutex_unlock(&xboxkrnl_dpc_list_mutex)

#define ENTER_EVENT         xboxkrnl_enter(&xboxkrnl_event_stack, __func__, "event")
#define LEAVE_EVENT         xboxkrnl_leave(&xboxkrnl_event_stack, __func__, "event")
#define PRINT_EVENT(x,...)  xboxkrnl_print(xboxkrnl_event_stack, (x), __VA_ARGS__)
#define HEXDUMP_EVENT(x)    xboxkrnl_hexdump(xboxkrnl_event_stack, (x), sizeof(x), #x)
#define HEXDUMPN_EVENT(x,y) xboxkrnl_hexdump(xboxkrnl_event_stack, (x), (y), #x)
static int8_t               xboxkrnl_event_stack = 0;
static pthread_t *          xboxkrnl_event_thread = NULL;
static pthread_mutexattr_t *xboxkrnl_event_mattr = NULL;
static pthread_mutex_t *    xboxkrnl_event_mutex = NULL;
static pthread_cond_t *     xboxkrnl_event_cond = NULL;
#define EVENT_LOCK          pthread_mutex_lock(xboxkrnl_event_mutex)
#define EVENT_UNLOCK        pthread_mutex_unlock(xboxkrnl_event_mutex)
#define EVENT_WAIT          pthread_cond_wait(xboxkrnl_event_cond, xboxkrnl_event_mutex)
#define EVENT_SIGNAL        EVENT_LOCK, \
                            pthread_cond_broadcast(xboxkrnl_event_cond), \
                            EVENT_UNLOCK

#define ENTER_IRQ_NV2A      xboxkrnl_enter(&xboxkrnl_irq_nv2a_stack, __func__, "irq_nv2a")
#define LEAVE_IRQ_NV2A      xboxkrnl_leave(&xboxkrnl_irq_nv2a_stack, __func__, "irq_nv2a")
#define PRINT_IRQ_NV2A(x,...) xboxkrnl_print(xboxkrnl_irq_nv2a_stack, (x), __VA_ARGS__)
#define HEXDUMP_IRQ_NV2A(x) xboxkrnl_hexdump(xboxkrnl_irq_nv2a_stack, (x), sizeof(x), #x)
#define HEXDUMPN_IRQ_NV2A(x,y) xboxkrnl_hexdump(xboxkrnl_irq_nv2a_stack, (x), (y), #x)
static int8_t               xboxkrnl_irq_nv2a_stack = 0;
static pthread_t *          xboxkrnl_irq_nv2a_thread = NULL;
static pthread_mutexattr_t *xboxkrnl_irq_nv2a_mattr = NULL;
static pthread_mutex_t *    xboxkrnl_irq_nv2a_mutex = NULL;
static pthread_cond_t *     xboxkrnl_irq_nv2a_cond = NULL;
#define IRQ_NV2A_LOCK       pthread_mutex_lock(xboxkrnl_irq_nv2a_mutex)
#define IRQ_NV2A_UNLOCK     pthread_mutex_unlock(xboxkrnl_irq_nv2a_mutex)
#define IRQ_NV2A_WAIT       pthread_cond_wait(xboxkrnl_irq_nv2a_cond, xboxkrnl_irq_nv2a_mutex)
#define IRQ_NV2A_SIGNAL     IRQ_NV2A_LOCK, \
                            pthread_cond_broadcast(xboxkrnl_irq_nv2a_cond), \
                            IRQ_NV2A_UNLOCK

typedef struct {
    uint8_t                 Type;
    uint8_t                 Absolute;
    uint8_t                 Size;
    uint8_t                 Inserted;
    int32_t                 SignalState;
    xboxkrnl_listentry      WaitListHead;
} PACKED xboxkrnl_dispatcher;

typedef struct {
    xboxkrnl_dispatcher     Header;
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

    const uint16_t          ioreg0_base;
    const uint16_t          ioreg0_size;
    const uint16_t          ioreg1_base;
    const uint16_t          ioreg1_size;
    const uint16_t          ioreg2_base;
    const uint16_t          ioreg2_size;
    const uint16_t          ioreg3_base;
    const uint16_t          ioreg3_size;
    const uint16_t          ioreg4_base;
    const uint16_t          ioreg4_size;

    const uint32_t          memreg_base;
    const uint32_t          memreg_size;

    void *                  memreg;

    void *                  irq_kinterrupt;
    int                     irq_connected;
    int                     irq_level;
    int                     irq_busy;
    void *                  irq_isr_routine;
    void *                  irq_isr_context;
};

enum {
    XPCI_HOSTBRIDGE =   0,
    XPCI_LPCBRIDGE, /*  1 */
    XPCI_SMBUS,     /*  2 */
    XPCI_USB0,      /*  3 */
    XPCI_USB1,      /*  4 */
    XPCI_NIC,       /*  5 */
    XPCI_APU,       /*  6 */
    XPCI_ACI,       /*  7 */
    XPCI_IDE,       /*  8 */
    XPCI_AGPBRIDGE, /*  9 */
    XPCI_GPU,       /* 10 */
    XPCI_INTERNAL   /* 11 */
};

static struct xpci xpci[] = {
    [XPCI_HOSTBRIDGE] = {
        .name        = "HOSTBRIDGE",
        .irq         = -1,
        .memreg_base = 0x80000000,
        .memreg_size = 0x08000000,
    },
    [XPCI_LPCBRIDGE] = {
        .name        = "LPCBRIDGE",
        .bus         = 0,
        .device      = 1,
        .function    = 0,
        .irq         = -1,
        .ioreg0_base = 0x8000,
        .ioreg0_size = 0x0100,
    },
    [XPCI_SMBUS] = {
        .name        = "SMBUS",
        .bus         = 0,
        .device      = 1,
        .function    = 1,
        .irq         = -1,
        .ioreg1_base = 0xc000,
        .ioreg1_size = 0x0010,
        .ioreg2_base = 0xc200,
        .ioreg2_size = 0x0020,
    },
    [XPCI_USB0] = {
        .name        = "USB0",
        .bus         = 0,
        .device      = 2,
        .function    = 0,
        .irq         = 1,
        .memreg_base = 0xfed00000,
        .memreg_size = 0x00001000,
    },
    [XPCI_USB1] = {
        .name        = "USB1",
        .bus         = 0,
        .device      = 3,
        .function    = 0,
        .irq         = 9,
        .memreg_base = 0xfed08000,
        .memreg_size = 0x00001000,
    },
    [XPCI_NIC] = {
        .name        = "NIC",
        .bus         = 0,
        .device      = 4,
        .function    = 0,
        .irq         = 4,
        .ioreg1_base = 0xe000,
        .ioreg1_size = 0x0008,
        .memreg_base = 0xfef00000,
        .memreg_size = 0x00000400,
    },
    [XPCI_APU] = {
        .name        = "APU",
        .bus         = 0,
        .device      = 5,
        .function    = 0,
        .irq         = 5,
        .memreg_base = 0xfe800000,
        .memreg_size = 0x00080000,
    },
    [XPCI_ACI] = {
        .name        = "ACI",
        .bus         = 0,
        .device      = 6,
        .function    = 0,
        .irq         = 6,
        .ioreg0_base = 0xd000,
        .ioreg0_size = 0x0100,
        .ioreg1_base = 0xd200,
        .ioreg1_size = 0x0080,
        .memreg_base = 0xfec00000,
        .memreg_size = 0x00001000,
    },
    [XPCI_IDE] = {
        .name        = "IDE",
        .bus         = 0,
        .device      = 9,
        .function    = 0,
        .irq         = 14,
        .ioreg4_base = 0xff60,
        .ioreg4_size = 0x0010,
    },
    [XPCI_AGPBRIDGE] = {
        .name        = "AGPBRIDGE",
        .bus         = 0,
        .device      = 30,
        .function    = 0,
        .irq         = -1,
    },
    [XPCI_GPU] = {
        .name        = "GPU",
        .bus         = 1,
        .device      = 0,
        .function    = 0,
        .irq         = 3,
        .memreg_base = 0xfd000000,
        .memreg_size = 0x01000000,
    },
    [XPCI_INTERNAL] = {
        .irq         = -1,
    },
};

static struct xpci *const   host = &xpci[XPCI_HOSTBRIDGE];
static struct xpci *const   usb0 = &xpci[XPCI_USB0];
static struct xpci *const   usb1 = &xpci[XPCI_USB1];
static struct xpci *const   apu  = &xpci[XPCI_APU];
static struct xpci *const   aci  = &xpci[XPCI_ACI];
static struct xpci *const   nv2a = &xpci[XPCI_GPU];

/* memory */
static void *               xboxkrnl_gpuinstmem = NULL;//FIXME
static size_t               xboxkrnl_gpuinstsz = 0;

enum XBOXKRNL_PAGE_TYPE {
    XBOXKRNL_PAGE_NOACCESS                  = 1 << 0,   /* 0x1 */
    XBOXKRNL_PAGE_READONLY                  = 1 << 1,   /* 0x2 */
    XBOXKRNL_PAGE_READWRITE                 = 1 << 2,   /* 0x4 */
    XBOXKRNL_PAGE_WRITECOPY                 = 1 << 3,   /* 0x8 */
    XBOXKRNL_PAGE_EXECUTE                   = 1 << 4,   /* 0x10 */
    XBOXKRNL_PAGE_EXECUTE_READ              = 1 << 5,   /* 0x20 */
    XBOXKRNL_PAGE_EXECUTE_READWRITE         = 1 << 6,   /* 0x40 */
    XBOXKRNL_PAGE_EXECUTE_WRITECOPY         = 1 << 7,   /* 0x80 */
    XBOXKRNL_PAGE_GUARD                     = 1 << 8,   /* 0x100 */
    XBOXKRNL_PAGE_NOCACHE                   = 1 << 9,   /* 0x200 */
    XBOXKRNL_PAGE_WRITECOMBINE              = 1 << 10,  /* 0x400 */
};

static const char *const xboxkrnl_page_type_name[] = {
    NAMEB(0,  XBOXKRNL_PAGE_NOACCESS),
    NAMEB(1,  XBOXKRNL_PAGE_READONLY),
    NAMEB(2,  XBOXKRNL_PAGE_READWRITE),
    NAMEB(3,  XBOXKRNL_PAGE_WRITECOPY),
    NAMEB(4,  XBOXKRNL_PAGE_EXECUTE),
    NAMEB(5,  XBOXKRNL_PAGE_EXECUTE_READ),
    NAMEB(6,  XBOXKRNL_PAGE_EXECUTE_READWRITE),
    NAMEB(7,  XBOXKRNL_PAGE_EXECUTE_WRITECOPY),
    NAMEB(8,  XBOXKRNL_PAGE_GUARD),
    NAMEB(9,  XBOXKRNL_PAGE_NOCACHE),
    NAMEB(10, XBOXKRNL_PAGE_WRITECOMBINE),
};

enum XBOXKRNL_MEM_TYPE {
    XBOXKRNL_MEM_COMMIT                     = 1 << 12,  /* 0x1000 */
    XBOXKRNL_MEM_RESERVE                    = 1 << 13,  /* 0x2000 */
    XBOXKRNL_MEM_DECOMMIT                   = 1 << 14,  /* 0x4000 */
    XBOXKRNL_MEM_RELEASE                    = 1 << 15,  /* 0x8000 */
    XBOXKRNL_MEM_FREE                       = 1 << 16,  /* 0x10000 */
    XBOXKRNL_MEM_PRIVATE                    = 1 << 17,  /* 0x20000 */
    XBOXKRNL_MEM_MAPPED                     = 1 << 18,  /* 0x40000 */
    XBOXKRNL_MEM_RESET                      = 1 << 19,  /* 0x80000 */
    XBOXKRNL_MEM_TOP_DOWN                   = 1 << 20,  /* 0x100000 */
    XBOXKRNL_MEM_NOZERO                     = 1 << 23,  /* 0x800000 */
    XBOXKRNL_MEM_LARGE_PAGES                = 1 << 29,  /* 0x20000000 */
    XBOXKRNL_MEM_4MB_PAGES                  = 1 << 31,  /* 0x80000000 */
};

static const char *const xboxkrnl_mem_type_name[] = {
    NAMEB(12, XBOXKRNL_MEM_COMMIT),
    NAMEB(13, XBOXKRNL_MEM_RESERVE),
    NAMEB(14, XBOXKRNL_MEM_DECOMMIT),
    NAMEB(15, XBOXKRNL_MEM_RELEASE),
    NAMEB(16, XBOXKRNL_MEM_FREE),
    NAMEB(17, XBOXKRNL_MEM_PRIVATE),
    NAMEB(18, XBOXKRNL_MEM_MAPPED),
    NAMEB(19, XBOXKRNL_MEM_RESET),
    NAMEB(20, XBOXKRNL_MEM_TOP_DOWN),
    NAMEB(23, XBOXKRNL_MEM_NOZERO),
    NAMEB(29, XBOXKRNL_MEM_LARGE_PAGES),
    NAMEB(31, XBOXKRNL_MEM_4MB_PAGES),
};

typedef struct {
    void *                  BaseAddress;
    void *                  AllocationBase;
    int32_t                 AllocationProtect;
    size_t                  RegionSize;
    int32_t                 State;
    int32_t                 Protect;
    int32_t                 Type;
} PACKED xboxkrnl_mem_basic;

typedef struct {
    void *                  BaseAddress;
    void *                  AllocationBase;
    int32_t                 AllocationProtect;
    size_t                  RegionSize;
    int32_t                 State;
    int32_t                 Protect;
    int32_t                 Type;
    /* internal */
    int                     index;
    int                     prot;
    int                     flags;
    off_t                   offset;
    int                     dirty;
    void *                  parent;     /* to write-protect parent page allocation for a dirty write catch */
} PACKED xboxkrnl_mem;

/* page tables */
typedef struct {
    int                     index;
    const char *            name;
    xboxkrnl_mem *          mem;
    ssize_t                 sz;
} PACKED xboxkrnl_mem_table;

enum MEM_TABLE_INDEX {
    MEM_DIRTY       =  0,
    MEM_EXEC,       /* 1 */
    MEM_HEAP,       /* 2 */
    MEM_MAP,        /* 3 */
    MEM_ALLOC,      /* 4 */
    MEM_RESERVE,    /* 5 */
};

static xboxkrnl_mem_table xboxkrnl_mem_tables[] = {
#define MEM_TABLE(x) \
    { .index = MEM_ ## x, .name = #x }
    MEM_TABLE(DIRTY),               /* dirty page tracking */
    MEM_TABLE(EXEC),                /* host executable virtual memory */
    MEM_TABLE(HEAP),                /* guest virtual memory heap; trackable parent page */
    MEM_TABLE(MAP),                 /* guest virtual memory map; trackable parent page */
    MEM_TABLE(ALLOC),               /* host virtual memory allocation; trackable parent page */
    MEM_TABLE(RESERVE),             /* host virtual memory reserve */
#undef MEM_TABLE
    { .index = -1 }
};

#define MEM_LOCK                    pthread_mutex_lock(&xboxkrnl_mem_mutex)
#define MEM_UNLOCK                  pthread_mutex_unlock(&xboxkrnl_mem_mutex)
#define MEM_RELEASE(x)              xboxkrnl_mem_release((x))
#define MEM_PUSH(x,y)               xboxkrnl_mem_push((x), (y), 0)
#define MEM_PUSH_LOCKED(x,y)        xboxkrnl_mem_push((x), (y), 1)
#define MEM_POP(x,y)                xboxkrnl_mem_pop((x), (void *)(y), 0)
#define MEM_POP_LOCKED(x,y)         xboxkrnl_mem_pop((x), (void *)(y), 1)
#define MEM_RET(x,y)                xboxkrnl_mem_ret((x), (void *)(y))
#define MEM_RET_LOCKED(x,y)         xboxkrnl_mem_ret_locked((x), (void *)(y), NULL)
#define MEM_RET_LOCKED2(x,y)        xboxkrnl_mem_ret_locked((x), NULL, (void *)(y))
#define MEM_DIRTY_PARENT_RET_LOCKED(x) xboxkrnl_mem_dirty_parent_ret_locked((void *)(x))
#define MEM_DIRTY_ENABLE(x,y)       xboxkrnl_mem_dirty_enable((void *)(x), (y))
#define MEM_DIRTY_DISABLE(x)        xboxkrnl_mem_dirty_disable((void *)(x))
#define MEM_DIRTY_ON(x)             xboxkrnl_mem_dirty_on((void *)(x))
#define MEM_DIRTY_ON_LOCKED(x)      xboxkrnl_mem_dirty_on_locked((void *)(x), NULL)
#define MEM_DIRTY_ON_LOCKED2(x)     xboxkrnl_mem_dirty_on_locked(NULL, (x))
#define MEM_DIRTY_OFF(x)            xboxkrnl_mem_dirty_off((void *)(x))
#define MEM_DIRTY_OFF_LOCKED(x)     xboxkrnl_mem_dirty_off_locked((void *)(x), NULL)
#define MEM_DIRTY_OFF_LOCKED2(x)    xboxkrnl_mem_dirty_off_locked(NULL, (x))
#define MEM_DIRTY_ON_MPROTECT(x)    if (mprotect((x)->AllocationBase, (x)->RegionSize, (x)->prot)) INT3
#define MEM_DIRTY_OFF_MPROTECT(x)   if (mprotect((x)->AllocationBase, (x)->RegionSize, (x)->prot & ~PROT_WRITE)) INT3
#define MEM_DIRTY_ON_LOCKED_PRE(x,y) do { \
            if (!MEM_DIRTY_ON_LOCKED((y)) && ((x) = MEM_DIRTY_PARENT_RET_LOCKED((y))) && !(x)->dirty) { \
                MEM_DIRTY_ON_MPROTECT((x)); \
            } else { \
                (x) = NULL; \
            } \
        } while (0)
#define MEM_DIRTY_ON_LOCKED_POST(x) if ((x)) MEM_DIRTY_OFF_MPROTECT((x))
#define MEM_DIRTY_GET(x)            xboxkrnl_mem_dirty_get((void *)(x))

static pthread_mutex_t              xboxkrnl_mem_mutex = PTHREAD_MUTEX_INITIALIZER;
xboxkrnl_mem *                      xboxkrnl_mem_dirty_parent_ret_locked(void *addr);
void                                xboxkrnl_mem_release(int index);
static void                         xboxkrnl_mem_push(int index, const xboxkrnl_mem *in, int locked);
static void                         xboxkrnl_mem_pop(int index, void *addr, int locked);
static xboxkrnl_mem *               xboxkrnl_mem_ret_locked(int index, void *addr, void *parent);
static xboxkrnl_mem                 xboxkrnl_mem_ret(int index, void *addr);
int                                 xboxkrnl_mem_dirty_enable(void *addr, size_t len);
void                                xboxkrnl_mem_dirty_disable(void *addr);
int                                 xboxkrnl_mem_dirty_on_locked(void *addr, xboxkrnl_mem *m);
int                                 xboxkrnl_mem_dirty_off_locked(void *addr, xboxkrnl_mem *m);
int                                 xboxkrnl_mem_dirty_on(void *addr);
int                                 xboxkrnl_mem_dirty_off(void *addr);
int                                 xboxkrnl_mem_dirty_get(void *addr);

static int
xboxkrnl_mem_prot_unix_to_winnt(int prot, int flags) {
    register int ret;

    if (!(prot & PROT_EXEC) &&
        !(prot & PROT_READ) &&
        !(prot & PROT_WRITE)) return XBOXKRNL_PAGE_NOACCESS /* 0x1 */;

    ret = (flags & MAP_NORESERVE) ? XBOXKRNL_PAGE_NOCACHE /* 0x200 */ : 0;

    if (!(prot & PROT_EXEC) &&
        prot & PROT_READ &&
        !(prot & PROT_WRITE)) return ret | XBOXKRNL_PAGE_READONLY /* 0x2 */;
    if (!(prot & PROT_EXEC) &&
        prot & PROT_READ &&
        prot & PROT_WRITE) return ret | XBOXKRNL_PAGE_READWRITE /* 0x4 */;
    if (prot & PROT_EXEC &&
        !(prot & PROT_READ) &&
        !(prot & PROT_WRITE)) return ret | XBOXKRNL_PAGE_EXECUTE /* 0x10 */;
    if (prot & PROT_EXEC &&
        prot & PROT_READ &&
        !(prot & PROT_WRITE)) return ret | XBOXKRNL_PAGE_EXECUTE_READ /* 0x20 */;
    if (prot & PROT_EXEC &&
        prot & PROT_READ &&
        prot & PROT_WRITE) return ret | XBOXKRNL_PAGE_EXECUTE_READWRITE /* 0x40 */;

    return 0;
}

xboxkrnl_mem *
xboxkrnl_mem_dirty_parent_ret_locked(void *addr) {
    register xboxkrnl_mem *m;

    do {
        if ((m = MEM_RET_LOCKED(MEM_HEAP, addr)) ||
            (m = MEM_RET_LOCKED(MEM_MAP, addr)) ||
            (m = MEM_RET_LOCKED(MEM_ALLOC, addr))) break;
    } while (0);

    return m;
}

void
xboxkrnl_mem_release(int index) {
    register xboxkrnl_mem_table *t = &xboxkrnl_mem_tables[index];

    MEM_LOCK;

    free(t->mem);
    t->mem = NULL;
    t->sz  = 0;

    MEM_UNLOCK;
}

static void
xboxkrnl_mem_push(int index, const xboxkrnl_mem *in, int locked) {
    register xboxkrnl_mem_table *t = &xboxkrnl_mem_tables[index];
    register xboxkrnl_mem *m;

    if (!locked) MEM_LOCK;

    if (!(t->mem = realloc(t->mem, sizeof(*t->mem) * ++t->sz))) INT3;
    m  = &t->mem[t->sz - 1];
    *m = *in;

    m->dirty = 1;
    if (m->parent) MEM_DIRTY_OFF_LOCKED2(m);

    if (!locked) MEM_UNLOCK;
}

static void
xboxkrnl_mem_pop(int index, void *addr, int locked) {
    register xboxkrnl_mem_table *t = &xboxkrnl_mem_tables[index];
    register xboxkrnl_mem *m;
    xboxkrnl_mem mem;
    register ssize_t i;
    register int ret = 0;

    if (!locked) MEM_LOCK;

    if (t->mem) {
        for (i = t->sz - 1; i >= 0; --i) {
            m = &t->mem[i];
            if (m->BaseAddress == addr) {
                mem = *m;
                ++i;
                if (i < t->sz) memmove(m, m + 1, sizeof(*t->mem) * (t->sz - i));
                --t->sz;
                ret = 1;
                break;
            }
        }
    }

    if (ret) {
        if (mem.parent) {
            for (i = t->sz - 1; i >= 0; --i) {
                m = &t->mem[i];
                if (m->parent == mem.parent && !m->dirty) break;
                if (!i) {
                    if ((m = MEM_DIRTY_PARENT_RET_LOCKED(mem.parent)) && !m->dirty) {
                        m->dirty = 1;
                        MEM_DIRTY_ON_MPROTECT(m);
                    }
                    break;
                }
            }
        } else {
            while ((m = xboxkrnl_mem_ret_locked(MEM_DIRTY, NULL, mem.BaseAddress))) {
                MEM_POP_LOCKED(MEM_DIRTY, m->BaseAddress);
            }
        }
    }

    if (!locked) MEM_UNLOCK;
}

static xboxkrnl_mem *
xboxkrnl_mem_ret_locked(int index, void *addr, void *parent) {
    register xboxkrnl_mem_table *t = &xboxkrnl_mem_tables[index];
    register xboxkrnl_mem *m;
    register ssize_t i;
    register xboxkrnl_mem *ret = NULL;

    if (t->mem) {
        for (i = t->sz - 1; i >= 0; --i) {
            m = &t->mem[i];
            if (parent) {
                if (m->parent == parent) {
                    ret = m;
                    break;
                }
            } else {
                if (addr >= m->BaseAddress && addr < m->BaseAddress + m->RegionSize) {
                    ret = m;
                    break;
                }
            }
        }
    }

    return ret;
}

static xboxkrnl_mem
xboxkrnl_mem_ret(int index, void *addr) {
    register xboxkrnl_mem *m;
    xboxkrnl_mem ret = { };

    MEM_LOCK;

    if ((m = MEM_RET_LOCKED(index, addr))) ret = *m;

    MEM_UNLOCK;

    return ret;
}

int
xboxkrnl_mem_dirty_enable(void *addr, size_t len) {
    register xboxkrnl_mem *m;
    xboxkrnl_mem mem;
    register int ret = 1;

    MEM_LOCK;

    do {
        if (MEM_RET_LOCKED(MEM_DIRTY, addr)) break;
        if (!(m = MEM_DIRTY_PARENT_RET_LOCKED(addr))) break;
        mem              = *m;
        mem.parent       = mem.BaseAddress;
        mem.BaseAddress  = addr;
        mem.RegionSize   = len;
        mem.dirty        = 0;
        MEM_PUSH_LOCKED(MEM_DIRTY, &mem);
    } while ((ret = 0));

    MEM_UNLOCK;

    return ret;
}

void
xboxkrnl_mem_dirty_disable(void *addr) {
    MEM_POP(MEM_DIRTY, addr);
}

int
xboxkrnl_mem_dirty_on_locked(void *addr, xboxkrnl_mem *m) {
    register xboxkrnl_mem_table *t;
    register xboxkrnl_mem *p;
    register ssize_t i;
    register int ret = 0;

    if ((m || (m = MEM_RET_LOCKED(MEM_DIRTY, addr)))) {
        if (!m->dirty) {
            m->dirty = 1;
            PRINT("MEM: dirty: 0x%.08x -> [0x%.08x+0x%.08x] (%lu)",
                addr,
                m->BaseAddress,
                m->RegionSize,
                m->RegionSize);//TODO loglevel
            if (m->parent) {
                t = &xboxkrnl_mem_tables[MEM_DIRTY];
                for (i = t->sz - 1; i >= 0; --i) {
                    p = &t->mem[i];
                    if (p != m && p->parent == m->parent && !p->dirty) break;
                    if (!i) {
                        if ((p = MEM_DIRTY_PARENT_RET_LOCKED(m->parent)) && !p->dirty) {
                            p->dirty = 1;
                            PRINT("MEM: dirty: 0x%.08x -> [0x%.08x+0x%.08x] (%lu) (parent)",
                                addr,
                                p->BaseAddress,
                                p->RegionSize,
                                p->RegionSize);//TODO loglevel
                            MEM_DIRTY_ON_MPROTECT(p);
                            ++ret;
                        }
                        break;
                    }
                }
            }
        }
        ++ret;
    }

    return ret;
}

int
xboxkrnl_mem_dirty_off_locked(void *addr, xboxkrnl_mem *m) {
    register xboxkrnl_mem *p;
    register int ret = 0;

    if ((m || (m = MEM_RET_LOCKED(MEM_DIRTY, addr)))) {
        if (m->dirty) {
            m->dirty = 0;
            if (m->parent && (p = MEM_DIRTY_PARENT_RET_LOCKED(m->parent)) && p->dirty) {
                p->dirty = 0;
                MEM_DIRTY_OFF_MPROTECT(p);
                ++ret;
            }
        }
        ++ret;
    }

    return ret;
}

int
xboxkrnl_mem_dirty_on(void *addr) {
    register int ret;

    MEM_LOCK;

    ret = MEM_DIRTY_ON_LOCKED(addr);

    MEM_UNLOCK;

    return ret;
}

int
xboxkrnl_mem_dirty_off(void *addr) {
    register int ret;

    MEM_LOCK;

    ret = MEM_DIRTY_OFF_LOCKED(addr);

    MEM_UNLOCK;

    return ret;
}

int
xboxkrnl_mem_dirty_get(void *addr) {
    register xboxkrnl_mem *m;
    register int ret = -1;

    MEM_LOCK;

    if ((m = MEM_RET_LOCKED(MEM_DIRTY, addr))) ret = m->dirty;

    MEM_UNLOCK;

    return ret;
}

static void *
xboxkrnl_mmap(int index, void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    xboxkrnl_mem m = { };
    register void *ret;

    if (index != MEM_EXEC) prot &= ~PROT_EXEC;

    if ((ret = mmap(addr, length, prot, flags, fd, offset)) != MAP_FAILED) {
        m.BaseAddress       = ret;
        m.AllocationBase    = ret;
        m.AllocationProtect = xboxkrnl_mem_prot_unix_to_winnt(prot, flags);
        m.RegionSize        = length;
        m.State             = XBOXKRNL_MEM_COMMIT /* 0x1000 */;
        m.Protect           = m.AllocationProtect;
        m.Type              = ((flags & MAP_PRIVATE) ? XBOXKRNL_MEM_PRIVATE /* 0x20000 */ : 0) | XBOXKRNL_MEM_MAPPED /* 0x40000 */;
        m.prot              = prot;
        m.flags             = flags;
        m.offset            = offset;
        m.index             = index;
        MEM_PUSH(index, &m);
        MEM_DIRTY_ENABLE(ret, length);
    }

    return ret;
}

static int
xboxkrnl_munmap(void *addr, size_t length) {
    xboxkrnl_mem m = MEM_RET(MEM_MAP, addr);
    register int ret;

    if (!length) length = m.RegionSize;

    if (!(ret = munmap(addr, length))) MEM_POP(MEM_MAP, addr);

    return ret;
}

static void *
xboxkrnl_malloc(size_t size) {
    xboxkrnl_mem m = { };
    register void *ret;

    if ((ret = malloc(size))) {
        m.BaseAddress       = ret;
        m.AllocationBase    = ret;
        m.AllocationProtect = XBOXKRNL_PAGE_READWRITE /* 0x4 */;
        m.RegionSize        = size;
        m.State             = XBOXKRNL_MEM_COMMIT /* 0x1000 */;
        m.Protect           = m.AllocationProtect;
        m.Type              = XBOXKRNL_MEM_PRIVATE /* 0x20000 */;
        MEM_PUSH(MEM_HEAP, &m);
    }

    return ret;
}

static void *
xboxkrnl_calloc(size_t nmemb, size_t size) {
    xboxkrnl_mem m = { };
    register void *ret;

    if ((ret = calloc(nmemb, size))) {
        m.BaseAddress       = ret;
        m.AllocationBase    = ret;
        m.AllocationProtect = XBOXKRNL_PAGE_READWRITE /* 0x4 */;
        m.RegionSize        = nmemb * size;
        m.State             = XBOXKRNL_MEM_COMMIT /* 0x1000 */;
        m.Protect           = m.AllocationProtect;
        m.Type              = XBOXKRNL_MEM_PRIVATE /* 0x20000 */;
        MEM_PUSH(MEM_HEAP, &m);
    }

    return ret;
}

static void *
xboxkrnl_realloc(void *ptr, size_t size) {
    xboxkrnl_mem m = { };
    register void *ret;

    if (ptr) MEM_POP(MEM_HEAP, ptr);

    if ((ret = realloc(ptr, size))) {
        m.BaseAddress       = ret;
        m.AllocationBase    = ret;
        m.AllocationProtect = XBOXKRNL_PAGE_READWRITE /* 0x4 */;
        m.RegionSize        = size;
        m.State             = XBOXKRNL_MEM_COMMIT /* 0x1000 */;
        m.Protect           = m.AllocationProtect;
        m.Type              = XBOXKRNL_MEM_PRIVATE /* 0x20000 */;
        MEM_PUSH(MEM_HEAP, &m);
    }

    return ret;
}

static void
xboxkrnl_free(void *ptr) {
    if (!ptr) return;

    MEM_POP(MEM_HEAP, ptr);

    free(ptr);
}

typedef struct {
    uint16_t                Length;
    uint16_t                MaximumLength;
    const char *            Buffer;
} PACKED xboxkrnl_string;

enum XBOXKRNL_OBJ_ATTRIBUTES {
    XBOXKRNL_OBJ_INHERIT                    = 1 << 1,   /* 0x2 */
    XBOXKRNL_OBJ_PERMANENT                  = 1 << 4,   /* 0x10 */
    XBOXKRNL_OBJ_EXCLUSIVE                  = 1 << 5,   /* 0x20 */
    XBOXKRNL_OBJ_CASE_INSENSITIVE           = 1 << 6,   /* 0x40 */
    XBOXKRNL_OBJ_OPENIF                     = 1 << 7,   /* 0x80 */
    XBOXKRNL_OBJ_OPENLINK                   = 1 << 8,   /* 0x100 */
};

static const char *const xboxkrnl_obj_attributes_name[] = {
    NAMEB(1, XBOXKRNL_OBJ_INHERIT),
    NAMEB(4, XBOXKRNL_OBJ_PERMANENT),
    NAMEB(5, XBOXKRNL_OBJ_EXCLUSIVE),
    NAMEB(6, XBOXKRNL_OBJ_CASE_INSENSITIVE),
    NAMEB(7, XBOXKRNL_OBJ_OPENIF),
    NAMEB(8, XBOXKRNL_OBJ_OPENLINK),
};

typedef struct {
    void *                  RootDirectory;
    xboxkrnl_string *       ObjectName;
    uint32_t                Attributes;
} PACKED xboxkrnl_objattr;

typedef struct {
    int                     Status;
    uint32_t                Information;
} PACKED xboxkrnl_iostatus;

enum XBOXKRNL_DEVICE_TYPE {
    XBOXKRNL_FILE_DEVICE_BEEP                   =   1,
    XBOXKRNL_FILE_DEVICE_CD_ROM,                /*  2 */
    XBOXKRNL_FILE_DEVICE_CD_ROM_FILE_SYSTEM,    /*  3 */
    XBOXKRNL_FILE_DEVICE_CONTROLLER,            /*  4 */
    XBOXKRNL_FILE_DEVICE_DATALINK,              /*  5 */
    XBOXKRNL_FILE_DEVICE_DFS,                   /*  6 */
    XBOXKRNL_FILE_DEVICE_DISK,                  /*  7 */
    XBOXKRNL_FILE_DEVICE_DISK_FILE_SYSTEM,      /*  8 */
    XBOXKRNL_FILE_DEVICE_FILE_SYSTEM,           /*  9 */
    XBOXKRNL_FILE_DEVICE_INPORT_PORT,           /* 10 */
    XBOXKRNL_FILE_DEVICE_KEYBOARD,              /* 11 */
    XBOXKRNL_FILE_DEVICE_MAILSLOT,              /* 12 */
    XBOXKRNL_FILE_DEVICE_MIDI_IN,               /* 13 */
    XBOXKRNL_FILE_DEVICE_MIDI_OUT,              /* 14 */
    XBOXKRNL_FILE_DEVICE_MOUSE,                 /* 15 */
    XBOXKRNL_FILE_DEVICE_MULTI_UNC_PROVIDER,    /* 16 */
    XBOXKRNL_FILE_DEVICE_NAMED_PIPE,            /* 17 */
    XBOXKRNL_FILE_DEVICE_NETWORK,               /* 18 */
    XBOXKRNL_FILE_DEVICE_NETWORK_BROWSER,       /* 19 */
    XBOXKRNL_FILE_DEVICE_NETWORK_FILE_SYSTEM,   /* 20 */
    XBOXKRNL_FILE_DEVICE_NULL,                  /* 21 */
    XBOXKRNL_FILE_DEVICE_PARALLEL_PORT,         /* 22 */
    XBOXKRNL_FILE_DEVICE_PHYSICAL_NETCARD,      /* 23 */
    XBOXKRNL_FILE_DEVICE_PRINTER,               /* 24 */
    XBOXKRNL_FILE_DEVICE_SCANNER,               /* 25 */
    XBOXKRNL_FILE_DEVICE_SERIAL_MOUSE_PORT,     /* 26 */
    XBOXKRNL_FILE_DEVICE_SERIAL_PORT,           /* 27 */
    XBOXKRNL_FILE_DEVICE_SCREEN,                /* 28 */
    XBOXKRNL_FILE_DEVICE_SOUND,                 /* 29 */
    XBOXKRNL_FILE_DEVICE_STREAMS,               /* 30 */
    XBOXKRNL_FILE_DEVICE_TAPE,                  /* 31 */
    XBOXKRNL_FILE_DEVICE_TAPE_FILE_SYSTEM,      /* 32 */
    XBOXKRNL_FILE_DEVICE_TRANSPORT,             /* 33 */
    XBOXKRNL_FILE_DEVICE_UNKNOWN,               /* 34 */
    XBOXKRNL_FILE_DEVICE_VIDEO,                 /* 35 */
    XBOXKRNL_FILE_DEVICE_VIRTUAL_DISK,          /* 36 */
    XBOXKRNL_FILE_DEVICE_WAVE_IN,               /* 37 */
    XBOXKRNL_FILE_DEVICE_WAVE_OUT,              /* 38 */
    XBOXKRNL_FILE_DEVICE_8042_PORT,             /* 39 */
    XBOXKRNL_FILE_DEVICE_NETWORK_REDIRECTOR,    /* 40 */
    XBOXKRNL_FILE_DEVICE_BATTERY,               /* 41 */
    XBOXKRNL_FILE_DEVICE_BUS_EXTENDER,          /* 42 */
    XBOXKRNL_FILE_DEVICE_MODEM,                 /* 43 */
    XBOXKRNL_FILE_DEVICE_VDM,                   /* 44 */
    XBOXKRNL_FILE_DEVICE_MASS_STORAGE,          /* 45 */
    XBOXKRNL_FILE_DEVICE_SMB,                   /* 46 */
    XBOXKRNL_FILE_DEVICE_KS,                    /* 47 */
    XBOXKRNL_FILE_DEVICE_CHANGER,               /* 48 */
    XBOXKRNL_FILE_DEVICE_SMARTCARD,             /* 49 */
    XBOXKRNL_FILE_DEVICE_ACPI,                  /* 50 */
    XBOXKRNL_FILE_DEVICE_DVD,                   /* 51 */
    XBOXKRNL_FILE_DEVICE_FULLSCREEN_VIDEO,      /* 52 */
    XBOXKRNL_FILE_DEVICE_DFS_FILE_SYSTEM,       /* 53 */
    XBOXKRNL_FILE_DEVICE_DFS_VOLUME,            /* 54 */
    XBOXKRNL_FILE_DEVICE_SERENUM,               /* 55 */
    XBOXKRNL_FILE_DEVICE_TERMSRV,               /* 56 */
    XBOXKRNL_FILE_DEVICE_KSEC,                  /* 57 */
    XBOXKRNL_FILE_DEVICE_MEMORY_UNIT,           /* 58 */
    XBOXKRNL_FILE_DEVICE_MEDIA_BOARD,           /* 59 */
};

static const char *const xboxkrnl_device_type_name[] = {
    NAME(XBOXKRNL_FILE_DEVICE_BEEP),
    NAME(XBOXKRNL_FILE_DEVICE_CD_ROM),
    NAME(XBOXKRNL_FILE_DEVICE_CD_ROM_FILE_SYSTEM),
    NAME(XBOXKRNL_FILE_DEVICE_CONTROLLER),
    NAME(XBOXKRNL_FILE_DEVICE_DATALINK),
    NAME(XBOXKRNL_FILE_DEVICE_DFS),
    NAME(XBOXKRNL_FILE_DEVICE_DISK),
    NAME(XBOXKRNL_FILE_DEVICE_DISK_FILE_SYSTEM),
    NAME(XBOXKRNL_FILE_DEVICE_FILE_SYSTEM),
    NAME(XBOXKRNL_FILE_DEVICE_INPORT_PORT),
    NAME(XBOXKRNL_FILE_DEVICE_KEYBOARD),
    NAME(XBOXKRNL_FILE_DEVICE_MAILSLOT),
    NAME(XBOXKRNL_FILE_DEVICE_MIDI_IN),
    NAME(XBOXKRNL_FILE_DEVICE_MIDI_OUT),
    NAME(XBOXKRNL_FILE_DEVICE_MOUSE),
    NAME(XBOXKRNL_FILE_DEVICE_MULTI_UNC_PROVIDER),
    NAME(XBOXKRNL_FILE_DEVICE_NAMED_PIPE),
    NAME(XBOXKRNL_FILE_DEVICE_NETWORK),
    NAME(XBOXKRNL_FILE_DEVICE_NETWORK_BROWSER),
    NAME(XBOXKRNL_FILE_DEVICE_NETWORK_FILE_SYSTEM),
    NAME(XBOXKRNL_FILE_DEVICE_NULL),
    NAME(XBOXKRNL_FILE_DEVICE_PARALLEL_PORT),
    NAME(XBOXKRNL_FILE_DEVICE_PHYSICAL_NETCARD),
    NAME(XBOXKRNL_FILE_DEVICE_PRINTER),
    NAME(XBOXKRNL_FILE_DEVICE_SCANNER),
    NAME(XBOXKRNL_FILE_DEVICE_SERIAL_MOUSE_PORT),
    NAME(XBOXKRNL_FILE_DEVICE_SERIAL_PORT),
    NAME(XBOXKRNL_FILE_DEVICE_SCREEN),
    NAME(XBOXKRNL_FILE_DEVICE_SOUND),
    NAME(XBOXKRNL_FILE_DEVICE_STREAMS),
    NAME(XBOXKRNL_FILE_DEVICE_TAPE),
    NAME(XBOXKRNL_FILE_DEVICE_TAPE_FILE_SYSTEM),
    NAME(XBOXKRNL_FILE_DEVICE_TRANSPORT),
    NAME(XBOXKRNL_FILE_DEVICE_UNKNOWN),
    NAME(XBOXKRNL_FILE_DEVICE_VIDEO),
    NAME(XBOXKRNL_FILE_DEVICE_VIRTUAL_DISK),
    NAME(XBOXKRNL_FILE_DEVICE_WAVE_IN),
    NAME(XBOXKRNL_FILE_DEVICE_WAVE_OUT),
    NAME(XBOXKRNL_FILE_DEVICE_8042_PORT),
    NAME(XBOXKRNL_FILE_DEVICE_NETWORK_REDIRECTOR),
    NAME(XBOXKRNL_FILE_DEVICE_BATTERY),
    NAME(XBOXKRNL_FILE_DEVICE_BUS_EXTENDER),
    NAME(XBOXKRNL_FILE_DEVICE_MODEM),
    NAME(XBOXKRNL_FILE_DEVICE_VDM),
    NAME(XBOXKRNL_FILE_DEVICE_MASS_STORAGE),
    NAME(XBOXKRNL_FILE_DEVICE_SMB),
    NAME(XBOXKRNL_FILE_DEVICE_KS),
    NAME(XBOXKRNL_FILE_DEVICE_CHANGER),
    NAME(XBOXKRNL_FILE_DEVICE_SMARTCARD),
    NAME(XBOXKRNL_FILE_DEVICE_ACPI),
    NAME(XBOXKRNL_FILE_DEVICE_DVD),
    NAME(XBOXKRNL_FILE_DEVICE_FULLSCREEN_VIDEO),
    NAME(XBOXKRNL_FILE_DEVICE_DFS_FILE_SYSTEM),
    NAME(XBOXKRNL_FILE_DEVICE_DFS_VOLUME),
    NAME(XBOXKRNL_FILE_DEVICE_SERENUM),
    NAME(XBOXKRNL_FILE_DEVICE_TERMSRV),
    NAME(XBOXKRNL_FILE_DEVICE_KSEC),
    NAME(XBOXKRNL_FILE_DEVICE_MEMORY_UNIT),
    NAME(XBOXKRNL_FILE_DEVICE_MEDIA_BOARD),
};

typedef struct {
    int16_t                 Type;                   /*  0 */
    uint16_t                Size;                   /*  2 */
    int                     ReferenceCount;         /*  4 */
    void *                  DriverObject;           /*  8 */
    void *                  MountedOrSelfDevice;    /* 12 */
    void *                  CurrentIrp;             /* 16 */
    uint32_t                Flags;                  /* 20 */
    void *                  DeviceExtension;        /* 24 */
    uint8_t                 DeviceType;             /* 28 */
    uint8_t                 StartIoFlags;           /* 29 */
    int8_t                  StackSize;              /* 30 */
    uint8_t                 DeletePending;          /* 31 */
    uint32_t                SectorSize;             /* 32 */
    uint32_t                AlignmentRequirement;   /* 36 */
    struct {
        int16_t             Type;                   /* 40 */
        int16_t             Size;                   /* 42 */
        xboxkrnl_listentry  DeviceListHead;         /* 44 */
        uint32_t *          Lock;                   /* 52 */
        uint8_t             Busy;                   /* 56 */
    } PACKED                DeviceQueue;
    struct {
        xboxkrnl_dispatcher Header;                 /* 57 */
    } PACKED                DeviceLock;
    uint32_t                StartIoKey;             /* 73 */
} PACKED xboxkrnl_iodevice;

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

enum XBOXKRNL_FILE_DUPLICATE_OPTIONS {
    XBOXKRNL_DUPLICATE_CLOSE_SOURCE         = 1 << 0,   /* 0x1 */
    XBOXKRNL_DUPLICATE_SAME_ACCESS          = 1 << 1,   /* 0x2 */
    XBOXKRNL_DUPLICATE_SAME_ATTRIBUTES      = 1 << 2,   /* 0x4 */
};

static const char *const xboxkrnl_file_duplicate_options_name[] = {
    NAMEB(0, XBOXKRNL_DUPLICATE_CLOSE_SOURCE),
    NAMEB(1, XBOXKRNL_DUPLICATE_SAME_ACCESS),
    NAMEB(2, XBOXKRNL_DUPLICATE_SAME_ATTRIBUTES),
};

enum XBOXKRNL_FILE_INFORMATION_CLASS {
    XBOXKRNL_FilePositionInformation        = 14,
    XBOXKRNL_FileNetworkOpenInformation     = 34,
};

static const char *const xboxkrnl_file_information_class_name[] = {
    NAME(XBOXKRNL_FilePositionInformation),
    NAME(XBOXKRNL_FileNetworkOpenInformation),
};

typedef struct {
    int64_t                 CurrentByteOffset;
} PACKED xboxkrnl_FilePositionInformation; /* 14 */

typedef struct {
    int64_t                 CreationTime;
    int64_t                 LastAccessTime;
    int64_t                 LastWriteTime;
    int64_t                 ChangeTime;
    int64_t                 AllocationSize;
    int64_t                 EndOfFile;
    uint32_t                FileAttributes;
} PACKED xboxkrnl_FileNetworkOpenInformation; /* 34 */

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
    [XC_PARENTAL_CONTROL_GAMES]             = XC_PC_ESRB_ALL,
    [XC_PARENTAL_CONTROL_MOVIES]            = XC_PC_ESRB_ALL,
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
static void
xboxkrnl_dpc_iterate(void) {
    register xboxkrnl_dpc *dpc;
    register size_t i;

    DPC_LIST_LOCK;

    if (xboxkrnl_dpc_list_sz) {
        for (i = 1; i <= xboxkrnl_dpc_list_sz; ++i) {
            dpc = xboxkrnl_dpc_list[i - 1];
            if (!dpc || !dpc->Inserted || !dpc->DeferredRoutine) continue;
            PRINT("/* deferred procedure call 0x%.08x handled */", dpc);
            ((void (STDCALL *)(void *, void *, void *, void *))dpc->DeferredRoutine)(
                dpc,
                dpc->DeferredContext,
                dpc->SystemArgument1,
                dpc->SystemArgument2);
            dpc->Inserted = 0;
            PRINT("/* deferred procedure call 0x%.08x finished */", dpc);
        }
        free(xboxkrnl_dpc_list);
        xboxkrnl_dpc_list = NULL;
        xboxkrnl_dpc_list_sz = 0;
    }

    DPC_LIST_UNLOCK;
}

static void *
xboxkrnl_worker(void *arg) {
    if (!arg) {
        ENTER;

#define THREAD(x,y) do { \
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
            PRINT("/* starting '%s' thread */", #x); \
            \
            pthread_create(xboxkrnl_ ## x ## _thread, NULL, xboxkrnl_worker, (void *)(y)); \
        } while (0)
        THREAD(dpc,      2);
        THREAD(event,    3);
        THREAD(irq_nv2a, 4);
#undef THREAD

        LEAVE;
    } else if (arg == (void *)2) {
        ENTER_DPC;

        for (;;) {
            DPC_LOCK;
            while (!xboxkrnl_dpc_list_sz) {
                PRINT("/* waiting for deferred procedure call routines */", 0);
                DPC_WAIT;
            }
            DPC_UNLOCK;
            xboxkrnl_dpc_iterate();
        }

        LEAVE_DPC;
        pthread_exit(NULL);
    } else if (arg == (void *)3) {
        ENTER_EVENT;

        for (;;) {
            EVENT_LOCK;

//            PRINT("/* waiting for event routines */", 0);

//            EVENT_WAIT;

//            PRINT("/* signaled for event routines */", 0);

            nv2a_pfifo_puller(NULL);

            EVENT_UNLOCK;
        }

        LEAVE_EVENT;
        pthread_exit(NULL);
    } else if (arg == (void *)4) {
        register int ret;

        ENTER_IRQ_NV2A;

        IRQ_NV2A_LOCK;

        for (;;) {
            while (!nv2a_irq()) {
                PRINT("/* waiting for '%s' interrupt */", nv2a->name);
                ret = nv2a->irq_busy;
                nv2a->irq_busy = 0;
                nv2a_irq_restore(ret);
                IRQ_NV2A_WAIT;
            }
            if (nv2a->irq_kinterrupt && nv2a->irq_connected && nv2a->irq_level && nv2a->irq_isr_routine) {
                nv2a->irq_busy = /* NV2A_IRQ_BUSY */ 1;
                PRINT("/* '%s' interrupt handled */", nv2a->name);
                ret = ((char (STDCALL *)(void *, void *))nv2a->irq_isr_routine)(
                    nv2a->irq_kinterrupt,
                    nv2a->irq_isr_context);
                PRINT("/* '%s' interrupt routine returned: %i */", nv2a->name, ret);
                xboxkrnl_dpc_iterate();
                nv2a->irq_level = 0;
            }
        }

        IRQ_NV2A_UNLOCK;

        LEAVE_IRQ_NV2A;
        pthread_exit(NULL);
    }

    return NULL;
}

static int
xboxkrnl_init_hw(void) {
    register void *ptr;
    register size_t sz;
    register size_t i;
    register int ret = 1;
    ENTER;

    for (i = 0; i < ARRAY_SIZE(xpci) && xpci[i].name; ++i) {
        if ((ptr = (typeof(ptr))xpci[i].memreg_base)) {
            sz = xpci[i].memreg_size;
            PRINT("/* reserve: '%s': address 0x%.08x size 0x%.08x (%lu) */", xpci[i].name, ptr, sz, sz);
            if (xboxkrnl_mmap(
                    MEM_RESERVE,
                    ptr,
                    sz,
                    PROT_NONE,
                    MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED | MAP_NORESERVE,
                    -1,
                    0) != ptr) {
                PRINT("error: failed to reserve hardware memory: mmap(): address %p size %p (%lu): '%s'",
                    ptr,
                    sz,
                    sz,
                    strerror(errno));
                LEAVE;
                return ret;
            }
        }
    }
    for (i = 0; i < ARRAY_SIZE(xpci) && xpci[i].name; ++i) {
        if ((ptr = (typeof(ptr))xpci[i].memreg_base)) {
            sz = xpci[i].memreg_size;
            if ((ptr = xboxkrnl_mmap(
                    MEM_EXEC,
                    NULL,
                    sz,
                    PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
                    -1,
                    0)) == MAP_FAILED) {
                PRINT("error: failed to allocate hardware memory: mmap(): size %p (%lu): '%s'",
                    sz,
                    sz,
                    strerror(errno));
                LEAVE;
                return ret;
            }
            xpci[i].memreg = ptr;
            PRINT("/* allocate: '%s': address 0x%.08x size 0x%.08x (%lu) */", xpci[i].name, ptr, sz, sz);
        }
    }
    do {
        if (nv2a_init()) break;
    } while ((ret = 0));

    LEAVE;
    return ret;
}

static void xboxkrnl_destroy_var(void);
static int xboxkrnl_init_var(void);

void
xboxkrnl_destroy(void/* *x*/) {
    ENTER;
#if 0
    if (!xboxkrnl_thread_mutex) {
        LEAVE;
        return;
    }
#endif
    xboxkrnl_destroy_var();
#if 0
    pthread_mutex_destroy(xboxkrnl_thread_mutex);
    xboxkrnl_thread_mutex = NULL;
    pthread_mutexattr_destroy(xboxkrnl_thread_mattr);
    xboxkrnl_thread_mattr = NULL;
#endif
    if (xboxkrnl_c_wc != (iconv_t)-1) {
        iconv_close(xboxkrnl_c_wc);
        xboxkrnl_c_wc = (iconv_t)-1;
    }
    if (xboxkrnl_wc_c != (iconv_t)-1) {
        iconv_close(xboxkrnl_wc_c);
        xboxkrnl_wc_c = (iconv_t)-1;
    }

    LEAVE;
}

int
xboxkrnl_init(void/* **x*/) {
    register int ret = 1;
    ENTER;
#if 0
    if (xboxkrnl_thread_mutex) {
        LEAVE;
        return ret;
    }
#endif
    xboxkrnl_tsc_init();

    xboxkrnl_wc_c = iconv_open("UTF-8", "UTF-16LE");
    xboxkrnl_c_wc = iconv_open("UTF-16LE", "UTF-8");
#if 0
    xboxkrnl_thread_mutex = calloc(1, sizeof(*xboxkrnl_thread_mutex));
    xboxkrnl_thread_mattr = calloc(1, sizeof(*xboxkrnl_thread_mattr));
    pthread_mutexattr_init(xboxkrnl_thread_mattr);
    pthread_mutexattr_settype(xboxkrnl_thread_mattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(xboxkrnl_thread_mutex, xboxkrnl_thread_mattr);
#endif
    if (!(ret = xboxkrnl_init_hw()) &&
        !(ret = xboxkrnl_init_var())) xboxkrnl_worker(0);

    if (ret) xboxkrnl_destroy();

    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
}

/* helpers */
static void
xboxkrnl_dma_write(uint32_t addr, const uint32_t *val, size_t sz) {
    register uint32_t wr = addr;
    register int ret;

    if (addr < PAGESIZE) wr += (typeof(addr))host->memreg;

    switch (sz) {
    case 1:
        PRINT("DMA: write: [0x%.08x]       (0x%.02hhx) <- 0x%.02hhx", addr, REG08(wr), REG08(val));
        REG08(wr) = REG08(val);
        break;
    case 2:
        PRINT("DMA: write: [0x%.08x]     (0x%.04hx) <- 0x%.04hx", addr, REG16(wr), REG16(val));
        REG16(wr) = REG16(val);
        break;
    case 4:
//        PRINT("DMA: write: [0x%.08x] (0x%.08x) <- 0x%.08x", addr, REG32(wr), REG32(val));
        REG32(wr) = REG32(val);
        break;
    default:
        INT3;
        break;
    }
}

static void
xboxkrnl_dma_read(uint32_t addr, uint32_t *val, size_t sz) {
    register uint32_t rd = addr;

    if (addr < PAGESIZE) rd += (typeof(addr))host->memreg;

    switch (sz) {
    case 1:
        REG08(val) = REG08(rd);
        PRINT("DMA:  read: [0x%.08x]              -> 0x%.02hhx", addr, REG08(val));
        break;
    case 2:
        REG16(val) = REG16(rd);
        PRINT("DMA:  read: [0x%.08x]              -> 0x%.04hx", addr, REG16(val));
        break;
    case 4:
        REG32(val) = REG32(rd);
//        PRINT("DMA:  read: [0x%.08x]              -> 0x%.08x", addr, REG32(val));
        break;
    default:
        INT3;
        break;
    }
}

static int
xboxkrnl_address_validate(uint32_t addr) {
    register int ret;

    ret =
        ((addr < 0x00001000) ||
        (addr & 0xf8000000) == host->memreg_base ||
        (addr & 0xff000000) == nv2a->memreg_base ||
        (addr >= usb0->memreg_base && addr < usb0->memreg_base + usb0->memreg_size) ||
        (addr >= usb1->memreg_base && addr < usb1->memreg_base + usb1->memreg_size) ||
        (addr >= apu->memreg_base && addr < apu->memreg_base + apu->memreg_size) ||
        (addr >= aci->memreg_base && addr < aci->memreg_base + aci->memreg_size) ||
        MEM_DIRTY_PARENT_RET_LOCKED(addr));

    return ret;
}

static int
xboxkrnl_write(uint32_t addr, const void *val, size_t sz) {
    register xboxkrnl_mem *m;
    register int ret;

    if ((addr & 0xf8000000) == host->memreg_base) addr &= host->memreg_size - 1;

    MEM_DIRTY_ON_LOCKED_PRE(m, addr);

    ret =
        nv2a_write(addr, val, sz) ||
        ohci_write(addr, val, sz) ||
        apu_write(addr, val, sz) ||
        aci_write(addr, val, sz);

    if (!ret) {
        xboxkrnl_dma_write(addr, val, sz);
        ret = 1;
    }

    MEM_DIRTY_ON_LOCKED_POST(m);

    return ret;
}

static int
xboxkrnl_read(uint32_t addr, void *val, size_t sz) {
    register int ret;

    if ((addr & 0xf8000000) == host->memreg_base) addr &= host->memreg_size - 1;

    ret =
        nv2a_read(addr, val, sz) ||
        ohci_read(addr, val, sz) ||
        apu_read(addr, val, sz) ||
        aci_read(addr, val, sz);

    if (!ret) {
        xboxkrnl_dma_read(addr, val, sz);
        ret = 1;
    }

    return ret;
}

static void
xboxkrnl_path_resolve_insensitive(char *path) {
    register char *p;
    register char *t;
    register size_t l;
    register DIR *d;
    register struct dirent *e;
    char *buf;
    size_t len;
    char *name;

    buf  = malloc(4);
    p    = buf;
    len  = 0;
    *p++ = '.', ++len;  /* 0 */
    *p++ = 0;           /* 1 */
    *p   = 0;           /* 2 */

    for (p = path; *p; p += l + !!t) {
        l = ((t = strchr(p, '/'))) ? (typeof(l))(t - p) : strlen(p);
        if (!l) continue;
        if (t) t = strndup(p, l);
        name = NULL;
        if ((d = opendir(buf))) {
            while ((e = readdir(d))) {
                if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..")) continue;
//PRINT("'%s' '%s'",(t) ? t : p,e->d_name);//XXX
                if (!strncasecmp((t) ? t : p, e->d_name, l + 1)) {
                    if (name) {
                        closedir(d);
                        free(name);
                        free(t);
                        free(buf);
                        return;
                    }
                    name = strdup(e->d_name);
                }
            }
            closedir(d);
        }
        buf = realloc(buf, len + 1 + l + 1);
        buf[len++] = '/';
        memcpy(&buf[len], (name) ? name : ((t) ? t : p), l);
        len += l;
        buf[len] = 0;
        free(name);
        free(t);
    }

//PRINT("'%s'",&buf[2]);//XXX
    if (buf[2]) strcpy(path, &buf[2]);
    free(buf);
}

static char *
xboxkrnl_path_winnt_to_unix(const char *path) {
    register const char *prefix = NULL;
    register char *tmp;
    register size_t len;
    register size_t i;
    register char *ret;

    tmp = strdup(path);
    len = strlen(path);

    for (i = 0; i < len; ++i) if (tmp[i] == '\\') tmp[i] = '/';

#define TRIM(x) \
    while ((ret = strstr(tmp, (x)))) { \
        i = (typeof(i))(ret - tmp); \
        memmove(&tmp[i], &tmp[i + (sizeof((x)) - 1)], len - (sizeof((x)) - 1)); \
        len -= sizeof((x)) - 1; \
        tmp[len] = 0; \
    }
    TRIM("/..");
    TRIM("/.");
#undef TRIM

    do {
#define PREFIX(x,y) \
        if (!strncasecmp(tmp, (x), (i = sizeof((x)) - 1))) { \
            prefix = (y); \
            break; \
        }
        PREFIX("/Device/CdRom0",               "D");
        PREFIX("/Device/Harddisk0/partition0", "");
        PREFIX("/Device/Harddisk0/partition1", "E");
        PREFIX("/\?\?/D:",                     "D");
        PREFIX("/\?\?/T:",                     "T");
        PREFIX("/\?\?/U:",                     "U");
        PREFIX("D:",                           "D");
#undef PREFIX
    } while (0);

    if (prefix) {
        len -= i;
        memmove(tmp, &tmp[i], len);
        tmp[len] = 0;
        i = strlen(prefix);
        ret = malloc(i + len + 1);
        memcpy(ret, prefix, i);
        memcpy(&ret[i], tmp, len);
        ret[i + len] = 0;
        free(tmp);
    } else {
        ret = tmp;
    }
    xboxkrnl_path_resolve_insensitive(ret);

    return ret;
}

/* variables */
static uint32_t             xboxkrnl_AvpCurrentMode = 0;
static void *               xboxkrnl_AvpSavedDataAddress = NULL;

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
static STDCALL void *
xboxkrnl_AvGetSavedDataAddress(void) { /* 001 (0x001) */
    ENTER;

    VARDUMP(DUMP, xboxkrnl_AvpSavedDataAddress);
    LEAVE;
    return xboxkrnl_AvpSavedDataAddress;
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
    case /* AV_OPTION_MACROVISION_MODE */ 1:
        break;
    case /* AV_OPTION_ENABLE_CC */ 2:
        break;
    case /* AV_OPTION_DISABLE_CC */ 3:
        break;
    case /* AV_OPTION_SEND_CC_DATA */ 4:
        break;
    case /* AV_QUERY_CC_STATUS */ 5:
        *Result = 0;
        break;
    case /* AV_QUERY_AV_CAPABILITIES */ 6:
        *Result  = XC_AV_PACK_HDTV;
        *Result |= xboxkrnl_os[XC_FACTORY_AV_REGION] << /* AV_STANDARD_BIT_SHIFT */ 8;
        *Result |= /* AV_FLAGS_60Hz */ 1 << 22;
        break;
    case /* AV_OPTION_BLANK_SCREEN */ 9:
        break;
    case /* AV_OPTION_MACROVISION_COMMIT */ 10:
        break;
    case /* AV_OPTION_FLICKER_FILTER */ 11:
        break;
    case /* AV_OPTION_ZERO_MODE */ 12:
        xboxkrnl_AvpCurrentMode = 0;
        break;
    case /* AV_OPTION_QUERY_MODE */ 13:
        *Result = xboxkrnl_AvpCurrentMode;
        break;
    case /* AV_OPTION_ENABLE_LUMA_FILTER */ 14:
        break;
    case /* AV_OPTION_GUESS_FIELD */ 15:
        *Result = 0;
        break;
    case /* AV_QUERY_ENCODER_TYPE */ 16:
        *Result = /* AV_ENCODER_CONEXANT_871 */ 0;
        break;
    case /* AV_QUERY_MODE_TABLE_VERSION */ 17:
        *Result = /* AV_MODE_TABLE_VERSION */ 0;
        break;
    case /* AV_OPTION_CGMS */ 18:
        break;
    case /* AV_OPTION_WIDESCREEN */ 19:
        xboxkrnl_AvpCurrentMode &= ~ /* AV_MODE_FLAGS_WSS */ 0x10000000;
        if (Param) xboxkrnl_AvpCurrentMode |= /* AV_MODE_FLAGS_WSS */ 0x10000000;
        break;
    default:
        INT3;
        break;
    }

    VARDUMPD(DUMP, Result);
    LEAVE;
}
static STDCALL uint32_t
xboxkrnl_AvSetDisplayMode( /* 003 (0x003) */
        /* in */ void *RegisterBase,
        /* in */ uint32_t Step,
        /* in */ uint32_t Mode,
        /* in */ uint32_t Format,
        /* in */ uint32_t Pitch,
        /* in */ uint32_t FrameBuffer) {
    ENTER;
    VARDUMP(VAR_IN, RegisterBase);
    VARDUMP(VAR_IN, Step);
    VARDUMP(VAR_IN, Mode);
    VARDUMP(VAR_IN, Format);
    VARDUMP(VAR_IN, Pitch);
    VARDUMP(VAR_IN, FrameBuffer);

    if (Mode == /* AV_MODE_OFF */ 0x00000000) {
        Mode =
            /* AV_MODE_640x480_TO_NTSC_M_YC */ 0x04010101 |
            /* AV_MODE_FLAGS_DACA_DISABLE */ 0x01000000 |
            /* AV_MODE_FLAGS_DACB_DISABLE */ 0x02000000 |
            /* AV_MODE_FLAGS_DACC_DISABLE */ 0x04000000 |
            /* AV_MODE_FLAGS_DACD_DISABLE */ 0x08000000;
    }
#if 0
    switch (Format) {
    case /* D3DFMT_LIN_A1R5G5B5 */ 16:
        *internalFormat = GL_RGB5_A1;
        *format = GL_BGRA;
        *type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
        break;
    case /* D3DFMT_LIN_X1R5G5B5 */ 28:
        *internalFormat = GL_RGB5;
        *format = GL_BGRA;
        *type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
        break;
    case /* D3DFMT_LIN_R5G6B5 */ 17:
        *internalFormat = GL_RGB;
        *format = GL_RGB;
        *type = GL_UNSIGNED_SHORT_5_6_5;
        break;
    case /* D3DFMT_LIN_A8R8G8B8 */ 18:
        *internalFormat = GL_RGBA8;
        *format = GL_BGRA;
        *type = GL_UNSIGNED_INT_8_8_8_8_REV;
        break;
    case /* D3DFMT_LIN_X8R8G8B8 */ 30:
        *internalFormat = GL_RGB8;
        *format = GL_BGRA;
        *type = GL_UNSIGNED_INT_8_8_8_8_REV;
        break;
    default:
        *internalFormat = GL_RGBA;
        *format = GL_RGBA;
        *type = GL_UNSIGNED_INT_8_8_8_8_REV;
        break;
    }
#endif
    Pitch /= 8;

    nv2a_framebuffer_set(FrameBuffer);

    if (xboxkrnl_AvpCurrentMode == Mode) {
        xboxkrnl_AvSendTVEncoderOption(RegisterBase, /* AV_OPTION_FLICKER_FILTER */ 11, 5, NULL);
        xboxkrnl_AvSendTVEncoderOption(RegisterBase, /* AV_OPTION_ENABLE_LUMA_FILTER */ 14, 0, NULL);
    } else {
        xboxkrnl_AvpCurrentMode = Mode;
    }

    LEAVE;
    return 0;
}
static STDCALL void
xboxkrnl_AvSetSavedDataAddress( /* 004 (0x004) */
        /* in */ void *Address) {
    ENTER;
    VARDUMP(VAR_IN, Address);

    xboxkrnl_AvpSavedDataAddress = Address;
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
static STDCALL int
xboxkrnl_DbgPrint( /* 008 (0x008) */
        /* in */ const char *Format,
        /* in */ ...) {
    va_list args;
    char buf[4096];
    register int ret;
    ENTER;

    va_start(args, Format);
    ret = vsnprintf(buf, sizeof(buf), Format, args);
    va_end(args);

    if (ret > 0) PRINT("/* kernel debug output: */\n%s", buf);

    ret = (ret < 0);

    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
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
    register void *ret;
    ENTER;
    VARDUMP(VAR_IN, NumberOfBytes);

    if (!(ret = xboxkrnl_malloc(NumberOfBytes)) && NumberOfBytes) INT3;

    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
}
static STDCALL void *
xboxkrnl_ExAllocatePoolWithTag( /* 015 (0x00f) */
        /* in */ size_t NumberOfBytes,
        /* in */ uint32_t Tag) {
    register void *ret;
    ENTER;
    VARDUMP(VAR_IN, NumberOfBytes);
    VARDUMP(VAR_IN, Tag);
    STRDUMPR(Tag);

    if (!(ret = xboxkrnl_malloc(NumberOfBytes)) && NumberOfBytes) INT3;

    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
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

    xboxkrnl_free(P);

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
static STDCALL uint32_t
xboxkrnl_ExQueryPoolBlockSize( /* 023 (0x017) */
        /* in */ void *PoolBlock) {
    register xboxkrnl_mem *m;
    register uint32_t ret;
    ENTER;
    VARDUMP(VAR_IN, PoolBlock);

    MEM_LOCK;

    if (!(m = MEM_RET_LOCKED(MEM_HEAP, PoolBlock)) &&
        !(m = MEM_RET_LOCKED(MEM_MAP, PoolBlock))) INT3;

    ret = m->RegionSize;

    MEM_UNLOCK;

    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
}
static STDCALL int
xboxkrnl_ExQueryNonVolatileSetting( /* 024 (0x018) */
        /* in */  uint32_t ValueIndex,
        /* out */ uint32_t *Type,
        /* out */ void *Value,
        /* in */  uint32_t ValueLength,
        /* out */ uint32_t *ResultLength) {
    register uint32_t val = 0;
    register int ret = 0;
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
        *Type = /* REG_DWORD */ 4;
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
            STRDUMP(xpci[i].name);
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
    VARDUMP(STUB, ShutdownRegistration);
    VARDUMP(STUB, Register);

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
static STDCALL int
xboxkrnl_IoCreateDevice( /* 065 (0x041) */
        /* in */       void *DriverObject,
        /* in */       uint32_t DeviceExtensionSize,
        /* in (opt) */ xboxkrnl_string *DeviceName,
        /* in */       uint32_t DeviceType,
        /* in */       uint8_t Exclusive,
        /* out */      xboxkrnl_iodevice **DeviceObject) {
    register xboxkrnl_iodevice *d;
    ENTER;
    VARDUMP(VAR_IN,  DriverObject);
    VARDUMP(VAR_IN,  DeviceExtensionSize);
    VARDUMP(VAR_IN,  DeviceName);
    STRDUMP2(DeviceName, Buffer);
    VARDUMP2(VAR_IN, DeviceType, xboxkrnl_device_type_name);
    VARDUMP(VAR_IN,  Exclusive);
    VARDUMP(VAR_OUT, DeviceObject);

    d                  = xboxkrnl_calloc(1, sizeof(*d));
    d->Type            = DeviceType;
    d->Size            = DeviceExtensionSize;
    d->DriverObject    = DriverObject;
    d->DeviceExtension = xboxkrnl_calloc(1, DeviceExtensionSize);
    *DeviceObject      = d;

    VARDUMP(DUMP, d);
    VARDUMP(DUMP, d->DeviceExtension);
    LEAVE;
    return 0;
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
    register char *src;
    register char *dest;
    register size_t len;
    register int ret = -1;
    ENTER;
    VARDUMP(VAR_IN, SymbolicLinkName);
    STRDUMP(SymbolicLinkName->Buffer);
    VARDUMP(VAR_IN, DeviceName);
    STRDUMP(DeviceName->Buffer);

    src = xboxkrnl_path_winnt_to_unix(DeviceName->Buffer);
    STRDUMP(src);
    dest = xboxkrnl_path_winnt_to_unix(SymbolicLinkName->Buffer);
    STRDUMP(dest);

    if ((len = strlen(src)) == strlen(dest) && !strncmp(src, dest, len + 1)) {
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
    register size_t i;
    register uint8_t ret = 0;
    ENTER;
    VARDUMP(VAR_IN, Interrupt);

    for (i = 0; xpci[i].name; ++i) {
        if (xpci[i].irq_kinterrupt == Interrupt) {
            xpci[i].irq_connected = 1;
            ret = 1;
            STRDUMP(xpci[i].name);
            break;
        }
    }

    VARDUMP(DUMP, ret);
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
    VARDUMP(VAR_IN,  WaitMode);
    VARDUMP(VAR_IN,  Alertable);
    VARDUMP(VAR_IN,  Interval);
    VARDUMPD(VAR_IN, Interval);

    nsec = *Interval;

    if (nsec < 0) {
        nsec       = -nsec;
        tp.tv_sec  = nsec / 10000000;
        tp.tv_nsec = nsec % 10000000;
        VARDUMP(DUMP, tp.tv_sec);
        VARDUMP(DUMP, tp.tv_nsec);
        nanosleep(&tp, NULL);
    } else if (nsec > 0) {
        INT3;
    }

    LEAVE;
    return 0;
}
static STDCALL uint8_t
xboxkrnl_KeDisconnectInterrupt( /* 100 (0x064) */
        /* in */ void *Interrupt) {
    register size_t i;
    register uint8_t ret = 0;
    ENTER;
    VARDUMP(VAR_IN, Interrupt);

    for (i = 0; xpci[i].name; ++i) {
        if (xpci[i].irq_kinterrupt == Interrupt) {
            xpci[i].irq_connected = 0;
            ret = 1;
            STRDUMP(xpci[i].name);
            break;
        }
    }

    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
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
            xpci[i].irq_kinterrupt  = Interrupt;
            xpci[i].irq_connected   = 0;
            xpci[i].irq_level       = 0;
            xpci[i].irq_busy        = 0;
            xpci[i].irq_isr_routine = ServiceRoutine;
            xpci[i].irq_isr_context = ServiceContext;
            STRDUMP(xpci[i].name);
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
static STDCALL uint8_t
xboxkrnl_KeInsertQueueDpc( /* 119 (0x077) */
        /* in */ xboxkrnl_dpc *Dpc,
        /* in */ void *SystemArgument1,
        /* in */ void *SystemArgument2) {
    register size_t i;
    register uint8_t ret = 1;
    ENTER;
    VARDUMP(VAR_IN, Dpc);
    VARDUMP(VAR_IN, SystemArgument1);
    VARDUMP(VAR_IN, SystemArgument2);

    if (Dpc) {
        DPC_LIST_LOCK;
        for (i = 1; i <= xboxkrnl_dpc_list_sz; ++i) {
            if (xboxkrnl_dpc_list[i - 1] == Dpc) {
                ret = 0;
                break;
            }
        }
        if (ret && !Dpc->Inserted) {
            Dpc->Inserted            = 1;
            Dpc->SystemArgument1     = SystemArgument1;
            Dpc->SystemArgument2     = SystemArgument2;
            i                        = xboxkrnl_dpc_list_sz;
            xboxkrnl_dpc_list        = realloc(xboxkrnl_dpc_list, sizeof(Dpc) * ++i);
            xboxkrnl_dpc_list_sz     = i;
            xboxkrnl_dpc_list[i - 1] = Dpc;
            if (!nv2a->irq_busy) DPC_SIGNAL;//TODO test all irq but nv2a
        }
        DPC_LIST_UNLOCK;
    } else {
        ret = 0;
    }

    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
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
    register int ret = 0;
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
    Timer->Dpc                = Dpc;
    Timer->Period             = Period;

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
static STDCALL int
xboxkrnl_KeWaitForSingleObject( /* 159 (0x09f) */
        /* in */       void *Object,
        /* in */       int WaitReason,
        /* in */       char WaitMode,
        /* in */       uint8_t Alertable,
        /* in (opt) */ int64_t *Timeout) {
    ENTER;
    VARDUMP(STUB,  Object);
    VARDUMP(STUB,  WaitReason);
    VARDUMP(STUB,  WaitMode);
    VARDUMP(STUB,  Alertable);
    VARDUMP(STUB,  Timeout);
    VARDUMPD(STUB, Timeout);

    LEAVE;
    return 0;
}
static FASTCALL uint8_t
xboxkrnl_KfRaiseIrql( /* 160 (0x0a0) */
        /* in */ uint8_t NewIrql) {
    ENTER;
    VARDUMP(STUB, NewIrql);

    LEAVE;
    return 0;
}
static FASTCALL void
xboxkrnl_KfLowerIrql( /* 161 (0x0a1) */
        /* in */ uint8_t NewIrql) {
    ENTER;
    VARDUMP(STUB, NewIrql);

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
#define LDT_LAUNCH_DASHBOARD 1
#define LDT_NONE             0xffffffff

#define LDF_HAS_BEEN_READ    0x00000001
#endif
typedef struct {
    struct {
        uint32_t            dwLaunchDataType;       /*    0 */
        uint32_t            dwTitleId;              /*    4 */
        char                szLaunchPath[520];      /*    8 */
        uint32_t            dwFlags;                /*  528 */
    } PACKED                Header;
    char                    Pad[492];               /*  532 */
    char                    LaunchData[3072];       /* 1024 */
} PACKED xboxkrnl_LaunchDataPage_t;
static xboxkrnl_LaunchDataPage_t
xboxkrnl_LaunchDataPage; /* 164 (0x0a4) */
static STDCALL void *
xboxkrnl_MmAllocateContiguousMemory( /* 165 (0x0a5) */
        /* in */ size_t NumberOfBytes) {
    register size_t sz;
    register void *ret;
    ENTER;
    VARDUMP(VAR_IN, NumberOfBytes);

    VARDUMP(DUMP, xboxkrnl_mem_contiguous);

    if ((sz = NumberOfBytes & PAGEMASK)) sz = NumberOfBytes + PAGESIZE - sz;
    else sz = NumberOfBytes;
    VARDUMP(DUMP, sz);

    if ((ret = xboxkrnl_mmap(
            MEM_MAP,
            xboxkrnl_mem_contiguous,
            sz,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
            -1,
            0)) == MAP_FAILED) INT3;

    xboxkrnl_mem_contiguous += sz;

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
    register size_t sz;
    register void *ret;
    ENTER;
    VARDUMP(VAR_IN,  NumberOfBytes);
    VARDUMP(VAR_IN,  LowestAcceptableAddress);
    VARDUMP(VAR_IN,  HighestAcceptableAddress);
    VARDUMP(VAR_IN,  Alignment);
    VARDUMP3(VAR_IN, Protect, xboxkrnl_page_type_name);

    VARDUMP(DUMP, xboxkrnl_mem_contiguous);

    if ((sz = NumberOfBytes & PAGEMASK)) sz = NumberOfBytes + PAGESIZE - sz;
    else sz = NumberOfBytes;
    VARDUMP(DUMP, sz);

    if ((ret = xboxkrnl_mmap(
            MEM_MAP,
            xboxkrnl_mem_contiguous,
            sz,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
            -1,
            0)) == MAP_FAILED) INT3;

    xboxkrnl_mem_contiguous += sz;

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
    register size_t tmp;
    register void *ret;
    ENTER;
    VARDUMP(VAR_IN,  NumberOfBytes);
    VARDUMP(VAR_OUT, NumberOfPaddingBytes);
    VARDUMP(VAR_OUT, *NumberOfPaddingBytes);

    if (NumberOfBytes == ~0UL) {
        ret = xboxkrnl_gpuinstmem + xboxkrnl_gpuinstsz;
    } else {
        if (xboxkrnl_gpuinstmem) xboxkrnl_munmap(xboxkrnl_gpuinstmem, xboxkrnl_gpuinstsz);
        if ((tmp = NumberOfBytes & PAGEMASK)) NumberOfBytes += PAGESIZE - tmp;
        if ((ret = xboxkrnl_mmap(
                MEM_MAP,
                NULL,
                NumberOfBytes,
                0,//PROT_READ | PROT_WRITE,//TODO
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
                -1,
                0)) == MAP_FAILED) INT3;
        xboxkrnl_gpuinstmem = ret;
        xboxkrnl_gpuinstsz  = NumberOfBytes;
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
    register uint32_t ret = (typeof(ret))BaseAddress;
    ENTER;
    VARDUMP(VAR_IN, BaseAddress);

    if ((ret & 0xf8000000) == host->memreg_base) ret &= host->memreg_size - 1;

    VARDUMP(DUMP, ret);
    LEAVE;
    return (void *)ret;
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
    VARDUMP(STUB, BaseAddress);
    VARDUMP(STUB, NumberOfBytes);
    VARDUMP(STUB, UnlockPages);

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
static STDCALL size_t
xboxkrnl_MmQueryAllocationSize( /* 180 (0x0b4) */
        /* in */ void *BaseAddress) {
    register xboxkrnl_mem *m;
    register size_t ret;
    ENTER;
    VARDUMP(VAR_IN, BaseAddress);

    MEM_LOCK;

    if (!(m = MEM_RET_LOCKED(MEM_MAP, BaseAddress)) &&
        !(m = MEM_RET_LOCKED(MEM_HEAP, BaseAddress))) INT3;

    ret = m->RegionSize;

    MEM_UNLOCK;

    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
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
    register int flag = 0;
    register int ret = -1;
    ENTER;
    VARDUMP(VAR_IN,  BaseAddress);
    VARDUMP(VAR_OUT, *BaseAddress);
    VARDUMP(VAR_IN,  ZeroBits);
    VARDUMPD(VAR_IN, ZeroBits);
    VARDUMP(VAR_IN,  RegionSize);
    VARDUMP(VAR_OUT, *RegionSize);
    VARDUMP3(VAR_IN, AllocationType, xboxkrnl_mem_type_name);
    VARDUMP3(VAR_IN, Protect, xboxkrnl_page_type_name);

    if (AllocationType & XBOXKRNL_MEM_RESERVE /* 0x2000 */) {
        if (Protect & XBOXKRNL_PAGE_NOCACHE /* 0x200 */ ||
            Protect & XBOXKRNL_PAGE_WRITECOMBINE /* 0x400 */) flag |= MAP_NORESERVE;
        if ((*BaseAddress = xboxkrnl_mmap(
                MEM_MAP,
                *BaseAddress,
                *RegionSize,
                PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS | flag,
                -1,
                0)) == MAP_FAILED) {
            PRINT("error: mmap(): '%s'", strerror(errno));
            *BaseAddress = NULL;
        } else {
            ret = 0;
        }
    } else if (AllocationType & XBOXKRNL_MEM_COMMIT /* 0x1000 */) {
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
        STRDUMP(Handle->path);
        VARDUMP(VAR_IN, Handle->fd);
        VARDUMP(VAR_IN, Handle->dir);
        if (Handle->fd >= 0) {
            VARDUMP(DUMP, lseek(Handle->fd, 0, SEEK_CUR));
            close(Handle->fd);
        }
        if (Handle->dir) closedir(Handle->dir);
        xboxkrnl_free(Handle);
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
    xboxkrnl_file f = { .fd = -1 };
    struct stat st;
//    int ci = 0;
    int rdonly;
    int exists;
    register char *path;
    register int tmp;
    register int ret = -1;
    ENTER;
    VARDUMP(VAR_OUT, FileHandle);
    VARDUMP3(VAR_IN, DesiredAccess, xboxkrnl_file_access_mask_name);
    VARDUMP(VAR_IN,  ObjectAttributes);
    VARDUMP(VAR_IN,  ObjectAttributes->RootDirectory);
    VARDUMP(VAR_IN,  ObjectAttributes->ObjectName);
    STRDUMP(ObjectAttributes->ObjectName->Buffer);
    VARDUMP3(VAR_IN, ObjectAttributes->Attributes, xboxkrnl_obj_attributes_name);
    VARDUMP(VAR_OUT, IoStatusBlock);
    VARDUMP(VAR_OUT, IoStatusBlock->Status);
    VARDUMP(VAR_OUT, IoStatusBlock->Information);
    VARDUMP(VAR_IN,  AllocationSize);
    VARDUMPD(VAR_IN, AllocationSize);
    VARDUMP(VAR_IN,  FileAttributes);
    VARDUMP(VAR_IN,  ShareAccess);
    VARDUMP2(VAR_IN, CreateDisposition, xboxkrnl_file_create_disposition_name);
    VARDUMP3(VAR_IN, CreateOptions, xboxkrnl_file_open_create_name);

    path = xboxkrnl_path_winnt_to_unix(ObjectAttributes->ObjectName->Buffer);
    STRDUMP(path);
    tmp = sizeof(f.path) - 1;
    f.path[tmp] = 0;
    strncpy(f.path, path, tmp);
    free(path);
    path = f.path;
    rdonly = (path[0] == 'D' && path[1] == '/');
#if 0
    ci = !!(ObjectAttributes->Attributes & XBOXKRNL_OBJ_CASE_INSENSITIVE);

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
        exists = !tmp;
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
        if (CreateOptions & XBOXKRNL_FILE_DIRECTORY_FILE) {
            if (!(f.dir = opendir(path))) {
                PRINT("error: opendir(): '%s'", strerror(errno));
                break;
            }
        } else {
            if (rdonly) {
                CreateDisposition = XBOXKRNL_FILE_OPEN;
                tmp = O_RDONLY;
            } else {
                tmp = (DesiredAccess & XBOXKRNL_FILE_WRITE_DATA ||
                    DesiredAccess & XBOXKRNL_FILE_APPEND_DATA ||
                    DesiredAccess & XBOXKRNL_GENERIC_WRITE ||
                    DesiredAccess & XBOXKRNL_GENERIC_ALL) ? O_RDWR : O_RDONLY;
                tmp |= O_CREAT;
                switch (CreateDisposition) {
                case XBOXKRNL_FILE_SUPERSEDE:
                case XBOXKRNL_FILE_OVERWRITE:
                case XBOXKRNL_FILE_OVERWRITE_IF:
                    tmp |= O_TRUNC;
                    break;
                }
            }
            if ((f.fd = open(path, tmp, 0644)) < 0) {
                PRINT("error: open(): '%s'", strerror(errno));
                break;
            }
            if (tmp & O_RDWR) {
                if (!exists &&
                    AllocationSize &&
                    *AllocationSize &&
                    ftruncate(f.fd, *AllocationSize) < 0) {
                    PRINT("error: ftruncate(): '%s'", strerror(errno));
                    break;
                }
                if (DesiredAccess & XBOXKRNL_FILE_APPEND_DATA &&
                    lseek(f.fd, 0, SEEK_END) < 0) {
                    PRINT("error: lseek(): '%s'", strerror(errno));
                    break;
                }
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
        if (!(*FileHandle = xboxkrnl_malloc(sizeof(**FileHandle)))) INT3;
        **FileHandle = f;
        VARDUMP(DUMP, *FileHandle);
        VARDUMP(DUMP, (*FileHandle)->fd);
        VARDUMP(DUMP, (*FileHandle)->dir);
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
static STDCALL int
xboxkrnl_NtDuplicateObject( /* 197 (0x0c5) */
        /* in */  xboxkrnl_file *SourceHandle,
        /* out */ xboxkrnl_file **TargetHandle,
        /* in */  uint32_t Options) {
    ENTER;
    VARDUMP(VAR_IN,   SourceHandle);
    STRDUMP(SourceHandle->path);
    VARDUMP(VAR_OUT,  TargetHandle);
    VARDUMPD(VAR_OUT, TargetHandle);
    VARDUMP3(VAR_IN,  Options, xboxkrnl_file_duplicate_options_name);

    if (!(*TargetHandle = xboxkrnl_malloc(sizeof(**TargetHandle)))) INT3;
    **TargetHandle = *SourceHandle;
    VARDUMP(DUMP, *TargetHandle);
    STRDUMP((*TargetHandle)->path);

    if (SourceHandle->fd >= 0) {
        if (((*TargetHandle)->fd = dup(SourceHandle->fd)) < 0) {
            PRINT("error: dup(): '%s'", strerror(errno));
            INT3;
        }
        VARDUMP(DUMP, SourceHandle->fd);
        VARDUMP(DUMP, (*TargetHandle)->fd);
    }
    if (SourceHandle->dir) INT3;

    if (Options & XBOXKRNL_DUPLICATE_CLOSE_SOURCE /* 1 */) xboxkrnl_NtClose(SourceHandle);

    LEAVE;
    return 0;
}
static STDCALL void
xboxkrnl_NtFlushBuffersFile() { /* 198 (0x0c6) */
    ENTER;

    INT3;
    

    LEAVE;
}
static STDCALL int
xboxkrnl_NtFreeVirtualMemory( /* 199 (0x0c7) */
        /* in */ void **BaseAddress,
        /* in */ size_t *RegionSize,
        /* in */ uint32_t FreeType) {
    register int ret = -1;
    ENTER;
    VARDUMP(VAR_IN,  BaseAddress);
    VARDUMP(VAR_IN,  *BaseAddress);
    VARDUMP(VAR_IN,  RegionSize);
    VARDUMP(VAR_IN,  *RegionSize);
    VARDUMP3(VAR_IN, FreeType, xboxkrnl_mem_type_name);

    if (FreeType & XBOXKRNL_MEM_RELEASE /* 0x8000 */) {
        if (xboxkrnl_munmap(BaseAddress, *RegionSize) < 0) {
            PRINT("error: munmap(): '%s'", strerror(errno));
        } else {
            ret = 0;
        }
    } else if (FreeType & XBOXKRNL_MEM_DECOMMIT /* 0x4000 */) {
        ret = 0;
    }

    if (ret) INT3;

    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
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
    register char *path;
    register int ret = -1;
    ENTER;
    VARDUMP(VAR_OUT, FileHandle);
    VARDUMP3(VAR_IN, DesiredAccess, xboxkrnl_file_access_mask_name);
    VARDUMP(VAR_IN,  ObjectAttributes);
    VARDUMP(VAR_IN,  ObjectAttributes->RootDirectory);
    VARDUMP(VAR_IN,  ObjectAttributes->ObjectName);
    STRDUMP(ObjectAttributes->ObjectName->Buffer);
    VARDUMP3(VAR_IN, ObjectAttributes->Attributes, xboxkrnl_obj_attributes_name);
    VARDUMP(VAR_OUT, IoStatusBlock);
    VARDUMP(VAR_OUT, IoStatusBlock->Status);
    VARDUMP(VAR_OUT, IoStatusBlock->Information);
    VARDUMP(VAR_IN,  ShareAccess);
    VARDUMP3(VAR_IN, OpenOptions, xboxkrnl_file_open_create_name);

    path = xboxkrnl_path_winnt_to_unix(ObjectAttributes->ObjectName->Buffer);
    STRDUMP(path);

    if (OpenOptions & XBOXKRNL_FILE_OPEN_FOR_FREE_SPACE_QUERY /* 0x800000 */) {
        if (!(*FileHandle = xboxkrnl_calloc(1, sizeof(**FileHandle)))) INT3;
        strncpy((*FileHandle)->path, path, sizeof((*FileHandle)->path) - 1);
        (*FileHandle)->fd = -1;
        VARDUMP(DUMP, *FileHandle);
        STRDUMP((*FileHandle)->path);
        VARDUMP(DUMP, (*FileHandle)->fd);
        VARDUMP(DUMP, (*FileHandle)->dir);
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
    register void *buf = NULL;
    register size_t sz;
    register int ret = -1;
    ENTER;
    VARDUMP(VAR_IN,  FileHandle);
    STRDUMP(FileHandle->path);
    VARDUMP(VAR_IN,  FileHandle->fd);
    VARDUMP(VAR_IN,  FileHandle->dir);
    VARDUMP(VAR_OUT, IoStatusBlock);
    VARDUMP(VAR_OUT, IoStatusBlock->Status);
    VARDUMP(VAR_OUT, IoStatusBlock->Information);
    VARDUMP(VAR_OUT, FileInformation);
    VARDUMP(VAR_IN,  Length);
    VARDUMP2(VAR_IN, FileInformationClass, xboxkrnl_file_information_class_name);

    IoStatusBlock->Information = 0;

    switch (FileInformationClass) {
    case XBOXKRNL_FilePositionInformation /* 14 */:
        {
            register xboxkrnl_FilePositionInformation *i;

            if (!(i = calloc(1, (sz = sizeof(*i))))) INT3;
            if ((i->CurrentByteOffset = lseek(FileHandle->fd, 0, SEEK_CUR)) < 0) {
                PRINT("error: lseek(): '%s'", strerror(errno));
                free(i);
                break;
            }
            VARDUMP(DUMP, i->CurrentByteOffset);
            buf = i;
            ret = 0;
        }
        break;
    case XBOXKRNL_FileNetworkOpenInformation /* 34 */:
        {
            register xboxkrnl_FileNetworkOpenInformation *i;
            struct stat st;

            if (!(i = calloc(1, (sz = sizeof(*i))))) INT3;
            if (fstat(FileHandle->fd, &st) < 0) {
                PRINT("error: fstat(): '%s'", strerror(errno));
                free(i);
                break;
            }
            i->CreationTime   = st.st_ctime;
            i->LastAccessTime = st.st_atime;
            i->LastWriteTime  = st.st_mtime;
            i->ChangeTime     = st.st_mtime;
            i->AllocationSize = st.st_size;
            i->EndOfFile      = st.st_size;
            i->FileAttributes = 0;
            VARDUMP(DUMP, i->CreationTime);
            VARDUMP(DUMP, i->LastAccessTime);
            VARDUMP(DUMP, i->LastWriteTime);
            VARDUMP(DUMP, i->ChangeTime);
            VARDUMP(DUMP, i->AllocationSize);
            VARDUMP(DUMP, i->EndOfFile);
            VARDUMP(DUMP, i->FileAttributes);
            buf = i;
            ret = 0;
        }
        break;
    default:
        INT3;
        break;
    }

    if ((IoStatusBlock->Status = ret)) INT3;
    else {
        if (sz > Length) sz = Length;
        memcpy(FileInformation, buf, sz);
        free(buf);
        IoStatusBlock->Information = sz;
    }

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
        /* out */ xboxkrnl_mem_basic *MemoryInformation) {
    register int ret = -1;
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
} PACKED xboxkrnl_mem_basic;
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
        /* in */  xboxkrnl_file *FileHandle,
        /* out */ xboxkrnl_iostatus *IoStatusBlock,
        /* out */ void *FsInformation,
        /* in */  uint32_t Length,
        /* in */  int FsInformationClass) {
    union {
        struct {
            int64_t     TotalAllocationUnits;
            int64_t     AvailableAllocationUnits;
            uint32_t    SectorsPerAllocationUnit;
            uint32_t    BytesPerSector;
        } PACKED FileFsSizeInformation; /* 3 */
    } u;//FIXME
    register size_t sz;
    register int ret = -1;
    ENTER;
    VARDUMP(VAR_IN,  FileHandle);
    STRDUMP(FileHandle->path);
    VARDUMP(VAR_IN,  FileHandle->fd);
    VARDUMP(VAR_IN,  FileHandle->dir);
    VARDUMP(VAR_OUT, IoStatusBlock);
    VARDUMP(VAR_OUT, IoStatusBlock->Status);
    VARDUMP(VAR_OUT, IoStatusBlock->Information);
    VARDUMP(VAR_OUT, FsInformation);
    VARDUMP(VAR_IN,  Length);
    VARDUMP(VAR_IN,  FsInformationClass);

    IoStatusBlock->Information = 0;

    switch (FsInformationClass) {
    case /* FileFsSizeInformation */ 3:
        {
            u.FileFsSizeInformation.SectorsPerAllocationUnit = 32;
            u.FileFsSizeInformation.BytesPerSector = 512;
            VARDUMP(DUMP, u.FileFsSizeInformation.SectorsPerAllocationUnit);
            VARDUMP(DUMP, u.FileFsSizeInformation.BytesPerSector);
            sz  = sizeof(u.FileFsSizeInformation);
            ret = 0;
        }
        break;
    default:
        INT3;
        break;
    }

    if ((IoStatusBlock->Status = ret)) INT3;
    else {
        if (sz > Length) sz = Length;
        memcpy(FsInformation, &u, sz);
        IoStatusBlock->Information = sz;
    }

    VARDUMP(DUMP, IoStatusBlock->Status);
    VARDUMP(DUMP, IoStatusBlock->Information);
    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
}
static STDCALL int
xboxkrnl_NtReadFile( /* 219 (0x0db) */
        /* in */       xboxkrnl_file *FileHandle,
        /* in (opt) */ void *Event,
        /* in (opt) */ void *ApcRoutine,
        /* in (opt) */ void *ApcContext,
        /* out */      xboxkrnl_iostatus *IoStatusBlock,
        /* out */      void *Buffer,
        /* in */       uint32_t Length,
        /* in (opt) */ int64_t *ByteOffset) {
    register ssize_t rd;
    register int ret = -1;
    ENTER;
    VARDUMP(VAR_IN,  FileHandle);
    STRDUMP(FileHandle->path);
    VARDUMP(VAR_IN,  FileHandle->fd);
    VARDUMP(VAR_IN,  FileHandle->dir);
    VARDUMP(VAR_IN,  Event);
    VARDUMP(VAR_IN,  ApcRoutine);
    VARDUMP(VAR_IN,  ApcContext);
    VARDUMP(VAR_OUT, IoStatusBlock);
    VARDUMP(VAR_OUT, IoStatusBlock->Status);
    VARDUMP(VAR_OUT, IoStatusBlock->Information);
    VARDUMP(VAR_OUT, Buffer);
    VARDUMP(VAR_IN,  Length);
    VARDUMP(VAR_IN,  ByteOffset);
    VARDUMPD(VAR_IN, ByteOffset);

    IoStatusBlock->Information = 0;

    do {
        VARDUMP(DUMP, lseek(FileHandle->fd, 0, SEEK_CUR));
        if (ByteOffset) INT3;
        if ((rd = read(FileHandle->fd, Buffer, Length)) < 0) {
            PRINT("error: read(): '%s'", strerror(errno));
            break;
        }
    } while ((ret = 0));

    if ((IoStatusBlock->Status = ret)) INT3;
    else IoStatusBlock->Information = rd;

    VARDUMP(DUMP, IoStatusBlock->Status);
    VARDUMP(DUMP, IoStatusBlock->Information);
    VARDUMP(DUMP, ret);
//INT3;//XXX
    LEAVE;
    return ret;
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
static STDCALL int
xboxkrnl_NtSetInformationFile( /* 226 (0x0e2) */
        /* in */  xboxkrnl_file *FileHandle,
        /* out */ xboxkrnl_iostatus *IoStatusBlock,
        /* in */  void *FileInformation,
        /* in */  uint32_t Length,
        /* in */  int FileInformationClass) {
    register size_t pos = 0;
    register size_t sz;
    register int ret = -1;
    ENTER;
    VARDUMP(VAR_IN,  FileHandle);
    STRDUMP(FileHandle->path);
    VARDUMP(VAR_IN,  FileHandle->fd);
    VARDUMP(VAR_IN,  FileHandle->dir);
    VARDUMP(VAR_OUT, IoStatusBlock);
    VARDUMP(VAR_OUT, IoStatusBlock->Status);
    VARDUMP(VAR_OUT, IoStatusBlock->Information);
    VARDUMP(VAR_IN,  FileInformation);
    VARDUMP(VAR_IN,  Length);
    VARDUMP2(VAR_IN, FileInformationClass, xboxkrnl_file_information_class_name);

    IoStatusBlock->Information = 0;

    switch (FileInformationClass) {
#define SET \
            ret = 0; \
            if ((sz = sizeof(*i)) > Length) sz = Length
#define INC(x) \
            if ((pos += sizeof(x)) > sz) break; \
            VARDUMP(DUMP, x)
#define BREAK \
            ret = -1; \
            break
    case XBOXKRNL_FilePositionInformation /* 14 */:
        {
            register xboxkrnl_FilePositionInformation *i = FileInformation;

            SET;
            INC(i->CurrentByteOffset);
            if (lseek(FileHandle->fd, i->CurrentByteOffset, SEEK_SET) < 0) {
                PRINT("error: lseek(): '%s'", strerror(errno));
                BREAK;
            }
        }
        break;
#if 0 //TODO
    case XBOXKRNL_FileNetworkOpenInformation /* 34 */:
        {
            register xboxkrnl_FileNetworkOpenInformation *i;
            struct stat st;

            if (!(i = calloc(1, (sz = sizeof(*i))))) INT3;
            if (fstat(FileHandle->fd, &st) < 0) {
                PRINT("error: fstat(): '%s'", strerror(errno));
                free(i);
                break;
            }
            i->CreationTime   = st.st_ctime;
            i->LastAccessTime = st.st_atime;
            i->LastWriteTime  = st.st_mtime;
            i->ChangeTime     = st.st_mtime;
            i->AllocationSize = st.st_size;
            i->EndOfFile      = st.st_size;
            i->FileAttributes = 0;
            VARDUMP(DUMP, i->CreationTime);
            VARDUMP(DUMP, i->LastAccessTime);
            VARDUMP(DUMP, i->LastWriteTime);
            VARDUMP(DUMP, i->ChangeTime);
            VARDUMP(DUMP, i->AllocationSize);
            VARDUMP(DUMP, i->EndOfFile);
            VARDUMP(DUMP, i->FileAttributes);
            buf = i;
            ret = 0;
        }
        break;
#endif
    default:
        INT3;
        break;
#undef BREAK
#undef INC
#undef SET
    }

    if ((IoStatusBlock->Status = ret)) INT3;
    else IoStatusBlock->Information = sz;

    VARDUMP(DUMP, IoStatusBlock->Status);
    VARDUMP(DUMP, IoStatusBlock->Information);
    VARDUMP(DUMP, ret);
    LEAVE;
    return ret;
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
    register ssize_t wr;
    register int ret = -1;
    ENTER;
    VARDUMP(VAR_IN,  FileHandle);
    STRDUMP(FileHandle->path);
    VARDUMP(VAR_IN,  FileHandle->fd);
    VARDUMP(VAR_IN,  FileHandle->dir);
    VARDUMP(VAR_IN,  Event);
    VARDUMP(VAR_IN,  ApcRoutine);
    VARDUMP(VAR_IN,  ApcContext);
    VARDUMP(VAR_OUT, IoStatusBlock);
    VARDUMP(VAR_OUT, IoStatusBlock->Status);
    VARDUMP(VAR_OUT, IoStatusBlock->Information);
    VARDUMP(VAR_IN,  Buffer);
    VARDUMP(VAR_IN,  Length);
    VARDUMP(VAR_IN,  ByteOffset);
    VARDUMPD(VAR_IN, ByteOffset);

    IoStatusBlock->Information = 0;

    do {
        VARDUMP(DUMP, lseek(FileHandle->fd, 0, SEEK_CUR));
        if (ByteOffset) INT3;
        if ((wr = write(FileHandle->fd, Buffer, Length)) < 0) {
            PRINT("error: write(): '%s'", strerror(errno));
            break;
        }
    } while ((ret = 0));

    if ((IoStatusBlock->Status = ret)) INT3;
    else IoStatusBlock->Information = wr;

    VARDUMP(DUMP, IoStatusBlock->Status);
    VARDUMP(DUMP, IoStatusBlock->Information);
    VARDUMP(DUMP, ret);
//INT3;//XXX
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
    // sched_yield / pthread_yield

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
            /* KERNEL_STACK_SIZE */ 12288,
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

    PRINT("/* calling thread is system thread */", 0);
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

    /* FIXME: some locks are passed from the stack in game code */
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
    register size_t len1;
    register size_t len2;
    register uint8_t ret = 0;
    ENTER;
    VARDUMP(VAR_IN, String1);
    STRDUMP(String1->Buffer);
    VARDUMP(VAR_IN, String2);
    STRDUMP(String2->Buffer);
    VARDUMP(VAR_IN, CaseInSensitive);

    len1 = strlen(String1->Buffer) + 1;
    len2 = strlen(String2->Buffer) + 1;

    if (len1 == len2 &&
        ((CaseInSensitive && !strncasecmp(String1->Buffer, String2->Buffer, len1)) ||
        (!CaseInSensitive && !strncmp(String1->Buffer, String2->Buffer, len1)))) ret = 1;

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
    STRDUMP(SourceString);

    DestinationString->Length = strlen(SourceString);
    DestinationString->MaximumLength = DestinationString->Length + 1;
    DestinationString->Buffer = SourceString;

    VARDUMP(DUMP, DestinationString->Length);
    VARDUMP(DUMP, DestinationString->MaximumLength);
    VARDUMP(DUMP, DestinationString->Buffer);
    STRDUMP(DestinationString->Buffer);
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

    if (!(*CriticalSection = xboxkrnl_calloc(1, sizeof(**CriticalSection)))) INT3;
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
    register int ret;
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
enum XBOXKRNL_XBOXHARDWAREINFO_FLAGS {
    XBOXKRNL_HW_FLAG_INTERNAL_USB_HUB       = 1 << 0,   /* 0x1 */
    XBOXKRNL_HW_FLAG_DEVKIT_KERNEL          = 1 << 1,   /* 0x2 */
    XBOXKRNL_HW_FLAG_480P_MACROVISION       = 1 << 2,   /* 0x4 */
    XBOXKRNL_HW_FLAG_ARCADE                 = 1 << 3,   /* 0x8 */
};
typedef struct {
    uint32_t                Flags;                  /* 0 */
    uint8_t                 GpuRevision;            /* 4 */
    uint8_t                 McpRevision;            /* 5 */
    uint8_t                 reserved[2];            /* 6 */
} PACKED xboxkrnl_XboxHardwareInfo_t;
static xboxkrnl_XboxHardwareInfo_t
xboxkrnl_XboxHardwareInfo; /* 322 (0x142) */
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
typedef struct {
    uint16_t                Major;                  /* 0 */
    uint16_t                Minor;                  /* 2 */
    uint16_t                Build;                  /* 4 */
    uint16_t                Qfe;                    /* 6 */
} PACKED xboxkrnl_XboxKrnlVersion_t;
static xboxkrnl_XboxKrnlVersion_t
xboxkrnl_XboxKrnlVersion; /* 324 (0x144) */
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
    /* 164 */ [0x0a4] = &xboxkrnl_LaunchDataPage,                       /* variable */
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

static void
xboxkrnl_destroy_var(void) {
//    register void **p;
    ENTER;
#if 0
#define FREE(x) \
    if (*(p = &(x))) { \
        free(*p); \
        *p = NULL; \
    }
//    FREE(xboxkrnl_XboxHardwareInfo);
#undef FREE
#endif
#define ZERO(x) \
    memset(&(x), 0, sizeof((x)))
    ZERO(xboxkrnl_LaunchDataPage);
    ZERO(xboxkrnl_XboxKrnlVersion);
    ZERO(xboxkrnl_XboxHardwareInfo);
#undef ZERO

    LEAVE;
}

static int
xboxkrnl_init_var(void) {
//    register void **p;
    register int ret = 1;
    ENTER;

    xboxkrnl_destroy_var();

    do {
#if 0
#define ALLOC(x) \
        p = &(x); \
        if (!(*p = calloc(1, sizeof(*(x))))) break
//        ALLOC(xboxkrnl_XboxHardwareInfo);
#undef ALLOC
#endif
        xboxkrnl_XboxHardwareInfo.Flags       = XBOXKRNL_HW_FLAG_DEVKIT_KERNEL | XBOXKRNL_HW_FLAG_ARCADE;
        xboxkrnl_XboxHardwareInfo.GpuRevision = 0xa2;
        xboxkrnl_XboxHardwareInfo.McpRevision = 0xb1;
        xboxkrnl_XboxKrnlVersion.Major = 1;
        xboxkrnl_XboxKrnlVersion.Minor = 0;
        xboxkrnl_XboxKrnlVersion.Build = 5838;
        xboxkrnl_XboxKrnlVersion.Qfe   = 1;
    } while ((ret = 0));

    LEAVE;
    return ret;
}

