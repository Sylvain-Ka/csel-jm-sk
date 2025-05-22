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

/* Includes ------------------------------------------------------------------*/
#include "button.h"

#include "hal/hal_gpio.h"

/* Private constants --------------------------------------------------------*/
const char* edge_types[] = {"rising", "falling", "both"};

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Clear the button event by reading the value
 * @param fd: File descriptor of the button
 * @retval 0 or 1 on success, -1 on error
 */
int button_read_clear_event(int fd)
{
    int bval = hal_gpio_read(fd);
    if (0 > bval) {
        return -1;
    }
    return (bval == 1) ? 1 : 0;
}

/**
 * @brief Initialize the button
 * @param button: Pointer to the button_t structure
 * @param gpio: GPIO number for the button
 * @param edge: Edge detection type (0: rising, 1: falling, 2: both)
 * @retval 0 on success, -1 on error
 */
int button_init(button_t* button, int gpio, int edge)
{
    button->gpio = gpio;
    if (hal_gpio_unexport(gpio) < 0) {
        return -1;
    }
    if (hal_gpio_export(gpio) < 0) {
        return -1;
    }
    if (hal_gpio_direction(gpio, "in") < 0) {
        return -1;
    }
    if (hal_gpio_set_edge(gpio, edge_types[edge]) < 0) {
        return -1;
    }
    button->fd = hal_gpio_open_value_fd(gpio);
    if (button->fd < 0) {
        return -1;
    }
    // Clear the event
    if (button_read_clear_event(button->fd) < 0) {
        return -1;
    }
    return 0;
}

/**
 * @brief uninitialize the button
 * @param button: Pointer to the button_t structure
 * @retval 0 on success, -1 on error
 */
int button_uninit(button_t* button)
{
    if (button->fd < 0) {
        return -1;
    }
    hal_gpio_close_value_fd(button->fd);
    button->fd = -1;
    if (hal_gpio_unexport(button->gpio) < 0) {
        return -1;
    }
    return 0;
}
