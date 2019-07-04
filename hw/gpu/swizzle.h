/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  Texture swizzling routines
 *
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

#ifndef SWIZZLE_H
#define SWIZZLE_H

#include <stdint.h>

void
swizzle_box(
        const uint8_t *in,
        uint32_t width,
        uint32_t height,
        uint32_t depth,
        uint8_t *out,
        uint32_t pitch,
        uint32_t pslice,
        uint32_t bytes_per_pixel);
void
unswizzle_box(
        const uint8_t *in,
        uint32_t width,
        uint32_t height,
        uint32_t depth,
        uint8_t *out,
        uint32_t pitch,
        uint32_t pslice,
        uint32_t bytes_per_pixel);
void
swizzle_rect(
        const uint8_t *in,
        uint32_t width,
        uint32_t height,
        uint8_t *out,
        uint32_t pitch,
        uint32_t bytes_per_pixel);
void
unswizzle_rect(
        const uint8_t *in,
        uint32_t width,
        uint32_t height,
        uint8_t *out,
        uint32_t pitch,
        uint32_t bytes_per_pixel);

#endif

