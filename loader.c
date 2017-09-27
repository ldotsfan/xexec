/* gcc -m32 -g -Wall -Wextra loader.c -o loader -pthread */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <errno.h>

#include <pthread.h>

//#include <wine/library.h>

static int dbg = 1;

void
debug(const char *format, ...) {
    va_list args;
    char buf[1024];
    char time[32];
    struct timeval tv;
    struct tm *tm;

    if (!dbg) return;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    gettimeofday(&tv, NULL);
    if (!(tm = localtime(&tv.tv_sec)) || !strftime(time, sizeof(time), "%T", tm)) {
        fprintf(stderr, "xbox: %s\n", buf);
    } else {
        fprintf(stderr, "xbox: %s.%06lu: %s\n", time, tv.tv_usec, buf);
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

#include "xboxkrnl.c"

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

int
patch_seh(void *addr, size_t size) {
    size_t i;
    size_t tmp;
    int ret = 0;

    printf("searching & patching SEH prolog & epilog routines... ");

    for (i = 0; !ret && i <= size; ++i) {
        if (i >= size) {
            printf("not found.\n");
            break;
        }
        for (tmp = 0; i + tmp < size && tmp <= sizeof(X86_SEH_PROLOG) - 1; ++tmp) {
            if (tmp >= sizeof(X86_SEH_PROLOG) - 1) {
                if (!memcmp(addr + i + tmp, X86_SEH_EPILOG, sizeof(X86_SEH_EPILOG) - 1)) {
                    printf("found!\nSEH prolog & epilog @ %p\n", addr + i);
//                    *((char *)addr + i)             = 0xc3;
                    memcpy(addr + i, X86_SEH_PROLOG_PATCH, sizeof(X86_SEH_PROLOG_PATCH) - 1);
                    printf("SEH prolog patched @ %p\n", addr + i);
//                    *((char *)addr + i + tmp)       = 0xc3;
                    memcpy(addr + i + tmp, X86_SEH_EPILOG_PATCH, sizeof(X86_SEH_EPILOG_PATCH) - 1);
                    printf("SEH epilog patched @ %p\n", addr + i + tmp);
                    ret = 2;
                } else {
                    printf("found!\nSEH prolog @ %p\n", addr + i);
//                    *((char *)addr + i)             = 0xc3;
                    memcpy(addr + i, X86_SEH_PROLOG_PATCH, sizeof(X86_SEH_PROLOG_PATCH) - 1);
                    printf("SEH prolog patched @ %p\n", addr + i);
                    ret = 1;
                }
                break;
            }
            if (((char *)addr)[i + tmp] != X86_SEH_PROLOG[tmp]) break;
        }
    }
    if (ret == 1) {
        printf("searching & patching SEH epilog routine... ");

        for (i = 0; ret == 1 && i <= size; ++i) {
            if (i >= size) {
                printf("not found.\n");
                break;
            }
            for (tmp = 0; i + tmp < size && tmp <= sizeof(X86_SEH_EPILOG) - 1; ++tmp) {
                if (tmp >= sizeof(X86_SEH_EPILOG) - 1) {
                    printf("found!\nSEH epilog @ %p\n", addr + i);
//                    *((char *)addr + i)             = 0xc3;
                    memcpy(addr + i, X86_SEH_EPILOG_PATCH, sizeof(X86_SEH_EPILOG_PATCH) - 1);
                    printf("SEH epilog patched @ %p\n", addr + i);
                    ret = 2;
                    break;
                }
                if (((char *)addr)[i + tmp] != X86_SEH_EPILOG[tmp]) break;
            }
        }
    }

    return ret;
}

int
main(int argc, char **argv) {
    XbeHeader *xbeh;
    XbeSectionHeader *xbes;
    void *addr;
    int fd;
    size_t ctr;
    int ret;

    if (argc < 2) {
        fprintf(stderr, "usage: %s <xbox executable>.xbe\n", argv[0]);
        return 1;
    }

    if (xboxkrnl_init_hw()) return 1;

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        fprintf(stderr, "error: open: %s: %s\n", argv[1], strerror(errno));
        return 1;
    }
    addr = (void *)XBE_BASE_ADDR;
    if (mmap(addr, PAGESIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED | MAP_NORESERVE | MAP_POPULATE, fd, 0) != addr) {
        fprintf(stderr, "error: could not mmap xbe header at %p\n", addr);
        close(fd);
        return 1;
    }
    xbeh = addr;
    if (xbeh->dwMagic != *(uint32_t *)XBE_MAGIC) {
        fprintf(stderr, "error: invalid xbe magic: 0x%.08x\n", xbeh->dwMagic);
        close(fd);
        return 1;
    }

    xbeh->dwEntryAddr ^=
        ((xbeh->dwEntryAddr ^ XBE_XOR_EP_RETAIL) > 0x1000000)
            ? XBE_XOR_EP_DEBUG
            : XBE_XOR_EP_RETAIL;
    xbeh->dwKernelImageThunkAddr ^=
        ((xbeh->dwKernelImageThunkAddr ^ XBE_XOR_KT_RETAIL) > 0x1000000)
            ? XBE_XOR_KT_DEBUG
            : XBE_XOR_KT_RETAIL;

    printf("xbe entry: 0x%x | thunk table: 0x%x\n", xbeh->dwEntryAddr, xbeh->dwKernelImageThunkAddr);

    xbes = (void *)xbeh->dwSectionHeadersAddr;
    {
        uint32_t vaddr;
        uint32_t vmask;
        uint32_t vsize;
        uint32_t raddr;
        uint32_t rsize;

        for (ctr = 0; ctr < xbeh->dwSections; ++ctr) {
            vaddr = xbes[ctr].dwVirtualAddr;
            vmask = vaddr & PAGEMASK;
            vsize = xbes[ctr].dwVirtualSize;
            raddr = xbes[ctr].dwRawAddr;
            rsize = xbes[ctr].dwSizeofRaw;

            printf("mmap: section %2u | "
                "name %10s | "
                "virtual: address 0x%.08x size 0x%.08x | "
                "raw: address 0x%.08x size 0x%.08x\n",
                ctr, (char *)xbes[ctr].dwSectionNameAddr, vaddr, vsize, raddr, rsize);

            addr = (void *)vaddr - vmask;
            if (mmap(addr,
                    vsize + vmask,
                    PROT_READ | PROT_WRITE | PROT_EXEC,
                    MAP_PRIVATE | MAP_FIXED | MAP_NORESERVE | MAP_ANONYMOUS,
                    -1,
                    0) != addr) {
                fprintf(stderr, "error: could not mmap xbe section %u @ %p\n", ctr, addr);
                close(fd);
                return 1;
            }
            if (lseek(fd, raddr, SEEK_SET) != (int)raddr) {
                fprintf(stderr, "error: lseek: %s: %s\n", argv[1], strerror(errno));
                close(fd);
                return 1;
            }
            if ((ret = read(fd, (void *)vaddr, rsize)) != (int)rsize) {
                fprintf(stderr, "error: read: %s: %s\n", argv[1], (ret < 0) ? strerror(errno) : "short read");
                close(fd);
                return 1;
            }
            if (!strncmp((char *)xbes[ctr].dwSectionNameAddr, ".text", sizeof(".text"))) {
                patch_seh((void *)vaddr, rsize);
            }
        }
    }

    close(fd);

    {
        char buf[32];
        snprintf(buf, sizeof(buf), "cat /proc/%hu/maps", getpid());
        system(buf);
    }

    {
        const void **ktt = (void *)xbeh->dwKernelImageThunkAddr;
        uint32_t t;

        printf("patching in .rdata kernel thunk table @ %p\n", ktt);

        for (ctr = 0; ktt[ctr]; ++ctr) {
            t = ((typeof(t) *)ktt)[ctr] & 0x1ff;
		    ktt[ctr] = xboxkrnl_thunk[t];
		    printf("thunk %03u (0x%03x): .rdata table @ %p <- thunk %p %s()\n",
	            t,
	            t,
	            &ktt[ctr],
	            ktt[ctr],
	            xboxkrnl_thunk_name[t]);
		}

        printf("resolved %zu .rdata kernel thunk exports\n", ctr);
    }

xbeh->dwEntryAddr = 0x11000; /* FIXME .text vaddr */
    printf("jumping into the entry point @ %p\n", (void *)xbeh->dwEntryAddr);

    INT3;

    goto *xbeh->dwEntryAddr;

    return 0;
}

