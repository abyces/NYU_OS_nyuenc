CC=gcc
CFLAGS=-g -pedantic -pthread -std=gnu17 -Wall -Werror -Wextra 

.PHONY: all 
all: nyuenc 

nyuenc: nyuenc.o encoder.o task_queue.o task_builder.o threads.o	
	$(CC) -o $@ $^ $(CFLAGS)

nyuenc.o: nyuenc.c task_queue.h task_builder.h threads.h encoder.h

encoder.o: encoder.c encoder.h task_builder.h task_queue.h threads.h 

task_queue.o: task_queue.c task_queue.h threads.h

task_builder.o: task_builder.c task_builder.h task_queue.h threads.h

threads.o: threads.c threads.h task_queue.h task_builder.h encoder.h

.PHONY: clean
clean:
	rm -f *.o nyuenc
