#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include<sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h> 

#define NOMBRE_THREAD 5
#define NOMBRE_PREMIER 4000000
#define REPETITION 100
#define N_SEM NOMBRE_THREAD 

struct param_thread {
    int i; // l'offset
    int *tab_addr; // le tableau des chiffres
    int iteration; // la cellule à parcourir
} typedef param_thread;

void *thread_action(void *p);
void *thread_action_opti(void *p);
void creat_thread_opti(param_thread *param, pthread_t *threads);
int thread();
int thread_opti();
int syncro_opti();
void create_threads(param_thread *param, pthread_t *threads);
int V();
int P();
int detruire_semaphore();
int init_semaphore();

static int sem_id = -1; // Identifiant du groupe de sémaphores


int main(int argc, char *argv[]) {
    FILE *f = fopen("out.csv", "w");
        for (int j = 0; j < REPETITION; j++) {
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
			syncro_opti();
            clock_gettime(CLOCK_MONOTONIC, &end);
            long double duree = (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_nsec - start.tv_nsec) / 1e3;
            fprintf(f, "%.0Lf ", duree);
            fprintf(f, "\n");
        }
    fclose(f);
    return EXIT_SUCCESS;
}

int syncro() {
    unsigned int *liste = (unsigned int *)malloc(sizeof(unsigned int) * (NOMBRE_PREMIER - 2));
    if (liste == NULL) {
        perror("malloc");
        return EXIT_FAILURE;
    }
    int nb = ceil(sqrt(NOMBRE_PREMIER));
    for (int i = 0; i < NOMBRE_PREMIER - 2; i++) {
        liste[i] = i + 2;
    }
    for (int i = 2; i <= nb; i++) {
        if (liste[i - 2] != -1) {
            for (int j = i * i; j < NOMBRE_PREMIER; j += i) {
                liste[j - 2] = -1;
            }
        }
    }
    free(liste);
    return EXIT_SUCCESS;
}




void *thread_action_opti(void *p) {
    param_thread *param = (param_thread *)p;
    while (1) {
        P(param->i/2);
        if (param->iteration == -1)
            break;
        for (int i = param->iteration * param->iteration + param->i * param->iteration; i <= NOMBRE_PREMIER; i += param->iteration * NOMBRE_THREAD * 2) {
            param->tab_addr[i / 2] = -1;
        }
        V(NOMBRE_THREAD);
    }
    pthread_exit(NULL);
}

void create_threads_opti(param_thread *param, pthread_t *threads) {
    for (int i = 0; i < NOMBRE_THREAD; i++) {
        if (pthread_create(&threads[i], NULL, thread_action_opti, (void *)&param[i])) {
            perror("Erreur création thread");
            exit(1);
        }
    }
}

int thread_opti() {
    int max = NOMBRE_PREMIER / 2;
    if (NOMBRE_PREMIER % 2 == 1) {
        max++;
    }
    param_thread *params = (param_thread *)malloc(sizeof(param_thread) * NOMBRE_THREAD);
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * NOMBRE_THREAD);
    if (params == NULL || threads == NULL) {
        perror("malloc");
        free(params);
        free(threads);
        return EXIT_FAILURE;
    }
    unsigned int *liste = (unsigned int *)malloc(sizeof(unsigned int) * (max));
    if (liste == NULL) {
        perror("malloc");
        free(params);
        free(threads);
        return EXIT_FAILURE;
    }
    liste[0] = 2;
    int nb = ceil(sqrt(NOMBRE_PREMIER));
    for (int i = 1; i < max; i++) {
        liste[i] = 1 + i * 2;
    }
     
    for (int i = 0; i < NOMBRE_THREAD; i++) {
        params[i].tab_addr = liste;
        params[i].i = i*2;
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
    
    create_threads_opti(params, threads);
    
    for (int i = 3; i <= nb; i += 2) {
        if (liste[i / 2] != -1) {
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
    detruire_semaphore();
    free(threads);
    free(params);
    free(liste);
    return EXIT_SUCCESS;
}

int syncro_opti() {
    int max = NOMBRE_PREMIER / 2;
    if (NOMBRE_PREMIER % 2 == 1) {
        max++;
    }
    unsigned int *liste = (unsigned int *)malloc(sizeof(unsigned int) * (max));
    liste[0] = 2;
    int nb = ceil(sqrt(NOMBRE_PREMIER));
    for (int i = 1; i < max; i++) {
        liste[i] = 1 + i * 2;
    }
    for (int i = 3; i <= nb; i += 2) {
        if (liste[i / 2] != -1) {
            for (int j = i * i; j <= NOMBRE_PREMIER; j += i * 2) {
                liste[j / 2] = -1;
            }
        }
    }
    free(liste);
    return EXIT_SUCCESS;
}


int thread() {
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
    key_t key = ftok("comparaison_algo.c", 'J');
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
