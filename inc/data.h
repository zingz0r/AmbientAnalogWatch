#if !defined(_DATA_H)
#define _DATA_H

#include <watch_app.h>
#include <Elementary.h>

typedef struct appdata
{
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
	bool show_colon;
	Ecore_Animator *animator;
	bool isAmbient;
} appdata_s;

typedef struct _date_time {
	int year;
	int month;
	int day;
	int day_of_week;
	int hour;
	int minute;
	int second;
} date_time_t;

void data_get_date_time_from_watch_time(const watch_time_h watch_time, date_time_t *dt);

#endif
