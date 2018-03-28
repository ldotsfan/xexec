/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  Copyright (C) 2012-2018  Michael Saga. All rights reserved.
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

#ifndef XBE_H
#define XBE_H

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
#endif

