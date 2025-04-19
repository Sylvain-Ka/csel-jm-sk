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
    .fd_press = -1,
    .error = 0
};

static struct timespec period_time;
static struct timespec duty_time;
static struct timespec press_time;

static struct timespec init_period_time;
static struct timespec init_duty_time;

static struct itimerspec period_timer_value = {
    .it_value = {0, 0},
    .it_interval = {0, 0}
};
static struct itimerspec duty_timer_value = {
    .it_value = {0, 0},
    .it_interval = {0, 0}
};
static struct itimerspec press_timer_value = {
    .it_value = {0, 0},
    .it_interval = {0, 0}
};

static const long press_time_ns = 1000*1000000;
static const long press_factor = 120; //%

/******************************************************************************
 * Local functions
 *****************************************************************************/
static void update_timers_value()
{
    duty_timer_value.it_value = duty_time;
    duty_timer_value.it_interval = period_time;

    period_timer_value.it_value = period_time;
    period_timer_value.it_interval = period_time;
}

static void update_time_values(long period_ns, long duty_ns)
{
    period_time.tv_sec = period_ns / 1000000000;
    period_time.tv_nsec = period_ns % 1000000000;

    duty_time.tv_sec = duty_ns / 1000000000;
    duty_time.tv_nsec = duty_ns % 1000000000;
}

/******************************************************************************
 * Function: init_timers
 *****************************************************************************/
timers_fd_t init_timers(long period_ms, long duty_percent)
{
    // Convert to nanoseconds
    long period_ns = period_ms * 1000000;  // in ns
    long duty_ns = (long) ((long long)period_ns * duty_percent / 100);

    // Format period and duty cycle values for the timer
    update_time_values(period_ns, duty_ns);
    
    press_time.tv_sec = press_time_ns / 1000000000;
    press_time.tv_nsec = (press_time_ns % 1000000000);

    // Store the initial values
    init_period_time = period_time;
    init_duty_time = duty_time;
    
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
    timers_fd.fd_press = timerfd_create(CLOCK_MONOTONIC, 0);
    if (timers_fd.fd_press < 0) {
        timers_fd.error = -1;
        perror("Couldn't create timerfd");
        return timers_fd;
    }


    // Set the timer values
    update_timers_value();

    press_timer_value.it_value = press_time;
    press_timer_value.it_interval = press_time;

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

/******************************************************************************
 * Function: decrease_timer_period
 *****************************************************************************/
void decrease_timer_period()
{
    long long period_time_ns = period_time.tv_sec * 1000000000 + period_time.tv_nsec;
    long long duty_time_ns = duty_time.tv_sec * 1000000000 + duty_time.tv_nsec;
    
    period_time_ns = period_time_ns * press_factor / 100;
    duty_time_ns = duty_time_ns * press_factor / 100;

    // Update period and duty cycle values
    update_time_values(period_time_ns, duty_time_ns);

    // Update the timer values
    update_timers_value();

    // Restart the timers
    start_timers();
}

/******************************************************************************
 * Function: increase_timer_period
 *****************************************************************************/
void increase_timer_period()
{
    long long period_time_ns = period_time.tv_sec * 1000000000 + period_time.tv_nsec;
    long long duty_time_ns = duty_time.tv_sec * 1000000000 + duty_time.tv_nsec;

    period_time_ns = period_time_ns * 100 / press_factor;
    duty_time_ns = duty_time_ns * 100 / press_factor;

    // Update period and duty cycle values
    update_time_values(period_time_ns, duty_time_ns);

    // Update the timer values
    update_timers_value();

    // Restart the timers
    start_timers();
}

/******************************************************************************
 * Function: reset_timer_period
 *****************************************************************************/
void reset_timer_period()
{
    // Reset the period and duty cycle values
    period_time = init_period_time;
    duty_time = init_duty_time;

    // Update the timer values
    update_timers_value();

    // Restart the timers
    start_timers();
}
/******************************************************************************
 * Function: start_press_timer
 *****************************************************************************/
void start_press_timer()
{
    // Start the press timer
    if (timerfd_settime(timers_fd.fd_press, 0, &press_timer_value, NULL) < 0) {
        perror("Couldn't start press timer");
    }
}

/******************************************************************************
 * Function: stop_press_timer
 *****************************************************************************/
void stop_press_timer()
{   
    struct itimerspec stop_timer_value = {
        .it_value = {0, 0},
        .it_interval = {0, 0}
    };

    // Stop the press timer
    if (timerfd_settime(timers_fd.fd_press, 0, &stop_timer_value, NULL) < 0) {
        perror("Couldn't stop press timer");
    }
}