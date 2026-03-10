# Rapport de profiling --- TSP Solver

**Instance testée** : `gr431.tsp` (431 villes)\
**Algorithme** : 2-opt avec `--all-start`

------------------------------------------------------------------------

# 1. Problème observé

Quand j'exécutais l'algorithme **2-opt** sur l'instance `gr431`, le
programme prenait **plus de 3 minutes** à terminer.

Comme l'algorithme 2-opt est connu pour être **O(n²)** par passe, je
voulais comprendre **où le programme passait réellement son temps** :

-   dans les boucles de l'algorithme ?
-   dans les calculs de distance ?
-   ailleurs ?

Pour répondre à cette question, j'ai utilisé **l'outil Linux `perf`**.

------------------------------------------------------------------------

# 2. Mesure globale avec `perf stat`

J'ai compilé le programme avec :

    -O2 -g

Cela active les optimisations tout en gardant les symboles nécessaires
pour profiler.

Ensuite j'ai exécuté :

``` bash
perf stat ./build/tsp_parser_profile \
  --input-file data/gr431.tsp --algo 2opt --all-start \
  --quiet --output-file /dev/null
```

Résultat principal :

-   **197 secondes** d'exécution
-   beaucoup de cycles CPU utilisés
-   **IPC ≈ 3.19**

À ce moment-là, je savais que le programme était lent, mais **je ne
savais toujours pas quelle partie du code causait le problème**.

------------------------------------------------------------------------

# 3. Trouver la fonction la plus lente (`perf record`)

Pour identifier **quelle fonction prenait le plus de temps**, j'ai
utilisé :

``` bash
perf record -e cpu_core/cycles/ -g ./build/tsp_parser_profile ...
perf report --stdio --no-children
```

Résultat :

    74.51%  libm.so.6  hypot

Cela signifie que **74 % du temps total était passé dans la fonction
`hypot`**.

`hypot` est une fonction de la bibliothèque mathématique qui calcule :

    sqrt(dx² + dy²)

Autrement dit : **la distance euclidienne entre deux villes**.

En regardant la chaîne d'appels :

    2-opt loop
      → compute_2opt_delta
        → Map::distance
          → hypot

J'ai réalisé que **la distance entre les villes était recalculée à
chaque fois**, même si elle avait déjà été calculée auparavant.

Avec **431 villes**, cela produit énormément d'appels à `hypot`.

------------------------------------------------------------------------

# 4. Solution appliquée

Au lieu de recalculer les distances à chaque fois, j'ai décidé de :

1.  **calculer toutes les distances une seule fois au début**
2.  les stocker dans une **matrice de distances**
3.  ensuite simplement **lire la valeur dans le tableau**

Exemple simplifié :

### Avant

``` cpp
double Map::distance(size_t i, size_t j) const {
    return cities[i].distanceTo(cities[j]);
}
```

Chaque appel recalculait `hypot`.

------------------------------------------------------------------------

### Après

``` cpp
double Map::distance(size_t i, size_t j) const {
    return dist_matrix_[i * cities.size() + j];
}
```

La distance est simplement **lue dans un tableau**.

Pour `gr431`, la matrice prend environ :

    431 × 431 × 8 bytes ≈ 1.5 MB

Ce qui tient facilement en mémoire.

------------------------------------------------------------------------

# 5. Problème rencontré pendant l'implémentation

Après avoir modifié `map.hpp`, j'ai lancé :

    make test

Mais j'ai obtenu un **segfault**.

La raison : certains fichiers `.o` avaient été compilés avec
**l'ancienne version de la classe `Map`**.

La solution :

``` bash
make clean
make test
```

Après recompilation complète, le problème a disparu.

------------------------------------------------------------------------

# 6. Résultats après optimisation

Même test avec `perf stat`.

| Mesure | Avant | Après |
|---|---|---|
| Temps total | 197 s | 32 s |
| Instructions | 1415 G | 365 G |
| IPC | 3.19 | 4.93 |

Le programme est devenu **plus de 6 fois plus rapide**.

------------------------------------------------------------------------

# 7. Note sur l’IPC

Dans les résultats de `perf stat`, on observe aussi la métrique **IPC (Instructions Per Cycle)**.

L’IPC indique **combien d’instructions le processeur exécute en moyenne à chaque cycle d’horloge**.  
Plus cette valeur est élevée, plus le processeur est utilisé efficacement.

Dans mon cas :

| Avant | Après |
|---|---|
| 3.19 | 4.93 |

Après l’optimisation, l’IPC augmente significativement. Cela signifie que le processeur passe moins de temps à attendre et exécute plus d’instructions utiles à chaque cycle.

Cela s’explique par la suppression des appels répétés à `hypot`, qui introduisaient plusieurs vérifications internes et branches. En remplaçant ces calculs par une simple lecture dans la matrice de distances, le pipeline du processeur est mieux utilisé.

------------------------------------------------------------------------

# 8. Conclusion

Au départ, je pensais que le problème venait de l'algorithme 2-opt
lui-même.

Mais le profiling a montré que **le vrai problème était le calcul répété
des distances**.

Après optimisation :

-   le code est **6× plus rapide**
-   l'algorithme reste le même
-   seule la gestion des distances a changé

Cette expérience montre l'importance de **mesurer avant d'optimiser**.
