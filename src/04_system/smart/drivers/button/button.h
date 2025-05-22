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
 * Purpose: Driver for the buttons control
 *
 * Autĥors: Julien Michel / Sylvain Kämpfer
 * Date:    22.05.2025
 */

#ifndef DRIVERS_BUTTON_BUTTON_H
#define DRIVERS_BUTTON_BUTTON_H

/* Typedefs ------------------------------------------------------------------*/
typedef struct {
    int gpio;
    int fd;
} button_t;

typedef enum {
    RISING  = 0,
    FALLING = 1,
    BOTH    = 2,
} edge_t;
/* Functions prototypes ------------------------------------------------------*/

int button_init(button_t* button, int gpio, int edge);
int button_read_clear_event(int fd);
int button_uninit(button_t* button);

#endif  // DRIVERS_BUTTON_BUTTON_H
