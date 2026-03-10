# TSP Solver

## Objectif

Ce projet implémente et explore différentes approches pour résoudre le problème du voyageur de commerce (TSP — Traveling Salesman Problem) sur des instances [TSPLIB](https://github.com/mastqe/tsplib/tree/master).

Il s'agit d'un projet personnel et pédagogique visant à :

- Approfondir la compréhension des algorithmes heuristiques classiques
- Expérimenter l'optimisation des performances (profiling, matrices de distance)
- Développer une approche rigoureuse de benchmarking et de parallélisation
- Produire une CLI robuste avec sortie JSON structurée

L'objectif n'est pas simplement d'obtenir une solution fonctionnelle, mais de comprendre en profondeur le comportement des algorithmes, leurs limites, et leurs implications pratiques.

Référence : <https://en.wikipedia.org/wiki/Travelling_salesman_problem>

---

## Architecture

Le projet est structuré autour de plusieurs composants principaux :

- **CLI parser** (`cli_parser`) : validation stricte et parsing des arguments en ligne de commande
- **TSPLIB parser** (`Map`) : lecture des fichiers `.tsp` et précalcul de la matrice de distances
- **Solveurs** (`NearestNeighborSolver`, `InsertionSolver`, `TwoOptSolver`) : implémentations des heuristiques, toutes conformes à l'interface `ISolver`
- **Thread runner** (`ParallelRunner`) : distribution lock-free des tâches sur un pool de threads via compteur atomique
- **JSON writer** (`JsonWriter`) : export des résultats structurés (config, status, coût, tour)

## Structure du projet

```
.
├── data/        # instances TSPLIB (.tsp)
├── include/     # headers C++ (interfaces, structs, déclarations)
├── src/         # implémentations C++
├── tests/       # suite de tests GTest
├── scripts/     # visualisation Python (plot_tour.py)
├── results/     # résultats JSON générés
└── build/       # binaires compilés (généré par make)
```

---

## Prérequis

**C++ :**

- Système Linux
- g++ 14, support C++23
- Optionnel (tests unitaires) : `sudo apt install libgtest-dev`
- Optionnel (profiling) : `sudo apt install linux-tools-common linux-tools-generic valgrind`

**Python (visualisation) :**

- Python 3.10+

---

## Build

```bash
make          # compile le binaire dans build/tsp_parser
make test     # compile et exécute la suite de tests GTest
make clean    # supprime le répertoire build/
```

---

## Utilisation

```bash
./build/tsp_parser --input-file <fichier.tsp> --algo <nn|2opt|insertion> --output-file <résultats.json> [options]
```

### Options

| Option | Description |
|---|---|
| `--input-file <path>` | Fichier `.tsp` TSPLIB à charger *(obligatoire)* |
| `--algo <name>` | Algorithme : `nn`, `2opt`, `insertion` *(obligatoire)* |
| `--output-file <path>` | Fichier JSON de sortie *(obligatoire)* |
| `--all-start` | Exécute depuis chaque ville de départ |
| `--repeat <k>` | Répète l'exécution k fois (k ≥ 1) |
| `--threads <t>` | Nombre de threads (1 ≤ t ≤ hardware_concurrency) |
| `--time-limit <Ns>` | Limite de temps (ex : `10s`, `30s`) |
| `--quiet` | Réduit l'affichage terminal |

### Exemples

```bash
# Exécution simple
./build/tsp_parser --input-file data/berlin52.tsp --algo nn --output-file results/berlin52_nn.json

# 2-opt depuis toutes les villes de départ, parallélisé sur 4 threads
./build/tsp_parser --input-file data/gr431.tsp --algo 2opt --all-start --threads 4 --output-file results/gr431_2opt.json

# Grande instance avec limite de temps
./build/tsp_parser --input-file data/pla7397.tsp --algo 2opt --all-start --threads 8 --time-limit 30s --output-file results/pla7397_2opt.json
```

---

## Algorithmes

| Algorithme | Clé CLI | Complexité par passe | Description |
|---|---|---|---|
| Nearest Neighbor | `nn` | O(n²) | Heuristique gloutonne : visite toujours la ville la plus proche non visitée |
| Cheapest Insertion | `insertion` | O(n²) à O(n³) | Construit le tour en insérant à chaque étape la ville dont l'insertion coûte le moins |
| 2-opt | `2opt` | O(n²) | Amélioration locale : démarre sur un tour NN, inverse des segments tant que ça réduit le coût |

Tous les algorithmes supportent `--all-start`, `--repeat`, `--threads` et `--time-limit`.

---

## Format de sortie JSON

Chaque run produit un fichier JSON structuré :

```json
{
  "config": {
    "algo": "2opt",
    "threads": 4,
    "repeat": 1,
    "all_start": true,
    "time_limit_s": 30
  },
  "status": "ok",
  "file_name": "gr431.tsp",
  "algo_name": "two_opt",
  "cost": 12345.67,
  "time_ms": 4340.0,
  "distance_calls": 18543210,
  "best_tour": [0, 12, 7, ...]
}
```

Le champ `status` vaut `ok`, `timeout` (limite de temps atteinte) ou `error` (tour invalide détecté).

---

## Performances

### Optimisation : matrice de distances précalculée

Sur `gr431.tsp` (431 villes) avec 2-opt `--all-start`, le profiling `perf` a révélé que **74 % du temps CPU** était passé dans `hypot()` — les distances étant recalculées à chaque itération de la boucle 2-opt.

Solution : précalcul d'une matrice de distances n×n au chargement du fichier (O(1) par accès au lieu de O(1) + coût flottant).

| Mesure | Avant | Après |
|---|---|---|
| Temps total | 197 s | 32 s |
| IPC | 3.19 | 4.93 |
| **Speedup** | — | **×6.15** |

Rapport complet : [report_profiling.md](report_profiling.md)

### Parallélisation : thread pool

Distribution lock-free avec un compteur atomique `fetch_add`. Chaque thread crée son propre solveur, sans mutex.

Mesures sur `gr431.tsp`, 2-opt `--all-start` (431 tâches indépendantes) :

| Threads | Temps (s) | Speedup |
|--------:|----------:|--------:|
| 1 | 19.45 | ×1.00 |
| 2 | 9.77 | ×1.99 |
| 4 | 6.84 | ×2.84 |
| 6 | 5.29 | ×3.68 |
| **8** | **4.34** | **×4.48** |
| 12 | 4.54 | ×4.28 |

Zone efficace : **6–8 threads**. L'hyperthreading au-delà des cœurs physiques apporte peu voire dégrade.

Rapport complet : [report_speedup.md](report_speedup.md)

---

## Instances recommandées

| Instance | Villes | Usage recommandé |
|---|---|---|
| `berlin52.tsp` | 52 | Tests unitaires, développement |
| `ch150.tsp` | 150 | Validation rapide |
| `gr431.tsp` | 431 | Benchmark algorithmes et threads |
| `p654.tsp` | 654 | Benchmark moyen |
| `vm1084.tsp` | 1 084 | Grosse instance heuristique |
| `pla7397.tsp` | 7 397 | Stress test (utiliser `--time-limit`) |
| `usa13509.tsp` | 13 509 | Stress test (utiliser `--time-limit`) |

---

## Visualisation

### Installation

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r scripts/requirements.txt
```

### Tracer un tour

```bash
python3 scripts/plot_tour.py results/berlin52_nn.json
```

Le script lit le JSON produit par `tsp_parser`, charge les coordonnées depuis le `.tsp` dans `data/`, et affiche le tour.

| Option | Description |
|---|---|
| `--data-dir <dir>` | Dossier contenant les `.tsp` (défaut : `data/`) |
| `--output <file>` | Sauvegarde en `.png` ou `.svg` au lieu d'afficher |

```bash
python3 scripts/plot_tour.py results/berlin52_2opt.json --output tour.png
```

---

## Sources de données

Instances TSPLIB : <https://github.com/mastqe/tsplib/tree/master>
