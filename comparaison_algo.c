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

#define NOMBRE_THREAD 7
#define NOMBRE_PREMIER 2000000
#define REPETITION 100
#define N_SEM NOMBRE_THREAD 

struct param_thread {
    int i; // l'offset
    unsigned short *tab_addr; // le tableau des chiffres
    int iteration; // la cellule à parcourir
} typedef param_thread;

char travailler = 0;

void *thread_action(void *p);
void *thread_action_opti(void *p);
void creat_thread_opti(param_thread *param, pthread_t *threads);
void create_threads(param_thread *param, pthread_t *threads);

int thread();
int thread_opti();
int syncro_opti();
int syncro();

int main(int argc, char *argv[]) {
    FILE *f = fopen("out.csv", "w");
    fprintf(f, "data\n");
        for (int j = 0; j < REPETITION; j++) {
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
			thread_opti();
            clock_gettime(CLOCK_MONOTONIC, &end);
            long double duree = (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_nsec - start.tv_nsec) / 1e3;
            fprintf(f, "%.0Lf ", duree);
            fprintf(f, "\n");
        }
    fclose(f);
    return EXIT_SUCCESS;
}

int syncro() {
    unsigned short *liste = (unsigned short* )malloc(sizeof(unsigned short)*(NOMBRE_PREMIER-1));
	 if (liste == NULL) {
        perror("malloc liste error");
        return EXIT_FAILURE;
    }
	int nb = floor(sqrt(NOMBRE_PREMIER));
	for(int i=2; i<=nb; i++){
		//si on doit parcourir la cellule
		if(liste[i-2]!=1){
			//on retire tous les non premiers
			for(int j=i*i; j<=NOMBRE_PREMIER; j+=i){
				liste[j-2] = 1;
			}
		}
	}
	free(liste);
    return EXIT_SUCCESS;
}

int syncro_opti() {
   int max = NOMBRE_PREMIER / 2;
    if (NOMBRE_PREMIER % 2 == 1) {
        max++;
    }
    unsigned short *liste = (unsigned short *)malloc(sizeof(unsigned short) * (max));
     if (liste == NULL) {
        perror("malloc liste error");
        return EXIT_FAILURE;
    }
    liste[0] = 2;
    int nb = floor(sqrt(NOMBRE_PREMIER));
    for (int i = 3; i <= nb; i += 2) {
        if (liste[i / 2] != 1) {
            for (int j = i * i; j <= NOMBRE_PREMIER; j += i * 2) {
                liste[j / 2] = 1;
            }
        }
    }
    free(liste);
    return EXIT_SUCCESS;
}


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

int thread_opti() {
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
    travailler=0;
    free(threads);
    free(params);
    free(liste);
    return EXIT_SUCCESS;
}

int thread() {
    param_thread *params = (param_thread *)malloc(sizeof(param_thread) * NOMBRE_THREAD);
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * NOMBRE_THREAD);
    if (params == NULL || threads == NULL) {
        perror("malloc threads || params error");
        free(params);
        free(threads);
        return EXIT_FAILURE;
    }
    unsigned short *liste = (unsigned short *)malloc(sizeof(unsigned short) * (NOMBRE_PREMIER - 1));
     if (liste == NULL) {
        perror("malloc liste error");
        free(params);
        free(threads);
        return EXIT_FAILURE;
    }
    int nb = floor(sqrt(NOMBRE_PREMIER));
    create_threads(params, threads);
    for (int i = 0; i < NOMBRE_THREAD; i++) {
        params[i].tab_addr = liste;
        params[i].i = i;
        params[i].iteration = nb;
    }
    travailler=1;
    for (int i = 0; i < NOMBRE_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }
    travailler=0;
    free(threads);
    free(params);
    free(liste);
    return EXIT_SUCCESS;
}


// on calcule les cellules associées à notre thread
void *thread_action(void *p) {
    param_thread *param = (param_thread *)p;
    while(!travailler){}
    for(long long int i=2; i<=param->iteration; i++){
		//si on doit parcourir la cellule
		if(param->tab_addr[i-2]!=1){
			//on retire tous les non premiers
			for(int j=i*i + param->i * i; j<=NOMBRE_PREMIER; j+=i * NOMBRE_THREAD){
				param->tab_addr[j-2]=1;
			}
		}
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