/**************************************************************************/
/*  context_egl_vita.cpp                                                  */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "context_egl_vita.h"

#ifndef PVR_PSP2
#define MEMORY_MAIN_THRESHOLD 0
#define MEMORY_GFX_THRESHOLD 0
#define MEMORY_PHYCONT_THRESHOLD 0
#define MEMORY_CDGL_THRESHOLD 26 * 1024 * 1024

static bool b_vglInitialized = 0;
#endif

#ifdef PVR_PSP2
void ContextEGL_Vita::release_current() {
	eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, context);
}

void ContextEGL_Vita::make_current() {
	eglMakeCurrent(display, surface, surface, context);
}
#endif

int ContextEGL_Vita::get_window_width() {
#ifdef PVR_PSP2
	return width;
#else
	return 960;
#endif
}

int ContextEGL_Vita::get_window_height() {
#ifdef PVR_PSP2
	return height;
#else
	return 544;
#endif
}

void ContextEGL_Vita::reset() {
	cleanup();
	initialize();
}

void ContextEGL_Vita::swap_buffers() {
#ifdef PVR_PSP2
	if (eglSwapBuffers(display, surface) != EGL_TRUE) {
		cleanup();
		initialize();
	}
#else
	vglSwapBuffers(GL_FALSE);
#endif
};

Error ContextEGL_Vita::initialize() {
#ifdef PVR_PSP2
	// Get an appropriate EGL framebuffer configuration
	static const EGLint attributeList[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 8,
		EGL_STENCIL_SIZE, 8,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	static const EGLint contextAttributeList[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	EGLConfig config = nullptr;
	EGLint numConfigs = 0;

	// Initialize PVR_PSP2
	PVRSRV_PSP2_APPHINT hint;

	sceKernelLoadStartModule("app0:module/libgpu_es4_ext.suprx", 0, NULL, 0, NULL, NULL);
	sceKernelLoadStartModule("app0:module/libIMGEGL.suprx", 0, NULL, 0, NULL, NULL);

	PVRSRVInitializeAppHint(&hint);
	hint.ui32SwTexOpCleanupDelay = 16000;
	PVRSRVCreateVirtualAppHint(&hint);

	// Connect to the EGL default display
	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (!display) {
		goto _fail0;
	}

	// Initialize the display
	eglInitialize(display, NULL, NULL);

	// Get avaiable EGL framebuffer configurations
	eglChooseConfig(display, attributeList, &config, 1, &numConfigs);
	if (numConfigs == 0) {
		goto _fail1;
	}

	// Create an EGL window surface
	window.type = PSP2_DRAWABLE_TYPE_WINDOW;
	window.numFlipBuffers = 2;
	window.flipChainThrdAffinity = 0x20000;
	window.windowSize = PSP2_WINDOW_960X544;

	// Create a window surface
	surface = eglCreateWindowSurface(display, config, &window, NULL);
	if (!surface) {
		goto _fail1;
	}

	// Create an EGL rendering context
	context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributeList);
	if (!context) {
		goto _fail2;
	}

	// Connect the context to the surface
	eglMakeCurrent(display, surface, surface, context);

	eglQuerySurface(display, surface, EGL_WIDTH, &width);
	eglQuerySurface(display, surface, EGL_HEIGHT, &height);
#else
	if (!b_vglInitialized) {
		vglInitExtended(0, 960, 544, 8 * 1024 * 1024, SCE_GXM_MULTISAMPLE_NONE);
		vglUseExtraMem(GL_FALSE);
		vglSetSemanticBindingMode(VGL_MODE_GLOBAL);
		b_vglInitialized = 1;
	}
#endif
	return OK;
#ifdef PVR_PSP2
_fail2:
	eglDestroySurface(display, surface);
	surface = NULL;
_fail1:
	eglTerminate(display);
	display = NULL;
_fail0:
	return ERR_UNCONFIGURED;
#endif
}

void ContextEGL_Vita::cleanup() {
#ifdef PVR_PSP2
	if (display != EGL_NO_DISPLAY && surface != EGL_NO_SURFACE) {
		eglDestroySurface(display, surface);
		surface = EGL_NO_SURFACE;
	}

	if (display != EGL_NO_DISPLAY && context != EGL_NO_CONTEXT) {
		eglDestroyContext(display, context);
		context = EGL_NO_CONTEXT;
	}

	if (display != EGL_NO_DISPLAY) {
		eglTerminate(display);
		display = EGL_NO_DISPLAY;
	}
#else
	if (b_vglInitialized) {
		b_vglInitialized = 0;
		vglEnd();
	}
#endif
}

ContextEGL_Vita::ContextEGL_Vita(bool gles) :
		gles2_context(gles) {}

ContextEGL_Vita::~ContextEGL_Vita() {
	cleanup();
}
