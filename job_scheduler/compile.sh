#! /bin/sh

gcc src/scheduler.c -o bin/scheduler
gcc src/enq.c -o bin/enq
gcc src/deq.c -o bin/deq
gcc src/stat.c -o bin/stat
