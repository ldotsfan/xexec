/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  Copyright (c) 2018 Michael Saga. All rights reserved.
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

/*
 * https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/74
 *
 * $ gcc -m64 -g tsc-scaler.c -o tsc-scaler
 * $ ./tsc-scaler
 * host TSC frequency    : 2793889043
 * TSC scaling factor    : 3.809848696 per 733 MHz tick
 *
 * host tick count       : 496106460379494
 * emulated tick count   : 130216840628375
 * emulated tick diff    : 733357523
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/prctl.h>

uint64_t tsc_scaler = 0;

int tsc_on() {
    return prctl(PR_SET_TSC, PR_TSC_ENABLE);
}

int tsc_off() {
    return prctl(PR_SET_TSC, PR_TSC_SIGSEGV);
}

uint64_t tsc(int xbox) {
    unsigned __int128 t;
    register uint32_t a;
    register uint32_t d;

    tsc_on();
    __asm__ __volatile__("rdtsc" : "=a" (a), "=d" (d));
    tsc_off();

    if (xbox && tsc_scaler) {
        t  = 1000000000;
        t *= ((uint64_t)d << 32) | a;
        t /= tsc_scaler;
        return (uint64_t)t;
    }

    return ((uint64_t)d << 32) | a;
}

uint64_t tsc_calibrate() {
    register uint64_t x;
    register uint64_t y;

    x  = tsc(0);
    sleep(1);
    y  = tsc(0);
    y -= x;

    fprintf(stderr, "host TSC frequency    : %lu\n", y);

    return y;
}

int main() {
    unsigned __int128 t;
    register unsigned i;
    register uint64_t x;

    t  = 1000000000;
    t *= tsc_calibrate();
    t /= 733333333;
    tsc_scaler = t;

    fprintf(stderr, "TSC scaling factor    : %lu.%.09lu per 733 MHz tick\n\n",
        tsc_scaler / 1000000000,
        tsc_scaler % 1000000000);

    for (t = tsc(1), i = 0; i < 10; ++i) {
        sleep(1);
        fprintf(stderr, "host tick count       : %lu\n", tsc(0));
        x = tsc(1);
        fprintf(stderr, "emulated tick count   : %lu\n", x);
        fprintf(stderr, "emulated tick diff    : %lu\n\n", x - (uint64_t)t);
        t = x;
    }

    return 0;
}

