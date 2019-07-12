/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  Copyright (c) 2017-2019 Michael Saga
 *  All rights reserved.
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

/* same as in ucontext.h */
enum {
    X86_GS = 0,
    X86_FS,
    X86_ES,
    X86_DS,
    X86_EDI,
    X86_ESI,
    X86_EBP,
    X86_ESP,
    X86_EBX,
    X86_EDX,
    X86_ECX,
    X86_EAX,
    X86_TRAPNO,
    X86_ERR,
    X86_EIP,
    X86_CS,
    X86_EFL,
    X86_UESP,
    X86_SS,
    X86_MAX
};

static const char *const x86_greg_name[] = {
    [X86_GS]     = " gs",
    [X86_FS]     = " fs",
    [X86_ES]     = " es",
    [X86_DS]     = " ds",
    [X86_EDI]    = "edi",
    [X86_ESI]    = "esi",
    [X86_EBP]    = "ebp",
    [X86_ESP]    = "esp",
    [X86_EBX]    = "ebx",
    [X86_EDX]    = "edx",
    [X86_ECX]    = "ecx",
    [X86_EAX]    = "eax",
    [X86_TRAPNO] = "tno",
    [X86_ERR]    = "err",
    [X86_EIP]    = "eip",
    [X86_CS]     = " cs",
    [X86_EFL]    = "efl",
    [X86_UESP]   = "esp",
    [X86_SS]     = " ss",
};

typedef struct {
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
} x86_eflags;

static const char *const x86_eflags_name[] = {
    NAMEB(0,  carry),
    NAMEB(2,  parity),
    NAMEB(4,  adjust),
    NAMEB(6,  zero),
    NAMEB(7,  sign),
    NAMEB(8,  trap),
    NAMEB(9,  irq_enable),
    NAMEB(10, direction),
    NAMEB(11, overflow),
    NAMEB(14, nested),
    NAMEB(16, resume),
    NAMEB(17, vm86),
    NAMEB(18, align),
    NAMEB(19, virq),
    NAMEB(20, virq_pending),
    NAMEB(21, cpuid),
};
#if 0
typedef struct {
    //TODO
} x86_context;

void
x86_push_callback(ucontext_t *uc) {

}
#endif
int
x86_iterate(ucontext_t *uc, int si_code) {
    register void **bp;
    register void **ip = (void **)&uc->uc_mcontext.gregs[X86_EIP];
    register x86_eflags *fl = (void *)&uc->uc_mcontext.gregs[X86_EFL];
    int prefix = 0;
    uint32_t sz = 4;
    uint32_t i;
    uint32_t v;
    register int ret = 0;

    switch (si_code) {
    case 0:
    case SEGV_MAPERR /* 1 */:
    case SEGV_ACCERR /* 2 */:
        if ((prefix = (REG08(*ip) == 0x66))) {                      /* operand-size override prefix */
            sz   = 2;
            *ip += 1;
        }
        do {
            if (REG08(*ip) == 0xf3) {                               // rep
// rep movsd // F3 A5
                if (REG08(*ip + 1) == 0xa5) {                       // rep movsd
//break;//XXX turok dirty disc trigger
//INT3;//XXX
                    register uint32_t *c   = (void *)&uc->uc_mcontext.gregs[X86_ECX];
                    register uint32_t *di  = (void *)&uc->uc_mcontext.gregs[X86_EDI];
                    register uint32_t **si = (void *)&uc->uc_mcontext.gregs[X86_ESI];
                    while (*c && xboxkrnl_write(REG32(di), *si, sz)) {
//PRINT(XEXEC_DBG_ALL,"x86: rep movsd: c=%u di=%p si=%p (0x%.08x)",*c,*di,*si,**si);//XXX
                        --*c;
                        if (fl->df) *di -= 4, *si -= 1;
                        else        *di += 4, *si += 1;
                    }
                    if (!*c) {
                        *ip += 2;                                   //   opcode: 0xf3 0xa5
                        ret = 1;
                    }
                    break;
                }
// rep stosd // F3 AB
                if (REG08(*ip + 1) == 0xab) {                       // rep stosd
                    register uint32_t *a  = (void *)&uc->uc_mcontext.gregs[X86_EAX];
                    register uint32_t *c  = (void *)&uc->uc_mcontext.gregs[X86_ECX];
                    register uint32_t *di = (void *)&uc->uc_mcontext.gregs[X86_EDI];
                    while (*c && xboxkrnl_write(REG32(di), a, sz)) {
                        --*c;
                        if (fl->df) *di -= 4;
                        else        *di += 4;
                    }
                    if (!*c) {
                        *ip += 2;                                   //   opcode: 0xf3 0xab
                        ret = 1;
                    }
                    break;
                }
            }
// cmp     [esi+2100h], edi // 39 BE 00 21 00 00
            if (REG08(*ip) == 0x39) {                               // cmp
                if (REG08(*ip + 1) == 0xbe) {                       // cmp     [esi+<1>], edi
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    if ((ret = xboxkrnl_read(REG32(&i), &v, sz))) {
                        *ip   += 2;                                 //   opcode: 0x39 0xbe <1:dword>
                        *ip   += 4;                                 // <1> size: dword
                        bp     = (void **)&uc->uc_mcontext.gregs[X86_EDI];
                        fl->zf = (REG32(&v) == REG32(bp));
                        PRINT(XEXEC_DBG_REG,
                            "cmp: "
                            "([0x%.08x] (0x%.08x) == "
                            "edi (0x%.08x)) = %u",
                            i, REG32(&v),                           // [esi+<1>]
                            REG32(bp),                              // edi
                            fl->zf);                                // zero flag
                    }
                    break;
                }
            }
// cmp     dword ptr [esi+400100h], 0 // 83 BE 00 01 40 00 00
            if (REG08(*ip) == 0x83) {                               // cmp
                if (REG08(*ip + 1) == 0xbe) {                       // cmp     [esi+<1>], <2>
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    if ((ret = xboxkrnl_read(REG32(&i), &v, sz))) {
                        *ip   += 2;                                 //   opcode: 0x83 0xbe <1:dword> <2:byte>
                        *ip   += 4;                                 // <1> size: dword
                        fl->zf = (REG32(&v) == REG08(*ip));
                        PRINT(XEXEC_DBG_REG,
                            "cmp: "
                            "([0x%.08x] (0x%.08x) == "
                            "0x%.02hhx) = %u",
                            i, REG32(&v),                           // [esi+<1>]
                            REG08(*ip),                             // <2>
                            fl->zf);                                // zero flag
                        *ip += 1;                                   // <2>: value, <2> size: byte
                    }
                    break;
                }
            }
// cmp     eax, [ecx] // 3B 01
            if (REG08(*ip) == 0x3b) {                               // cmp
                if (REG08(*ip + 1) == 0x01) {                       // cmp     eax, [ecx]
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    if ((ret = xboxkrnl_read(REG32(&i), &v, sz))) {
                        *ip   += 2;                                 //   opcode: 0x3b 0x01
                        bp     = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                        fl->zf = (REG32(bp) == REG32(&v));
                        PRINT(XEXEC_DBG_REG,
                            "cmp: "
                            "(eax (0x%.08x) == "
                            "[0x%.08x] (0x%.08x)) = %u",
                            REG32(bp),                              // eax
                            i, REG32(&v),                           // [ecx]
                            fl->zf);                                // zero flag
                    }
                    break;
                }
// cmp     eax, [ecx+400908h] // 3B 81 08 09 40 00
                if (REG08(*ip + 1) == 0x81) {                       // cmp     eax, [ecx+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    if ((ret = xboxkrnl_read(REG32(&i), &v, sz))) {
                        *ip   += 2;                                 //   opcode: 0x3b 0x81 <1:dword>
                        *ip   += 4;                                 // <1> size: dword
                        bp     = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                        fl->zf = (REG32(bp) == REG32(&v));
                        PRINT(
                            XEXEC_DBG_REG,
                            "cmp: "
                            "(eax (0x%.08x) == "
                            "[0x%.08x] (0x%.08x)) = %u",
                            REG32(bp),                              // eax
                            i, REG32(&v),                           // [ecx+<1>]
                            fl->zf);                                // zero flag
                    }
                    break;
                }
// cmp     eax, [esi+edi*4+400980h] // 3B 84 BE 80 09 40 00
                if (REG08(*ip + 1) == 0x84) {
                    if (REG08(*ip + 2) == 0xbe) {                   // cmp     eax, [esi+edi*4+<1>]
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_EDI]) * 4;
                        REG32(&i) += REG32(*ip + 3);                //      <1>: offset
                        if ((ret = xboxkrnl_read(REG32(&i), &v, sz))) {
                            *ip   += 3;                             //   opcode: 0x3b 0x84 0xbe <1:dword>
                            *ip   += 4;                             // <1> size: dword
                            bp     = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                            fl->zf = (REG32(bp) == REG32(&v));
                            PRINT(
                                XEXEC_DBG_REG,
                                "cmp: "
                                "(eax (0x%.08x) == "
                                "[0x%.08x] (0x%.08x)) = %u",
                                REG32(bp),                          // eax
                                i, REG32(&v),                       // [esi+edi*4+<1>]
                                fl->zf);                            // zero flag
                        }
                        break;
                    }
                }
            }
// test    dword ptr [eax+100410h], 10000h // F7 80 10 04 10 00 00 00 01 00
            if (REG08(*ip) == 0xf7) {                               // test
                if (REG08(*ip + 1) == 0x80) {                       // test    [eax+<1>], <2>
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    if ((ret = xboxkrnl_read(REG32(&i), &v, sz))) {
                        *ip   += 2;                                 //   opcode: 0xf7 0x80 <1:dword> <2:dword>
                        *ip   += 4;                                 // <1> size: dword
                        fl->zf = !(REG32(&v) & REG32(*ip));
                        PRINT(
                            XEXEC_DBG_REG,
                            "test: "
                            "!([0x%.08x] (0x%.08x) & "
                            "0x%.08x) = %u",
                            i, REG32(&v),                           // [eax+<1>]
                            REG32(*ip),                             // <2>
                            fl->zf);                                // zero flag
                        *ip += 4;                                   // <2>: value, <2> size: dword
                    }
                    break;
                }
// test    dword ptr [esi+100h], 1000000h // F7 86 00 01 00 00 00 00 00 01
                if (REG08(*ip + 1) == 0x86) {                       // test    [esi+<1>], <2>
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    if ((ret = xboxkrnl_read(REG32(&i), &v, sz))) {
                        *ip   += 2;                                 //   opcode: 0xf7 0x86 <1:dword> <2:dword>
                        *ip   += 4;                                 // <1> size: dword
                        fl->zf = !(REG32(&v) & REG32(*ip));
                        PRINT(
                            XEXEC_DBG_REG,
                            "test: "
                            "!([0x%.08x] (0x%.08x) & "
                            "0x%.08x) = %u",
                            i, REG32(&v),                           // [esi+<1>]
                            REG32(*ip),                             // <2>
                            fl->zf);                                // zero flag
                        *ip += 4;                                   // <2>: value, <2> size: dword
                    }
                    break;
                }
            }
            if (REG08(*ip) == 0xf6) {                               // test
                sz = 1;
// test    byte ptr [esi+3214h], 10h // F6 86 14 32 00 00 10
                if (REG08(*ip + 1) == 0x86) {                       // test    [esi+<1>], <2>
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    if ((ret = xboxkrnl_read(REG32(&i), &v, sz))) {
                        *ip   += 2;                                 //   opcode: 0xf6 0x86 <1:dword> <2:byte>
                        *ip   += 4;                                 // <1> size: dword
                        fl->zf = !(REG08(&v) & REG08(*ip));
                        PRINT(
                            XEXEC_DBG_REG,
                            "test: "
                            "!([0x%.08x] (0x%.02hhx) & "
                            "0x%.02hhx) = %u",
                            i, REG08(&v),                           // [esi+<1>]
                            REG08(*ip),                             // <2>
                            fl->zf);                                // zero flag
                        *ip += 1;                                   // <2>: value, <2> size: byte
                    }
                    break;
                }
// test    byte ptr [eax], 10h // F6 00 10
                if (REG08(*ip + 1) == 0x00) {                       // test    [eax], <1>
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    if ((ret = xboxkrnl_read(REG32(&i), &v, sz))) {
                        *ip   += 2;                                 //   opcode: 0xf6 0x00 <1:byte>
                        fl->zf = !(REG08(&v) & REG08(*ip));
                        PRINT(
                            XEXEC_DBG_REG,
                            "test: "
                            "!([0x%.08x] (0x%.02hhx) & "
                            "0x%.02hhx) = %u",
                            i, REG08(&v),                           // [eax]
                            REG08(*ip),                             // <1>
                            fl->zf);                                // zero flag
                        *ip += 1;                                   // <1>: value, <1> size: byte
                    }
                    break;
                }
// test    byte ptr ds:0FEC00134h, 1 // F6 05 34 01 C0 FE 01
                if (REG08(*ip + 1) == 0x05) {                       // test    ds:<1>, <2>
                    REG32(&i) = REG32(*ip + 2);                     //      <1>: offset
                    if ((ret = xboxkrnl_read(REG32(&i), &v, sz))) {
                        *ip   += 2;                                 //   opcode: 0xf6 0x05 <1:dword> <2:byte>
                        *ip   += 4;                                 // <1> size: dword
                        fl->zf = !(REG08(&v) & REG08(*ip));
                        PRINT(
                            XEXEC_DBG_REG,
                            "test: "
                            "!([0x%.08x] (0x%.02hhx) & "
                            "0x%.02hhx) = %u",
                            i, REG08(&v),                           // <1>
                            REG08(*ip),                             // <2>
                            fl->zf);                                // zero flag
                        *ip += 1;                                   // <2>: value, <2> size: byte
                    }
                    break;
                }
// test    byte ptr [eax-13FFEFAh], 1 // F6 80 06 01 C0 FE 01
                if (REG08(*ip + 1) == 0x80) {                       // test    [eax+<1>], <2>
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    if ((ret = xboxkrnl_read(REG32(&i), &v, sz))) {
                        *ip   += 2;                                 //   opcode: 0xf6 0x80 <1:dword> <2:byte>
                        *ip   += 4;                                 // <1> size: dword
                        fl->zf = !(REG08(&v) & REG08(*ip));
                        PRINT(
                            XEXEC_DBG_REG,
                            "test: "
                            "!([0x%.08x] (0x%.02hhx) & "
                            "0x%.02hhx) = %u",
                            i, REG08(&v),                           // [eax+<1>]
                            REG08(*ip),                             // <2>
                            fl->zf);                                // zero flag
                        *ip += 1;                                   // <2>: value, <2> size: byte
                    }
                    break;
                }
            }
            if (REG08(*ip) == 0x85) {                               // test
// test    ds:0FEC00130h, esi // 85 35 30 01 C0 FE
                if (REG08(*ip + 1) == 0x35) {                       // test    ds:<1>, esi
                    REG32(&i) = REG32(*ip + 2);                     //      <1>: offset
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_ESI];
                    if ((ret = xboxkrnl_read(REG32(&i), &v, sz))) {
                        *ip   += 2;                                 //   opcode: 0x85 0x35 <1:dword>
                        *ip   += 4;                                 // <1> size: dword
                        fl->zf = !(REG32(&v) & REG32(bp));
                        PRINT(
                            XEXEC_DBG_REG,
                            "test: "
                            "!([0x%.08x] (0x%.08x) & "
                            "0x%.08x) = %u",
                            i, REG32(&v),                           // <1>
                            REG32(bp),                              // esi
                            fl->zf);                                // zero flag
                    }
                    break;
                }
            }
            if (REG08(*ip) == 0xa1) {                               // mov     eax, ds:<1>
                REG32(&i) = REG32(*ip + 1);                         //      <1>: offset
                bp        = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                    *ip += 1;                                       //   opcode: 0xa1 <1:dword>
                    *ip += 4;                                       // <1> size: dword
                }
                break;
            }
            if (REG08(*ip) == 0xa3) {                               // mov     ds:<1>, eax
                REG32(&i) = REG32(*ip + 1);                         //      <1>: offset
                bp        = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                    *ip += 1;                                       //   opcode: 0xa3 <1:dword>
                    *ip += 4;                                       // <1> size: dword
                }
                break;
            }
            if (REG08(*ip) == 0xc7) {                               // mov
                if (REG08(*ip + 1) == 0x05) {                       // mov     ds:<1>, <2>
                    REG32(&i) = REG32(*ip + 2);                     //      <1>: offset
                    if ((ret = xboxkrnl_write(REG32(&i), *ip + 6, sz))) {
                        *ip += 2;                                   //   opcode: 0xc7 0x05 <1:dword> <2:dword>
                        *ip += 4;                                   // <1> size: dword
                        *ip += 4;                                   // <2> size: dword
                    }
                    break;
                }
// mov     dword ptr [eax+2500h], 0 // C7 80 00 25 00 00 00 00 00 00
                if (REG08(*ip + 1) == 0x80) {                       // mov     [eax+<1>], <2>
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    if ((ret = xboxkrnl_write(REG32(&i), *ip + 6, sz))) {
                        *ip += 2;                                   //   opcode: 0xc7 0x80 <1:dword> <2:dword>
                        *ip += 4;                                   // <1> size: dword
                        *ip += 4;                                   // <2> size: dword
                    }
                    break;
                }
// mov     dword ptr [eax+edi*4], 0 // C7 04 B8 00 00 00 00
                if (REG08(*ip + 1) == 0x04) {
                    if (REG08(*ip + 2) == 0xb8) {                   // mov     [eax+edi*4], <1>
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_EDI]) * 4;
                        if ((ret = xboxkrnl_write(REG32(&i), *ip + 3, sz))) {
                            *ip += 3;                               //   opcode: 0xc7 0x04 0xb8 <1:dword>
                            *ip += 4;                               // <1> size: dword
                        }
                        break;
                    }
// mov     dword ptr [eax+esi*4], 3F800000h // C7 04 B0 00 00 80 3F
                    if (REG08(*ip + 2) == 0xb0) {                   // mov     [eax+esi*4], <1>
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_ESI]) * 4;
                        if ((ret = xboxkrnl_write(REG32(&i), *ip + 3, sz))) {
                            *ip += 3;                               //   opcode: 0xc7 0x04 0xb0 <1:dword>
                            *ip += 4;                               // <1> size: dword
                        }
                        break;
                    }
// mov     dword ptr [eax+ebx*4], 40000000h // C7 04 98 00 00 00 40
                    if (REG08(*ip + 2) == 0x98) {                   // mov     [eax+ebx*4], <1>
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_EBX]) * 4;
                        if ((ret = xboxkrnl_write(REG32(&i), *ip + 3, sz))) {
                            *ip += 3;                               //   opcode: 0xc7 0x04 0x98 <1:dword>
                            *ip += 4;                               // <1> size: dword
                        }
                        break;
                    }
// c7 04 11 00 00 00 00 	mov    DWORD PTR [ecx+edx*1],0x0
                    if (REG08(*ip + 2) == 0x11) {                   // mov     [ecx+edx], <1>
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_EDX]);
                        if ((ret = xboxkrnl_write(REG32(&i), *ip + 3, sz))) {
                            *ip += 3;                               //   opcode: 0xc7 0x04 0x11 <1:dword>
                            *ip += 4;                               // <1> size: dword
                        }
                        break;
                    }
// mov     dword ptr [ecx+edi], 0 // C7 04 39 00 00 00 00
                    if (REG08(*ip + 2) == 0x39) {                   // mov     [ecx+edi], <1>
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                        if ((ret = xboxkrnl_write(REG32(&i), *ip + 3, sz))) {
                            *ip += 3;                               //   opcode: 0xc7 0x04 0x39 <1:dword>
                            *ip += 4;                               // <1> size: dword
                        }
                        break;
                    }
                }
// mov     dword ptr [edi], 0   // C7 07 00 00 00 00
                if (REG08(*ip + 1) == 0x07) {                       // mov     [edi], <1>
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                    if ((ret = xboxkrnl_write(REG32(&i), *ip + 2, sz))) {
                        *ip += 2;                                   //   opcode: 0xc7 0x07 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     dword ptr [eax+ecx*4+33Ch], 0FFFF0000h // C7 84 88 3C 03 00 00 00 00 FF FF
                if (REG08(*ip + 1) == 0x84) {
                    if (REG08(*ip + 2) == 0x88) {                   // mov     [eax+ecx*4+<1>], <2>
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_ECX]) * 4;
                        REG32(&i) += REG32(*ip + 3);                //      <1>: offset
                        if ((ret = xboxkrnl_write(REG32(&i), *ip + 7, sz))) {
                            *ip += 3;                               //   opcode: 0xc7 0x84 0x88 <1:dword> <2:dword>
                            *ip += 4;                               // <1> size: dword
                            *ip += 4;                               // <2> size: dword
                        }
                        break;
                    }
// mov     dword ptr [eax+edi*4+700004h], 0BF800000h // C7 84 B8 04 00 70 00 00 00 80 BF
                    if (REG08(*ip + 2) == 0xb8) {                   // mov     [eax+edi*4+<1>], <2>
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_EDI]) * 4;
                        REG32(&i) += REG32(*ip + 3);                //      <1>: offset
                        if ((ret = xboxkrnl_write(REG32(&i), *ip + 7, sz))) {
                            *ip += 3;                               //   opcode: 0xc7 0x84 0xb8 <1:dword> <2:dword>
                            *ip += 4;                               // <1> size: dword
                            *ip += 4;                               // <2> size: dword
                        }
                        break;
                    }
                }
// mov     dword ptr [esi], 7FF0000h // C7 06 00 00 FF 07
                if (REG08(*ip + 1) == 0x06) {                       // mov     [esi], <1>
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    if ((ret = xboxkrnl_write(REG32(&i), *ip + 2, sz))) {
                        *ip += 2;                                   //   opcode: 0xc7 0x06 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     dword ptr [edi+1830h], 0     // C7 87 30 18 00 00 00 00 00 00
                if (REG08(*ip + 1) == 0x87) {                       // mov     [edi+<1>], <2>
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    if ((ret = xboxkrnl_write(REG32(&i), *ip + 6, sz))) {
                        *ip += 2;                                   //   opcode: 0xc7 0x87 <1:dword> <2:dword>
                        *ip += 4;                                   // <1> size: dword
                        *ip += 4;                                   // <2> size: dword
                    }
                    break;
                }
// mov     dword ptr [ecx], 0FFFFFFFFh // C7 01 FF FF FF FF
                if (REG08(*ip + 1) == 0x01) {                       // mov     [ecx], <1>
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    if ((ret = xboxkrnl_write(REG32(&i), *ip + 2, sz))) {
                        *ip += 2;                                   //   opcode: 0xc7 0x01 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     dword ptr [eax+18h], 0 // C7 40 18 00 00 00 00
                if (REG08(*ip + 1) == 0x40) {                       // mov     [eax+<1>], <2>
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    if ((ret = xboxkrnl_write(REG32(&i), *ip + 3, sz))) {
                        *ip += 2;                                   //   opcode: 0xc7 0x40 <1:byte> <2:dword>
                        *ip += 1;                                   // <1> size: byte
                        *ip += 4;                                   // <2> size: dword
                    }
                    break;
                }
// mov     dword ptr [ebx+10h], 40h // C7 43 10 40 00 00 00
                if (REG08(*ip + 1) == 0x43) {                       // mov     [ebx+<1>], <2>
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EBX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    if ((ret = xboxkrnl_write(REG32(&i), *ip + 3, sz))) {
                        *ip += 2;                                   //   opcode: 0xc7 0x43 <1:byte> <2:dword>
                        *ip += 1;                                   // <1> size: byte
                        *ip += 4;                                   // <2> size: dword
                    }
                    break;
                }
// mov     dword ptr [edi+48h], 1200h // C7 47 48 00 12 00 00
                if (REG08(*ip + 1) == 0x47) {                       // mov     [edi+<1>], <2>
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    if ((ret = xboxkrnl_write(REG32(&i), *ip + 3, sz))) {
                        *ip += 2;                                   //   opcode: 0xc7 0x47 <1:byte> <2:dword>
                        *ip += 1;                                   // <1> size: byte
                        *ip += 4;                                   // <2> size: dword
                    }
                    break;
                }
// mov     dword ptr [eax], 0 // C7 00 00 00 00 00
                if (REG08(*ip + 1) == 0x00) {                       // mov     [eax], <1>
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    if ((ret = xboxkrnl_write(REG32(&i), *ip + 2, sz))) {
                        *ip += 2;                                   //   opcode: 0xc7 0x00 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     dword ptr [esi+400750h], 0EA000Ch // C7 86 50 07 40 00 0C 00 EA 00
                if (REG08(*ip + 1) == 0x86) {                       // mov     [esi+<1>], <2>
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    if ((ret = xboxkrnl_write(REG32(&i), *ip + 6, sz))) {
                        *ip += 2;                                   //   opcode: 0xc7 0x86 <1:dword> <2:dword>
                        *ip += 4;                                   // <1> size: dword
                        *ip += 4;                                   // <2> size: dword
                    }
                    break;
                }
// mov     dword ptr [ecx+400750h], 0E00050h // C7 81 50 07 40 00 50 00 E0 00
                if (REG08(*ip + 1) == 0x81) {                       // mov     [ecx+<1>], <2>
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    if ((ret = xboxkrnl_write(REG32(&i), *ip + 6, sz))) {
                        *ip += 2;                                   //   opcode: 0xc7 0x81 <1:dword> <2:dword>
                        *ip += 4;                                   // <1> size: dword
                        *ip += 4;                                   // <2> size: dword
                    }
                    break;
                }
            }
            if (REG08(*ip) == 0x8b) {                               // mov
                if (REG08(*ip + 1) == 0x82) {                       // mov     eax, [edx+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x82 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
                if (REG08(*ip + 1) == 0x92) {                       // mov     edx, [edx+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x92 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     esi, [eax+2500h]     // 8B B0 00 25 00 00
                if (REG08(*ip + 1) == 0xb0) {                       // mov     esi, [eax+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_ESI];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0xb0 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     ecx, [esi]           // 8B 0E
                if (REG08(*ip + 1) == 0x0e) {                       // mov     ecx, [esi]
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x0e
                    }
                    break;
                }
// mov     ecx, [edx]           // 8B 0A
                if (REG08(*ip + 1) == 0x0a) {                       // mov     ecx, [edx]
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EDX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x0a
                    }
                    break;
                }
// mov     eax, [ecx] // 8B 01
                if (REG08(*ip + 1) == 0x01) {                       // mov     eax, [ecx]
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x01
                    }
                    break;
                }
// mov     eax, [edi+140h] // 8B 87 40 01 00 00
                if (REG08(*ip + 1) == 0x87) {                       // mov     eax, [edi+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x87 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     ecx, [eax] // 8B 08
                if (REG08(*ip + 1) == 0x08) {                       // mov     ecx, [eax]
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x08
                    }
                    break;
                }
// mov     eax, [eax] // 8B 00
                if (REG08(*ip + 1) == 0x00) {                       // mov     eax, [eax]
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x00
                    }
                    break;
                }
// mov     ecx, [eax+8088h] // 8B 88 88 80 00 00
                if (REG08(*ip + 1) == 0x88) {                       // mov     ecx, [eax+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x88 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     edx, [ecx] // 8B 11
                if (REG08(*ip + 1) == 0x11) {                       // mov     edx, [ecx]
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x11
                    }
                    break;
                }
// mov     eax, [esi+400700h] // 8B 86 00 07 40 00
                if (REG08(*ip + 1) == 0x86) {                       // mov     eax, [esi+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x86 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     ebp, [eax-3006C0h] // 8B A8 40 F9 CF FF
                if (REG08(*ip + 1) == 0xa8) {                       // mov     ebp, [eax+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EBP];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0xa8 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     edx, [eax-300680h] // 8B 90 80 F9 CF FF
                if (REG08(*ip + 1) == 0x90) {                       // mov     edx, [eax+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x90 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     edi, [eax+3240h] // 8B B8 40 32 00 00
                if (REG08(*ip + 1) == 0xb8) {                       // mov     edi, [eax+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDI];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0xb8 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     edi, [eax] // 8B 38
                if (REG08(*ip + 1) == 0x38) {                       // mov     edi, [eax]
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EDI];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x38
                    }
                    break;
                }
// mov     edi, [edx] // 8B 3A
                if (REG08(*ip + 1) == 0x3a) {                       // mov     edi, [edx]
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EDX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EDI];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x3a
                    }
                    break;
                }
// mov     edi, [edx+4] // 8B 7A 04
                if (REG08(*ip + 1) == 0x7a) {                       // mov     edi, [edx+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDI];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x7a <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     edx, [edx+30h] // 8B 52 30
                if (REG08(*ip + 1) == 0x52) {                       // mov     edx, [edx+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x52 <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     eax, [edi+8] // 8B 47 08
                if (REG08(*ip + 1) == 0x47) {                       // mov     eax, [edi+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x47 <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     ecx, [eax+4] // 8B 48 04
                if (REG08(*ip + 1) == 0x48) {                       // mov     ecx, [eax+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x48 <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     ecx, [ecx+44h] // 8B 49 44
                if (REG08(*ip + 1) == 0x49) {                       // mov     ecx, [ecx+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x49 <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     edx, [esi+2100h] // 8B 96 00 21 00 00
                if (REG08(*ip + 1) == 0x96) {                       // mov     edx, [esi+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x96 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     ecx, [esi+2100h] // 8B 8E 00 21 00 00
                if (REG08(*ip + 1) == 0x8e) {                       // mov     ecx, [esi+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x8e <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     edi, [esi+400704h] // 8B BE 04 07 40 00
                if (REG08(*ip + 1) == 0xbe) {                       // mov     edi, [esi+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDI];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0xbe <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     ebx, [esi+400108h] // 8B 9E 08 01 40 00
                if (REG08(*ip + 1) == 0x9e) {                       // mov     ebx, [esi+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EBX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x9e <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     ebp, [edx+400700h] // 8B AA 00 07 40 00
                if (REG08(*ip + 1) == 0xaa) {                       // mov     ebp, [edx+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EBP];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0xaa <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     eax, [ecx+100248h] // 8B 81 48 02 10 00
                if (REG08(*ip + 1) == 0x81) {                       // mov     eax, [ecx+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x81 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     eax, [eax+400904h] // 8B 80 04 09 40 00
                if (REG08(*ip + 1) == 0x80) {                       // mov     eax, [eax+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x80 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     eax, [ebx] // 8B 03
                if (REG08(*ip + 1) == 0x03) {                       // mov     eax, [ebx]
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EBX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x03
                    }
                    break;
                }
// mov     eax, [eax+44h] // 8B 40 44
                if (REG08(*ip + 1) == 0x40) {                       // mov     eax, [eax+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x40 <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     eax, [ebx+50h] // 8B 43 50
                if (REG08(*ip + 1) == 0x43) {                       // mov     eax, [ebx+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EBX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x43 <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     esi, [edi+100h] // 8B B7 00 01 00 00
                if (REG08(*ip + 1) == 0xb7) {                       // mov     esi, [edi+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_ESI];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0xb7 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     edx, ds:0FD003244h // 8B 15 44 32 00 FD
                if (REG08(*ip + 1) == 0x15) {                       // mov     edx, ds:<1>
                    REG32(&i) = REG32(*ip + 2);                     //      <1>: offset
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x15 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     ebx, ds:0FE820010h // 8B 1D 10 00 82 FE
                if (REG08(*ip + 1) == 0x1d) {                       // mov     ebx, ds:<1>
                    REG32(&i) = REG32(*ip + 2);                     //      <1>: offset
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EBX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x1d <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     eax, [ecx+401A88h] // 8B 81 88 1A 40 00
                if (REG08(*ip + 1) == 0x81) {                       // mov     eax, [ecx+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x81 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     edx, [ecx+40186Ch] // 8B 91 6C 18 40 00
                if (REG08(*ip + 1) == 0x91) {                       // mov     edx, [ecx+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x91 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     ecx, ds:0FD0032A4h // 8B 0D A4 32 00 FD
                if (REG08(*ip + 1) == 0x0d) {                       // mov     ecx, ds:<1>
                    REG32(&i) = REG32(*ip + 2);                     //      <1>: offset
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8b 0x0d <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
            }
// mov     [ecx+700008h], eax   // 89 81 08 00 70 00
            if (REG08(*ip) == 0x89) {                               // mov
                if (REG08(*ip + 1) == 0x81) {                       // mov     [ecx+<1>], eax
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x81 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [eax+edi], esi       // 89 34 38
                if (REG08(*ip + 1) == 0x34) {
                    if (REG08(*ip + 2) == 0x38) {                   // mov     [eax+edi], esi
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_ESI];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0x34 0x38
                        }
                        break;
                    }
// mov     [eax+edi*4], esi     // 89 34 B8
                    if (REG08(*ip + 2) == 0xb8) {                   // mov     [eax+edi*4], esi
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_EDI]) * 4;
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_ESI];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0x34 0xb8
                        }
                        break;
                    }
                }
// mov     ds:0FE83FF00h, esi // 89 35 00 FF 83 FE
                if (REG08(*ip + 1) == 0x35) {                       // mov     ds:<1>, esi
                    REG32(&i) = REG32(*ip + 2);                     //      <1>: offset
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_ESI];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x35 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     ds:0FE803028h, edi // 89 3D 28 30 80 FE
                if (REG08(*ip + 1) == 0x3d) {                       // mov     ds:<1>, edi
                    REG32(&i) = REG32(*ip + 2);                     //      <1>: offset
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EDI];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x3d <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [esi+0Ch], edi       // 89 7E 0C
                if (REG08(*ip + 1) == 0x7e) {                       // mov     [esi+<1>], edi
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDI];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x7e <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     [esi+14h], ebx       // 89 5E 14
                if (REG08(*ip + 1) == 0x5e) {                       // mov     [esi+<1>], ebx
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EBX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x5e <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     [eax+2504h], esi     // 89 B0 04 25 00 00
                if (REG08(*ip + 1) == 0xb0) {                       // mov     [eax+<1>], esi
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_ESI];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0xb0 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
                if (REG08(*ip + 1) == 0x0c) {
// mov     ds:0FEC00000h[edx*2], cx // 66 89 0C 55 00 00 C0 FE
                    if (REG08(*ip + 2) == 0x55) {                   // mov     ds:<1>[edx*2], ecx
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDX]) * 2;
                        REG32(&i) += REG32(*ip + 3);                //      <1>: offset
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0x0c 0x55 <1:dword>
                            *ip += 4;                               // <1> size: dword
                        }
                        break;
                    }
// mov     ds:0FE820200h[eax*4], ecx // 89 0C 85 00 02 82 FE
                    if (REG08(*ip + 2) == 0x85) {                   // mov     ds:<1>[eax*4], ecx
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]) * 4;
                        REG32(&i) += REG32(*ip + 3);                //      <1>: offset
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0x0c 0x85 <1:dword>
                            *ip += 4;                               // <1> size: dword
                        }
                        break;
                    }
                }
// mov     ds:0FE801004h, ecx // 89 0D 04 10 80 FE
                if (REG08(*ip + 1) == 0x0d) {                       // mov     ds:<1>, ecx
                    REG32(&i) = REG32(*ip + 2);                     //      <1>: offset
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x0d <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [esi], ecx           // 89 0E
                if (REG08(*ip + 1) == 0x0e) {                       // mov     [esi], ecx
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x0e
                    }
                    break;
                }
// mov     [edi], ecx // 89 0F
                if (REG08(*ip + 1) == 0x0f) {                       // mov     [edi], ecx
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x0f
                    }
                    break;
                }
// mov     [eax+3240h], ecx     // 89 88 40 32 00 00
                if (REG08(*ip + 1) == 0x88) {                       // mov     [eax+<1>], ecx
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x88 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [eax+3224h], ebx     // 89 98 24 32 00 00
                if (REG08(*ip + 1) == 0x98) {                       // mov     [eax+<1>], ebx
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EBX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x98 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [eax], ecx           // 89 08
                if (REG08(*ip + 1) == 0x08) {                       // mov     [eax], ecx
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x08
                    }
                    break;
                }
// mov     [ecx+18h], eax // 89 41 18
                if (REG08(*ip + 1) == 0x41) {                       // mov     [ecx+<1>], eax
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x41 <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// 89 42 e8             	mov    DWORD PTR [edx-0x18],eax
                if (REG08(*ip + 1) == 0x42) {                       // mov     [edx+<1>], eax
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x42 <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     [ebx+50h], eax // 89 43 50
                if (REG08(*ip + 1) == 0x43) {                       // mov     [ebx+<1>], eax
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EBX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x43 <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     [edi+8], eax // 89 47 08
                if (REG08(*ip + 1) == 0x47) {                       // mov     [edi+<1>], eax
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x47 <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     [eax+4], ecx         // 89 48 04
                if (REG08(*ip + 1) == 0x48) {                       // mov     [eax+<1>], ecx
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x48 <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     [edi+34h], ecx // 89 4F 34
                if (REG08(*ip + 1) == 0x4f) {                       // mov     [edi+<1>], ecx
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x4f <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     [edx], ecx           // 89 0A
                if (REG08(*ip + 1) == 0x0a) {                       // mov     [edx], ecx
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EDX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x0a
                    }
                    break;
                }
// mov     [eax+ecx*4+340h], edx    // 89 94 88 40 03 00 00
                if (REG08(*ip + 1) == 0x94) {
                    if (REG08(*ip + 2) == 0x88) {                   // mov     [eax+ecx*4+<1>], edx
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_ECX]) * 4;
                        REG32(&i) += REG32(*ip + 3);                //      <1>: offset
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0x94 0x88 <1:dword>
                            *ip += 4;                               // <1> size: dword
                        }
                        break;
                    }
// mov     [eax+edi*4+700004h], edx // 89 94 B8 04 00 70 00
                    if (REG08(*ip + 2) == 0xb8) {                   // mov     [eax+edi*4+<1>], edx
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_EDI]) * 4;
                        REG32(&i) += REG32(*ip + 3);                //      <1>: offset
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0x94 0xb8 <1:dword>
                            *ip += 4;                               // <1> size: dword
                        }
                        break;
                    }
                }
// mov     [eax+ecx*4+3A0h], esi    // 89 B4 88 A0 03 00 00
                if (REG08(*ip + 1) == 0xb4) {
                    if (REG08(*ip + 2) == 0x88) {                   // mov     [eax+ecx*4+<1>], esi
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_ECX]) * 4;
                        REG32(&i) += REG32(*ip + 3);                //      <1>: offset
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_ESI];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0xb4 0x88 <1:dword>
                            *ip += 4;                               // <1> size: dword
                        }
                        break;
                    }
                }
// mov     [eax+ecx*4+5FCh], ebx // 89 9C 88 FC 05 00 00
                if (REG08(*ip + 1) == 0x9c) {
                    if (REG08(*ip + 2) == 0x88) {                   // mov     [eax+ecx*4+<1>], ebx
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_ECX]) * 4;
                        REG32(&i) += REG32(*ip + 3);                //      <1>: offset
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_EBX];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0x9c 0x88 <1:dword>
                            *ip += 4;                               // <1> size: dword
                        }
                        break;
                    }
                }
// mov     ds:80000000h, edx // 89 15 00 00 00 80
                if (REG08(*ip + 1) == 0x15) {                       // mov     ds:<1>, edx
                    REG32(&i) = REG32(*ip + 2);                     //      <1>: offset
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x15
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [esi], edx // 89 16
                if (REG08(*ip + 1) == 0x16) {                       // mov     [esi], edx
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x16
                    }
                    break;
                }
// mov     [ebx], edx // 89 13
                if (REG08(*ip + 1) == 0x13) {                       // mov     [ebx], edx
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EBX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x13
                    }
                    break;
                }
// mov     [edi], edx // 89 17
                if (REG08(*ip + 1) == 0x17) {                       // mov     [edi], edx
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x17
                    }
                    break;
                }
// mov     [eax+esi*4], edx // 89 14 B0
                if (REG08(*ip + 1) == 0x14) {
                    if (REG08(*ip + 2) == 0xb0) {                   // mov     [eax+esi*4], edx
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_ESI]) * 4;
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0x14 0xb0
                        }
                        break;
                    }
// mov     [eax+ebx*4], edx // 89 14 98
                    if (REG08(*ip + 2) == 0x98) {                   // mov     [eax+ebx*4], edx
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_EBX]) * 4;
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0x14 0x98
                        }
                        break;
                    }
// mov     [eax+ecx*4], edx // 89 14 88
                    if (REG08(*ip + 2) == 0x88) {                   // mov     [eax+ecx*4], edx
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_ECX]) * 4;
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0x14 0x88
                        }
                        break;
                    }
// mov     [eax+ecx], edx // 89 14 08
                    if (REG08(*ip + 2) == 0x08) {                   // mov     [eax+ecx], edx
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0x14 0x08
                        }
                        break;
                    }
                }
// mov     [eax+edx*4], ecx // 89 0C 90
                if (REG08(*ip + 1) == 0x0c) {
                    if (REG08(*ip + 2) == 0x90) {                   // mov     [eax+edx*4], ecx
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_EDX]) * 4;
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0x0c 0x90
                        }
                        break;
                    }
// mov     [eax+esi*4], ecx // 89 0C B0
                    if (REG08(*ip + 2) == 0xb0) {                   // mov     [eax+esi*4], ecx
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_ESI]) * 4;
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0x0c 0xb0
                        }
                        break;
                    }
// mov     [edi+edx], ecx // 89 0C 17
                    if (REG08(*ip + 2) == 0x17) {                   // mov     [edi+edx], ecx
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_EDX]);
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0x0c 0x17
                        }
                        break;
                    }
                }
// mov     [eax+edx*4+700014h], ecx // 89 8C 90 14 00 70 00
                if (REG08(*ip + 1) == 0x8c) {
                    if (REG08(*ip + 2) == 0x90) {                   // mov     [eax+edx*4+<1>], ecx
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_EDX]) * 4;
                        REG32(&i) += REG32(*ip + 3);                //      <1>: offset
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0x8c 0x90 <1:dword>
                            *ip += 4;                               // <1> size: dword
                        }
                        break;
                    }
                }
// mov     [ecx], eax // 89 01
                if (REG08(*ip + 1) == 0x01) {                       // mov     [ecx], eax
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x01
                    }
                    break;
                }
// mov     [edi+2210h], ecx // 89 8F 10 22 00 00
                if (REG08(*ip + 1) == 0x8f) {                       // mov     [edi+<1>], ecx
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x8f <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [edi+2214h], edx // 89 97 14 22 00 00
                if (REG08(*ip + 1) == 0x97) {                       // mov     [edi+<1>], edx
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x97 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [ecx], edx // 89 11
                if (REG08(*ip + 1) == 0x11) {                       // mov     [ecx], edx
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x11
                    }
                    break;
                }
// mov     [eax-8], edx // 89 50 F8
                if (REG08(*ip + 1) == 0x50) {                       // mov     [eax+<1>], edx
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x50 <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     [ecx+4], edx // 89 51 04
                if (REG08(*ip + 1) == 0x51) {                       // mov     [ecx+<1>], edx
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x51 <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     [esi+2], dx // 66 89 56 02
                if (REG08(*ip + 1) == 0x56) {                       // mov     [esi+<1>], edx
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x56 <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     [eax], edx // 89 10
                if (REG08(*ip + 1) == 0x10) {                       // mov     [eax], edx
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x10
                    }
                    break;
                }
// mov     [edx+400780h], eax // 89 82 80 07 40 00
                if (REG08(*ip + 1) == 0x82) {                       // mov     [edx+<1>], eax
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x82 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [eax+3210h], edx // 89 90 10 32 00 00
                if (REG08(*ip + 1) == 0x90) {                       // mov     [eax+<1>], edx
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x90 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [esi+200h], ebx // 89 9E 00 02 00 00
                if (REG08(*ip + 1) == 0x9e) {                       // mov     [esi+<1>], ebx
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EBX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x9e <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [esi+140h], eax // 89 86 40 01 00 00
                if (REG08(*ip + 1) == 0x86) {                       // mov     [esi+<1>], eax
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x86 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [esi+9400h], ebp // 89 AE 00 94 00 00
                if (REG08(*ip + 1) == 0xae) {                       // mov     [esi+<1>], ebp
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EBP];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0xae <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [eax], ebp // 89 28
                if (REG08(*ip + 1) == 0x28) {                       // mov     [eax], ebp
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EBP];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x28
                    }
                    break;
                }
// mov     [eax+4], ebp // 89 68 04
                if (REG08(*ip + 1) == 0x68) {                       // mov     [eax+<1>], ebp
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EBP];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x68 <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     [esi+400754h], edx // 89 96 54 07 40 00
                if (REG08(*ip + 1) == 0x96) {                       // mov     [esi+<1>], edx
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x96 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [esi+400750h], ecx // 89 8E 50 07 40 00
                if (REG08(*ip + 1) == 0x8e) {                       // mov     [esi+<1>], ecx
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x8e <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [edi], ebp // 89 2F
                if (REG08(*ip + 1) == 0x2f) {                       // mov     [edi], ebp
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EBP];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x2f
                    }
                    break;
                }
// mov     [eax], edi // 89 38
                if (REG08(*ip + 1) == 0x38) {                       // mov     [eax], edi
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EDI];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x38
                    }
                    break;
                }
// mov     [edx], edi // 89 3A
                if (REG08(*ip + 1) == 0x3a) {                       // mov     [edx], edi
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EDX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EDI];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x3a
                    }
                    break;
                }
// mov     [ecx+4], edi // 89 79 04
                if (REG08(*ip + 1) == 0x79) {                       // mov     [ecx+<1>], edi
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDI];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x79 <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     [edx+4], edi // 89 7A 04
                if (REG08(*ip + 1) == 0x7a) {                       // mov     [edx+<1>], edi
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EDX]);
                    REG08(&i) += REG08(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDI];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x7a <1:byte>
                        *ip += 1;                                   // <1> size: byte
                    }
                    break;
                }
// mov     [eax+3240h], edi // 89 B8 40 32 00 00
                if (REG08(*ip + 1) == 0xb8) {                       // mov     [eax+<1>], edi
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EDI];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0xb8 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     ds:0FE85FF00h, ebx // 89 1D 00 FF 85 FE
                if (REG08(*ip + 1) == 0x1d) {                       // mov     ds:<1>, ebx
                    REG32(&i) = REG32(*ip + 2);                     //      <1>: offset
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EBX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x1d
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [edi], ebx // 89 1F
                if (REG08(*ip + 1) == 0x1f) {                       // mov     [edi], ebx
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EDI]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EBX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x1f
                    }
                    break;
                }
// mov     [edx], eax // 89 02
                if (REG08(*ip + 1) == 0x02) {                       // mov     [edx], eax
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EDX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x02
                    }
                    break;
                }
// mov     [ebx], eax // 89 03
                if (REG08(*ip + 1) == 0x03) {                       // mov     [ebx], eax
                    REG32(&i) = REG32(&uc->uc_mcontext.gregs[X86_EBX]);
                    bp        = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x89 0x03
                    }
                    break;
                }
// mov     [esi+edi*4+400980h], eax // 89 84 BE 80 09 40 00
                if (REG08(*ip + 1) == 0x84) {
                    if (REG08(*ip + 2) == 0xbe) {                   // mov     [esi+edi*4+<1>], eax
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ESI]);
                        REG32(&i) += REG32(&uc->uc_mcontext.gregs[X86_EDI]) * 4;
                        REG32(&i) += REG32(*ip + 3);                //      <1>: offset
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_EAX];
                        if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x89 0x84 0xbe <1:dword>
                            *ip += 4;                               // <1> size: dword
                        }
                        break;
                    }
                }
            }
// movzx   ecx, byte ptr [eax+1] // 0F B6 48 01
            if (REG08(*ip) == 0x0f) {
                if (REG08(*ip + 1) == 0xb6) {
                    sz = 1;
                    if (REG08(*ip + 2) == 0x48) {                   // movzx   ecx, [eax+<1>]
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG08(&i) += REG08(*ip + 3);                //      <1>: offset
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                        REG32(bp) = 0;
                        if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x0f 0xb6 0x48 <1:byte>
                            *ip += 1;                               // <1> size: byte
                        }
                        break;
                    }
// movzx   edi, byte ptr [eax+1] // 0F B6 78 01
                    if (REG08(*ip + 2) == 0x78) {                   // movzx   edi, [eax+<1>]
                        REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                        REG08(&i) += REG08(*ip + 3);                //      <1>: offset
                        bp         = (void **)&uc->uc_mcontext.gregs[X86_EDI];
                        REG32(bp) = 0;
                        if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                            *ip += 3;                               //   opcode: 0x0f 0xb6 0x78 <1:byte>
                            *ip += 1;                               // <1> size: byte
                        }
                        break;
                    }
                }
            }
// mov     byte ptr [eax+6013D4h], 1Fh // C6 80 D4 13 60 00 1F
            if (REG08(*ip) == 0xc6) {
                sz = 1;
                if (REG08(*ip + 1) == 0x80) {                       // mov     [eax+<1>], <2>
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    if ((ret = xboxkrnl_write(REG32(&i), *ip + 6, sz))) {
                        *ip += 2;                                   //   opcode: 0xc6 0x80 <1:dword> <2:byte>
                        *ip += 4;                                   // <1> size: dword
                        *ip += 1;                                   // <2> size: byte
                    }
                    break;
                }
// mov     byte ptr [ecx-13FFEF5h], 2 // C6 81 0B 01 C0 FE 02
                if (REG08(*ip + 1) == 0x81) {                       // mov     [ecx+<1>], <2>
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_ECX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    if ((ret = xboxkrnl_write(REG32(&i), *ip + 6, sz))) {
                        *ip += 2;                                   //   opcode: 0xc6 0x81 <1:dword> <2:byte>
                        *ip += 4;                                   // <1> size: dword
                        *ip += 1;                                   // <2> size: byte
                    }
                    break;
                }
            }
// mov     bl, [eax+6013D5h] // 8A 98 D5 13 60 00
            if (REG08(*ip) == 0x8a) {
                sz = 1;
                if (REG08(*ip + 1) == 0x98) {                       // mov     bl, [eax+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EBX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8a 0x98 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     cl, [eax+6013D5h] // 8A 88 D5 13 60 00
                if (REG08(*ip + 1) == 0x88) {                       // mov     cl, [eax+<1>]
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_read(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x8a 0x88 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
            }
// mov     [eax+6013D5h], cl // 88 88 D5 13 60 00
            if (REG08(*ip) == 0x88) {
                sz = 1;
                if (REG08(*ip + 1) == 0x88) {                       // mov     [eax+<1>], cl
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_ECX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x88 0x88 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
// mov     [eax-13FFEFCh], bl // 88 98 04 01 C0 FE
                if (REG08(*ip + 1) == 0x98) {                       // mov     [eax+<1>], bl
                    REG32(&i)  = REG32(&uc->uc_mcontext.gregs[X86_EAX]);
                    REG32(&i) += REG32(*ip + 2);                    //      <1>: offset
                    bp         = (void **)&uc->uc_mcontext.gregs[X86_EBX];
                    if ((ret = xboxkrnl_write(REG32(&i), bp, sz))) {
                        *ip += 2;                                   //   opcode: 0x88 0x98 <1:dword>
                        *ip += 4;                                   // <1> size: dword
                    }
                    break;
                }
            }
        } while (0);
        if (ret) break;
        if (si_code) {
            if (prefix) *ip -= 1;
            break;
        }
        /* fall through */
    case 128:
        do {
            if (REG08(*ip) == 0x0f) {
                if (REG08(*ip + 1) == 0x31) {                       // rdtsc
#if 0 //XXX rdtsc emulation
uint64_t t1,t2;
uint32_t lo,hi;
xboxkrnl_tsc(&lo, &hi, &t1, 0);
PRINT(XEXEC_DBG_ALL,"tsc: host: %llu", t1);
xboxkrnl_tsc(&lo, &hi, &t1, 1);
PRINT(XEXEC_DBG_ALL,"tsc: xbox: %llu", t1);
//PRINT(XEXEC_DBG_ALL,"HIT! lo 0x%.08x | hi 0x%.08x | 0x%.016llx", lo, hi, t1);//XXX
int t;
for (t=0;t<10;++t){
sleep(1);
xboxkrnl_tsc(&lo, &hi, &t2, 1);
PRINT(XEXEC_DBG_ALL,"tsc: xbox: %llu (%llu)", t2 - t1,t2);
t1 = t2;
}
_exit(0);//XXX
#endif
                    xboxkrnl_tsc(
                        (void *)&uc->uc_mcontext.gregs[X86_EAX],
                        (void *)&uc->uc_mcontext.gregs[X86_EDX],
                        NULL,
                        1);
                    *ip += 2;                                       //   opcode: 0x0f 0x31
                    ret = 1;
                    break;
                }
            }
            if (REG08(*ip) == 0xee) {                               // out     dx, al
                REG08(&i) = REG08(&uc->uc_mcontext.gregs[X86_EAX]);
                bp        = (void **)&uc->uc_mcontext.gregs[X86_EDX];
                PRINT(
                    XEXEC_DBG_REG,
                    "out: "
                    "[0x%.04hx], "
                    "al (0x%.02hhx)",
                    REG16(bp),                                      // dx
                    REG08(&i));                                     // al
                //TODO
                *ip += 1;                                           //   opcode: 0xee
                ret = 1;
                break;
            }
        } while (0);
        if (ret) break;
        if (prefix) *ip -= 1;
        do {
#define EIP_PATCH(x,y,z) \
            if (!memcmp((x), (y), sizeof((y)) - 1)) { \
                memcpy((x), (z), sizeof((z)) - 1); \
                ret = 1; \
                break; \
            }

            /* patches sorted by length in descending order */

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

            EIP_PATCH(*ip,                              // __getptd()
                "\x64\xa1\x28\x00\x00\x00"              // 1: mov     eax, large fs:28h
                "\x83\x78\x28\x00",                     // 2: cmp     dword ptr [eax+28h], 0
                                                        // --
                "\x31\xc0\x90\x90\x90\x90"              // 1: xor     eax, eax
                "\x3c\x00\x90\x90");                    // 2: cmp     al, 0

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
                "\x64\xa1\x58\x00\x00\x00",             // 1: mov     eax, large fs:58h
                                                        // --
                "\x31\xc0\x90\x90\x90\x90");            // 1: xor     eax, eax

            EIP_PATCH(*ip,
                "\x0f\x09",                             // 1: wbinvd (Write Back and Invalidate Cache)
                                                        // --
                "\x90\x90");                            // 1: nop
#undef EIP_PATCH
        } while (0);
        break;
    }

    return ret;
}

static const char *const siginfo_si_code_name[] = {
    NULL,
    "SEGV_MAPERR",      /* Address not mapped to object. */
    "SEGV_ACCERR",      /* Invalid permissions for mapped object. */
    "SEGV_BNDERR",      /* (since Linux 3.19) Failed address bound checks. */
    "SEGV_PKUERR",      /* (since Linux 4.6) Access was denied by memory protection keys. */
};

static void
x86_signal_segv(int signum, siginfo_t *info, void *ptr) {
    register ucontext_t *uc = ptr;
    register void **bp;
    register void **ip = (void **)&uc->uc_mcontext.gregs[X86_EIP];
    register uint32_t i = (typeof(i))info->si_addr;

    xboxkrnl_interrupted = 1;

//if (i != 0x8001030) //XXX
//if (0) //XXX
    switch (info->si_code) {
    case SEGV_MAPERR /* 1 */:
    case SEGV_ACCERR /* 2 */:
        if (!xboxkrnl_address_validate(i)) break;
        /* fall through */
    case 128:
        if (x86_iterate(uc, info->si_code)) {
            xboxkrnl_interrupted = 0;
            return;
        }
        break;
    }

    xexec_debug = XEXEC_DBG_ALL;

    ENTER;
    PRINT(XEXEC_DBG_ALL, "/* Segmentation Fault! */", 0);

    VARDUMPN(DUMP,  "si_signo", signum);
    VARDUMPN(DUMP,  "si_errno", info->si_errno);
    if (info->si_errno) STRDUMP(strerror(info->si_errno));
    VARDUMPN2(DUMP, "si_code",  info->si_code, siginfo_si_code_name);
    VARDUMPN(DUMP,  "si_addr",  info->si_addr);

    PRINT(XEXEC_DBG_ALL, "/* x86 register dump */", 0);

    for (i = 0; i < ARRAY_SIZE(x86_greg_name); ++i) {
        if (i == X86_EFL) VARDUMPN3(DUMP, x86_greg_name[i], uc->uc_mcontext.gregs[i], x86_eflags_name);
        else VARDUMPN(DUMP, x86_greg_name[i], uc->uc_mcontext.gregs[i]);
    }

    HEXDUMPN(*ip, 15); /* x86/x86-64 maximum instruction length */

    PRINT(XEXEC_DBG_ALL, "/* stack trace */", 0);

    for (ip = (void **)&uc->uc_mcontext.gregs[X86_EIP],
        bp = (void **)uc->uc_mcontext.gregs[X86_EBP],
        i = 1;
        *ip;
        ip = (void **)&bp[1],
        bp = (void **)bp[0],
        ++i) {
        PRINT(XEXEC_DBG_ALL, "%.02i: bp = 0x%.08x, ip = 0x%.08x%s", i, bp, *ip, (i == 1) ? " <- SEGV" : "");
        if (!bp) break;
    }

    LEAVE;
    _exit(-1);
}

