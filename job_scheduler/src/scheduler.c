#include <sys/time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#define _GNU_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pwd.h>
#include "common.h"

#define INTERVAL 10

int readfifo, writefifo; // fifo file descriptor
int nullfile; //
char message[BUFSIZ]; // stores message for scheduler send to client
job_info *running_job = NULL; // always set to NULL if there is no job running
int next_jid = 0; // next available jid can be assigned
wait_queue wait_q = {&wait_q, &wait_q, NULL}; // a circular list of READY jobs with a head node

void signal_handler(int signum);
void schedule();
void do_enq(const job_cmd *cmd);
void do_deq(const job_cmd *cmd);
void do_stat(const job_cmd *cmd);

void update_jobs(wait_queue *q, job_info *running);
wait_queue *search_job(wait_queue *q, int jid);
void remove_from_queue(wait_queue *node); // only remove records from wait queue
void delete_job(job_info *job); // clear job information
void insert_job(wait_queue *q, job_info *job); // insert job to wait queue according to priority and wait_time
// so the queue is always orderd

int stringfy_info(job_info *job, int offset); // append job information to message[]

int main(int argn, char **argv) {

    struct itimerval timer;
    struct sigaction action;

    // init timer and register signal handler
    timer.it_interval = timer.it_value = (struct timeval){0, INTERVAL};
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
    memset(&action, 0, sizeof(action));
    action.sa_handler = signal_handler;
    sigaction(SIGVTALRM, &action, NULL);
    sigaction(SIGCHLD, &action, NULL);

    // open pipe to communicate with client
    if (access(CS_FIFO_PATH, F_OK) == 0) {
        remove(CS_FIFO_PATH);
    }
    if (access(SC_FIFO_PATH, F_OK) == 0) {
        remove(SC_FIFO_PATH);
    }
    mkfifo(CS_FIFO_PATH, 0666);
    mkfifo(SC_FIFO_PATH, 0666);
    readfifo = open(CS_FIFO_PATH, O_RDONLY | O_NONBLOCK);

    // open null device
    nullfile = open("/dev/null", O_WRONLY);

    // enter loop to keep running
    while (true);

    return 0;
}

void signal_handler(int signum) {
    if (signum == SIGVTALRM) {
        // time slice runs out, update jobs and re-schedule
        update_jobs(&wait_q, running_job); 
        schedule();
    }
    else if (signum == SIGCHLD) {
        int status;
        int pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED);

        // if a job has exited or killed by user, print information
        if (WIFEXITED(status) && running_job && running_job->pid == pid) {
            delete_job(running_job);
            running_job = NULL;
            printf("job(pid = %d) %s\n", pid, WEXITSTATUS(status) == EXIT_SUCCESS?"done":"failed");
        }
        else if (WIFSIGNALED(status)) {
            printf("job(pid = %d) removed by user\n", pid);
        }
    }
}

void schedule() {
    // read cmd from pipe
    job_cmd cmd;
    if (read(readfifo, &cmd, sizeof(cmd)) == sizeof(cmd)) {
        switch (cmd.type) {
        case ENQ:   do_enq(&cmd);   break;
        case DEQ:   do_deq(&cmd);   break;
        case STAT:  do_stat(&cmd);  break;
        }
        writefifo = open(SC_FIFO_PATH, O_WRONLY | O_NONBLOCK);
        write(writefifo, message, BUFSIZ); // write message back to client
    }

    // stop current running job and insert it into wait queue
    if (running_job) {
        kill(running_job->pid, SIGSTOP);
        running_job->current_priority = running_job->default_priority;
        running_job->state = READY;
        running_job->wait_time = 0;
        insert_job(&wait_q, running_job);
        running_job = NULL;
    }

    // run job with higest priority(the head of wait queue), if any.
    if (wait_q.next != &wait_q) {
        running_job = wait_q.next->job;
        running_job->state = RUNNING;
        running_job->wait_time = 0;
        kill(running_job->pid, SIGCONT);
        remove_from_queue(wait_q.next);
    }
}

void do_enq(const job_cmd *cmd) {
    char **args =(char**)malloc(sizeof(char*)*(cmd->arg_num + 1));
    int i, k;
    const char *str = cmd->data;

    // parse cmd
    for (i = 0; i < cmd->arg_num; i += 1) {
        for (k = 0; str[k] != ' '; k += 1);
        args[i] = (char*)malloc(sizeof(char)*(k + 1));
        strncpy(args[i], str, k);
        args[i][k] = '\0';
    }
    args[cmd->arg_num] = '\0';

    // fork a child process and stop it.
    int fd[2];
    pipe2(fd, O_CLOEXEC);
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) { // child process
        dup2(nullfile, 1); // redirect stdout to /dev/null
        execv(args[0], &args[1]);
        // if exec fails, let parent know
        close(fd[0]);
        bool success = false;
        write(fd[1], &success, sizeof(bool));
        close(fd[1]);
        _exit(EXIT_FAILURE);
    }
    else { // parent process
        close(fd[1]);
        bool success = true;
        read(fd[0], &success, sizeof(bool));
        close(fd[0]);
        if (success) { // generate job info and insert to wait queue
            kill(pid, SIGSTOP);
            job_info *new_job = (job_info*)malloc(sizeof(job_info));
            new_job->jid = next_jid++;
            new_job->pid = pid;
            new_job->cmd_args = args;
            new_job->current_priority = new_job->default_priority = cmd->default_priority;
            new_job->owner_id = cmd->owner_id;
            new_job->wait_time = 0;
            new_job->create_time = time(0);
            new_job->run_time = 0;
            new_job->state = READY;
            insert_job(&wait_q, new_job);
            printf("user submit job(jid = %d, pid = %d)\n", new_job->jid, new_job->pid);
            snprintf(message, BUFSIZ, "successful add job, id =  %d", new_job->jid);
        }
        else { // write error message if failed to add a job
            snprintf(message, BUFSIZ, "incorrect job, check arguments");
        }
    }
}

void do_deq(const job_cmd *cmd) {
    int jid = atoi(cmd->data);
    bool found = false;

    if (running_job && running_job->jid == jid) {
        kill(running_job->pid, SIGKILL);
        delete_job(running_job);
        running_job = NULL;
        found = true;
    }
    else {
        wait_queue *to_remove = search_job(&wait_q, jid);
        if (to_remove) {
            kill(to_remove->job->pid, SIGKILL);
            delete_job(to_remove->job);
            remove_from_queue(to_remove);
            found = true;
        }
    }
    if (found) {
        snprintf(message, BUFSIZ, "remove job(id = %d)", jid);
    }
    else {
        snprintf(message, BUFSIZ, "no job matches id %d", jid);
    }
}

void do_stat(const job_cmd *cmd) {
    int len = sprintf(message, "%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
                      "jid", "pid", "user name", "run time", "wait time", "create time", "state");
    if (running_job) {
       len += stringfy_info(running_job, len);
    }
    for (wait_queue *p = wait_q.next; p != &wait_q; p = p->next) {
        len += stringfy_info(p->job, len);
    }
}

void update_jobs(wait_queue *q, job_info *running) {
    // add run time for running job and add wait time for every job in wait queue
    if (running) {
        running->run_time += INTERVAL;
    }
    for (wait_queue *p = q->next; p != q; p = p->next) {
        job_info *job = p->job;
        job->wait_time += INTERVAL;
        int priority = job->default_priority + job->wait_time/100;
        job->current_priority = (priority > 3)?3:priority;
    }
}

wait_queue *search_job(wait_queue *q, int jid) {
    for (wait_queue *p = q->next; p != q; p = p->next) {
        if (p->job->jid == jid) {
            return p;
        }
    }
    return NULL;
}

void remove_from_queue(wait_queue *node) {
    wait_queue *prev = node->prev, *next = node->next;
    prev->next = next;
        next->prev = prev;
    free(node);
}

void delete_job(job_info *job) {
    char **args = job->cmd_args;
    while (*args != '\0') {
        free(*args);
        args += 1;
    }
    free(job);
}

void insert_job(wait_queue *q, job_info *job) {
    wait_queue *pos = q;
    while (pos->next != q) {
        // newly inserted job's wait time is always 0, so we only have to compare priority.
        if (pos->next->job->current_priority < job->current_priority) {
            break;
        }
        pos = pos->next;
    }
    wait_queue *node = (wait_queue*)malloc(sizeof(wait_queue));
    node->job = job;
    node->next = pos->next;
    node->prev = pos;
    node->next->prev = node;
    pos->next = node;
}

int stringfy_info(job_info *job, int offset) {
    char time_str[20];
    strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", localtime(&job->create_time));
    return snprintf(&message[offset], BUFSIZ - offset,
                    "%d\t%d\t%s\t%dms\t%dms\t%s\t%s\n",
                    job->jid,
                    job->pid,
                    getpwuid(job->owner_id)->pw_name,
                    job->run_time,
                    job->wait_time,
                    time_str,
                    job->state == READY?"READY":"RUNING");
}
