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
#define NBR_BLOCKS (50)
#define MEBIBYTE (1024 * 1024)

#define CGROUP_PATH "/sys/fs/cgroup/memory"
#define CGROUP_MEM_PATH "/sys/fs/cgroup/memory/mem"
#define CGROUP_MEM_LIMIT_PATH "/sys/fs/cgroup/memory/mem/memory.limit_in_bytes"
#define TASKS_FILE "/sys/fs/cgroup/memory/mem/tasks"
/* Includes ------------------------------------------------------------------*/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>

/* Private functions ---------------------------------------------------------*/

/* Public function -----------------------------------------------------------*/

/**
 * @brief Main function
 * @return 0 on success, 1 on error
 */
int main(void)
{
    /* Mount the CGroups */
    mount("none", "/sys/fs/cgroup", "tmpfs", 0, "");
    mkdir("/sys/fs/cgroup/memory", 0755);
    mount("memory", "/sys/fs/cgroup/memory", "cgroup", 0, "memory");
    mkdir("/sys/fs/cgroup/memory/mem", 0755);

    pid_t pid = getpid();
    char pid_str[20];
    (void)snprintf(pid_str, sizeof(pid_str), "%d", pid);
    int fd = open("/sys/fs/cgroup/memory/mem/tasks", O_WRONLY);
    write(fd, pid_str, strlen(pid_str));
    close(fd);

    /* Set memory limit to 20 MiB */
    int fd2 = open("/sys/fs/cgroup/memory/mem/memory.limit_in_bytes", O_WRONLY);
    write(fd2, "20971520", 8);  // 20 MB in bytes
    close(fd2);

    /* Allocate NBR_BLOCKS of 1 MiB */
    char* pmem[NBR_BLOCKS];
    for (int i = 0; i < NBR_BLOCKS; i++) {
        pmem[i] = (char*)malloc(MEBIBYTE);
        if (pmem[i] == NULL) {
            (void)printf("Invalid pointer %d\n", i);
            break;
        }
        memset(pmem[i], 0, MEBIBYTE);
        (void)printf("Allocated %d MiB\n", i + 1);
    }

    for (int i = 0; i < NBR_BLOCKS; i++) {
        if (pmem[i] != NULL) {
            free(pmem[i]);
            pmem[i] = NULL;
        }
    }

    (void)printf("Freed all memory\n");
    return 0;
}
