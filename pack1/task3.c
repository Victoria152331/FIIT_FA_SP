#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#define NUM_PHILOSOPHERS 5

typedef struct thread_data{
    pthread_mutex_t* forks;
    sem_t* semaphore;
    int id;
} thread_data;

void* philosopher_no_sync(void* data) {
    int id = ((thread_data*)data)->id;
    pthread_mutex_t* forks = ((thread_data*)data)->forks;

    while (1) {
        //printf("Philosopher %d is thinking.\n", id);
        sleep(1);

        pthread_mutex_lock(&forks[id]);
        pthread_mutex_lock(&forks[(id + 1) % NUM_PHILOSOPHERS]);

        printf("Philosopher %d is eating.\n", id);
        sleep(1);

        pthread_mutex_unlock(&forks[(id + 1) % NUM_PHILOSOPHERS]);
        pthread_mutex_unlock(&forks[id]);
        pthread_testcancel();
    }
}

void* philosopher_sync(void* data) {
    int id = ((thread_data*)data)->id;
    pthread_mutex_t* forks = ((thread_data*)data)->forks;
    sem_t* semaphore = ((thread_data*)data)->semaphore;

    while (1) {
        //printf("Philosopher %d is thinking.\n", id);
        sleep(1);
        
        sem_wait(semaphore);

        pthread_mutex_lock(&forks[id]);
        pthread_mutex_lock(&forks[(id + 1) % NUM_PHILOSOPHERS]); 

        printf("Philosopher %d is eating.\n", id);
        sleep(1);

        pthread_mutex_unlock(&forks[(id + 1) % NUM_PHILOSOPHERS]);
        pthread_mutex_unlock(&forks[id]);

        sem_post(semaphore);                         
        pthread_testcancel();
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <sync|nosync>\n", argv[0]);
        return 1;
    }

    pthread_t philosophers[NUM_PHILOSOPHERS];
    thread_data data[NUM_PHILOSOPHERS];
    pthread_mutex_t forks[NUM_PHILOSOPHERS];
    sem_t semaphore;

    if (!strcmp(argv[1], "sync")) {
        sem_init(&(semaphore), 0, (NUM_PHILOSOPHERS - 1) / 2);
    } else if (strcmp(argv[1], "nosync")) {
        printf("Invalid argument. Use 'sync' or 'nosync'.\n");
        return 1;
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&(forks[i]), NULL);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        data[i].forks = forks;
        data[i].id = i;
        data[i].semaphore = &semaphore;
        if (strcmp(argv[1], "sync") == 0) {
            pthread_create(&philosophers[i], NULL, philosopher_sync, &data[i]);
        } else {
            pthread_create(&philosophers[i], NULL, philosopher_no_sync, &data[i]);
        }
    }

    while (1) {
        char c = getchar();
        if (c == 'q' || c == 'Q') {
            break;
        }
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_cancel(philosophers[i]);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&forks[i]);
    }
    
    if (strcmp(argv[1], "sync") == 0) {
        sem_destroy(&semaphore);
    }
    //printf("all is well\n");
    return 0;
}
