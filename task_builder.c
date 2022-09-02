//
// Created by apple on 10/30/21.
//

#include "task_builder.h"
#include "task_queue.h"
#include "threads.h"

int task_builder(int file_num){
    int cur_proc_fid = 0;
    while(cur_proc_fid < file_num){
        char* this_file = file_ptrs[cur_proc_fid];
        int doc_size_remained = (int)file_remain_size[cur_proc_fid];
        int start_pos = 0;
        while(start_pos < doc_size_remained){
            struct Task* task = malloc(sizeof (struct Task));
            task->tid = tasks->t_num; task->fp = this_file + start_pos; task->next = NULL;
            task->cp_size = start_pos + TASK_SIZE <= doc_size_remained? TASK_SIZE : doc_size_remained - start_pos;

            if(mode==MODE_THREAD) {
                pthread_mutex_lock(&pool->remain_task_cnt_mutex);
                push(task);
                tasks->t_num += 1;

                // we should wake up all worker if the queue changes state from empty to non-empty
                if (pool->remain_task_cnt == 0)
                    pthread_cond_broadcast(&pool->remain_task_cnt_cond);
                pool->remain_task_cnt += 1;

                // we should change this tag when it is the last job
                if (cur_proc_fid == file_num - 1 && start_pos + TASK_SIZE >= doc_size_remained) {
                    pthread_mutex_lock(&pool->builder_done_mutex);
                    pool->builder_done = 1;
                    pthread_mutex_unlock(&pool->builder_done_mutex);
                }

                pthread_mutex_unlock(&pool->remain_task_cnt_mutex);
            } else {
                push(task);
                tasks->t_num += 1;
            }

            start_pos += TASK_SIZE;
        }
        cur_proc_fid += 1;  // this file is done
    }
    return 0;
}
