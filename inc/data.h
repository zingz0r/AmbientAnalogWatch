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

#if !defined(_DATA_H)
#define _DATA_H

#include <watch_app.h>

struct _date_time {
	int year;
	int month;
	int day;
	int day_of_week;
	int hour;
	int minute;
	int second;
};

typedef struct _date_time date_time_t;

void data_get_date_time_from_watch_time(const watch_time_h watch_time, date_time_t *dt);

#endif
