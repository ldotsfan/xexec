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

#include <X11/Xlib.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>

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
    static int glo_glewInit = 1;
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

