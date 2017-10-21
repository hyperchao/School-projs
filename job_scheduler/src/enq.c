#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

void generate_cmd(job_cmd *cmd, cmd_type t, int arg_num, int priority, char **argv);

int main(int argn, char **argv) {
    // parse cmd arguments
    job_cmd cmd;
    if (argn >= 4 && strcmp(argv[1], "-p") == 0) { // with -p option
        generate_cmd(&cmd, ENQ, argn - 3, atoi(argv[2]), &argv[3]);
    }
    else if (argn >= 2) { // without -p option
        generate_cmd(&cmd, ENQ, argn - 1, 0, &argv[1]);
    }
    else {
        fprintf(stderr, "Usage: enq [-p num] e_file args\n");
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
    if (writefifo < 0 || readfifo < 0) {
        fprintf(stderr, "unable to open fifo\n");
        exit(EXIT_FAILURE);
    }
    write(writefifo, &cmd, sizeof(cmd));
    while (read(readfifo, message, BUFSIZ) != BUFSIZ);
    printf("From scheduler: %s\n", message);
    close(writefifo);
    close(readfifo);

    return 0;
}

void generate_cmd(job_cmd *cmd, cmd_type t, int arg_num, int priority, char **argv) {
    cmd->type = t;
    cmd->arg_num = arg_num;
    cmd->owner_id = getuid();
    cmd->default_priority = (priority >= 0 && priority <= 3)?priority:0;

    int i = 0, j;
    for (j = 0; j < arg_num; j += 1) {
        char *p = argv[j];
        while (*p != '\0') {
            cmd->data[i++] = *p++;
        }
        cmd->data[i++] = ' ';
    }
    cmd->data[i] = '\0';
}
