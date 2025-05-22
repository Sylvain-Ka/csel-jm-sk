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
 * Purpose: Board driver library
 *
 * Autĥors: Julien Michel / Sylvain Kämpfer
 * Date:    22.05.2025
 */

#ifndef DRIVERS_BRD_H
#define DRIVERS_BRD_H

/* Includes ------------------------------------------------------------------*/
#include "button/button.h"
#include "led/led.h"
#include "timer/timer.h"

/* Defines -------------------------------------------------------------------*/
#define STATUS_LED_GPIO 10
#define BUTTONK1_GPIO 0
#define BUTTONK2_GPIO 2
#define BUTTONK3_GPIO 3

/* Typedefs ------------------------------------------------------------------*/
typedef enum {
    BUTTONK1_EVENT = 0,
    BUTTONK2_EVENT,
    BUTTONK3_EVENT,
    TIMER_BLINK_EVENT,
    TIMER_POOLING_EVENT,
} ev_types;

/* Exported variables --------------------------------------------------------*/
extern led_t status_led;

/* Functions prototypes ------------------------------------------------------*/

int brd_init(void);
void brd_set_status_led_on(void);
void brd_set_status_led_off(void);
int brd_update_blink_timer(time_t sec, long nsec);
int brd_start_pooling_timer(time_t sec, long nsec);
int brd_stop_pooling_timer(void);
int brd_clear_pooling_timer_event(void);
int brd_clear_blink_timer_event(void);
int brd_read_k1(void);
int brd_read_k2(void);
int brd_read_k3(void);
int brd_get_epoll_fd(void);

#endif  // DRIVERS_BRD_H
