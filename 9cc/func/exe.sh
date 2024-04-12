#!/bin/bash
cc -c func.c
cc -o test func.o test.o
ls
./test