#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

//Function that we will call in error state and it will print through perror the msg we gave as an argument
#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

#define BUF_SIZE 1024   /* Maximum size for exchanged string */

struct shm_cl_dis {
    sem_t  sem1_wait_client;
    sem_t  sem2_wait_dispatcher;              
    size_t line_num;         /* The line that we need*/       
    char   line[BUF_SIZE];   /* Data being transferred */
};

struct shm_dis_serv {
    sem_t  sem1_wait_dispatcher;
    sem_t  sem2_wait_server;              
    size_t line_num;         /* The line that we need*/       
    char   line[BUF_SIZE];   /* Data being transferred */
};