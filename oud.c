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

int main(int argc, char **argv)
{
    const char *LOG_TAG = "OUD";
    serial_port sp = NULL;
    int res;
    char buf[256];
    int cnt = 0;
    FILE *in = stdin;
    size_t r;
    uint32_t size_r = 0;
    const char *path = "./oud";

    fprintf(stdout, "Welcome - %s %s\n", __DATE__, __TIME__);

    sp = uart_open(TTY_UART);
    if ((sp == INVALID_SERIAL_PORT) || (sp == CLAIMED_SERIAL_PORT)) {
        perror(TTY_UART);
        exit(-1);
    }
    // We need to flush input to be sure first reply does not comes from older byte transceive
    uart_flush_input(sp, true);
    uart_set_speed(sp, 115200);

#if 0
    // Test send
    const char *str = "Hello from " TTY_UART "\r\n";
    uart_send(sp, (uint8_t *)str, strlen(str), 0);
#endif

    if (*(argv + 1)) {
        path = *(argv + 1);
    }

    in = fopen(path, "rb");
    if (!in) {
        printf("Fail to open %s\n", path);
        return 1;
    }

    while (1) {
        uint8_t cmd[4] = { 0x55, 0xAA, 0x00, 16 };

        if (feof(in) || ferror(in)) {
            break;
        }

        // Receive message and timeout if nothing
        res = uart_recv(sp, buf_in, 255, NULL, 50);
        buf_in[res] = '\0';
        printf("%s", buf_in);

        r = fread(buf, 1, 16, in);
        // Send cmd
        cmd[3] = r;
        uart_send(sp, (uint8_t *)cmd, sizeof(cmd), 0);

        // Wait for ACK
        res = uart_recv(sp, buf_in, 255, NULL, 0);
        if (!(buf_in[0] == 0x55 && buf_in[1] == 0xAA)) {
            // Invalid ACK
            // Continue to resend cmd
            continue;
        }

        // Send data
        size_r += r;
        uart_send(sp, (uint8_t *)buf, r, 0);

        // Receive feedback
        res = uart_recv(sp, buf_in, 255, NULL, 0);
        buf_in[res] = '\0';
        printf("%s", buf_in);
    }

    uart_close(sp);

    return 0;
}

/* vim: set ts=4 sw=4 tw=0 list : */
