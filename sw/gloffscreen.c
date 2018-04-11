/*
 *  xexec - XBE x86 direct execution LLE & XBOX kernel POSIX translation HLE
 *
 *  Offscreen OpenGL abstraction layer for GLX (X11)
 *
 *  Copyright (c) 2013 Wayo
 *  Copyright (c) 2014 Jannik Vogel
 *  Copyright (c) 2017 Michael Saga. All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 */

#include <X11/Xlib.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>

static int glo_glewInit = 1;

typedef struct {
    void *      x_display;
    GLXDrawable glx_drawable;
    GLXContext  glx_context;
} glo_context;

static const int fb_attribute_list[] = {
    GLX_RENDER_TYPE,   GLX_RGBA_BIT,
    GLX_RED_SIZE,      8,
    GLX_GREEN_SIZE,    8,
    GLX_BLUE_SIZE,     8,
    GLX_ALPHA_SIZE,    8,
    GLX_DEPTH_SIZE,    24,
    GLX_STENCIL_SIZE,  8,
    GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT,
//    GLX_DOUBLEBUFFER,  True,
    None
};

static const int surface_attribute_list[] = {
    GLX_PBUFFER_WIDTH,   16,
    GLX_PBUFFER_HEIGHT,  16,
    GLX_LARGEST_PBUFFER, True,
    None
};

static const int context_attribute_list[] = {
    GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
    GLX_CONTEXT_MINOR_VERSION_ARB, 3,
    GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
    None
};

static PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = NULL;

void
glo_current(glo_context *c, int on) {
    if (!c) return;
    if (!on) glXMakeCurrent(c->x_display, None, NULL);
    else glXMakeCurrent(c->x_display, c->glx_drawable, c->glx_context);
}

void
glo_destroy(glo_context **c) {
    if (!c || !*c) return;
    glo_current(*c, 0);
    if ((*c)->glx_context) {
        glXDestroyContext((*c)->x_display, (*c)->glx_context);
        (*c)->glx_context = NULL;
    }
    if ((*c)->glx_drawable) {
        glXDestroyPbuffer((*c)->x_display, (*c)->glx_drawable);
        (*c)->glx_drawable = None;
    }
    if ((*c)->x_display) {
        XCloseDisplay((*c)->x_display);
        (*c)->x_display = NULL;
    }
    free(*c);
    *c = NULL;
}

void *
glo_address(const char *proc) {
    return (void *)glXGetProcAddress((void *)proc);
}

int
glo_extension(const char *ext) {
    register size_t i;
    register const char *n;

    for (i = 0; i < GL_NUM_EXTENSIONS; ++i) {
        if (!(n = (typeof(n))glGetStringi(GL_EXTENSIONS, i))) break;
        if (!strcmp(ext, n)) return 1;
    }

    return 0;
}

int
glo_create(glo_context **c) {
    GLXFBConfig *fbc;
    int tmp;
    register int ret = 1;

    if (!c || *c) return ret;

    *c = calloc(1, sizeof(**c));

    do {
        if (!glXCreateContextAttribsARB) {
            if (!(glXCreateContextAttribsARB = glo_address("glXCreateContextAttribsARB"))) {
                fprintf(stderr, "error: gloffscreen: GLX does not support the ARB_create_context extension\n");
                break;
            }
        }
        if (!((*c)->x_display = XOpenDisplay(NULL))) {
            fprintf(stderr, "error: gloffscreen: XOpenDisplay() failed\n");
            break;
        }
        printf("gloffscreen: GLX_VERSION = %s\n", glXGetClientString((*c)->x_display, GLX_VERSION));
        printf("gloffscreen: GLX_VENDOR = %s\n", glXGetClientString((*c)->x_display, GLX_VENDOR));
        if (!(fbc = glXChooseFBConfig((*c)->x_display, DefaultScreen((*c)->x_display), fb_attribute_list, &tmp))) {
            fprintf(stderr, "error: gloffscreen: glXChooseFBConfig() failed: error %i\n", glGetError());
            break;
        }
        if (!tmp) {
            fprintf(stderr, "error: gloffscreen: glXChooseFBConfig() failed: 0 nelements\n");
            break;
        }
        if (!((*c)->glx_drawable = glXCreatePbuffer((*c)->x_display, *fbc, surface_attribute_list))) {
            fprintf(stderr, "error: gloffscreen: glXCreatePbuffer() failed: error %i\n", glGetError());
            break;
        }
        if (!((*c)->glx_context = glXCreateContextAttribsARB((*c)->x_display, *fbc, 0, True, context_attribute_list))) {
            fprintf(stderr, "error: gloffscreen: glXCreateContextAttribsARB() failed: error %i\n", glGetError());
            break;
        }
        XSync((*c)->x_display, False);
        glo_current(*c, 1);
        if (glo_glewInit) {
            glewExperimental = GL_TRUE;
            if ((tmp = glewInit()) != GLEW_OK) {
                fprintf(stderr, "error: gloffscreen: glewInit() failed: error %i: '%s'\n", tmp, glewGetErrorString(tmp));
                break;
            }
            while (glGetError() != GL_NO_ERROR);
            glo_glewInit = 0;
            if (!glo_extension("GL_EXT_texture_compression_s3tc")) { /* DXT textures */
                fprintf(stderr, "error: gloffscreen: extension GL_EXT_texture_compression_s3tc not found\n");
                break;
            }
            if (!glo_extension("GL_ARB_ES2_compatibility")) { /* internal RGB565 texture format */
                fprintf(stderr, "error: gloffscreen: extension GL_ARB_ES2_compatibility not found\n");
                break;
            }
        }
    } while ((ret = 0));

    if (ret) glo_destroy(c);

    return ret;
}

void
glo_readpixels(
        GLenum gl_format,
        GLenum gl_type,
        uint32_t bytes_per_pixel,
        uint32_t stride,
        uint32_t width,
        uint32_t height,
        void *out) {
    register void *a, *b, *c;
    register int i, h;
    register size_t w;
    GLint prl, pa;

    if (!bytes_per_pixel || !stride || !width || !height || !out) INT3;
    if (stride % bytes_per_pixel) INT3; //TODO: weird strides

    /* save GL state */
    glGetIntegerv(GL_PACK_ROW_LENGTH, &prl);
    glGetIntegerv(GL_PACK_ALIGNMENT, &pa);

    glPixelStorei(GL_PACK_ROW_LENGTH, stride / bytes_per_pixel);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    if (height % 2) {
        /* slow flip */
        w = width;
        a = out;
        for (i = height - 1; i >= 0; --i) {
            glReadPixels(0, i, w, 1, gl_format, gl_type, a);
            a += stride;
        }
    } else {
        /* fast flip */
        glReadPixels(0, 0, width, height, gl_format, gl_type, out);
        w = width * bytes_per_pixel;
        a = out;
        b = a + stride * (height - 1);
        if (!(c = malloc(w))) INT3;
        for (h = height / 2, i = 0; i < h; ++i) {
            memcpy(c, a, w);
            memcpy(a, b, w);
            memcpy(b, c, w);
            a += stride;
            b -= stride;
        }
        free(c);
    }

    /* restore GL state */
    glPixelStorei(GL_PACK_ROW_LENGTH, prl);
    glPixelStorei(GL_PACK_ALIGNMENT, pa);
}

