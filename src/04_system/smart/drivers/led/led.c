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

/* Includes ------------------------------------------------------------------*/
#include "led.h"

#include "hal/hal_gpio.h"

/* Functions -----------------------------------------------------------------*/

/**
 * @brief  Initialize the LED
 * @param  led: Pointer to the led_t structure
 * @param  gpio: GPIO number for the LED
 * @retval 0 on success, -1 on error
 */
int led_init(led_t* led, int gpio)
{
    led->gpio = gpio;
    if (hal_gpio_unexport(gpio) < 0) {
        return -1;
    }
    if (hal_gpio_export(gpio) < 0) {
        return -1;
    }
    if (hal_gpio_direction(gpio, "out") < 0) {
        return -1;
    }
    led->fd = hal_gpio_open_value_fd(gpio);
    return (led->fd < 0) ? -1 : 0;
}

/**
 * @brief  Turn on the LED
 * @param  led: Pointer to the led_t structure
 * @retval None
 */
void led_on(led_t* led) { hal_gpio_write(led->fd, 1); }

/**
 * @brief  Turn off the LED
 * @param  led: Pointer to the led_t structure
 * @retval None
 */
void led_off(led_t* led) { hal_gpio_write(led->fd, 0); }
