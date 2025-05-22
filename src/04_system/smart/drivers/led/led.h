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
 * Purpose: Led driver library
 *
 * Autĥors: Julien Michel / Sylvain Kämpfer
 * Date:    22.05.2025
 */
#ifndef DRIVERS_LED_LED_H
#define DRIVERS_LED_LED_H

/* Typedefs ------------------------------------------------------------------*/

typedef struct {
    int gpio;
    int fd;
} led_t;

/* Functions prototypes ------------------------------------------------------*/

int led_init(led_t* led, int gpio);
void led_on(led_t* led);
void led_off(led_t* led);

#endif  // DRIVERS_LED_LED_H
