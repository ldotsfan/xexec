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

#ifndef INT128_H
#define INT128_H

#include <stdint.h>

#define _INT128(x,y)        (x).lo = (uint64_t)(y), (x).hi = 0

#define _INT128_INIT(x,y)   { .hi = (x), .lo = (y) }
#define _INT128_0           _INT128_INIT(0,0)
#define _INT128_1           _INT128_INIT(0,1)

#define uint128_0           _INT128_0
#define uint128_1           _INT128_1
#define uint128_set(x,y)    _INT128(x,y)

typedef struct {
    uint64_t lo;
    uint64_t hi;
} uint128_t;

static const uint128_t _uint128_0 = _INT128_0;
static const uint128_t _uint128_1 = _INT128_1;

int
uint128_z(const uint128_t *l) {
    return (!l->lo && !l->hi);
}

int
uint128_nz(const uint128_t *l) {
    return (l->lo || l->hi);
}

int
uint128_eq(const uint128_t *l, const uint128_t *r) {
    return (l->hi == r->hi && l->lo == r->lo);
}

int
uint128_lt(const uint128_t *l, const uint128_t *r) {
    return (l->hi == r->hi) ? (l->lo < r->lo) : (l->hi < r->hi);
}

int
uint128_gt(const uint128_t *l, const uint128_t *r) {
    return (l->hi == r->hi) ? (l->lo > r->lo) : (l->hi > r->hi);
}

int
uint128_lte(const uint128_t *l, const uint128_t *r) {
    return (uint128_lt(l, r) || uint128_eq(l, r));
}

int
uint128_gte(const uint128_t *l, const uint128_t *r) {
    return (uint128_gt(l, r) || uint128_eq(l, r));
}

uint128_t
uint128_add(const uint128_t *l, const uint128_t *r) {
    uint128_t v = *l;

    v.lo += r->lo;
    v.hi += r->hi + (v.lo < l->lo);

    return v;
}

uint128_t
uint128_sub(const uint128_t *l, const uint128_t *r) {
    uint128_t v = *l;

    v.lo -= r->lo;
    v.hi -= r->hi - (v.lo > l->lo);

    return v;
}

uint128_t
uint128_inc(uint128_t *l) {
    return (*l = uint128_add(l, &_uint128_1));
}

uint128_t
uint128_dec(uint128_t *l) {
    return (*l = uint128_sub(l, &_uint128_1));
}

uint128_t
uint128_shl(const uint128_t *l, unsigned s) {
    uint128_t v = *l;

    if (!s) return v;
    if (s < 64) {
        v.hi <<= s;
        v.hi  += v.lo >> (64 - s);
        v.lo <<= s;
        return v;
    }
    if (s == 64) {
        v.hi = v.lo;
        v.lo = 0;
        return v;
    }
    if (s > 64 && s < 128) {
        v.hi = v.lo << (s - 64);
        v.lo = 0;
        return v;
    }

    return _uint128_0;
}

uint128_t
uint128_shr(const uint128_t *l, unsigned s) {
    uint128_t v = *l;

    if (!s) return v;
    if (s < 64) {
        v.lo >>= s;
        v.lo  += v.hi << (64 - s);
        v.hi >>= s;
        return v;
    }
    if (s == 64) {
        v.lo = v.hi;
        v.hi = 0;
        return v;
    }
    if (s > 64 && s < 128) {
        v.lo = v.hi >> (s - 64);
        v.hi = 0;
        return v;
    }

    return _uint128_0;
}

uint128_t
uint128_mul(const uint128_t *l, const uint128_t *r) {
    uint128_t v;
    uint64_t t[4];
    uint64_t b[4];
    uint64_t p[4][4];
    register int x;
    register int y;

    t[0]  = l->hi >> 32;
    t[1]  = l->hi & 0xffffffff;
    t[2]  = l->lo >> 32;
    t[3]  = l->lo & 0xffffffff;
    b[0]  = r->hi >> 32;
    b[1]  = r->hi & 0xffffffff;
    b[2]  = r->lo >> 32;
    b[3]  = r->lo & 0xffffffff;

    for (y = 3; y >= 0; --y) for (x = 3; x >= 0; --x) p[3 - x][y] = t[x] * b[y];

    t[3]  = p[0][3] & 0xffffffff;
    t[2]  = p[0][2] & 0xffffffff;
    t[2] += p[0][3] >> 32;
    t[1]  = p[0][1] & 0xffffffff;
    t[1] += p[0][2] >> 32;
    t[0]  = p[0][0] & 0xffffffff;
    t[0] += p[0][1] >> 32;

    t[2] += p[1][3] & 0xffffffff;
    t[1] += p[1][2] & 0xffffffff;
    t[1] += p[1][3] >> 32;
    t[0] += p[1][1] & 0xffffffff;
    t[0] += p[1][2] >> 32;

    t[1] += p[2][3] & 0xffffffff;
    t[0] += p[2][2] & 0xffffffff;
    t[0] += p[2][3] >> 32;

    t[0] += p[3][3] & 0xffffffff;

    t[1] += t[2] >> 32;
    t[0] += t[1] >> 32;

    t[2] &= 0xffffffff;
    t[1] &= 0xffffffff;
    t[0] &= 0xffffffff;

    v.hi  = (t[0] << 32) | t[1];
    v.lo  = (t[2] << 32) | t[3];

    return v;
}

unsigned
uint128_bits(const uint128_t *l) {
    uint128_t v = *l;
    register unsigned x;

    if (v.hi) {
        x = 64;
        while (v.hi) v.hi >>= 1, ++x;
    } else {
        x = 0;
        while (v.lo) v.lo >>= 1, ++x;
    }

    return x;
}

void
uint128_divmod(const uint128_t *l, const uint128_t *r, uint128_t *d, uint128_t *m) {
    uint128_t div = _INT128_0;
    uint128_t mod = _INT128_0;
    register unsigned x;

    if (!d) d = &div;
    if (!m) m = &mod;

    if (uint128_z(r)) {
        *d = _uint128_0;
        *m = _uint128_0;
        return;
    }
    if (uint128_eq(r, &_uint128_1)) {
        *d = *l;
        *m = _uint128_0;
        return;
    }
    if (uint128_eq(l, r)) {
        *d = _uint128_1;
        *m = _uint128_0;
        return;
    }
    if (uint128_lt(l, r)) {
        *d = _uint128_0;
        *m = *l;
        return;
    }
    for (x = uint128_bits(l); x; --x) {
        div = uint128_shl(&div, 1);
        mod = uint128_shl(&mod, 1);
        if (uint128_shr(l, x - 1).lo & 1) uint128_inc(&mod);
        if (uint128_gte(&mod, r)) {
            mod = uint128_sub(&mod, r);
            uint128_inc(&div);
        }
    }

    *d = div;
    *m = mod;
}

uint128_t
uint128_div(const uint128_t *l, const uint128_t *r) {
    uint128_t v;

    uint128_divmod(l, r, &v, NULL);

    return v;
}

uint128_t
uint128_mod(const uint128_t *l, const uint128_t *r) {
    uint128_t v;

    uint128_divmod(l, r, NULL, &v);

    return v;
}

#endif

