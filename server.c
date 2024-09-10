/*server.c*/
#include "client-server.h"
#define MAX_LINE 1024

int main(int argc, char *argv[]){
    int            fd;
    char           *shmpath;
    struct shm_dis_serv  *dis_serv;
    FILE * fp;
    int num_line = 0;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s /shm-path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_txt_fd;
    remove("/tmp/server_on.txt");
    if ((server_txt_fd = open("/tmp/server_on.txt", O_WRONLY | O_CREAT | O_TRUNC,0777 )) < 0){
        perror("problem with creating txt file server");
        exit(5);
    }
    
    //The name of the shm segment
    shmpath = argv[1];
    fd = shm_open(shmpath, O_RDWR, 0);
    if (fd == -1) errExit("shm_open");
    
    dis_serv = mmap(NULL, sizeof(*dis_serv), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (dis_serv == MAP_FAILED)  errExit("mmap");
    close(fd);

    //Main loop of server
    bool not_exit = true;
    while(not_exit){

        //Wait the dispatcher num line
        if (sem_wait(&dis_serv->sem2_wait_server) == -1) errExit("sem_wait");
        num_line = dis_serv->line_num;
        //If client send 0, means exit
        if(num_line == 0){
            printf("-Server: I receive line 0. Safe EXIT from the program client - server\n");
            strcpy(dis_serv->line,"exit");
            not_exit = false;
        }
        else{   /*Try to open the file "argv[2]"*/
            fp = fopen(argv[2], "r");
            if (fp == NULL){
                printf("Server: Wrong file name %s. Safe EXIT\n",argv[2]);
                strcpy(dis_serv->line,"exit");
                if(sem_post(&dis_serv->sem1_wait_dispatcher) == -1)
                    errExit("sem_post dispatcher");
                remove("/tmp/server_on.txt");
                exit(EXIT_FAILURE);
            }
            bool keep_reading = true;
            bool find = false;
            int current_line = 1;

            //Loop for find the line who clients wants
            do{
                //read the next line from the file, store it into buffer
                fgets(dis_serv->line, MAX_LINE, fp);

                //if we've found the line the user is looking for, print it out
                if (current_line == num_line){
                    keep_reading = false;
                    find = true;
                    //printf("Line: %s\n", dis_serv->line);
                    //wake up dispatcher
                    if (sem_post(&dis_serv->sem1_wait_dispatcher) == -1)
                        errExit("sem_post dispatcher");
                }
                //if we've reached the end of the file, we didn't find the line
                if (feof(fp) && !find){
                    keep_reading = false;
                    strcpy(dis_serv->line,"no");
                    //wake up dispatcher
                    if (sem_post(&dis_serv->sem1_wait_dispatcher) == -1)
                        errExit("sem_post dispatcher");
                }
                current_line++;

            } while (keep_reading);
            fclose(fp);
        }
        
    }
    //wake up dispatcher
    if (sem_post(&dis_serv->sem1_wait_dispatcher) == -1)
        errExit("sem_post dispatcher");

    remove("/tmp/server_on.txt");
    exit(EXIT_SUCCESS);
}