#include "insertion_solver.hpp"
#include "evaluator.hpp"
#include "map.hpp"
#include "tour.hpp"
#include <chrono>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {

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
 * @brief Calcule le delta de coût pour l'insertion de x entre u et v.
 * @param map Carte contenant les villes.
 * @param u Index interne de la ville précédente.
 * @param v Index interne de la ville suivante.
 * @param x Index interne de la ville à insérer.
 * @return d(u,x) + d(x,v) - d(u,v)
 */
inline double compute_insertion_delta(const Map &map, int u, int v, int x,
                                      int &distance_calls) {
  auto idx = [](int i) { return static_cast<std::size_t>(i); };
  distance_calls += 3;
  return map.distance(idx(u), idx(x)) + map.distance(idx(x), idx(v)) -
         map.distance(idx(u), idx(v));
}

/**
 * @brief Trouve l'index interne de la ville correspondant à un identifiant
 *        TSPLIB.
 * @param map Carte contenant les villes.
 * @param start_city_id Identifiant TSPLIB de la ville cherchée.
 * @return Index interne dans [0, n-1].
 * NOTE: Lève std::invalid_argument si aucune ville ne possède cet identifiant.
 */
std::size_t find_start_index(const Map &map, std::size_t start_city_id) {
  const auto &cities = map.getCities();
  for (std::size_t i = 0; i < cities.size(); ++i) {
    if (static_cast<std::size_t>(cities[i].getId()) == start_city_id)
      return i;
  }
  throw std::invalid_argument("start_city_id introuvable dans la carte");
}

/**
 * @brief Trouve l'index interne de la ville la plus proche de from_index.
 * @param map Carte contenant les villes.
 * @param from_index Index interne de la ville de référence (ignorée dans la
 *        recherche).
 * @return Index interne de la ville la plus proche.
 */
std::size_t find_nearest_city(const Map &map, std::size_t from_index,
                              int &distance_calls) {
  double nearest_distance = std::numeric_limits<double>::infinity();
  std::size_t nearest_index = 0;

  for (std::size_t v = 0; v < map.size(); ++v) {
    if (v == from_index)
      continue;
    ++distance_calls;
    const double d = map.distance(from_index, v);
    if (d < nearest_distance) {
      nearest_distance = d;
      nearest_index = v;
    }
  }

  return nearest_index;
}

struct InsertionCandidate {
  int city_index;  // -1 si aucun candidat trouvé
  std::size_t pos; // insérer après order[pos]
  double delta;
};

/**
 * @brief Trouve parmi les villes non insérées celle dont le coût d'insertion
 *        minimal est le plus faible (cheapest insertion globale).
 * @param map Carte contenant les villes.
 * @param order Ordre de visite courant.
 * @param in_tour Vecteur indiquant les villes déjà insérées.
 * @return Meilleur candidat trouvé (city_index == -1 si aucun).
 */
InsertionCandidate find_cheapest_candidate(const Map &map,
                                           const std::vector<int> &order,
                                           const std::vector<bool> &in_tour,
                                           int &distance_calls) {
  // NOTE: time_limit n'est pas vérifié dans ces boucles internes.
  // Si time_limit expire pendant find_cheapest_candidate, la passe en cours
  // se termine normalement avant que run_cheapest_insertion ne détecte le
  // dépassement. La ville retournée est valide ; le tour reste cohérent.
  InsertionCandidate best{-1, 0, std::numeric_limits<double>::infinity()};
  const std::size_t tour_size = order.size();

  for (std::size_t city = 0; city < in_tour.size(); ++city) {
    if (in_tour[city])
      continue;

    double city_best_delta = std::numeric_limits<double>::infinity();
    std::size_t city_best_pos = 0;

    for (std::size_t pos = 0; pos < tour_size; ++pos) {
      const int u = order[pos];
      const int v = order[(pos + 1) % tour_size];
      const double delta =
          compute_insertion_delta(map, u, v, static_cast<int>(city), distance_calls);
      if (delta < city_best_delta) {
        city_best_delta = delta;
        city_best_pos = pos;
      }
    }

    if (city_best_delta < best.delta)
      best = {static_cast<int>(city), city_best_pos, city_best_delta};
  }

  return best;
}

/**
 * @brief Construit un tour par cheapest insertion itérative.
 * @param map Carte contenant les villes.
 * @param visit_order Ordre de visite initial (2 villes), complété en place.
 * @param in_tour Vecteur indiquant les villes déjà insérées, mis à jour en place.
 * @param options Options de résolution (limite de temps optionnelle).
 * @return Durée de la construction.
 */
std::chrono::microseconds
run_cheapest_insertion(const Map &map, std::vector<int> &visit_order,
                       std::vector<bool> &in_tour, const SolveOptions &options,
                       int &distance_calls) {
  const auto start_time = std::chrono::steady_clock::now();
  const std::size_t city_count = in_tour.size();

  while (visit_order.size() < city_count) {
    if (time_limit_exceeded(start_time, options))
      break;

    const InsertionCandidate candidate =
        find_cheapest_candidate(map, visit_order, in_tour, distance_calls);

    if (candidate.city_index < 0)
      break;

    visit_order.insert(visit_order.begin() +
                           static_cast<std::ptrdiff_t>(candidate.pos + 1),
                       candidate.city_index);
    in_tour[static_cast<std::size_t>(candidate.city_index)] = true;
  }

  // Si time_limit a interrompu avant la fin, on complète avec les villes
  // restantes pour garantir une permutation valide.
  for (std::size_t city = 0; city < city_count; ++city) {
    if (!in_tour[city])
      visit_order.push_back(static_cast<int>(city));
  }

  return std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::steady_clock::now() - start_time);
}

} // namespace

SolveResult InsertionSolver::solve(const Map &map, const SolveOptions &options) {
  const std::size_t city_count = map.size();

  if (city_count < 2) {
    std::vector<int> order;
    for (std::size_t i = 0; i < city_count; ++i)
      order.push_back(static_cast<int>(i));
    Tour final_tour(order);
    return SolveResult{"", "cheapest_insertion", std::move(final_tour), 0.0,
                       std::chrono::microseconds{0}, 0, 0, 0};
  }

  const std::size_t start_index =
      options.start_city_id.has_value()
          ? find_start_index(map, options.start_city_id.value())
          : 0;

  // Tour initial : start + son voisin le plus proche
  int distance_calls = 0;

  const std::size_t second_index =
      find_nearest_city(map, start_index, distance_calls);

  std::vector<int> visit_order;
  visit_order.reserve(city_count);
  visit_order.push_back(static_cast<int>(start_index));
  visit_order.push_back(static_cast<int>(second_index));

  std::vector<bool> in_tour(city_count, false);
  in_tour[start_index] = true;
  in_tour[second_index] = true;

  const auto elapsed =
      run_cheapest_insertion(map, visit_order, in_tour, options, distance_calls);

  Tour final_tour(visit_order);
  const double final_cost = cost(map, final_tour);
  SolveResult result{"", "cheapest_insertion", std::move(final_tour),
                     final_cost, elapsed, distance_calls, 0, 0};
  if (options.time_limit.has_value() && elapsed >= *options.time_limit)
    result.status = SolveStatus::timeout;
  return result;
}
