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
 * Date:    22.05.2025
 */

#ifndef DRIVERS_TIMER_TIMER_H
#define DRIVERS_TIMER_TIMER_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <time.h>

/* Typedefs ------------------------------------------------------------------*/
typedef struct {
    int fd;
} mytimer_t;

// Functions prototypes ------------------------------------------------------*/

int timer_init(mytimer_t *t);
int timer_start(mytimer_t *t, time_t sec, long nsec);
int timer_stop(mytimer_t *t);
int timer_update(mytimer_t *t, time_t sec, long nsec);
int timer_get_fd(mytimer_t *t);
void timer_destroy(mytimer_t *t);

#endif // DRIVERS_TIMER_TIMER_H
