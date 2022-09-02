//
// Created by apple on 10/29/21.
//

#ifndef NYUENC_TASK_QUEUE_H
#define NYUENC_TASK_QUEUE_H

#endif //NYUENC_TASK_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>


// single task
struct Task{
    unsigned int tid;   //task id,  = tnum - 1
    char* fp;
    int cp_size;

    struct Task* next;
};

// task queue
struct Task_queue{
    int t_num;    // task_id, num, starts from 0.

    struct Task* fake_head;
    struct Task* head;
};

struct Task_queue *tasks;
unsigned char** result_queue;
char** file_ptrs;
unsigned int * file_remain_size;

void task_queue_constructor(char** ipt_file, int file_num);
void push(struct Task* task);
struct Task* pop();
int empty();
void print_task_info();
