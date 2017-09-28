/* gcc -m32 -g -Wall -Wextra loader.c -o loader -pthread */

#ifndef __i386__
# error "Compile with `gcc -m32'"
#endif

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <ucontext.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <errno.h>

#include <pthread.h>

static int dbg = 1;

void
debug(const char *format, ...) {
    va_list args;
    char buf[4096];
    char time[32];
    struct timeval tv;
    struct tm *tm;
    int ret;

    if (!dbg) return;

    va_start(args, format);
    ret = sizeof(buf) - 1;
    ret = vsnprintf(buf, ret, format, args);
    va_end(args);

    if (ret > 0) {
        buf[ret] = 0;
        gettimeofday(&tv, NULL);
        if (!(tm = localtime(&tv.tv_sec)) || !strftime(time, sizeof(time), "%T", tm)) {
            fprintf(stderr, "%s\n", buf);
        } else {
            fprintf(stderr, "%s.%06lu: %s\n", time, tv.tv_usec, buf);
        }
    }
}

#define hexdump(x,y) fhexdump(stderr, (x), (y))

void
fhexdump(FILE *stream, const void *data, size_t len) {
    char buf[66];
    char *b;
    const unsigned char *d = data;
    int i;

    for (i = 0; i < (int)sizeof(buf); ++i) buf[i] = ' ';

    for (b = buf; len; b = buf) {
        for (i = 0; i < 16; ++i) {
            if (len) {
                *b++ = "0123456789abcdef"[*d >> 4];
                *b++ = "0123456789abcdef"[*d & 0xf];
                ++b;
                buf[50 + i] = (*d >= ' ' && *d < 0x7f) ? *d : '.';
                ++d, --len;
            } else {
                *b++ = ' ';
                *b++ = ' ';
                *b++ = ' ';
                buf[50 + i] = 0;
            }
        }
        fprintf(stream, "%.*s\n", (int)sizeof(buf), buf);
    }
}

#define PACKED              __attribute__((__packed__))

#define PAGESIZE            0x1000
#define PAGEMASK            (PAGESIZE - 1)

#define XBE_MAGIC           "XBEH"
#define XBE_BASE_ADDR       0x10000

#define XBE_XOR_EP_DEBUG    0x94859d4b      /* entry point (Debug) */
#define XBE_XOR_EP_RETAIL   0xa8fc57ab      /* entry point (Retail) */
#define XBE_XOR_EP_CHIHIRO  0x40b5c16e      /* entry point (Chihiro) */
#define XBE_XOR_KT_DEBUG    0xefb1f152      /* kernel thunk (Debug) */
#define XBE_XOR_KT_RETAIL   0x5b6d40b6      /* kernel thunk (Retail) */
#define XBE_XOR_KT_CHIHIRO  0x2290059d      /* kernel thunk (Chihiro) */

typedef struct {
    uint32_t dwMagic;                       /* 0x0000 - magic number [should be "XBEH"] */
    uint8_t  pbDigitalSignature[256];       /* 0x0004 - digital signature */
    uint32_t dwBaseAddr;                    /* 0x0104 - base address */
    uint32_t dwSizeofHeaders;               /* 0x0108 - size of headers */
    uint32_t dwSizeofImage;                 /* 0x010c - size of image */
    uint32_t dwSizeofImageHeader;           /* 0x0110 - size of image header */
    uint32_t dwTimeDate;                    /* 0x0114 - timedate stamp */
    uint32_t dwCertificateAddr;             /* 0x0118 - certificate address */
    uint32_t dwSections;                    /* 0x011c - number of sections */
    uint32_t dwSectionHeadersAddr;          /* 0x0120 - section headers address */

    struct {                                /* 0x0124 - initialization flags */
        uint32_t bMountUtilityDrive   : 1;  /* 0x0124 - mount utility drive flag */
        uint32_t bFormatUtilityDrive  : 1;  /* 0x0124 - format utility drive flag */
        uint32_t bLimit64MB           : 1;  /* 0x0124 - limit development kit run time memory to 64mb flag */
        uint32_t bDontSetupHarddisk   : 1;  /* 0x0124 - don't setup hard disk flag */
        uint32_t Unused1              : 4;  /* 0x0124 - unused */
        uint32_t Unused2              : 8;  /* 0x0125 - unused */
        uint32_t Unused3              : 8;  /* 0x0126 - unused */
        uint32_t Unused4              : 8;  /* 0x0127 - unused */
    } PACKED dwInitFlags;

    uint32_t dwEntryAddr;                   /* 0x0128 - entry point address */
    uint32_t dwTLSAddr;                     /* 0x012c - thread local storage directory address */
    uint32_t dwPeStackCommit;               /* 0x0130 - size of stack commit */
    uint32_t dwPeHeapReserve;               /* 0x0134 - size of heap reserve */
    uint32_t dwPeHeapCommit;                /* 0x0138 - size of heap commit */
    uint32_t dwPeBaseAddr;                  /* 0x013c - original base address */
    uint32_t dwPeSizeofImage;               /* 0x0140 - size of original image */
    uint32_t dwPeChecksum;                  /* 0x0144 - original checksum */
    uint32_t dwPeTimeDate;                  /* 0x0148 - original timedate stamp */
    uint32_t dwDebugPathnameAddr;           /* 0x014c - debug pathname address */
    uint32_t dwDebugFilenameAddr;           /* 0x0150 - debug filename address */
    uint32_t dwDebugUnicodeFilenameAddr;    /* 0x0154 - debug unicode filename address */
    uint32_t dwKernelImageThunkAddr;        /* 0x0158 - kernel image thunk address */
    uint32_t dwNonKernelImportDirAddr;      /* 0x015c - non kernel import directory address */
    uint32_t dwLibraryVersions;             /* 0x0160 - number of library versions */
    uint32_t dwLibraryVersionsAddr;         /* 0x0164 - library versions address */
    uint32_t dwKernelLibraryVersionAddr;    /* 0x0168 - kernel library version address */
    uint32_t dwXAPILibraryVersionAddr;      /* 0x016c - xapi library version address */
    uint32_t dwLogoBitmapAddr;              /* 0x0170 - logo bitmap address */
    uint32_t dwSizeofLogoBitmap;            /* 0x0174 - logo bitmap size */
} PACKED XbeHeader;

typedef struct {
    struct {                                /* 0x0000 - section flags */
        uint32_t bWritable        : 1;      /* 0x0000 - writable flag */
        uint32_t bPreload         : 1;      /* 0x0000 - preload flag */
        uint32_t bExecutable      : 1;      /* 0x0000 - executable flag */
        uint32_t bInsertedFile    : 1;      /* 0x0000 - inserted file flag */
        uint32_t bHeadPageRO      : 1;      /* 0x0000 - head page read only flag */
        uint32_t bTailPageRO      : 1;      /* 0x0000 - tail page read only flag */
        uint32_t Unused1          : 1;      /* 0x0000 - unused */
        uint32_t Unused2          : 1;      /* 0x0000 - unused */
        uint32_t Unused3          : 8;      /* 0x0001 - unused */
        uint32_t Unused4          : 8;      /* 0x0002 - unused */
        uint32_t Unused5          : 8;      /* 0x0003 - unused */
    } PACKED dwFlags;

    uint32_t dwVirtualAddr;                 /* 0x0004 - virtual address */
    uint32_t dwVirtualSize;                 /* 0x0008 - virtual size */
    uint32_t dwRawAddr;                     /* 0x000c - file offset to raw data */
    uint32_t dwSizeofRaw;                   /* 0x0010 - size of raw data */
    uint32_t dwSectionNameAddr;             /* 0x0014 - section name addr */
    uint32_t dwSectionRefCount;             /* 0x0018 - section reference count */
    uint32_t dwHeadSharedRefCountAddr;      /* 0x001c - head shared page reference count address */
    uint32_t dwTailSharedRefCountAddr;      /* 0x0020 - tail shared page reference count address */
    uint8_t  bzSectionDigest[20];           /* 0x0024 - section digest */
} PACKED XbeSectionHeader;

typedef struct {
    uint32_t dwDataStartAddr;               /* 0x0000 - raw start address */
    uint32_t dwDataEndAddr;                 /* 0x0004 - raw end address */
    uint32_t dwTLSIndexAddr;                /* 0x0008 - tls index address */
    uint32_t dwTLSCallbackAddr;             /* 0x000c - tls callback address */
    uint32_t dwSizeofZeroFill;              /* 0x0010 - size of zero fill */
    uint32_t dwCharacteristics;             /* 0x0014 - characteristics */
} PACKED XbeTLS;

#include "xboxkrnl.c"
#if 0
#define X86_SEH_PROLOG \
        "\x68\xb0\x6a\x0f\x00\x64\xa1\x00\x00\x00\x00\x50\x8b\x44\x24\x10" \
        "\x89\x6c\x24\x10\x8d\x6c\x24\x10\x2b\xe0\x53\x56\x57\x8b\x45\xf8" \
        "\x89\x65\xe8\x50\x8b\x45\xfc\xc7\x45\xfc\xff\xff\xff\xff\x89\x45" \
        "\xf8\x8d\x45\xf0\x64\xa3\x00\x00\x00\x00\xc3"
#define X86_SEH_PROLOG_PATCH \
        "\x68\xb0\x6a\x0f\x00\x90\x90\x90\x90\x90\x90\x50\x8b\x44\x24\x10" \
        "\x89\x6c\x24\x10\x8d\x6c\x24\x10\x2b\xe0\x53\x56\x57\x8b\x45\xf8" \
        "\x89\x65\xe8\x50\x8b\x45\xfc\xc7\x45\xfc\xff\xff\xff\xff\x89\x45" \
        "\xf8\x90\x90\x90\x90\x90\x90\x90\x90\x90\xc3"
#define X86_SEH_EPILOG \
        "\x8b\x4d\xf0\x64\x89\x0d\x00\x00\x00\x00\x59\x5f\x5e\x5b\xc9\x51" \
        "\xc3"
#define X86_SEH_EPILOG_PATCH \
        "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x59\x5f\x5e\x5b\xc9\x51" \
        "\xc3"

int
xboxkrnl_patch_seh(void *addr, size_t size) {
    size_t i;
    size_t tmp;
    int ret = 0;
    ENTER;

    PRINT("/* searching & patching SEH prolog & epilog routines... */", 0);

    for (i = 0; !ret && i <= size; ++i) {
        if (i >= size) {
            PRINT("/* ... not found. */", 0);
            break;
        }
        for (tmp = 0; i + tmp < size && tmp <= sizeof(X86_SEH_PROLOG) - 1; ++tmp) {
            if (tmp >= sizeof(X86_SEH_PROLOG) - 1) {
                if (!memcmp(addr + i + tmp,
                        X86_SEH_EPILOG,
                        sizeof(X86_SEH_EPILOG) - 1)) {
                    PRINT("/* ... found! */", 0);
                    PRINT("/* SEH prolog & epilog @ %p */", addr + i);
//                    *((char *)addr + i)             = 0xc3;
                    memcpy(addr + i,
                            X86_SEH_PROLOG_PATCH,
                            sizeof(X86_SEH_PROLOG_PATCH) - 1);
                    PRINT("/* SEH prolog patched @ %p */", addr + i);
//                    *((char *)addr + i + tmp)       = 0xc3;
                    memcpy(addr + i + tmp,
                            X86_SEH_EPILOG_PATCH,
                            sizeof(X86_SEH_EPILOG_PATCH) - 1);
                    PRINT("/* SEH epilog patched @ %p */", addr + i + tmp);
                    ret = 2;
                } else {
                    PRINT("/* ... found! */", 0);
                    PRINT("/* SEH prolog @ %p */", addr + i);
//                    *((char *)addr + i)             = 0xc3;
                    memcpy(addr + i,
                            X86_SEH_PROLOG_PATCH,
                            sizeof(X86_SEH_PROLOG_PATCH) - 1);
                    PRINT("/* SEH prolog patched @ %p */", addr + i);
                    ret = 1;
                }
                break;
            }
            if (((char *)addr)[i + tmp] != X86_SEH_PROLOG[tmp]) break;
        }
    }
    if (ret == 1) {
        PRINT("/* searching & patching SEH epilog routine... */", 0);

        for (i = 0; ret == 1 && i <= size; ++i) {
            if (i >= size) {
                PRINT("/* ... not found. */", 0);
                break;
            }
            for (tmp = 0; i + tmp < size && tmp <= sizeof(X86_SEH_EPILOG) - 1; ++tmp) {
                if (tmp >= sizeof(X86_SEH_EPILOG) - 1) {
                    PRINT("/* ... found! */", 0);
                    PRINT("/* SEH epilog @ %p */", addr + i);
//                    *((char *)addr + i)             = 0xc3;
                    memcpy(addr + i,
                            X86_SEH_EPILOG_PATCH,
                            sizeof(X86_SEH_EPILOG_PATCH) - 1);
                    PRINT("/* SEH epilog patched @ %p */", addr + i);
                    ret = 2;
                    break;
                }
                if (((char *)addr)[i + tmp] != X86_SEH_EPILOG[tmp]) break;
            }
        }
    }

    LEAVE;
    return ret;
}
#endif
static const char *const greg_names[] = {
    NAME(REG_GS),
    NAME(REG_FS),
    NAME(REG_ES),
    NAME(REG_DS),
    NAME(REG_EDI),
    NAME(REG_ESI),
    NAME(REG_EBP),
    NAME(REG_ESP),
    NAME(REG_EBX),
    NAME(REG_EDX),
    NAME(REG_ECX),
    NAME(REG_EAX),
    NAME(REG_TRAPNO),
    NAME(REG_ERR),
    NAME(REG_EIP),
    NAME(REG_CS),
    NAME(REG_EFL),
    NAME(REG_UESP),
    NAME(REG_SS),
};

static const char *const signal_segv_si_codes[] = {
    "",
    "SEGV_MAPERR",  /* Address not mapped to object. */
    "SEGV_ACCERR",  /* Invalid permissions for mapped object. */
    "SEGV_BNDERR",  /* (since Linux 3.19) Failed address bound checks. */
    "SEGV_PKUERR"   /* (since Linux 4.6) Access was denied by memory protection keys. */
};

struct eflags {
    uint32_t cf   : 1;  /*  0 - carry flag */
    uint32_t r1   : 1;  /*  1 - reserved */
    uint32_t pf   : 1;  /*  2 - parity flag */
    uint32_t r2   : 1;  /*  3 - reserved */
    uint32_t af   : 1;  /*  4 - adjust flag */
    uint32_t r3   : 1;  /*  5 - reserved */
    uint32_t zf   : 1;  /*  6 - zero flag */
    uint32_t sf   : 1;  /*  7 - sign flag */
    uint32_t tf   : 1;  /*  8 - trap flag */
    uint32_t ief  : 1;  /*  9 - interrupt enable flag */
    uint32_t df   : 1;  /* 10 - direction flag */
    uint32_t of   : 1;  /* 11 - overflow flag */
    uint32_t iopl : 2;  /* 12 - i/o privilege level */
    uint32_t nt   : 1;  /* 14 - nested task flag */
    uint32_t r4   : 1;  /* 15 - reserved */
    uint32_t rf   : 1;  /* 16 - resume flag */
    uint32_t vm   : 1;  /* 17 - virtual 8086 mode flag */
    uint32_t ac   : 1;  /* 18 - alignment check flag */
    uint32_t vif  : 1;  /* 19 - virtual interrupt flag */
    uint32_t vip  : 1;  /* 20 - virtual interrupt pending flag */
    uint32_t id   : 1;  /* 21 - cpuid instruction permission flag */
    uint32_t r5   : 10; /* 22 - reserved */
};

static void
signal_segv(int signum, siginfo_t *info, void *ptr) {
    ucontext_t *uc = ptr;
    register void **bp;
    register void **ip = (void **)&uc->uc_mcontext.gregs[REG_EIP];
    register struct eflags *fl;
    register const char *n;
    register uint32_t i = info->si_code;
    uint32_t v;

    if (i == SEGV_ACCERR) {
#if 0
A1 04 18 00 FD                  // mov     eax, ds:0FD001804h
A3 04 18 00 FD                  // mov     ds:0FD001804h, eax
C7 05 40 01 60 FD 00 00 00 00   // mov     dword ptr ds:0FD600140h, 0
C7 05 40 91 00 FD 00 00 00 00   // mov     dword ptr ds:0FD009140h, 0
89 9E 00 02 00 00               // mov     [esi+200h], ebx
89 86 40 01 00 00               // mov     [esi+140h], eax
89 AE 00 94 00 00               // mov     [esi+9400h], ebp
C7 87 0C 18 00 00 00 F8 00 00   // mov     dword ptr [edi+180Ch], 0F800h
#endif
        do {
            i = (typeof(i))info->si_addr;
            if ((i & 0xff000000) == 0xfd000000) {
                n = xpci[XPCI_GPU].name;
// cmp     [esi+2100h], edi // 39 BE 00 21 00 00
                if ((*(char *)*ip & 0xff) == 0x39) {                // cmp
                    if ((*(char *)(*ip + 1) & 0xff) == 0xbe) {      // cmp     [esi+<1>], edi
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x39 0xbe <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDI];
                        fl   = (void *)&uc->uc_mcontext.gregs[REG_EFL];
                        if (nv2a_read(i, &v, 4)) {
                            fl->zf = (*(int *)&v == *(int *)bp);
                            PRINT("%s: "
                                "cmp: "
                                "([0x%.08x] (0x%.08x) == "
                                "edi (0x%.08x)) = %u",
                                n,
                                i, *(int *)&v,                      // [esi+<1>]
                                *(int *)bp,                         // edi
                                fl->zf);                            // zero flag
                        }
                        return;
                    }
                }
// cmp     dword ptr [esi+400100h], 0 // 83 BE 00 01 40 00 00
                if ((*(char *)*ip & 0xff) == 0x83) {                // cmp
                    if ((*(char *)(*ip + 1) & 0xff) == 0xbe) {      // cmp     [esi+<1>], <2>
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x83 0xbe <1:dword> <2:byte>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        fl   = (void *)&uc->uc_mcontext.gregs[REG_EFL];
                        if (nv2a_read(i, &v, 4)) {
                            fl->zf = (*(int *)&v == *(char *)*ip);
                            PRINT("%s: "
                                "cmp: "
                                "([0x%.08x] (0x%.08x) == "
                                "0x%.02hhx) = %u",
                                n,
                                i, *(int *)&v,                      // [esi+<1>]
                                *(char *)*ip,                       // <2>
                                fl->zf);                            // zero flag
                        }
                        *ip += 1;                                   // <2>: value, <2> size: byte
                        return;
                    }
                }
// cmp     eax, [ecx] // 3B 01
                if ((*(char *)*ip & 0xff) == 0x3b) {                // cmp
                    if ((*(char *)(*ip + 1) & 0xff) == 0x01) {      // cmp     eax, [ecx]
                        i    = uc->uc_mcontext.gregs[REG_ECX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x3b 0x01
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        fl   = (void *)&uc->uc_mcontext.gregs[REG_EFL];
                        if (nv2a_read(i, &v, 4)) {
                            fl->zf = (*(int *)bp == *(int *)&v);
                            PRINT("%s: "
                                "cmp: "
                                "(eax (0x%.08x) == "
                                "[0x%.08x] (0x%.08x)) = %u",
                                n,
                                *(int *)bp,                         // eax
                                i, *(int *)&v,                      // [ecx]
                                fl->zf);                            // zero flag
                        }
                        return;
                    }
// cmp     eax, [ecx+400908h] // 3B 81 08 09 40 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x81) {      // cmp     eax, [ecx+<1>]
                        i    = uc->uc_mcontext.gregs[REG_ECX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x3b 0x81 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        fl   = (void *)&uc->uc_mcontext.gregs[REG_EFL];
                        if (nv2a_read(i, &v, 4)) {
                            fl->zf = (*(int *)bp == *(int *)&v);
                            PRINT("%s: "
                                "cmp: "
                                "(eax (0x%.08x) == "
                                "[0x%.08x] (0x%.08x)) = %u",
                                n,
                                *(int *)bp,                         // eax
                                i, *(int *)&v,                      // [ecx+<1>]
                                fl->zf);                            // zero flag
                        }
                        return;
                    }
// cmp     eax, [esi+edi*4+400980h] // 3B 84 BE 80 09 40 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x84) {
                        if ((*(char *)(*ip + 2) & 0xff) == 0xbe) {  // cmp     eax, [esi+edi*4+<1>]
                            i    = uc->uc_mcontext.gregs[REG_ESI];
                            i   += uc->uc_mcontext.gregs[REG_EDI] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x3b 0x84 0xbe <1:dword>
                            i   += *(int *)*ip;                     //      <1>: offset
                            *ip += 4;                               // <1> size: dword
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                            fl   = (void *)&uc->uc_mcontext.gregs[REG_EFL];
                            if (nv2a_read(i, &v, 4)) {
                                fl->zf = (*(int *)bp == *(int *)&v);
                                PRINT("%s: "
                                    "cmp: "
                                    "(eax (0x%.08x) == "
                                    "[0x%.08x] (0x%.08x)) = %u",
                                    n,
                                    *(int *)bp,                     // eax
                                    i, *(int *)&v,                  // [esi+edi*4+<1>]
                                    fl->zf);                        // zero flag
                            }
                            return;
                        }
                    }
                }
// test    dword ptr [eax+100410h], 10000h // F7 80 10 04 10 00 00 00 01 00
                if ((*(char *)*ip & 0xff) == 0xf7) {                // test
                    if ((*(char *)(*ip + 1) & 0xff) == 0x80) {      // test    [eax+<1>], <2>
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0xf7 0x80 <1:dword> <2:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        fl   = (void *)&uc->uc_mcontext.gregs[REG_EFL];
                        if (nv2a_read(i, &v, 4)) {
                            fl->zf = !(*(int *)&v & *(int *)*ip);
                            PRINT("%s: "
                                "test: "
                                "!([0x%.08x] (0x%.08x) & "
                                "0x%.08x) = %u",
                                n,
                                i, *(int *)&v,                      // [eax+<1>]
                                *(int *)*ip,                        // <2>
                                fl->zf);                            // zero flag
                        }
                        *ip += 4;                                   // <2>: value, <2> size: dword
                        return;
                    }
                }
// test    byte ptr [esi+3214h], 10h // F6 86 14 32 00 00 10
                if ((*(char *)*ip & 0xff) == 0xf6) {                // test
                    if ((*(char *)(*ip + 1) & 0xff) == 0x86) {      // test    [esi+<1>], <2>
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0xf6 0x86 <1:dword> <2:byte>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        fl   = (void *)&uc->uc_mcontext.gregs[REG_EFL];
                        if (nv2a_read(i, &v, 1)) {
                            fl->zf = !(*(char *)&v & *(char *)*ip);
                            PRINT("%s: "
                                "test: "
                                "!([0x%.08x] (0x%.02hhx) & "
                                "0x%.02hhx) = %u",
                                n,
                                i, *(char *)&v,                     // [esi+<1>]
                                *(char *)*ip,                       // <2>
                                fl->zf);                            // zero flag
                        }
                        *ip += 1;                                   // <2>: value, <2> size: byte
                        return;
                    }
// test    byte ptr [eax], 10h // F6 00 10
                    if ((*(char *)(*ip + 1) & 0xff) == 0x00) {      // test    [eax], <1>
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0xf6 0x00 <1:byte>
                        fl   = (void *)&uc->uc_mcontext.gregs[REG_EFL];
                        if (nv2a_read(i, &v, 1)) {
                            fl->zf = !(*(char *)&v & *(char *)*ip);
                            PRINT("%s: "
                                "test: "
                                "!([0x%.08x] (0x%.02hhx) & "
                                "0x%.02hhx) = %u",
                                n,
                                i, *(char *)&v,                     // [eax]
                                *(char *)*ip,                       // <1>
                                fl->zf);                            // zero flag
                        }
                        *ip += 1;                                   // <1>: value, <1> size: byte
                        return;
                    }
                }
                if ((*(char *)*ip & 0xff) == 0xa1) {                // mov     eax, ds:<1>
                    i    = *(int *)(*ip + 1);                       //      <1>: offset
                    if ((i & 0xff000000) != 0xfd000000) break;
                    *ip += 1;                                       //   opcode: 0xa1 <1:dword>
                    *ip += 4;                                       // <1> size: dword
                    bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
#if 0
                    if (nv2a_read(i, &v, 4)) {
                        PRINT("GPU: mov: "
                            "eax (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                                 // eax
                            i, *(int *)&v);                             // ds:<1>
                        *(int *)bp = *(int *)&v;
                    }
#endif
                    nv2a_read(i, bp, 4);
                    return;
                }
                if ((*(char *)*ip & 0xff) == 0xa3) {                // mov     ds:<1>, eax
                    i    = *(int *)(*ip + 1);                       //      <1>: offset
                    if ((i & 0xff000000) != 0xfd000000) break;
                    *ip += 1;                                       //   opcode: 0xa3 <1:dword>
                    *ip += 4;                                       // <1> size: dword
                    bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
#if 0
                    PRINT("GPU: mov: "
                        "[0x%.08x] (0x%.08x), "
                        "eax (0x%.08x)",
                        i, *(int *)&v,                              // ds:<1>
                        *(int *)bp);                                // eax
                    *(int *)(p + i) = *(int *)bp;
#endif
                    nv2a_write(i, bp, 4);
                    return;
                }
                if ((*(char *)*ip & 0xff) == 0xc7) {                // mov
                    if ((*(char *)(*ip + 1) & 0xff) == 0x05) {      // mov     ds:<1>, <2>
                        i    = *(int *)(*ip + 2);                   //      <1>: offset
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0xc7 0x05 <1:dword> <2:dword>
                        *ip += 4;                                   // <1> size: dword
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "0x%.08x",
                            i, *(int *)&v,                          // ds:<1>
                            *(int *)*ip);                           // <2>
                        *(int *)(p + i) = *(int *)*ip;              //      <2>: value
#endif
                        nv2a_write(i, *ip, 4);
                        *ip += 4;                                   // <2> size: dword
                        return;
                    }
// mov     dword ptr [eax+2500h], 0 // C7 80 00 25 00 00 00 00 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x80) {      // mov     [eax+<1>], <2>
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0xc7 0x80 <1:dword> <2:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "0x%.08x",
                            i, *(int *)&v,                  // [eax+<1>]
                            *(int *)*ip);                           // <2>
                        *(int *)(p + i) = *(int *)*ip;              //      <2>: value
#endif
                        nv2a_write(i, *ip, 4);
                        *ip += 4;                                   // <2> size: dword
                        return;
                    }
// mov     dword ptr [eax+edi*4], 0 // C7 04 B8 00 00 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x04) {
                        if ((*(char *)(*ip + 2) & 0xff) == 0xb8) {  // mov     [eax+edi*4], <1>
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_EDI] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0xc7 0x04 0xb8 <1:dword>
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "0x%.08x",
                                i, *(int *)&v,              // [eax+edi*4]
                                *(int *)*ip);                       // <1>
                            *(int *)(p + i) = *(int *)*ip;          //      <1>: value
#endif
                            nv2a_write(i, *ip, 4);
                            *ip += 4;                               // <1> size: dword
                            return;
                        }
// mov     dword ptr [eax+esi*4], 3F800000h // C7 04 B0 00 00 80 3F
                        if ((*(char *)(*ip + 2) & 0xff) == 0xb0) {  // mov     [eax+esi*4], <1>
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_ESI] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0xc7 0x04 0xb0 <1:dword>
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "0x%.08x",
                                i, *(int *)&v,              // [eax+esi*4]
                                *(int *)*ip);                       // <1>
                            *(int *)(p + i) = *(int *)*ip;          //      <1>: value
#endif
                            nv2a_write(i, *ip, 4);
                            *ip += 4;                               // <1> size: dword
                            return;
                        }
// mov     dword ptr [eax+ebx*4], 40000000h // C7 04 98 00 00 00 40
                        if ((*(char *)(*ip + 2) & 0xff) == 0x98) {  // mov     [eax+ebx*4], <1>
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_EBX] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0xc7 0x04 0x98 <1:dword>
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "0x%.08x",
                                i, *(int *)&v,              // [eax+ebx*4]
                                *(int *)*ip);                       // <1>
                            *(int *)(p + i) = *(int *)*ip;          //      <1>: value
#endif
                            nv2a_write(i, *ip, 4);
                            *ip += 4;                               // <1> size: dword
                            return;
                        }
// mov     dword ptr [ecx+edi], 0 // C7 04 39 00 00 00 00
                        if ((*(char *)(*ip + 2) & 0xff) == 0x39) {  // mov     [ecx+edi], <1>
                            i    = uc->uc_mcontext.gregs[REG_ECX];
                            i   += uc->uc_mcontext.gregs[REG_EDI];
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0xc7 0x04 0x39 <1:dword>
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "0x%.08x",
                                i, *(int *)&v,              // [ecx+edi]
                                *(int *)*ip);                       // <1>
                            *(int *)(p + i) = *(int *)*ip;          //      <1>: value
#endif
                            nv2a_write(i, *ip, 4);
                            *ip += 4;                               // <1> size: dword
                            return;
                        }
                    }
// mov     dword ptr [edi], 0   // C7 07 00 00 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x07) {      // mov     [edi], <1>
                        i    = uc->uc_mcontext.gregs[REG_EDI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0xc7 0x07 <1:dword>
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "0x%.08x",
                            i, *(int *)&v,                  // [edi]
                            *(int *)*ip);                           // <1>
                        *(int *)(p + i) = *(int *)*ip;              //      <1>: value
#endif
                        nv2a_write(i, *ip, 4);
                        *ip += 4;                                   // <1> size: dword
                        return;
                    }
// mov     dword ptr [eax+ecx*4+33Ch], 0FFFF0000h // C7 84 88 3C 03 00 00 00 00 FF FF
                    if ((*(char *)(*ip + 1) & 0xff) == 0x84) {
                        if ((*(char *)(*ip + 2) & 0xff) == 0x88) {  // mov     [eax+ecx*4+<1>], <2>
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_ECX] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0xc7 0x84 0x88 <1:dword> <2:dword>
                            i   += *(int *)*ip;                     //      <1>: offset
                            *ip += 4;                               // <1> size: dword
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "0x%.08x",
                                i, *(int *)&v,              // [eax+ecx*4+<1>]
                                *(int *)*ip);                       // <2>
                            *(int *)(p + i) = *(int *)*ip;          //      <2>: value
#endif
                            nv2a_write(i, *ip, 4);
                            *ip += 4;                               // <2> size: dword
                            return;
                        }
// mov     dword ptr [eax+edi*4+700004h], 0BF800000h // C7 84 B8 04 00 70 00 00 00 80 BF
                        if ((*(char *)(*ip + 2) & 0xff) == 0xb8) {  // mov     [eax+edi*4+<1>], <2>
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_EDI] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0xc7 0x84 0xb8 <1:dword> <2:dword>
                            i   += *(int *)*ip;                     //      <1>: offset
                            *ip += 4;                               // <1> size: dword
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "0x%.08x",
                                i, *(int *)&v,              // [eax+edi*4+<1>]
                                *(int *)*ip);                       // <2>
                            *(int *)(p + i) = *(int *)*ip;          //      <2>: value
#endif
                            nv2a_write(i, *ip, 4);
                            *ip += 4;                               // <2> size: dword
                            return;
                        }
                    }
// mov     dword ptr [esi], 7FF0000h // C7 06 00 00 FF 07
                    if ((*(char *)(*ip + 1) & 0xff) == 0x06) {      // mov     [esi], <1>
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0xc7 0x06 <1:dword>
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "0x%.08x",
                            i, *(int *)&v,                  // [esi]
                            *(int *)*ip);                           // <1>
                        *(int *)(p + i) = *(int *)*ip;              //      <1>: value
#endif
                        nv2a_write(i, *ip, 4);
                        *ip += 4;                                   // <1> size: dword
                        return;
                    }
// mov     dword ptr [edi+1830h], 0     // C7 87 30 18 00 00 00 00 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x87) {      // mov     [edi+<1>], <2>
                        i    = uc->uc_mcontext.gregs[REG_EDI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0xc7 0x87 <1:dword> <2:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "0x%.08x",
                            i, *(int *)&v,                  // [edi+<1>]
                            *(int *)*ip);                           // <2>
                        *(int *)(p + i) = *(int *)*ip;              //      <2>: value
#endif
                        nv2a_write(i, *ip, 4);
                        *ip += 4;                                   // <2> size: dword
                        return;
                    }
// mov     dword ptr [ecx], 0FFFFFFFFh // C7 01 FF FF FF FF
                    if ((*(char *)(*ip + 1) & 0xff) == 0x01) {      // mov     [ecx], <1>
                        i    = uc->uc_mcontext.gregs[REG_ECX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0xc7 0x01 <1:dword>
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "0x%.08x",
                            i, *(int *)&v,                  // [ecx]
                            *(int *)*ip);                           // <1>
                        *(int *)(p + i) = *(int *)*ip;              //      <1>: value
#endif
                        nv2a_write(i, *ip, 4);
                        *ip += 4;                                   // <1> size: dword
                        return;
                    }
// mov     dword ptr [eax+18h], 0 // C7 40 18 00 00 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x40) {      // mov     [eax+<1>], <2>
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0xc7 0x40 <1:byte> <2:dword>
                        i   += *(char *)*ip;                        //      <1>: offset
                        *ip += 1;                                   // <1> size: byte
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "0x%.08x",
                            i, *(int *)&v,                  // [eax+<1>]
                            *(int *)*ip);                           // <2>
                        *(int *)(p + i) = *(int *)*ip;              //      <2>: value
#endif
                        nv2a_write(i, *ip, 4);
                        *ip += 4;                                   // <2> size: dword
                        return;
                    }
// mov     dword ptr [eax], 0 // C7 00 00 00 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x00) {      // mov     [eax], <1>
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0xc7 0x00 <1:dword>
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "0x%.08x",
                            i, *(int *)&v,                  // [eax]
                            *(int *)*ip);                           // <1>
                        *(int *)(p + i) = *(int *)*ip;              //      <1>: value
#endif
                        nv2a_write(i, *ip, 4);
                        *ip += 4;                                   // <1> size: dword
                        return;
                    }
// mov     dword ptr [esi+400750h], 0EA000Ch // C7 86 50 07 40 00 0C 00 EA 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x86) {      // mov     [esi+<1>], <2>
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0xc7 0x86 <1:dword> <2:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "0x%.08x",
                            i, *(int *)&v,                  // [esi+<1>]
                            *(int *)*ip);                           // <2>
                        *(int *)(p + i) = *(int *)*ip;              //      <2>: value
#endif
                        nv2a_write(i, *ip, 4);
                        *ip += 4;                                   // <2> size: dword
                        return;
                    }
                }
                if ((*(char *)*ip & 0xff) == 0x8b) {                // mov
                    if ((*(char *)(*ip + 1) & 0xff) == 0x82) {      // mov     eax, [edx+<1>]
                        i    = uc->uc_mcontext.gregs[REG_EDX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x82 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
#if 0
                        PRINT("GPU: mov: "
                            "eax (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // eax
                            i, *(int *)&v);                 // [edx+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
                    if ((*(char *)(*ip + 1) & 0xff) == 0x92) {      // mov     edx, [edx+<1>]
                        i    = uc->uc_mcontext.gregs[REG_EDX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x92 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                        PRINT("GPU: mov: "
                            "edx (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // edx
                            i, *(int *)&v);                 // [edx+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     esi, [eax+2500h]     // 8B B0 00 25 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0xb0) {      // mov     esi, [eax+<1>]
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0xb0 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ESI];
#if 0
                        PRINT("GPU: mov: "
                            "esi (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // esi
                            i, *(int *)&v);                 // [eax+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     ecx, [esi]           // 8B 0E
                    if ((*(char *)(*ip + 1) & 0xff) == 0x0e) {      // mov     ecx, [esi]
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x0e
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                        PRINT("GPU: mov: "
                            "ecx (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // ecx
                            i, *(int *)&v);                 // [esi]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     ecx, [edx]           // 8B 0A
                    if ((*(char *)(*ip + 1) & 0xff) == 0x0a) {      // mov     ecx, [edx]
                        i    = uc->uc_mcontext.gregs[REG_EDX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x0a
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                        PRINT("GPU: mov: "
                            "ecx (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // ecx
                            i, *(int *)&v);                 // [edx]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     eax, [ecx] // 8B 01
                    if ((*(char *)(*ip + 1) & 0xff) == 0x01) {      // mov     eax, [ecx]
                        i    = uc->uc_mcontext.gregs[REG_ECX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x01
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
#if 0
                        PRINT("GPU: mov: "
                            "eax (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // eax
                            i, *(int *)&v);                 // [ecx]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     eax, [edi+140h] // 8B 87 40 01 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x87) {      // mov     eax, [edi+<1>]
                        i    = uc->uc_mcontext.gregs[REG_EDI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x87 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
#if 0
                        PRINT("GPU: mov: "
                            "eax (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // eax
                            i, *(int *)&v);                 // [edi+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     ecx, [eax] // 8B 08
                    if ((*(char *)(*ip + 1) & 0xff) == 0x08) {      // mov     ecx, [eax]
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x08
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                        PRINT("GPU: mov: "
                            "ecx (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // ecx
                            i, *(int *)&v);                 // [eax]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     eax, [eax] // 8B 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x00) {      // mov     eax, [eax]
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x00
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
#if 0
                        PRINT("GPU: mov: "
                            "eax (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // eax
                            i, *(int *)&v);                 // [eax]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     ecx, [eax+8088h] // 8B 88 88 80 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x88) {      // mov     ecx, [eax+<1>]
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x88 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                        PRINT("GPU: mov: "
                            "ecx (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // ecx
                            i, *(int *)&v);                 // [eax+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     edx, [ecx] // 8B 11
                    if ((*(char *)(*ip + 1) & 0xff) == 0x11) {      // mov     edx, [ecx]
                        i    = uc->uc_mcontext.gregs[REG_ECX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x11
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                        PRINT("GPU: mov: "
                            "edx (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // edx
                            i, *(int *)&v);                 // [ecx]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     eax, [esi+400700h] // 8B 86 00 07 40 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x86) {      // mov     eax, [esi+<1>]
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x86 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
#if 0
                        PRINT("GPU: mov: "
                            "eax (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // eax
                            i, *(int *)&v);                 // [esi+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     ebp, [eax-3006C0h] // 8B A8 40 F9 CF FF
                    if ((*(char *)(*ip + 1) & 0xff) == 0xa8) {      // mov     ebp, [eax+<1>]
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0xa8 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EBP];
#if 0
                        PRINT("GPU: mov: "
                            "ebp (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // ebp
                            i, *(int *)&v);                 // [eax+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     edx, [eax-300680h] // 8B 90 80 F9 CF FF
                    if ((*(char *)(*ip + 1) & 0xff) == 0x90) {      // mov     edx, [eax+<1>]
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x90 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                        PRINT("GPU: mov: "
                            "edx (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // edx
                            i, *(int *)&v);                 // [eax+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     edi, [eax+3240h] // 8B B8 40 32 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0xb8) {      // mov     edi, [eax+<1>]
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0xb8 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDI];
#if 0
                        PRINT("GPU: mov: "
                            "edi (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // edi
                            i, *(int *)&v);                 // [eax+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     edi, [edx] // 8B 3A
                    if ((*(char *)(*ip + 1) & 0xff) == 0x3a) {      // mov     edi, [edx]
                        i    = uc->uc_mcontext.gregs[REG_EDX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x3a
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDI];
#if 0
                        PRINT("GPU: mov: "
                            "edi (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // edi
                            i, *(int *)&v);                 // [edx]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     edi, [edx+4] // 8B 7A 04
                    if ((*(char *)(*ip + 1) & 0xff) == 0x7a) {      // mov     edi, [edx+<1>]
                        i    = uc->uc_mcontext.gregs[REG_EDX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x7a <1:byte>
                        i   += *(char *)*ip;                        //      <1>: offset
                        *ip += 1;                                   // <1> size: byte
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDI];
#if 0
                        PRINT("GPU: mov: "
                            "edi (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // edi
                            i, *(int *)&v);                 // [edx+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     edx, [edx+30h] // 8B 52 30
                    if ((*(char *)(*ip + 1) & 0xff) == 0x52) {      // mov     edx, [edx+<1>]
                        i    = uc->uc_mcontext.gregs[REG_EDX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x52 <1:byte>
                        i   += *(char *)*ip;                        //      <1>: offset
                        *ip += 1;                                   // <1> size: byte
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                        PRINT("GPU: mov: "
                            "edx (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // edx
                            i, *(int *)&v);                 // [edx+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     ecx, [ecx+44h] // 8B 49 44
                    if ((*(char *)(*ip + 1) & 0xff) == 0x49) {      // mov     ecx, [ecx+<1>]
                        i    = uc->uc_mcontext.gregs[REG_ECX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x49 <1:byte>
                        i   += *(char *)*ip;                        //      <1>: offset
                        *ip += 1;                                   // <1> size: byte
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                        PRINT("GPU: mov: "
                            "ecx (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // ecx
                            i, *(int *)&v);                 // [ecx+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
//if (i == 0x00800044) *(int *)bp = *(int *)uc->uc_mcontext.gregs[REG_EAX]; //TODO
//if (i == 0x00800044) PRINT("0x%.08x", *(int *)uc->uc_mcontext.gregs[REG_EAX]); //TODO
                        nv2a_read(i, bp, 4);
//if (i == 0xfd800044) { PRINT("0x%.08x", *(int *)uc->uc_mcontext.gregs[REG_EAX]); INT3; } //TODO
                        return;
                    }
// mov     edx, [esi+2100h] // 8B 96 00 21 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x96) {      // mov     edx, [esi+<1>]
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x96 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                        PRINT("GPU: mov: "
                            "edx (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // edx
                            i, *(int *)&v);                 // [esi+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     ecx, [esi+2100h] // 8B 8E 00 21 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x8e) {      // mov     ecx, [esi+<1>]
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x8e <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                        PRINT("GPU: mov: "
                            "ecx (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // ecx
                            i, *(int *)&v);                 // [esi+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     edi, [esi+400704h] // 8B BE 04 07 40 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0xbe) {      // mov     edi, [esi+<1>]
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0xbe <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDI];
#if 0
                        PRINT("GPU: mov: "
                            "edi (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // edi
                            i, *(int *)&v);                 // [esi+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     ebx, [esi+400108h] // 8B 9E 08 01 40 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x9e) {      // mov     ebx, [esi+<1>]
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x9e <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EBX];
#if 0
                        PRINT("GPU: mov: "
                            "ebx (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // ebx
                            i, *(int *)&v);                 // [esi+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     ebp, [edx+400700h] // 8B AA 00 07 40 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0xaa) {      // mov     ebp, [edx+<1>]
                        i    = uc->uc_mcontext.gregs[REG_EDX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0xaa <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EBP];
#if 0
                        PRINT("GPU: mov: "
                            "ebp (0x%.08x), "
                            "[0x%.08x] (0x%.08x)",
                            *(int *)bp,                             // ebp
                            i, *(int *)&v);                 // [edx+<1>]
                        *(int *)bp = *(int *)(p + i);
#endif
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     eax, [ecx+100248h] // 8B 81 48 02 10 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x81) {      // mov     eax, [ecx+<1>]
                        i    = uc->uc_mcontext.gregs[REG_ECX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x81 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     eax, [eax+400904h] // 8B 80 04 09 40 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x80) {      // mov     eax, [eax+<1>]
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x80 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     eax, [ebx] // 8B 03
                    if ((*(char *)(*ip + 1) & 0xff) == 0x03) {      // mov     eax, [ebx]
                        i    = uc->uc_mcontext.gregs[REG_EBX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x03
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        nv2a_read(i, bp, 4);
                        return;
                    }
// mov     eax, [eax+44h] // 8B 40 44
                    if ((*(char *)(*ip + 1) & 0xff) == 0x40) {      // mov     eax, [eax+<1>]
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8b 0x40 <1:byte>
                        i   += *(char *)*ip;                        //      <1>: offset
                        *ip += 1;                                   // <1> size: byte
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        nv2a_read(i, bp, 4);
                        return;
                    }
                }
// mov     [ecx+700008h], eax   // 89 81 08 00 70 00
                if ((*(char *)*ip & 0xff) == 0x89) {                // mov
                    if ((*(char *)(*ip + 1) & 0xff) == 0x81) {      // mov     [ecx+<1>], eax
                        i    = uc->uc_mcontext.gregs[REG_ECX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x81 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "eax (0x%.08x)",
                            i, *(int *)&v,                  // [ecx+<1>]
                            *(int *)bp);                            // eax
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [eax+edi], esi       // 89 34 38
                    if ((*(char *)(*ip + 1) & 0xff) == 0x34) {
                        if ((*(char *)(*ip + 2) & 0xff) == 0x38) {  // mov     [eax+edi], esi
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_EDI];
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x89 0x34 0x38
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_ESI];
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "esi (0x%.08x)",
                                i, *(int *)&v,              // [eax+edi]
                                *(int *)bp);                        // esi
                            *(int *)(p + i) = *(int *)bp;
#endif
                            nv2a_write(i, bp, 4);
                            return;
                        }
// mov     [eax+edi*4], esi     // 89 34 B8
                        if ((*(char *)(*ip + 2) & 0xff) == 0xb8) {  // mov     [eax+edi*4], esi
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_EDI] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x89 0x34 0xb8
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_ESI];
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "esi (0x%.08x)",
                                i, *(int *)&v,              // [eax+edi*4]
                                *(int *)bp);                        // esi
                            *(int *)(p + i) = *(int *)bp;
#endif
                            nv2a_write(i, bp, 4);
                            return;
                        }
                    }
// mov     [esi+0Ch], edi       // 89 7E 0C
                    if ((*(char *)(*ip + 1) & 0xff) == 0x7e) {      // mov     [esi+<1>], edi
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x7e <1:byte>
                        i   += *(char *)*ip;                        //      <1>: offset
                        *ip += 1;                                   // <1> size: byte
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDI];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "edi (0x%.08x)",
                            i, *(int *)&v,                  // [esi+<1>]
                            *(int *)bp);                            // edi
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [esi+14h], ebx       // 89 5E 14
                    if ((*(char *)(*ip + 1) & 0xff) == 0x5e) {      // mov     [esi+<1>], ebx
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x5e <1:byte>
                        i   += *(char *)*ip;                        //      <1>: offset
                        *ip += 1;                                   // <1> size: byte
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EBX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "ebx (0x%.08x)",
                            i, *(int *)&v,                  // [esi+<1>]
                            *(int *)bp);                            // ebx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [eax+2504h], esi     // 89 B0 04 25 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0xb0) {      // mov     [eax+<1>], esi
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0xb0 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ESI];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "esi (0x%.08x)",
                            i, *(int *)&v,                  // [eax+<1>]
                            *(int *)bp);                            // esi
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [esi], ecx           // 89 0E
                    if ((*(char *)(*ip + 1) & 0xff) == 0x0e) {      // mov     [esi], ecx
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x0e
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "ecx (0x%.08x)",
                            i, *(int *)&v,                  // [esi]
                            *(int *)bp);                            // ecx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [eax+3240h], ecx     // 89 88 40 32 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x88) {      // mov     [eax+<1>], ecx
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x88 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "ecx (0x%.08x)",
                            i, *(int *)&v,                  // [eax+<1>]
                            *(int *)bp);                            // ecx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [eax+3224h], ebx     // 89 98 24 32 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x98) {      // mov     [eax+<1>], ebx
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x98 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EBX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "ebx (0x%.08x)",
                            i, *(int *)&v,                  // [eax+<1>]
                            *(int *)bp);                            // ebx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [eax], ecx           // 89 08
                    if ((*(char *)(*ip + 1) & 0xff) == 0x08) {      // mov     [eax], ecx
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x08
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "ecx (0x%.08x)",
                            i, *(int *)&v,                  // [eax]
                            *(int *)bp);                            // ecx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [eax+4], ecx         // 89 48 04
                    if ((*(char *)(*ip + 1) & 0xff) == 0x48) {      // mov     [eax+<1>], ecx
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x48 <1:byte>
                        i   += *(char *)*ip;                        //      <1>: offset
                        *ip += 1;                                   // <1> size: byte
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "ecx (0x%.08x)",
                            i, *(int *)&v,                  // [eax+<1>]
                            *(int *)bp);                            // ecx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [edx], ecx           // 89 0A
                    if ((*(char *)(*ip + 1) & 0xff) == 0x0a) {      // mov     [edx], ecx
                        i    = uc->uc_mcontext.gregs[REG_EDX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x0a
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "ecx (0x%.08x)",
                            i, *(int *)&v,                  // [edx]
                            *(int *)bp);                            // ecx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [eax+ecx*4+340h], edx    // 89 94 88 40 03 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x94) {
                        if ((*(char *)(*ip + 2) & 0xff) == 0x88) {  // mov     [eax+ecx*4+<1>], edx
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_ECX] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x89 0x94 0x88 <1:dword>
                            i   += *(int *)*ip;                     //      <1>: offset
                            *ip += 4;                               // <1> size: dword
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "edx (0x%.08x)",
                                i, *(int *)&v,              // [eax+ecx*4+<1>]
                                *(int *)bp);                        // edx
                            *(int *)(p + i) = *(int *)bp;
#endif
                            nv2a_write(i, bp, 4);
                            return;
                        }
// mov     [eax+edi*4+700004h], edx // 89 94 B8 04 00 70 00
                        if ((*(char *)(*ip + 2) & 0xff) == 0xb8) {  // mov     [eax+edi*4+<1>], edx
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_EDI] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x89 0x94 0xb8 <1:dword>
                            i   += *(int *)*ip;                     //      <1>: offset
                            *ip += 4;                               // <1> size: dword
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "edx (0x%.08x)",
                                i, *(int *)&v,              // [eax+edi*4+<1>]
                                *(int *)bp);                        // edx
                            *(int *)(p + i) = *(int *)bp;
#endif
                            nv2a_write(i, bp, 4);
                            return;
                        }
                    }
// mov     [eax+ecx*4+3A0h], esi    // 89 B4 88 A0 03 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0xb4) {
                        if ((*(char *)(*ip + 2) & 0xff) == 0x88) {  // mov     [eax+ecx*4+<1>], esi
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_ECX] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x89 0xb4 0x88 <1:dword>
                            i   += *(int *)*ip;                     //      <1>: offset
                            *ip += 4;                               // <1> size: dword
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_ESI];
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "esi (0x%.08x)",
                                i, *(int *)&v,              // [eax+ecx*4+<1>]
                                *(int *)bp);                        // esi
                            *(int *)(p + i) = *(int *)bp;
#endif
                            nv2a_write(i, bp, 4);
                            return;
                        }
                    }
// mov     [eax+ecx*4+5FCh], ebx // 89 9C 88 FC 05 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x9c) {
                        if ((*(char *)(*ip + 2) & 0xff) == 0x88) {  // mov     [eax+ecx*4+<1>], ebx
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_ECX] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x89 0x9c 0x88 <1:dword>
                            i   += *(int *)*ip;                     //      <1>: offset
                            *ip += 4;                               // <1> size: dword
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_EBX];
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "ebx (0x%.08x)",
                                i, *(int *)&v,              // [eax+ecx*4+<1>]
                                *(int *)bp);                        // ebx
                            *(int *)(p + i) = *(int *)bp;
#endif
                            nv2a_write(i, bp, 4);
                            return;
                        }
                    }
// mov     [esi], edx // 89 16
                    if ((*(char *)(*ip + 1) & 0xff) == 0x16) {      // mov     [esi], edx
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x16
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "edx (0x%.08x)",
                            i, *(int *)&v,                  // [esi]
                            *(int *)bp);                            // edx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [ebx], edx // 89 13
                    if ((*(char *)(*ip + 1) & 0xff) == 0x13) {      // mov     [ebx], edx
                        i    = uc->uc_mcontext.gregs[REG_EBX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x13
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "edx (0x%.08x)",
                            i, *(int *)&v,                  // [ebx]
                            *(int *)bp);                            // edx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [edi], edx // 89 17
                    if ((*(char *)(*ip + 1) & 0xff) == 0x17) {      // mov     [edi], edx
                        i    = uc->uc_mcontext.gregs[REG_EDI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x17
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "edx (0x%.08x)",
                            i, *(int *)&v,                  // [edi]
                            *(int *)bp);                            // edx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [eax+esi*4], edx // 89 14 B0
                    if ((*(char *)(*ip + 1) & 0xff) == 0x14) {
                        if ((*(char *)(*ip + 2) & 0xff) == 0xb0) {  // mov     [eax+esi*4], edx
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_ESI] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x89 0x14 0xb0
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "edx (0x%.08x)",
                                i, *(int *)&v,              // [eax+esi*4]
                                *(int *)bp);                        // edx
                            *(int *)(p + i) = *(int *)bp;
#endif
                            nv2a_write(i, bp, 4);
                            return;
                        }
// mov     [eax+ebx*4], edx // 89 14 98
                        if ((*(char *)(*ip + 2) & 0xff) == 0x98) {  // mov     [eax+ebx*4], edx
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_EBX] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x89 0x14 0x98
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "edx (0x%.08x)",
                                i, *(int *)&v,              // [eax+ebx*4]
                                *(int *)bp);                        // edx
                            *(int *)(p + i) = *(int *)bp;
#endif
                            nv2a_write(i, bp, 4);
                            return;
                        }
// mov     [eax+ecx*4], edx // 89 14 88
                        if ((*(char *)(*ip + 2) & 0xff) == 0x88) {  // mov     [eax+ecx*4], edx
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_ECX] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x89 0x14 0x88
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "edx (0x%.08x)",
                                i, *(int *)&v,              // [eax+ecx*4]
                                *(int *)bp);                        // edx
                            *(int *)(p + i) = *(int *)bp;
#endif
                            nv2a_write(i, bp, 4);
                            return;
                        }
                    }
// mov     [eax+edx*4], ecx // 89 0C 90
                    if ((*(char *)(*ip + 1) & 0xff) == 0x0c) {
                        if ((*(char *)(*ip + 2) & 0xff) == 0x90) {  // mov     [eax+edx*4], ecx
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_EDX] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x89 0x0c 0x90
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "ecx (0x%.08x)",
                                i, *(int *)&v,              // [eax+edx*4]
                                *(int *)bp);                        // ecx
                            *(int *)(p + i) = *(int *)bp;
#endif
                            nv2a_write(i, bp, 4);
                            return;
                        }
// mov     [eax+esi*4], ecx // 89 0C B0
                        if ((*(char *)(*ip + 2) & 0xff) == 0xb0) {  // mov     [eax+esi*4], ecx
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_ESI] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x89 0x0c 0xb0
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "ecx (0x%.08x)",
                                i, *(int *)&v,              // [eax+esi*4]
                                *(int *)bp);                        // ecx
                            *(int *)(p + i) = *(int *)bp;
#endif
                            nv2a_write(i, bp, 4);
                            return;
                        }
// mov     [edi+edx], ecx // 89 0C 17
                        if ((*(char *)(*ip + 2) & 0xff) == 0x17) {  // mov     [edi+edx], ecx
                            i    = uc->uc_mcontext.gregs[REG_EDI];
                            i   += uc->uc_mcontext.gregs[REG_EDX];
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x89 0x0c 0x17
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "ecx (0x%.08x)",
                                i, *(int *)&v,              // [edi+edx]
                                *(int *)bp);                        // ecx
                            *(int *)(p + i) = *(int *)bp;
#endif
                            nv2a_write(i, bp, 4);
                            return;
                        }
                    }
// mov     [eax+edx*4+700014h], ecx // 89 8C 90 14 00 70 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x8c) {
                        if ((*(char *)(*ip + 2) & 0xff) == 0x90) {  // mov     [eax+edx*4+<1>], ecx
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            i   += uc->uc_mcontext.gregs[REG_EDX] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x89 0x8c 0x90 <1:dword>
                            i   += *(int *)*ip;                     //      <1>: offset
                            *ip += 4;                               // <1> size: dword
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                            PRINT("GPU: mov: "
                                "[0x%.08x] (0x%.08x), "
                                "ecx (0x%.08x)",
                                i, *(int *)&v,              // [eax+edx*4+<1>]
                                *(int *)bp);                        // ecx
                            *(int *)(p + i) = *(int *)bp;
#endif
                            nv2a_write(i, bp, 4);
                            return;
                        }
                    }
// mov     [ecx], eax // 89 01
                    if ((*(char *)(*ip + 1) & 0xff) == 0x01) {      // mov     [ecx], eax
                        i    = uc->uc_mcontext.gregs[REG_ECX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x01
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "eax (0x%.08x)",
                            i, *(int *)&v,                  // [ecx]
                            *(int *)bp);                            // eax
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [edi+2210h], ecx // 89 8F 10 22 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x8f) {      // mov     [edi+<1>], ecx
                        i    = uc->uc_mcontext.gregs[REG_EDI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x8f <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "ecx (0x%.08x)",
                            i, *(int *)&v,                  // [edi+<1>]
                            *(int *)bp);                            // ecx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [edi+2214h], edx // 89 97 14 22 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x97) {      // mov     [edi+<1>], edx
                        i    = uc->uc_mcontext.gregs[REG_EDI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x97 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "edx (0x%.08x)",
                            i, *(int *)&v,                  // [edi+<1>]
                            *(int *)bp);                            // edx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [ecx], edx // 89 11
                    if ((*(char *)(*ip + 1) & 0xff) == 0x11) {      // mov     [ecx], edx
                        i    = uc->uc_mcontext.gregs[REG_ECX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x11
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "edx (0x%.08x)",
                            i, *(int *)&v,                  // [ecx]
                            *(int *)bp);                            // edx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [eax-8], edx // 89 50 F8
                    if ((*(char *)(*ip + 1) & 0xff) == 0x50) {      // mov     [eax+<1>], edx
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x50 <1:byte>
                        i   += *(char *)*ip;                        //      <1>: offset
                        *ip += 1;                                   // <1> size: byte
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "edx (0x%.08x)",
                            i, *(int *)&v,                  // [eax+<1>]
                            *(int *)bp);                            // edx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [eax], edx // 89 10
                    if ((*(char *)(*ip + 1) & 0xff) == 0x10) {      // mov     [eax], edx
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x10
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "edx (0x%.08x)",
                            i, *(int *)&v,                  // [eax]
                            *(int *)bp);                            // edx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [edx+400780h], eax // 89 82 80 07 40 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x82) {      // mov     [edx+<1>], eax
                        i    = uc->uc_mcontext.gregs[REG_EDX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x82 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "eax (0x%.08x)",
                            i, *(int *)&v,                  // [edx+<1>]
                            *(int *)bp);                            // eax
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [eax+3210h], edx // 89 90 10 32 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x90) {      // mov     [eax+<1>], edx
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x90 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "edx (0x%.08x)",
                            i, *(int *)&v,                  // [eax+<1>]
                            *(int *)bp);                            // edx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [esi+200h], ebx // 89 9E 00 02 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x9e) {      // mov     [esi+<1>], ebx
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x9e <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EBX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "ebx (0x%.08x)",
                            i, *(int *)&v,                  // [esi+<1>]
                            *(int *)bp);                            // ebx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [esi+140h], eax // 89 86 40 01 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x86) {      // mov     [esi+<1>], eax
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x86 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "eax (0x%.08x)",
                            i, *(int *)&v,                  // [esi+<1>]
                            *(int *)bp);                            // eax
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [esi+9400h], ebp // 89 AE 00 94 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0xae) {      // mov     [esi+<1>], ebp
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0xae <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EBP];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "ebp (0x%.08x)",
                            i, *(int *)&v,                  // [esi+<1>]
                            *(int *)bp);                            // ebp
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [eax], ebp // 89 28
                    if ((*(char *)(*ip + 1) & 0xff) == 0x28) {      // mov     [eax], ebp
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x28
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EBP];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "ebp (0x%.08x)",
                            i, *(int *)&v,                  // [eax]
                            *(int *)bp);                            // ebp
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [eax+4], ebp // 89 68 04
                    if ((*(char *)(*ip + 1) & 0xff) == 0x68) {      // mov     [eax+<1>], ebp
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x68 <1:byte>
                        i   += *(char *)*ip;                        //      <1>: offset
                        *ip += 1;                                   // <1> size: byte
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EBP];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "ebp (0x%.08x)",
                            i, *(int *)&v,                  // [eax+<1>]
                            *(int *)bp);                            // ebp
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [esi+400754h], edx // 89 96 54 07 40 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x96) {      // mov     [esi+<1>], edx
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x96 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "edx (0x%.08x)",
                            i, *(int *)&v,                  // [esi+<1>]
                            *(int *)bp);                            // edx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [esi+400750h], ecx // 89 8E 50 07 40 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x8e) {      // mov     [esi+<1>], ecx
                        i    = uc->uc_mcontext.gregs[REG_ESI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x8e <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "ecx (0x%.08x)",
                            i, *(int *)&v,                  // [esi+<1>]
                            *(int *)bp);                            // ecx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [edi], ebp // 89 2F
                    if ((*(char *)(*ip + 1) & 0xff) == 0x2f) {      // mov     [edi], ebp
                        i    = uc->uc_mcontext.gregs[REG_EDI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x2f
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EBP];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "ebp (0x%.08x)",
                            i, *(int *)&v,                  // [edi]
                            *(int *)bp);                            // ebp
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [edx], edi // 89 3A
                    if ((*(char *)(*ip + 1) & 0xff) == 0x3a) {      // mov     [edx], edi
                        i    = uc->uc_mcontext.gregs[REG_EDX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x3a
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDI];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "edi (0x%.08x)",
                            i, *(int *)&v,                  // [edx]
                            *(int *)bp);                            // edi
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [edx+4], edi // 89 7A 04
                    if ((*(char *)(*ip + 1) & 0xff) == 0x7a) {      // mov     [edx+<1>], edi
                        i    = uc->uc_mcontext.gregs[REG_EDX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x7a <1:byte>
                        i   += *(char *)*ip;                        //      <1>: offset
                        *ip += 1;                                   // <1> size: byte
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDI];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "edi (0x%.08x)",
                            i, *(int *)&v,                  // [edx+<1>]
                            *(int *)bp);                            // edi
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [eax+3240h], edi // 89 B8 40 32 00 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0xb8) {      // mov     [eax+<1>], edi
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0xb8 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EDI];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "edi (0x%.08x)",
                            i, *(int *)&v,                  // [eax+<1>]
                            *(int *)bp);                            // edi
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [edi], ebx // 89 1F
                    if ((*(char *)(*ip + 1) & 0xff) == 0x1f) {      // mov     [edi], ebx
                        i    = uc->uc_mcontext.gregs[REG_EDI];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x1f
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EBX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.08x), "
                            "ebx (0x%.08x)",
                            i, *(int *)&v,                  // [edi]
                            *(int *)bp);                            // ebx
                        *(int *)(p + i) = *(int *)bp;
#endif
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [edx], eax // 89 02
                    if ((*(char *)(*ip + 1) & 0xff) == 0x02) {      // mov     [edx], eax
                        i    = uc->uc_mcontext.gregs[REG_EDX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x02
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [ebx], eax // 89 03
                    if ((*(char *)(*ip + 1) & 0xff) == 0x03) {      // mov     [ebx], eax
                        i    = uc->uc_mcontext.gregs[REG_EBX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x89 0x03
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                        nv2a_write(i, bp, 4);
                        return;
                    }
// mov     [esi+edi*4+400980h], eax // 89 84 BE 80 09 40 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x84) {
                        if ((*(char *)(*ip + 2) & 0xff) == 0xbe) {  // mov     [esi+edi*4+<1>], eax
                            i    = uc->uc_mcontext.gregs[REG_ESI];
                            i   += uc->uc_mcontext.gregs[REG_EDI] * 4;
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x89 0x84 0xbe <1:dword>
                            i   += *(int *)*ip;                     //      <1>: offset
                            *ip += 4;                               // <1> size: dword
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_EAX];
                            nv2a_write(i, bp, 4);
                            return;
                        }
                    }
                }
// movzx   ecx, byte ptr [eax+1] // 0F B6 48 01
                if ((*(char *)*ip & 0xff) == 0x0f) {
                    if ((*(char *)(*ip + 1) & 0xff) == 0xb6) {
                        if ((*(char *)(*ip + 2) & 0xff) == 0x48) {  // movzx   ecx, [eax+<1>]
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x0f 0xb6 0x48 <1:byte>
                            i   += *(char *)*ip;                    //      <1>: offset
                            *ip += 1;                               // <1> size: byte
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                            PRINT("GPU: mov: "
                                "ecx (0x%.08x), "
                                "[0x%.08x] (0x%.02hhx)",
                                *(int *)bp,                         // ecx
                                i, *(char *)&v);            // [eax+<1>]
                            *(int *)bp = *(char *)(p + i);
#endif
                            *(int *)bp = 0;
                            nv2a_read(i, bp, 1);
                            return;
                        }
// movzx   edi, byte ptr [eax+1] // 0F B6 78 01
                        if ((*(char *)(*ip + 2) & 0xff) == 0x78) {  // movzx   edi, [eax+<1>]
                            i    = uc->uc_mcontext.gregs[REG_EAX];
                            if ((i & 0xff000000) != 0xfd000000) break;
                            *ip += 3;                               //   opcode: 0x0f 0xb6 0x78 <1:byte>
                            i   += *(char *)*ip;                    //      <1>: offset
                            *ip += 1;                               // <1> size: byte
                            bp   = (void **)&uc->uc_mcontext.gregs[REG_EDI];
#if 0
                            PRINT("GPU: mov: "
                                "edi (0x%.08x), "
                                "[0x%.08x] (0x%.02hhx)",
                                *(int *)bp,                         // edi
                                i, *(char *)&v);            // [eax+<1>]
                            *(int *)bp = *(char *)(p + i);
#endif
                            *(int *)bp = 0;
                            nv2a_read(i, bp, 1);
                            return;
                        }
                    }
                }
// mov     byte ptr [eax+6013D4h], 1Fh // C6 80 D4 13 60 00 1F
                if ((*(char *)*ip & 0xff) == 0xc6) {
                    if ((*(char *)(*ip + 1) & 0xff) == 0x80) {      // mov     [eax+<1>], <2>
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0xc6 0x80 <1:dword> <2:byte>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.02hhx), "
                            "0x%.02hhx",
                            i, *(char *)&v,                 // [eax+<1>]
                            *(char *)*ip);                          // <2>
                        *(char *)(p + i) = *(char *)*ip;            //      <2>: value
#endif
                        nv2a_write(i, *ip, 1);
                        *ip += 1;                                   // <2> size: byte
                        return;
                    }
                }
// mov     bl, [eax+6013D5h] // 8A 98 D5 13 60 00
                if ((*(char *)*ip & 0xff) == 0x8a) {
                    if ((*(char *)(*ip + 1) & 0xff) == 0x98) {      // mov     bl, [eax+<1>]
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8a 0x98 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_EBX];
#if 0
                        PRINT("GPU: mov: "
                            "bl (0x%.02hhx), "
                            "[0x%.08x] (0x%.02hhx)",
                            *(char *)bp,                            // bl
                            i, *(char *)&v);                // [eax+<1>]
                        *(char *)bp = *(char *)(p + i);
#endif
                        nv2a_read(i, bp, 1);
                        return;
                    }
// mov     cl, [eax+6013D5h] // 8A 88 D5 13 60 00
                    if ((*(char *)(*ip + 1) & 0xff) == 0x88) {      // mov     cl, [eax+<1>]
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x8a 0x88 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                        PRINT("GPU: mov: "
                            "cl (0x%.02hhx), "
                            "[0x%.08x] (0x%.02hhx)",
                            *(char *)bp,                            // cl
                            i, *(char *)&v);                // [eax+<1>]
                        *(char *)bp = *(char *)(p + i);
#endif
                        nv2a_read(i, bp, 1);
                        return;
                    }
                }
// mov     [eax+6013D5h], cl // 88 88 D5 13 60 00
                if ((*(char *)*ip & 0xff) == 0x88) {
                    if ((*(char *)(*ip + 1) & 0xff) == 0x88) {      // mov     [eax+<1>], cl
                        i    = uc->uc_mcontext.gregs[REG_EAX];
                        if ((i & 0xff000000) != 0xfd000000) break;
                        *ip += 2;                                   //   opcode: 0x88 0x88 <1:dword>
                        i   += *(int *)*ip;                         //      <1>: offset
                        *ip += 4;                                   // <1> size: dword
                        bp   = (void **)&uc->uc_mcontext.gregs[REG_ECX];
#if 0
                        PRINT("GPU: mov: "
                            "[0x%.08x] (0x%.02hhx), "
                            "cl (0x%.02hhx)",
                            i, *(char *)&v,                 // [eax+<1>]
                            *(char *)bp);                           // cl
                        *(char *)(p + i) = *(char *)bp;
#endif
                        nv2a_write(i, bp, 1);
                        return;
                    }
                }
            }
        } while (0);
    } else if (i == 128) {

#define EIP_PATCH(x,y,z) do { \
        if (!memcmp((x), (y), sizeof((y)) - 1)) { \
            memcpy((x), (z), sizeof((z)) - 1); \
            return; \
        } \
    } while (0)

        EIP_PATCH(*ip,                              // SetLastError() / GetLastError()
            "\x64\x0f\xb6\x05\x24\x00\x00\x00",     // 1: movzx   eax, large byte ptr fs:24h
                                                    // --
            "\x31\xc0\x90\x90\x90\x90\x90\x90");    // 1: xor     eax, eax

        EIP_PATCH(*ip,                              // SetLastError()
            "\x64\xa1\x28\x00\x00\x00"              // 1: mov     eax, large fs:28h
            "\x64\x8b\x0d\x04\x00\x00\x00"          // 2: mov     ecx, large fs:4
            "\xa1"/*\xa0\xea\x24\x00"*/             // 3: mov     eax, TlsIndex
          /*"\x8b\x04\x81"*/                        // 4: mov     eax, [ecx+eax*4]
          /*"\x8b\x4c\x24\x04"*/                    // 5: mov     ecx, [esp+dwErrCode]
          /*"\x89\x88\x04\x00\x00\x00"*/,           // 6: mov     [eax+4], ecx
                                                    // --
            "\x31\xc0\x90\x90\x90\x90"              // 1: xor     eax, eax
            "\x31\xc9\x90\x90\x90\x90\x90"          // 2: xor     ecx, ecx
            "\x90\x90\x90\x90\x90"                  // 3: nop
            "\x90\x90\x90"                          // 4: nop
            "\x90\x90\x90\x90"                      // 5: nop
            "\x90\x90\x90\x90\x90\x90");            // 6: nop

        EIP_PATCH(*ip,                              // GetLastError()
            "\x64\xa1\x28\x00\x00\x00"              // 1: mov     eax, large fs:28h
            "\xa1"/*\xa0\xea\x24\x00"*/             // 2: mov     eax, TlsIndex
          /*"\x64\x8b\x0d\x04\x00\x00\x00"*/        // 3: mov     ecx, large fs:4
          /*"\x8b\x04\x81"*/                        // 4: mov     eax, [ecx+eax*4]
          /*"\x8b\x80\x04\x00\x00\x00"*/,           // 5: mov     eax, [eax+4]
                                                    // --
            "\x31\xc0\x90\x90\x90\x90"              // 1: xor     eax, eax
            "\x90\x90\x90\x90\x90"                  // 2: nop
            "\x31\xc9\x90\x90\x90\x90\x90"          // 3: xor     ecx, ecx
            "\x90\x90\x90"                          // 4: nop
            "\x90\x90\x90\x90\x90\x90");            // 5: nop

        EIP_PATCH(*ip,
            "\x64\xa1\x20\x00\x00\x00"              // 1: mov     eax, large fs:20h
            "\x8b\x80\x50\x02\x00\x00",             // 2: mov     eax, [eax+250h]
                                                    // --
            "\x31\xc0\x90\x90\x90\x90"              // 1: xor     eax, eax
            "\x90\x90\x90\x90\x90\x90");            // 2: nop

        EIP_PATCH(*ip,                              // __mtinit()
            "\x64\x8b\x0d\x04\x00\x00\x00"          // 1: mov     ecx, large fs:4
            "\x8b\x04\x81"                          // 2: mov     eax, [ecx+eax*4]
            "\x89\xb0\x08\x00\x00\x00",             // 3: mov     [eax+8], esi
                                                    // --
            "\x90\x90\x90\x90\x90\x90\x90"          // 1: nop
            "\x90\x90\x90"                          // 2: nop
            "\x90\x90\x90\x90\x90\x90");            // 3: nop

        EIP_PATCH(*ip,                              // GetCurrentThreadId()
            "\x64\xa1\x28\x00\x00\x00"              // 1: mov     eax, large fs:28h
            "\x8b\x80\x2c\x01\x00\x00",             // 2: mov     eax, [eax+12Ch]
                                                    // --
            "\x31\xc0\x90\x90\x90\x90"              // 1: xor     eax, eax
            "\x90\x90\x90\x90\x90\x90");            // 2: nop

        EIP_PATCH(*ip,
            "\x64\xa1\x00\x00\x00\x00",             // 1: mov     eax, large fs:0
                                                    // --
            "\x31\xc0\x90\x90\x90\x90");            // 1: xor     eax, eax

        EIP_PATCH(*ip,
            "\x64\xa3\x00\x00\x00\x00",             // 1: mov     large fs:0, eax
                                                    // --
            "\x90\x90\x90\x90\x90\x90");            // 1: nop

        EIP_PATCH(*ip,
            "\x64\x89\x0d\x00\x00\x00\x00",         // 1: mov     large fs:0, ecx
                                                    // --
            "\x90\x90\x90\x90\x90\x90\x90");        // 1: nop

        EIP_PATCH(*ip,
            "\x64\x89\x25\x00\x00\x00\x00",         // 1: mov     large fs:0, esp
                                                    // --
            "\x90\x90\x90\x90\x90\x90\x90");        // 1: nop

        EIP_PATCH(*ip,
            "\x0f\x09",                             // 1: wbinvd (Write Back and Invalidate Cache)
                                                    // --
            "\x90\x90");                            // 1: nop

        if ((*(char *)*ip & 0xff) == 0xee) {                        // out     dx, al
            *ip += 1;                                               //   opcode: 0xee
            i    = uc->uc_mcontext.gregs[REG_EAX] & 0xff;
            bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
            PRINT("out: "
                "[0x%.04hx], "
                "al (0x%.02hhx)",
                *(short *)bp,                                       // dx
                i);                                                 // al
            //TODO
            return;
        }
    } else if (i == SEGV_MAPERR) {
// mov     ds:80000000h, edx // 89 15 00 00 00 80
        if ((*(char *)*ip & 0xff) == 0x89) {
            if ((*(char *)(*ip + 1) & 0xff) == 0x15) {              // mov     ds:<1>, edx
                *ip += 2;                                           //   opcode: 0x89 0x15
                i    = *(int *)*ip;                                 //      <1>: offset
                *ip += 4;                                           // <1> size: dword
                bp   = (void **)&uc->uc_mcontext.gregs[REG_EDX];
                PRINT("mov: "
                    "[0x%.08x], "
                    "edx (0x%.08x)",
                    i,                                              // ds:<1>
                    *(int *)bp);                                    // edx
                if ((i & 0xf0000000) == 0x80000000) {
                    i &= 0x07ffffff;
                    nv2a_dma_write(i, *(int *)bp);
                }
                return;
            }
        }
// mov     dword ptr ds:80000000h, 0DEADBEEFh // C7 05 00 00 00 80 EF BE AD DE
        if ((*(char *)*ip & 0xff) == 0xc7) {
            if ((*(char *)(*ip + 1) & 0xff) == 0x05) {              // mov     ds:<1>, <2>
                *ip += 2;                                           //   opcode: 0xc7 0x05
                i    = *(int *)*ip;                                 //      <1>: offset
                *ip += 4;                                           // <1> size: dword
                PRINT("mov: "
                    "[0x%.08x], "
                    "0x%.08x",
                    i,                                              // ds:<1>
                    *(int *)*ip);                                   // <2>
                if ((i & 0xf0000000) == 0x80000000) {
                    i &= 0x07ffffff;
                    nv2a_dma_write(i, *(int *)*ip);
                }
                *ip += 4;                                           // <2> size: dword
                return;
            }
        }
    }

    ENTER;
    PRINT("Segmentation Fault!", 0);

    PRINT("si_signo \t= %i", signum);
    PRINT("si_errno \t= %i", info->si_errno);
    i = info->si_code;
    PRINT("si_code  \t= %i (%s)",
        info->si_code,
        (i < sizeof(signal_segv_si_codes) / sizeof(*signal_segv_si_codes))
            ? signal_segv_si_codes[i]
            : "(unknown)");
    PRINT("si_addr  \t= %p", info->si_addr);
    for (i = 0; i < NGREG; ++i) {
        n = (i < sizeof(greg_names) / sizeof(*greg_names)) ? greg_names[i] : NULL;
        if (n) PRINT("%s\t= 0x%.08x",
                n,
                uc->uc_mcontext.gregs[i]);
        else PRINT("reg[%02i] \t= 0x%.08x",
                i,
                uc->uc_mcontext.gregs[i]);
    }

    PRINT("Stack trace:", 0);
    for (ip = (void **)&uc->uc_mcontext.gregs[REG_EIP],
        bp = (void **)uc->uc_mcontext.gregs[REG_EBP],
        i = 1;
        bp && *ip;
        ip = (void **)&bp[1],
        bp = (void **)bp[0],
        ++i) {
        PRINT("%02i: bp = %p, ip = %p%s", i, bp, *ip, (i == 1) ? " <- SEGV" : "");
    }

    LEAVE;
    _exit(-1);
}

int
main(int argc, char **argv) {
    struct sigaction s = {
        .sa_sigaction   = signal_segv,
        .sa_flags       = SA_SIGINFO,
    };
    pthread_t entry;
    XbeHeader *xbeh;
    void *addr;
    int fd;
    size_t ctr;
    int ret;

    if (argc < 2) {
        fprintf(stderr, "usage: %s <xbox executable>.xbe [GDB flag]\n", argv[0]);
        return 1;
    }

    if (xboxkrnl_init()) {
        fprintf(stderr, "error: initialization failed\n");
        return 1;
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        PRINT("error: open(): '%s': '%s'", argv[1], strerror(errno));
        return 1;
    }
    addr = (void *)XBE_BASE_ADDR;
    if (mmap(addr,
            PAGESIZE,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_FIXED | MAP_POPULATE | MAP_NORESERVE,
            fd,
            0) != addr) {
        PRINT("error: mmap(): failed to map XBE header @ %p", addr);
        close(fd);
        return 1;
    }
    xbeh = addr;
    if (xbeh->dwMagic != *(uint32_t *)XBE_MAGIC) {
        PRINT("error: invalid XBE magic: 0x%.08x", xbeh->dwMagic);
        close(fd);
        return 1;
    }

    memset(&xbeh->dwInitFlags, 0, sizeof(xbeh->dwInitFlags));

    xbeh->dwEntryAddr ^=
        ((xbeh->dwEntryAddr ^ XBE_XOR_EP_RETAIL) > 0x1000000)
            ? XBE_XOR_EP_DEBUG
            : XBE_XOR_EP_RETAIL;
    xbeh->dwKernelImageThunkAddr ^=
        ((xbeh->dwKernelImageThunkAddr ^ XBE_XOR_KT_RETAIL) > 0x1000000)
            ? XBE_XOR_KT_DEBUG
            : XBE_XOR_KT_RETAIL;

    PRINT("XBE entry: %p | thunk table: %p", xbeh->dwEntryAddr, xbeh->dwKernelImageThunkAddr);

    {
        XbeSectionHeader *xbes;
        const char *sname;
        uint32_t vaddr;
        uint32_t vmask;
        uint32_t vsize;
        uint32_t raddr;
        uint32_t rsize;

        if (xbeh->dwSections) {
            xbes   = (typeof(xbes))xbeh->dwSectionHeadersAddr;
            vaddr  = xbes[0].dwVirtualAddr;
            if ((vmask = vaddr & PAGEMASK)) vaddr -= vmask;
            raddr  = xbes[xbeh->dwSections - 1].dwVirtualAddr;
            raddr += xbes[xbeh->dwSections - 1].dwVirtualSize;
            if ((vmask = raddr & PAGEMASK)) raddr += PAGESIZE - vmask;
            vsize  = raddr - vaddr;

            xboxkrnl_mem_contiguous = (void *)raddr;

            PRINT("mmap(): total section map: "
                "start: 0x%.08x | "
                "end: 0x%.08x | "
                "size: 0x%.08x",
                vaddr, raddr, vsize);

            addr = (typeof(addr))vaddr;
            if (mmap(addr,
                    vsize,
                    PROT_READ | PROT_WRITE | PROT_EXEC,
                    MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS | MAP_NORESERVE,
                    -1,
                    0) != addr) {
                PRINT("error: mmap(): failed to map XBE sections: '%s'", strerror(errno));
                close(fd);
                return 1;
            }
            for (ctr = 0; ctr < xbeh->dwSections; ++ctr) {
                sname = (typeof(sname))xbes[ctr].dwSectionNameAddr;
                vaddr = xbes[ctr].dwVirtualAddr;
                vsize = xbes[ctr].dwVirtualSize;
                raddr = xbes[ctr].dwRawAddr;
                rsize = xbes[ctr].dwSizeofRaw;

                PRINT("mmap(): section %2u | "
                    "name %10s | "
                    "virtual: address 0x%.08x size 0x%.08x | "
                    "raw: address 0x%.08x size 0x%.08x",
                    ctr, sname, vaddr, vsize, raddr, rsize);

                if (lseek(fd, raddr, SEEK_SET) != (int)raddr) {
                    PRINT("error: lseek(): '%s'", strerror(errno));
                    close(fd);
                    return 1;
                }
                if ((ret = read(fd, (void *)vaddr, rsize)) != (int)rsize) {
                    PRINT("error: read(): '%s'", (ret < 0) ? strerror(errno) : "short read");
                    close(fd);
                    return 1;
                }
                if (!strncmp(sname, ".text", sizeof(".text"))) {
                    xbeh->dwEntryAddr = vaddr;
//                    xboxkrnl_patch_seh((void *)vaddr, rsize);

#if 0 //FIXME turok
# if 0
                                    // SetLastError()
*(short *)0x118f2 = 0xc031;         // movzx   eax, large byte ptr fs:24h
*(int *)  0x118f4 = 0x90909090;     // 64 0F B6 05 24 00 00 00
*(short *)0x118f8 = 0x9090;

*(short *)0x118fe = 0xc031;         // mov     eax, large fs:28h
*(int *)  0x11900 = 0x90909090;     // 64 A1 28 00 00 00

*(short *)0x11904 = 0xc931;         // mov     ecx, large fs:4
*(int *)  0x11906 = 0x90909090;     // 64 8B 0D 04 00 00 00
*(char *) 0x1190a = 0x90;
# else
                                    // SetLastError()
*(int *)  0x118f2 = 0xcc0004c2;     // movzx   eax, large byte ptr fs:24h
                                    // 64 0F B6 05 24 00 00 00
# endif
                                    // GetLastError()
*(char *) 0x118ca = 0xc3;           // movzx   eax, large byte ptr fs:24h
                                    // 64 0F B6 05 24 00 00 00

*(short *)0x11005 = 0xc031;         // mov     eax, large fs:20h
*(int *)  0x11007 = 0x90909090;     // 64 A1 20 00 00 00
*(int *)  0x1100b = 0x90909090;     // mov     eax, [eax+250h]
*(short *)0x1100f = 0x9090;         // 8B 80 50 02 00 00

*(short *)0x141b0 = 0xc031;         // mov     eax, large fs:20h
*(int *)  0x141b2 = 0x90909090;     // 64 A1 20 00 00 00
*(int *)  0x141b6 = 0x90909090;     // mov     eax, [eax+250h]
*(short *)0x141ba = 0x9090;         // 8B 80 50 02 00 00

*(int *)  0xf8ba7 = 0x90909090;     // mov     ecx, large fs:4
*(int *)  0xf8bab = 0x90909090;     // 64 8B 0D 04 00 00 00
*(short *)0xf8baf = 0x9090;         // mov     eax, [ecx+eax*4]
                                    // 8B 04 81
*(int *)  0xf8bb1 = 0x90909090;     // mov     [eax+8], esi
*(short *)0xf8bb5 = 0x9090;         // 89 B0 08 00 00 00

*(short *)0x11bec = 0xc031;         // mov     eax, large fs:28h
*(int *)  0x11bee = 0x90909090;     // 64 A1 28 00 00 00
*(int *)  0x11bf2 = 0x90909090;     // mov     eax, [eax+12Ch]
*(short *)0x11bf6 = 0x9090;         // 8B 80 2C 01 00 00

*(short *)0x14227 = 0xc031;         // mov     eax, large fs:20h
*(int *)  0x14229 = 0x90909090;     // 64 A1 20 00 00 00
*(int *)  0x1422d = 0x90909090;     // mov     eax, [eax+250h]
*(short *)0x14231 = 0x9090;         // 8B 80 50 02 00 00

*(short *)0x101797 = 0xc031;        // mov     eax, large fs:0
*(int *)  0x101799 = 0x90909090;    // 64 A1 00 00 00 00

*(int *)  0x10179e = 0x90909090;    // mov     large fs:0, esp
*(short *)0x1017a2 = 0x9090;        // 64 89 25 00 00 00 00
*(char *) 0x1017a4 = 0x90;

*(int *)  0x101814 = 0x90909090;    // mov     large fs:0, ecx
*(short *)0x101818 = 0x9090;        // 64 89 0D 00 00 00 00
*(char *) 0x10181a = 0x90;

*(int *)  0x10182d = 0x90909090;    // mov     large fs:0, ecx
*(short *)0x101831 = 0x9090;        // 64 89 0D 00 00 00 00
*(char *) 0x101833 = 0x90;

*(short *)0x1014f0 = 0xc031;        // mov     eax, large fs:0
*(int *)  0x1014f2 = 0x90909090;    // 64 A1 00 00 00 00

*(int *)  0x101502 = 0x90909090;    // mov     large fs:0, esp
*(short *)0x101506 = 0x9090;        // 64 89 25 00 00 00 00
*(char *) 0x101508 = 0x90;

*(int *)  0x101594 = 0x90909090;    // mov     large fs:0, ecx
*(short *)0x101598 = 0x9090;        // 64 89 0D 00 00 00 00
*(char *) 0x10159a = 0x90;

*(short *)0x98b47 = 0xc031;         // mov     eax, large fs:0
*(int *)  0x98b49 = 0x90909090;     // 64 A1 00 00 00 00

*(int *)  0x98b4e = 0x90909090;     // mov     large fs:0, esp
*(short *)0x98b52 = 0x9090;         // 64 89 25 00 00 00 00
*(char *) 0x98b54 = 0x90;

*(int *)  0x98be9 = 0x90909090;     // mov     large fs:0, ecx
*(short *)0x98bed = 0x9090;         // 64 89 0D 00 00 00 00
*(char *) 0x98bef = 0x90;

*(short *)0x16b50 = 0xc031;         // mov     eax, large fs:0
*(int *)  0x16b52 = 0x90909090;     // 64 A1 00 00 00 00

*(int *)  0x16b5e = 0x90909090;     // mov     large fs:0, esp
*(short *)0x16b62 = 0x9090;         // 64 89 25 00 00 00 00
*(char *) 0x16b64 = 0x90;

*(int *)  0x16d64 = 0x90909090;     // mov     large fs:0, ecx
*(short *)0x16d68 = 0x9090;         // 64 89 0D 00 00 00 00
*(char *) 0x16d6a = 0x90;

*(short *)0x17b32 = 0xc031;         // mov     eax, large fs:0
*(int *)  0x17b34 = 0x90909090;     // 64 A1 00 00 00 00

*(int *)  0x17b3e = 0x90909090;     // mov     large fs:0, esp
*(short *)0x17b42 = 0x9090;         // 64 89 25 00 00 00 00
*(char *) 0x17b44 = 0x90;

*(int *)  0x17d64 = 0x90909090;     // mov     large fs:0, ecx
*(short *)0x17d68 = 0x9090;         // 64 89 0D 00 00 00 00
*(char *) 0x17d6a = 0x90;

*(short *)0x25337 = 0xc031;         // mov     eax, large fs:0
*(int *)  0x25339 = 0x90909090;     // 64 A1 00 00 00 00

*(int *)  0x2533e = 0x90909090;     // mov     large fs:0, esp
*(short *)0x25342 = 0x9090;         // 64 89 25 00 00 00 00
*(char *) 0x25344 = 0x90;

*(int *)  0x2553f = 0x90909090;     // mov     large fs:0, ecx
*(short *)0x25543 = 0x9090;         // 64 89 0D 00 00 00 00
*(char *) 0x25545 = 0x90;
#endif
                }
            }
        }
    }

    close(fd);

    {
        char buf[32];
        snprintf(buf, sizeof(buf), "cat /proc/%hu/maps", getpid());
        system(buf);
    }

    xboxkrnl_thunk_resolve((void *)xbeh->dwKernelImageThunkAddr);

    if (sigaction(SIGSEGV, &s, NULL) < 0) {
        PRINT("error: sigaction(): '%s'", strerror(errno));
        return 1;
    }

    PRINT("/* spawning entry thread with entry point @ %p */", xbeh->dwEntryAddr);

    if (argc >= 3) INT3;

    pthread_create(&entry, NULL, (void *)xbeh->dwEntryAddr, NULL);
    pthread_join(entry, NULL);

    return 0;
}

