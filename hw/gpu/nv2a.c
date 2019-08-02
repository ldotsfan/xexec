/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  Copyright (c) 2012 espes
 *  Copyright (c) 2015 Jannik Vogel
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

#include "nv2a.h"

#include "swizzle.c" //TODO move to a makefile

#define DEBUG_NV2A 2

#ifdef DEBUG_NV2A
# if DEBUG_NV2A == 1
#  define PRINT_NV2A        PRINT
#  define PRINTF_NV2A       PRINTF
# elif DEBUG_NV2A >= 2
#  define PRINT_NV2A        PRINT
#  define PRINTF_NV2A       PRINTF
#  define VARDUMP_NV2A      VARDUMP
#  define VARDUMP2_NV2A     VARDUMP2
#  define VARDUMP3_NV2A     VARDUMP3
#  define VARDUMP4_NV2A     VARDUMP4
# else
#  define ENTER_NV2A        ENTER
#  define LEAVE_NV2A        LEAVE
#  define PRINT_NV2A        PRINT
#  define PRINTF_NV2A       PRINTF
#  define VARDUMP_NV2A      VARDUMP
#  define VARDUMP2_NV2A     VARDUMP2
#  define VARDUMP3_NV2A     VARDUMP3
#  define VARDUMP4_NV2A     VARDUMP4
# endif
#endif
#ifndef ENTER_NV2A
# define ENTER_NV2A
#endif
#ifndef LEAVE_NV2A
# define LEAVE_NV2A
#endif
#ifndef PRINT_NV2A
# define PRINT_NV2A(...)
#endif
#ifndef PRINTF_NV2A
# define PRINTF_NV2A(...)
#endif
#ifndef VARDUMP_NV2A
# define VARDUMP_NV2A(...)
#endif
#ifndef VARDUMP2_NV2A
# define VARDUMP2_NV2A(...)
#endif
#ifndef VARDUMP3_NV2A
# define VARDUMP3_NV2A(...)
#endif
#ifndef VARDUMP4_NV2A
# define VARDUMP4_NV2A(...)
#endif

typedef struct {
    union {
        uint32_t                flags;
        struct {
            uint32_t class  : 12;   /*  0 */
            uint32_t pad1   : 4;    /* 12 */
            uint32_t target : 2;    /* 16 */
            uint32_t pad2   : 2;    /* 18 */
            uint32_t adjust : 12;   /* 20 */
        } PACKED;
    } PACKED;
    uint32_t                    limit;
    uint32_t                    frame;
} PACKED nv2a_dma_t;

#define NV2A_DMA(x) *(nv2a_dma_t *)(x)
#define NV2A_DMA_ADDRESS(x) ((((x)->frame & /* NV_DMA_ADDRESS */ 0xfffff000) | ((x)->adjust << 20)) & (host->memreg_size - 1))

typedef struct {
    GLuint                      gl_texture;
    GLenum                      gl_target;
    int                         ref;
} nv2a_texture_binding_t;

typedef struct {
    uint32_t                    index;
    int                         dirty;
    const nv2a_color_format_t * cf;
    nv2a_texture_binding_t *    b;
    /* texture data */
    uint32_t                    toffset;
    void *                      tdata;
    uint32_t                    tlen;
    /* palette data */
    uint32_t                    poffset;
    void *                      pdata;
    uint32_t                    plen;
    /* register state */
    union {
        uint32_t     field;
        struct {
            uint32_t context_dma         : 2;   /*  0 */
            uint32_t cubemap             : 1;   /*  2 */
            uint32_t border_source       : 1;   /*  3 */
            uint32_t dimensionality      : 4;   /*  4 */
            uint32_t color               : 8;   /*  8 */
            uint32_t mipmap_levels       : 4;   /* 16 */
            uint32_t base_size_u         : 4;   /* 20 */
            uint32_t base_size_v         : 4;   /* 24 */
            uint32_t base_size_p         : 4;   /* 28 */
        } PACKED;
    } PACKED                    format;
    union {
        uint32_t     field;
        struct {
            uint32_t u                   : 4;   /*  0 */
            uint32_t cylwrap_u           : 4;   /*  4 */
            uint32_t v                   : 4;   /*  8 */
            uint32_t cylwrap_v           : 4;   /* 12 */
            uint32_t p                   : 4;   /* 16 */
            uint32_t cylwrap_p           : 4;   /* 20 */
            uint32_t cylwrap_q           : 8;   /* 24 */
        } PACKED;
    } PACKED                    address;
    union {
        uint32_t     field;
        struct {
            uint32_t color_key_operation : 2;   /*  0 */
            uint32_t alpha_kill_enable   : 1;   /*  2 */
            uint32_t image_field_enable  : 1;   /*  3 */
            uint32_t log_max_aniso       : 2;   /*  4 */
            uint32_t max_lod_clamp       : 12;  /*  6 */
            uint32_t min_lod_clamp       : 12;  /* 18 */
            uint32_t enable              : 2;   /* 30 */
        } PACKED;
    } PACKED                    ctl0;
    uint32_t                    pitch;
    union {
        uint32_t     field;
        struct {
            uint32_t mipmap_lod_bias     : 13;  /*  0 */
            uint32_t convolution_kernel  : 3;   /* 13 */
            uint32_t min                 : 8;   /* 16 */
            uint32_t mag                 : 4;   /* 24 */
            uint32_t asigned_enabled     : 1;   /* 28 */
            uint32_t rsigned_enabled     : 1;   /* 29 */
            uint32_t gsigned_enabled     : 1;   /* 30 */
            uint32_t bsigned_enabled     : 1;   /* 31 */
        } PACKED;
    } PACKED                    filter;
    uint32_t                    width;
    uint32_t                    height;
    uint32_t                    depth;
    union {
        uint32_t     field;
        struct {
            uint32_t context_dma         : 2;   /*  0 */
            uint32_t length              : 4;   /*  2 */
            uint32_t offset              : 26;  /*  6 */
        } PACKED;
    } PACKED                    palette;
    nv2a_color_bgra_t           bcolor;
    uint32_t                    mlevels;
} nv2a_texture_t;

static const GLenum nv2a_texture_format_cubemap_map[] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

static const GLenum nv2a_texture_filter_min_map[] = {
    0,
    GL_NEAREST,
    GL_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_LINEAR,
    GL_LINEAR, /* TODO: Convolution filter... */
};

static const GLenum nv2a_texture_filter_mag_map[] = {
    0,
    GL_NEAREST,
    GL_LINEAR,
    0,
    GL_LINEAR, /* TODO: Convolution filter... */
};

static const GLenum nv2a_texture_wrap_map[] = {
    0,
    GL_REPEAT,
    GL_MIRRORED_REPEAT,
    GL_CLAMP_TO_EDGE,
    GL_CLAMP_TO_BORDER,
//    GL_CLAMP,
};

typedef struct {
#if 0 // rewrite
//    bool dma_select;
//    hwaddr offset;

    /* inline arrays are packed in order?
     * Need to pass the offset to converted attributes */
    unsigned int inline_array_offset;

//    float inline_value[4];

//    unsigned int format;//XXX type
//    unsigned int size; /* size of the data type */
    unsigned int count; /* number of components *///XXX size
//    uint32_t stride;

//    bool needs_conversion;
    uint8_t *converted_buffer;
    unsigned int converted_elements;
    unsigned int converted_size;
    unsigned int converted_count;

//    float *inline_buffer;

    GLint gl_count;
//    GLenum gl_type;
//    GLboolean gl_normalize;

//    GLuint gl_converted_buffer;
//    GLuint gl_inline_buffer;
#endif
    uint32_t                    index;
    int                         dirty;
    const nv2a_inline_array_format_t *f;
    uint32_t                    inline_array_offset;
    /* converted inline array data */
    uint32_t                    converted_count;
    void *                      converted_buffer;
    GLuint                      gl_converted_buffer;
    /* inline array data */
//    uint32_t                    voffset;
    void *                      vdata;
    uint32_t                    vstride;
    float                       inline_value[4];
    uint32_t                    inline_buffer_len;
    float                       inline_buffer[NV2A_MAX_BATCH_LENGTH][4];
    GLuint                      gl_inline_buffer;
    /* register state */
    union {
        uint32_t     field;
        struct {
            uint32_t offset      : 31;  /*  0 */
            uint32_t context_dma : 1;   /* 31 */
        } PACKED;
    } PACKED                    vertex;
    union {
        uint32_t     field;
        struct {
            uint32_t type        : 4;   /*  0 */
            uint32_t size        : 4;   /*  4 */
            uint32_t stride      : 24;  /*  8 */
        } PACKED;
    } PACKED                    format;
} nv2a_vertex_attrib_t;

typedef struct {
    glo_context *               glo;

    GLuint                      gl_framebuffer;

    GLuint                      gl_color_buffer;
    GLuint                      gl_zeta_buffer;

//    GLuint                      gl_memory_buffer;

    GLuint                      gl_vertex_array;

    nv2a_texture_t              t[NV2A_MAX_TEXTURES];

    nv2a_vertex_attrib_t        va[NV2A_MAX_VERTEX_ATTRIBS];
    uint32_t                    inline_buffer_pos;
    uint32_t                    inline_element_len;
    uint32_t                    inline_element[NV2A_MAX_BATCH_LENGTH];
    GLuint                      gl_inline_element_buffer;
    uint32_t                    inline_array_len;
    uint32_t                    inline_array[NV2A_MAX_BATCH_LENGTH];
    GLuint                      gl_inline_array_buffer;
    uint32_t                    draw_arrays_max;
    uint32_t                    draw_arrays_len;
    GLint                       gl_draw_arrays_start[1000]; //FIXME: unknown size, possibly endless, 1000 will do for now
    GLsizei                     gl_draw_arrays_count[1000]; //FIXME: unknown size, possibly endless, 1000 will do for now
} nv2a_context_t;

static nv2a_context_t *         nv2a_ctx = NULL;

typedef struct {
    union {
        uint32_t     field;
        struct {
            uint32_t method       : 13; /*  0 */
            uint32_t subchannel   : 3;  /* 13 */
            uint32_t non_inc      : 2;  /* 16 */
            uint32_t method_count : 11; /* 18 */
            uint32_t error        : 3;  /* 29 */
        } PACKED;
    } PACKED;
    uint32_t                    param;
} PACKED nv2a_pfifo_command_t;

typedef struct {
    uint32_t                    handle;
    union {
        uint32_t     field;
        struct {
            uint32_t instance : 16; /*  0 */
            uint32_t engine   : 2;  /* 16 */
#define NV_RAMHT_ENGINE_SW       0
#define NV_RAMHT_ENGINE_GRAPHICS 1
#define NV_RAMHT_ENGINE_DVD      2
            uint32_t pad1     : 6;  /* 18 */
            uint32_t chid     : 5;  /* 24 */
            uint32_t pad2     : 2;  /* 29 */
            uint32_t valid    : 1;  /* 31 */
        } PACKED;
    } PACKED;
} PACKED nv2a_pfifo_ramht_t;

static pthread_mutex_t          nv2a_pgraph_fifo_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t           nv2a_pgraph_fifo_cond = PTHREAD_COND_INITIALIZER;
#define PGRAPH_FIFO_LOCK        pthread_mutex_lock(&nv2a_pgraph_fifo_mutex)
#define PGRAPH_FIFO_UNLOCK      pthread_mutex_unlock(&nv2a_pgraph_fifo_mutex)
#define PGRAPH_FIFO_WAIT        pthread_cond_wait(&nv2a_pgraph_fifo_cond, &nv2a_pgraph_fifo_mutex)
#define PGRAPH_FIFO_SIGNAL      PGRAPH_FIFO_LOCK, \
                                pthread_cond_broadcast(&nv2a_pgraph_fifo_cond), \
                                PGRAPH_FIFO_UNLOCK
static pthread_mutex_t          nv2a_pgraph_intr_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t           nv2a_pgraph_intr_cond = PTHREAD_COND_INITIALIZER;
#define PGRAPH_INTR_LOCK        pthread_mutex_lock(&nv2a_pgraph_intr_mutex)
#define PGRAPH_INTR_UNLOCK      pthread_mutex_unlock(&nv2a_pgraph_intr_mutex)
#define PGRAPH_INTR_WAIT        pthread_cond_wait(&nv2a_pgraph_intr_cond, &nv2a_pgraph_intr_mutex)
#define PGRAPH_INTR_SIGNAL      PGRAPH_INTR_LOCK, \
                                pthread_cond_broadcast(&nv2a_pgraph_intr_cond), \
                                PGRAPH_INTR_UNLOCK

static pthread_mutex_t          nv2a_flip_stall_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t           nv2a_flip_stall_cond = PTHREAD_COND_INITIALIZER;
#define FLIP_STALL_LOCK         pthread_mutex_lock(&nv2a_flip_stall_mutex)
#define FLIP_STALL_UNLOCK       pthread_mutex_unlock(&nv2a_flip_stall_mutex)
#define FLIP_STALL_WAIT         pthread_cond_wait(&nv2a_flip_stall_cond, &nv2a_flip_stall_mutex)
#define FLIP_STALL_SIGNAL       FLIP_STALL_LOCK, \
                                pthread_cond_broadcast(&nv2a_flip_stall_cond), \
                                FLIP_STALL_UNLOCK

static int
nv2a_offset(uint32_t *addr) {
    register int ret;

    if ((ret = (*addr & 0xff000000) == nv2a->memreg_base)) *addr &= ~0xff000000;

    return !ret;
}

static const hw_block_t *
nv2a_block_lookup(uint32_t addr, const char **reg) {
    register const hw_block_t *b;
    register size_t i;
    ENTER_NV2A;

    if (reg) *reg = NULL;

    for (i = 0; i <= ARRAY_SIZE(nv2a_block); ++i) {
        if (i >= ARRAY_SIZE(nv2a_block)) INT3;
        b = &nv2a_block[i];
        if (RANGE(b->offset, b->size, addr)) {
            if (reg) {
                i = (addr - b->offset) / 4;
                switch (b->index) {
                case NV_PMC:
                    ARRAY(*reg, nv2a_pmc_name, i);
                    break;
                case NV_PBUS:
                    ARRAY(*reg, nv2a_pbus_name, i);
                    break;
                case NV_PFIFO:
                    ARRAY(*reg, nv2a_pfifo_name, i);
                    break;
                case NV_PVIDEO:
                    ARRAY(*reg, nv2a_pvideo_name, i);
                    break;
                case NV_PTIMER:
                    ARRAY(*reg, nv2a_ptimer_name, i);
                    break;
                case NV_PFB:
                    ARRAY(*reg, nv2a_pfb_name, i);
                    break;
                case NV_PGRAPH:
                    ARRAY(*reg, nv2a_pgraph_name, i);
                    break;
                case NV_PCRTC:
                    ARRAY(*reg, nv2a_pcrtc_name, i);
                    break;
                case NV_PRAMDAC:
                    ARRAY(*reg, nv2a_pramdac_name, i);
                    break;
                case NV_USER:
                    i = NV2A_MASK_BITSHIFT_GET(addr - b->offset, NV_USER_REG) / 4;
                    ARRAY(*reg, nv2a_user_name, i);
                    break;
                }
            }
            break;
        }
    }

    LEAVE_NV2A;
    return b;
}

#define NV2A_IRQ_BUSY   (1 << 0)
#define NV2A_IRQ_PGRAPH (1 << 1)
#define NV2A_IRQ_FIFO   (1 << 2)

#define NV2A_IRQ_WAIT(x,y,z) \
        x##_##y##_LOCK; \
        while (nv2a->irq_busy || NV2A_REG32_MASK_GET(nv2a->memreg, NV_##x, y, z)) { \
            if (nv2a->irq_busy) nv2a->irq_busy |= NV2A_IRQ_##x; \
            x##_##y##_WAIT; \
        } \
        x##_##y##_UNLOCK
#define NV2A_IRQ_WAITN(x,y,z) \
        x##_##y##_LOCK; \
        while (nv2a->irq_busy || !NV2A_REG32_MASK_GET(nv2a->memreg, NV_##x, y, z)) { \
            if (nv2a->irq_busy) nv2a->irq_busy |= NV2A_IRQ_##x; \
            x##_##y##_WAIT; \
        } \
        x##_##y##_UNLOCK

static void
nv2a_irq_restore(int mask) {
    if (!mask || !(mask & ~NV2A_IRQ_BUSY)) return;
    if (mask & NV2A_IRQ_PGRAPH) PGRAPH_INTR_SIGNAL;
    if (mask & NV2A_IRQ_FIFO)   PGRAPH_FIFO_SIGNAL;
}

static void
nv2a_irq_raise(int mask) {
    if (!mask) return;
    if (nv2a->irq_busy) {
        if (mask & NV2A_IRQ_PGRAPH) nv2a->irq_busy |= NV2A_IRQ_PGRAPH;
        if (mask & NV2A_IRQ_FIFO)   nv2a->irq_busy |= NV2A_IRQ_FIFO;
    } else {
        nv2a_irq_restore(mask);
    }
}

static int
nv2a_irq(void) {
    register void *p;
    register uint32_t enabled;
    register uint32_t pending;
    register int ret = 0;
    ENTER_NV2A;

    p = nv2a->memreg;

    if (!p ||
        !nv2a->irq_kinterrupt ||
        !nv2a->irq_connected ||
        !nv2a->irq_isr_routine) {
        LEAVE_NV2A;
        return ret;
    }

    /* PFIFO */
    enabled = NV2A_REG32(p, NV_PFIFO, INTR_EN_0);
    pending = NV2A_REG32(p, NV_PFIFO, INTR_0);
    VARDUMP3_NV2A(DUMP, enabled, nv2a_pfifo_intr_name);
    VARDUMP3_NV2A(DUMP, pending, nv2a_pfifo_intr_name);
    PRINTF_NV2A(XEXEC_DBG_IRQ, "PFIFO:  %spending", !(pending & enabled) ? "not " : "");
    if (pending & enabled) {
        NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PMC, INTR_0, PFIFO, PENDING);
    } else {
        NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PMC, INTR_0, PFIFO, NOT_PENDING);
    }
    /* PCRTC */
    enabled = NV2A_REG32(p, NV_PCRTC, INTR_EN_0);
    pending = NV2A_REG32(p, NV_PCRTC, INTR_0);
    VARDUMP3_NV2A(DUMP, enabled, nv2a_pcrtc_intr_name);
    VARDUMP3_NV2A(DUMP, pending, nv2a_pcrtc_intr_name);
    PRINTF_NV2A(XEXEC_DBG_IRQ, "PCRTC:  %spending", !(pending & enabled) ? "not " : "");
    if (pending & enabled) {
        NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PMC, INTR_0, PCRTC, PENDING);
    } else {
        NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PMC, INTR_0, PCRTC, NOT_PENDING);
    }
    /* PGRAPH */
    enabled = NV2A_REG32(p, NV_PGRAPH, INTR_EN);
    pending = NV2A_REG32(p, NV_PGRAPH, INTR);
    VARDUMP3_NV2A(DUMP, enabled, nv2a_pgraph_intr_name);
    VARDUMP3_NV2A(DUMP, pending, nv2a_pgraph_intr_name);
    PRINTF_NV2A(XEXEC_DBG_IRQ, "PGRAPH: %spending", !(pending & enabled) ? "not " : "");
    if (pending & enabled) {
        NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PMC, INTR_0, PGRAPH, PENDING);
    } else {
        NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PMC, INTR_0, PGRAPH, NOT_PENDING);
    }

    if (NV2A_REG32(p, NV_PMC, INTR_0) &&
        NV2A_REG32(p, NV_PMC, INTR_EN_0)) {
        PRINTF_NV2A(XEXEC_DBG_IRQ, "raised!", 0);
        nv2a->irq_level = 1;
        ret = 1;
    }

    LEAVE_NV2A;
    return ret;
}

static void
nv2a_pgraph_intr_context_switch(void *p, uint32_t chid) {
    ENTER_NV2A;

    if (//NV2A_REG32_MASK_BITSHIFT_TEST(p, NV_PGRAPH, CTX_CONTROL, CHID, VALID) &&//FIXME will be invalid
        NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, CTX_USER, CHID) == chid) {
        LEAVE_NV2A;
        return;
    }
INT3;//XXX
    NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, TRAPPED_ADDR, CHID, chid);
    NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PGRAPH, INTR, CONTEXT_SWITCH, PENDING);

    IRQ_NV2A_SIGNAL;

    NV2A_IRQ_WAIT(PGRAPH, INTR, CONTEXT_SWITCH);

    LEAVE_NV2A;
}

static void
nv2a_color_convert_yuy2_to_rgb(
        const uint8_t *line,
        uint32_t ix,
        uint8_t *r,
        uint8_t *g,
        uint8_t *b) {
    register int c;
    register int d;
    register int e;
    ENTER_NV2A;

    c = (int)line[ix * 2] - 16;
    if (ix % 2) {
        d = (int)line[ix * 2 - 1] - 128;
        e = (int)line[ix * 2 + 1] - 128;
    } else {
        d = (int)line[ix * 2 + 1] - 128;
        e = (int)line[ix * 2 + 3] - 128;
    }
    *r = CLIP08((298 * c + 409 * e + 128) >> 8);
    *g = CLIP08((298 * c - 100 * d - 208 * e + 128) >> 8);
    *b = CLIP08((298 * c + 516 * d + 128) >> 8);

    LEAVE_NV2A;
}

static uint8_t *
nv2a_texture_convert(
        nv2a_texture_t *t,
        const uint8_t *tdata,
        const uint8_t *pdata,
        uint32_t width,
        uint32_t height,
        uint32_t depth,
        uint32_t pitch,
        uint32_t pslice) {
    register uint32_t x;
    register uint32_t y;
    register uint8_t *ret = NULL;
    ENTER_NV2A;

    switch (t->format.color) {
    case NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_I8_A8R8G8B8 /* 11 */:
        if (depth != 1) INT3;//FIXME
        if (!(ret = malloc(width * height * 4))) INT3;
        for (y = 0; y < height; ++y) {
            for (x = 0; x < width; ++x) {
                REG32(&ret[y * width * 4 + x * 4]) = REG32(&pdata[tdata[y * pitch + x] * 4]);
            }
        }
        break;
    case NV_097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_CR8YB8CB8YA8 /* 36 */:
        if (depth != 1) INT3;//FIXME
        if (!(ret = malloc(width * height * 4))) INT3;
        for (y = 0; y < height; ++y) {
            register const uint8_t *line = &tdata[y * width * 2];
            for (x = 0; x < width; ++x) {
                register uint8_t *p = &ret[(y * width + x) * 4];
                /* FIXME: Actually needs uyvy? */
                nv2a_color_convert_yuy2_to_rgb(line, x, &p[0], &p[1], &p[2]);
                p[3] = 255;
            }
        }
        break;
    case NV_097_SET_TEXTURE_FORMAT_COLOR_SZ_R6G5B5 /* 39 */:
        if (depth != 1) INT3;//FIXME
        if (!(ret = malloc(width * height * 3))) INT3;
        for (y = 0; y < height; ++y) {
            for (x = 0; x < width; ++x) {
                register nv2a_color_r6g5b5_t rgb655 = *(typeof(rgb655) *)&tdata[y * pitch + x * 2];
                register int8_t *p = (void *)&ret[(y * width + x) * 3];
                /* Maps 5 bit G and B signed value range to 8 bit
                 * signed values. R is probably unsigned.
                 */
                rgb655.field ^= (1 << 9) | (1 << 4);
                p[0] = rgb655.r * 0x7f / 0x3f;
                p[1] = rgb655.g * 0xff / 0x1f - 0x80;
                p[2] = rgb655.b * 0xff / 0x1f - 0x80;
            }
        }
        break;
    }

    LEAVE_NV2A;
    return ret;
}

static void
nv2a_texture_binding_upload(nv2a_texture_binding_t *b, nv2a_texture_t *t) {
    GLenum gl_target;
    register const void *p;
    register void *u;
    register void *c;
    register uint32_t w;
    register uint32_t h;
    register uint32_t d;
    register uint32_t pitch;
    register uint32_t pslice;
    register uint32_t length;
    register uint32_t i;
    register uint32_t j;
    ENTER_NV2A;

    gl_target = b->gl_target;
    p         = t->tdata;

    switch (gl_target) {
    case GL_TEXTURE_1D:
        PRINTF_NV2A(XEXEC_DBG_ALL, "/* FIXME: 1d texture not supported */", 0);
        INT3;
        break;
    case GL_TEXTURE_RECTANGLE:
        /* cannot handle strides unaligned to pixels */
        if (t->pitch % t->cf->pixel_size) INT3;
        glPixelStorei(GL_UNPACK_ROW_LENGTH, t->pitch / t->cf->pixel_size);
        c = nv2a_texture_convert(t, p, t->pdata, t->width, t->height, 1, t->pitch, 0);
        glTexImage2D(gl_target, 0, t->cf->gl_internal_format, t->width, t->height, 0, t->cf->gl_format, t->cf->gl_type, (c) ? c : p);
        if (c) free(c);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        break;
    case GL_TEXTURE_CUBE_MAP:
        j = 0;
        do {
            gl_target = nv2a_texture_format_cubemap_map[j];
            /* fall through */
    case GL_TEXTURE_2D:
            if (t->cf->gl_format) {
                for (w = t->width, h = t->height, i = 0; i < t->mlevels; ++i) {
                    w       = MAX(w, 1);
                    h       = MAX(h, 1);
                    pitch   = w * t->cf->pixel_size;
                    length  = h * pitch;
                    if (!(u = malloc(length))) INT3;
                    unswizzle_rect(p, w, h, u, pitch, t->cf->pixel_size);
                    c       = nv2a_texture_convert(t, u, t->pdata, w, h, 1, pitch, 0);
                    glTexImage2D(gl_target, i, t->cf->gl_internal_format, w, h, 0, t->cf->gl_format, t->cf->gl_type, (c) ? c : u);
                    if (c) free(c);
                    free(u);
                    p      += length;
                    w      /= 2;
                    h      /= 2;
                }
            } else {
                /* compressed */
                for (w = t->width, h = t->height, i = 0; i < t->mlevels; ++i) {
                    w       = MAX(w, 4);
                    h       = MAX(h, 4);
                    length  = (w / 4) * (h / 4) * t->cf->block_size;
                    glCompressedTexImage2D(gl_target, i, t->cf->gl_internal_format, w, h, 0, length, p);
                    p      += length;
                    w      /= 2;
                    h      /= 2;
                }
            }
        } while (b->gl_target == GL_TEXTURE_CUBE_MAP && ++j < ARRAY_SIZE(nv2a_texture_format_cubemap_map));
        break;
    case GL_TEXTURE_3D:
        if (t->cf->gl_format) {
            if (t->cf->linear) INT3;
            for (w = t->width, h = t->height, d = t->depth, i = 0; i < t->mlevels; ++i) {
                w       = MAX(w, 1);
                h       = MAX(h, 1);
                d       = MAX(d, 1);
                pitch   = w * t->cf->pixel_size;
                pslice  = h * pitch;
                length  = d * pslice;
                if (!(u = malloc(length))) INT3;
                unswizzle_box(p, w, h, d, u, pitch, pslice, t->cf->pixel_size);
                c       = nv2a_texture_convert(t, u, t->pdata, w, h, d, pitch, pslice);
                glTexImage3D(gl_target, i, t->cf->gl_internal_format, w, h, d, 0, t->cf->gl_format, t->cf->gl_type, (c) ? c : u);
                if (c) free(c);
                free(u);
                p      += length;
                w      /= 2;
                h      /= 2;
                d      /= 2;
            }
        } else {
            PRINTF_NV2A(XEXEC_DBG_ALL, "/* FIXME: compressed 3d texture not supported yet */", 0);
            INT3;
        }
        break;
    default:
        INT3;
        break;
    }

    LEAVE_NV2A;
}

static nv2a_texture_binding_t *
nv2a_texture_binding_create(nv2a_texture_t *t) {
    register nv2a_texture_binding_t *b;
    ENTER_NV2A;

    if (!(b = calloc(1, sizeof(*b)))) INT3;
    b->ref = 1;
    glGenTextures(1, &b->gl_texture);

    if (t->format.cubemap) {
        if (t->cf->linear) INT3;
        if (t->format.dimensionality != 2) INT3;
        b->gl_target = GL_TEXTURE_CUBE_MAP;
    } else if (t->cf->linear) {
        /* linear textures use unnormalised texcoords.
         * GL_TEXTURE_RECTANGLE_ARB conveniently also does, but
         * does not allow repeat and mirror wrap modes.
         *  (or mipmapping, but xbox d3d says 'Non swizzled and non
         *   compressed textures cannot be mip mapped.')
         * Not sure if that'll be an issue. */
        /* FIXME: GLSL 330 provides us with textureSize()! Use that? */
        if (t->format.dimensionality != 2) INT3;
        b->gl_target = GL_TEXTURE_RECTANGLE;
    } else {
        switch (t->format.dimensionality) {
        case 1: b->gl_target = GL_TEXTURE_1D; break;
        case 2: b->gl_target = GL_TEXTURE_2D; break;
        case 3: b->gl_target = GL_TEXTURE_3D; break;
        default:
            INT3;
            break;
        }
    }

    glBindTexture(b->gl_target, b->gl_texture);

    PRINTF_NV2A(
        XEXEC_DBG_VARDUMP,
        "texture: index %zu | "
        "%u dimensions%s, "
        "w=%u x h=%u x d=%u | "
        "format: '%s%s' (%i), "
        "bpp=%u",
        t->index,
        t->format.dimensionality,
        (t->format.cubemap) ? " (cubemap)" : "",
        t->width,
        t->height,
        t->depth,
        t->cf->name,
        (!t->cf->linear) ? " (sz)" : "",
        t->cf->index,
        t->cf->pixel_size * 8);

    nv2a_texture_binding_upload(b, t);

    /* linear textures do not support mipmapping */
    if (!t->cf->linear) {
        glTexParameteri(b->gl_target, GL_TEXTURE_BASE_LEVEL, t->ctl0.min_lod_clamp);
        glTexParameteri(b->gl_target, GL_TEXTURE_MAX_LEVEL, t->mlevels - 1);
    }
    if (t->cf->gl_swizzle_mask[0] ||
        t->cf->gl_swizzle_mask[1] ||
        t->cf->gl_swizzle_mask[2] ||
        t->cf->gl_swizzle_mask[3]) {
        glTexParameteriv(b->gl_target, GL_TEXTURE_SWIZZLE_RGBA, t->cf->gl_swizzle_mask);
    }

    LEAVE_NV2A;
    return b;
}

static int
nv2a_texture_binding_destroy(nv2a_texture_binding_t **b, int now) {
    register int ret = 0;
    ENTER_NV2A;

    if (b && *b) {
        if (!now && (*b)->ref <= 0) INT3;
        if (now || !--(*b)->ref) {
            glDeleteTextures(1, &(*b)->gl_texture);
            free(*b);
            *b = NULL;
            ret = 1;
        }
    }

    LEAVE_NV2A;
    return ret;
}

static void
nv2a_texture_bind(void *p, uint32_t index) {
    register nv2a_texture_t *t;
    nv2a_dma_t dma;
    uint32_t bsizeu;
    uint32_t bsizev;
    uint32_t bsizep;
    GLfloat gl_border_color[4];
    register uint32_t pos;
    register uint32_t w;
    register uint32_t h;
    register uint32_t i;
    ENTER_NV2A;

    if (index >= NV2A_MAX_TEXTURES) INT3;
    t = &nv2a_ctx->t[index];

    do {
        glActiveTexture(GL_TEXTURE0 + index);
        if (t->b && !t->dirty) {
            glBindTexture(t->b->gl_target, t->b->gl_texture);
            break;
        }

        pos               = index * 64;
        t->index          = index;
        t->toffset        = NV2A_REG32(p + pos, NV_097, SET_TEXTURE_OFFSET);
        t->format.field   = NV2A_REG32(p + pos, NV_097, SET_TEXTURE_FORMAT);
        t->address.field  = NV2A_REG32(p + pos, NV_097, SET_TEXTURE_ADDRESS);
        t->ctl0.field     = NV2A_REG32(p + pos, NV_097, SET_TEXTURE_CONTROL0);
        t->pitch          = NV2A_REG32_MASK_BITSHIFT_GET(p + pos, NV_097, SET_TEXTURE_CONTROL1, IMAGE_PITCH);
        t->filter.field   = NV2A_REG32(p + pos, NV_097, SET_TEXTURE_FILTER);
        t->width          = NV2A_REG32_MASK_BITSHIFT_GET(p + pos, NV_097, SET_TEXTURE_IMAGE_RECT, WIDTH);
        t->height         = NV2A_REG32_MASK_BITSHIFT_GET(p + pos, NV_097, SET_TEXTURE_IMAGE_RECT, HEIGHT);
        t->palette.field  = NV2A_REG32(p + pos, NV_097, SET_TEXTURE_PALETTE);
        t->bcolor.field   = NV2A_REG32(p + pos, NV_097, SET_TEXTURE_BORDER_COLOR);
        t->poffset        = t->palette.offset << 6;

        if (t->format.color >= ARRAY_SIZE(nv2a_texture_color_format)) INT3;
        t->cf = &nv2a_texture_color_format[t->format.color];
        if (!t->cf->name) INT3;

        PRINTF_NV2A(
            XEXEC_DBG_VARDUMP,
            "texture: index %zu | "
            "%u dimensions%s, "
            "pitch=%u, "
            "w=%u (u=%u) x h=%u (v=%u) x d=%u (p=%u) | "
            "format: '%s%s' (%i), "
            "bpp=%u",
            t->index,
            t->format.dimensionality,
            (t->format.cubemap) ? " (cubemap)" : "",
            t->pitch,
            t->width,
            1 << t->format.base_size_u,
            t->height,
            1 << t->format.base_size_v,
            t->depth,
            1 << t->format.base_size_p,
            t->cf->name,
            t->cf->index,
            (!t->cf->linear) ? " (sz)" : "",
            t->cf->pixel_size * 8);

        if (!t->ctl0.enable) {
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            glBindTexture(GL_TEXTURE_RECTANGLE, 0);
            glBindTexture(GL_TEXTURE_1D, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindTexture(GL_TEXTURE_3D, 0);
            break;
        }
        /* FIXME: check for unsupported features */
        if (t->filter.asigned_enabled ||
            t->filter.rsigned_enabled ||
            t->filter.gsigned_enabled ||
            t->filter.bsigned_enabled) INT3;
        if (t->cf->linear) {
            if (t->format.dimensionality != 2) INT3;
            t->depth   = 1;
        } else {
            bsizeu     = t->format.base_size_u;
            bsizev     = t->format.base_size_v;
            bsizep     = t->format.base_size_p;
            t->width   = 1 << bsizeu;
            t->height  = 1 << bsizev;
            t->depth   = 1 << bsizep;
            t->mlevels = t->format.mipmap_levels;
            i          = t->ctl0.max_lod_clamp + 1;
            t->mlevels = MIN(t->mlevels, i);//FIXME: What about 3D mipmaps?
            if (t->cf->gl_format) {
                /* Discard mipmap levels that would be smaller than 1x1.
                 * FIXME: Is this actually needed?
                 *
                 * >> Level 0: 32 x 4
                 *    Level 1: 16 x 2
                 *    Level 2: 8 x 1
                 *    Level 3: 4 x 1
                 *    Level 4: 2 x 1
                 *    Level 5: 1 x 1
                 */
                i          = MAX(bsizeu, bsizev) + 1;
                t->mlevels = MIN(t->mlevels, i);
            } else {
                /* OpenGL requires DXT textures to always have a width and
                 * height a multiple of 4. The Xbox and DirectX handles DXT
                 * textures smaller than 4 by padding the rest of the block.
                 *
                 * See:
                 * https://msdn.microsoft.com/en-us/library/windows/desktop/bb204843(v=vs.85).aspx
                 * https://msdn.microsoft.com/en-us/library/windows/desktop/bb694531%28v=vs.85%29.aspx#Virtual_Size
                 *
                 * Work around this for now by discarding mipmap levels that
                 * would result in too-small textures. A correct solution
                 * will be to decompress these levels manually, or add texture
                 * sampling logic.
                 *
                 * >> Level 0: 64 x 8
                 *    Level 1: 32 x 4
                 *    Level 2: 16 x 2 << Ignored
                 * >> Level 0: 16 x 16
                 *    Level 1: 8 x 8
                 *    Level 2: 4 x 4 << OK!
                 */
                if (bsizeu < 2 || bsizev < 2) {
                    /* Base level is smaller than 4x4... */
                    t->mlevels = 1;
                } else {
                    i          = MIN(bsizeu, bsizev) - 1;
                    t->mlevels = MIN(t->mlevels, i);
                }
            }
            VARDUMP_NV2A(DUMP, t->mlevels);
            if (!t->mlevels) INT3;
        }
        t->pdata = t->tdata = NV2A_REGADDR(p, NV_PRAMIN, BASE);
        switch (t->format.context_dma) {
        case NV_097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A /* 0 */:
            t->tdata += NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, CONTEXTDMAA, V);
            break;
        case NV_097_SET_TEXTURE_FORMAT_CONTEXT_DMA_B /* 1 */:
            t->tdata += NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, CONTEXTDMAB, V);
            break;
        default:
            INT3;
            break;
        }
        dma = NV2A_DMA(t->tdata);
        if (t->toffset > dma.limit) INT3;
        t->tdata = (void *)(NV2A_DMA_ADDRESS(&dma) + t->toffset);
        VARDUMP_NV2A(DUMP, t->tdata);
        switch (t->palette.context_dma) {
        case NV_097_SET_TEXTURE_PALETTE_CONTEXT_DMA_A /* 0 */:
            t->pdata += NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, CONTEXTDMAA, V);
            break;
        case NV_097_SET_TEXTURE_PALETTE_CONTEXT_DMA_B /* 1 */:
            t->pdata += NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, CONTEXTDMAB, V);
            break;
        default:
            INT3;
            break;
        }
        dma = NV2A_DMA(t->pdata);
        if (t->poffset > dma.limit) INT3;
        t->pdata = (void *)(NV2A_DMA_ADDRESS(&dma) + t->poffset);
        VARDUMP_NV2A(DUMP, t->pdata);
        t->tlen = 0;
        if (t->cf->linear) {
            if (t->format.cubemap) INT3;
            if (t->format.dimensionality != 2) INT3;
            t->tlen = t->height * t->pitch;
        } else if (t->format.dimensionality >= 2) {
            if (t->cf->gl_format) {
                for (w = t->width, h = t->height, i = 0; i < t->mlevels; ++i) {
                    w        = MAX(w, 1);
                    h        = MAX(h, 1);
                    t->tlen += w * h * t->cf->pixel_size;
                    w       /= 2;
                    h       /= 2;
                }
            } else {
                /* compressed */
                for (w = t->width, h = t->height, i = 0; i < t->mlevels; ++i) {
                    w        = MAX(w, 4);
                    h        = MAX(h, 4);
                    t->tlen += (w / 4) * (h / 4) * t->cf->block_size;
                    w       /= 2;
                    h       /= 2;
                }
            }
            if (t->format.cubemap) {
                if (t->format.dimensionality != 2) INT3;
                t->tlen *= 6;
            }
            if (t->format.dimensionality >= 3) {
                t->tlen *= t->depth;
            }
        }
        VARDUMP_NV2A(DUMP, t->tlen);
        switch (t->palette.length) {
        case NV_097_SET_TEXTURE_PALETTE_LENGTH_256 /* 0 */:
            t->plen = 256;
            break;
        case NV_097_SET_TEXTURE_PALETTE_LENGTH_128 /* 1 */:
            t->plen = 128;
            break;
        case NV_097_SET_TEXTURE_PALETTE_LENGTH_64 /* 2 */:
            t->plen = 64;
            break;
        case NV_097_SET_TEXTURE_PALETTE_LENGTH_32 /* 3 */:
            t->plen = 32;
            break;
        default:
            INT3;
            break;
        }
        VARDUMP_NV2A(DUMP, t->plen);
#if 1 // no cache
        nv2a_texture_binding_destroy(&t->b, 0);
        if (!(t->b = nv2a_texture_binding_create(t))) INT3;
#elif 0 //TODO cache
        if (!(t->b = nv2a_texture_binding_ret(t))) INT3;//TODO here; do dirty page track
        ++t->b->ref;

MEM_DIRTY_CREATE(t->tdata, t->tlen);//TODO cache
MEM_DIRTY_CREATE(t->pdata, t->plen);//TODO cache
#elif 0
        TextureShape state = {
            .cubemap = cubemap,
            .dimensionality = dimensionality,
            .color_format = color_format,
            .levels = levels,
            .width = width,
            .height = height,
            .depth = depth,
            .min_mipmap_level = min_mipmap_level,
            .max_mipmap_level = max_mipmap_level,
            .pitch = pitch,
        };
# ifdef USE_TEXTURE_CACHE
        TextureKey key = {
            .state = state,
            .data_hash = fast_hash(t->tdata, t->tlen, 5003)
                            ^ fnv_hash(t->pdata, t->plen),
            .texture_data = t->tdata,
            .palette_data = t->pdata,
        };
        gpointer cache_key = g_malloc(sizeof(key));
        *((typeof(key) *)cache_key) = key;
        t->b = g_lru_cache_get(pg->texture_cache, cache_key);
        if (!t->b) INT3;
        ++t->b->ref;
# else
        t->b = generate_texture(state, t->tdata, t->pdata);
        if (!t->b) INT3;
# endif
#endif
        glBindTexture(t->b->gl_target, t->b->gl_texture);
        if (t->cf->linear) {
            /* FIXME: sometimes games try to set mipmap min filters on linear textures. this could indicate a bug... */
            switch (t->filter.min) {
            case NV_097_SET_TEXTURE_FILTER_MIN_BOX_NEARESTLOD /* 3 */:
            case NV_097_SET_TEXTURE_FILTER_MIN_BOX_TENT_LOD /* 5 */:
                t->filter.min = NV_097_SET_TEXTURE_FILTER_MIN_BOX_LOD0 /* 1 */;
                break;
            case NV_097_SET_TEXTURE_FILTER_MIN_TENT_NEARESTLOD /* 4 */:
            case NV_097_SET_TEXTURE_FILTER_MIN_TENT_TENT_LOD /* 6 */:
                t->filter.min = NV_097_SET_TEXTURE_FILTER_MIN_TENT_LOD0 /* 2 */;
                break;
            }
        }
        if (t->filter.min >= ARRAY_SIZE(nv2a_texture_filter_min_map)) INT3;
        glTexParameteri(t->b->gl_target, GL_TEXTURE_MIN_FILTER, nv2a_texture_filter_min_map[t->filter.min]);
        if (t->filter.mag >= ARRAY_SIZE(nv2a_texture_filter_mag_map)) INT3;
        glTexParameteri(t->b->gl_target, GL_TEXTURE_MAG_FILTER, nv2a_texture_filter_mag_map[t->filter.mag]);
        /* texture wrapping */
        if (t->address.u >= ARRAY_SIZE(nv2a_texture_wrap_map)) INT3;
        glTexParameteri(t->b->gl_target, GL_TEXTURE_WRAP_S, nv2a_texture_wrap_map[t->address.u]);
        if (t->format.dimensionality > 1) {
            if (t->address.v >= ARRAY_SIZE(nv2a_texture_wrap_map)) INT3;
            glTexParameteri(t->b->gl_target, GL_TEXTURE_WRAP_T, nv2a_texture_wrap_map[t->address.v]);
        }
        if (t->format.dimensionality > 2) {
            if (t->address.p >= ARRAY_SIZE(nv2a_texture_wrap_map)) INT3;
            glTexParameteri(t->b->gl_target, GL_TEXTURE_WRAP_R, nv2a_texture_wrap_map[t->address.p]);
        }
        /* FIXME: Only upload if necessary? [s, t or r = GL_CLAMP_TO_BORDER] */
        if (t->format.border_source == NV_PGRAPH_TEXFMT0_BORDER_SOURCE_COLOR) {
            /* FIXME: Color channels might be wrong order */
            gl_border_color[0] = t->bcolor.r / 255.f;
            gl_border_color[1] = t->bcolor.g / 255.f;
            gl_border_color[2] = t->bcolor.b / 255.f;
            gl_border_color[3] = t->bcolor.a / 255.f;
            glTexParameterfv(t->b->gl_target, GL_TEXTURE_BORDER_COLOR, gl_border_color);
        }
    } while ((t->dirty = 0));

    LEAVE_NV2A;
}

static void
nv2a_texture_bind_all(void *p) {
    register uint32_t i;
    ENTER_NV2A;

    for (i = 0; i < NV2A_MAX_TEXTURES; nv2a_texture_bind(p, i), ++i);

    LEAVE_NV2A;
}
#if 0
static int
nv2a_pgraph_ctx_lookup(void *p, uint32_t grclass, nv2a_pgraph_ctx *ctx) {
    register uint32_t i;
    register uint32_t pos;
    nv2a_pgraph_ctx C;
    register int ret = 1;
    ENTER_NV2A;

    for (i = 0; i < NV2A_MAX_SUBCHANNELS; ++i) {
        pos    = NV_PGRAPH_CTX_CACHE__OFFSET(i);
        C.ctx1 = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE1);
        C.ctx2 = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE2);
        C.ctx3 = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE3);
        C.ctx4 = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE4);
        C.ctx5 = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE5);
        if (C.ctx1->grclass == grclass) {
            *ctx = C;
            ret  = 0;
            break;
        }
    }

    LEAVE_NV2A;
    return ret;
}
#endif
static void
nv2a_09f_blit(void *p) {
//    nv2a_pgraph_ctx C;
    register const nv2a_color_format_t *cf;
    nv2a_dma_t dma;
    uint8_t *src, *dest;
    uint32_t srcp, destp, inx, iny, outx, outy, w, h;
    register uint8_t *srcr, *destr;
    register uint32_t addr;
    register uint32_t i;
    ENTER_NV2A;

    switch (NV2A_REG32(p, NV_09F, SET_OPERATION)) {
    case NV_09F_SET_OPERATION_MODE_SRCCOPY /* 3 */:
//        if (nv2a_pgraph_ctx_lookup(p, NV10_CONTEXT_SURFACES_2D, &C)) INT3;

        i = NV2A_REG32(p, NV_062, COLOR_FORMAT);
        if (i >= ARRAY_SIZE(nv2a_062_color_format)) INT3;
        cf = &nv2a_062_color_format[i];
        if (!cf->name) INT3;

        src = dest = NV2A_REGADDR(p, NV_PRAMIN, BASE);

        src  += NV2A_REG32(p, NV_062, SET_CONTEXT_DMA_IMAGE_SOURCE);
        dma   = NV2A_DMA(src);
        addr  = NV2A_REG32(p, NV_062, OFFSET_SOURCE);
        if (addr > dma.limit) INT3;
        addr += NV2A_DMA_ADDRESS(&dma);
        src   = (typeof(src))addr;
        dest += NV2A_REG32(p, NV_062, SET_CONTEXT_DMA_IMAGE_DESTIN);
        dma   = NV2A_DMA(dest);
        addr  = NV2A_REG32(p, NV_062, OFFSET_DESTIN);
        if (addr > dma.limit) INT3;
        addr += NV2A_DMA_ADDRESS(&dma);
        dest  = (typeof(dest))addr;

        srcp  = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_062, PITCH, SOURCE);
        destp = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_062, PITCH, DESTIN);
        inx   = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_09F, POINT_IN, X);
        iny   = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_09F, POINT_IN, Y);
        outx  = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_09F, POINT_OUT, X);
        outy  = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_09F, POINT_OUT, Y);
        w     = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_09F, SIZE, WIDTH);
        h     = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_09F, SIZE, HEIGHT);

        for (i = 0; i < h; ++i) {
            srcr  = src  + (iny  + i) * srcp  + inx  * cf->pixel_size;
            destr = dest + (outy + i) * destp + outx * cf->pixel_size;
            memmove(destr, srcr, w * cf->pixel_size);
        }
        break;
    default:
        INT3;
        break;
    }

    LEAVE_NV2A;
}

static nv2a_vertex_attrib_t *
nv2a_vertex_attrib_inline_buffer_start(uint32_t index) {
    register nv2a_vertex_attrib_t *a;
    register uint32_t i;
    ENTER_NV2A;

    if (index >= NV2A_MAX_VERTEX_ATTRIBS) INT3;
    a = &nv2a_ctx->va[index];

    if (!a->inline_buffer_len && nv2a_ctx->inline_buffer_pos) {
        for (i = 0; i < nv2a_ctx->inline_buffer_pos; ++i) {
            /* upload the previous inline value */
            memcpy(
                a->inline_buffer[i],
                a->inline_value,
                sizeof(a->inline_value));
        }
        a->inline_buffer_len = i;
    }

    LEAVE_NV2A;
    return a;
}

static void
nv2a_vertex_attrib_inline_buffer_finish(void) {
    register nv2a_vertex_attrib_t *a;
    register uint32_t pos;
    register uint32_t i;
    ENTER_NV2A;

    if ((pos = ++nv2a_ctx->inline_buffer_pos) >= NV2A_MAX_BATCH_LENGTH) INT3;

    for (i = 0; i < NV2A_MAX_VERTEX_ATTRIBS; ++i) {
        if ((a = &nv2a_ctx->va[i])->inline_buffer_len) {
            memcpy(
                a->inline_buffer[pos - 1],
                a->inline_value,
                sizeof(a->inline_value));
            a->inline_buffer_len = pos;
        }
    }

    LEAVE_NV2A;
}

static void
nv2a_vertex_attrib_bind(void *p, uint32_t count, uint32_t inline_array_stride) {
    register nv2a_vertex_attrib_t *a;
//    void *vdata;
    nv2a_dma_t dma;
    register uint32_t stride;
    register uint32_t i;
    register uint32_t j;
    ENTER_NV2A;
    VARDUMP_NV2A(VAR_IN, count);
    VARDUMP_NV2A(VAR_IN, inline_array_stride);

    for (i = 0; i < NV2A_MAX_VERTEX_ATTRIBS; ++i) {
        if ((a = &nv2a_ctx->va[i])->format.size) {
#if 0
renamed:
num_elements      -> count
attribute->count  -> a->format.size
data              -> a->vdata
in_stride         -> a->vstride
attribute->stride -> a->format.stride

//    for (i=0; i<NV2A_VERTEXSHADER_ATTRIBUTES; i++) {
//        VertexAttribute *attribute = &pg->vertex_attributes[i];
//        if (attribute->count) {
//            uint8_t *data;
//            unsigned int in_stride;
//            if (use_inline_array && attribute->needs_conversion) {
//                data = (uint8_t*)pg->inline_array
//                        + attribute->inline_array_offset;
//                in_stride = inline_array_stride;
//            } else {
//                hwaddr dma_len;
//                if (attribute->dma_select) {
//                    data = nv_dma_map(d, pg->dma_vertex_b, &dma_len);
//                } else {
//                    data = nv_dma_map(d, pg->dma_vertex_a, &dma_len);
//                }
//                assert(attribute->offset < dma_len);
//                data += attribute->offset;
//                in_stride = attribute->stride;
//            }
#endif
            if (inline_array_stride && a->f->convert) {
                a->vdata    = nv2a_ctx->inline_array;
                a->vdata   += a->inline_array_offset;
                a->vstride  = inline_array_stride;
            } else {
                a->vdata = NV2A_REGADDR(p, NV_PRAMIN, BASE);
                switch (a->vertex.context_dma) {
                case NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_CONTEXT_DMA_VERTEX_A /* 0 */:
                    a->vdata += NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, CONTEXTVTXA, V);
                    break;
                case NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_CONTEXT_DMA_VERTEX_B /* 1 */:
                    a->vdata += NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, CONTEXTVTXB, V);
                    break;
                default:
                    INT3;
                    break;
                }
                dma = NV2A_DMA(a->vdata);
                if (a->vertex.offset > dma.limit) INT3;
                a->vdata   = (void *)(NV2A_DMA_ADDRESS(&dma) + a->vertex.offset);
                a->vstride = a->format.stride;
            }
            VARDUMP_NV2A(DUMP, a->vdata);
            VARDUMP_NV2A(DUMP, a->vstride);
            if (a->f->convert) {
                stride = a->format.size * a->f->convert_nmemb * a->f->convert_size;
                if (count > a->converted_count) {
                    if (!(a->converted_buffer = realloc(a->converted_buffer, stride * count))) INT3;
                }
                for (j = a->converted_count; j < count; ++j) {
                    switch (a->format.type) {
                    case NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_CMP /* 6 */:
                        {
                            register int32_t *in = a->vdata + a->vstride * j;
                            register float *out  = a->converted_buffer + stride * j;
                            out[0] = ((*in >>  0) & 0x7ff) / 1023.f;
                            out[1] = ((*in >> 11) & 0x7ff) / 1023.f;
                            out[2] = ((*in >> 22) & 0x3ff) / 511.f;
                        }
                        break;
                    default:
                        INT3;
                        break;
                    }
                }
                glBindBuffer(GL_ARRAY_BUFFER, a->gl_converted_buffer);
                if (a->converted_count != count) {
                    a->converted_count = count;
                    glBufferData(GL_ARRAY_BUFFER,
                        stride * count,
                        a->converted_buffer,
                        GL_DYNAMIC_DRAW);
                }
                glVertexAttribPointer(i,
                    a->format.size * a->f->convert_nmemb,
                    a->f->gl_type,
                    a->f->gl_normalized,
                    stride,
                    NULL);
#if 0
renamed:
attribute->format             -> a->format.type
num_elements                  -> count
attribute->converted_elements -> a->converted_count
attribute->converted_size     -> a->f->convert_size

//                unsigned int out_stride = attribute->converted_size
//                                        * attribute->converted_count;
//                if (num_elements > attribute->converted_elements) {
//                    attribute->converted_buffer = g_realloc(
//                        attribute->converted_buffer,
//                        num_elements * out_stride);
//                }
//                for (j=attribute->converted_elements; j<num_elements; j++) {
//                    uint8_t *in = data + j * in_stride;
//                    uint8_t *out = attribute->converted_buffer + j * out_stride;
//                    switch (attribute->format) {
//                    case NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_CMP: {
//                        uint32_t p = ldl_le_p((uint32_t*)in);
//                        float *xyz = (float*)out;
//                        xyz[0] = ((int32_t)(((p >>  0) & 0x7FF) << 21) >> 21)
//                                                                      / 1023.0f;
//                        xyz[1] = ((int32_t)(((p >> 11) & 0x7FF) << 21) >> 21)
//                                                                      / 1023.0f;
//                        xyz[2] = ((int32_t)(((p >> 22) & 0x3FF) << 22) >> 22)
//                                                                       / 511.0f;
//                        break;
//                    }
//                    default:
//                        assert(false);
//                        break;
//                    }
//                }
//                glBindBuffer(GL_ARRAY_BUFFER, attribute->gl_converted_buffer);
//                if (num_elements != attribute->converted_elements) {
//                    glBufferData(GL_ARRAY_BUFFER,
//                                 num_elements * out_stride,
//                                 attribute->converted_buffer,
//                                 GL_DYNAMIC_DRAW);
//                    attribute->converted_elements = num_elements;
//                }
//                glVertexAttribPointer(i,
//                    attribute->converted_count,
//                    attribute->gl_type,
//                    attribute->gl_normalize,
//                    out_stride,
//                    0);
#endif
            } else if (inline_array_stride) {
#if 0
renamed:
attribute->gl_count -> a->f->gl_size
attribute->gl_count == attribute->count == (a->f->gl_size) ? a->f->gl_size : a->format.size

//                glBindBuffer(GL_ARRAY_BUFFER, pg->gl_inline_array_buffer);
//                glVertexAttribPointer(i,
//                                      attribute->gl_count,
//                                      attribute->gl_type,
//                                      attribute->gl_normalize,
//                                      inline_array_stride,
//                                      (void*)(uintptr_t)attribute->inline_array_offset);
#endif
                glBindBuffer(GL_ARRAY_BUFFER, nv2a_ctx->gl_inline_array_buffer);
                glVertexAttribPointer(i,
                    (a->f->gl_size) ? a->f->gl_size : a->format.size,
                    a->f->gl_type,
                    a->f->gl_normalized,
                    inline_array_stride,
                    (void *)a->inline_array_offset);
            } else {
#if 0
//                hwaddr addr = data - d->vram_ptr;
//                pgraph_update_memory_buffer(d, addr,
//                                            num_elements * attribute->stride,
//                                            false);
//                glVertexAttribPointer(i,
//                    attribute->gl_count,
//                    attribute->gl_type,
//                    attribute->gl_normalize,
//                    attribute->stride,
//                    (void*)addr);
#elif 0 //TODO
                hwaddr addr = a->vdata - d->vram_ptr;
                pgraph_update_memory_buffer(d, addr, a->format.stride * count, false);//glBindBuffer(GL_ARRAY_BUFFER)
                glVertexAttribPointer(i,
                    (a->f->gl_size) ? a->f->gl_size : a->format.size,
                    a->f->gl_type,
                    a->f->gl_normalized,
                    a->format.stride,
                    (void *)addr);
#else
INT3;//XXX
#endif
            }
            glEnableVertexAttribArray(i);
        } else {
            glDisableVertexAttribArray(i);
            glVertexAttrib4fv(i, a->inline_value);
        }
    }

    LEAVE_NV2A;
}
#if 0
renamed:
attribute->count -> a->format.size
attribute->size  -> a->f->size
vertex_size      == stride (offset)

//static unsigned int pgraph_bind_inline_array(NV2AState *d)
//{
//    int i;
//    PGRAPHState *pg = &d->pgraph;
//    unsigned int offset = 0;
//    for (i=0; i<NV2A_VERTEXSHADER_ATTRIBUTES; i++) {
//        VertexAttribute *attribute = &pg->vertex_attributes[i];
//        if (attribute->count) {
//            attribute->inline_array_offset = offset;
//            NV2A_DPRINTF("bind inline attribute %d size=%d, count=%d\n",
//                i, attribute->size, attribute->count);
//            offset += attribute->size * attribute->count;
//            assert(offset % 4 == 0);
//        }
//    }
//    unsigned int vertex_size = offset;
//    unsigned int index_count = pg->inline_array_length*4 / vertex_size;
//    NV2A_DPRINTF("draw inline array %d, %d\n", vertex_size, index_count);
//    glBindBuffer(GL_ARRAY_BUFFER, pg->gl_inline_array_buffer);
//    glBufferData(GL_ARRAY_BUFFER, pg->inline_array_length*4, pg->inline_array,
//                 GL_DYNAMIC_DRAW);
//    pgraph_bind_vertex_attributes(d, index_count, true, vertex_size);
//    return index_count;
//}
#endif
static uint32_t
nv2a_inline_array_bind(void *p) {
    register nv2a_vertex_attrib_t *a;
    register uint32_t stride;
    register uint32_t size;
    register uint32_t count;
    register uint32_t i;
    ENTER_NV2A;

    for (stride = 0, i = 0; i < NV2A_MAX_VERTEX_ATTRIBS; ++i) {
        if ((a = &nv2a_ctx->va[i])->format.size) {
            a->inline_array_offset = stride;

            PRINTF_NV2A(
                XEXEC_DBG_VARDUMP,
                "inline array: bind: index %u | "
                "format: '%s' (%i), "
                "stride=0x%.08x (%u) += "
                "size=%u x count=%u = %u bytes",
                i,
                a->f->name,
                a->f->index,
                stride,
                stride,
                a->f->size,
                a->format.size,
                a->f->size * a->format.size);

            stride += a->f->size * a->format.size;
            if (stride % 4) INT3;
        }
    }

    if (!stride) INT3;

    size  = nv2a_ctx->inline_array_len * sizeof(*nv2a_ctx->inline_array);
    count = size / stride;

    PRINTF_NV2A(
        XEXEC_DBG_VARDUMP,
        "inline array: draw: "
        "count=%u = size=%u / stride=%u",
        count,
        size,
        stride);

    glBindBuffer(GL_ARRAY_BUFFER, nv2a_ctx->gl_inline_array_buffer);
    glBufferData(GL_ARRAY_BUFFER, size, nv2a_ctx->inline_array, GL_DYNAMIC_DRAW);

    nv2a_vertex_attrib_bind(p, count, stride);

    LEAVE_NV2A;
    return count;
}

static void
nv2a_pgraph_fifo(void *p, const nv2a_pfifo_command_t *c) {
    register nv2a_vertex_attrib_t *a;
    nv2a_pgraph_ctx1_t *ctx1;
    nv2a_pgraph_ctx2_t *ctx2;
    nv2a_pgraph_ctx3_t *ctx3;
    nv2a_pgraph_ctx4_t *ctx4;
    nv2a_pgraph_ctx5_t *ctx5;
    nv2a_dma_t dma;
    register uint32_t pos;
    register uint32_t *get;
    register uint32_t v;
    register uint32_t t;
    ENTER_NV2A;

    NV2A_IRQ_WAITN(PGRAPH, FIFO, ACCESS_ENABLED);
//    if (NV2A_REG32_MASK_BITSHIFT_TEST(p, NV_PGRAPH, CTX_CONTROL, CHID, INVALID)) INT3;//FIXME will be invalid

    VARDUMP_NV2A(VAR_IN, c->subchannel);

    pos  = NV_PGRAPH_CTX_CACHE__OFFSET(c->subchannel);
    ctx1 = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE1);
    ctx2 = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE2);
    ctx3 = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE3);
    ctx4 = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE4);
    ctx5 = NV2A_REGADDR(p + pos, NV_PGRAPH, CTX_CACHE5);
//fprintf(stderr,"pgraph: method:0x%x | subchannel:%u | param:0x%x | grclass:0x%x\n",c->method,c->subchannel,c->param,ctx1->grclass);//XXX
    if (c->method == /* NV_SET_OBJECT */ 0) {
        VARDUMP_NV2A(VAR_IN, c->method);
        VARDUMP_NV2A(VAR_IN, c->param);
        pos = c->param;
        if (pos + 20 > NV2A_BLOCK_SIZE(NV_PRAMIN) || pos % 4) INT3;
        get = NV2A_REGADDR(p + pos, NV_PRAMIN, BASE);
        NV2A_REG32(p, NV_PGRAPH, CTX_SWITCH1) = ctx1->field = *get++;
        NV2A_REG32(p, NV_PGRAPH, CTX_SWITCH2) = ctx2->field = *get++;
        NV2A_REG32(p, NV_PGRAPH, CTX_SWITCH3) = ctx3->field = *get++;
        NV2A_REG32(p, NV_PGRAPH, CTX_SWITCH4) = ctx4->field = *get++;
        NV2A_REG32(p, NV_PGRAPH, CTX_SWITCH5) = ctx5->field = *get;
        VARDUMP3_NV2A(DUMP, ctx1->field,        nv2a_pgraph_ctx1_name);
        VARDUMP2_NV2A(DUMP, ctx1->grclass,      nv2a_pgraph_ctx1_grclass_name);
        VARDUMP2_NV2A(DUMP, ctx1->patch_config, nv2a_pgraph_ctx1_patch_config_name);
        VARDUMP2_NV2A(DUMP, ctx1->dither_mode,  nv2a_pgraph_ctx1_dither_mode_name);
        VARDUMP2_NV2A(DUMP, ctx1->class_type,   nv2a_pgraph_ctx1_class_type_name);
        VARDUMP_NV2A(DUMP,  ctx2->field);
        VARDUMP2_NV2A(DUMP, ctx2->mono_format,  nv2a_pgraph_ctx2_mono_format_name);
        VARDUMP2_NV2A(DUMP, ctx2->color_format, nv2a_pgraph_ctx2_color_format_name);
        VARDUMP_NV2A(DUMP,  ctx2->notify_instance);
        VARDUMP_NV2A(DUMP,  ctx3->field);
        VARDUMP_NV2A(DUMP,  ctx3->dma_instance0);
        VARDUMP_NV2A(DUMP,  ctx3->dma_instance1);
        VARDUMP_NV2A(DUMP,  ctx4->field);
        VARDUMP_NV2A(DUMP,  ctx4->user_instance);
        VARDUMP_NV2A(DUMP,  ctx5->field);
        VARDUMP_NV2A(DUMP,  ctx5->trap_bits);
        LEAVE_NV2A;
        return;
    }

    VARDUMP2_NV2A(DUMP, ctx1->grclass, nv2a_pgraph_ctx1_grclass_name);

    switch (ctx1->grclass) {
    case NV10_CONTEXT_SURFACES_2D /* 0x62 */:
        VARDUMP4_NV2A(VAR_IN, c->method, nv2a_062_name);
        pos = c->method;
        if (pos + 4 > NV2A_BLOCK_SIZE(NV_062) || pos % 4) INT3;
        NV2A_REG32(p + pos, NV_062, NV10_CONTEXT_SURFACES_2D) = c->param;
        switch (c->method) {
        case NV_062_COLOR_FORMAT:
            VARDUMP2_NV2A(VAR_IN, c->param, nv2a_062_color_format_name);
            break;
        default:
            VARDUMP_NV2A(VAR_IN, c->param);
            break;
        }
        break;
    case NV12_IMAGE_BLIT /* 0x9f */:
        VARDUMP4_NV2A(VAR_IN, c->method, nv2a_09f_name);
        pos = c->method;
        if (pos + 4 > NV2A_BLOCK_SIZE(NV_09F) || pos % 4) INT3;
        NV2A_REG32(p + pos, NV_09F, NV12_IMAGE_BLIT) = c->param;
        switch (c->method) {
        case NV_09F_SET_OPERATION:
            VARDUMP2_NV2A(VAR_IN, c->param, nv2a_09f_set_operation_name);
            break;
        case NV_09F_SIZE:
            VARDUMP_NV2A(VAR_IN, c->param);
INT3;//XXX make sure blit works
            nv2a_09f_blit(p);
            break;
        default:
            VARDUMP_NV2A(VAR_IN, c->param);
            break;
        }
        break;
    case NV20_KELVIN_PRIMITIVE /* 0x97 */:
        VARDUMP4_NV2A(VAR_IN, c->method, nv2a_097_name);
        pos = c->method;
        if (pos + 4 > NV2A_BLOCK_SIZE(NV_097) || pos % 4) INT3;
        t = (NV2A_REG32(p + pos, NV_097, NV20_KELVIN_PRIMITIVE) != c->param);
        switch (c->method) {
        case NV_097_WAIT_FOR_IDLE:
        case NV_097_FLIP_STALL:
        case NV_097_SET_CONTEXT_DMA_COLOR:
//            /* try to get any straggling draws in before the surface's changed :/ */
        case NV_097_SET_SURFACE_CLIP_HORIZONTAL:
        case NV_097_SET_SURFACE_CLIP_VERTICAL:
        case NV_097_SET_SURFACE_FORMAT:
        case NV_097_SET_SURFACE_PITCH:
        case NV_097_SET_SURFACE_COLOR_OFFSET:
        case NV_097_SET_SURFACE_ZETA_OFFSET:
        case NV_097_SET_CONTROL0:
        case NV_097_BACK_END_WRITE_SEMAPHORE_RELEASE:
//            pgraph_update_surface(d, false, true, true);//TODO
            break;
        case NV_097_SET_COLOR_MASK:
//        pg->surface_color.write_enabled_cache |= pgraph_color_write_enabled(pg);//TODO
            break;
        case NV_097_SET_DEPTH_MASK:
//        pg->surface_zeta.write_enabled_cache |= pgraph_zeta_write_enabled(pg);//TODO
            break;
        case NV_097_SET_FOG_PARAMS_0:
        case NV_097_SET_FOG_PARAMS_1:
        case NV_097_SET_FOG_PARAMS_2:
//        pg->ltctxa[NV_IGRAPH_XF_LTCTXA_FOG_K][slot] = parameter;//TODO
//        pg->ltctxa_dirty[NV_IGRAPH_XF_LTCTXA_FOG_K] = true;//TODO
            break;
#define CASE(x) \
        case NV_097_SET_TEXTURE_OFFSET_##x: \
        case NV_097_SET_TEXTURE_FORMAT_##x: \
        case NV_097_SET_TEXTURE_ADDRESS_##x: \
        case NV_097_SET_TEXTURE_CONTROL0_##x: \
        case NV_097_SET_TEXTURE_CONTROL1_##x: \
        case NV_097_SET_TEXTURE_FILTER_##x: \
        case NV_097_SET_TEXTURE_IMAGE_RECT_##x: \
        case NV_097_SET_TEXTURE_PALETTE_##x: \
        case NV_097_SET_TEXTURE_BORDER_COLOR_##x: \
            if (t) nv2a_ctx->t[x].dirty = 1; \
            break
        CASE(0);
        CASE(1);
        CASE(2);
        CASE(3);
#undef CASE
        }
        NV2A_REG32(p + pos, NV_097, NV20_KELVIN_PRIMITIVE) = c->param;
        switch (c->method) {
        case NV_097_NO_OPERATION:
            VARDUMP_NV2A(VAR_IN, c->param);
            if (c->param) {
                if (NV2A_REG32_MASK_BITSHIFT_TEST(p, NV_PGRAPH, INTR, ERROR, NOT_PENDING)) {
                    v = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, CTX_USER, CHID);
                    NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, TRAPPED_ADDR, CHID, v);
                    NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, TRAPPED_ADDR, SUBCH, c->subchannel);
                    NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, TRAPPED_ADDR, MTHD, c->method);
                    NV2A_REG32(p, NV_PGRAPH, TRAPPED_DATA_LOW) = c->param;
                    NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PGRAPH, NSOURCE, NOTIFICATION, PENDING);
                    NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PGRAPH, INTR, ERROR, PENDING);
                    IRQ_NV2A_SIGNAL;
                }
                NV2A_IRQ_WAIT(PGRAPH, INTR, ERROR);
            }
            break;
        case NV_097_BACK_END_WRITE_SEMAPHORE_RELEASE:
            VARDUMP_NV2A(VAR_IN, c->param);
            pos  = NV2A_REG32(p, NV_097, SET_CONTEXT_DMA_SEMAPHORE);
            get  = NV2A_REGADDR(p + pos, NV_PRAMIN, BASE);
            dma  = NV2A_DMA(get);
            pos  = NV2A_REG32(p, NV_097, SET_SEMAPHORE_OFFSET);
            if (pos > dma.limit) INT3;
            pos += NV2A_DMA_ADDRESS(&dma);
            xboxkrnl_write(pos, &c->param, 4);
            break;
        case NV_097_FLIP_INCREMENT_WRITE:
            VARDUMP_NV2A(VAR_IN, c->param);
            v = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, SURFACE, WRITE_3D) + 1;
            t = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, SURFACE, MODULO_3D);
            if (t) v %= t;
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, SURFACE, WRITE_3D, v);
            if (glFrameTerminatorGREMEDY) glFrameTerminatorGREMEDY(); //TODO check GL_GREMEDY_frame_terminator
            break;
#if 0 //FIXME signal not implemented
        case NV_097_FLIP_STALL:
            VARDUMP_NV2A(VAR_IN, c->param);
            FLIP_STALL_LOCK;
            while (NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, SURFACE, READ_3D) ==
                NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, SURFACE, WRITE_3D)) FLIP_STALL_WAIT;
            FLIP_STALL_UNLOCK;
            break;
#endif
#if 0 //TODO
        case NV_097_SET_BEGIN_END:
            
            break;
#endif
        case NV_097_ARRAY_ELEMENT16:
            VARDUMP_NV2A(VAR_IN, c->param);
            if (nv2a_ctx->inline_element_len + 2 > ARRAY_SIZE(nv2a_ctx->inline_element)) INT3;
            v = NV2A_MASK_BITSHIFT_GET(c->param, NV_097_ARRAY_ELEMENT16_VERTEX0);
            nv2a_ctx->inline_element[nv2a_ctx->inline_element_len++] = v;
            v = NV2A_MASK_BITSHIFT_GET(c->param, NV_097_ARRAY_ELEMENT16_VERTEX1);
            nv2a_ctx->inline_element[nv2a_ctx->inline_element_len++] = v;
            break;
        case NV_097_ARRAY_ELEMENT32:
            VARDUMP_NV2A(VAR_IN, c->param);
            if (nv2a_ctx->inline_element_len + 1 > ARRAY_SIZE(nv2a_ctx->inline_element)) INT3;
            nv2a_ctx->inline_element[nv2a_ctx->inline_element_len++] = c->param;
            break;
        case NV_097_DRAW_ARRAYS:
            VARDUMP_NV2A(VAR_IN, c->param);
            if (nv2a_ctx->draw_arrays_len + 1 > ARRAY_SIZE(nv2a_ctx->gl_draw_arrays_start)) INT3;
            v = NV2A_MASK_BITSHIFT_GET(c->param, NV_097_DRAW_ARRAYS_START_INDEX);
            t = NV2A_MASK_BITSHIFT_GET(c->param, NV_097_DRAW_ARRAYS_COUNT) + 1;
            nv2a_ctx->draw_arrays_max = MAX(nv2a_ctx->draw_arrays_max, v + t);
            if (nv2a_ctx->draw_arrays_len) {
                /* attempt to connect primitives */
                register uint32_t last_start = nv2a_ctx->gl_draw_arrays_start[nv2a_ctx->draw_arrays_len - 1];
                register GLsizei *last_count = &nv2a_ctx->gl_draw_arrays_count[nv2a_ctx->draw_arrays_len - 1];
                if (v == last_start + *last_count) {
                    *last_count += t;
                    break;
                }
            }
            nv2a_ctx->gl_draw_arrays_start[nv2a_ctx->draw_arrays_len] = v;
            nv2a_ctx->gl_draw_arrays_count[nv2a_ctx->draw_arrays_len] = t;
            ++nv2a_ctx->draw_arrays_len;
            break;
        case NV_097_INLINE_ARRAY:
            VARDUMP_NV2A(VAR_IN, c->param);
            if (nv2a_ctx->inline_array_len + 1 > ARRAY_SIZE(nv2a_ctx->inline_array)) INT3;
            nv2a_ctx->inline_array[nv2a_ctx->inline_array_len++] = c->param;
            break;
#if 0
renamed:
attribute->count              -> a->format.size
attribute->converted_elements -> a->converted_count

//case NV097_SET_VERTEX_DATA_ARRAY_FORMAT ...
//        NV097_SET_VERTEX_DATA_ARRAY_FORMAT + 0x3c: {
//    slot = (method - NV097_SET_VERTEX_DATA_ARRAY_FORMAT) / 4;
//    VertexAttribute *vertex_attribute = &pg->vertex_attributes[slot];
//    vertex_attribute->format =
//        GET_MASK(parameter, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE);
//    vertex_attribute->count =
//        GET_MASK(parameter, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE);
//    vertex_attribute->stride =
//        GET_MASK(parameter, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE);
//    NV2A_DPRINTF("vertex data array format=%d, count=%d, stride=%d\n",
//        vertex_attribute->format,
//        vertex_attribute->count,
//        vertex_attribute->stride);
//    vertex_attribute->gl_count = vertex_attribute->count;
//    if (vertex_attribute->needs_conversion) {
//        vertex_attribute->converted_elements = 0;
//    } else {
//        if (vertex_attribute->converted_buffer) {
//            g_free(vertex_attribute->converted_buffer);
//            vertex_attribute->converted_buffer = NULL;
//        }
//    }
//    break;
//}

#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE                      0xffffff00
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE__BITSHIFT            8
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE                        0x000000f0
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE__BITSHIFT              4
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED               0
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_1                      1
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_2                      2
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_3                      3
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_4                      4
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_3W                     7
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE                        0x0000000f
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE__BITSHIFT              0
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_UB_D3D                 0
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S1                     1
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F                      2
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_UB_OGL                 4
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K                   5
#define NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_CMP                    6
#endif
        case NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_0 ...
             NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_15:
            if ((pos = c->method - NV_097_SET_VERTEX_DATA_ARRAY_OFFSET_0) % 4) INT3;
            pos /= 4;
            VARDUMP_NV2A(VAR_IN, c->param);
            a = &nv2a_ctx->va[pos];
            a->vertex.field = c->param;

            PRINTF_NV2A(
                XEXEC_DBG_VARDUMP,
                "vertex data array: index %u | "
                "offset=0x%.08x (%u), "
                "context_dma=%u",
                pos,
                a->vertex.offset,
                a->vertex.offset,
                a->vertex.context_dma);

            a->converted_count = 0;
            break;
        case NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_0 ...
             NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_15:
            if ((pos = c->method - NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_0) % 4) INT3;
            pos /= 4;
            VARDUMP_NV2A(VAR_IN, c->param);
            a = &nv2a_ctx->va[pos];
            a->format.field = c->param;
            if (a->format.type >= ARRAY_SIZE(nv2a_inline_array_format)) INT3;
            a->f = &nv2a_inline_array_format[a->format.type];
            if (!a->f->name) INT3;

            PRINTF_NV2A(
                XEXEC_DBG_VARDUMP,
                "vertex data array: index %u | "
                "format: '%s' (%i), "
                "count=%u, "
                "stride=%u",
                pos,
                a->f->name,
                a->f->index,
                a->format.size,
                a->format.stride);

            if (a->f->convert) {
                a->converted_count = 0;
            } else if (a->converted_buffer) {
                free(a->converted_buffer);
                a->converted_buffer = NULL;
            }
            switch (a->format.type) {
            case NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_UB_D3D:
                if (a->format.size != NV_097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_4) INT3;
                break;
            }
            break;
        /* parameters are uploaded to the vertex attributes' inline buffer below */
        /* x = vertex method name, y = vertex attribute index, z = inline value index */
#define CASES(y,z) \
            VARDUMP_NV2A(VAR_IN, c->param); \
            a = nv2a_vertex_attrib_inline_buffer_start(y); \
            CASEV(z)
#define CASE(x,y,z) \
        case NV_097_##x##_##z: \
            CASES(y,z) \
            break
#define CASEM(x,y,z) \
        case NV_097_##x##_M_##y##_##z: \
            CASES(y,z) \
            break
#define CASE2(x,y,z) \
        case NV_097_##x##_##z: \
            CASES(y,z) \
            CASE2V \
            break
#define CASE2M(x,y,z) \
        case NV_097_##x##_M_##y##_##z: \
            CASES(y,z) \
            CASE2V \
            break
#define CASEF(x,y,z) \
        case NV_097_##x##_##z: \
            CASES(y,z) \
            nv2a_vertex_attrib_inline_buffer_finish(); \
            break
#define CASEFM(x,y,z) \
        case NV_097_##x##_M_##y##_##z: \
            CASES(y,z) \
            nv2a_vertex_attrib_inline_buffer_finish(); \
            break
#define CASE2F(x,y,z) \
        case NV_097_##x##_##z: \
            CASES(y,z) \
            CASE2V \
            nv2a_vertex_attrib_inline_buffer_finish(); \
            break
#define CASE2FM(x,y,z) \
        case NV_097_##x##_M_##y##_##z: \
            CASES(y,z) \
            CASE2V \
            nv2a_vertex_attrib_inline_buffer_finish(); \
            break
#define CASEV(z) \
            a->inline_value[z] = *(float *)&c->param; \
            VARDUMP_NV2A(FLOAT, REG32(&a->inline_value[z]));
#define CASE2V \
            a->inline_value[3] = 1.f; \
            VARDUMP_NV2A(FLOAT, REG32(&a->inline_value[3]));
        CASE2(SET_VERTEX3F,  0, 0);
        CASE2(SET_VERTEX3F,  0, 1);
        CASE2F(SET_VERTEX3F, 0, 2);
#undef CASE2V
        CASE(SET_VERTEX4F,  0, 0);
        CASE(SET_VERTEX4F,  0, 1);
        CASE(SET_VERTEX4F,  0, 2);
        CASEF(SET_VERTEX4F, 0, 3);
#define CASE2V \
            PRINTF_NV2A(XEXEC_DBG_VARDUMP, "/* FIXME: Should these really be set to 0.0 and 1.0 ? Conditions? */", 0); \
            a->inline_value[2] = 0.f; \
            a->inline_value[3] = 1.f; \
            VARDUMP_NV2A(FLOAT, REG32(&a->inline_value[2])); \
            VARDUMP_NV2A(FLOAT, REG32(&a->inline_value[3]));
        CASE2M(SET_VERTEX_DATA2F,  0, 0);
        CASE2FM(SET_VERTEX_DATA2F, 0, 1);
        CASE2M(SET_VERTEX_DATA2F,  1, 0);
        CASE2M(SET_VERTEX_DATA2F,  1, 1);
        CASE2M(SET_VERTEX_DATA2F,  2, 0);
        CASE2M(SET_VERTEX_DATA2F,  2, 1);
        CASE2M(SET_VERTEX_DATA2F,  3, 0);
        CASE2M(SET_VERTEX_DATA2F,  3, 1);
        CASE2M(SET_VERTEX_DATA2F,  4, 0);
        CASE2M(SET_VERTEX_DATA2F,  4, 1);
        CASE2M(SET_VERTEX_DATA2F,  5, 0);
        CASE2M(SET_VERTEX_DATA2F,  5, 1);
        CASE2M(SET_VERTEX_DATA2F,  6, 0);
        CASE2M(SET_VERTEX_DATA2F,  6, 1);
        CASE2M(SET_VERTEX_DATA2F,  7, 0);
        CASE2M(SET_VERTEX_DATA2F,  7, 1);
        CASE2M(SET_VERTEX_DATA2F,  8, 0);
        CASE2M(SET_VERTEX_DATA2F,  8, 1);
        CASE2M(SET_VERTEX_DATA2F,  9, 0);
        CASE2M(SET_VERTEX_DATA2F,  9, 1);
        CASE2M(SET_VERTEX_DATA2F, 10, 0);
        CASE2M(SET_VERTEX_DATA2F, 10, 1);
        CASE2M(SET_VERTEX_DATA2F, 11, 0);
        CASE2M(SET_VERTEX_DATA2F, 11, 1);
        CASE2M(SET_VERTEX_DATA2F, 12, 0);
        CASE2M(SET_VERTEX_DATA2F, 12, 1);
        CASE2M(SET_VERTEX_DATA2F, 13, 0);
        CASE2M(SET_VERTEX_DATA2F, 13, 1);
        CASE2M(SET_VERTEX_DATA2F, 14, 0);
        CASE2M(SET_VERTEX_DATA2F, 14, 1);
        CASE2M(SET_VERTEX_DATA2F, 15, 0);
        CASE2M(SET_VERTEX_DATA2F, 15, 1);
#undef CASE2V
        CASEM(SET_VERTEX_DATA4F,  0, 0);
        CASEM(SET_VERTEX_DATA4F,  0, 1);
        CASEM(SET_VERTEX_DATA4F,  0, 2);
        CASEFM(SET_VERTEX_DATA4F, 0, 3);
        CASEM(SET_VERTEX_DATA4F,  1, 0);
        CASEM(SET_VERTEX_DATA4F,  1, 1);
        CASEM(SET_VERTEX_DATA4F,  1, 2);
        CASEM(SET_VERTEX_DATA4F,  1, 3);
        CASEM(SET_VERTEX_DATA4F,  2, 0);
        CASEM(SET_VERTEX_DATA4F,  2, 1);
        CASEM(SET_VERTEX_DATA4F,  2, 2);
        CASEM(SET_VERTEX_DATA4F,  2, 3);
        CASEM(SET_VERTEX_DATA4F,  3, 0);
        CASEM(SET_VERTEX_DATA4F,  3, 1);
        CASEM(SET_VERTEX_DATA4F,  3, 2);
        CASEM(SET_VERTEX_DATA4F,  3, 3);
        CASEM(SET_VERTEX_DATA4F,  4, 0);
        CASEM(SET_VERTEX_DATA4F,  4, 1);
        CASEM(SET_VERTEX_DATA4F,  4, 2);
        CASEM(SET_VERTEX_DATA4F,  4, 3);
        CASEM(SET_VERTEX_DATA4F,  5, 0);
        CASEM(SET_VERTEX_DATA4F,  5, 1);
        CASEM(SET_VERTEX_DATA4F,  5, 2);
        CASEM(SET_VERTEX_DATA4F,  5, 3);
        CASEM(SET_VERTEX_DATA4F,  6, 0);
        CASEM(SET_VERTEX_DATA4F,  6, 1);
        CASEM(SET_VERTEX_DATA4F,  6, 2);
        CASEM(SET_VERTEX_DATA4F,  6, 3);
        CASEM(SET_VERTEX_DATA4F,  7, 0);
        CASEM(SET_VERTEX_DATA4F,  7, 1);
        CASEM(SET_VERTEX_DATA4F,  7, 2);
        CASEM(SET_VERTEX_DATA4F,  7, 3);
        CASEM(SET_VERTEX_DATA4F,  8, 0);
        CASEM(SET_VERTEX_DATA4F,  8, 1);
        CASEM(SET_VERTEX_DATA4F,  8, 2);
        CASEM(SET_VERTEX_DATA4F,  8, 3);
        CASEM(SET_VERTEX_DATA4F,  9, 0);
        CASEM(SET_VERTEX_DATA4F,  9, 1);
        CASEM(SET_VERTEX_DATA4F,  9, 2);
        CASEM(SET_VERTEX_DATA4F,  9, 3);
        CASEM(SET_VERTEX_DATA4F, 10, 0);
        CASEM(SET_VERTEX_DATA4F, 10, 1);
        CASEM(SET_VERTEX_DATA4F, 10, 2);
        CASEM(SET_VERTEX_DATA4F, 10, 3);
        CASEM(SET_VERTEX_DATA4F, 11, 0);
        CASEM(SET_VERTEX_DATA4F, 11, 1);
        CASEM(SET_VERTEX_DATA4F, 11, 2);
        CASEM(SET_VERTEX_DATA4F, 11, 3);
        CASEM(SET_VERTEX_DATA4F, 12, 0);
        CASEM(SET_VERTEX_DATA4F, 12, 1);
        CASEM(SET_VERTEX_DATA4F, 12, 2);
        CASEM(SET_VERTEX_DATA4F, 12, 3);
        CASEM(SET_VERTEX_DATA4F, 13, 0);
        CASEM(SET_VERTEX_DATA4F, 13, 1);
        CASEM(SET_VERTEX_DATA4F, 13, 2);
        CASEM(SET_VERTEX_DATA4F, 13, 3);
        CASEM(SET_VERTEX_DATA4F, 14, 0);
        CASEM(SET_VERTEX_DATA4F, 14, 1);
        CASEM(SET_VERTEX_DATA4F, 14, 2);
        CASEM(SET_VERTEX_DATA4F, 14, 3);
        CASEM(SET_VERTEX_DATA4F, 15, 0);
        CASEM(SET_VERTEX_DATA4F, 15, 1);
        CASEM(SET_VERTEX_DATA4F, 15, 2);
        CASEM(SET_VERTEX_DATA4F, 15, 3);
#undef CASEV
        /* z = used as y here */
#define CASEV(z) \
            PRINTF_NV2A(XEXEC_DBG_ALL, "/* FIXME: Untested! */", 0); \
            INT3; /* FIXME */ \
            PRINTF_NV2A(XEXEC_DBG_VARDUMP, "/* FIXME: Is mapping to [-1,+1] correct? */", 0); \
            a->inline_value[0] = ((int16_t)((c->param >>  0) & 0xffff) * 2.f + 1) / 65535.f; \
            a->inline_value[1] = ((int16_t)((c->param >> 16) & 0xffff) * 2.f + 1) / 65535.f; \
            VARDUMP_NV2A(FLOAT, REG32(&a->inline_value[0])); \
            VARDUMP_NV2A(FLOAT, REG32(&a->inline_value[1]));
#define CASE2V \
            PRINTF_NV2A(XEXEC_DBG_VARDUMP, "/* FIXME: Should these really be set to 0.0 and 1.0 ? Conditions? */", 0); \
            a->inline_value[2] = 0.f; \
            a->inline_value[3] = 1.f; \
            VARDUMP_NV2A(FLOAT, REG32(&a->inline_value[2])); \
            VARDUMP_NV2A(FLOAT, REG32(&a->inline_value[3]));
        CASE2F(SET_VERTEX_DATA2S, 0,  0);
        CASE2(SET_VERTEX_DATA2S,  1,  1);
        CASE2(SET_VERTEX_DATA2S,  2,  2);
        CASE2(SET_VERTEX_DATA2S,  3,  3);
        CASE2(SET_VERTEX_DATA2S,  4,  4);
        CASE2(SET_VERTEX_DATA2S,  5,  5);
        CASE2(SET_VERTEX_DATA2S,  6,  6);
        CASE2(SET_VERTEX_DATA2S,  7,  7);
        CASE2(SET_VERTEX_DATA2S,  8,  8);
        CASE2(SET_VERTEX_DATA2S,  9,  9);
        CASE2(SET_VERTEX_DATA2S, 10, 10);
        CASE2(SET_VERTEX_DATA2S, 11, 11);
        CASE2(SET_VERTEX_DATA2S, 12, 12);
        CASE2(SET_VERTEX_DATA2S, 13, 13);
        CASE2(SET_VERTEX_DATA2S, 14, 14);
        CASE2(SET_VERTEX_DATA2S, 15, 15);
#undef CASE2V
#undef CASEV
        /* z = used as y here */
#define CASEV(z) \
            a->inline_value[0] = ((c->param >>  0) & 0xff) / 255.f; \
            a->inline_value[1] = ((c->param >>  8) & 0xff) / 255.f; \
            a->inline_value[2] = ((c->param >> 16) & 0xff) / 255.f; \
            a->inline_value[3] = ((c->param >> 24) & 0xff) / 255.f; \
            VARDUMP_NV2A(FLOAT, REG32(&a->inline_value[0])); \
            VARDUMP_NV2A(FLOAT, REG32(&a->inline_value[1])); \
            VARDUMP_NV2A(FLOAT, REG32(&a->inline_value[2])); \
            VARDUMP_NV2A(FLOAT, REG32(&a->inline_value[3]));
        CASEF(SET_VERTEX_DATA4UB, 0,  0);
        CASE(SET_VERTEX_DATA4UB,  1,  1);
        CASE(SET_VERTEX_DATA4UB,  2,  2);
        CASE(SET_VERTEX_DATA4UB,  3,  3);
        CASE(SET_VERTEX_DATA4UB,  4,  4);
        CASE(SET_VERTEX_DATA4UB,  5,  5);
        CASE(SET_VERTEX_DATA4UB,  6,  6);
        CASE(SET_VERTEX_DATA4UB,  7,  7);
        CASE(SET_VERTEX_DATA4UB,  8,  8);
        CASE(SET_VERTEX_DATA4UB,  9,  9);
        CASE(SET_VERTEX_DATA4UB, 10, 10);
        CASE(SET_VERTEX_DATA4UB, 11, 11);
        CASE(SET_VERTEX_DATA4UB, 12, 12);
        CASE(SET_VERTEX_DATA4UB, 13, 13);
        CASE(SET_VERTEX_DATA4UB, 14, 14);
        CASE(SET_VERTEX_DATA4UB, 15, 15);
#undef CASEV
#define CASEV(z) \
            PRINTF_NV2A(XEXEC_DBG_ALL, "/* FIXME: Untested! */", 0); \
            INT3; /* FIXME */ \
            PRINTF_NV2A(XEXEC_DBG_VARDUMP, "/* FIXME: Is mapping to [-1,+1] correct? */", 0); \
            a->inline_value[z * 2 + 0] = ((int16_t)((c->param >>  0) & 0xffff) * 2.f + 1) / 65535.f; \
            a->inline_value[z * 2 + 1] = ((int16_t)((c->param >> 16) & 0xffff) * 2.f + 1) / 65535.f; \
            VARDUMP_NV2A(FLOAT, REG32(&a->inline_value[z * 2 + 0])); \
            VARDUMP_NV2A(FLOAT, REG32(&a->inline_value[z * 2 + 1]));
        CASEM(SET_VERTEX_DATA4S,  0, 0);
        CASEFM(SET_VERTEX_DATA4S, 0, 1);
        CASEM(SET_VERTEX_DATA4S,  1, 0);
        CASEM(SET_VERTEX_DATA4S,  1, 1);
        CASEM(SET_VERTEX_DATA4S,  2, 0);
        CASEM(SET_VERTEX_DATA4S,  2, 1);
        CASEM(SET_VERTEX_DATA4S,  3, 0);
        CASEM(SET_VERTEX_DATA4S,  3, 1);
        CASEM(SET_VERTEX_DATA4S,  4, 0);
        CASEM(SET_VERTEX_DATA4S,  4, 1);
        CASEM(SET_VERTEX_DATA4S,  5, 0);
        CASEM(SET_VERTEX_DATA4S,  5, 1);
        CASEM(SET_VERTEX_DATA4S,  6, 0);
        CASEM(SET_VERTEX_DATA4S,  6, 1);
        CASEM(SET_VERTEX_DATA4S,  7, 0);
        CASEM(SET_VERTEX_DATA4S,  7, 1);
        CASEM(SET_VERTEX_DATA4S,  8, 0);
        CASEM(SET_VERTEX_DATA4S,  8, 1);
        CASEM(SET_VERTEX_DATA4S,  9, 0);
        CASEM(SET_VERTEX_DATA4S,  9, 1);
        CASEM(SET_VERTEX_DATA4S, 10, 0);
        CASEM(SET_VERTEX_DATA4S, 10, 1);
        CASEM(SET_VERTEX_DATA4S, 11, 0);
        CASEM(SET_VERTEX_DATA4S, 11, 1);
        CASEM(SET_VERTEX_DATA4S, 12, 0);
        CASEM(SET_VERTEX_DATA4S, 12, 1);
        CASEM(SET_VERTEX_DATA4S, 13, 0);
        CASEM(SET_VERTEX_DATA4S, 13, 1);
        CASEM(SET_VERTEX_DATA4S, 14, 0);
        CASEM(SET_VERTEX_DATA4S, 14, 1);
        CASEM(SET_VERTEX_DATA4S, 15, 0);
        CASEM(SET_VERTEX_DATA4S, 15, 1);
#undef CASEV
#undef CASE2FM
#undef CASE2F
#undef CASEFM
#undef CASEF
#undef CASE2M
#undef CASE2
#undef CASEM
#undef CASE
#undef CASES
        /* parameters are uploaded to PGRAPH below */
        /* x = PGRAPH method name, y = PGRAPH register name, z = PGRAPH register mask name */
#define CASE(x,y) \
        case NV_097_##x: \
            VARDUMP_NV2A(VAR_IN, c->param); \
            NV2A_REG32(p, NV_PGRAPH, y) = c->param; \
            break
#define CASEV(x,y,z) \
        case NV_097_##x: \
            VARDUMP_NV2A(VAR_IN, c->param); \
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, y, z, c->param); \
            break
#define CASEN(x) \
        case NV_097_##x: \
            INT3; \
            break
#define CASE2(x,y,z) \
        case NV_097_##x: \
            VARDUMP_NV2A(VAR_IN, c->param); \
            switch (c->param) { \
            CASE2M(x,y,z) \
            default: \
                INT3; \
                break; \
            } \
            break
#define CASE2I(x,i,y,z) \
        case NV_097_##x##_##i: \
            VARDUMP_NV2A(VAR_IN, c->param); \
            switch (c->param) { \
            CASE2M(x,y,z) \
            default: \
                INT3; \
                break; \
            } \
            break
#define CASE2V(x,y,z,v) \
            case NV_097_##x##_V_##v: \
                NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PGRAPH, y, z, v); \
                break
#define CASE3(x,y) \
        case NV_097_##x: \
            VARDUMP_NV2A(VAR_IN, c->param); \
            CASE3M(x,y) \
            break
#define CASE3I(x,i,y) \
        case NV_097_##x##_##i: \
            VARDUMP_NV2A(VAR_IN, c->param); \
            CASE3M(x,y) \
            break
#define CASE3V(x,y,z) \
            v = NV2A_MASK_BITSHIFT_GET(c->param, NV_097_##x##_##z); \
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, y, z, v)
#define CASE3V2(x,y,m,z) \
            v = NV2A_MASK_BITSHIFT_GET(c->param, NV_097_##x##_##m); \
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, y, z, v)
        CASEV(SET_FLIP_READ, SURFACE, READ_3D);
        CASEV(SET_FLIP_WRITE, SURFACE, WRITE_3D);
        CASEV(SET_FLIP_MODULO, SURFACE, MODULO_3D);
        CASE(SET_ANTI_ALIASING_CONTROL, ANTIALIASING);
        CASEV(SET_BLEND_ENABLE, BLEND, EN);
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,ZERO); \
            CASE2V(x,y,z,ONE); \
            CASE2V(x,y,z,SRC_COLOR); \
            CASE2V(x,y,z,ONE_MINUS_SRC_COLOR); \
            CASE2V(x,y,z,SRC_ALPHA); \
            CASE2V(x,y,z,ONE_MINUS_SRC_ALPHA); \
            CASE2V(x,y,z,DST_ALPHA); \
            CASE2V(x,y,z,ONE_MINUS_DST_ALPHA); \
            CASE2V(x,y,z,DST_COLOR); \
            CASE2V(x,y,z,ONE_MINUS_DST_COLOR); \
            CASE2V(x,y,z,SRC_ALPHA_SATURATE); \
            CASE2V(x,y,z,CONSTANT_COLOR); \
            CASE2V(x,y,z,ONE_MINUS_CONSTANT_COLOR); \
            CASE2V(x,y,z,CONSTANT_ALPHA); \
            CASE2V(x,y,z,ONE_MINUS_CONSTANT_ALPHA);
        CASE2(SET_BLEND_FUNC_SFACTOR, BLEND, SFACTOR);
        CASE2(SET_BLEND_FUNC_DFACTOR, BLEND, DFACTOR);
#undef CASE2M
        CASEV(SET_LOGIC_OP_ENABLE, BLEND, LOGICOP_ENABLE);
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,CLEAR); \
            CASE2V(x,y,z,AND); \
            CASE2V(x,y,z,AND_REVERSE); \
            CASE2V(x,y,z,COPY); \
            CASE2V(x,y,z,AND_INVERTED); \
            CASE2V(x,y,z,NOOP); \
            CASE2V(x,y,z,XOR); \
            CASE2V(x,y,z,OR); \
            CASE2V(x,y,z,NOR); \
            CASE2V(x,y,z,EQUIV); \
            CASE2V(x,y,z,INVERT); \
            CASE2V(x,y,z,OR_REVERSE); \
            CASE2V(x,y,z,COPY_INVERTED); \
            CASE2V(x,y,z,OR_INVERTED); \
            CASE2V(x,y,z,NAND); \
            CASE2V(x,y,z,SET);
        CASE2(SET_LOGIC_OP, BLEND, LOGICOP);
#undef CASE2M
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,FUNC_SUBTRACT); \
            CASE2V(x,y,z,FUNC_REVERSE_SUBTRACT); \
            CASE2V(x,y,z,FUNC_ADD); \
            CASE2V(x,y,z,MIN); \
            CASE2V(x,y,z,MAX); \
            CASE2V(x,y,z,FUNC_REVERSE_SUBTRACT_SIGNED); \
            CASE2V(x,y,z,FUNC_ADD_SIGNED);
        CASE2(SET_BLEND_EQUATION, BLEND, EQN);
#undef CASE2M
        CASE(SET_BLEND_COLOR, BLENDCOLOR);
        CASE(SET_TEXTURE_BORDER_COLOR_0, BORDERCOLOR0);
        CASE(SET_TEXTURE_BORDER_COLOR_1, BORDERCOLOR1);
        CASE(SET_TEXTURE_BORDER_COLOR_2, BORDERCOLOR2);
        CASE(SET_TEXTURE_BORDER_COLOR_3, BORDERCOLOR3);
        CASEN(SET_TEXTURE_SET_BUMP_ENV_MAT00_0);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT00_1, BUMPMAT00_1);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT00_2, BUMPMAT00_2);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT00_3, BUMPMAT00_3);
        CASEN(SET_TEXTURE_SET_BUMP_ENV_MAT01_0);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT01_1, BUMPMAT01_1);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT01_2, BUMPMAT01_2);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT01_3, BUMPMAT01_3);
        CASEN(SET_TEXTURE_SET_BUMP_ENV_MAT11_0);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT11_1, BUMPMAT11_1);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT11_2, BUMPMAT11_2);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT11_3, BUMPMAT11_3);
        CASEN(SET_TEXTURE_SET_BUMP_ENV_MAT10_0);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT10_1, BUMPMAT10_1);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT10_2, BUMPMAT10_2);
        CASE(SET_TEXTURE_SET_BUMP_ENV_MAT10_3, BUMPMAT10_3);
        CASEN(SET_TEXTURE_SET_BUMP_ENV_SCALE_0);
        CASE(SET_TEXTURE_SET_BUMP_ENV_SCALE_1, BUMPSCALE1);
        CASE(SET_TEXTURE_SET_BUMP_ENV_SCALE_2, BUMPSCALE2);
        CASE(SET_TEXTURE_SET_BUMP_ENV_SCALE_3, BUMPSCALE3);
        CASEN(SET_TEXTURE_SET_BUMP_ENV_OFFSET_0);
        CASE(SET_TEXTURE_SET_BUMP_ENV_OFFSET_1, BUMPOFFSET1);
        CASE(SET_TEXTURE_SET_BUMP_ENV_OFFSET_2, BUMPOFFSET2);
        CASE(SET_TEXTURE_SET_BUMP_ENV_OFFSET_3, BUMPOFFSET3);
        CASEV(SET_TRANSFORM_PROGRAM_LOAD, CHEOPS_OFFSET, PROG_LD_PTR);//assert(parameter < NV2A_MAX_TRANSFORM_PROGRAM_LENGTH);
        CASEV(SET_TRANSFORM_PROGRAM_START, CSV0_C, CHEOPS_PROGRAM_START);//assert(parameter < NV2A_MAX_TRANSFORM_PROGRAM_LENGTH);
        CASEV(SET_TRANSFORM_CONSTANT_LOAD, CHEOPS_OFFSET, CONST_LD_PTR);//assert(parameter < NV2A_VERTEXSHADER_CONSTANTS);
        CASEV(SET_NORMALIZATION_ENABLE, CSV0_C, NORMALIZATION_ENABLE);
        CASE(SET_CLEAR_RECT_HORIZONTAL, CLEARRECTX);
        CASE(SET_CLEAR_RECT_VERTICAL, CLEARRECTY);
        CASE(SET_COLOR_CLEAR_VALUE, COLORCLEARVALUE);
        CASE(SET_COLOR_KEY_COLOR_0, COLORKEYCOLOR0);
        CASE(SET_COLOR_KEY_COLOR_1, COLORKEYCOLOR1);
        CASE(SET_COLOR_KEY_COLOR_2, COLORKEYCOLOR2);
        CASE(SET_COLOR_KEY_COLOR_3, COLORKEYCOLOR3);
        CASE(SET_COMBINER_FACTOR0_0, COMBINEFACTOR0_0);
        CASE(SET_COMBINER_FACTOR0_1, COMBINEFACTOR0_1);
        CASE(SET_COMBINER_FACTOR0_2, COMBINEFACTOR0_2);
        CASE(SET_COMBINER_FACTOR0_3, COMBINEFACTOR0_3);
        CASE(SET_COMBINER_FACTOR0_4, COMBINEFACTOR0_4);
        CASE(SET_COMBINER_FACTOR0_5, COMBINEFACTOR0_5);
        CASE(SET_COMBINER_FACTOR0_6, COMBINEFACTOR0_6);
        CASE(SET_COMBINER_FACTOR0_7, COMBINEFACTOR0_7);
        CASE(SET_COMBINER_FACTOR1_0, COMBINEFACTOR1_0);
        CASE(SET_COMBINER_FACTOR1_1, COMBINEFACTOR1_1);
        CASE(SET_COMBINER_FACTOR1_2, COMBINEFACTOR1_2);
        CASE(SET_COMBINER_FACTOR1_3, COMBINEFACTOR1_3);
        CASE(SET_COMBINER_FACTOR1_4, COMBINEFACTOR1_4);
        CASE(SET_COMBINER_FACTOR1_5, COMBINEFACTOR1_5);
        CASE(SET_COMBINER_FACTOR1_6, COMBINEFACTOR1_6);
        CASE(SET_COMBINER_FACTOR1_7, COMBINEFACTOR1_7);
        CASE(SET_COMBINER_ALPHA_ICW_0, COMBINEALPHAI0);
        CASE(SET_COMBINER_ALPHA_ICW_1, COMBINEALPHAI1);
        CASE(SET_COMBINER_ALPHA_ICW_2, COMBINEALPHAI2);
        CASE(SET_COMBINER_ALPHA_ICW_3, COMBINEALPHAI3);
        CASE(SET_COMBINER_ALPHA_ICW_4, COMBINEALPHAI4);
        CASE(SET_COMBINER_ALPHA_ICW_5, COMBINEALPHAI5);
        CASE(SET_COMBINER_ALPHA_ICW_6, COMBINEALPHAI6);
        CASE(SET_COMBINER_ALPHA_ICW_7, COMBINEALPHAI7);
        CASE(SET_COMBINER_ALPHA_OCW_0, COMBINEALPHAO0);
        CASE(SET_COMBINER_ALPHA_OCW_1, COMBINEALPHAO1);
        CASE(SET_COMBINER_ALPHA_OCW_2, COMBINEALPHAO2);
        CASE(SET_COMBINER_ALPHA_OCW_3, COMBINEALPHAO3);
        CASE(SET_COMBINER_ALPHA_OCW_4, COMBINEALPHAO4);
        CASE(SET_COMBINER_ALPHA_OCW_5, COMBINEALPHAO5);
        CASE(SET_COMBINER_ALPHA_OCW_6, COMBINEALPHAO6);
        CASE(SET_COMBINER_ALPHA_OCW_7, COMBINEALPHAO7);
        CASE(SET_COMBINER_COLOR_ICW_0, COMBINECOLORI0);
        CASE(SET_COMBINER_COLOR_ICW_1, COMBINECOLORI1);
        CASE(SET_COMBINER_COLOR_ICW_2, COMBINECOLORI2);
        CASE(SET_COMBINER_COLOR_ICW_3, COMBINECOLORI3);
        CASE(SET_COMBINER_COLOR_ICW_4, COMBINECOLORI4);
        CASE(SET_COMBINER_COLOR_ICW_5, COMBINECOLORI5);
        CASE(SET_COMBINER_COLOR_ICW_6, COMBINECOLORI6);
        CASE(SET_COMBINER_COLOR_ICW_7, COMBINECOLORI7);
        CASE(SET_COMBINER_COLOR_OCW_0, COMBINECOLORO0);
        CASE(SET_COMBINER_COLOR_OCW_1, COMBINECOLORO1);
        CASE(SET_COMBINER_COLOR_OCW_2, COMBINECOLORO2);
        CASE(SET_COMBINER_COLOR_OCW_3, COMBINECOLORO3);
        CASE(SET_COMBINER_COLOR_OCW_4, COMBINECOLORO4);
        CASE(SET_COMBINER_COLOR_OCW_5, COMBINECOLORO5);
        CASE(SET_COMBINER_COLOR_OCW_6, COMBINECOLORO6);
        CASE(SET_COMBINER_COLOR_OCW_7, COMBINECOLORO7);
        CASE(SET_COMBINER_CONTROL, COMBINECTL);
        CASE(SET_COMBINER_SPECULAR_FOG_CW0, COMBINESPECFOG0);
        CASE(SET_COMBINER_SPECULAR_FOG_CW1, COMBINESPECFOG1);
#define CASE3M(x,y) \
            CASE3V(x,y,STENCIL_WRITE_ENABLE); \
            CASE3V(x,y,Z_PERSPECTIVE_ENABLE); \
            CASE3V2(x,y,COLOR_SPACE_CONVERT,CSCONVERT); \
            CASE3V2(x,CONTROL_3,PREMULTIPLIEDALPHA,PREMULTALPHA); \
            CASE3V(x,CONTROL_3,TEXTUREPERSPECTIVE); \
            CASE3V(x,SETUPRASTER,Z_FORMAT);
        CASE3(SET_CONTROL0, CONTROL_0);
#undef CASE3M
        CASEV(SET_ALPHA_TEST_ENABLE, CONTROL_0, ALPHATESTENABLE);
        CASEV(SET_DEPTH_TEST_ENABLE, CONTROL_0, ZENABLE);
        CASEV(SET_DITHER_ENABLE, CONTROL_0, DITHERENABLE);
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,NEVER); \
            CASE2V(x,y,z,LESS); \
            CASE2V(x,y,z,EQUAL); \
            CASE2V(x,y,z,LEQUAL); \
            CASE2V(x,y,z,GREATER); \
            CASE2V(x,y,z,NOTEQUAL); \
            CASE2V(x,y,z,GEQUAL); \
            CASE2V(x,y,z,ALWAYS);
        CASE2(SET_ALPHA_FUNC, CONTROL_0, ALPHAFUNC);
        CASEV(SET_ALPHA_REF, CONTROL_0, ALPHAREF);
        CASE2(SET_DEPTH_FUNC, CONTROL_0, ZFUNC);
#define CASE3M(x,y) \
            CASE3V(x,y,ALPHA_WRITE_ENABLE); \
            CASE3V(x,y,RED_WRITE_ENABLE); \
            CASE3V(x,y,GREEN_WRITE_ENABLE); \
            CASE3V(x,y,BLUE_WRITE_ENABLE);
        CASE3(SET_COLOR_MASK, CONTROL_0);
#undef CASE3M
        CASEV(SET_DEPTH_MASK, CONTROL_0, ZWRITEENABLE);
        CASEV(SET_STENCIL_TEST_ENABLE, CONTROL_1, STENCIL_TEST_ENABLE);
        CASEV(SET_STENCIL_MASK, CONTROL_1, STENCIL_MASK_WRITE);
        CASE2(SET_STENCIL_FUNC, CONTROL_1, STENCIL_FUNC);
#undef CASE2M
        CASEV(SET_STENCIL_FUNC_REF, CONTROL_1, STENCIL_REF);
        CASEV(SET_STENCIL_FUNC_MASK, CONTROL_1, STENCIL_MASK_READ);
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,KEEP); \
            CASE2V(x,y,z,ZERO); \
            CASE2V(x,y,z,REPLACE); \
            CASE2V(x,y,z,INCRSAT); \
            CASE2V(x,y,z,DECRSAT); \
            CASE2V(x,y,z,INVERT); \
            CASE2V(x,y,z,INCR); \
            CASE2V(x,y,z,DECR);
        CASE2(SET_STENCIL_OP_FAIL, CONTROL_2, STENCIL_OP_FAIL);
        CASE2(SET_STENCIL_OP_ZFAIL, CONTROL_2, STENCIL_OP_ZFAIL);
        CASE2(SET_STENCIL_OP_ZPASS, CONTROL_2, STENCIL_OP_ZPASS);
#undef CASE2M
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,LINEAR); \
            CASE2V(x,y,z,EXP); \
            CASE2V(x,y,z,EXP2); \
            CASE2V(x,y,z,EXP_ABS); \
            CASE2V(x,y,z,EXP2_ABS); \
            CASE2V(x,y,z,LINEAR_ABS);
        CASE2(SET_FOG_MODE, CONTROL_3, FOG_MODE);
#undef CASE2M
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,SPEC_ALPHA); \
            CASE2V(x,y,z,RADIAL); \
            CASE2V(x,y,z,PLANAR); \
            CASE2V(x,y,z,ABS_PLANAR); \
            CASE2V(x,y,z,FOG_X);
        CASE2(SET_FOG_GEN_MODE, CSV0_D, FOGGENMODE);
#undef CASE2M
        case NV_097_SET_FOG_ENABLE:
            VARDUMP_NV2A(VAR_IN, c->param);
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, CONTROL_3, FOGENABLE, c->param);
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, CSV0_D, FOGENABLE, c->param);
            break;
        case NV_097_SET_POINT_PARAMS_ENABLE:
            VARDUMP_NV2A(VAR_IN, c->param);
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, CONTROL_3, POINTPARAMSENABLE, c->param);
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, CSV0_D, POINTPARAMSENABLE, c->param);
            break;
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,FLAT); \
            CASE2V(x,y,z,SMOOTH);
        CASE2(SET_SHADE_MODE, CONTROL_3, SHADEMODE);
#undef CASE2M
        case NV_097_SET_SPECULAR_ENABLE:
            VARDUMP_NV2A(VAR_IN, c->param);
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, CONTROL_3, SPECULARENABLE, c->param);
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, CSV0_C, SPECULARENABLE, c->param);
            break;
        CASEV(SET_FLAT_SHADE_OP, CONTROL_3, FLATSHADE_OP);
        CASEV(SET_LIGHTING_ENABLE, CSV0_C, LIGHTING);
        CASEV(SET_SKIN_MODE, CSV0_D, SKIN);
        CASEV(SET_LIGHT_ENABLE_MASK, CSV0_D, LIGHT);
        CASEV(SET_TEXGEN_VIEW_MODEL, CSV0_D, TEXGEN_REF);
#define CASE3M(x,y) \
            CASE3V(x,y,MODE); \
            CASE3V(x,y,RANGE_MODE);
        CASE3(SET_TRANSFORM_EXECUTION_MODE, CSV0_D);
#undef CASE3M
        CASEV(SET_TRANSFORM_PROGRAM_CXT_WRITE_EN, CSV0_D, CHEOPS_STALL);
#define CASE3M(x,y) \
            CASE3V(x,y,RED); \
            CASE3V(x,y,GREEN); \
            CASE3V(x,y,BLUE); \
            CASE3V(x,y,ALPHA);
        CASE3(SET_FOG_COLOR, FOGCOLOR);
#undef CASE3M
        CASE(SET_FOG_PARAMS_0, FOGPARAM0);
        CASE(SET_FOG_PARAMS_1, FOGPARAM1);
        CASE(SET_FOG_PARAMS_2, POINTSIZE);
        CASEV(SET_WINDOW_CLIP_TYPE, SETUPRASTER, WINDOWCLIPTYPE);
        CASEV(SET_CULL_FACE_ENABLE, SETUPRASTER, CULLENABLE);
        CASEV(SET_POINT_SMOOTH_ENABLE, SETUPRASTER, POINTSMOOTHENABLE);
        CASEV(SET_LINE_SMOOTH_ENABLE, SETUPRASTER, LINESMOOTHENABLE);
        CASEV(SET_POLY_SMOOTH_ENABLE, SETUPRASTER, POLYSMOOTHENABLE);
        CASEV(SET_STIPPLE_CONTROL, SETUPRASTER, STIPPLE_EN);
        CASEV(SET_POLY_OFFSET_POINT_ENABLE, SETUPRASTER, POFFSETPOINTENABLE);
        CASEV(SET_POLY_OFFSET_LINE_ENABLE, SETUPRASTER, POFFSETLINEENABLE);
        CASEV(SET_POLY_OFFSET_FILL_ENABLE, SETUPRASTER, POFFSETFILLENABLE);
        CASEV(SET_LINE_WIDTH, SETUPRASTER, LINEWIDTH);
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,POINT); \
            CASE2V(x,y,z,LINE); \
            CASE2V(x,y,z,FILL);
        CASE2(SET_FRONT_POLYGON_MODE, SETUPRASTER, FRONTFACEMODE);
        CASE2(SET_BACK_POLYGON_MODE, SETUPRASTER, BACKFACEMODE);
#undef CASE2M
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,FRONT); \
            CASE2V(x,y,z,BACK); \
            CASE2V(x,y,z,FRONT_AND_BACK);
        CASE2(SET_CULL_FACE, SETUPRASTER, CULLCTRL);
#undef CASE2M
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,CW); \
            CASE2V(x,y,z,CCW);
        CASE2(SET_FRONT_FACE, SETUPRASTER, FRONTFACE);
#undef CASE2M
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,8); \
            CASE2V(x,y,z,16); \
            CASE2V(x,y,z,32); \
            CASE2V(x,y,z,64); \
            CASE2V(x,y,z,128); \
            CASE2V(x,y,z,OFF);
        CASE2(SET_SWATH_WIDTH, SETUPRASTER, SWATHWIDTH);
#undef CASE2M
#define CASE3M(x,y) \
            CASE3V(x,y,CULL_NEAR_FAR_EN); \
            CASE3V(x,ZCOMPRESSOCCLUDE,ZCLAMP_EN); \
            CASE3V2(x,y,CULL_IGNORE_W,IGNORE_WSIGN);
        CASE3(SET_ZMIN_MAX_CONTROL, SETUPRASTER);
#undef CASE3M
        CASE(SET_SHADER_CLIP_PLANE_MODE, SHADERCLIPMODE);
#define CASE3M(x,y) \
            CASE3V2(x,y,STAGE1,MAP1); \
            CASE3V2(x,y,STAGE2,MAP2); \
            CASE3V2(x,y,STAGE3,MAP3);
        CASE3(SET_DOT_RGBMAPPING, SHADERCTL);
#undef CASE3M
#define CASE3M(x,y) \
            CASE3V2(x,y,STAGE1,OUT1); \
            CASE3V2(x,y,STAGE2,OUT2); \
            CASE3V2(x,y,STAGE3,OUT3);
        CASE3(SET_SHADER_OTHER_STAGE_INPUT, SHADERCTL);
#undef CASE3M
        CASE(SET_SHADER_STAGE_PROGRAM, SHADERPROG);
        CASE(SET_SEMAPHORE_OFFSET, SEMAPHOREOFFSET);
        CASE(SET_SHADOW_ZSLOPE_THRESHOLD, SHADOWZSLOPETHRESHOLD);//assert(parameter == 0x7F800000); /* FIXME: Unimplemented */
        CASE(SET_SPECULAR_FOG_FACTOR_0, SPECFOGFACTOR0);
        CASE(SET_SPECULAR_FOG_FACTOR_1, SPECFOGFACTOR1);
        CASE(SET_SURFACE_CLIP_HORIZONTAL, SURFACECLIPX);
        CASE(SET_SURFACE_CLIP_VERTICAL, SURFACECLIPY);
        CASE(SET_TEXTURE_ADDRESS_0, TEXADDRESS0);
        CASE(SET_TEXTURE_ADDRESS_1, TEXADDRESS1);
        CASE(SET_TEXTURE_ADDRESS_2, TEXADDRESS2);
        CASE(SET_TEXTURE_ADDRESS_3, TEXADDRESS3);
        CASE(SET_TEXTURE_CONTROL0_0, TEXCTL0_0);
        CASE(SET_TEXTURE_CONTROL0_1, TEXCTL0_1);
        CASE(SET_TEXTURE_CONTROL0_2, TEXCTL0_2);
        CASE(SET_TEXTURE_CONTROL0_3, TEXCTL0_3);
        CASE(SET_TEXTURE_CONTROL1_0, TEXCTL1_0);
        CASE(SET_TEXTURE_CONTROL1_1, TEXCTL1_1);
        CASE(SET_TEXTURE_CONTROL1_2, TEXCTL1_2);
        CASE(SET_TEXTURE_CONTROL1_3, TEXCTL1_3);
        CASE(SET_TEXTURE_FILTER_0, TEXFILTER0);
        CASE(SET_TEXTURE_FILTER_1, TEXFILTER1);
        CASE(SET_TEXTURE_FILTER_2, TEXFILTER2);
        CASE(SET_TEXTURE_FILTER_3, TEXFILTER3);
#define CASE3M(x,y) \
            CASE3V(x,y,CONTEXT_DMA); \
            CASE3V2(x,y,CUBEMAP_ENABLE,CUBEMAPENABLE); \
            CASE3V(x,y,BORDER_SOURCE); \
            CASE3V(x,y,DIMENSIONALITY); \
            CASE3V(x,y,COLOR); \
            CASE3V(x,y,MIPMAP_LEVELS); \
            CASE3V(x,y,BASE_SIZE_U); \
            CASE3V(x,y,BASE_SIZE_V); \
            CASE3V(x,y,BASE_SIZE_P);
        CASE3I(SET_TEXTURE_FORMAT, 0, TEXFMT0);
        CASE3I(SET_TEXTURE_FORMAT, 1, TEXFMT1);
        CASE3I(SET_TEXTURE_FORMAT, 2, TEXFMT2);
        CASE3I(SET_TEXTURE_FORMAT, 3, TEXFMT3);
#undef CASE3M
        CASE(SET_TEXTURE_IMAGE_RECT_0, TEXIMAGERECT0);
        CASE(SET_TEXTURE_IMAGE_RECT_1, TEXIMAGERECT1);
        CASE(SET_TEXTURE_IMAGE_RECT_2, TEXIMAGERECT2);
        CASE(SET_TEXTURE_IMAGE_RECT_3, TEXIMAGERECT3);
        CASE(SET_TEXTURE_OFFSET_0, TEXOFFSET0);
        CASE(SET_TEXTURE_OFFSET_1, TEXOFFSET1);
        CASE(SET_TEXTURE_OFFSET_2, TEXOFFSET2);
        CASE(SET_TEXTURE_OFFSET_3, TEXOFFSET3);
        CASE(SET_TEXTURE_PALETTE_0, TEXPALETTE0);
        CASE(SET_TEXTURE_PALETTE_1, TEXPALETTE1);
        CASE(SET_TEXTURE_PALETTE_2, TEXPALETTE2);
        CASE(SET_TEXTURE_PALETTE_3, TEXPALETTE3);
        CASE(SET_WINDOW_CLIP_HORIZONTAL_0, WINDOWCLIPX0);
        CASE(SET_WINDOW_CLIP_HORIZONTAL_1, WINDOWCLIPX1);
        CASE(SET_WINDOW_CLIP_HORIZONTAL_2, WINDOWCLIPX2);
        CASE(SET_WINDOW_CLIP_HORIZONTAL_3, WINDOWCLIPX3);
        CASE(SET_WINDOW_CLIP_HORIZONTAL_4, WINDOWCLIPX4);
        CASE(SET_WINDOW_CLIP_HORIZONTAL_5, WINDOWCLIPX5);
        CASE(SET_WINDOW_CLIP_HORIZONTAL_6, WINDOWCLIPX6);
        CASE(SET_WINDOW_CLIP_HORIZONTAL_7, WINDOWCLIPX7);
        CASE(SET_WINDOW_CLIP_VERTICAL_0, WINDOWCLIPY0);
        CASE(SET_WINDOW_CLIP_VERTICAL_1, WINDOWCLIPY1);
        CASE(SET_WINDOW_CLIP_VERTICAL_2, WINDOWCLIPY2);
        CASE(SET_WINDOW_CLIP_VERTICAL_3, WINDOWCLIPY3);
        CASE(SET_WINDOW_CLIP_VERTICAL_4, WINDOWCLIPY4);
        CASE(SET_WINDOW_CLIP_VERTICAL_5, WINDOWCLIPY5);
        CASE(SET_WINDOW_CLIP_VERTICAL_6, WINDOWCLIPY6);
        CASE(SET_WINDOW_CLIP_VERTICAL_7, WINDOWCLIPY7);
        CASEV(SET_COMPRESS_ZBUFFER_EN, ZCOMPRESSOCCLUDE, COMPRESS_ZEN);
#define CASE3M(x,y) \
            CASE3V(x,y,OCCLUDE_ZEN); \
            CASE3V(x,y,OCCLUDE_STENCIL_EN);
        CASE3(SET_OCCLUDE_ZSTENCIL_EN, ZCOMPRESSOCCLUDE);
#undef CASE3M
        CASE(SET_ZSTENCIL_CLEAR_VALUE, ZSTENCILCLEARVALUE);
        CASE(SET_CLIP_MAX, ZCLIPMAX);
        CASE(SET_CLIP_MIN, ZCLIPMIN);
        CASEV(SET_CONTEXT_DMA_A, CONTEXTDMAA, V);
        CASEV(SET_CONTEXT_DMA_B, CONTEXTDMAB, V);
        CASEV(SET_CONTEXT_DMA_VERTEX_A, CONTEXTVTXA, V);
        CASEV(SET_CONTEXT_DMA_VERTEX_B, CONTEXTVTXB, V);
        CASE(SET_POLYGON_OFFSET_BIAS, ZOFFSETBIAS);
        CASE(SET_POLYGON_OFFSET_SCALE_FACTOR, ZOFFSETFACTOR);
        CASE(SET_EYE_VECTOR_0, EYEVEC0);
        CASE(SET_EYE_VECTOR_1, EYEVEC1);
        CASE(SET_EYE_VECTOR_2, EYEVEC2);
        CASE(SET_STIPPLE_PATTERN_0, STIPPLE0);
        CASE(SET_STIPPLE_PATTERN_1, STIPPLE1);
        CASE(SET_STIPPLE_PATTERN_2, STIPPLE2);
        CASE(SET_STIPPLE_PATTERN_3, STIPPLE3);
        CASE(SET_STIPPLE_PATTERN_4, STIPPLE4);
        CASE(SET_STIPPLE_PATTERN_5, STIPPLE5);
        CASE(SET_STIPPLE_PATTERN_6, STIPPLE6);
        CASE(SET_STIPPLE_PATTERN_7, STIPPLE7);
        CASE(SET_STIPPLE_PATTERN_8, STIPPLE8);
        CASE(SET_STIPPLE_PATTERN_9, STIPPLE9);
        CASE(SET_STIPPLE_PATTERN_10, STIPPLE10);
        CASE(SET_STIPPLE_PATTERN_11, STIPPLE11);
        CASE(SET_STIPPLE_PATTERN_12, STIPPLE12);
        CASE(SET_STIPPLE_PATTERN_13, STIPPLE13);
        CASE(SET_STIPPLE_PATTERN_14, STIPPLE14);
        CASE(SET_STIPPLE_PATTERN_15, STIPPLE15);
        CASE(SET_STIPPLE_PATTERN_16, STIPPLE16);
        CASE(SET_STIPPLE_PATTERN_17, STIPPLE17);
        CASE(SET_STIPPLE_PATTERN_18, STIPPLE18);
        CASE(SET_STIPPLE_PATTERN_19, STIPPLE19);
        CASE(SET_STIPPLE_PATTERN_20, STIPPLE20);
        CASE(SET_STIPPLE_PATTERN_21, STIPPLE21);
        CASE(SET_STIPPLE_PATTERN_22, STIPPLE22);
        CASE(SET_STIPPLE_PATTERN_23, STIPPLE23);
        CASE(SET_STIPPLE_PATTERN_24, STIPPLE24);
        CASE(SET_STIPPLE_PATTERN_25, STIPPLE25);
        CASE(SET_STIPPLE_PATTERN_26, STIPPLE26);
        CASE(SET_STIPPLE_PATTERN_27, STIPPLE27);
        CASE(SET_STIPPLE_PATTERN_28, STIPPLE28);
        CASE(SET_STIPPLE_PATTERN_29, STIPPLE29);
        CASE(SET_STIPPLE_PATTERN_30, STIPPLE30);
        CASE(SET_STIPPLE_PATTERN_31, STIPPLE31);
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,DISABLE); \
            CASE2V(x,y,z,NORMAL_MAP); \
            CASE2V(x,y,z,REFLECTION_MAP); \
            CASE2V(x,y,z,EYE_LINEAR); \
            CASE2V(x,y,z,OBJECT_LINEAR); \
            CASE2V(x,y,z,SPHERE_MAP);
        CASE2I(SET_TEXGEN_S, 0, CSV1_A, T0_S);
        CASE2I(SET_TEXGEN_S, 1, CSV1_A, T1_S);
        CASE2I(SET_TEXGEN_S, 2, CSV1_B, T2_S);
        CASE2I(SET_TEXGEN_S, 3, CSV1_B, T3_S);
        CASE2I(SET_TEXGEN_T, 0, CSV1_A, T0_T);
        CASE2I(SET_TEXGEN_T, 1, CSV1_A, T1_T);
        CASE2I(SET_TEXGEN_T, 2, CSV1_B, T2_T);
        CASE2I(SET_TEXGEN_T, 3, CSV1_B, T3_T);
#undef CASE2M
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,DISABLE); \
            CASE2V(x,y,z,NORMAL_MAP); \
            CASE2V(x,y,z,REFLECTION_MAP); \
            CASE2V(x,y,z,EYE_LINEAR); \
            CASE2V(x,y,z,OBJECT_LINEAR);
        CASE2I(SET_TEXGEN_R, 0, CSV1_A, T0_R);
        CASE2I(SET_TEXGEN_R, 1, CSV1_A, T1_R);
        CASE2I(SET_TEXGEN_R, 2, CSV1_B, T2_R);
        CASE2I(SET_TEXGEN_R, 3, CSV1_B, T3_R);
#undef CASE2M
#define CASE2M(x,y,z) \
            CASE2V(x,y,z,DISABLE); \
            CASE2V(x,y,z,EYE_LINEAR); \
            CASE2V(x,y,z,OBJECT_LINEAR);
        CASE2I(SET_TEXGEN_Q, 0, CSV1_A, T0_Q);
        CASE2I(SET_TEXGEN_Q, 1, CSV1_A, T1_Q);
        CASE2I(SET_TEXGEN_Q, 2, CSV1_B, T2_Q);
        CASE2I(SET_TEXGEN_Q, 3, CSV1_B, T3_Q);
#undef CASE2M
        CASEV(SET_TEXTURE_MATRIX_ENABLE_0, CSV1_A, T0_ENABLE);
        CASEV(SET_TEXTURE_MATRIX_ENABLE_1, CSV1_A, T1_ENABLE);
        CASEV(SET_TEXTURE_MATRIX_ENABLE_2, CSV1_B, T2_ENABLE);
        CASEV(SET_TEXTURE_MATRIX_ENABLE_3, CSV1_B, T3_ENABLE);
#undef CASE3V2
#undef CASE3V
#undef CASE3I
#undef CASE3
#undef CASE2V
#undef CASE2I
#undef CASE2
#undef CASEN
#undef CASEV
#undef CASE
        default:
            VARDUMP_NV2A(VAR_IN, c->param);
            t = (c->method / 4 < ARRAY_SIZE(nv2a_097_name));
            PRINTF_NV2A(
                XEXEC_DBG_ERROR,
                "warning: unhandled method: 0x%.08x (%u)%s%s%s",
                c->method,
                c->method,
                (t) ? " (" : "",
                (t) ? nv2a_097_name[c->method / 4] : "",
                (t) ?  ")" : "");
            break;
        }
        break;
    default:
        VARDUMP_NV2A(VAR_IN, c->method);
        VARDUMP_NV2A(VAR_IN, c->param);
        PRINTF_NV2A(
            XEXEC_DBG_ERROR,
            "warning: unhandled graphics class: 0x%.08x (%u)",
            ctx1->grclass,
            ctx1->grclass);
        break;
    }

    LEAVE_NV2A;
}

static int
nv2a_pfifo_ramht_lookup(void *p, uint32_t *handle, nv2a_pfifo_ramht_t *r) {
    register uint32_t bits;
    register uint32_t size;
    register uint32_t hash;
    register void *ptr;
    ENTER_NV2A;

    bits = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, RAMHT, SIZE) + 12;
    size = 1 << bits;

    for (--bits, hash = 0; *handle; hash ^= *handle & (size - 1), *handle >>= bits);
    hash  ^= NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID) << (bits - 4);
    hash <<= 3;

    if (hash >= size) {
        INT3;
        LEAVE_NV2A;
        return 0;
    }

    ptr  = NV2A_REGADDR(p, NV_PRAMIN, BASE);
    ptr += NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, RAMHT, BASE_ADDRESS) << 12;
    ptr += hash;

    *r = *(typeof(r))ptr;
//PRINT(XEXEC_DBG_ALL,"ramht_lookup: handle:%u | context:0x%.08x | pgraph param:0x%.04x | engine:%u | chid:%u | valid:%u | reg:%p",r->handle,r->context,r->instance << 4,r->engine,r->chid,r->valid,ptr-NV2A_REGADDR(p, NV_PRAMIN, BASE));//XXX
    if (r->valid) *handle = (typeof(*handle))r->instance << 4;

    LEAVE_NV2A;
    return r->valid;
}

static void
nv2a_pfifo_puller(void *p, nv2a_pfifo_command_t *c) {
    nv2a_pfifo_ramht_t r;
    ENTER_NV2A;

    do {
        if (NV2A_REG32_MASK_BITSHIFT_TEST(p, NV_PFIFO, CACHE1_PULL0, ACCESS, DISABLED)) {
            PRINTF_NV2A(
                XEXEC_DBG_ERROR,
                "warning: puller access disabled; cannot process command method: 0x%x",
                c->method);
            break;
        }
//glo_current(nv2a_ctx->glo, 1);//TODO
        if (c->method == /* NV_SET_OBJECT */ 0 || (c->method >= 0x180 && c->method <= 0x1a8)) {
PRINTF_NV2A(XEXEC_DBG_ALL,"ramht_lookup: method: 0x%x | handle:%u / 0x%x",c->method,c->param,c->param);//XXX
            if (!nv2a_pfifo_ramht_lookup(p, &c->param, &r)) break;
            if (r.chid != NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID)) INT3;
        }
        if (c->method == /* NV_SET_OBJECT */ 0) {
            switch (r.engine) {
            case NV_RAMHT_ENGINE_GRAPHICS /* 1 */:
                nv2a_pgraph_intr_context_switch(p, r.chid);
                nv2a_pgraph_fifo(p, c);
                break;
            case NV_RAMHT_ENGINE_SW /* 0 */:
            case NV_RAMHT_ENGINE_DVD /* 2 */:
            default:
                INT3;
                break;
            }
            switch (c->subchannel) {
#define CASE(x) \
            case x: \
                NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PFIFO, CACHE1_ENGINE, x, r.engine); \
                break
            CASE(0);
            CASE(1);
            CASE(2);
            CASE(3);
            CASE(4);
            CASE(5);
            CASE(6);
            CASE(7);
#undef CASE
            }
        } else if (c->method >= 0x100) {
            switch (c->subchannel) {
#define CASE(x) \
            case x: \
                r.engine = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_ENGINE, x); \
                break
            CASE(0);
            CASE(1);
            CASE(2);
            CASE(3);
            CASE(4);
            CASE(5);
            CASE(6);
            CASE(7);
#undef CASE
            }
            switch (r.engine) {
            case NV_RAMHT_ENGINE_GRAPHICS /* 1 */:
                nv2a_pgraph_fifo(p, c);
                break;
            case NV_RAMHT_ENGINE_SW /* 0 */:
            case NV_RAMHT_ENGINE_DVD /* 2 */:
            default:
                INT3;
                break;
            }
        }
    } while (0);

    LEAVE_NV2A;
}

void
nv2a_pfifo_pusher(void *p) {
    register nv2a_pfifo_command_t *c;
    nv2a_pfifo_command_t cmd;
    nv2a_dma_t dma;
    uint32_t addr;
    uint32_t v;
    register uint32_t *get;
    register uint32_t *put;
    register uint32_t *d;
    register uint32_t word;
    ENTER_NV2A;

    if (!p) p = nv2a->memreg;

    if (NV2A_REG32_MASK_BITSHIFT_TEST(p, NV_PFIFO, CACHE1_PUSH0, ACCESS, DISABLED) ||
        NV2A_REG32_MASK_BITSHIFT_TEST(p, NV_PFIFO, CACHE1_DMA_PUSH, ACCESS, DISABLED) ||
        NV2A_REG32_MASK_BITSHIFT_TEST(p, NV_PFIFO, CACHE1_DMA_PUSH, STATUS, SUSPENDED)) {
        NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PFIFO, CACHE1_DMA_PUSH, STATE, IDLE);
        LEAVE_NV2A;
        return;
    }

    NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PFIFO, CACHE1_DMA_PUSH, STATE, BUSY);

    v = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID);
    /* TODO: PIO not supported */
    if (!(NV2A_REG32(p, NV_PFIFO, MODE) & (1 << v))) {
        PRINTF_NV2A(XEXEC_DBG_ALL, "error: channel %u is not in DMA mode", v);
        INT3;
    }
    if (NV2A_REG32_MASK_BITSHIFT_TEST(p, NV_PFIFO, CACHE1_PUSH1, MODE, PIO)) {
        PRINTF_NV2A(XEXEC_DBG_ALL, "error: PIO not supported", 0);
        INT3;
    }

    get    = NV2A_REGADDR(p, NV_PRAMIN, BASE);
//PRINT(XEXEC_DBG_ALL,"    get:%p",get);//XXX
    get   += NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_DMA_INSTANCE, ADDRESS) << 2;
//PRINT(XEXEC_DBG_ALL,"    get + instance:%p | instance:%p",get,((NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_DMA_INSTANCE, ADDRESS)) << 4));//XXX
    dma    = NV2A_DMA(get);
    addr   = NV2A_DMA_ADDRESS(&dma);
//PRINT(XEXEC_DBG_ALL,"    addr:%p | flags:%p | limit:%p | frame:%p | class:%p | target:%p",addr,dma.flags,dma.limit,dma.frame,dma.class,dma.target);//XXX
//INT3;//XXX
    get    = NV2A_REGADDR(p + NV_USER_OBJECT__OFFSET(v, 0), NV_USER, DMA_GET);
    put    = NV2A_REGADDR(p + NV_USER_OBJECT__OFFSET(v, 0), NV_USER, DMA_PUT);
    c      = NV2A_REGADDR(p, NV_PFIFO, CACHE1_DMA_STATE);
    d      = NV2A_REGADDR(p, NV_PFIFO, CACHE1_DMA_DCOUNT);

    PRINTF_NV2A(
        XEXEC_DBG_VARDUMP,
        "begin: "
        "limit=0x%.08x | "
        "get=0x%.08x < put=0x%.08x",
        dma.limit,
        *get,
        *put);

    /* based on the convenient pseudocode in envytools */
    while (*get < *put) {
        if (*get > dma.limit) {
            c->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_PROTECTION;
            break;
        }
        xboxkrnl_read_dma(addr + *get, &v, 4);
        *get += 4;
        word  = v;
//PRINT(XEXEC_DBG_ALL,"pfifo_pusher: word/param:0x%.08x | method:0x%.03hx | subchannel:0x%.02hx | method_count:0x%.02hx | non_inc:%hhu | dcount:%u | error:%u",word,c->method,c->subchannel,c->method_count,c->non_inc,*d,c->error);//XXX
        if (c->method_count) {
            /* data word of methods command */
            NV2A_REG32(p, NV_PFIFO, CACHE1_DMA_DATA_SHADOW) = word;
            cmd       = *c;
            cmd.param = word;
            nv2a_pfifo_puller(p, &cmd);
            if (!c->non_inc) c->method += 4;
            --c->method_count;
            ++(*d);
        } else {
            /* no command active - this is the first word of a new one */
            NV2A_REG32(p, NV_PFIFO, CACHE1_DMA_RSVD_SHADOW) = word;
            /* match all forms */
            if ((word & 0xe0000003) == 0x20000000) {
                /* old jump */
                NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PFIFO, CACHE1_DMA_GET_JMP_SHADOW, OFFSET, *get);
                *get = NV2A_MASK_GET(word, 0x1ffffffc);
                PRINTF_NV2A(XEXEC_DBG_VARDUMP, "OLD_JMP 0x%.08x", *get);
            } else if ((word & 3) == 1) {
                /* jump */
                NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PFIFO, CACHE1_DMA_GET_JMP_SHADOW, OFFSET, *get);
                *get = NV2A_MASK_BITSHIFT_GET(word, NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET);
                PRINTF_NV2A(XEXEC_DBG_VARDUMP, "JMP 0x%.08x", *get);
            } else if ((word & 3) == 2) {
                /* call */
                if (NV2A_REG32_MASK_BITSHIFT_TEST(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, STATE, ACTIVE)) {
                    c->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_CALL;
                    break;
                }
                NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, RETURN_OFFSET, *get);
                NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, STATE, ACTIVE);
                *get = NV2A_MASK_BITSHIFT_GET(word, NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET);
                PRINTF_NV2A(XEXEC_DBG_VARDUMP, "CALL 0x%.08x", *get);
            } else if (word == 0x00020000) {
                /* return */
                if (NV2A_REG32_MASK_BITSHIFT_TEST(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, STATE, INACTIVE)) {
                    c->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_RETURN;
                    break;
                }
                *get = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, RETURN_OFFSET);
                NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PFIFO, CACHE1_DMA_SUBROUTINE, STATE, INACTIVE);
                PRINTF_NV2A(XEXEC_DBG_VARDUMP, "RET 0x%.08x", *get);
            } else if (!(word & 0xe0030003)) {
                /* increasing methods */
                c->field   = word;
                c->non_inc = 0;
                c->error   = 0;
                *d         = 0;
            } else if ((word & 0xe0030003) == 0x40000000) {
                /* non-increasing methods */
                c->field   = word;
                c->non_inc = 1;
                c->error   = 0;
                *d         = 0;
            } else {
                PRINTF_NV2A(XEXEC_DBG_VARDUMP, "RESERVED_CMD 0x%.08x, word 0x%.08x", *get, word);
                c->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_RESERVED_CMD;
                break;
            }
        }
    }

    PRINTF_NV2A(
        XEXEC_DBG_VARDUMP,
        "  end: "
        "limit=0x%.08x | "
        "get=0x%.08x < put=0x%.08x",
        dma.limit,
        *get,
        *put);

    if (c->error) {
        VARDUMP2_NV2A(DUMP, c->error, nv2a_pfifo_cache1_dma_state_error_name);
        NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PFIFO, CACHE1_DMA_PUSH, STATUS, SUSPENDED);
        NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PFIFO, INTR_0, DMA_PUSHER, PENDING);
        IRQ_NV2A_SIGNAL;
    }

    NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PFIFO, CACHE1_DMA_PUSH, STATE, IDLE);

    LEAVE_NV2A;
}

void
nv2a_framebuffer_set(uint32_t addr) {
    ENTER_NV2A;

    addr &= host->memreg_size - 1;
    NV2A_REG32(nv2a->memreg, NV_PCRTC, START) = addr;

    LEAVE_NV2A;
}

static int
nv2a_write(uint32_t addr, const void *val, size_t sz) {
    register const hw_block_t *b;
    register void *p;
    register uint32_t r;
    register uint32_t v;
    register uint32_t o;
    const char *reg;
    register int ret = 0;
    ENTER_NV2A;

    if (nv2a_offset(&addr)) {
        LEAVE_NV2A;
        return ret;
    }

    b = nv2a_block_lookup(addr, &reg);
    p = nv2a->memreg;
    r = addr - b->offset;

    switch (sz) {
    case 1:
        v = REG08(val);
        o = REG08(p + addr);
        PRINTF_NV2A(
            (b->index == NV_PRAMIN) ? XEXEC_DBG_DMA : XEXEC_DBG_REG,
            "[0x%.08x+0x%.08x] (0x%.02hhx)       <- 0x%.02hhx       | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            p,
            addr,
            o,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG08(p + addr) = v;
        ret = 1;
        break;
    case 2:
        v = REG16(val);
        o = REG16(p + addr);
        PRINTF_NV2A(
            (b->index == NV_PRAMIN) ? XEXEC_DBG_DMA : XEXEC_DBG_REG,
            "[0x%.08x+0x%.08x] (0x%.04hx)     <- 0x%.04hx     | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            p,
            addr,
            o,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG16(p + addr) = v;
        ret = 1;
        break;
    case 4:
        v = REG32(val);
        o = REG32(p + addr);
        PRINTF_NV2A(
            (b->index == NV_PRAMIN) ? XEXEC_DBG_DMA : XEXEC_DBG_REG,
            "[0x%.08x+0x%.08x] (0x%.08x) <- 0x%.08x | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            p,
            addr,
            o,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG32(p + addr) = v;
        ret = 1;
        break;
    default:
        LEAVE_NV2A;
        return ret;
    }

    switch (b->index) {
    case NV_PMC:
        switch (r) {
        case NV_PMC_INTR_0:
            NV2A_REG32(p, NV_PMC, INTR_0) = o & ~v;
            /* fall through */
        case NV_PMC_INTR_EN_0:
            IRQ_NV2A_SIGNAL;
            break;
        }
        break;
//    case NV_PBUS:
//        switch (r) {
//        }
//        break;
    case NV_PFIFO:
        switch (r) {
        case NV_PFIFO_INTR_0:
            NV2A_REG32(p, NV_PFIFO, INTR_0) = o & ~v;
            /* fall through */
        case NV_PFIFO_INTR_EN_0:
            IRQ_NV2A_SIGNAL;
            break;
        case NV_PFIFO_CACHE1_DMA_PUSH:
            if (NV2A_MASK_BITSHIFT_TEST(o, NV_PFIFO_CACHE1_DMA_PUSH_STATUS, SUSPENDED) &&
                NV2A_MASK_BITSHIFT_TEST(v, NV_PFIFO_CACHE1_DMA_PUSH_STATUS, RUNNING)) {
                NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PFIFO, CACHE1_DMA_PUSH, STATE, BUSY);
                EVENT_SIGNAL;//TODO rename for nv2a_pfifo_pusher()
            }
            break;
        case NV_PFIFO_CACHE1_DMA_PUT:
            o = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID);
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p + NV_USER_OBJECT__OFFSET(o, 0), NV_USER, DMA_PUT, OFFSET, v);
            break;
        case NV_PFIFO_CACHE1_DMA_GET:
            o = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID);
            NV2A_REG32_MASK_BITSHIFT_SET_VAL(p + NV_USER_OBJECT__OFFSET(o, 0), NV_USER, DMA_GET, OFFSET, v);
            break;
        }
        break;
    case NV_PVIDEO:
#if 0 //TODO
static void pvideo_vga_invalidate(NV2AState *d)
{
    int y1 = GET_MASK(d->pvideo.regs[NV_PVIDEO_POINT_OUT],
                      NV_PVIDEO_POINT_OUT_Y);
    int y2 = y1 + GET_MASK(d->pvideo.regs[NV_PVIDEO_SIZE_OUT],
                           NV_PVIDEO_SIZE_OUT_HEIGHT);
    NV2A_DPRINTF("pvideo_vga_invalidate %d %d\n", y1, y2);
    vga_invalidate_scanlines(&d->vga, y1, y2);
}
#endif
        switch (r) {
        case NV_PVIDEO_BUFFER:
//            d->vga.enable_overlay = true;
//            pvideo_vga_invalidate(d);
            break;
        case NV_PVIDEO_STOP:
            NV2A_REG32(p, NV_PVIDEO, BUFFER) = 0;
//            d->vga.enable_overlay = false;
//            pvideo_vga_invalidate(d);
            break;
        }
        break;
    case NV_PTIMER:
        switch (r) {
        case NV_PTIMER_INTR_0:
            NV2A_REG32(p, NV_PTIMER, INTR_0) = o & ~v;
            /* fall through */
        case NV_PTIMER_INTR_EN_0:
            IRQ_NV2A_SIGNAL;
            break;
        }
        break;
//    case NV_PFB:
//        switch (r) {
//        }
//        break;
    case NV_PGRAPH:
        switch (r) {
        case NV_PGRAPH_INTR:
            NV2A_REG32(p, NV_PGRAPH, INTR) = o & ~v;
            nv2a_irq_raise(NV2A_IRQ_PGRAPH);
            break;
        case NV_PGRAPH_FIFO:
            if (NV2A_MASK_BITSHIFT_TEST(o, NV_PGRAPH_FIFO_ACCESS, DISABLED) &&
                NV2A_MASK_BITSHIFT_TEST(v, NV_PGRAPH_FIFO_ACCESS, ENABLED)) {
                nv2a_irq_raise(NV2A_IRQ_FIFO);
            }
            break;
        case NV_PGRAPH_INCREMENT:
            if (NV2A_MASK_BITSHIFT_TEST(v, NV_PGRAPH_INCREMENT_READ_3D, TRIGGER)) {
                v = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, SURFACE, READ_3D) + 1;
                o = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PGRAPH, SURFACE, MODULO_3D);
                if (o) v %= o;
                NV2A_REG32_MASK_BITSHIFT_SET_VAL(p, NV_PGRAPH, SURFACE, READ_3D, v);
                FLIP_STALL_SIGNAL;
            }
            break;
        case NV_PGRAPH_CHANNEL_CTX_TRIGGER:
            if (NV2A_MASK_BITSHIFT_TEST(v, NV_PGRAPH_CHANNEL_CTX_TRIGGER_READ_IN, ACTIVATE)) {
                o = NV2A_REG32(p, NV_PGRAPH, CHANNEL_CTX_POINTER) << 4;
                v = NV2A_REG32(p + o, NV_PRAMIN, BASE);
                NV2A_REG32(p, NV_PGRAPH, CTX_USER) = v;
PRINT(XEXEC_DBG_ALL,"ctx_user: 0x%.08x",v);//XXX make sure this works
INT3;//XXX make sure this works
            }
            if (NV2A_MASK_BITSHIFT_TEST(v, NV_PGRAPH_CHANNEL_CTX_TRIGGER_WRITE_OUT, ACTIVATE)) {
                /* do stuff ... */
            }
            break;
        }
        break;
    case NV_PCRTC:
        switch (r) {
        case NV_PCRTC_INTR_0:
            NV2A_REG32(p, NV_PCRTC, INTR_0) = o & ~v;
            /* fall through */
        case NV_PCRTC_INTR_EN_0:
            IRQ_NV2A_SIGNAL;
            break;
        case NV_PCRTC_START:
            nv2a_framebuffer_set(v);
            break;
        }
        break;
    case NV_USER:
//PRINT(XEXEC_DBG_ALL,"nv_user: (mode:0x%.08x & (1 << chid:%u)) == %s",NV2A_REG32(p,NV_PFIFO,MODE),NV2A_MASK_BITSHIFT_GET(r,NV_USER_CHID),(NV2A_REG32(p,NV_PFIFO,MODE) & (1 << NV2A_MASK_BITSHIFT_GET(r,NV_USER_CHID)))?"true":"false");//XXX
        if (NV2A_REG32(p, NV_PFIFO, MODE) & (1 << NV2A_MASK_BITSHIFT_GET(r, NV_USER_CHID))) {
            /* dma */
            switch (NV2A_MASK_BITSHIFT_GET(r, NV_USER_REG)) {
            case NV_USER_DMA_PUT:
                NV2A_REG32_MASK_BITSHIFT_SET(p, NV_PFIFO, CACHE1_DMA_PUSH, STATE, BUSY);
                EVENT_SIGNAL;//TODO rename for nv2a_pfifo_pusher()
                break;
            }
        } else {
            /* pio */
            PRINTF_NV2A(XEXEC_DBG_ALL, "/* FIXME: pio not supported */", 0);
            INT3;
        }
        break;
    }

    LEAVE_NV2A;
    return ret;
}

static int
nv2a_read(uint32_t addr, void *val, size_t sz) {
    register const hw_block_t *b;
    register void *p;
    register uint32_t r;
    register uint32_t v;
    const char *reg;
    register int ret = 0;
    ENTER_NV2A;

    if (nv2a_offset(&addr)) {
        LEAVE_NV2A;
        return ret;
    }

    b = nv2a_block_lookup(addr, &reg);
    p = nv2a->memreg;
    r = addr - b->offset;
    v = REG32(p + addr);

    switch (b->index) {
    case NV_PMC:
        switch (r) {
        case NV_PMC_BOOT_0:
            v = NV_PMC_BOOT_0_ID_NV2A_A2_DEVID0; /* NV2A, A02, Rev 0 */
            break;
        }
        break;
    case NV_PBUS:
        switch (r) {
        case NV_PBUS_FBIO_RAM:
            v = 0x00000114; /* RAM capabilities */
            break;
        case NV_PBUS_PCI_NV_0:
            v = 0x02a010de; /* NV2A, NVIDIA */
            break;
        case NV_PBUS_PCI_NV_2:
            v = 0x030000a1; /* VGA, A01 */
            break;
        case NV_PBUS_PCI_NV_4:
            v = 0xfd000000; /* BAR descriptor */
            break;
        case NV_PBUS_PCI_NV_5:
            v = 0xf0000008; /* BAR descriptor */
            break;
        case NV_PBUS_PCI_NV_6:
            v = 0x00000008; /* BAR descriptor */
            break;
        case NV_PBUS_PCI_NV_15:
            v = 0x01050103; /* IRQ, latency */
            break;
        case NV_PBUS_PCI_NV_17:
            v = 0x00200002; /* AGP revision */
            break;
        case NV_PBUS_PCI_NV_18:
            v = 0x1f000017; /* AGP status */
            break;
        case NV_PBUS_PCI_NV_19:
            v = 0x00000114; /* AGP command */
            break;
        case NV_PBUS_PCI_NV_21:
            v = 0x00000001; /* VGA enabled */
            break;
        case NV_PBUS_PCI_NV_22:
            v = NV_PBUS_PCI_NV_22_SCRATCH_DEFAULT;
            break;
        case NV_PBUS_PCI_NV_23:
            v = NV_PBUS_PCI_NV_23_DT_TIMEOUT_16;
            break;
        case NV_PBUS_PCI_NV_24:
            v = 0x00024401;
            break;
        }
        break;
    case NV_PFIFO:
        switch (r) {
        case NV_PFIFO_RUNOUT_STATUS:
            NV2A_MASK_BITSHIFT_SET(v, NV_PFIFO_RUNOUT_STATUS_LOW_MARK, EMPTY);
            break;
        case NV_PFIFO_CACHE1_STATUS:
            if (NV2A_REG32_MASK_BITSHIFT_TEST(p, NV_PFIFO, CACHE1_DMA_PUSH, STATE, BUSY)) {
                NV2A_MASK_BITSHIFT_SET(v, NV_PFIFO_CACHE1_STATUS_LOW_MARK, NOT_EMPTY);
            } else {
                NV2A_MASK_BITSHIFT_SET(v, NV_PFIFO_CACHE1_STATUS_LOW_MARK, EMPTY);
            }
            break;
        case NV_PFIFO_CACHE1_DMA_PUSH:
            NV2A_MASK_BITSHIFT_SET(v, NV_PFIFO_CACHE1_DMA_PUSH_BUFFER, EMPTY);
            break;
        case NV_PFIFO_CACHE1_DMA_PUT:
            v = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID);
            v = NV2A_REG32_MASK_BITSHIFT_GET(p + NV_USER_OBJECT__OFFSET(v, 0), NV_USER, DMA_PUT, OFFSET);
            break;
        case NV_PFIFO_CACHE1_DMA_GET:
            v = NV2A_REG32_MASK_BITSHIFT_GET(p, NV_PFIFO, CACHE1_PUSH1, CHID);
            v = NV2A_REG32_MASK_BITSHIFT_GET(p + NV_USER_OBJECT__OFFSET(v, 0), NV_USER, DMA_GET, OFFSET);
            break;
        }
        break;
    case NV_PVIDEO:
        switch (r) {
        case NV_PVIDEO_STOP:
            v = 0;
            break;
        }
        break;
#if 0 //TODO
nv2a_initfn:
d->pramdac.core_clock_coeff = 0x00011c01; /* 189MHz...? */
d->pramdac.core_clock_freq = 189000000;
d->pramdac.memory_clock_coeff = 0;
d->pramdac.video_clock_coeff = 0x0003C20D; /* 25182Khz...? */

pramdac_write:
case NV_PRAMDAC_NVPLL_COEFF:
    d->pramdac.core_clock_coeff = val;

    m = val & NV_PRAMDAC_NVPLL_COEFF_MDIV;
    n = (val & NV_PRAMDAC_NVPLL_COEFF_NDIV) >> 8;
    p = (val & NV_PRAMDAC_NVPLL_COEFF_PDIV) >> 16;

    if (m == 0) {
        d->pramdac.core_clock_freq = 0;
    } else {
        d->pramdac.core_clock_freq = (NV2A_CRYSTAL_FREQ * n)
                                      / (1 << p) / m;
    }

static uint64_t ptimer_get_clock(NV2AState *d)
{
    return muldiv64(qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL),
                    d->pramdac.core_clock_freq * d->ptimer.numerator,
                    get_ticks_per_sec() * d->ptimer.denominator);
}

    case NV_PTIMER:
        switch (r) {
        case NV_PTIMER_TIME_0:
            r = (ptimer_get_clock(d) & 0x7ffffff) << 5;
            break;
        case NV_PTIMER_TIME_1:
            r = (ptimer_get_clock(d) >> 27) & 0x1fffffff;
            break;
        }
        break;
#endif
    case NV_PFB:
        switch (r) {
        case NV_PFB_CFG0:
            v = NV_PFB_CFG0_PART_4; /* 3-4 memory partitions; debug bios checks this */
            break;
        case NV_PFB_CSTATUS:
            v = host->memreg_size; /* vram size */
            break;
        case NV_PFB_WBC:
            if (NV2A_REG32_MASK_BITSHIFT_TEST(p, NV_PFIFO, CACHE1_DMA_PUSH, STATE, BUSY)) {
                NV2A_MASK_BITSHIFT_SET(v, NV_PFB_WBC_FLUSH, PENDING);
            } else {
                NV2A_MASK_BITSHIFT_SET(v, NV_PFB_WBC_FLUSH, NOT_PENDING);
            }
            break;
        }
        break;
    case NV_PGRAPH:
        switch (r) {
        case NV_PGRAPH_CTX_USER:
            NV2A_MASK_BITSHIFT_SET(v, NV_PGRAPH_CTX_USER_CHANNEL_3D_VALID, TRUE);
            break;
        }
        break;
//    case NV_PCRTC:
//        switch (r) {
//        }
//        break;
    case NV_PRAMDAC:
        switch (r) {
        case NV_PRAMDAC_PLL_TEST_COUNTER:
            NV2A_MASK_BITSHIFT_SET(v, NV_PRAMDAC_PLL_TEST_COUNTER_VPLL2_LOCK, LOCKED);
            NV2A_MASK_BITSHIFT_SET(v, NV_PRAMDAC_PLL_TEST_COUNTER_NVPLL_LOCK, LOCKED);
            NV2A_MASK_BITSHIFT_SET(v, NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCK, LOCKED);
            NV2A_MASK_BITSHIFT_SET(v, NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCK, LOCKED);
            break;
        }
        break;
//    case NV_USER:
//        switch (r) {
//        }
//        break;
    }

    switch (sz) {
    case 1:
        v &= 0xff;
        PRINTF_NV2A(
            XEXEC_DBG_REG,
            " [0x%.08x+0x%.08x]              -> 0x%.02hhx       | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            p,
            addr,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG08(val) = v;
        ret = 1;
        break;
    case 2:
        v &= 0xffff;
        PRINTF_NV2A(
            XEXEC_DBG_REG,
            " [0x%.08x+0x%.08x]              -> 0x%.04hx     | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            p,
            addr,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG16(val) = v;
        ret = 1;
        break;
    case 4:
        PRINTF_NV2A(
            XEXEC_DBG_REG,
            " [0x%.08x+0x%.08x]              -> 0x%.08x | "
            "block: '%s' | "
            "reg: 0x%x%s%s%s",
            p,
            addr,
            v,
            b->name,
            r,
            (reg) ? " (" : "",
            (reg) ?  reg : "",
            (reg) ?  ")" : "");
        REG32(val) = v;
        ret = 1;
        break;
    default:
        break;
    }

    LEAVE_NV2A;
    return ret;
}

static int
nv2a_destroy(void) {
    register uint32_t i;
    ENTER_NV2A;

    if (!nv2a_ctx) {
        LEAVE_NV2A;
        return 1;
    }

    glo_current(nv2a_ctx->glo, 1);
    glDeleteVertexArrays(1, &nv2a_ctx->gl_vertex_array);
//    glDeleteBuffers(1, &nv2a_ctx->gl_memory_buffer);
    glDeleteBuffers(1, &nv2a_ctx->gl_inline_array_buffer);
    glDeleteBuffers(1, &nv2a_ctx->gl_inline_element_buffer);
    for (i = 0; i < NV2A_MAX_VERTEX_ATTRIBS; ++i) {
        glDeleteBuffers(1, &nv2a_ctx->va[i].gl_inline_buffer);
        glDeleteBuffers(1, &nv2a_ctx->va[i].gl_converted_buffer);
    }
    glDeleteTextures(1, &nv2a_ctx->gl_zeta_buffer);
    glDeleteTextures(1, &nv2a_ctx->gl_color_buffer);
    glDeleteFramebuffers(1, &nv2a_ctx->gl_framebuffer);
    glo_destroy(&nv2a_ctx->glo);

    free(nv2a_ctx);
    nv2a_ctx = NULL;

    LEAVE_NV2A;
    return 0;
}

static int
nv2a_init(void) {
    GLint tmp;
    register uint32_t i;
    register int ret = 1;
    ENTER_NV2A;

    nv2a_destroy();

    nv2a_ctx = calloc(1, sizeof(*nv2a_ctx));

    do {
        if (glo_create(&nv2a_ctx->glo)) break;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &tmp);
        if (tmp < NV2A_MAX_VERTEX_ATTRIBS) {
            PRINTF_NV2A(
                XEXEC_DBG_ALL,
                "error: not enough GL vertex attributes supported: have %i, expected %zu\n",
                tmp,
                NV2A_MAX_VERTEX_ATTRIBS);
            break;
        }
        glGenFramebuffers(1, &nv2a_ctx->gl_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, nv2a_ctx->gl_framebuffer);
        glGenTextures(1, &nv2a_ctx->gl_color_buffer);
        glBindTexture(GL_TEXTURE_2D, nv2a_ctx->gl_color_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 640, 480, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nv2a_ctx->gl_color_buffer, 0);
        if ((tmp = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
            PRINTF_NV2A(
                XEXEC_DBG_ALL,
                "error: GL framebuffer operation did not complete: error %i\n",
                tmp);
            break;
        }
        for (i = 0; i < NV2A_MAX_VERTEX_ATTRIBS; ++i) {
            glGenBuffers(1, &nv2a_ctx->va[i].gl_converted_buffer);
            glGenBuffers(1, &nv2a_ctx->va[i].gl_inline_buffer);
        }
        glGenBuffers(1, &nv2a_ctx->gl_inline_element_buffer);
        glGenBuffers(1, &nv2a_ctx->gl_inline_array_buffer);
//        glGenBuffers(1, &nv2a_ctx->gl_memory_buffer);
//        glBindBuffer(GL_ARRAY_BUFFER, nv2a_ctx->gl_memory_buffer);
//        glBufferData(GL_ARRAY_BUFFER, 0x08000000, NULL, GL_DYNAMIC_DRAW);
        glGenVertexArrays(1, &nv2a_ctx->gl_vertex_array);
        glBindVertexArray(nv2a_ctx->gl_vertex_array);
        if ((ret = ((tmp = glGetError()) != GL_NO_ERROR))) {
            PRINTF_NV2A(
                XEXEC_DBG_ALL,
                "error: GL error occurred: error %i\n",
                tmp);
            break;
        }
        glo_current(nv2a_ctx->glo, 0);
    } while (0);

    if (ret) nv2a_destroy();

    LEAVE_NV2A;
    return ret;
}

static void
nv2a_reset(void) {
    //TODO disconnect irq
    //TODO free resources
}

const hw_ops_t nv2a_op = {
    .init           = nv2a_init,
    .destroy        = nv2a_destroy,
    .reset          = nv2a_reset,
    .irq            = nv2a_irq,
    .irq_raise      = nv2a_irq_raise,
    .irq_restore    = nv2a_irq_restore,
    .write          = nv2a_write,
    .read           = nv2a_read,
};

