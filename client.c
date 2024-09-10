/* client.c*/
#include <time.h>
#include "client-server.h"

int main(int argc, char *argv[]){
    int            fd;
    char           *shmpath;
    size_t         num_line;
    struct shm_cl_dis  *cl_dis;
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    if (argc != 3) {
        fprintf(stderr, "Usage: %s /shm-path string\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    //shared memory object name, the name of shm MUST have as inittial character the charachter /
    shmpath = argv[1];
    num_line = atoi(argv[2]);

    fd = shm_open(shmpath, O_RDWR, 0);
    if (fd == -1) errExit("shm_open");
    
    cl_dis = mmap(NULL, sizeof(*cl_dis), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (cl_dis == MAP_FAILED) errExit("mmap");

    //printf("Shared memory object \"%s\" has been created at address\"%p\"\n", shmpath, cl_dis);

    close(fd);          /* 'fd' is no longer needed */
    //Wait for access in shm1
    if (sem_wait(&cl_dis->sem1_wait_client) == -1) errExit("sem_post");
    cl_dis->line_num = num_line;
    /*Wake up dispatcher!*/
    if (sem_post(&cl_dis->sem2_wait_dispatcher) == -1) errExit("sem_post");
    
    //Wait to take back the content of the line
    if (sem_wait(&cl_dis->sem1_wait_client) == -1) errExit("sem_post");
    //memcpy(&shmp->buf, string, len);
    if(strcmp(cl_dis->line,"no") == 0){
        printf("Server couldn't find line %ld.\n", num_line);
    }
    else if(strcmp(cl_dis->line,"exit") == 0){
        printf("Exit from the program client - server\n");
    }
    else{
        printf("Client: %s\n",cl_dis->line);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("client took %f seconds to execute \n", cpu_time_used); 
    exit(EXIT_SUCCESS);
}