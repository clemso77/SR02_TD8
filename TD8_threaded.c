#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define NOMBRE_THREAD 7
#define NOMBRE_PREMIER 4000000

typedef struct param_thread {
    int i; // l'offset
    unsigned int *tab_addr; // le tableau des chiffres
    int iteration; // la cellule à parcourir
} param_thread;

void *thread_action(void *p);
void create_threads(param_thread *param, pthread_t *threads);
int V(int num);
int P(int num);
int detruire_semaphore();
int init_semaphore();

static int sem_id = -1; // Identifiant du groupe de sémaphores

int main(int argc, char *argv[]) {
    param_thread *params = (param_thread *)malloc(sizeof(param_thread) * NOMBRE_THREAD);
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * NOMBRE_THREAD);
    if (params == NULL || threads == NULL) {
        perror("malloc");
        free(params);
        free(threads);
        return EXIT_FAILURE;
    }

    unsigned int *liste = (unsigned int *)malloc(sizeof(unsigned int) * (NOMBRE_PREMIER - 2));
    if (liste == NULL) {
        perror("malloc");
        free(params);
        free(threads);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < NOMBRE_PREMIER - 2; i++) {
        liste[i] = i + 2;
    }

    for (int i = 0; i < NOMBRE_THREAD; i++) {
        params[i].tab_addr = liste;
        params[i].i = i;
        params[i].iteration = 0;
    }

    int sigma = init_semaphore();
    if (sigma != 0) {
        perror("init_semaphore");
        free(params);
        free(threads);
        free(liste);
        return EXIT_FAILURE;
    }

    create_threads(params, threads);

    int nb = ceil(sqrt(NOMBRE_PREMIER));

    for (int i = 2; i <= nb; i++) {
        if (liste[i - 2] != -1) {
            for (int k = 0; k < NOMBRE_THREAD; k++) {
                params[k].iteration = i;
                V(k);
            }
            for (int k = 0; k < NOMBRE_THREAD; k++) {
                P(NOMBRE_THREAD);
            }
        }
    }
    for (int i = 0; i < NOMBRE_THREAD; i++) {
        params[i].iteration = -1;
        V(i);
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < NOMBRE_PREMIER - 2; i++) {
        if (liste[i] != -1) {
            printf("Nombre premier: %u\n", liste[i]);
        }
    }

    detruire_semaphore();
    free(threads);
    free(params);
    free(liste);
    return EXIT_SUCCESS;
}


// on calcule les cellules associées à notre thread
void *thread_action(void *p) {
    param_thread *param = (param_thread *)p;
    while (1) {
        P(param->i);
        if (param->iteration == -1)
            break;
        for (int i = param->iteration * param->iteration + param->i * param->iteration; i < NOMBRE_PREMIER; i += param->iteration * NOMBRE_THREAD) {
            param->tab_addr[i - 2] = -1;
        }
        V(NOMBRE_THREAD);
    }
    pthread_exit(NULL);
}

void create_threads(param_thread *param, pthread_t *threads) {
    for (int i = 0; i < NOMBRE_THREAD; i++) {
        if (pthread_create(&threads[i], NULL, thread_action, (void *)&param[i])) {
            perror("Erreur création thread");
            exit(1);
        }
    }
}

// Fonction pour initialiser le groupe de sémaphores
int init_semaphore(void) {
    if (sem_id != -1) {
        return -1;
    }
    // Générer une clé unique pour les sémaphores
    key_t key = ftok("TD8_threaded.c", 'J');
    if (key == -1) {
        perror("ftok");
        return -2;
    }
    // Créer le groupe de sémaphores
    sem_id = semget(key, (NOMBRE_THREAD + 1), 0666 | IPC_CREAT | IPC_EXCL);
    if (sem_id == -1) {
        perror("semget");
        return -2;
    }
    // Initialiser tous les sémaphores à 0
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
        struct seminfo *__buf;
    } arg;
    arg.val = 0;
    for (int i = 0; i < (NOMBRE_THREAD + 1); i++) {
        if (semctl(sem_id, i, SETVAL, arg) == -1) {
            perror("semctl");
            // Si l'initialisation échoue, supprimer le groupe de sémaphores
            semctl(sem_id, 0, IPC_RMID);
            return -2;
        }
    }
    return 0;
}

// Fonction pour détruire le groupe de sémaphores
int detruire_semaphore(void) {
    if (sem_id == -1) {
        return -1;
    }
    int ret = semctl(sem_id, 0, IPC_RMID);
    if (ret == -1) {
        perror("semctl");
    } else {
        sem_id = -1; // Réinitialiser l'identifiant du sémaphore
    }
    return ret;
}

// Fonction pour réaliser l'opération P (wait) sur un sémaphore spécifique
int P(int sem) {
    if (sem_id == -1) {
        return -1;
    }
    if (sem < 0 || sem >= NOMBRE_THREAD + 1) {
        return -2;
    }
    struct sembuf op;
    op.sem_num = sem;
    op.sem_op = -1;
    op.sem_flg = 0;
    int ret = semop(sem_id, &op, 1);
    if (ret == -1) {
        perror("semop");
    }
    return ret;
}

// Fonction pour réaliser l'opération V (signal) sur un sémaphore spécifique
int V(int sem) {
    if (sem_id == -1) {
        return -1;
    }
    if (sem < 0 || sem >= NOMBRE_THREAD + 1) {
        return -2;
    }
    struct sembuf op;
    op.sem_num = sem;
    op.sem_op = 1;
    op.sem_flg = 0;

    int ret = semop(sem_id, &op, 1);
    if (ret == -1) {
        perror("semop");
    }
    return ret;
}
