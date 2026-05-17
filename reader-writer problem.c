#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define R 4
#define W 2

int read_count = 0, write_count = 0;
int resource = 0;

sem_t resource_sem;   // controls access to resource
sem_t read_try;       // blocks readers when writer waiting
sem_t rmutex;         // protects read_count
sem_t wmutex;         // protects write_count

void *reader(void *arg) {
    int id = *(int *)arg;
    srand(time(NULL) + id);

    while (1) {
        usleep((rand()%1000) * 1000);

        sem_wait(&read_try);
        sem_wait(&rmutex);

        read_count++;
        if (read_count == 1)
            sem_wait(&resource_sem);

        sem_post(&rmutex);
        sem_post(&read_try);

        printf("[Reader %d] Reading resource: %d (Active Readers: %d)\n",
               id, resource, read_count);

        usleep((rand()%1000) * 1000);

        sem_wait(&rmutex);
        read_count--;
        if (read_count == 0)
            sem_post(&resource_sem);
        sem_post(&rmutex);

        usleep((rand()%3000) * 1000);
    }
    return NULL;
}

void *writer(void *arg) {
    int id = *(int *)arg;
    srand(time(NULL) + id + 100);

    while (1) {
        usleep((rand()%5000) * 1000);

        sem_wait(&wmutex);
        write_count++;
        if (write_count == 1)
            sem_wait(&read_try);
        sem_post(&wmutex);

        sem_wait(&resource_sem);

        resource += 5;
        printf("[Writer %d] Writing to resource: %d\n", id, resource);

        usleep((rand()%1500) * 1000);

        sem_post(&resource_sem);

        sem_wait(&wmutex);
        write_count--;
        if (write_count == 0)
            sem_post(&read_try);
        sem_post(&wmutex);
    }
    return NULL;
}

int main() {
    pthread_t r[R], w[W];
    int rid[R], wid[W];

    sem_init(&resource_sem, 0, 1);
    sem_init(&read_try, 0, 1);
    sem_init(&rmutex, 0, 1);
    sem_init(&wmutex, 0, 1);

    for (int i = 0; i < R; i++) {
        rid[i] = i + 1;
        pthread_create(&r[i], NULL, reader, &rid[i]);
    }

    for (int i = 0; i < W; i++) {
        wid[i] = i + 1;
        pthread_create(&w[i], NULL, writer, &wid[i]);
    }

    for (int i = 0; i < R; i++) pthread_join(r[i], NULL);
    for (int i = 0; i < W; i++) pthread_join(w[i], NULL);

    return 0;
}