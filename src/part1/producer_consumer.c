#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 8
#define TOTAL 131072

static int buffer[BUFFER_SIZE];

pthread_mutex_t mutex; // To manage the unique acces to the buffer
sem_t empty; // Number of available slots 
sem_t full; // Number of filled slots
int nb_prod;
int nb_cons;

static int in_index = 0;   // Next index to write
static int out_index = 0;  // Next index to read

void *producer(void* id);
void *consumer(void* id);
int quota(int id, int nb);

int main(int argc, char *argv[]) {
    // Check if we have enough argument for the number of producer and consumer
    if (argc < 3) {
        fprintf(stderr, "Missing arguments for the number of producer/consumer\n");
        return EXIT_FAILURE;
    }
    // Translate the strings we got from the agrument into integers.
    nb_prod = atoi(argv[1]);
    nb_cons = atoi(argv[2]);
    if (nb_prod <= 0 || nb_cons <= 0) {
        fprintf(stderr, "The number of threads must be positive\n");
        return EXIT_FAILURE;
    }
    // Create the mutex and init
    pthread_mutex_init(&mutex, NULL); 
    sem_init(&empty, 0, BUFFER_SIZE); 
    sem_init(&full, 0, 0);
    

    printf("Starting: %d producers, %d consumers, %d items\n",nb_prod, nb_cons, TOTAL);
    
    // Creation of the producer and consumer threads and their ids 
    pthread_t prod[nb_prod];
    pthread_t cons[nb_cons];
    int prod_id[nb_prod];
    int cons_id[nb_cons];

    for (int i = 0; i < nb_prod; i++)
    {
        prod_id[i] = i;
        pthread_create(&prod[i], NULL, producer, &prod_id[i]);
    }
    
    for (int i = 0; i < nb_cons; i++)
    {
        cons_id[i] = i;
        pthread_create(&cons[i], NULL, consumer, &cons_id[i]);
    }

    // We wait for the threads to end
    for (int i = 0; i < nb_prod; i++) pthread_join(prod[i], NULL);
    for (int i = 0; i < nb_cons; i++) pthread_join(cons[i], NULL);

    printf("Done: %d items produced and consumed\n", TOTAL);

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    return EXIT_SUCCESS;
}

void *producer(void* arg) {
    int id = *(int *)arg;
    int n = quota(id, nb_prod);
    printf("Producer %d starting, quota = %d\n", id, n);
    for (int i = 0; i < n; i++) {
        for (int i = 0; i < 10000; i++); // Simulated work

        sem_wait(&empty); // Wait for an available slot
        pthread_mutex_lock(&mutex); // Start of the critical section
        buffer[in_index] = id;
        in_index = (in_index + 1) % BUFFER_SIZE; // % BUFFER_SIZE to "cycle" on buffer
        pthread_mutex_unlock(&mutex); // End of the critical section
        sem_post(&full); // Free an available slot
    }
    printf("Producer %d finished\n", id);
    return NULL;
}

void *consumer(void* arg) {
    int id = *(int *)arg;
    int n = quota(id, nb_cons);
    printf("Consumer %d starting, quota = %d\n", id, n);
    for (int i = 0; i < n; i++) {
        sem_wait(&full); // Wait for an used slot to be freed
        pthread_mutex_lock(&mutex); // Start of the critical section
        // int item = buffer[out_index]; The consuer can rad in the critical section
        out_index = (out_index + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex); // End of the critical section
        sem_post(&empty); // Free used slot

        for (int i = 0; i < 10000; i++); // Simulated work
    }
    printf("Consumer %d finished\n", id);
    return NULL;
}

int quota(int id, int nb_thread) {
    int thread_quota = TOTAL / nb_thread;
    if (id < TOTAL % nb_thread) { 
        thread_quota++;
    }
    return thread_quota;
}

