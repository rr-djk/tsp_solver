#ifndef SOLVE_RESULT_HPP
#define SOLVE_RESULT_HPP

#include "tour.hpp"
#include <chrono>
#include <string>

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
};

#endif
