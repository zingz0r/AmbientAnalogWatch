/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <watch_app_efl.h>
#include "ambientanalogwatch.h"
#include "view.h"
#include "data.h"
#include "view_defines.h"

static struct view_info {
	Evas_Object *win;
	Evas_Object *layout;
	int w;
	int h;
} s_info = {
	.win = NULL,
	.layout = NULL,
	.w = 0,
	.h = 0,
};

static char *_create_resource_path(const char *file_name);
static Evas_Object *_create_main_layout(void);

/**
 * @brief Create Essential Object window, conformant and layout with size preassigned.
 * @param[in] width width of the window.
 * @param[in] height height of the window.
 */
void view_create_with_size(int width, int height)
{
	s_info.w = width;
	s_info.h = height;
	view_create();
}

/**
 * @brief Create Essential Object window, conformant and layout
 */
void view_create(void)
{
	s_info.win = view_create_win(PACKAGE);
	if (!s_info.win) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create a window.");
		return;
	}

	s_info.layout = _create_main_layout();
	if (!s_info.layout) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create main layout.");
		return;
	}

	evas_object_show(s_info.win);
}

/**
 * @brief Makes a basic window named with package name.
 * @param[in] pkg_name Name of the window.
 * @return This function returns the window object if it was created successfully,
 * otherwise NULL is returned.
 */
Evas_Object *view_create_win(const char *pkg_name)
{
	Evas_Object *win = NULL;
	int ret;

	/*
	 * Window
	 * Create and initialize elm_win.
	 * elm_win is mandatory to manipulate window
	 */
	ret = watch_app_get_elm_win(&win);
	if (ret != APP_ERROR_NONE)
		return NULL;

	elm_win_title_set(win, pkg_name);
	elm_win_borderless_set(win, EINA_TRUE);
	elm_win_alpha_set(win, EINA_FALSE);
	elm_win_indicator_mode_set(win, ELM_WIN_INDICATOR_HIDE);
	elm_win_indicator_opacity_set(win, ELM_WIN_INDICATOR_BG_TRANSPARENT);
	elm_win_prop_focus_skip_set(win, EINA_TRUE);
	elm_win_role_set(win, "no-effect");

	evas_object_resize(win, s_info.w, s_info.h);

	return win;
}

/**
 * @brief Makes and sets a layout to the part
 * @param[in] parent Object to which you want to set this layout
 * @param[in] file_path File path of EDJ will be used
 * @param[in] group_name Group name in EDJ that you want to set to layout
 * @return This function returns the layout object if it was created successfully,
 * otherwise NULL is returned.
 */
Evas_Object *view_create_layout(Evas_Object *parent, char *file_path, char *group_name)
{
	Evas_Object *layout = elm_layout_add(parent);
	if (!layout) {
		dlog_print(DLOG_ERROR, LOG_TAG, "elm_layout_add() failed.");
		return NULL;
	}

	if (!elm_layout_file_set(layout, file_path, group_name)) {
		dlog_print(DLOG_ERROR, LOG_TAG, "elm_layout_file_set() failed for '%s' path.", file_path);
		return NULL;
	}

	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_min_set(layout, s_info.w, s_info.h);
	evas_object_resize(layout, s_info.w, s_info.h);
	evas_object_show(layout);

	return layout;
}

/**
 * @brief Draws the clock's hands.
 * @param[in] t the structure of time components.
 */
void view_set_time(date_time_t t)
{
	Edje_Message_Int_Set *msg = malloc(sizeof(Edje_Message_Int_Set) + 2 * sizeof(int));

	msg->count = 3;
	msg->val[0] = t.hour;
	msg->val[1] = t.minute;
	msg->val[2] = t.second;

	edje_object_message_send(elm_layout_edje_get(s_info.layout), EDJE_MESSAGE_INT_SET, MSG_ID_SET_TIME, msg);

	free(msg);
}

/**
 * @brief Toggles the ambient mode on (draws a second hand) and off (hides a second hand).
 * @param[in] ambient_mode The flag denoting the state of the ambient mode.
 */
void view_toggle_ambient_mode(bool ambient_mode)
{
	Edje_Message_Int msg = {0,};

	msg.val = (int)ambient_mode;

	edje_object_message_send(elm_layout_edje_get(s_info.layout), EDJE_MESSAGE_INT, MSG_ID_AMBIENT_MODE, &msg);
}

/**
 * @brief Destroy window and free important data to finish this application
 */
void view_destroy(void)
{
	if (s_info.win == NULL)
		return;

	evas_object_del(s_info.win);
}

/**
 * @brief Creates path to the given resource file by concatenation of the basic resource path and the given file_name.
 * @param[in] file_name File name or path relative to the resource directory.
 * @return The absolute path to the resource with given file_name is returned.
 */
static char *_create_resource_path(const char *file_name)
{
	static char res_path_buff[PATH_MAX] = {0,};
	char *res_path = app_get_resource_path();

	if (!res_path) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_get_resource_path() failed.");
		return NULL;
	}

	snprintf(res_path_buff, PATH_MAX, "%s%s", res_path, file_name);
	free(res_path);

	return &res_path_buff[0];
}

/**
 * @brief Creates the application's layout.
 * @return This function returns the main layout object if it was created successfully,
 * otherwise NULL is returned.
 */
static Evas_Object *_create_main_layout(void)
{
	char *edj_path = NULL;

	edj_path = _create_resource_path(EDJ_MAIN);

	Evas_Object *layout = view_create_layout(s_info.win, edj_path, GRP_MAIN);
	if (!layout)
		return NULL;

	return layout;
}
