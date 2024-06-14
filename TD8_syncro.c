#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define NOMBRE_PREMIER 200

int main(int argc, char *argv[]) {
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
	for(int i=2; i<=NOMBRE_PREMIER; i++){
		if(liste[i-2] != 1){
			printf("Nombre premier: %d\n", i);
		}
	}
	free(liste);
    return EXIT_SUCCESS;
}

