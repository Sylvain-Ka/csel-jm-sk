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
 * Abstract: System programming -  file system
 *
 * Purpose: NanoPi smart status led control system
 *
 * Autĥors: Julien Michel / Sylvain Kämpfer
 * Date:    18.04.2025
 */
/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <syslog.h>
#include <unistd.h>

#include "brd.h"

/* Defines -------------------------------------------------------------------*/
#define HALF_SECOND 500000000

/* Public functions ---------------------------------------------------------*/
/**
 * @brief  Main function
 * @param  argc: Number of arguments
 * @param  argv: Arguments
 * @retval None
 */
int main(int argc, char* argv[])
{
    struct epoll_event events[5];
    bool led_state = false;
    bool k1_down   = false;
    bool k3_down   = false;

    int blink_timer_period     = 1000;  // 1 second
    int blink_timer_duty_cycle = 50;    // 50% duty cycle

    if (argc > 2) {
        blink_timer_duty_cycle = atoi(argv[1]);
    }

    openlog("led-controller", LOG_PID | LOG_CONS, LOG_USER);

    if (brd_init() < 0) {
        syslog(LOG_ERR, "Board initialization failed");
        closelog();
        perror("Board initialization failed\n");
        return -1;
    }

    int epoll_fd = brd_get_epoll_fd();

    while (1) {
        int nr = epoll_wait(epoll_fd, events, 5, -1);
        if (nr < 0) {
            perror("epoll_wait failed");
            break;
        }
        for (int ev_nbr = 0; ev_nbr < nr; ev_nbr++) {
            switch (events[ev_nbr].data.u32) {
                case TIMER_BLINK_EVENT:
                    if (0 > brd_clear_blink_timer_event()) {
                        syslog(LOG_ERR, "Failed to clear blink timer event");
                        break;
                    }
                    int next_blink_time = 0;
                    if (led_state) {
                        brd_set_status_led_off();
                        led_state = false;
                        next_blink_time =
                            blink_timer_period * blink_timer_duty_cycle / 100;
                    } else {
                        brd_set_status_led_on();
                        led_state       = true;
                        next_blink_time = blink_timer_period *
                                          (100 - blink_timer_duty_cycle) / 100;
                    }
                    brd_update_blink_timer(next_blink_time / 1000,
                                           (next_blink_time % 1000) * 1000000);
                    break;

                case BUTTONK1_EVENT:
                    if (1 == brd_read_k1()) {
                        k1_down = true;
                        if (blink_timer_period > 100) {
                            blink_timer_period -= 50;
                            syslog(LOG_INFO,
                                   "Blink timer period: %d ms",
                                   blink_timer_period);
                        }
                        (void)brd_start_pooling_timer(0, HALF_SECOND);
                    } else {
                        k1_down = false;
                        if (!k3_down) brd_stop_pooling_timer();
                    }
                    break;

                case BUTTONK2_EVENT:
                    (void)brd_read_k2(); /* To clear event */
                    blink_timer_period = 1000;
                    break;

                case BUTTONK3_EVENT:
                    if (1 == brd_read_k3()) {
                        k3_down = true;
                        if (blink_timer_period < 5000) {
                            blink_timer_period += 50;
                            syslog(LOG_INFO,
                                   "Blink timer period: %d ms",
                                   blink_timer_period);
                        }
                        (void)brd_start_pooling_timer(0, HALF_SECOND);
                    } else {
                        k3_down = false;
                        if (!k1_down) brd_stop_pooling_timer();
                    }
                    break;

                case TIMER_POOLING_EVENT:
                    if (0 > brd_clear_pooling_timer_event()) {
                        syslog(LOG_ERR, "Failed to clear pooling timer event");
                        break;
                    }
                    (void)brd_start_pooling_timer(0, HALF_SECOND);
                    if (k1_down && (100 <= blink_timer_period)) {
                        blink_timer_period -= 50;
                        syslog(LOG_INFO,
                               "Blink timer period: %d ms",
                               blink_timer_period);
                    } else if (k3_down && (5000 > blink_timer_period)) {
                        blink_timer_period += 50;
                        syslog(LOG_INFO,
                               "Blink timer period: %d ms",
                               blink_timer_period);
                    } else {
                        brd_stop_pooling_timer();
                    }
                    break;
                default:
                    syslog(
                        LOG_ERR, "Unknown event: %d", events[ev_nbr].data.u32);
                    break;
            }
        }
    }

    return 0;
}
