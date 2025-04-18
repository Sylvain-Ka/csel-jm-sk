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
 * Purpose: Driver for the LED control
 *
 * Autĥors: Julien Michel / Sylvain Kämpfer
 * Date:    18.04.2025
 */

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "led.h"

/******************************************************************************
 * Global variables
 *****************************************************************************/
static int led_fd = -1;

/******************************************************************************
 * Function: open_led
 *****************************************************************************/
void open_led()
{
    // unexport pin out of sysfs (reinitialization)
    int f = open(GPIO_UNEXPORT, O_WRONLY);
    write(f, LED, strlen(LED));
    close(f);

    // export pin to sysfs
    f = open(GPIO_EXPORT, O_WRONLY);
    write(f, LED, strlen(LED));
    close(f);

    // config pin
    f = open(GPIO_LED "/direction", O_WRONLY);
    write(f, "out", 3);
    close(f);

    // open gpio value attribute
    led_fd = open(GPIO_LED "/value", O_RDWR);
    if(led_fd < 0) {
        perror("Error opening LED");
    }
}

/******************************************************************************
 * Function: close_led
 *****************************************************************************/
void close_led()
{
    if(led_fd < 0) {
        printf("LED not opened\n");
        return;
    }
    // close gpio value attribute
    close(led_fd);
    led_fd = -1;

    // unexport pin out of sysfs
    int f = open(GPIO_UNEXPORT, O_WRONLY);
    write(f, LED, strlen(LED));
    close(f);
}

/******************************************************************************
 * Function: turn_on_led
 *****************************************************************************/
void turn_on_led()
{
    if(led_fd < 0) {
        printf("LED not opened\n");
        return;
    }

    pwrite(led_fd, "1", sizeof("1"), 0);
    
}

/******************************************************************************
 * Function: turn_off_led
 *****************************************************************************/
void turn_off_led()
{
    if(led_fd < 0) {
        printf("LED not opened\n");
        return;
    }

    pwrite(led_fd, "0", sizeof("0"), 0);
}

/******************************************************************************
 * Function: toggle_led
 *****************************************************************************/
void toggle_led()
{
    char value[2];

    if(led_fd < 0) {
        printf("LED not opened\n");
        return;
    }

    // Read current value
    pread(led_fd, value, sizeof(value), 0);
    // Toggle value
    if (value[0] == '0') {
        pwrite(led_fd, "1", sizeof("1"), 0);
    } else {
        pwrite(led_fd, "0", sizeof("0"), 0);
    }    
}