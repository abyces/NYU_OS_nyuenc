//
// Created by apple on 10/30/21.
//

#ifndef NYUENC_THEADS_H
#define NYUENC_THEADS_H

#endif //NYUENC_THEADS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//#define BUFFER_SIZE 5000
#define TASK_SIZE 4096

struct Thread_pool{
    pthread_t *workers;

    int remain_task_cnt;   // cond variable for recording remaining tasks in the queue
    int builder_done;  // cond variable for notifying whether jobs are done

    pthread_mutex_t remain_task_cnt_mutex;  // used for control push, pop, and record remain tasks
    pthread_mutex_t builder_done_mutex;

    pthread_cond_t remain_task_cnt_cond;
    pthread_cond_t builder_done_cond;
};

struct Thread_pool *pool;

// mutex for writer thread, control main thread to quit
pthread_mutex_t writer_mutex;
pthread_t writer;
int writer_done;
pthread_cond_t writer_done_cond;

// mutex for result queue, control read / write for result queue
int buffer_size;
int* buffer_stat;
pthread_cond_t* buffer_stat_cond;
pthread_mutex_t* buffer_mutex;


// functions
void init_thread_pool(int t_num);
void* grab_jobs();
void* writer_threads();
void writer_single();
