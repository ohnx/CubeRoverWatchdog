/* uart_test.c - test uart */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_LEN 1024

unsigned char my_packet[] = {
    0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65,

    0x00, // seq num
    0x00, 0x07, // length
    0x00, 0x00, // checksum
    0x00, 0xBA, 0xDA, 0x55, // type
    0xF3, 0x00, // opcode
    0x08 // component
};

int serial_fd;

void sig_handler(int signo) {
    close(serial_fd);
    exit(0);
}

void hexDump(const char * desc, const void * addr, const int len) {
    int i;
    unsigned char buff[17];
    const unsigned char * pc = (const unsigned char *)addr;

    // Output description if given.

    if (desc != NULL)
        printf ("%s:\n", desc);

    // Length checks.

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    else if (len < 0) {
        printf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }

    // Process every byte in the data.

    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Don't print ASCII buffer for the "zeroth" line.

            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.

            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And buffer a printable ASCII character for later.

        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.

    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII buffer.

    printf ("  %s\n", buff);
}


int main(int argc, char **argv) {
    char *device = argv[1];
    unsigned long baud = 9600;
    struct termios config;
    int n;
    char buffer[BUF_LEN];

    printf("%ld\n", sizeof(my_packet));
    
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
    config.c_cc[VMIN]  = 0;
    config.c_cc[VTIME] = 0;

    if (tcsetattr(serial_fd, TCSAFLUSH, &config) < 0) {
        perror("can't set serial attributes");
        exit(errno);
    }

    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("can't catch SIGINT\n");

    printf("> ");
    while (fgets(buffer, BUF_LEN, stdin)) {
        n = strlen(buffer);
        if (buffer[n - 1] == '\n') {
            buffer[n - 1] = '\0';
        } else {
            n += 1;
        }
        write (serial_fd, buffer, n);
//        write(serial_fd, my_packet, sizeof(my_packet));
        buffer[0] = 0xC0;
        write(serial_fd, buffer, 1);
        sleep(1);
        if ((n = read(serial_fd, buffer, BUF_LEN - 1))) {
            printf("< ");
            hexDump(NULL, buffer, n);
        }
        printf("> ");
    }

    close(serial_fd);
    return 0;
}

