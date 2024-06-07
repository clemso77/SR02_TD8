#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define K 16
#define N 200


int main(int argc, char *argv[]) {
	int liste[N-2];
	int nb = ceil(sqrt(N));
	for(int i=0; i<N-1; i++){
		liste[i]=i+2;
	}
	for(int i=2; i<=nb; i++){
		//si on doit parcourir ka cellule
		if(liste[i-2]!=-1){
			//on retire tous les non premiers
			for(int j=i*i; j<=N; j+=i){
				liste[j-2] = -1;
			}
		}
	}
	for(int i=0; i<N-1; i++){
		if(liste[i] != -1){
			printf("Nombre premier: %d\n", liste[i]);
		}
	}
    return EXIT_SUCCESS;
}

