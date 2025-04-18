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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/timerfd.h>
#include <unistd.h>


#include "timer.h"

/******************************************************************************
 * Global variables
 *****************************************************************************/
static timers_fd_t timers_fd = {
    .fd_p1 = -1,
    .fd_p2 = -1,
    .error = 0
};

static struct itimerspec period_timer_value;
static struct itimerspec duty_timer_value;

/******************************************************************************
 * Function: init_timers
 *****************************************************************************/
timers_fd_t init_timers(long period_ms, long duty_percent)
{
    // Convert to nanoseconds
    long period_ns = period_ms * 1000000;  // in ns

    // Set period and duty cycle for the timer
    struct timespec period_time = {
        .tv_sec = period_ns / 1000000000,
        .tv_nsec = period_ns % 1000000000
    };
    struct timespec duty_time = {
        .tv_sec = period_ns / 1000000000 * duty_percent / 100,
        .tv_nsec = (period_ns % 1000000000) * duty_percent / 100
    };

    // timerfd creation
    timers_fd.fd_p1 = timerfd_create(CLOCK_MONOTONIC, 0);
    if (timers_fd.fd_p1 < 0) {
        timers_fd.error = -1;
        perror("Couldn't create timerfd");
        return timers_fd;
    }
    timers_fd.fd_p2 = timerfd_create(CLOCK_MONOTONIC, 0);
    if (timers_fd.fd_p2 < 0) {
        timers_fd.error = -1;
        perror("Couldn't create timerfd");
        return timers_fd;
    }


    // Set the timer values
    duty_timer_value.it_value = duty_time;
    duty_timer_value.it_interval = period_time;

    period_timer_value.it_value = period_time;
    period_timer_value.it_interval = period_time;

    return timers_fd;
}

/******************************************************************************
 * Function: start_timers
 *****************************************************************************/
void start_timers()
{
    // Start the timers
    if (timerfd_settime(timers_fd.fd_p1, 0, &duty_timer_value, NULL) < 0) {
        perror("Couldn't start duty timer");
    }
    if (timerfd_settime(timers_fd.fd_p2, 0, &period_timer_value, NULL) < 0) {
        perror("Couldn't start period timer");
    }
}

/******************************************************************************
 * Function: clear_timer_event
 *****************************************************************************/
void clear_timer_event(int fd)
{
    // Read to clear the event
    uint64_t expirations = 0;
    read(fd, &expirations, sizeof(uint64_t));
    if(expirations > 1){
        printf("Timer expired %lu times\n", expirations);
    }
}