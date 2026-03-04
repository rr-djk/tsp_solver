#ifndef ISOLVER_HPP
#define ISOLVER_HPP

#include "map.hpp"
#include "solve_options.hpp"
#include "solve_result.hpp"

/**
 * @brief Interface commune à tous les algorithmes de résolution TSP.
 */
class ISolver {
public:
  virtual ~ISolver() = default;

  /**
   * @brief Résout le TSP sur une carte donnée et retourne le résultat.
   * @param map Carte contenant les villes à visiter.
   * @param options Paramètres de résolution (ville de départ, limite de temps, graine).
   * @return Résultat contenant le tour trouvé, son coût et la durée d'exécution.
   */
  virtual SolveResult solve(const Map &map, const SolveOptions &options) = 0;
};

#endif
