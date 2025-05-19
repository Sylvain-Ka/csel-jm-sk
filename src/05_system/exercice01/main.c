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
 * Date:    09.05.2025
 */

/* Private define ------------------------------------------------------------*/
#define _GNU_SOURCE

/* Includes ------------------------------------------------------------------*/
#include <sched.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Private variables ---------------------------------------------------------*/

static volatile sig_atomic_t child_alive = 1;

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Signal handler for ignored signals
 * @param signal Signal number
 * @return None
 */
static void catch_sig_ign(int signal)
{
    (void)printf("\nIgnored signal: %d\n", signal);
}

/**
 * @brief Error exit function
 * @param msg Message to print
 * @return None
 */
static void error_exit(const char* msg)
{
    perror(msg);
    _exit(1); /* error */
}

/**
 * @brief Child termination signal handler
 * @param signal Signal number
 * @return None
 */
static void handle_sigchild(int signal)
{
    (void)signal; /* Unused parameter */
    /* Non-blocking wait on child termination */
    while (0 < waitpid(-1, NULL, WNOHANG)) {
    }
    child_alive = 0;
    (void)printf("Child was terminated: %d\n", signal);
}

/* Public function -----------------------------------------------------------*/
/**
 * @brief Main function
 * @return 0 on success, 1 on error
 */
int main(void)
{
    /* Messages to send */
    const char* msg_to_send[] = {"Hello from child!",
                                 "This is message #2.",
                                 "Almost done...",
                                 "Bye, see you soon :)",
                                 "exit"};
    const int num_msg         = sizeof(msg_to_send) / sizeof(msg_to_send[0]);

    /* Signals to ignore */
    const int signals[]      = {SIGINT, SIGQUIT, SIGTERM, SIGHUP, SIGUSR1};
    const size_t nbr_signals = sizeof(signals) / sizeof(signals[0]);

    /* Create a socket pair */
    int fd[2];
    static const int parentsocket = 0;
    static const int childsocket  = 1;
    if (-1 == socketpair(AF_UNIX, SOCK_STREAM, 0, fd)) {
        error_exit("socketpair error");
    }

    /* Ignore some signals */
    struct sigaction sig_ign = {
        .sa_handler = SIG_IGN,
    };
    for (size_t i = 0; i < nbr_signals; i++) {
        if (-1 == sigaction(signals[i], &sig_ign, NULL)) {
            error_exit("Child sigaction failed, SIG_IGN");
        }
    }

    /* Fork */
    pid_t pid = fork();
    if (0 == pid) { /* Child -------------------------------------------------*/
        /* Put child process on CPU 1 */
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(1, &cpuset);
        if (-1 == sched_setaffinity(getpid(), sizeof(cpu_set_t), &cpuset)) {
            error_exit("Child sched_setaffinity error");
        }

        /* Close the parent socket */
        close(fd[parentsocket]);

        /* Send signal periodically */
        for (int msg_cnt = 0; msg_cnt < num_msg; msg_cnt++) {
            if (-1 == write(fd[childsocket],
                            msg_to_send[msg_cnt],
                            strlen(msg_to_send[msg_cnt]) + (size_t)1)) {
                error_exit("Child write error");
            }
            unsigned int sleep_time = 2;
            do {
                sleep_time = sleep(sleep_time);
            } while (sleep_time > (unsigned int)0);
        }
        close(fd[childsocket]);

    } else if (0 < pid) { /* Parent ------------------------------------------*/
        /* Put parent process on CPU 0 */
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(0, &cpuset);
        if (-1 == sched_setaffinity(getpid(), sizeof(cpu_set_t), &cpuset)) {
            error_exit("Parent sched_setaffinity error");
        }

        /* Close the child socket */
        close(fd[childsocket]);

        /* Handle SIGCHLD signal */
        struct sigaction sig_child = {
            .sa_handler = handle_sigchild,
            .sa_flags   = SA_RESTART | SA_NOCLDSTOP,
        };
        if (-1 == sigaction(SIGCHLD, &sig_child, NULL)) {
            error_exit("Child sigaction failed, SIGCHLD");
        }

        /* Ignore some signals */
        struct sigaction sig_ign_notify = {
            .sa_handler = catch_sig_ign,
        };
        for (size_t i = 0; i < nbr_signals; i++) {
            if (-1 == sigaction(signals[i], &sig_ign_notify, NULL)) {
                error_exit("Parent sigaction failed");
            }
        }

        /* Loop forever & wait for messages */
        char buf[256];
        while (1) {
            ssize_t nbytes = read(fd[parentsocket], buf, sizeof(buf));
            if (0 < nbytes) {
                (void)printf("Received message from child: %s\n", buf);
                if(0 == strncmp(buf, "exit", 4)){
                    printf("Terminating application...\n");
                    break;
                }
            } else {
            }
        }
        close(fd[parentsocket]);
        (void)printf("Parent waiting for child termination\n");
        while (1 == child_alive);
        (void)printf("Parent terminating\n");
    } else {
    }
    return 0;
}
