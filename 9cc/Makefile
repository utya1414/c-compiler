GFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
		$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
		./test.sh

clean: 
		rm -f 9cc *.o *~ tmp*

start:
	docker start 3ee03c6933d6

exec:
	docker exec -it 3ee03c6933d6 /bin/bash

gen_tmp:
	gcc -g -static tmp.s tmp2.o -o tmp

.PHONY: test clean exec start gen_tmp