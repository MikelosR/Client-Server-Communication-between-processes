/* dispatcher.c*/
#include "client-server.h"

int main(int argc, char *argv[]){
    int            fd1, fd2, retval = -1;
    char           *shmpath1, *shmpath2;
    struct shm_cl_dis  *cl_dis;
    struct shm_dis_serv *dis_serv;
    pid_t server_pid;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s /shm-path \"file_name\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    //if server file exists, delete it and try again
    if (access("/tmp/server_on.txt", 0) == 0) {
        printf("Server is ON\n");
        printf("The file /tmp/server_on.txt deleted, try again to run the dispatcher\n");
        remove("/tmp/server_on.txt");
        exit(EXIT_FAILURE);
    }
    else {
        printf("Dispatcher is ON\n");
        printf("Server is OFF\n");
        //child (run the server)
        if((server_pid = fork()) == 0){
            retval = execl("./server", "server", argv[2],argv[3], NULL);
            //this part will not run (exec rules)
            if(retval == -1){
                perror("execl");
                exit(5);
            }
        }
    }
    printf("Server now is ON\n");
    //the name of shared memory object
    shmpath1 = argv[1];
    shmpath2 = argv[2];

    /* Create shared's memory objects and set its size to the size of our structure. */
    shm_unlink(shmpath1);
    fd1 = shm_open(shmpath1, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (fd1 == -1)
        errExit("shm1_open");

    shm_unlink(shmpath2);
    fd2 = shm_open(shmpath2, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (fd2 == -1)
        errExit("shm2_open");
    
    //initially, the size of shared memory is 0
    if (ftruncate(fd1, sizeof(struct shm_cl_dis)) == -1)
        errExit("ftruncate1");
    if (ftruncate(fd2, sizeof(struct shm_dis_serv)) == -1)
        errExit("ftruncate2");

    /* Map the objects into the caller's address space. */
    cl_dis = mmap(NULL, sizeof(*cl_dis), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
    if (cl_dis == MAP_FAILED)
        errExit("mmap1");
    dis_serv = mmap(NULL, sizeof(*dis_serv), PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
    if (dis_serv == MAP_FAILED)
        errExit("mmap2");
    
    //Empty buffers and ints
    cl_dis->line_num = 0;
    cl_dis->client_pid = 0;
    dis_serv->line_num = 0;
    memset(cl_dis->line,0,strlen(cl_dis->line));
    memset(dis_serv->line,0,strlen(dis_serv->line));

    close(fd1);          /* 'fd's is no longer needed */
    close(fd2);

    /* Initialize semaphores as process-shared, with value 0. */
    //initialize sem1_wait_client to 1
    if (sem_init(&cl_dis->sem1_wait_client, 1, 1) == -1)
        errExit("sem_init-sem1 client-dispatcher");
    if (sem_init(&cl_dis->sem2_wait_dispatcher, 1, 0) == -1)
        errExit("sem_init-sem2 client-dispatcher");

    if (sem_init(&dis_serv->sem1_wait_dispatcher, 1, 0) == -1)
        errExit("sem_init-sem1 dispatcher-server");
    if (sem_init(&dis_serv->sem2_wait_server, 1, 0) == -1)
        errExit("sem_init-sem2 dispatcher-server");
    
    bool not_exit = true;
    while(not_exit){
        printf("-Dispatcher: I am waiting the customer to type line number\n");

        /* Wait client to add the line number that it wants. */
        if (sem_wait(&cl_dis->sem2_wait_dispatcher) == -1)
            errExit("sem_wait client");
        
        printf("-Dispatcher: Client wants the line %ld\n",cl_dis->line_num);

        //Give the line number to the server, and wait response from the server
        dis_serv->line_num = cl_dis->line_num;

        //Wake up server, i have the line number
        if (sem_post(&dis_serv->sem2_wait_server) == -1)
            errExit("sem_wait server");

        /* Wait the server to add the content of the line. */
        if (sem_wait(&dis_serv->sem1_wait_dispatcher) == -1)
            errExit("sem_wait server");

        //Copy the contents of the line
        strcpy(cl_dis->line,dis_serv->line);

        //if server wrote exit, dispatcher exit
        if(strcmp(dis_serv->line,"exit") == 0) not_exit = false;

        /*Wake up client*/
        if(sem_post(&cl_dis->sem1_wait_client) == -1)
            errExit("sem_post client");
    }

    shm_unlink(shmpath1);
    shm_unlink(shmpath2);
    exit(EXIT_SUCCESS);
}