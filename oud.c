#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/ioctl.h>

#include "uart.h"

#define DEBUG               1

#define TTY_UART            "/dev/tty.usbserial-A900fZZq"

#define BUFFER_SIZE         256

static uint8_t buf_in[BUFFER_SIZE];
#if DEBUG
static uint8_t buf2[BUFFER_SIZE / 2];
#endif

int main(void)
{
    const char *LOG_TAG = "IOD";
    serial_port sp = NULL;
    int res;

    fprintf(stdout, "Welcome - %s %s\n", __DATE__, __TIME__);

    sp = uart_open(TTY_UART);
    if ((sp == INVALID_SERIAL_PORT) || (sp == CLAIMED_SERIAL_PORT)) {
        perror(TTY_UART);
        exit(-1);
    }
    // We need to flush input to be sure first reply does not comes from older byte transceive
    uart_flush_input(sp, true);
    uart_set_speed(sp, 115200);

    // Test send
    const char *str = "Hello from " TTY_UART "\r\n";
    uart_send(sp, (uint8_t *)str, strlen(str), 0);

    while (1) {
        // Test uart_recv
        res = uart_recv(sp, buf_in, 255, NULL, 0);
        buf_in[res] = '\0';

        printf("%s\n", buf_in);
        if (strcmp((char *)buf_in, "quit") == 0) {
            break;
        }

        printf("...\n");
        uart_send(sp, (uint8_t *)str, strlen(str), 0);
    }

    uart_close(sp);

    return 0;
}

/* vim: set ts=4 sw=4 tw=0 list : */
