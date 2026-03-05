#include "two_opt_solver.hpp"
#include "evaluator.hpp"
#include "map.hpp"
#include "nearest_neighbor_solver.hpp"
#include "tour.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <vector>

namespace {
/**
 * @brief Inverse le segment [i+1..j] dans l'ordre du tour (mouvement 2-opt).
 * @param order Vecteur d'ordre de visite à modifier en place.
 * @param i Borne gauche du segment (exclusive).
 * @param j Borne droite du segment (inclusive).
 */
inline void apply_2opt_swap(std::vector<int> &order, std::size_t i,
                            std::size_t j) {
  std::reverse(order.begin() + static_cast<std::ptrdiff_t>(i + 1),
               order.begin() + static_cast<std::ptrdiff_t>(j + 1));
}

/**
 * @brief Calcule le delta de coût pour un échange 2-opt entre arêtes (a,b)
 *        et (c,d), remplacées par (a,c) et (b,d).
 * @param map Carte contenant les villes.
 * @param a Index interne de la ville a.
 * @param b Index interne de la ville b (successeur de a).
 * @param c Index interne de la ville c.
 * @param d Index interne de la ville d (successeur de c).
 * @return Delta de coût : négatif si l'échange est bénéfique.
 */
inline double compute_2opt_delta(const Map &map, int a, int b, int c, int d,
                                 int &distance_calls) {
  auto idx = [](int i) { return static_cast<std::size_t>(i); };
  distance_calls += 4;
  return -map.distance(idx(a), idx(b)) - map.distance(idx(c), idx(d)) +
         map.distance(idx(a), idx(c)) + map.distance(idx(b), idx(d));
}

/**
 * @brief Vérifie si la limite de temps allouée a été dépassée.
 * @param start_time Point de départ de la mesure.
 * @param options Options de résolution contenant la limite de temps
 * optionnelle.
 * @return true si la limite est dépassée, false sinon ou si absente.
 */
inline bool
time_limit_exceeded(const std::chrono::steady_clock::time_point &start_time,
                    const SolveOptions &options) {
  if (!options.time_limit.has_value())
    return false;
  const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::steady_clock::now() - start_time);
  return elapsed >= *options.time_limit;
}

/**
 * @brief Améliore un tour en place par 2-opt (first-improvement).
 * @param map Carte contenant les villes.
 * @param visit_order Ordre de visite à améliorer, modifié en place.
 * @param options Options de résolution (limite de temps optionnelle).
 * @return Durée de l'optimisation.
 */
std::chrono::microseconds improve_with_2opt(const Map &map,
                                            std::vector<int> &visit_order,
                                            const SolveOptions &options,
                                            int &distance_calls) {
  const auto start_time = std::chrono::steady_clock::now();
  const std::size_t city_count = visit_order.size();

  bool improved = true;
  while (improved) {
    improved = false;

    if (time_limit_exceeded(start_time, options))
      break;

    for (std::size_t i = 0; i + 1 < city_count; ++i) {
      if (time_limit_exceeded(start_time, options))
        break;

      for (std::size_t j = i + 2; j < city_count; ++j) {
        // Évite les arêtes adjacentes en cas de wrap-around
        if (i == 0 && j == city_count - 1)
          continue;

        const std::size_t next_i = i + 1;
        const std::size_t next_j = (j + 1) % city_count;

        const double delta =
            compute_2opt_delta(map, visit_order[i], visit_order[next_i],
                               visit_order[j], visit_order[next_j],
                               distance_calls);

        if (delta < -1e-12) {
          apply_2opt_swap(visit_order, i, j);
          improved = true;
          break; // first-improvement : on recommence une passe complète
        }
      }

      if (improved)
        break;
    }
  }

  return std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::steady_clock::now() - start_time);
}

} // namespace

SolveResult TwoOptSolver::solve(const Map &map, const SolveOptions &options) {
  NearestNeighborSolver nn_solver;
  SolveResult nn_result = nn_solver.solve(map, options);

  std::vector<int> visit_order(nn_result.tour.getOrder());
  int distance_calls = 0;
  const auto elapsed = improve_with_2opt(map, visit_order, options, distance_calls);

  Tour final_tour(visit_order);
  const double final_cost = cost(map, final_tour);
  return SolveResult{nn_result.file_name, "two_opt", std::move(final_tour),
                     final_cost, elapsed, distance_calls};
}
