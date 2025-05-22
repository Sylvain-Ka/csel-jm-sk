/**
 * Copyright 2025 University of Applied Sciences Western Switzerland / Fribourg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Project: HEIA-FR / HES-SO MSE - MA-CSEL1 Laboratory
 *
 * Purpose: Driver for the timer control
 *
 * Autĥors: Julien Michel / Sylvain Kämpfer
 * Date:    18.04.2025
 */

#include "timer.h"
#include <sys/timerfd.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int timer_init(mytimer_t *t)
{
    if (NULL == t) {return -1;}
    t->fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    return (t->fd == -1) ? -1 : 0;
}

int timer_start(mytimer_t *t, time_t sec, long nsec)
{
    if ((NULL == t) || (0 > t->fd)) {return -1;}

    struct itimerspec spec = {0};
    spec.it_value.tv_sec = sec;
    spec.it_value.tv_nsec = nsec;

    return timerfd_settime(t->fd, 0, &spec, NULL);
}

int timer_update(mytimer_t *t, time_t sec, long nsec)
{
    return timer_start(t, sec, nsec);  // Alias for same behavior
}

int timer_stop(mytimer_t *t)
{
    if ((NULL == t) || (0 > t->fd)) {return -1;}

    struct itimerspec spec = {0};
    return timerfd_settime(t->fd, 0, &spec, NULL);
}

int timer_get_fd(mytimer_t *t)
{
    return (NULL != t) ? t->fd : -1;
}

void timer_destroy(mytimer_t *t)
{
    if ((NULL != t) && (0 <= t->fd)) {
        close(t->fd);
        t->fd = -1;
    }
}
