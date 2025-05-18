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
 * Date:   18.05.2025
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>

/* Private functions ---------------------------------------------------------*/
/**
 * @brief Helper function to write a string to a file
 * @param path Path to the file
 * @param str String to write
 * @return 0 on success, -1 on error
 */
static int write_to_file(const char* path, const char* str)
{
    FILE* file = fopen(path, "w");
    if (NULL == file) {
        perror("Failed to open file");
        return -1;
    }
    if (0 > fprintf(file, "%s", str)) {
        perror("Failed to write to file");
        (void)fclose(file);
        return -1;
    }
    (void)fclose(file);
    return 0;
}

/**
 * @brief PI calculation function
 * @param n Number of iterations
 * @return PI value
 */
static double calculate_pi(int n)
{
    double pi = 0.0;
    for (int i = 0; i < n; i++) {
        pi += (4.0 * (((i % 2) == 0) ? 1 : -1)) / ((2.0 * i) + 1.0);
    }
    return pi;
}

/* Public function -----------------------------------------------------------*/

/**
 * @brief Main function
 * @return 0 on success, 1 on error
 */
int main(void)
{
    /* Mount the CGroups */
    mount("none", "/sys/fs/cgroup", "tmpfs", 0, "");
    mkdir("/sys/fs/cgroup/cpuset", 0755);
    mount("cpu,cpuset", "/sys/fs/cgroup/cpuset", "cgroup", 0, "cpuset");
    mkdir("/sys/fs/cgroup/cpuset/high", 0755);
    mkdir("/sys/fs/cgroup/cpuset/low", 0755);

    (void)write_to_file("/sys/fs/cgroup/cpuset/high/cpuset.cpus", "3");
    (void)write_to_file("/sys/fs/cgroup/cpuset/high/cpuset.mems", "0");
    (void)write_to_file("/sys/fs/cgroup/cpuset/low/cpuset.cpus", "2");
    (void)write_to_file("/sys/fs/cgroup/cpuset/low/cpuset.mems", "0");

    /* create two process */
    pid_t pid1 = fork();
    if (pid1 == 0) {
        /* Child process */
        pid_t pid = getpid();
        char pid_str[10];
        (void)snprintf(pid_str, sizeof(pid_str), "%d", pid);
        (void)write_to_file("/sys/fs/cgroup/cpuset/high/tasks", pid_str);
        (void)printf("pid1 (high): %d\n", pid);
        double pi = 0.0;
        for (int i = 1; i < 10000000; i++) {
            double new_pi = calculate_pi(i);
            if (pi < new_pi) {
                pi = new_pi;
            }
        }
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        /* Child process */
        pid_t pid = getpid();
        char pid_str[10];
        (void)snprintf(pid_str, sizeof(pid_str), "%d", pid);
        (void)write_to_file("/sys/fs/cgroup/cpuset/low/tasks", pid_str);
        (void)printf("pid2 (low): %d\n", pid);
        double pi = 0.0;
        for (int i = 1; i < 10000000; i++) {
            double new_pi = calculate_pi(i);
            if (pi < new_pi) {
                pi = new_pi;
            }
        }
    }
    return 0;
}
