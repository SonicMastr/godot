/**************************************************************************/
/*  godot_vita.cpp                                                        */
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

#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>

#include "main/main.h"
#include "os_vita.h"

#include <taihen.h>

#ifdef PVR_PSP2
#ifndef MEMORY_GRAPHICS_MB
#define MEMORY_GRAPHICS_MB 256 // Default Split, 256 Graphics/221 Main
#endif
#define MEMORY_NEWLIB_MB (477 - MEMORY_GRAPHICS_MB)
#define MEMORY_SCELIBC_MB 10

//#define DEVKIT_ENABLED 1

int _newlib_heap_size_user = MEMORY_NEWLIB_MB * 1024 * 1024;
unsigned int sceLibcHeapSize = MEMORY_SCELIBC_MB * 1024 * 1024;
#else
int _newlib_heap_size_user = 256 * 1024 * 1024;
#endif

int main(int argc, char *argv[]) {
	OS_Vita os;
#ifdef PVR_PSP2
	char title_id[0xA];
	char app_dir_path[0x100];
	char app_kernel_module_path[0x100];
	SceUID pid = -1;
	sceKernelLoadStartModule("vs0:sys/external/libfios2.suprx", 0, NULL, 0, NULL, NULL);
	sceKernelLoadStartModule("vs0:sys/external/libc.suprx", 0, NULL, 0, NULL, NULL);

	pid = sceKernelGetProcessId();
	sceAppMgrAppParamGetString(pid, 12, title_id, sizeof(title_id));
	snprintf(app_dir_path, sizeof(app_dir_path), "ux0:app/%s", title_id);
	snprintf(app_kernel_module_path, sizeof(app_kernel_module_path), "%s/module/libgpu_es4_kernel_ext.skprx", app_dir_path);

	SceUID res = taiLoadStartKernelModule(app_kernel_module_path, 0, NULL, 0);
	if (res < 0) {
		sceClibPrintf("Failed to load kernel module: %08x\n", res);
	}
#endif
	scePowerSetArmClockFrequency(444);
	scePowerSetBusClockFrequency(222);
	scePowerSetGpuClockFrequency(222);
	scePowerSetGpuXbarClockFrequency(166);

	sceClibPrintf("Showing the path now UwU: %d %s\n", argc, argv[0]);
	char *args[] = { "--path", "app0:/game_data", "--main-pack", "app0:/game_data/game.pck" };

	Error err = Main::setup("", sizeof(args) / sizeof(args[0]), args);
	if (err != OK) {
		return 255;
	}

	if (Main::start())
		os.run(); // it is actually the OS that decides how to run
	Main::cleanup();
	return 0;
}
