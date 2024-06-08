#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include<unistd.h>
#include<signal.h>

char* crible;
int n = 0;
char debuter_travail = 0;
char* fin_threads;

void* thread_function(int* param) {
    int id = param[0], debut = param[1], fin = param[2];
    free(param);
    printf("Mes boules en id : %d [%d, %d]\n", id, debut, fin);
    fflush(stdout);
    while (!debuter_travail) { }
    for(int i = 2; i < (int)floor(sqrt(n)); i++) {
        if (crible[i-2]) {
            int j = (int)pow(i, 2);
            while (j-2 < debut) {
                j += i;
            }
            for(j = (int)floor(pow(i,2)); j < n+1; j += i) {
                crible[j-2] = 0;
            }
        }
    }
    fin_threads[id] = 1;
    return NULL;
}

pthread_t* create_thread(int id, int debut, int fin) {
    int* param = (int*)malloc(sizeof(int) * 3);
    param[0] = id;
    param[1] = debut;
    param[2] = fin;
    pthread_t* thread = (pthread_t*)malloc(sizeof(pthread_t));
    if (pthread_create(thread, NULL, thread_function, param) >= 0) {
        return thread;
    }
    return  NULL;
}

int main(int nargs, char* args[]) {
    if (nargs < 3) {
        printf("Veuillez indiquer un nombre de threads et un max pour le crible.\n");
        return -1;
    }

    int nb_threads = 0;
    sscanf(args[1], "%d", &nb_threads);
    sscanf(args[2], "%d", &n);

    if (nb_threads < 1) {
        printf("Veuillez indiquer un nombre positif & non nul de threads.\n");
        return -1;
    }

    pthread_t* threads[nb_threads];
    fin_threads = (char*)malloc(nb_threads);

    for (int k = 0; k < nb_threads; k++) {
        int debut = (int)floor(k*n/nb_threads + (k > 0));
        int fin = (int)floor((k + 1)*n/nb_threads);
        threads[k] = create_thread(k, debut, fin);
        if (threads[k] == NULL) {
            printf("Erreur durant la création de thread.\n");
            return -1;
        }
        fin_threads[k] = 0;
    }

    crible = (char*)malloc(n-1);
    for (int i = 0; i < n-1; i++) {
        crible[i] = 1;
    }

    debuter_travail = 1;
    char travail_fini = 0;
    while(!travail_fini) {
        travail_fini = 1;
        for(int k = 0; k < nb_threads; k++) {
            travail_fini = travail_fini && fin_threads[k];
        }
    }

    for(int k = 0; k < nb_threads; k++) {
        pthread_kill(threads[k], SIGKILL);
    }

    for (int i = 0; i < n-1; i++) {
        if (crible[i]) {
            printf("%d est un nombre premier\n", i+2);
        }
    }

    free(crible);
    free(fin_threads);

    return 0;
}