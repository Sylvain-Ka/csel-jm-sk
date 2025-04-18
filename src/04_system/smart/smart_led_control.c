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
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <stdio.h>

#include "drivers/drivers.h"

/******************************************************************************
 * Enumerations
 *****************************************************************************/
typedef enum {
    eP1Timer,
    eP2Timer,
    eButton
} event_type_t;

/******************************************************************************
 * Global variables
 *****************************************************************************/
static timers_fd_t timers_fd;
static int epollfd = -1;

/******************************************************************************
 * Local functions
 *****************************************************************************/
static int init_app(long period_ms, long duty_percent)
{
    // Open the LED
    open_led();
    turn_on_led();

    // Initialize the timers
    timers_fd = init_timers(period_ms, duty_percent);

    // epoll creation
    epollfd = epoll_create1(0);
    if (epollfd < 0) {
        perror("Couldn't create epollfd");
        return -1;
    }

    // Create timerfd events
    struct epoll_event p1_event = {
        .events = EPOLLIN,
        .data.u32 = eP1Timer
    };
    struct epoll_event p2_event = {
        .events = EPOLLIN,
        .data.u32 = eP2Timer
    };

    // Configure epoll
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, timers_fd.fd_p1, &p1_event) < 0) {
        perror("Couldn't add timerfd to epoll");
        return -1;
    }
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, timers_fd.fd_p2, &p2_event) < 0) {
        perror("Couldn't add timerfd to epoll");
        return -1;
    }

    // Start the timers
    start_timers();
    
    return 0;
}

static int handle_events()
{
    struct epoll_event events[2];

    // Wait for events
    int nr = epoll_wait(epollfd, events, 2, -1);
    if (nr < 0) {
        perror("epoll_wait failed");
        return -1;
    }

    // Handle events
    for(int i=0; i<nr; i++){
        switch(events[i].data.u32){
            case eP1Timer:
                // Read to clear the event
                clear_timer_event(timers_fd.fd_p1);
                // Turn off the LED
                turn_off_led();
                break;
            case eP2Timer:
                // Read to clear the event
                clear_timer_event(timers_fd.fd_p2);
                // Turn on the LED
                turn_on_led();
                break;
            case eButton:
                // Handle button event
                break;
            default:
                break;
        }
    }
    return 0;
}

/******************************************************************************
 * Main function
 *****************************************************************************/
int main(int argc, char* argv[])
{
    long duty   = 25;     // %
    long period = 500;  // ms
    if (argc >= 2) period = atoi(argv[1]);

    int ret;

    // Init application
    ret = init_app(period, duty);
    
    // Main loop
    while (1) {
        if (ret < 0) {
            printf("Error, exiting application\n");
            close_led();
            return -1;
        }
        ret = handle_events();
    }
    return 0;
}