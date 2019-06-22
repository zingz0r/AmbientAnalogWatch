#include <watch_app.h>
#include <system_settings.h>
#include "ambientanalogwatch.h"
#include "view.h"
#include "data.h"
#include <tizen.h>
#include <watch_app.h>
#include <watch_app_efl.h>
#include <Elementary.h>
#include <dlog.h>


typedef struct appdata
{
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
	bool show_colon;
	Ecore_Animator *animator;
	bool isAmbient;
} appdata_s;

#define TEXT_BUF_SIZE 256

static void update_watch(appdata_s *ad, watch_time_h watch_time)
{
	date_time_t dt = {0,};

	data_get_date_time_from_watch_time(watch_time, &dt);
	view_set_time(dt);
	/*
	char watch_text[TEXT_BUF_SIZE];
	int hour24, minute, second;

	if (watch_time == NULL)
	return;

	watch_time_get_hour24(watch_time, &hour24);
	watch_time_get_minute(watch_time, &minute);
	watch_time_get_second(watch_time, &second);

	char* showAmbient = (ad->isAmbient ? "Ambient" : "Awake");

	snprintf
	(
		watch_text, TEXT_BUF_SIZE,
		"<align=center>%s<br/>%02d%s%02d%s%02d</align>",
		showAmbient
		,
		hour24,
		":",
		minute,
		":",
		second
	);

	ad->show_colon = !ad->show_colon;

	elm_object_text_set(ad->label, watch_text);
	*/
}

static void create_base_gui(appdata_s *ad, int width, int height)
{
	watch_time_h watch_time = NULL;
	date_time_t dt = {0,};

	app_event_handler_h handlers[5] = { NULL, };

	watch_time_get_current_time(&watch_time);

	if (!watch_time)
		dlog_print(DLOG_ERROR, LOG_TAG, "Could not get time at start-up");

	data_get_date_time_from_watch_time(watch_time, &dt);

	watch_time_delete(watch_time);

	view_create_with_size(width, height);
	view_set_time(dt);
	/*
	int ret = watch_app_get_elm_win(&ad->win);
	watch_time_h watch_time = NULL;

	if (ret != APP_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get window. err = %d", ret);
		return;
	}

	evas_object_resize(ad->win, width, height);

	// Conformant
	ad->conform = elm_conformant_add(ad->win);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND,
	EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	// Label
	ad->label = elm_label_add(ad->conform);
	evas_object_resize(ad->label, width, height / 3);
	evas_object_move(ad->label, 0, height / 3);
	evas_object_show(ad->label);

	ret = watch_time_get_current_time(&watch_time);
	if (ret != APP_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get current time. err = %d",	ret);
	}

	update_watch(ad, watch_time);
	watch_time_delete(watch_time);

	// Show window after base gui is set up
	evas_object_show(ad->win);
	*/
}

static bool app_create(int width, int height, void *data)
{
	/* Hook to take necessary actions before main event loop starts
	 Initialize UI resources and application's data
	 If this function returns true, the main loop of application starts
	 If this function returns false, the application is terminated */
	appdata_s *ad = data;

	create_base_gui(ad, width, height);

	return true;
}

static void app_time_tick(watch_time_h watch_time, void *data)
{
	/* Called at each second while your app is visible. Update watch UI. */
	appdata_s *ad = data;
	update_watch(ad, watch_time);
}

static void app_ambient_tick(watch_time_h watch_time, void *data)
{
}

static Eina_Bool ambient_tick_seconds(void* data)
{
	dlog_print(DLOG_ERROR, LOG_TAG, "ambient_tick_seconds");
	appdata_s *ad = data;
	watch_time_h watch_time = NULL;
	int ret;
	ret = watch_time_get_current_time(&watch_time);
	if (ret != APP_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get current time. err = %d",	ret);
	}

	update_watch(ad, watch_time);
	watch_time_delete(watch_time);

	return EINA_TRUE;
}

static void app_ambient_changed(bool ambient_mode, void *data)
{
	// hide bg and so on
	//view_toggle_ambient_mode(ambient_mode);

	dlog_print(DLOG_ERROR, LOG_TAG, "app_ambient_changed %d", ambient_mode);
	appdata_s *ad = data;

	ad->isAmbient = ambient_mode;

	if (ambient_mode)
	{
		ad->animator = ecore_animator_add(ambient_tick_seconds, ad);
		ecore_animator_frametime_set(.5f); //lower fps in ambient mode
	}
	else if (ad->animator != NULL)
	{
		ecore_animator_del(ad->animator);
		ad->animator = NULL;
		//ecore_animator_frametime_set(.32f); //higher fps if your watchface needs it
	}
}

int main(int argc, char *argv[])
{
	appdata_s ad = { 0, };
	int ret = 0;

	watch_app_lifecycle_callback_s event_callback = { 0, };
	app_event_handler_h handlers[5] = { NULL, };

	event_callback.create = app_create;
	event_callback.time_tick = app_time_tick;
	event_callback.ambient_tick = app_ambient_tick;
	event_callback.ambient_changed = app_ambient_changed;
	ret = watch_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE)
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_main() is failed. err = %d",	ret);
	}
	//disable native tick events if you are using 2.3.2+
	watch_app_set_ambient_tick_type(WATCH_APP_AMBIENT_TICK_NO_TICK);

	return ret;
}

/*


static bool app_create(int width, int height, void* user_data)
{
	watch_time_h watch_time = NULL;
	date_time_t dt = {0,};

	app_event_handler_h handlers[5] = { NULL, };

	watch_time_get_current_time(&watch_time);

	if (!watch_time)
		dlog_print(DLOG_ERROR, LOG_TAG, "Could not get time at start-up");

	data_get_date_time_from_watch_time(watch_time, &dt);

	watch_time_delete(watch_time);

	view_create_with_size(width, height);
	view_set_time(dt);

	return true;
}

static void app_control(app_control_h app_control, void *user_data)
{

}

static void app_pause(void *user_data)
{

}


static void app_resume(void *user_data)
{

}


static void app_terminate(void *user_data)
{
	view_destroy();
}


static void app_time_tick(watch_time_h watch_time, void* user_data)
{
	date_time_t dt = {0,};

	data_get_date_time_from_watch_time(watch_time, &dt);
	view_set_time(dt);
}

void app_ambient_tick(watch_time_h watch_time, void* user_data)
{
	date_time_t dt = {0,};

	data_get_date_time_from_watch_time(watch_time, &dt);
	view_set_time(dt);
}

static void app_ambient_changed(bool ambient_mode, void* user_data)
{
	view_toggle_ambient_mode(ambient_mode);
}

int main(int argc, char *argv[])
{
	int ret = 0;

	watch_app_lifecycle_callback_s event_callback = {0, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;
	event_callback.time_tick = app_time_tick;
	event_callback.ambient_tick = app_ambient_tick;
	event_callback.ambient_changed = app_ambient_changed;

	ret = watch_app_main(argc, argv, &event_callback, NULL);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_main() is failed. err = %d", ret);

	return ret;
}
*/
