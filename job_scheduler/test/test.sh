#! /bin/sh

echo "xterm -e ../bin/scheduler"

xterm -e ../bin/scheduler &

echo "press enter to start:"

read p

echo "../bin/enq $(pwd)/job_1s"

../bin/enq $(pwd)/job_1s

echo

echo "../bin/enq wrong_job"

../bin/enq wrong_job

echo

echo "../bin/enq -p 2 $(pwd)/job_10s -l -p -g"

../bin/enq -p 2 $(pwd)/job_10s -l -p -g

echo

echo "../bin/enq -p 4 $(pwd)/job_100s"

../bin/enq -p 4 $(pwd)/job_100s

echo

echo "../bin/deq 1"

../bin/deq 1

echo

echo "../bin/deq 100"

../bin/deq 100

echo

echo "../bin/enq -p 1 $(pwd)/job_10s_fail"

../bin/enq -p 1 $(pwd)/job_10s_fail

echo

echo "../bin/stat"

../bin/stat

echo

