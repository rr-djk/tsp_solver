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

## Paramètres CLI

```
- `--input_file`
  Fichier `.tsp` à charger (instance TSPLIB contenant les coordonnées des villes).

- `--output_file`
  Fichier JSON où sont exportées les métriques (coût, temps, statistiques, etc.).

- `--algo`
  Algorithme à exécuter (ex : `nn`, `2opt`, `bnb`, etc.).

- `--all-start`
  Exécute l’algorithme en démarrant depuis chaque ville (0 → n-1).

- `--quiet`
  Réduit l’affichage terminal au strict minimum.

- `--repeat`
  Répète l’exécution complète k fois pour obtenir des mesures stables.

- `--threads`
  Nombre de threads utilisés pour paralléliser les runs indépendants.

- `--time_limit <ms|s>`
  Temps maximal autorisé avant arrêt anticipé de l’algorithme.
```
