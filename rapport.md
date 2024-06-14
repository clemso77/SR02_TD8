# AI26 TD8 - Crible d’Eratosthenes 

## Tâche 1
 
### Déroulement de l'algorithme pour n = 20
| i |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  10 |  11 |  12 |  13 |  14 |  15 |  16 |  17 |  18 |  19 |  20 |
|----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| 2  |  V  |  ?  |  F  |  ?  |  F  |  ?  |  F  |  ?  |  F  |  ?  | F  |  ?  |  F  |  ?  |  F  |  ?  |  F  |  ?  |  F  |
| 
| 3  |  V  |  V  |  F  |  ?  |  F  |  ?  |  F  |  F  |  F  |  ?  | F  |  ?  |  F  |  F  |  F  |  ?  |  F  |  ?  |  F  |
| 
| 4  |  V  |  V  |  F  |  ?  |  F  |  ?  |  F  |  F  |  F  |  ?  | F  |  ?  |  F  |  F  |  F  |  ?  |  F  |  ?  |  F  |
| 
| 5  |  V  |  V  |  F  |  V  |  F  |  V  |  F  |  F  |  F  |  V  | F  |  V  |  F  |  F  |  F  |  V  |  F  |  V  |  F  |
| 

### Pourquoi la boucle de l’intérieure (la deuxième boucle) commence à $i^2$ et pas $0$ ou $i$ ?

Par souci d'optimisation. Lorsqu'on arrive à $i$, nous sommes forcément passés par $(i-n) i$ avec $\forall n \in [ 1, i-2 ]$ et nous n'avons pas besoin de recalculer cette valeur.

### Pourquoi la première boucle s’exécute jusqu’à √n? Que faites-vous si √n n’est pas un entier ? 

La deuxième boucle commence à $i^2$ et $sqrt(n)^2 = n$. Donc si la première boucle s'arrêtait plus loin que $sqrt(n)$, nous n'entrerions pas dans la deuxième boucle car $i^2 > n$.

