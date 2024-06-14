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
#define NOMBRE_PREMIER 200

typedef struct param_thread {
    int i; // l'offset
    unsigned short *tab_addr; // le tableau des chiffres
    int iteration; // la cellule à parcourir
} param_thread;

char travailler = 0;

void *thread_action(void *p);
void create_threads(param_thread *param, pthread_t *threads);

int main(int argc, char *argv[]) {
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
	for(int i=2; i<=NOMBRE_PREMIER; i++){
		if(liste[i-2] != 1){
			printf("Nombre premier: %d\n", i);
		}
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