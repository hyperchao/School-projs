#ifndef COMMON_H
#define COMMON_H

#include <time.h>
#include <unistd.h>

// client to server fifo path
#define CS_FIFO_PATH "/tmp/to_scheduler_fifo"
// server to client fifo path
#define SC_FIFO_PATH "/tmp/from_scheduler_fifo"

typedef enum {READY, RUNNING} job_state;

typedef enum {ENQ, DEQ, STAT} cmd_type;

typedef struct {
    int jid;
    pid_t pid;
    char **cmd_args;
    int default_priority;
    int current_priority;
    int owner_id;
    int wait_time;
    time_t create_time;
    int run_time;
    job_state state;
} job_info;

typedef struct {
    cmd_type type;
    int arg_num;
    int owner_id;
    int default_priority;
    char data[BUFSIZ];
} job_cmd;

typedef struct wait_queue wait_queue;

struct wait_queue {
    wait_queue *next, *prev;
    job_info *job;
};

#endif
