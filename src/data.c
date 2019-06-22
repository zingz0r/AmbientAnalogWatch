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

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "ambientanalogwatch.h"
#include "data.h"

/**
 * @brief: The functions transforms the date and time referenced by the variable
 * of watch_time_h type into the date_time_t structure.
 * @param[watch_time]: the source of the date and time obtained via the time_tick callback.
 * @param[dt]: reference to the time and date destination structure.
 */
void data_get_date_time_from_watch_time(const watch_time_h watch_time, date_time_t *dt)
{
	watch_time_get_hour24(watch_time, &dt->hour);
	watch_time_get_minute(watch_time, &dt->minute);
	watch_time_get_second(watch_time, &dt->second);
	watch_time_get_year(watch_time, &dt->year);
	watch_time_get_month(watch_time, &dt->month);
	watch_time_get_day(watch_time, &dt->day);
	watch_time_get_day_of_week(watch_time, &dt->day_of_week);
}
