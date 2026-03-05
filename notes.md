# Notes - Compréhension du TSP

## Objectif du projet
On veut déterminer le **chemin le plus court** qui relie tous les points dans un plan (Traveling Salesman Problem).

## Format des données TSPLIB

Les fichiers `.tsp` dans le répertoire `data/` suivent le format TSPLIB :

### Structure d'un fichier .tsp
```
NAME: berlin52
TYPE: TSP
COMMENT: 52 locations in Berlin (Groetschel)
DIMENSION: 52
EDGE_WEIGHT_TYPE: EUC_2D
NODE_COORD_SECTION
1 565.0 575.0
2 25.0 185.0
3 345.0 750.0
...
EOF
```

### Format des coordonnées
Après la ligne `NODE_COORD_SECTION`, chaque ville est représentée par :
```
num_ville coordX coordY
```
- Séparateur : **espaces** (pas de virgules)
- Exemple : `1 565.0 575.0` = ville n°1 aux coordonnées (565.0, 575.0)

### Métadonnées importantes
- `DIMENSION` : nombre de villes dans le problème
- `EDGE_WEIGHT_TYPE: EUC_2D` : la distance est calculée avec la distance euclidienne (formule : √[(x₂-x₁)² + (y₂-y₁)²])
- `EOF` : marque la fin du fichier

---

## Fichiers disponibles
- `berlin52.tsp` : 52 villes à Berlin
- `ch150.tsp` : 150 villes en Suisse
- Et d'autres instances de tailles variées (127 à 85900 villes)

## `Map` vs `Tour` : rôles et relation

### `Map` - le problème (données statiques)

`Map` représente **l'ensemble des villes et leur géographie**. Elle est chargée une seule fois depuis un fichier `.tsp` et ne change jamais pendant l'exécution.

- Contient un `vector<City>`, chaque `City` ayant un `id` TSPLIB et des coordonnées `(x, y)`.
- Les villes sont indexées **par position dans le vecteur** (`0..n-1`), indépendamment de leur `id` TSPLIB (qui commence à 1).
- Expose `distance(i, j)` pour calculer la distance euclidienne entre deux villes via leur index interne.
- Se remplit via `operator>>` (lecture d'un flux `.tsp`).

> `Map` = la carte du monde. Elle ne sait rien des chemins qu'on va emprunter.

### `Tour` - une solution (données variables)

`Tour` représente **un ordre de visite des villes**, c'est-à-dire une solution candidate au problème TSP.

- Contient un `vector<int>` d'**indices internes** (`0..n-1`), **pas** les ids TSPLIB.
- L'ordre est une permutation : chaque ville apparaît exactement une fois.
- Expose `is_valid(n)` pour vérifier que la permutation est cohérente avec une `Map` de taille `n`.
- Ne contient aucune coordonnée ni distance : il ne sait pas ce que représentent les indices, seulement leur ordre.

> `Tour` = un itinéraire sur la carte. Il ne connaît pas la carte, seulement l'ordre de passage.

### Relation entre `Map` et `Tour`

Les deux classes sont **volontairement découplées** : `Tour` ne dépend pas de `Map`. C'est la fonction libre `cost(map, tour)` (dans `evaluator`) qui les réunit :

```
cost(map, tour)
  ├── vérifie que tour.is_valid(map.size())
  └── somme map.distance(ord[i], ord[i+1]) pour i in 0..n-1
       + retour implicite : map.distance(ord[n-1], ord[0])
```

Cette séparation permet de :
- Générer/modifier des `Tour` sans toucher à la `Map`
- Évaluer n'importe quel `Tour` sur n'importe quelle `Map` de même taille
- Tester `Tour` et `Map` indépendamment

**Résumé :** `Map` contient *où sont* les villes, `Tour` contient *dans quel ordre* on les visite, et `cost()` mesure la longueur totale du chemin en combinant les deux.

---

## Algorithmes implémentés

### 1. Nearest Neighbor (NN)

On part d'une ville de départ (configurable via `start_city_id`, sinon index 0).
À chaque étape, on rejoint la ville non visitée la plus proche de la ville courante.
On répète jusqu'à avoir visité toutes les villes.

### 2. 2-opt

On part d'un tour existant (généré par NN).
On cherche deux arêtes dont l'échange réduit le coût total.
On répète jusqu'à ce qu'aucun échange ne soit améliorant.

### 3. Cheapest Insertion

On commence avec un cycle initial à deux villes.
À chaque étape, on insère la ville restante dont l'ajout augmente le moins le coût total.
On répète jusqu'à inclure toutes les villes.

---

## Paramètres CLI

### Obligatoires

| Option | Description |
|---|---|
| `--input-file <path>` | Fichier `.tsp` TSPLIB à charger |
| `--algo <name>` | Algorithme à exécuter (`nn`, `2opt`, `insertion`, …) |
| `--output-file <path>` | Fichier JSON de sortie |

### Optionnels

| Option | Description | Contraintes |
|---|---|---|
| `--all-start` | Exécute depuis chaque ville de départ | (flag, pas de valeur) |
| `--quiet` | Affichage minimal | (flag, pas de valeur) |
| `--repeat <k>` | Répète k fois | `k >= 1`, entier |
| `--threads <t>` | Nombre de threads | `1 ≤ t ≤ hardware_concurrency()` |
| `--time-limit <Ns>` | Limite de temps | entier > 0 + `s` minuscule, ex : `10s` |

### Règles de validation

- Option inconnue → erreur
- Option dupliquée → erreur
- Valeur attendue mais absente → erreur
- Valeur hors contrainte → erreur

Dans tous les cas : message descriptif affiché, exécution arrêtée immédiatement.
