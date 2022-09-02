//
// Created by apple on 10/29/21.
//

#include "task_queue.h"
#include "threads.h"

void task_queue_constructor(char** ipt_file, int file_num){
    tasks = (struct Task_queue*)malloc(sizeof (struct Task_queue));
    tasks->fake_head = malloc(sizeof (struct Task));
    tasks->head = NULL;
    tasks->t_num = 0;    // task num

    // ptrs for each file and its size
    file_ptrs = (char**)malloc(sizeof (char*) * file_num);
    file_remain_size = (unsigned int *)malloc(sizeof (unsigned int) * file_num);

    // the tasks that will be created, each size = TASK_SIZE, 4096
    buffer_size = 0;
    for(int i = 0; i<file_num; i++){
        int fd = open(ipt_file[i], O_RDONLY);
        if(fd == -1){
            fprintf(stderr, "cannot open file\n");
            exit(-1);
        }

        struct stat statbuf;
        int err = fstat(fd, &statbuf);
        if(err < 0){
            fprintf(stderr, "cannot open file\n");
            exit(-1);
        }
        file_ptrs[i] = malloc(sizeof (char*));
        file_ptrs[i] = mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
        file_remain_size[i] = statbuf.st_size;

        buffer_size += (int)((statbuf.st_size + TASK_SIZE - 1) / TASK_SIZE);
    }

    result_queue = (unsigned char**)malloc(sizeof (unsigned char*) * buffer_size);
}

void push(struct Task* task){
    if(empty()) {
        tasks->head = task;
        tasks->fake_head->next = tasks->head;
    } else {
        tasks->head->next = task;
        tasks->head = tasks->head->next;
    }
}

struct Task* pop(){
    if(!empty()){
        struct Task* front = tasks->fake_head->next;
        tasks->fake_head->next = front->next;
        return front;
    } else {
        return NULL;
    }
}

int empty(){
    return tasks->fake_head->next == NULL;
}

void print_task_info(){
    struct Task* ptr = tasks->fake_head->next;
    while(ptr){
        printf("tid:%d, data:%s, size:%d\n", ptr->tid, ptr->fp, ptr->cp_size);
        ptr = ptr->next;
    }
}
