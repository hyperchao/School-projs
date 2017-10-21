#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

int main(int argn, char **argv) {
    job_cmd cmd;
    cmd.type = STAT;

    // open pipes to write commond and read message
    if (access(CS_FIFO_PATH, F_OK) != 0 || access(SC_FIFO_PATH, F_OK) != 0) {
        fprintf(stderr, "scheduler is not running, exit\n");
        exit(EXIT_FAILURE);
    }
    int readfifo = open(SC_FIFO_PATH, O_RDONLY | O_NONBLOCK);
    int writefifo = open(CS_FIFO_PATH, O_WRONLY);
    char message[BUFSIZ] = {'\0'};
    if (writefifo < 0 || readfifo < 0) {
        fprintf(stderr, "unable to open fifo\n");
        exit(EXIT_FAILURE);
    }
    write(writefifo, &cmd, sizeof(cmd));
    while (read(readfifo, message, BUFSIZ) != BUFSIZ);
    printf("From scheduler:\n%s\n", message);
    close(writefifo);
    close(readfifo);

    return 0;
}

