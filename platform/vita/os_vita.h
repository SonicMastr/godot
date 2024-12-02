/**************************************************************************/
/*  os_vita.h                                                             */
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

#ifndef OS_VITA_H
#define OS_VITA_H

#include "audio_driver_vita.h"
#include "context_egl_vita.h"
#include "core/os/os.h"
#include "drivers/dummy/texture_loader_dummy.h"
#include "drivers/unix/os_unix.h"
#include "joypad_vita.h"
#include "main/input_default.h"
#include "servers/audio_server.h"
#include "servers/visual/rasterizer.h"

#include <psp2/appmgr.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/libime.h>
#include <psp2/power.h>
#include <psp2/rtc.h>
#include <psp2/sysmodule.h>

class OS_Vita : public OS {
	bool secondary_gl_available = false;

	ContextEGL_Vita *gl_context;
	AudioDriverVita driver_vita;
	VisualServer *visual_server;
	VideoMode video_mode;
	int video_driver_index;

	InputDefault *input;
	JoypadVita *joypad;

	Vector2 last_touch_pos[SCE_TOUCH_MAX_REPORT];
	SceTouchData touch;
	SceTouchPanelInfo front_panel_info;
	Vector2 front_panel_size;
	void process_touch();

	SceMotionState motion_state;
	void process_motion();
	void process_accelerometer(const Vector3 &m_accelerometer);
	void process_gravity(const Vector3 &m_gravity);
	void process_magnetometer(const Vector3 &m_magnetometer);
	void process_gyroscope(const Vector3 &m_gyroscope);

	MainLoop *main_loop;

protected:
	virtual void initialize_core();
	virtual Error initialize(const VideoMode &p_desired, int p_video_driver, int p_audio_driver);

	virtual bool is_offscreen_gl_available() const;
	virtual void set_offscreen_gl_current(bool p_current);

	virtual void set_main_loop(MainLoop *p_main_loop);
	virtual void delete_main_loop();

	virtual void finalize();
	virtual void finalize_core();

public:
	virtual void alert(const String &p_alert, const String &p_title = "ALERT!");

	virtual int get_video_driver_count() const;

	virtual int get_audio_driver_count() const;
	virtual const char *get_audio_driver_name(int p_driver) const;

	virtual int get_current_video_driver() const;

	virtual Point2 get_mouse_position() const;
	virtual int get_mouse_button_state() const;
	virtual void set_window_title(const String &p_title);

	virtual void set_video_mode(const VideoMode &p_video_mode, int p_screen = 0);
	virtual VideoMode get_video_mode(int p_screen = 0) const;
	virtual void get_fullscreen_mode_list(List<VideoMode> *p_list, int p_screen = 0) const;

	virtual Size2 get_window_size() const;

	virtual String get_name() const;
	virtual MainLoop *get_main_loop() const;

	virtual void swap_buffers();

	virtual bool can_draw() const;

	void key(uint32_t p_key, bool p_pressed);
	virtual bool has_touchscreen_ui_hint() const;
	virtual bool has_virtual_keyboard() const;
	virtual void show_virtual_keyboard(const String &p_existing_text, const Rect2 &p_screen_rect = Rect2(), bool p_multiline = false, int p_max_input_length = -1, int p_cursor_start = -1, int p_cursor_end = -1);
	virtual void hide_virtual_keyboard();
	virtual int get_virtual_keyboard_height() const;

	void set_offscreen_gl_available(bool p_available);

	//virtual String get_executable_path() const;
	virtual String get_user_data_dir() const;
	virtual String get_data_path() const;
	//virtual String get_cache_path() const;
	//virtual String get_resource_dir() const;
	virtual String get_model_name() const;

	void run();

	virtual bool _check_internal_feature_support(const String &p_feature);

	static OS_Vita *get_singleton();

	// libdl
	virtual Error open_dynamic_library(const String p_path, void *&p_library_handle, bool p_also_set_library_path = false);
	virtual Error close_dynamic_library(void *p_library_handle);
	virtual Error get_dynamic_library_symbol_handle(void *p_library_handle, const String p_name, void *&p_symbol_handle, bool p_optional = false);

	OS_Vita();
	~OS_Vita();

	// Misc
	virtual Error shell_open(String p_uri);
	virtual Error execute(const String &p_path, const List<String> &p_arguments, bool p_blocking = true, ProcessID *r_child_id = nullptr, String *r_pipe = nullptr, int *r_exitcode = nullptr, bool read_stderr = false, Mutex *p_pipe_mutex = nullptr, bool p_open_console = false);
	virtual Error kill(const ProcessID &p_pid);
	virtual bool is_process_running(const ProcessID &p_pid) const;
	virtual Date get_date(bool local = false) const;
	virtual Time get_time(bool local = false) const;
	virtual TimeZoneInfo get_time_zone_info() const;
	virtual uint64_t get_unix_time() const;
	virtual uint64_t get_system_time_secs() const;
	virtual uint64_t get_system_time_msecs() const;
	virtual double get_subsecond_unix_time() const;
	virtual void delay_usec(uint32_t p_usec) const;
	virtual uint64_t get_ticks_usec() const;
	virtual String get_stdin_string();
	virtual bool has_environment(const String &p_var) const;
	virtual String get_environment(const String &p_var) const;
	virtual bool set_environment(const String &p_var, const String &p_value) const;
};

#endif // OS_VITA_H
