#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NB_CYCLES 1000000

int PHILOSOPHES;
pthread_mutex_t *baguettes;

void* philosophe (void* arg);
void mange(int id);

void* philosophe (void* arg)
{
    int *id = (int *) arg;
    int left = *id;
    int right = (left + 1) % PHILOSOPHES;

    for (int i = 0; i < NB_CYCLES; i++) {
        // philosophe pense
        if(left<right) {
            pthread_mutex_lock(&baguettes[left]);
            pthread_mutex_lock(&baguettes[right]);
        }
        else {
            pthread_mutex_lock(&baguettes[right]);
            pthread_mutex_lock(&baguettes[left]);
        }
        // philosophe mange
        pthread_mutex_unlock(&baguettes[left]);
        pthread_mutex_unlock(&baguettes[right]);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Utilisation : %s <nb_philosophes>\n", argv[0]);
        return EXIT_FAILURE;
    }

    PHILOSOPHES = atoi(argv[1]);

    if (PHILOSOPHES < 2)
    {
        fprintf(stderr, "Le nombre de philosophes doit être supérieur ou égal à 2.\n");
        return EXIT_FAILURE;
    }

    pthread_t *philosophes = malloc(PHILOSOPHES * sizeof(pthread_t));
    baguettes = malloc(PHILOSOPHES * sizeof(pthread_mutex_t));
    int *ids = malloc(PHILOSOPHES * sizeof(int));

    if (philosophes == NULL || baguettes == NULL || ids == NULL)
    {
        perror("malloc");
        return EXIT_FAILURE;
    }

    // Init des baguettes
    for (int i = 0; i < PHILOSOPHES; i++)
        pthread_mutex_init(&baguettes[i], NULL);

    // Création des philosophes
    for (int i = 0; i < PHILOSOPHES; i++) {
        ids[i] = i;
        pthread_create(&philosophes[i], NULL, philosophe, &ids[i]);
    }

    // Attente de la fin des threads
    for (int i = 0; i < PHILOSOPHES; i++)
        pthread_join(philosophes[i], NULL);

    // Destruction des mutex
    for (int i = 0; i < PHILOSOPHES; i++)
        pthread_mutex_destroy(&baguettes[i]);

    free(baguettes);
    free(ids);
    free(philosophes);

    return EXIT_SUCCESS;
}