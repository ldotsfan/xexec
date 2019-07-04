/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  Texture swizzling routines
 *
 *  Copyright (c) 2007 The Nouveau Project
 *  Copyright (c) 2013 espes
 *  Copyright (c) 2015 Jannik Vogel
 *  Copyright (c) 2018-2019 Michael Saga
 *  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <http://www.gnu.org/licenses/>
 *
 *  Contributions after 2012-01-13 are licensed under the terms of the
 *  GNU GPL, version 2 or (at your option) any later version.
 */

#include "swizzle.h"

/* This should be pretty straightforward.
 * It creates a bit pattern like ..zyxzyxzyx from ..xxx, ..yyy and ..zzz
 * If there are no bits left from any component it will pack the other masks
 * more tighly (Example: zzxzxzyx = Fewer x than z and even fewer y)
 */
static void
swizzle_mask_generate(
        uint32_t width,
        uint32_t height,
        uint32_t depth,
        uint32_t *mx,
        uint32_t *my,
        uint32_t *mz) {
    register uint32_t x;
    register uint32_t y;
    register uint32_t z;
    register uint32_t bit;
    register uint32_t mbit;
    register int done;

    for (done = x = y = z = 0, bit = mbit = 1; !done; bit <<= 1) {
        done = 1;
        if (bit < width)  x |= mbit, mbit <<= 1, done = 0;
        if (bit < height) y |= mbit, mbit <<= 1, done = 0;
        if (bit < depth)  z |= mbit, mbit <<= 1, done = 0;
    }

    if ((x ^ y ^ z) != mbit - 1) INT3;

    if (mx) *mx = x;
    if (my) *my = y;
    if (mz) *mz = z;
}

/* This fills a pattern with a value if your value has bits abcd and your
 * pattern is 11010100100 this will return: 0a0b0c00d00
 */
static uint32_t
swizzle_pattern_fill(uint32_t pattern, uint32_t value) {
    register uint32_t bit;
    register uint32_t ret;

    for (ret = 0, bit = 1; value; bit <<= 1) {
        if (pattern & bit) {
            /* copy bit to result */
            if (value & 1) ret |= bit;
            value >>= 1;
        }
    }

    return ret;
}

static uint32_t
swizzle_offset(
        uint32_t x,
        uint32_t y,
        uint32_t z,
        uint32_t mx,
        uint32_t my,
        uint32_t mz,
        uint32_t bytes_per_pixel) {
    register uint32_t ret;

    ret  = swizzle_pattern_fill(mx, x);
    ret |= swizzle_pattern_fill(my, y);
    ret |= swizzle_pattern_fill(mz, z);
    ret *= bytes_per_pixel;

    return ret;
}

void
swizzle_box(
        const uint8_t *in,
        uint32_t width,
        uint32_t height,
        uint32_t depth,
        uint8_t *out,
        uint32_t pitch,
        uint32_t pslice,
        uint32_t bytes_per_pixel) {
    register const uint8_t *src;
    register uint8_t *dst;
    register uint32_t x;
    register uint32_t y;
    register uint32_t z;
    uint32_t mx;
    uint32_t my;

    swizzle_mask_generate(width, height, depth, &mx, &my, NULL);

    for (z = 0; z < depth; ++z) {
        for (y = 0; y < height; ++y) {
            for (x = 0; x < width; ++x) {
                src = &in[y * pitch + x * bytes_per_pixel];
                dst = &out[swizzle_offset(x, y, 0, mx, my, 0, bytes_per_pixel)];
                memcpy(dst, src, bytes_per_pixel);
            }
        }
        in += pslice;
    }
}

void
unswizzle_box(
        const uint8_t *in,
        uint32_t width,
        uint32_t height,
        uint32_t depth,
        uint8_t *out,
        uint32_t pitch,
        uint32_t pslice,
        uint32_t bytes_per_pixel) {
    register const uint8_t *src;
    register uint8_t *dst;
    register uint32_t x;
    register uint32_t y;
    register uint32_t z;
    uint32_t mx;
    uint32_t my;
    uint32_t mz;

    swizzle_mask_generate(width, height, depth, &mx, &my, &mz);

    for (z = 0; z < depth; ++z) {
        for (y = 0; y < height; ++y) {
            for (x = 0; x < width; ++x) {
                src = &in[swizzle_offset(x, y, z, mx, my, mz, bytes_per_pixel)];
                dst = &out[y * pitch + x * bytes_per_pixel];
                memcpy(dst, src, bytes_per_pixel);
            }
        }
        out += pslice;
    }
}

void
swizzle_rect(
        const uint8_t *in,
        uint32_t width,
        uint32_t height,
        uint8_t *out,
        uint32_t pitch,
        uint32_t bytes_per_pixel) {
    swizzle_box(in, width, height, 1, out, pitch, 0, bytes_per_pixel);
}

void
unswizzle_rect(
        const uint8_t *in,
        uint32_t width,
        uint32_t height,
        uint8_t *out,
        uint32_t pitch,
        uint32_t bytes_per_pixel) {
    unswizzle_box(in, width, height, 1, out, pitch, 0, bytes_per_pixel);
}

