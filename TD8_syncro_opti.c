#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define K 16
#define N 201


int main(int argc, char *argv[]) {
	int max=N/2;
	if(N%2==1){
		max++;
	}
	int liste[max];
	liste[0] = 2;
	int nb = ceil(sqrt(N));
	for(int i=1; i<max; i++){
		liste[i]=(1) + i*2;
	}
	for(int i=3; i<=nb; i+=2){
		//si on doit parcourir ka cellule
		if(liste[i-2]!=-1){
			//on retire tous les non premiers
			for(int j=i*i; j<=N; j+=i*2){
				liste[j/2] = -1;
			}
		}
	}
	for(int i=0; i<max; i++){
		if(liste[i] != -1){
			printf("Nombre premier: %d\n", liste[i]);
		}
	}
    return EXIT_SUCCESS;
}

