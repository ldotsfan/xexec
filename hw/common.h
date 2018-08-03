/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  Copyright (c) 2017 Michael Saga. All rights reserved.
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

#ifndef HW_COMMON_H
#define HW_COMMON_H

typedef struct {
    int                     (*init)(void);
    int                     (*destroy)(void);
    void                    (*reset)(void);
    int                     (*irq)(void);
    void                    (*irq_raise)(int mask);
    void                    (*irq_restore)(int mask);
    int                     (*write)(uint32_t addr, const void *val, size_t sz);
    int                     (*read)(uint32_t addr, void *val, size_t sz);
} hw_ops_t;

typedef struct {
    const uint32_t          index;
    const char *            name;
    const uint32_t          offset;
    const uint32_t          size;
} hw_block_t;

#define HW_BLOCK(x,y,z) \
        [x] = { \
            .index = x, \
            .name = #x, \
            .offset = y, \
            .size = z \
        }

#endif

