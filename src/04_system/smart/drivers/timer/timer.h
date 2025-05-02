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

#ifndef DRIVERS_TIMER_TIMER_H_
#define DRIVERS_TIMER_TIMER_H_

typedef struct {
    int fd_p1;
    int fd_p2;
    int fd_press;
    int error;
} timers_fd_t;

timers_fd_t init_timers(long period_ms, long duty_percent);
void start_timers(void);
void clear_timer_event(int fd);

void increase_timer_period(void);
void decrease_timer_period(void);
void reset_timer_period(void);

void start_press_timer(void);
void stop_press_timer(void);

#endif  // DRIVERS_TIMER_TIMER_H_
