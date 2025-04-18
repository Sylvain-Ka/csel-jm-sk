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
#include <time.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <stdio.h>

/*
 * status led - gpioa.10 --> gpio10
 * power led  - gpiol.10 --> gpio362
 */
#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO_UNEXPORT "/sys/class/gpio/unexport"
#define GPIO_LED "/sys/class/gpio/gpio10"
#define LED "10"

typedef enum {
    eDutyTimer,
    ePeriodTimer,
    eButton
} event_type_t;

static int open_led()
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
    f = open(GPIO_LED "/value", O_RDWR);
    return f;
}

static void toggle_led(int ledfd)
{
    char value[2];
    // Read current value
    pread(ledfd, value, sizeof(value), 0);
    // Toggle value
    if (value[0] == '0') {
        pwrite(ledfd, "1", sizeof("1"), 0);
    } else {
        pwrite(ledfd, "0", sizeof("0"), 0);
    }    
}

static void turn_on_led(int ledfd)
{
    pwrite(ledfd, "1", sizeof("1"), 0);
}

static void turn_off_led(int ledfd)
{
    pwrite(ledfd, "0", sizeof("0"), 0);
}

int main(int argc, char* argv[])
{
    long duty   = 25;     // %
    long period = 500;  // ms
    if (argc >= 2) period = atoi(argv[1]);
    period *= 1000000;  // in ns

    // Set period and duty cycle for the timer
    struct timespec period_time = {
        .tv_sec = period / 1000000000,
        .tv_nsec = period % 1000000000
    };
    struct timespec duty_time = {
        .tv_sec = period / 1000000000 * duty / 100,
        .tv_nsec = (period % 1000000000) * duty / 100
    };

    // Open the LED
    int led = open_led();
    pwrite(led, "1", sizeof("1"), 0);

    // timerfd creation
    int dutytimerfd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (dutytimerfd < 0) {
        perror("Couldn't create timerfd");
        return -1;
    }
    int periodtimerfd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (periodtimerfd < 0) {
        perror("Couldn't create timerfd");
        return -1;
    }

    // Set the timer values
    struct itimerspec duty_timer_value = {
        .it_value = duty_time,
        .it_interval = period_time
    };
    struct itimerspec period_timer_value = {
        .it_value = period_time,
        .it_interval = period_time
    };

    // Start the timers
    if (timerfd_settime(dutytimerfd, 0, &duty_timer_value, NULL) < 0) {
        perror("Couldn't set timerfd");
        return -1;
    }
    if (timerfd_settime(periodtimerfd, 0, &period_timer_value, NULL) < 0) {
        perror("Couldn't set timerfd");
        return -1;
    }

    // epoll creation
    int epollfd = epoll_create1(0);
    if (epollfd < 0) {
        perror("Couldn't create epollfd");
        return -1;
    }

    // Add the timerfd to the epoll instance
    struct epoll_event duty_timer_event = {
        .events = EPOLLIN,
        .data.u32 = eDutyTimer
    };
    struct epoll_event period_timer_event = {
        .events = EPOLLIN,
        .data.u32 = ePeriodTimer
    };

    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, dutytimerfd, &duty_timer_event) < 0) {
        perror("Couldn't add timerfd to epoll");
        return -1;
    }
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, periodtimerfd, &period_timer_event) < 0) {
        perror("Couldn't add timerfd to epoll");
        return -1;
    }
    
    while (1) {
        struct epoll_event events[2];
        // Wait for events
        int nr = epoll_wait(epollfd, events, 2, -1);
        if (nr < 0) {
            perror("epoll_wait failed");
            return -1;
        }
        // Handle events
        for(int i=0; i<nr; i++){
            uint64_t expirations;

            switch(events[i].data.u32){
                case eDutyTimer:
                    // Read to clear the event
                    read(dutytimerfd, &expirations, sizeof(uint64_t));
                    if(expirations > 1){
                        printf("Timer expired %lu times\n", expirations);
                    }
                    // Turn off the LED
                    turn_off_led(led);
                    break;
                case ePeriodTimer:
                    // Read to clear the event
                    read(periodtimerfd, &expirations, sizeof(uint64_t));
                    if(expirations > 1){
                        printf("Timer expired %lu times\n", expirations);
                    }
                    // Turn on the LED
                    turn_on_led(led);
                    break;
                case eButton:
                    // Handle button event
                    break;
                default:
                    break;
            }
        }
    }

    return 0;
}