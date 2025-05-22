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
 * Purpose: HAL library for the GPIO control
 *
 * Autĥors: Julien Michel / Sylvain Kämpfer
 * Date:    22.05.2025
 */
#ifndef DRIVERS_HAL_HAL_GPIO_H
#define DRIVERS_HAL_HAL_GPIO_H

/* Functions prototypes ------------------------------------------------------*/
int hal_gpio_export(int gpio);
int hal_gpio_unexport(int gpio);
int hal_gpio_direction(int gpio, const char* direction);
int hal_gpio_open_value_fd(int gpio);
int hal_gpio_close_value_fd(int fd);
int hal_gpio_write(int fd, int value);
int hal_gpio_read(int fd);
int hal_gpio_set_edge(int gpio, const char* edge);


#endif // DRIVERS_HAL_HAL_GPIO_H
