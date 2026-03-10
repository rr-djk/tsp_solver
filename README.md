# TSP Solver

## Objectif

Ce projet a pour but d’implémenter et d’explorer différentes approches pour résoudre le problème du voyageur de commerce (TSP - Traveling Salesman Problem).

Il s’agit d’un projet personnel et pédagogique visant à :

- Approfondir la compréhension des algorithmes classiques
- Expérimenter l’optimisation des performances
- Développer une approche rigoureuse de benchmarking et d’analyse


L’objectif n’est pas simplement d’obtenir une solution fonctionnelle, mais de comprendre en profondeur le comportement des algorithmes, leurs limites, et leurs implications pratiques.

## Définition du TSP

Dans ce projet, le TSP consiste à prendre un ensemble de villes (points fournis dans un fichier) et à déterminer l’ordre optimal dans lequel les visiter afin de minimiser la distance totale d’un tour complet, en commençant et en terminant à la même ville.

Ce n’est pas simplement un problème de “plus court chemin” entre deux points : il faut déterminer le meilleur ordre global de visite parmi toutes les possibilités.

Référence : https://en.wikipedia.org/wiki/Travelling_salesman_problem

## Prérequis

**C++ :**
- Système Linux
- g++ 14, support C++23
- Optionnel (profiling) : `sudo apt install linux-tools-common linux-tools-generic valgrind`
- Optionnel (tests unitaires) : `sudo apt install libgtest-dev`

**Python (visualisation) :**
- Python 3.10+

## Build et exécution

```bash
make          # compile le binaire dans build/tsp_parser
make test     # compile et exécute les tests unitaires
make clean    # supprime le répertoire build/
```

### Exécution

```bash
./build/tsp_parser --input-file <fichier.tsp> --algo <nn|2opt|insertion> --output-file <résultats.json> [options]
```

**Options disponibles :**

| Option | Description |
|---|---|
| `--input-file <path>` | Fichier `.tsp` TSPLIB à charger *(obligatoire)* |
| `--algo <name>` | Algorithme : `nn`, `2opt`, `insertion` *(obligatoire)* |
| `--output-file <path>` | Fichier JSON de sortie *(obligatoire)* |
| `--all-start` | Exécute depuis chaque ville de départ |
| `--quiet` | Réduit l'affichage terminal |
| `--repeat <k>` | Répète l'exécution k fois (k ≥ 1) |
| `--threads <t>` | Nombre de threads (1 ≤ t ≤ hardware_concurrency) |
| `--time-limit <Ns>` | Limite de temps (ex : `10s`) |

**Exemples :**

```bash
# Exécution simple
./build/tsp_parser --input-file data/berlin52.tsp --algo nn --output-file results/berlin52_nn.json

# Parallèle : toutes les villes de départ sur 4 threads (all-start × threads)
./build/tsp_parser --input-file data/berlin52.tsp --algo 2opt --all-start --threads 4 --output-file results/berlin52_2opt.json

# Grosse instance avec limite de temps et parallélisation
./build/tsp_parser --input-file data/pla7397.tsp --algo nn --all-start --threads 8 --time-limit 30s --output-file results/pla7397_nn.json
```

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

Le script lit le fichier JSON produit par `tsp_solver`, charge les coordonnées
depuis le fichier `.tsp` correspondant dans `data/`, et affiche le tour optimal.

**Options :**

| Option | Description |
|---|---|
| `--data-dir <dir>` | Dossier contenant les `.tsp` (défaut : `data/`) |
| `--output <file>` | Sauvegarde en `.png` ou `.svg` au lieu d'ouvrir une fenêtre graphique |

```bash
python3 scripts/plot_tour.py results/berlin52_nn.json --output tour.png
```

---

## Sources de données

Les villes utilisées proviennent du dépôt TSPLIB : https://github.com/mastqe/tsplib/tree/master
