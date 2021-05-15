/* slip_test.c - test Serial Line Internet Protocol */
/* */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

/* end byte */
#define SLIP_END 0xC0
/* escape byte */
#define SLIP_ESC 0xDB
/* escaped end */
#define SLIP_ESC_END 0xDC
/* escaped escape */
#define SLIP_ESC_ESC 0xDD

int serial_fd;

void tx_bytes(unsigned char *tx_buf, uint16_t len) {
    uint16_t i, buf_i;
    unsigned char b;
    unsigned char buffer[64];

    buf_i = 0;
    for (i = 0; i < len; i++) {
        b = tx_buf[i];

        /* check what byte this is */
        switch (b) {
        case SLIP_END:
            /* have to send 2 characters to escape END */
            buffer[buf_i++] = SLIP_ESC;
            printf("sending %x\n", buffer[buf_i-1]);
            buffer[buf_i++] = SLIP_ESC_END;
            printf("sending %x\n", buffer[buf_i-1]);
            break;
        case SLIP_ESC:
            /* have to send 2 characters to escape ESC */
            buffer[buf_i++] = SLIP_ESC;
            printf("sending %x\n", buffer[buf_i-1]);
            buffer[buf_i++] = SLIP_ESC_ESC;
            printf("sending %x\n", buffer[buf_i-1]);
            break;
        default:
            /* no need to escape or w/e */
            buffer[buf_i++] = b;
            printf("sending %x\n", buffer[buf_i-1]);
            break;
        }

        /* buffer blocks of 60 to send */
        if (buf_i > 60) {
            write(serial_fd, buffer, buf_i);
            buf_i = 0;
        }
    }

    /* send the end byte */
    buffer[buf_i++] = SLIP_END;

    /* write any remaining buffered pieces */
    if (buf_i != 0) {
        write(serial_fd, buffer, buf_i);
    }
}

int main(int argc, char **argv) {
    char *device = argv[1];
    unsigned long baud = 9600;
    struct termios config;
    unsigned char buffer[64] = {'p', 'o', 't', 'a', 't', 'o', 'p', 'o', 't', 'a', 't', 'o', 'p', 'o', 't', 'a', 't', 'o', 'p', 'o', 't', 'a', 't', 'o', 'p', 'o', 't', 'a', 't', 'o', 'p', 'o', 't', 'a', 't', 'o', 'p', 'o', 't', 'a', 't', 'o', 'p', 'o', 't', 'a', 't', 'o', 'p', 'o', 't', 'a', 't', 'o', 'p', 'o', 't', 'a', 't', 'o', '1', '2', '3', '4'};

    if (argc < 2 || argc > 3) {
        printf("usage: %s device [baud]\n", argv[0]);
        printf("on mac, use the /dev/cu device and not the /dev/tty device!\n");
        return 1;
    }

    if (argc == 3) {
        char *result;
        baud = strtoul(argv[2], &result, 10);
        if (*result != '\0') {
            printf("usage: %s device [baud]\n", argv[0]);
            return 1;
        }
    }

    serial_fd = open(device, O_RDWR | O_NDELAY | O_NOCTTY);
    if (serial_fd == -1) {
        fprintf(stderr, "can't open ");
        perror(device);
        exit(errno);
    }

    if (tcgetattr(serial_fd, &config) < 0) {
        perror("can't get serial attributes");
        exit(errno);
    }

    if (cfsetispeed(&config, baud) < 0 || cfsetospeed(&config, baud) < 0) {
        perror("can't set baud rate");
        exit(errno);
    }

    config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
    config.c_oflag = 0;
    config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    config.c_cflag &= ~(CSIZE | PARENB);
    config.c_cflag |= CS8; // full 8 bits
    config.c_cc[VMIN]  = 1;
    config.c_cc[VTIME] = 0;

    if (tcsetattr(serial_fd, TCSAFLUSH, &config) < 0) {
        perror("can't set serial attributes");
        exit(errno);
    }

    printf("connected to device successfully, now transmitting packets...\n");

    tx_bytes(buffer, sizeof(buffer));

    printf("doing raw input now; type anything to send (no escaping):");
    while (1) {
        size_t n = read(STDIN_FILENO, buffer, sizeof(buffer));
        write(serial_fd, buffer, n);
    }

    close(serial_fd);
    return 0;
}

