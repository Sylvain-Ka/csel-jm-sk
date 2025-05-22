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

/* Includes ------------------------------------------------------------------*/
#include "brd.h"

#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

/* Private variables ---------------------------------------------------------*/
led_t status_led;
button_t buttonk1;
button_t buttonk2;
button_t buttonk3;
mytimer_t blink_timer;
mytimer_t pooling_timer;
int epoll_fd;

/**
 * @brief  Initialize the board
 * @param  None
 * @retval 0 on success, -1 on error
 */
int brd_init(void)
{
    /* Initialize the led */
    led_init(&status_led, STATUS_LED_GPIO);  // GPIO10 for status LED
    led_on(&status_led);

    /* Initialize the buttons */
    button_init(&buttonk1, BUTTONK1_GPIO, BOTH);    // GPIO11 for buttonk1
    button_init(&buttonk2, BUTTONK2_GPIO, RISING);  // GPIO12 for buttonk2
    button_init(&buttonk3, BUTTONK3_GPIO, BOTH);    // GPIO13 for buttonk3

    /* Ccreate epoll */
    epoll_fd = epoll_create1(0);
    if (-1 == epoll_fd) {
        perror("epoll_create1");
        return -1;
    }

    /* Add buttons to epoll */
    struct epoll_event ev_k1 = {.events = EPOLLPRI, .data.u32 = BUTTONK1_EVENT};
    struct epoll_event ev_k2 = {.events = EPOLLPRI, .data.u32 = BUTTONK2_EVENT};
    struct epoll_event ev_k3 = {.events = EPOLLPRI, .data.u32 = BUTTONK3_EVENT};
    if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, buttonk1.fd, &ev_k1)) {
        perror("button k1 epoll_ctl");
        return -1;
    } else if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, buttonk2.fd, &ev_k2)) {
        perror("button k2 epoll_ctl");
        return -1;
    } else if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, buttonk3.fd, &ev_k3)) {
        perror("button k3 epoll_ctl");
        return -1;
    }

    /* Setup timer */
    if (0 > timer_init(&blink_timer)) {
        perror("timer init");
        return -1;
    }
    if (0 > timer_init(&pooling_timer)) {
        perror("timer init");
        return -1;
    }

    /* Add timer to epoll */
    struct epoll_event ev_blink   = {.events   = EPOLLIN,
                                     .data.u32 = TIMER_BLINK_EVENT};
    struct epoll_event ev_pooling = {.events   = EPOLLIN,
                                     .data.u32 = TIMER_POOLING_EVENT};
    if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, blink_timer.fd, &ev_blink)) {
        perror("blink timer epoll_ctl");
        return -1;
    } else if (-1 ==
               epoll_ctl(
                   epoll_fd, EPOLL_CTL_ADD, pooling_timer.fd, &ev_pooling)) {
        perror("pooling timer epoll_ctl");
        return -1;
    }

    /* Start timers */
    if (0 > timer_start(&blink_timer, 0, 500000000)) {  // 2 Hz (500ms period)
        perror("blink timer start");
        return -1;
    }
    if (0 > timer_stop(&pooling_timer)) {
        perror("pooling timer start");
        return -1;
    }

    return 0;
}

/**
 * @brief  update blink timer
 * @param  sec: seconds
 * @param  nsec: nanoseconds
 * @retval 0 on success, -1 on error
 */
int brd_update_blink_timer(time_t sec, long nsec)
{
    if (0 > timer_update(&blink_timer, sec, nsec)) {
        perror("blink timer update");
        return -1;
    }
    return 0;
}

/**
 * @brief start pooling timer
 * @param sec: seconds
 * @param nsec: nanoseconds
 * @retval 0 on success, -1 on error
 */
int brd_start_pooling_timer(time_t sec, long nsec)
{
    if (0 > timer_start(&pooling_timer, sec, nsec)) {
        perror("pooling timer start");
        return -1;
    }
    return 0;
}

/**
 * @brief stop pooling timer
 * @param None
 * @retval 0 on success, -1 on error
 */
int brd_stop_pooling_timer(void)
{
    if (0 > timer_stop(&pooling_timer)) {
        perror("pooling timer stop");
        return -1;
    }
    return 0;
}

/**
 * @brief Clear pooling timer event
 * @param None
 * @retval 0 on success, -1 on error
 */
int brd_clear_pooling_timer_event(void)
{
    if (0 > timer_get_fd(&pooling_timer)) {
        perror("pooling timer get fd");
        return -1;
    }
    uint64_t expirations = 0;
    read(pooling_timer.fd, &expirations, sizeof(expirations));
    return 0;
}

/**
 * @brief Clear blink timer event
 * @param None
 * @retval 0 on success, -1 on error
 */
int brd_clear_blink_timer_event(void)
{
    if (0 > timer_get_fd(&blink_timer)) {
        perror("blink timer get fd");
        return -1;
    }
    uint64_t expirations = 0;
    read(blink_timer.fd, &expirations, sizeof(expirations));
    return 0;
}

/**
 * @brief  Set the status LED ON
 * @param  None
 * @retval None
 */
void brd_set_status_led_on(void) { led_on(&status_led); }

/**
 * @brief  Set the status LED OFF
 * @param  None
 * @retval None
 */
void brd_set_status_led_off(void) { led_off(&status_led); }

/**
 * @brief Read K1 button state
 * @param None
 * @retval 0 if pressed, 1 if released
 */
int brd_read_k1(void)
{
    int btnval = button_read_clear_event(buttonk1.fd);
    if (0 > btnval) {
        perror("button k1 read");
        return -1;
    }
    return btnval;
}

/**
 * @brief Read K2 button state
 * @param None
 * @retval 0 if pressed, 1 if released
 */
int brd_read_k2(void)
{
    int btnval = button_read_clear_event(buttonk2.fd);
    if (0 > btnval) {
        perror("button k2 read");
        return -1;
    }
    return btnval;
}

/**
 * @brief Read K3 button state
 * @param None
 * @retval 0 if pressed, 1 if released
 */
int brd_read_k3(void)
{
    int btnval = button_read_clear_event(buttonk3.fd);
    if (0 > btnval) {
        perror("button k3 read");
        return -1;
    }
    return btnval;
}

/**
 * @brief Getter for epoll file descriptor
 * @param None
 * @retval epoll file descriptor
 */
int brd_get_epoll_fd(void) { return epoll_fd; }
