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
#define NOMBRE_THREAD 6
#define NOMBRE_PREMIER 100


struct param_thread{
	int i;	//l'offset
	unsigned short* tab_addr; //le tableau des chiffres
	int iteration; //la cellule à parcourir
}typedef param_thread;

char travailler = 0;

void *thread_action_opti(void *p) {
    param_thread *param = (param_thread *)p;
    while (!travailler) {}
    for (long long int i = 3; i <= param->iteration; i += 2) {
        if (param->tab_addr[i / 2] != 1) {
            for (int j = i * i + i*param->i ; j <= NOMBRE_PREMIER; j += i * 2 *NOMBRE_THREAD) {
                param->tab_addr[j / 2] = 1;
            }
        }
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

int main(int argc, char *argv[]) {
    int max = NOMBRE_PREMIER / 2;
    if (NOMBRE_PREMIER % 2 == 1) {
        max++;
    }
    param_thread *params = (param_thread *)malloc(sizeof(param_thread) * NOMBRE_THREAD);
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * NOMBRE_THREAD);
    if (params == NULL || threads == NULL) {
        perror("malloc threads || params error");
        free(params);
        free(threads);
        return EXIT_FAILURE;
    }
    unsigned short *liste = (unsigned short *)malloc(sizeof(unsigned short) * (max));
    if (liste == NULL) {
        perror("malloc liste error");
        free(params);
        free(threads);
        return EXIT_FAILURE;
    }
    liste[0] = 2;
    int nb = floor(sqrt(NOMBRE_PREMIER));
     
    create_threads_opti(params, threads);
    for (int i = 0; i < NOMBRE_THREAD; i++) {
        params[i].tab_addr = liste;
        params[i].i = i*2;
        params[i].iteration = nb;
    }
    travailler = 1;
    for (int i = 0; i < NOMBRE_THREAD; i++){
        pthread_join(threads[i], NULL);
    }
    printf("Nombre premier: %u\n", 2);
    for (int i = 1; i < max; i++) {
        if (liste[i] != 1) {
            printf("Nombre premier: %u\n", i*2 +1);
        }
    }
    travailler=0;
    free(threads);
    free(params);
    free(liste);
    return EXIT_SUCCESS;
}