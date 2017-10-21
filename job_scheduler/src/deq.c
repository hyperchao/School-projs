#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"

bool is_number(const char *s) {
    if (*s == '\0') {
        return true;
    }
    return *s >= '0' && *s <= '9' && is_number(s + 1);
}

int main(int argn, char **argv) {
    // parse cmd argument
    job_cmd cmd;
    if (argn == 2 && is_number(argv[1])) {
        cmd.type = DEQ;
        sprintf(cmd.data, "%d", atoi(argv[1]));
    }
    else {
        fprintf(stderr, "Usage: deq jid(non-negative integer)\n");
        exit(EXIT_FAILURE);
    }

    // open pipes to write commond and read message
    if (access(CS_FIFO_PATH, F_OK) != 0 || access(SC_FIFO_PATH, F_OK) != 0) {
        fprintf(stderr, "scheduler is not running, exit\n");
        exit(EXIT_FAILURE);
    }
    int readfifo = open(SC_FIFO_PATH, O_RDONLY | O_NONBLOCK);
    int writefifo = open(CS_FIFO_PATH, O_WRONLY);
    char message[BUFSIZ] = {'\0'};
    if (readfifo < 0 || writefifo < 0) {
        fprintf(stderr, "unable to open fifo\n");
        exit(EXIT_FAILURE);
    }
    write(writefifo, &cmd, sizeof(cmd));
    while(read(readfifo, message, BUFSIZ) != BUFSIZ);
    printf("From scheduler: %s\n", message);
    close(writefifo);
    close(readfifo);

    return 0;
}

