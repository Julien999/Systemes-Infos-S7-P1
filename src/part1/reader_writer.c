#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define TOTAL_READ_COUNT 25400
#define TOTAL_WRITE_COUNT 6400

sem_t reader_sem;
sem_t writer_sem;

pthread_mutex_t mutex_readcount = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_writecount = PTHREAD_MUTEX_INITIALIZER;

int readcount = 0;
int writecount = 0;

int *reads_per_reader;
int *writes_per_writer;

/* Simulation d'un accès à la base de données */
void db_access()
{
    for (int i = 0; i < 10000; i++);
}

void *reader(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < reads_per_reader[id]; i++)
    {
        sem_wait(&reader_sem);
        pthread_mutex_lock(&mutex_readcount);
        readcount++;
        if (readcount == 1)
            sem_wait(&writer_sem);
        pthread_mutex_unlock(&mutex_readcount);
        sem_post(&reader_sem);

        // Lecture dans la section critique
        db_access();

        pthread_mutex_lock(&mutex_readcount);
        readcount--;
        if (readcount == 0)
            sem_post(&writer_sem);
        pthread_mutex_unlock(&mutex_readcount);
    }

    return NULL;
}

void *writer(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < writes_per_writer[id]; i++)
    {
        pthread_mutex_lock(&mutex_writecount);
        writecount++;
        if (writecount == 1)
            sem_wait(&reader_sem);
        pthread_mutex_unlock(&mutex_writecount);

        sem_wait(&writer_sem);
        // Écriture dans la section critique
        db_access();
        sem_post(&writer_sem);

        pthread_mutex_lock(&mutex_writecount);
        writecount--;
        if (writecount == 0)
            sem_post(&reader_sem);
        pthread_mutex_unlock(&mutex_writecount);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Utilisation : %s <nb_lecteurs> <nb_ecrivains>\n", argv[0]);
        return 1;
    }

    int reader_count = atoi(argv[1]);
    int writer_count = atoi(argv[2]);

    if (reader_count <= 0 || writer_count <= 0)
    {
        printf("Les nombres de threads doivent être positifs.\n");
        return 1;
    }

    // Nombre de lectures/écritures à faire pour chaque lecteur/écrivain
    reads_per_reader = malloc(reader_count * sizeof(int));
    writes_per_writer = malloc(writer_count * sizeof(int));

    if (reads_per_reader == NULL || writes_per_writer == NULL)
    {
        perror("malloc");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < reader_count; i++)
    {
        reads_per_reader[i] = TOTAL_READ_COUNT / reader_count;

        if (i < TOTAL_READ_COUNT % reader_count)
            reads_per_reader[i]++;
    }

    for (int i = 0; i < writer_count; i++)
    {
        writes_per_writer[i] = TOTAL_WRITE_COUNT / writer_count;

        if (i < TOTAL_WRITE_COUNT % writer_count)
            writes_per_writer[i]++;
    }

    int reader_ids[reader_count];
    int writer_ids[writer_count];

    pthread_t lecteurs[reader_count];
    pthread_t ecrivains[writer_count];

    sem_init(&reader_sem, 0, 1);
    sem_init(&writer_sem, 0, 1);


    // Création des lecteurs
    for (int i = 0; i < reader_count; i++)
    {
        reader_ids[i] = i;
        pthread_create(&lecteurs[i], NULL, reader, &reader_ids[i]);
    }

    // Création des écrivains
    for (int i = 0; i < writer_count; i++)
    {
        writer_ids[i] = i;
        pthread_create(&ecrivains[i], NULL, writer, &writer_ids[i]);
    }

    // Attente des lecteurs
    for (int i = 0; i < reader_count; i++)
    {
        pthread_join(lecteurs[i], NULL);
    }

    // Attente des écrivains
    for (int i = 0; i < writer_count; i++)
    {
        pthread_join(ecrivains[i], NULL);
    }

    sem_destroy(&reader_sem);
    sem_destroy(&writer_sem);

    pthread_mutex_destroy(&mutex_readcount);
    pthread_mutex_destroy(&mutex_writecount);

    free(reads_per_reader);
    free(writes_per_writer);

    printf("Toutes les lectures et écritures sont terminées.\n");

    return 0;
}