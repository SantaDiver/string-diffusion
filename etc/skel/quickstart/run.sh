#!/bin/bash -x
mpisubmit.bg -n 128 -w 00:15:00 -m dual -e "OMP_NUM_THREADS=2" hello
