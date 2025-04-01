#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>

#define DEVICE_PATH "/dev/mymodule"

int main() {
    int fd;
    int interrupt_count = 0;
    fd_set rfds;

    // Open device file
    fd = open(DEVICE_PATH, O_RDONLY);

    printf("Wait for interrupts...\n");

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    while (1) {
        select(fd + 1, &rfds, NULL, NULL, NULL);

        interrupt_count++;
        printf("Interrupt occured ! Total number of interrupts : %d\n", interrupt_count);
    }

    close(fd);
    return 0;
}
