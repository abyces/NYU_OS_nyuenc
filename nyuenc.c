#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "task_queue.h"
#include "task_builder.h"
#include "threads.h"
#include "encoder.h"


int to_digits(char* argv);

int main(int argc, char **argv) {
    char** ipt_files = malloc(sizeof (char*) * argc);
    int c;
    int pthread_num = 0;
    int file_num = 0;

    // parse -j n
    while((c = getopt(argc, argv, "j:")) != -1 ){
        if(c == 'j'){
            pthread_num = to_digits(argv[optind - 1]);
            if (pthread_num == -1) fprintf(stderr, "invalid number\n");
        } else {
            abort();
        }
    }

    // parse file names
    for(int i = optind; i<argc; i++){
        ipt_files[file_num] = malloc(sizeof (char) * (strlen(argv[i]) + 1));
        strcpy(ipt_files[file_num], argv[i]);
        ipt_files[file_num][strlen(argv[i])] = '\0';
        file_num++;
    }


    if(pthread_num > 0 ) {
        mode = MODE_THREAD;
        // init task_queue, threads pool
        task_queue_constructor(ipt_files, file_num);
        init_thread_pool(pthread_num);
        // begin work
        task_builder(file_num);

        pthread_mutex_lock(&writer_mutex);
        if(writer_done == 0)
            pthread_cond_wait(&writer_done_cond, &writer_mutex);
        pthread_mutex_unlock(&writer_mutex);
    } else {
        mode = MODE_NON_THREAD;
        task_queue_constructor(ipt_files, file_num);
        // build and write
        task_builder(file_num);
        writer_single();
    }

    return 0;
}

int to_digits(char* argv){
    int num = 0;
    for(char* ptr = argv; *ptr; ptr++){
        if(*ptr >= '0' && *ptr <= '9')
            num = 10 * num + (*ptr - '0');
        else
            return -1;
    }

    return num;
}
