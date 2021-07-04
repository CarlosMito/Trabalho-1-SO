#!/bin/bash

gcc -c src/linkedList.c
gcc -c src/program.c
gcc -c src/pcb.c
gcc -c src/cpu.c

gcc -o commander src/commander.c
gcc -o manager linkedList.o program.o cpu.o pcb.o src/manager.c
