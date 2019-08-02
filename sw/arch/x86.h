/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  x86 direct execution & CPU emulation (ucontext.h x86 ABI coroutine backend hook)
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

#ifndef X86_H
#define X86_H 1

#include <stdint.h>
#ifdef __i386__ /* i686-only direct execution */
# include <signal.h>
# include <ucontext.h>
#else
# include <bits/types.h>
# include <bits/types/sigset_t.h>
# include <bits/types/stack_t.h>
#endif

/* same as in ucontext.h x86 ABI coroutine backend */
enum {
    X86_GS       =  0,
    X86_FS,     /*  1 */
    X86_ES,     /*  2 */
    X86_DS,     /*  3 */
    X86_EDI,    /*  4 */
    X86_ESI,    /*  5 */
    X86_EBP,    /*  6 */
    X86_ESP,    /*  7 */
    X86_EBX,    /*  8 */
    X86_EDX,    /*  9 */
    X86_ECX,    /* 10 */
    X86_EAX,    /* 11 */
    X86_TRAPNO, /* 12 */
    X86_ERR,    /* 13 */
    X86_EIP,    /* 14 */
    X86_CS,     /* 15 */
    X86_EFL,    /* 16 */
    X86_UESP,   /* 17 */
    X86_SS,     /* 18 */
    X86_MAX     /* 19 */
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

typedef int32_t     x86_greg_t;
typedef x86_greg_t  x86_gregset_t[X86_MAX];

typedef union {
    uint32_t     field;
    struct {
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
} x86_greg_flags_t;

static const char *const x86_greg_flags_name[] = {
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

typedef struct {
    uint16_t        significand[4];
    uint16_t        exponent;
} x86_fpreg_t;

typedef struct {
    uint32_t        cw;
    uint32_t        sw;
    uint32_t        tag;
    uint32_t        ipoff;
    uint32_t        cssel;
    uint32_t        dataoff;
    uint32_t        datasel;
    x86_fpreg_t     st[8];
    uint32_t        status;
} x86_fpstate_t;

typedef x86_fpstate_t *x86_fpregset_t;

typedef struct {
    x86_gregset_t   gregs;
    x86_fpregset_t  fpregs;
    uint32_t        oldmask;
    uint32_t        cr2;
} x86_mcontext_t;

typedef struct x86_ucontext_t x86_ucontext_t;

struct x86_ucontext_t {
    uint32_t        uc_flags;
    x86_ucontext_t *uc_link;
    stack_t         uc_stack;
    x86_mcontext_t  uc_mcontext;
    sigset_t        uc_sigmask;
    x86_fpstate_t   fpregs_mem;
};

typedef struct {
    x86_greg_t       eip;  /* esp +  0 */
    void *           func; /* esp +  4 */
    void *           arg;  /* esp +  8 */
    x86_greg_t       edi;  /* esp + 12 */
    x86_greg_t       esi;  /* esp + 16 */
    x86_greg_t       ebp;  /* esp + 20 */
    x86_greg_t       esp;  /* esp + 24 */
    x86_greg_t       ebx;  /* esp + 28 */
    x86_greg_t       edx;  /* esp + 32 */
    x86_greg_t       ecx;  /* esp + 36 */
    x86_greg_t       eax;  /* esp + 40 */
    x86_greg_flags_t efl;  /* esp + 44 */
} PACKED x86_trampoline_gregset_t;

#ifdef __i386__ /* i686-only direct execution */
static const char *const siginfo_si_signo_name[] = {
    NAME(SIGHUP),       /*  1 - hangup */
    NAME(SIGINT),       /*  2 - interactive attention signal */
    NAME(SIGQUIT),      /*  3 - quit */
    NAME(SIGILL),       /*  4 - illegal instruction */
    NAME(SIGTRAP),      /*  5 - trace/breakpoint trap */
    NAME(SIGABRT),      /*  6 - abnormal termination */
    NAME(SIGFPE),       /*  8 - erroneous arithmetic operation */
    NAME(SIGKILL),      /*  9 - killed */
    NAME(SIGBUS),       /* 10 - bus error */
    NAME(SIGSEGV),      /* 11 - invalid access to storage */
    NAME(SIGSYS),       /* 12 - bad system call */
    NAME(SIGPIPE),      /* 13 - broken pipe */
    NAME(SIGALRM),      /* 14 - alarm clock */
    NAME(SIGTERM),      /* 15 - termination request */
    NAME(SIGURG),       /* 16 - urgent data is available at a socket */
    NAME(SIGSTOP),      /* 17 - stop, unblockable */
    NAME(SIGTSTP),      /* 18 - keyboard stop */
    NAME(SIGCONT),      /* 19 - continue */
    NAME(SIGCHLD),      /* 20 - child terminated or stopped */
    NAME(SIGTTIN),      /* 21 - background read from control terminal */
    NAME(SIGTTOU),      /* 22 - background write to control terminal */
    NAME(SIGPOLL),      /* 23 - pollable event occurred */
    NAME(SIGXCPU),      /* 24 - CPU time limit exceeded */
    NAME(SIGXFSZ),      /* 25 - file size limit exceeded */
    NAME(SIGVTALRM),    /* 26 - virtual timer expired */
    NAME(SIGPROF),      /* 27 - profiling timer expired */
    NAME(SIGWINCH),     /* 28 - window size change */
    NAME(SIGUSR1),      /* 30 - user-defined signal 1 */
    NAME(SIGUSR2),      /* 31 - user-defined signal 2 */
};

static const char *const siginfo_si_code_name[] = {
    NULL,
    "SEGV_MAPERR",      /* Address not mapped to object. */
    "SEGV_ACCERR",      /* Invalid permissions for mapped object. */
    "SEGV_BNDERR",      /* (since Linux 3.19) Failed address bound checks. */
    "SEGV_PKUERR",      /* (since Linux 4.6) Access was denied by memory protection keys. */
};
#endif

void x86_ucontext_dump(x86_ucontext_t *uc);
void x86_ucontext_reset(x86_ucontext_t *uc);
int  x86_ucontext_iterate(x86_ucontext_t *uc);

void x86_push(x86_ucontext_t *uc, uint32_t sz, void *val);
void x86_pop(x86_ucontext_t *uc, uint32_t sz, void *val);
void x86_pusha(x86_ucontext_t *uc, uint32_t sz);
void x86_popa(x86_ucontext_t *uc, uint32_t sz);
void x86_pushf(x86_ucontext_t *uc, uint32_t sz);
void x86_popf(x86_ucontext_t *uc, uint32_t sz);

void x86_call_near(x86_ucontext_t *uc, int prefix, void *offset);
void x86_call_func(x86_ucontext_t *uc, void **func);

void x86_trampoline_prologue(
        x86_ucontext_t *uc,
        int (*func)(x86_ucontext_t *, x86_trampoline_gregset_t *, void *),
        void *arg);
void x86_trampoline_epilogue(x86_ucontext_t *uc);

#ifdef __i386__ /* i686-only direct execution */
void x86_sigaction(int signum, siginfo_t *info, void *ptr);
#endif

#endif /* X86_H */

