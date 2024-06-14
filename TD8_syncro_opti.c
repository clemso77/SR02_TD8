#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define NOMBRE_PREMIER 4000000


int main(int argc, char *argv[]) {
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
    printf("Nombre premier: %u\n", 2);
    for (int i = 1; i < max; i++) {
        if (liste[i] != 1) {
            printf("Nombre premier: %u\n", i*2 +1);
        }
    }
    free(liste);
    return EXIT_SUCCESS;
}

