//
// Created by apple on 10/30/21.
//

#ifndef NYUENC_TASK_FUNC_H
#define NYUENC_TASK_FUNC_H

#endif //NYUENC_TASK_FUNC_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include<fcntl.h>

#define MODE_THREAD 1
#define MODE_NON_THREAD 0

int mode;

int task_builder(int file_num);
