//
// Created by apple on 10/30/21.
//

#include "threads.h"
#include "task_queue.h"
#include "task_builder.h"
#include "encoder.h"

void init_thread_pool(int pthread_num){
    pool = (struct Thread_pool*)malloc(sizeof (struct Thread_pool));
    pool->workers = malloc(sizeof (pthread_t) * pthread_num);
    buffer_stat = malloc(sizeof (int) * buffer_size);
    buffer_stat_cond = (pthread_cond_t*)malloc(sizeof (pthread_cond_t) * buffer_size);
    buffer_mutex = (pthread_mutex_t*)malloc(sizeof (pthread_mutex_t) * buffer_size);

    // initialize mutex and cond vars
    pool->remain_task_cnt = 0;
    pool->builder_done = 0;

    pool->remain_task_cnt_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    pool->builder_done_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

    pool->remain_task_cnt_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    pool->builder_done_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

    // init and lock all the result queue mutex
    for (int i = 0; i < buffer_size; i++){
        *(buffer_stat+i) = 0;
        buffer_stat_cond[i] = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
        buffer_mutex[i] = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    }

    // create workers
    for(int i = 0; i<pthread_num; i++){
        pthread_create(&pool->workers[i], NULL, grab_jobs, NULL);
    }

    // create writer
    writer_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    writer_done = 0;
    writer_done_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

    pthread_create(&writer, NULL, writer_threads, NULL);
}

// Worker threads
// grab the task from queue, encode and write to buffer
void* grab_jobs(){
    while(1) {
        pthread_mutex_lock(&pool->builder_done_mutex);
        if(!pool->builder_done){
            pthread_mutex_unlock(&pool->builder_done_mutex);
            // still jobs to be submitted but the queue might be empty
            pthread_mutex_lock(&pool->remain_task_cnt_mutex);
            while(pool->remain_task_cnt == 0)
                pthread_cond_wait(&pool->remain_task_cnt_cond, &pool->remain_task_cnt_mutex);
            pool->remain_task_cnt -= 1;
        } else {
            // about to quit but there might be remaining tasks in quueue
            pthread_mutex_unlock(&pool->builder_done_mutex);
            pthread_mutex_lock(&pool->remain_task_cnt_mutex);
            if(pool->remain_task_cnt == 0){
                pthread_mutex_unlock(&pool->remain_task_cnt_mutex);
                break;
            } else {
                pool->remain_task_cnt -= 1;
            }
        }

        // get job
        struct Task* mytask = pop();
        pthread_mutex_unlock(&pool->remain_task_cnt_mutex);

        int task_id = (int) mytask->tid;
        unsigned char* encoded = encode(mytask->fp, mytask->cp_size);

        // write to the buffer
        pthread_mutex_lock(buffer_mutex + task_id);
        *(buffer_stat + task_id) = 1;
        *(result_queue + task_id) = encoded;
        pthread_cond_signal(buffer_stat_cond + task_id);
        pthread_mutex_unlock(buffer_mutex + task_id);

        free(mytask);
    }

    return NULL;
}

// Writer thread
// read the encoded strings from buffer and print to stdout
void* writer_threads(){
    // reserved last-two chars for combination
    unsigned char last_char = '\0';
    unsigned char last_digit = '\0';
    int i = 0;

    while(i < buffer_size){
        int task_id = i;

        // read from buffer
        pthread_mutex_lock(buffer_mutex + task_id);
        if(*(buffer_stat + task_id) == 0)
            pthread_cond_wait(buffer_stat_cond + task_id, buffer_mutex + task_id);
        unsigned char* cur = *(result_queue + task_id);
        pthread_mutex_unlock(buffer_mutex + task_id);

        // process combination between tasks
        int len = (int)strlen((char*)cur);
        if(i == 0){
            last_char = *(cur + len - 2);
            last_digit = *(cur + len - 1);
            *(cur + len - 2) = '\0';
            printf("%s", cur);
        } else {
            if(cur[0] != last_char){
                printf("%c", last_char);
                printf("%c", last_digit);
            } else {
                cur[1] += last_digit;
//                cur[1] += last_digit - '0';
            }
            last_char = *(cur + len - 2);
            last_digit = *(cur + len - 1);
            *(cur + len - 2) = '\0';
            printf("%s", cur);
            free(cur);
        }
        i += 1;
    }

    printf("%c%c", last_char, last_digit);

    // notify the main thread that the task this finished
    pthread_mutex_lock(&writer_mutex);
    if(writer_done == 0)
        pthread_cond_signal(&writer_done_cond);
    writer_done = 1;
    pthread_mutex_unlock(&writer_mutex);

    return NULL;
}

// Writer for single thread
void writer_single(){
    unsigned char last_char;
    unsigned char last_digit;
    int i = 0;
    while(i < tasks->t_num){
        struct Task* task = pop();
        unsigned char* cur = encode(task->fp, task->cp_size);
        int len = (int)strlen((char*)cur);
        if(i == 0){
            last_char = *(cur + len - 2);
            last_digit = *(cur + len - 1);
            *(cur + len - 2) = '\0';
            printf("%s", cur);
        } else {
            if(cur[0] != last_char){
                printf("%c%c", last_char, last_digit);
            } else {
//                cur[1] += last_digit - '0';
                cur[1] += last_digit;
            }
            last_char = *(cur + len - 2);
            last_digit = *(cur + len - 1);
            *(cur + len - 2) = '\0';
            printf("%s", cur);
        }

        i += 1;
    }
    printf("%c%c", last_char, last_digit);
}
