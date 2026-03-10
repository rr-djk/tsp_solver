# Rapport de speedup — Thread pool TSP

**Instance testée** : `gr431.tsp` (431 villes)  
**Algorithme** : 2-opt avec `--all-start` (431 tâches indépendantes)  
**Binaire** : compilé avec `-O2 -g`

---

# 1. Contexte

L'option `--all-start` lance l'algorithme 2-opt en utilisant chaque ville
comme point de départ. Sur l'instance `gr431`, cela génère donc **431 exécutions
indépendantes** de l'algorithme.

Comme ces exécutions ne dépendent pas les unes des autres, elles peuvent être
traitées en parallèle.

La distribution des tâches est faite avec un **compteur atomique** partagé :
chaque thread demande l'indice de la prochaine tâche avec `fetch_add`.
Cela permet d'éviter l'utilisation de mutex.

Le but de cette expérience est de mesurer **comment les performances évoluent
lorsqu'on augmente le nombre de threads**.

---

# 2. Résultats

| Threads | Temps (s) | Speedup |
|--------:|----------:|--------:|
|       1 |     19.45 |   ×1.00 |
|       2 |      9.77 |   ×1.99 |
|       4 |      6.84 |   ×2.84 |
|       6 |      5.29 |   ×3.68 |
|       8 |      4.34 |   ×4.48 |
|      12 |      4.54 |   ×4.28 |

---

# 3. Analyse

## Bon gain avec peu de threads

Avec 2 threads, le speedup est **×1.99**, ce qui est très proche du speedup
idéal (×2). Cela indique que :

- les tâches sont réellement indépendantes
- la distribution des tâches avec `fetch_add` fonctionne bien

## Diminution du gain après quelques threads

Lorsque le nombre de threads augmente, le speedup continue de progresser,
mais de moins en moins rapidement.

Deux raisons peuvent expliquer ce comportement.

**Premièrement**, une petite partie du programme reste séquentielle :
le chargement de la carte, la sélection du meilleur tour et l'écriture du
résultat en JSON.

**Deuxièmement**, tous les threads accèdent à la même matrice de distances
en mémoire. Avec beaucoup de threads, cela augmente la pression sur le cache,
ce qui peut ralentir les accès mémoire.

## Légère dégradation à 12 threads

Avec 12 threads, le temps d'exécution augmente légèrement
(4.34 s → 4.54 s).

---

# 4. Conclusion

| Observation | Valeur |
|---|---|
| Meilleur speedup observé | ×4.48 (8 threads) |
| Speedup à 2 threads | ×1.99 |
| Zone efficace | 6–8 threads |
| Effet de l'hyperthreading | peu bénéfique |

Le thread pool basé sur un **compteur atomique** permet de paralléliser
efficacement les 431 exécutions de 2-opt.

Les performances augmentent presque linéairement avec quelques threads,
puis se stabilisent lorsque l'on atteint le nombre de cœurs physiques.

Sur la machine utilisée, le **meilleur compromis se situe entre
6 et 8 threads**.
