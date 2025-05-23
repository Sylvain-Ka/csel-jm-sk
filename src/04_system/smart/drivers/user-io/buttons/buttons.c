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

#include "buttons.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/******************************************************************************
 * Global variables
 *****************************************************************************/
static buttons_fd_t buttons_fd = {.fd_k1 = -1, .fd_k2 = -1, .fd_k3 = -1};

static const int n_buttons = 3;

static const char* gpio_buttons[] = {
    GPIO_BUTTON_K1, GPIO_BUTTON_K2, GPIO_BUTTON_K3};

static const char* buttons[] = {BUTTON_K1, BUTTON_K2, BUTTON_K3};

/******************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief Open button file descriptor
 * @param None
 * @retval None
 */
void open_buttons(void)
{
    int f;
    // unexport pin out of sysfs (reinitialization)
    for (int i = 0; i < n_buttons; i++) {
        f = open(GPIO_UNEXPORT, O_WRONLY);
        write(f, buttons[i], strlen(buttons[i]));
        close(f);
    }

    // export pin to sysfs
    for (int i = 0; i < n_buttons; i++) {
        f = open(GPIO_EXPORT, O_WRONLY);
        write(f, buttons[i], strlen(buttons[i]));
        close(f);
    }

    // config pin
    for (int i = 0; i < n_buttons; i++) {
        char path[128];
        int ret = snprintf(path, sizeof(path), "%s/direction", gpio_buttons[i]);
        if (ret < 0) {
            perror("Error while configuring pin");
        }
        f = open(path, O_WRONLY);
        write(f, "in", 2);
        close(f);
    }

    // set edge detection
    f = open(GPIO_BUTTON_K1 "/edge", O_WRONLY);
    write(f, "both", 4);
    close(f);

    f = open(GPIO_BUTTON_K2 "/edge", O_WRONLY);
    write(f, "rising", 6);
    close(f);

    f = open(GPIO_BUTTON_K3 "/edge", O_WRONLY);
    write(f, "both", 4);
    close(f);

    // open gpio value attribute
    buttons_fd.fd_k1 = open(GPIO_BUTTON_K1 "/value", O_RDONLY);
    if (buttons_fd.fd_k1 < 0) {
        perror("Error opening button K1");
    }
    buttons_fd.fd_k2 = open(GPIO_BUTTON_K2 "/value", O_RDONLY);
    if (buttons_fd.fd_k2 < 0) {
        perror("Error opening button K2");
    }
    buttons_fd.fd_k3 = open(GPIO_BUTTON_K3 "/value", O_RDONLY);
    if (buttons_fd.fd_k3 < 0) {
        perror("Error opening button K3");
    }

    // Clear the events
    (void)clear_button_event(buttons_fd.fd_k1);
    (void)clear_button_event(buttons_fd.fd_k2);
    (void)clear_button_event(buttons_fd.fd_k3);
}

/**
 * @brief Close buttons fds
 * @param None
 * @retval None
 */
void close_buttons(void)
{
    if (buttons_fd.fd_k1 < 0) {
        (void)printf("Button K1 not opened\n");
    } else {
        close(buttons_fd.fd_k1);
        buttons_fd.fd_k1 = -1;
    }
    if (buttons_fd.fd_k2 < 0) {
        (void)printf("Button K2 not opened\n");
    } else {
        close(buttons_fd.fd_k2);
        buttons_fd.fd_k2 = -1;
    }
    if (buttons_fd.fd_k3 < 0) {
        (void)printf("Button K3 not opened\n");
    } else {
        close(buttons_fd.fd_k3);
        buttons_fd.fd_k3 = -1;
    }

    // unexport pin out of sysfs
    int f = open(GPIO_UNEXPORT, O_WRONLY);
    for (int i = 0; i < n_buttons; i++) {
        write(f, buttons[i], strlen(buttons[i]));
    }
    close(f);
}

/**
 * @brief Get button file descriptors
 * @param None
 * @retval buttons_fd_t structure with file descriptors
 */
buttons_fd_t get_button_fds(void) { return buttons_fd; }

/**
 * @brief Clear button event
 * @param fd File descriptor to the GPIO
 * @retval 1 if button pressed, 0 if not, -1 if error
 */
int clear_button_event(int fd)
{
    // Read to clear the event
    char buf[2];
    lseek(fd, 0, SEEK_SET);
    int ret = read(fd, buf, sizeof(buf));
    if (ret < 0) {
        perror("Error clearing button event");
    }
    if (buf[0] == '1') {
        ret = 1;
    } else {
        ret = 0;
    }
    return ret;
}
