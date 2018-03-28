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

static void
XbeHeader_dump(const XbeHeader *xbeh) {
    if (!dbg || !xbeh) return;
    ENTER;

    VARDUMP(DUMP, xbeh);
    VARDUMP(DUMP, xbeh->dwMagic);
    STRDUMPR(xbeh->dwMagic);
    VARDUMP(DUMP, xbeh->pbDigitalSignature);
    HEXDUMP(xbeh->pbDigitalSignature);
    VARDUMP(DUMP, xbeh->dwBaseAddr);
    VARDUMP(DUMP, xbeh->dwSizeofHeaders);
    VARDUMP(DUMP, xbeh->dwSizeofImage);
    VARDUMP(DUMP, xbeh->dwSizeofImageHeader);
    VARDUMP(DUMP, xbeh->dwTimeDate);
    VARDUMP(DUMP, xbeh->dwCertificateAddr);
    VARDUMP(DUMP, xbeh->dwSections);
    VARDUMP(DUMP, xbeh->dwSectionHeadersAddr);
    VARDUMP(DUMP, xbeh->dwInitFlags.bMountUtilityDrive);
    VARDUMP(DUMP, xbeh->dwInitFlags.bFormatUtilityDrive);
    VARDUMP(DUMP, xbeh->dwInitFlags.bLimit64MB);
    VARDUMP(DUMP, xbeh->dwInitFlags.bDontSetupHarddisk);
    VARDUMP(DUMP, xbeh->dwEntryAddr);
    VARDUMP(DUMP, xbeh->dwTLSAddr);
    VARDUMP(DUMP, xbeh->dwPeStackCommit);
    VARDUMP(DUMP, xbeh->dwPeHeapReserve);
    VARDUMP(DUMP, xbeh->dwPeHeapCommit);
    VARDUMP(DUMP, xbeh->dwPeBaseAddr);
    VARDUMP(DUMP, xbeh->dwPeSizeofImage);
    VARDUMP(DUMP, xbeh->dwPeChecksum);
    VARDUMP(DUMP, xbeh->dwPeTimeDate);
    VARDUMP(DUMP, xbeh->dwDebugPathnameAddr);
    STRDUMP(xbeh->dwDebugPathnameAddr);
    VARDUMP(DUMP, xbeh->dwDebugFilenameAddr);
    STRDUMP(xbeh->dwDebugFilenameAddr);
    VARDUMP(DUMP, xbeh->dwDebugUnicodeFilenameAddr);
    STRDUMPW(xbeh->dwDebugUnicodeFilenameAddr);
    VARDUMP(DUMP, xbeh->dwKernelImageThunkAddr);
    VARDUMP(DUMP, xbeh->dwNonKernelImportDirAddr);
    VARDUMP(DUMP, xbeh->dwLibraryVersions);
    VARDUMP(DUMP, xbeh->dwLibraryVersionsAddr);
    VARDUMP(DUMP, xbeh->dwKernelLibraryVersionAddr);
    VARDUMP(DUMP, xbeh->dwXAPILibraryVersionAddr);
    VARDUMP(DUMP, xbeh->dwLogoBitmapAddr);
    VARDUMP(DUMP, xbeh->dwSizeofLogoBitmap);

    LEAVE;
}

static void
XbeSectionHeader_dump(const XbeSectionHeader *xbes) {
    if (!dbg || !xbes) return;
    ENTER;

    VARDUMP(DUMP, xbes);
    VARDUMP(DUMP, xbes->dwFlags.bWritable);
    VARDUMP(DUMP, xbes->dwFlags.bPreload);
    VARDUMP(DUMP, xbes->dwFlags.bExecutable);
    VARDUMP(DUMP, xbes->dwFlags.bInsertedFile);
    VARDUMP(DUMP, xbes->dwFlags.bHeadPageRO);
    VARDUMP(DUMP, xbes->dwFlags.bTailPageRO);
    VARDUMP(DUMP, xbes->dwVirtualAddr);
    VARDUMP(DUMP, xbes->dwVirtualSize);
    VARDUMP(DUMP, xbes->dwRawAddr);
    VARDUMP(DUMP, xbes->dwSizeofRaw);
    VARDUMP(DUMP, xbes->dwSectionNameAddr);
    STRDUMP(xbes->dwSectionNameAddr);
    VARDUMP(DUMP, xbes->dwSectionRefCount);
    VARDUMP(DUMP, xbes->dwHeadSharedRefCountAddr);
    VARDUMP(DUMP, xbes->dwTailSharedRefCountAddr);
    VARDUMP(DUMP, xbes->bzSectionDigest);
    HEXDUMP(xbes->bzSectionDigest);

    LEAVE;
}

static void
XbeTLS_dump(const XbeTLS *xbet) {
    if (!dbg || !xbet) return;
    ENTER;

    VARDUMP(DUMP, xbet);
    VARDUMP(DUMP, xbet->dwDataStartAddr);
    VARDUMP(DUMP, xbet->dwDataEndAddr);
    VARDUMP(DUMP, xbet->dwTLSIndexAddr);
    VARDUMP(DUMP, xbet->dwTLSCallbackAddr);
    VARDUMP(DUMP, xbet->dwSizeofZeroFill);
    VARDUMP(DUMP, xbet->dwCharacteristics);

    LEAVE;
}

