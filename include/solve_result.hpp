#ifndef SOLVE_RESULT_HPP
#define SOLVE_RESULT_HPP

#include "tour.hpp"
#include <chrono>
#include <string>

/** @brief Statut de terminaison d'une résolution TSP. */
enum class SolveStatus {
  ok,      ///< Résolution complète sans interruption.
  timeout, ///< Interrompue par --time-limit avant convergence.
  error,   ///< Tour invalide détecté après résolution.
};

/**
 * @brief Résultat produit par un solveur TSP après résolution.
 */
struct SolveResult {
  /** Nom du fichier .tsp utilisé comme entrée. */
  std::string file_name;

  /** Nom de l'algorithme ayant produit ce résultat. */
  std::string algo_name;

  /** Tour trouvé (permutation des indices internes). */
  Tour tour;

  /** Coût total du tour trouvé. */
  double cost;

  /** Durée d'exécution de l'algorithme. */
  std::chrono::microseconds duration;

  /** Combien de distances l’algorithme a calculées. */
  int distance_calls;

  /** Nombre de passes ou d’étapes de construction effectuées. */
  int iterations;

  /** Nombre d’améliorations appliquées (swaps, insertions bénéfiques). */
  int improvements;

  /** Statut de terminaison de la résolution. */
  SolveStatus status{SolveStatus::ok};
};

#endif
