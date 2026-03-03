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

- Système Linux
- g++ 14
- Support de C++23
- Optionnel (pour reproduire les résultats de profiling) : `sudo apt install linux-tools-common linux-tools-generic valgrind`
- Optionnel (pour les tests unitaires) : `sudo apt install libgtest-dev`

## Sources de données

Les villes utilisées proviennent du dépôt TSPLIB : https://github.com/mastqe/tsplib/tree/master
