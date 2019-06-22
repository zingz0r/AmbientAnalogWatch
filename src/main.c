#include <watch_app.h>
#include <system_settings.h>
#include "ambientanalogwatch.h"
#include "view.h"
#include "data.h"
#include <tizen.h>
#include <watch_app.h>
#include <watch_app_efl.h>
#include <dlog.h>

static void update_watch(appdata_s *ad, watch_time_h watch_time)
{
	date_time_t dt = {0,};

	data_get_date_time_from_watch_time(watch_time, &dt);
	view_set_time(dt);
}

static void create_base_gui(appdata_s *ad, int width, int height)
{
	watch_time_h watch_time = NULL;
	date_time_t dt = {0,};

	watch_time_get_current_time(&watch_time);

	if (!watch_time)
		dlog_print(DLOG_ERROR, LOG_TAG, "Could not get time at start-up");

	data_get_date_time_from_watch_time(watch_time, &dt);

	watch_time_delete(watch_time);

	view_create_with_size(width, height);
	view_set_time(dt);
}

static bool app_create(int width, int height, void *data)
{
	appdata_s *ad = data;

	create_base_gui(ad, width, height);

	return true;
}

static void app_time_tick(watch_time_h watch_time, void *data)
{
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

	int ret = watch_time_get_current_time(&watch_time);

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
	view_toggle_ambient_mode(ambient_mode);

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
