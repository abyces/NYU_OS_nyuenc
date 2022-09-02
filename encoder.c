//
// Created by apple on 11/1/21.
//

#include "encoder.h"
#include "task_builder.h"
#include "task_queue.h"
#include "threads.h"

unsigned char* encode(char* fp, int size){
    unsigned char* encoded = malloc(sizeof (unsigned char) * (TASK_SIZE*2 + 1));

    int cnt = 0;
    char *pre = (char*)fp, *cur = (char*)fp;
    unsigned char *ep = &encoded[0];
    int i = 0;

    while(i < size) {
        while(i<size && *pre == *cur){
            cur++;
            cnt++; i++;
        }
        *ep++ = toascii(*pre);
        *ep++ = (unsigned char)cnt;
//        *ep++ = toascii(*pre);
//        *ep++ = cnt + '0';
        pre = cur;
        cnt = 0;
    }

    *ep = '\0';

    return encoded;
}
