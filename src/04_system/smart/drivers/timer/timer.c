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

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>

/******************************************************************************
 * Local variables
 *****************************************************************************/
static timers_fd_t timers_fd = {
    .fd_p1 = -1, .fd_p2 = -1, .fd_press = -1, .error = 0};

static struct timespec press_time;

static long long period_ns;
static long long duty_ns;

static long init_period_ns;
static long init_duty_ns;

static struct itimerspec p1_timer_value    = {.it_value    = {0, 0},
                                              .it_interval = {0, 0}};
static struct itimerspec p2_timer_value    = {.it_value    = {0, 0},
                                              .it_interval = {0, 0}};
static struct itimerspec press_timer_value = {.it_value    = {0, 0},
                                              .it_interval = {0, 0}};

static const long press_time_ns = 200 * 1000000;
static const long press_factor  = 120;  //%

/******************************************************************************
 * Local functions
 *****************************************************************************/

/**
 * @brief Update the timer values for the given duty times. Period is common for
 * both timers.
 * @param p1_val_ns: Timer 1 value in nanoseconds
 * @param p2_val_ns: Timer 2 value in nanoseconds
 * @retval None
 */
static void update_it_values(long p1_val_ns, long p2_val_ns)
{
    p1_timer_value.it_value.tv_sec     = p1_val_ns / 1000000000;
    p1_timer_value.it_value.tv_nsec    = p1_val_ns % 1000000000;
    p1_timer_value.it_interval.tv_sec  = period_ns / 1000000000;
    p1_timer_value.it_interval.tv_nsec = period_ns % 1000000000;

    p2_timer_value.it_value.tv_sec     = p2_val_ns / 1000000000;
    p2_timer_value.it_value.tv_nsec    = p2_val_ns % 1000000000;
    p2_timer_value.it_interval.tv_sec  = period_ns / 1000000000;
    p2_timer_value.it_interval.tv_nsec = period_ns % 1000000000;
}

/**
 * @brief Update the timer values for the given duty times. Period is common for
 * both timers.
 * @param cur_val: Current value of the timer
 * @retval None
 */
static void update_timers_value(bool cur_val)
{
    if (cur_val) {
        // Take current value into account to set the next timer values for
        // smooth transition
        struct itimerspec current_val_p1;
        struct itimerspec current_val_p2;
        timerfd_gettime(timers_fd.fd_p1, &current_val_p1);
        timerfd_gettime(timers_fd.fd_p2, &current_val_p2);

        long current_val_p1_ns = (current_val_p1.it_value.tv_sec * 1000000000) +
                                 current_val_p1.it_value.tv_nsec;
        long current_val_p2_ns = (current_val_p2.it_value.tv_sec * 1000000000) +
                                 current_val_p2.it_value.tv_nsec;
        long p1_it_val_ns, p2_it_val_ns;
        // Which is the next timer to expire?
        if (current_val_p1_ns < current_val_p2_ns) {
            p1_it_val_ns = current_val_p1_ns;
            p2_it_val_ns = current_val_p1_ns + (period_ns - duty_ns);
        } else {
            p1_it_val_ns = current_val_p2_ns + duty_ns;
            p2_it_val_ns = current_val_p2_ns;
        }

        update_it_values(p1_it_val_ns, p2_it_val_ns);
    } else {
        update_it_values(duty_ns, period_ns);
    }
}

/*******************************************************************************
 * Public functions
 *****************************************************************************/

/**
 * @brief Initialize the timers
 * @param period_ms: Timer period in milliseconds
 * @param duty_percent: Duty cycle in percent
 * @retval timers_fd_t: Structure containing the file descriptors and error
 * code
 */
timers_fd_t init_timers(long period_ms, long duty_percent)
{
    // Convert to nanoseconds
    period_ns = period_ms * 1000000;  // in ns
    duty_ns   = (long)((long long)period_ns * duty_percent / 100);

    press_time.tv_sec  = press_time_ns / 1000000000;
    press_time.tv_nsec = (press_time_ns % 1000000000);

    // Store the initial values
    init_period_ns = period_ns;
    init_duty_ns   = duty_ns;

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
    update_timers_value(false);

    press_timer_value.it_value    = press_time;
    press_timer_value.it_interval = press_time;

    return timers_fd;
}

/**
 * @brief Start the timers
 * @param None
 * @retval None
 */
void start_timers(void)
{
    // Start the timers
    if (timerfd_settime(timers_fd.fd_p1, 0, &p1_timer_value, NULL) < 0) {
        perror("Couldn't start duty timer");
    }
    if (timerfd_settime(timers_fd.fd_p2, 0, &p2_timer_value, NULL) < 0) {
        perror("Couldn't start period timer");
    }
}

/**
 * @brief Clear the timer event
 * @param fd: File descriptor to the timer
 * @retval None
 */
void clear_timer_event(int fd)
{
    // Read to clear the event
    uint64_t expirations = 0;
    read(fd, &expirations, sizeof(uint64_t));
    if (expirations > 1ULL) {
        (void)printf("Timer expired %lu times\n", expirations);
    }
}

/**
 * @brief Decrease the timer period
 * @param None
 * @retval None
 */
void decrease_timer_period(void)
{
    period_ns = period_ns * press_factor / 100;
    duty_ns   = duty_ns * press_factor / 100;

    // Update the timer values
    update_timers_value(true);

    // Restart the timers
    start_timers();
}

/**
 * @brief Increase the timer period
 * @param None
 * @retval None
 */
void increase_timer_period(void)
{
    period_ns = period_ns * 100 / press_factor;
    duty_ns   = duty_ns * 100 / press_factor;

    // Update the timer values
    update_timers_value(true);

    // Restart the timers
    start_timers();
}

/**
 * @brief Reset the timer period and duty cycle to the initial values
 * @param None
 * @retval None
 */
void reset_timer_period(void)
{
    // Reset the period and duty cycle values
    period_ns = init_period_ns;
    duty_ns   = init_duty_ns;

    // Update the timer values
    update_timers_value(true);

    // Restart the timers
    start_timers();
}

/**
 * @brief Start the press timer
 * @param None
 * @retval None
 */
void start_press_timer(void)
{
    // Start the press timer
    if (timerfd_settime(timers_fd.fd_press, 0, &press_timer_value, NULL) < 0) {
        perror("Couldn't start press timer");
    }
}

/**
 * @brief Stop the press timer
 * @param None
 * @retval None
 */
void stop_press_timer(void)
{
    struct itimerspec stop_timer_value = {.it_value    = {0, 0},
                                          .it_interval = {0, 0}};

    // Stop the press timer
    if (timerfd_settime(timers_fd.fd_press, 0, &stop_timer_value, NULL) < 0) {
        perror("Couldn't stop press timer");
    }
}
