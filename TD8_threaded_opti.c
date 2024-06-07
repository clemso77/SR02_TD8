#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>

#define K 16
#define N 201


struct param_thread{
	int i;	//l'offset
	int* tab_addr; //le tableau des chiffres
	int	iteration; //la cellule à parcourir
}typedef param_thread;


//on calcules les cellules associé à notre thread
void *thread_action(void *p) {
	//mon param
    param_thread *param = (param_thread*)p;
	//Pour i^2 + notre offset < N avec un pas de n*nombre de thread
	for(int i=param->iteration*param->iteration+param->i*param->iteration; i<=N; i+=param->iteration*K*2){
		param->tab_addr[i/2]=-1;
	}
    pthread_exit((void *)0);
}

void creat_thread(int iteration, param_thread *param) {
	pthread_t thread[K];
	//on créer nos K thread et on en profite pour initialiser la cellule à parcourir
    for (int i = 0; i < K; i++) {
		param[i].iteration = iteration;
        if (pthread_create(&thread[i], NULL, thread_action, (void *)&param[i])) {
            perror("Erreur création thread");
            exit(1);
        }
    }
	//on attend la fin avant de chercher la prochaine cellule
    for (int i = 0; i < K; i++) {
        pthread_join(thread[i], NULL);
    }
}

int main(int argc, char *argv[]) {
	int max= N/2;
	if(N%2 == 1)
		max++;
	int liste[max];
	liste[0]=2;
	//initialisation du tableau
	for(int i=1; i<max; i++){
		liste[i]=1+i*2;
	}
	param_thread *p=(param_thread*)malloc(sizeof(param_thread)*K);
	//initialisation des paramètre de nos threads
	for(int i=0; i<K; i++){
		p[i].tab_addr = liste;
		p[i].i = i*2;
	}
	int nb = ceil(sqrt(N));
	for(int i=3; i<=nb; i+=2){
		//si on doit parcourir ka cellule
		if(liste[i-2]!=-1){
			//on lance nos thread pour le faire
    		creat_thread(i, p);
		}
	}
	for(int i=0; i<max; i++){
		if(liste[i] != -1){
			printf("Nombre premier: %d\n", liste[i]);
		}
	}
	free(p);
    return EXIT_SUCCESS;
}

