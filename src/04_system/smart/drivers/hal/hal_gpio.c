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

/* Includes ------------------------------------------------------------------*/
#include "hal_gpio.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Create the path to the GPIO file
 * @param  buffer: The buffer to store the path
 * @param  size: The size of the buffer
 * @param  gpio: The GPIO number
 * @param  attr: The attribute to access (value, direction, etc.)
 * @retval None
 */
static void gpio_path(char* buffer, size_t size, int gpio, const char* attr)
{
    snprintf(buffer, size, "/sys/class/gpio/gpio%d/%s", gpio, attr);
}

/**
 * @brief  Export the GPIO
 * @param  gpio: The GPIO number
 * @retval 0 on success, -1 on error
 */
int hal_gpio_export(int gpio)
{
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0) return -1;
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", gpio);
    write(fd, buf, strlen(buf));
    close(fd);
    return 0;
}

/**
 * @brief  Unexport the GPIO
 * @param  gpio: The GPIO number
 * @retval 0 on success, -1 on error
 */
int hal_gpio_unexport(int gpio)
{
    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd < 0) return -1;
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", gpio);
    write(fd, buf, strlen(buf));
    close(fd);
    return 0;
}

/**
 * @brief  Set the GPIO direction
 * @param  gpio: The GPIO number
 * @param  direction: The direction (in or out)
 * @retval 0 on success, -1 on error
 */
int hal_gpio_direction(int gpio, const char* direction)
{
    char path[64];
    gpio_path(path, sizeof(path), gpio, "direction");
    int fd = open(path, O_WRONLY);
    if (fd < 0) return -1;
    write(fd, direction, strlen(direction));
    close(fd);
    return 0;
}

/**
 * @brief  Open the GPIO value file
 * @param  gpio: The GPIO number
 * @retval File descriptor on success, -1 on error
 */
int hal_gpio_open_value_fd(int gpio)
{
    char path[64];
    gpio_path(path, sizeof(path), gpio, "value");
    return open(path, O_RDWR);
}

/**
 * @brief Close the GPIO value file
 * @param fd: The file descriptor of the GPIO value file
 * @retval 0 on success, -1 on error
 */
int hal_gpio_close_value_fd(int fd)
{
    if (fd < 0) return -1;
    close(fd);
    return 0;
}

/**
 * @brief  Write a value to the GPIO
 * @param  fd: The file descriptor of the GPIO value file
 * @param  value: The value to write (0 or 1)
 * @retval Number of bytes written on success, -1 on error
 */
int hal_gpio_write(int fd, int value)
{
    const char* val = (value) ? "1" : "0";
    return pwrite(fd, val, 1, 0);
}


/**
 * @brief  Read a value from the GPIO
 * @param  fd: The file descriptor of the GPIO value file
 * @retval 0 or 1 on success, -1 on error
 */
int hal_gpio_read(int fd)
{
    char buf;
    if (pread(fd, &buf, 1, 0) != 1) {
        return -1;
    }
    return (buf == '1') ? 1 : 0;
}
/**
 * @brief  Set the GPIO edge detection
 * @param  gpio: The GPIO number
 * @param  edge: The edge type ("none", "rising", "falling", "both")
 * @retval 0 on success, -1 on error
 */
int hal_gpio_set_edge(int gpio, const char* edge)
{
    char path[64];
    gpio_path(path, sizeof(path), gpio, "edge");
    int fd = open(path, O_WRONLY);
    if (fd < 0) return -1;
    int result = write(fd, edge, strlen(edge));
    close(fd);
    return (result == (ssize_t)strlen(edge)) ? 0 : -1;
}
