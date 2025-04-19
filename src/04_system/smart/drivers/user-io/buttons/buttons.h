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
 * Date:    19.04.2025
 */

#ifndef BUTTONS_H
#define BUTTONS_H
 
#include "../board.h"

typedef struct {
    int fd_k1;
    int fd_k2;
    int fd_k3;
} buttons_fd_t;

void open_buttons();
void close_buttons();

buttons_fd_t get_button_fds();
int clear_button_event(int fd);

#endif // BUTTONS_H 