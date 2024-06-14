#!/bin/bash

# Compilation du programme
gcc comparaison_algo.c -lpthread -lm

# Nombre maximum de threads
MAX_THREADS=7

# Boucle pour exécuter le programme avec 1 à 7 threads
for i in $(seq 1 $MAX_THREADS); do
    # Modifier le fichier source pour changer le nombre de threads
    sed -i "s/^#define NOMBRE_THREAD .*/#define NOMBRE_THREAD $i/" comparaison_algo.c

    # Recompiler le programme avec le nouveau nombre de threads
    gcc comparaison_algo.c

    # Exécuter le programme et rediriger la sortie vers le fichier correspondant
    ./a.out
	mv out.csv "thread_${i}.cvs"
done

