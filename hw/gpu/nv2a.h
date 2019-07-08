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

#ifndef NV2A_H
#define NV2A_H

/* TODO: refactor into boilerplate macros for all hw */
#include "../common.h"

#define NV2A_REGADDR(x,y,z)         ((x) + NV2A_BLOCK_OFFSET(y) + y##_##z)

#define NV2A_MASK_GET(x,y)          ((x) & (y))
#define NV2A_MASK_SET(x,y)          ((x) |= (y))
#define NV2A_MASK_UNSET(x,y)        ((x) &= ~(y))

#define NV2A_MASK_BITSHIFT_GET(x,y) \
        (NV2A_MASK_GET(x,y) >> y##__BITSHIFT)
#define NV2A_MASK_BITSHIFT_SET(x,y,z) \
        NV2A_MASK_UNSET(x,y); \
        if ((y##_##z)) NV2A_MASK_SET(x,y##_##z << y##__BITSHIFT)
#define NV2A_MASK_BITSHIFT_TEST(x,y,z) \
        ((NV2A_MASK_GET(x,y) >> y##__BITSHIFT) == y##_##z)

/*
 * x = memreg base,
 * y = block index name,
 * z = register name (after block index name),
 * m = mask name (after register name),
 * v = variable
 */
#define NV2A_REG32(x,y,z) \
        REG32(NV2A_REGADDR(x,y,z))

//TODO: NV2A_REG32_MASK_GET is deprecated
#define NV2A_REG32_MASK_GET(x,y,z,m) \
        NV2A_MASK_GET(NV2A_REG32(x,y,z),y##_##z##_##m)
//TODO: NV2A_REG32_MASK_SET is deprecated; use NV2A_REG32_MASK_BITSHIFT_SET
#if 0
#define NV2A_REG32_MASK_SET(x,y,z,m) \
        NV2A_MASK_SET(NV2A_REG32(x,y,z),y##_##z##_##m)
#endif
//TODO: NV2A_REG32_MASK_UNSET is deprecated
#define NV2A_REG32_MASK_UNSET(x,y,z,m) \
        NV2A_MASK_UNSET(NV2A_REG32(x,y,z),y##_##z##_##m)

#define NV2A_REG32_MASK_SET_VAL(x,y,z,m,v) \
        NV2A_REG32_MASK_UNSET(x,y,z,m); \
        if ((v)) NV2A_MASK_SET(NV2A_REG32(x,y,z),NV2A_MASK_GET((v),y##_##z##_##m))

#define NV2A_REG32_MASK_BITSHIFT_GET(x,y,z,m) \
        (NV2A_REG32_MASK_GET(x,y,z,m) >> y##_##z##_##m##__BITSHIFT)
#define NV2A_REG32_MASK_BITSHIFT_SET(x,y,z,m,v) \
        NV2A_REG32_MASK_UNSET(x,y,z,m); \
        if ((y##_##z##_##m##_##v)) NV2A_MASK_SET(NV2A_REG32(x,y,z),NV2A_MASK_GET((y##_##z##_##m##_##v) << y##_##z##_##m##__BITSHIFT,y##_##z##_##m))
#define NV2A_REG32_MASK_BITSHIFT_SET_VAL(x,y,z,m,v) \
        NV2A_REG32_MASK_UNSET(x,y,z,m); \
        if ((v)) NV2A_MASK_SET(NV2A_REG32(x,y,z),NV2A_MASK_GET((v) << y##_##z##_##m##__BITSHIFT,y##_##z##_##m))
#define NV2A_REG32_MASK_BITSHIFT_TEST(x,y,z,m,v) \
        ((NV2A_REG32_MASK_GET(x,y,z,m) >> y##_##z##_##m##__BITSHIFT) == y##_##z##_##m##_##v)

#define NV2A_MAX_BATCH_LENGTH   0x0001ffff
#define NV2A_MAX_CHANNELS       32
#define NV2A_MAX_SUBCHANNELS    8
#define NV2A_MAX_TEXTURES       4
#define NV2A_MAX_VERTEX_ATTRIBS 16

enum {
    NV2A_VERTEX_ATTRIB_POSITION         =   0,
    NV2A_VERTEX_ATTRIB_WEIGHT,          /*  1 */
    NV2A_VERTEX_ATTRIB_NORMAL,          /*  2 */
    NV2A_VERTEX_ATTRIB_DIFFUSE,         /*  3 */
    NV2A_VERTEX_ATTRIB_SPECULAR,        /*  4 */
    NV2A_VERTEX_ATTRIB_FOG,             /*  5 */
    NV2A_VERTEX_ATTRIB_POINT_SIZE,      /*  6 */
    NV2A_VERTEX_ATTRIB_BACK_DIFFUSE,    /*  7 */
    NV2A_VERTEX_ATTRIB_BACK_SPECULAR,   /*  8 */
    NV2A_VERTEX_ATTRIB_TEXTURE0,        /*  9 */
    NV2A_VERTEX_ATTRIB_TEXTURE1,        /* 10 */
    NV2A_VERTEX_ATTRIB_TEXTURE2,        /* 11 */
    NV2A_VERTEX_ATTRIB_TEXTURE3,        /* 12 */
    NV2A_VERTEX_ATTRIB_RESERVED1,       /* 13 */
    NV2A_VERTEX_ATTRIB_RESERVED2,       /* 14 */
    NV2A_VERTEX_ATTRIB_RESERVED3,       /* 15 */
};

static const char *const nv2a_vertex_attrib_name[] = {
    NAME(NV2A_VERTEX_ATTRIB_POSITION),
    NAME(NV2A_VERTEX_ATTRIB_WEIGHT),
    NAME(NV2A_VERTEX_ATTRIB_NORMAL),
    NAME(NV2A_VERTEX_ATTRIB_DIFFUSE),
    NAME(NV2A_VERTEX_ATTRIB_SPECULAR),
    NAME(NV2A_VERTEX_ATTRIB_FOG),
    NAME(NV2A_VERTEX_ATTRIB_POINT_SIZE),
    NAME(NV2A_VERTEX_ATTRIB_BACK_DIFFUSE),
    NAME(NV2A_VERTEX_ATTRIB_BACK_SPECULAR),
    NAME(NV2A_VERTEX_ATTRIB_TEXTURE0),
    NAME(NV2A_VERTEX_ATTRIB_TEXTURE1),
    NAME(NV2A_VERTEX_ATTRIB_TEXTURE2),
    NAME(NV2A_VERTEX_ATTRIB_TEXTURE3),
    NAME(NV2A_VERTEX_ATTRIB_RESERVED1),
    NAME(NV2A_VERTEX_ATTRIB_RESERVED2),
    NAME(NV2A_VERTEX_ATTRIB_RESERVED3),
};

enum {
    NV_PMC          =   0,      /* card master control */
    NV_PBUS,        /*  1 */    /* bus control */
    NV_PFIFO,       /*  2 */    /* MMIO and DMA FIFO submission to PGRAPH and VPE */
    NV_PFIFO_CACHE, /*  3 */
    NV_PRMA,        /*  4 */    /* access to BAR0/BAR1 from real mode */
    NV_PVIDEO,      /*  5 */    /* video overlay */
    NV_PTIMER,      /*  6 */    /* time measurement and time-based alarms */
    NV_PCOUNTER,    /*  7 */    /* performance monitoring counters */
    NV_PVPE,        /*  8 */    /* MPEG2 decoding engine */
    NV_PTV,         /*  9 */    /* TV encoder */
    NV_PRMFB,       /* 10 */    /* aliases VGA memory window */
    NV_PRMVIO,      /* 11 */    /* aliases VGA sequencer and graphics controller registers */
    NV_PFB,         /* 12 */    /* memory interface */
    NV_PSTRAPS,     /* 13 */    /* straps readout / override */
    NV_PGRAPH,      /* 14 */    /* accelerated 2d/3d drawing engine */
    NV_09F,         /* 15 */    /* NV12_IMAGE_BLIT object */
    NV_097,         /* 16 */    /* NV20_KELVIN_PRIMITIVE object */
    NV_PCRTC,       /* 17 */    /* more CRTC controls */
    NV_PRMCIO,      /* 18 */    /* aliases VGA CRTC and attribute controller registers */
    NV_PRAMDAC,     /* 19 */    /* RAMDAC, cursor, and PLL control */
    NV_PRMDIO,      /* 20 */    /* aliases VGA palette registers */
    NV_062,         /* 21 */    /* NV10_CONTEXT_SURFACES_2D object */
    NV_PRAMIN,      /* 22 */    /* RAMIN access */
    NV_USER,        /* 23 */    /* PFIFO MMIO and DMA submission area */
};

#define NV2A_BLOCK_OFFSET(x) nv2a_block[x].offset
#define NV2A_BLOCK_SIZE(x)   nv2a_block[x].size

static const hw_block_t nv2a_block[] = {
    HW_BLOCK(NV_PMC,         0x000000, 0x001000),   /*  0 */
    HW_BLOCK(NV_PBUS,        0x001000, 0x001000),   /*  1 */
    HW_BLOCK(NV_PFIFO,       0x002000, 0x002000),   /*  2 */
    HW_BLOCK(NV_PFIFO_CACHE, 0x004000, 0x003000),   /*  3 */
    HW_BLOCK(NV_PRMA,        0x007000, 0x001000),   /*  4 */    /* no implementation */
    HW_BLOCK(NV_PVIDEO,      0x008000, 0x001000),   /*  5 */
    HW_BLOCK(NV_PTIMER,      0x009000, 0x001000),   /*  6 */
    HW_BLOCK(NV_PCOUNTER,    0x00a000, 0x001000),   /*  7 */    /* no implementation */
    HW_BLOCK(NV_PVPE,        0x00b000, 0x001000),   /*  8 */    /* no implementation */
    HW_BLOCK(NV_PTV,         0x00d000, 0x001000),   /*  9 */    /* no implementation */
    HW_BLOCK(NV_PRMFB,       0x0a0000, 0x020000),   /* 10 */    /* no implementation */
    HW_BLOCK(NV_PRMVIO,      0x0c0000, 0x001000),   /* 11 */    /* no implementation */
    HW_BLOCK(NV_PFB,         0x100000, 0x001000),   /* 12 */
    HW_BLOCK(NV_PSTRAPS,     0x101000, 0x001000),   /* 13 */    /* no implementation */
    HW_BLOCK(NV_PGRAPH,      0x400000, 0x002000),   /* 14 */
    HW_BLOCK(NV_09F,         0x500000, 0x002000),   /* 15 */
    HW_BLOCK(NV_097,         0x5c0000, 0x002000),   /* 16 */
    HW_BLOCK(NV_PCRTC,       0x600000, 0x001000),   /* 17 */
    HW_BLOCK(NV_PRMCIO,      0x601000, 0x001000),   /* 18 */    /* no implementation */
    HW_BLOCK(NV_PRAMDAC,     0x680000, 0x001000),   /* 19 */
    HW_BLOCK(NV_PRMDIO,      0x681000, 0x001000),   /* 20 */    /* no implementation */
    HW_BLOCK(NV_062,         0x6d0000, 0x002000),   /* 21 */
    HW_BLOCK(NV_PRAMIN,      0x700000, 0x100000),   /* 22 */
    HW_BLOCK(NV_USER,        0x800000, 0x800000),   /* 23 */
};

#include "reg/nv10.h"       /* NV_062 */
#include "reg/nv12.h"       /* NV_09F */
#include "reg/nv20.h"       /* NV_097 */
#include "reg/pmc.h"        /* NV_PMC */
#include "reg/pbus.h"       /* NV_PBUS */
#include "reg/pfifo.h"      /* NV_PFIFO */
#include "reg/pvideo.h"     /* NV_PVIDEO */
#include "reg/ptimer.h"     /* NV_PTIMER */
#include "reg/pfb.h"        /* NV_PFB */
#include "reg/pgraph.h"     /* NV_PGRAPH */
#include "reg/pcrtc.h"      /* NV_PCRTC */
#include "reg/pramdac.h"    /* NV_PRAMDAC */
#include "reg/pramin.h"     /* NV_PRAMIN */
#include "reg/user.h"       /* NV_USER */

typedef struct {
    const int      index;
    const char *   name;
    const uint32_t pixel_size;
    const uint32_t block_size;
    const int      linear;
    const GLint    gl_internal_format;
    const GLint    gl_format;
    const GLint    gl_type;
    const GLint    gl_swizzle_mask[4];
} nv2a_color_format_t;

static const nv2a_color_format_t nv2a_texture_color_format[] = {
#define FORMAT1(a,b,c,d,e,f,g) \
        FORMAT2(a,b,c,d,e,f,g,0,0,0,0)
#define FORMAT2(a,b,c,d,e,f,g,h,i,j,k) \
    [NV_097_SET_TEXTURE_FORMAT_COLOR_##a] = { NV_097_SET_TEXTURE_FORMAT_COLOR_##a, #a, b, c, d, e, f, g, { h, i, j, k } }
    /*  0 */ FORMAT2(SZ_Y8,
                1, 0, 0, GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_RED, GL_RED, GL_RED, GL_ONE),
    /*  1 */ FORMAT2(SZ_AY8,
                1, 0, 0, GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_RED, GL_RED, GL_RED, GL_RED),
    /*  2 */ FORMAT1(SZ_A1R5G5B5,
                2, 0, 0, GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV),
    /*  3 */ FORMAT1(SZ_X1R5G5B5,
                2, 0, 0, GL_RGB5, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV),
    /*  4 */ FORMAT1(SZ_A4R4G4B4,
                2, 0, 0, GL_RGBA4, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV),
    /*  5 */ FORMAT1(SZ_R5G6B5,
                2, 0, 0, GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5),
    /*  6 */ FORMAT1(SZ_A8R8G8B8,
                4, 0, 0, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV),
    /*  7 */ FORMAT1(SZ_X8R8G8B8,
                4, 0, 0, GL_RGB8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV),
    /* 11 */ FORMAT1(SZ_I8_A8R8G8B8,
                1, 0, 0, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV),
    /* 12 */ FORMAT1(L_DXT1_A1R5G5B5,
                2, 8, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 0, GL_RGBA),
    /* 14 */ FORMAT1(L_DXT23_A8R8G8B8,
                4, 16, 0, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 0, GL_RGBA),
    /* 15 */ FORMAT1(L_DXT45_A8R8G8B8,
                4, 16, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 0, GL_RGBA),
    /* 16 */ FORMAT1(LU_IMAGE_A1R5G5B5,
                2, 0, 1, GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV),
    /* 17 */ FORMAT1(LU_IMAGE_R5G6B5,
                2, 0, 1, GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5),
    /* 18 */ FORMAT1(LU_IMAGE_A8R8G8B8,
                4, 0, 1, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV),
    /* 19 */ FORMAT2(LU_IMAGE_Y8,
                1, 0, 1, GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_RED, GL_RED, GL_RED, GL_ONE),
    /* 20 */ // LU_IMAGE_SY8
    /* 21 */ // LU_IMAGE_X7SY9
    /* 22 */ // LU_IMAGE_R8B8
    /* 23 */ // LU_IMAGE_G8B8
    /* 24 */ // LU_IMAGE_SG8SB8
    /* 25 */ FORMAT2(SZ_A8,
                1, 0, 0, GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_ONE, GL_ONE, GL_ONE, GL_RED),
    /* 26 */ FORMAT2(SZ_A8Y8,
                2, 0, 0, GL_RG8, GL_RG, GL_UNSIGNED_BYTE, GL_GREEN, GL_GREEN, GL_GREEN, GL_RED),
    /* 27 */ FORMAT2(LU_IMAGE_AY8,
                1, 0, 1, GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_RED, GL_RED, GL_RED, GL_RED),
    /* 28 */ FORMAT1(LU_IMAGE_X1R5G5B5,
                2, 0, 1, GL_RGB5, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV),
    /* 29 */ FORMAT1(LU_IMAGE_A4R4G4B4,
                2, 0, 0, GL_RGBA4, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV),
    /* 30 */ FORMAT1(LU_IMAGE_X8R8G8B8,
                4, 0, 1, GL_RGB8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV),
    /* 31 */ FORMAT2(LU_IMAGE_A8,
                1, 0, 1, GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_ONE, GL_ONE, GL_ONE, GL_RED),
    /* 32 */ FORMAT2(LU_IMAGE_A8Y8,
                2, 0, 1, GL_RG8, GL_RG, GL_UNSIGNED_BYTE, GL_GREEN, GL_GREEN, GL_GREEN, GL_RED),
    /* 36 */ FORMAT1(LC_IMAGE_CR8YB8CB8YA8,
                2, 0, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV),//TODO format conversion ?
    /* 37 */ // LC_IMAGE_YB8CR8YA8CB8
    /* 38 */ // LU_IMAGE_A8CR8CB8Y8
    /* 39 */ FORMAT1(SZ_R6G5B5,
                2, 0, 0, GL_RGB8_SNORM, GL_RGB, GL_BYTE),//FIXME signed?
    /* 40 */ FORMAT2(SZ_G8B8,
                2, 0, 0, GL_RG8_SNORM, GL_RG, GL_BYTE, GL_ZERO, GL_RED, GL_GREEN, GL_ONE),//FIXME signed?
    /* 41 */ FORMAT2(SZ_R8B8,
                2, 0, 0, GL_RG8_SNORM, GL_RG, GL_BYTE, GL_RED, GL_ZERO, GL_GREEN, GL_ONE),//FIXME signed?
    /* 42 */ FORMAT1(SZ_DEPTH_X8_Y24_FIXED,
                4, 0, 1, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8),//TODO format conversion
    /* 43 */ // SZ_DEPTH_X8_Y24_FLOAT
    /* 44 */ // SZ_DEPTH_Y16_FIXED
    /* 45 */ // SZ_DEPTH_Y16_FLOAT
    /* 46 */ // LU_IMAGE_DEPTH_X8_Y24_FIXED
    /* 47 */ // LU_IMAGE_DEPTH_X8_Y24_FLOAT
    /* 48 */ FORMAT1(LU_IMAGE_DEPTH_Y16_FIXED,
                2, 0, 1, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT),//TODO format conversion
    /* 49 */ // LU_IMAGE_DEPTH_Y16_FLOAT
    /* 50 */ // SZ_Y16
    /* 51 */ // SZ_YB_16_YA_16
    /* 52 */ // LC_IMAGE_A4V6YB6A4U6YA6
    /* 53 */ FORMAT2(LU_IMAGE_Y16,
                2, 0, 1, GL_R16, GL_RED, GL_UNSIGNED_SHORT, GL_RED, GL_RED, GL_RED, GL_ONE),//TODO format conversion
    /* 54 */ // LU_IMAGE_YB16YA16
    /* 55 */ // LU_IMAGE_R6G5B5
    /* 56 */ // SZ_R5G5B5A1
    /* 57 */ // SZ_R4G4B4A4
    /* 58 */ FORMAT1(SZ_A8B8G8R8,
                4, 0, 0, GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV),//TODO format conversion
    /* 59 */ // SZ_B8G8R8A8
    /* 60 */ FORMAT1(SZ_R8G8B8A8,
                4, 0, 0, GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8),//TODO format conversion
    /* 61 */ // LU_IMAGE_R5G5B5A1
    /* 62 */ // LU_IMAGE_R4G4B4A4
    /* 63 */ FORMAT1(LU_IMAGE_A8B8G8R8,
                4, 0, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV),//TODO format conversion
    /* 64 */ FORMAT1(LU_IMAGE_B8G8R8A8,
                4, 0, 1, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8),//TODO format conversion
    /* 65 */ FORMAT1(LU_IMAGE_R8G8B8A8,
                4, 0, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8),//TODO format conversion
#undef FORMAT2
#undef FORMAT1
};

static const nv2a_color_format_t nv2a_surface_color_format[] = {
#define FORMAT(a,b,c,d,e) \
    [NV_097_SET_SURFACE_FORMAT_COLOR_##a] = { NV_097_SET_SURFACE_FORMAT_COLOR_##a, #a, b, 0, 0, c, d, e, { 0, 0, 0, 0 } }
    /*  1 */ FORMAT(LE_X1R5G5B5_Z1R5G5B5,
                2, GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV),
    /*  2 */ // LE_X1R5G5B5_O1R5G5B5
    /*  3 */ FORMAT(LE_R5G6B5,
                2, GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5),
    /*  4 */ FORMAT(LE_X8R8G8B8_Z8R8G8B8,
                4, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV),
    /*  5 */ // LE_X8R8G8B8_O8R8G8B8
    /*  6 */ // LE_X1A7R8G8B8_Z1A7R8G8B8
    /*  7 */ // LE_X1A7R8G8B8_O1A7R8G8B8
    /*  8 */ FORMAT(LE_A8R8G8B8,
                4, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV),
    /*  9 */ // LE_B8
    /* 10 */ // LE_G8B8
#undef FORMAT
};

static const nv2a_color_format_t nv2a_062_color_format[] = {
#define FORMAT(a,b,c,d,e) \
    [NV_062_COLOR_FORMAT_##a] = { NV_062_COLOR_FORMAT_##a, #a, b, 0, 0, c, d, e, { 0, 0, 0, 0 } }
    /*  1 */ // LE_Y8
    /*  2 */ FORMAT(LE_X1R5G5B5_Z1R5G5B5,
                2, GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV),
    /*  3 */ // LE_X1R5G5B5_O1R5G5B5
    /*  4 */ FORMAT(LE_R5G6B5,
                2, GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5),
    /*  5 */ // LE_Y16
    /*  6 */ FORMAT(LE_X8R8G8B8_Z8R8G8B8,
                4, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV),
    /*  7 */ // LE_X8R8G8B8_O8R8G8B8
    /*  8 */ // LE_X1A7R8G8B8_Z1A7R8G8B8
    /*  9 */ // LE_X1A7R8G8B8_O1A7R8G8B8
    /* 10 */ FORMAT(LE_A8R8G8B8,
                4, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV),
    /* 11 */ // LE_Y32
#undef FORMAT
};

/* FIXME: color channels might be wrong order + endianness */
typedef union {
    uint32_t     field;
    struct {
        uint8_t  b;         /*  0 */
        uint8_t  g;         /*  8 */
        uint8_t  r;         /* 16 */
        uint8_t  a;         /* 24 */
    } PACKED;
} PACKED nv2a_color_bgra_t;

typedef union {
    uint16_t     field;
    struct {
        uint16_t b : 5;     /*  0 */
        uint16_t g : 5;     /*  5 */
        uint16_t r : 6;     /* 10 */
    } PACKED;
} PACKED nv2a_color_r6g5b5_t;

#if 0
renamed:
attribute->count -> a->format.size

//    case NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_UB_D3D:
//        assert(vertex_attribute->count == 4);
//        // http://www.opengl.org/registry/specs/ARB/vertex_array_bgra.txt
//        vertex_attribute->gl_count = GL_BGRA;
//        break;
//    case NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S1:
//        break;
//    case NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F:
//        break;
//    case NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_UB_OGL:
//        break;
//    case NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K:
//        break;
//    case NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_CMP:
//        vertex_attribute->converted_size = sizeof(float);
//        vertex_attribute->converted_count = 3 * vertex_attribute->count;
//        break;
#endif
typedef struct {
    const int       index;
    const char *    name;
    const uint32_t  size;
    const GLint     gl_size;
    const GLenum    gl_type;
    const GLboolean gl_normalized;
    const int       convert;
    const uint32_t  convert_nmemb;
    const uint32_t  convert_size;
} nv2a_inline_array_format_t;

static const nv2a_inline_array_format_t nv2a_inline_array_format[] = {
#define FORMAT1(a,b,c,d,e) \
    [NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_##a] = { NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_##a, #a, b, c, d, e, 0, 0, 0 }
#define FORMAT2(a,b,c,d,e,f,g) \
    [NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_##a] = { NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_##a, #a, b, c, d, e, 1, f, g }
    /* 0 */ FORMAT1(UB_D3D, /* http://www.opengl.org/registry/specs/ARB/vertex_array_bgra.txt */
                1, GL_BGRA, GL_UNSIGNED_BYTE, GL_TRUE),
    /* 1 */ FORMAT1(S1,
                2, 0, GL_SHORT, GL_TRUE),
    /* 2 */ FORMAT1(F,
                4, 0, GL_FLOAT, GL_FALSE),
    /* 4 */ FORMAT1(UB_OGL,
                1, 0, GL_UNSIGNED_BYTE, GL_TRUE),
    /* 5 */ FORMAT1(S32K,
                2, 0, GL_SHORT, GL_FALSE),
    /* 6 */ FORMAT2(CMP, /* 3 signed, normalized components packed in 32-bits [11,11,10] */
                4, 0, GL_FLOAT, GL_FALSE, 3, sizeof(float)),
#undef FORMAT2
#undef FORMAT1
};

#endif

