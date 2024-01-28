/**************************************************************************/
/*  context_egl_vita.h                                                    */
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

#ifndef CONTEXT_EGL_VITA_H
#define CONTEXT_EGL_VITA_H

#include "core/os/os.h"
#include <psp2/kernel/modulemgr.h>
#include <psp2/sysmodule.h>
#include <psp2/types.h>

#include <EGL/egl.h> // EGL library
#ifdef PVR_PSP2
extern "C" {
#include <gpu_es4/psp2_pvr_hint.h>
}
#else
#define HAVE_GL_HEADERS
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>
#include <vitaGL.h>
#endif
class ContextEGL_Vita {
#ifdef PVR_PSP2
	Psp2NativeWindow window;
#endif
	EGLDisplay display;
	EGLContext context;
	EGLSurface surface;

	EGLint width;
	EGLint height;

	bool vsync;
	bool gles2_context;

public:
#ifdef PVR_PSP2
	void release_current();

	void make_current();
#endif
	int get_window_width();
	int get_window_height();
	void swap_buffers();

	void set_use_vsync(bool use) { vsync = use; }
	bool is_using_vsync() const { return vsync; }

	Error initialize();
	void reset();

	void cleanup();

	ContextEGL_Vita(bool gles2);
	~ContextEGL_Vita();
};

#endif // CONTEXT_EGL_VITA_H
