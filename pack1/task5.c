#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include <pthread.h>

#define WOMEN 1
#define MEN 2

#define PEOPLE_NUM 20

typedef union semun {
    int val;
    struct semid_ds *buf;
    unsigned short array [3];
} semun;

int woman_wants_to_enter(const int semid) {
    struct sembuf sops[3];
    sops[0].sem_num = 0;
    sops[WOMEN].sem_num = WOMEN;
    sops[MEN].sem_num = MEN;

    sops[0].sem_flg = 0;
    sops[WOMEN].sem_flg = 0;
    sops[MEN].sem_flg = 0;

    sops[0].sem_op = -1;
    sops[MEN].sem_op = 0;
    sops[WOMEN].sem_op = 1;

    semop(semid, sops, 3);

    printf("woman entered\n");
}

int woman_leaves(const int semid) {
    struct sembuf sops[3];
    sops[0].sem_num = 0;
    sops[WOMEN].sem_num = WOMEN;
    sops[MEN].sem_num = MEN;

    sops[0].sem_flg = 0;
    sops[WOMEN].sem_flg = 0;
    sops[MEN].sem_flg = 0;

    sops[0].sem_op = 1;
    sops[MEN].sem_op = 0;
    sops[WOMEN].sem_op = -1;

    semop(semid, sops, 3);

    printf("woman left\n");
}

int man_wants_to_enter(const int semid) {
    struct sembuf sops[3];
    sops[0].sem_num = 0;
    sops[WOMEN].sem_num = WOMEN;
    sops[MEN].sem_num = MEN;

    sops[0].sem_flg = 0;
    sops[WOMEN].sem_flg = 0;
    sops[MEN].sem_flg = 0;

    sops[0].sem_op = -1;
    sops[MEN].sem_op = 1;
    sops[WOMEN].sem_op = 0;

    semop(semid, sops, 3);

    printf("man entered\n");
}

int man_leaves(const int semid) {
    struct sembuf sops[3];
    sops[0].sem_num = 0;
    sops[WOMEN].sem_num = WOMEN;
    sops[MEN].sem_num = MEN;

    sops[0].sem_flg = 0;
    sops[WOMEN].sem_flg = 0;
    sops[MEN].sem_flg = 0;

    sops[0].sem_op = 1;
    sops[MEN].sem_op = -1;
    sops[WOMEN].sem_op = 0;

    semop(semid, sops, 3);

    printf("man left \n");
}

void* woman(void* data) {
    int semid = *(int*)data;
    woman_wants_to_enter(semid);
    sleep(1);
    woman_leaves(semid);
}

void* man(void* data) {
    int semid = *(int*)data;
    man_wants_to_enter(semid);
    sleep(1);
    man_leaves(semid);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("usage: %s N\n", argv[0]);
        return 1;
    }
    int n;
    if (!sscanf(argv[1], "%d", &n)) {
        printf("invalid N\n");
        return 1;
    }
    key_t key = ftok("semaphore", 123);

    if(key == -1) {
        printf("fail to get key\n");
        return 0;
    }

    semun sem_attr;
    sem_attr.val = n;
    int semid = semget(key, 3, 0666|IPC_CREAT);

    if (semid == -1) {
        printf("fail to get semaphore\n");
        return 1;
    }

    if(semctl(semid, 0, SETVAL, sem_attr) == -1) {
        semctl(semid, 0, IPC_RMID);
        printf("semctl error\n");
    }
    
    sem_attr.val = 0;
    if (semctl(semid, MEN, SETVAL, sem_attr) == -1) {
        semctl(semid, 0, IPC_RMID);
        printf("semctl error\n");
    }
    if (semctl(semid, WOMEN, SETVAL, sem_attr) == -1) {
        semctl(semid, 0, IPC_RMID);
        printf("semctl error\n");
    }

    pthread_t people[PEOPLE_NUM];

    for (int i = 0; i < PEOPLE_NUM; i = i + 2) {
        if (pthread_create(&people[i], NULL, woman, &semid) == -1) {
            semctl(semid, 0, IPC_RMID);
            printf("thread create fail\n");
        }
        if (pthread_create(&people[i + 1], NULL, man, &semid) == -1) {
            semctl(semid, 0, IPC_RMID);
            printf("thread create fail\n");
        }
    }

    for (int i = 0; i < PEOPLE_NUM; i++) {
        pthread_join(people[i], NULL);
    }

    semctl(semid, 0, IPC_RMID);
}