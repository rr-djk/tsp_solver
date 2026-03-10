#include "nearest_neighbor_solver.hpp"
#include "evaluator.hpp"
#include "map.hpp"
#include "tour.hpp"
#include <chrono>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

/**
 * @brief Trouve l'index interne de la ville correspondant à un identifiant
 * TSPLIB.
 * @param map Carte contenant les villes.
 * @param start_city_id Identifiant TSPLIB de la ville cherchée.
 * @return Index interne dans [0, n-1].
 * NOTE: Lève std::invalid_argument si aucune ville ne possède cet identifiant.
 */
static std::size_t find_start_index(const Map &map, std::size_t start_city_id) {
  const auto &cities = map.getCities();
  for (std::size_t i = 0; i < cities.size(); ++i) {
    if (static_cast<std::size_t>(cities[i].getId()) == start_city_id)
      return i;
  }
  throw std::invalid_argument("start_city_id introuvable dans la carte");
}

/**
 * @brief Trouve le voisin non visité le plus proche d'une ville donnée.
 * @param map Carte contenant les villes.
 * @param current_city_index Index interne de la ville courante.
 * @param visited_cities Vecteur indiquant les villes déjà visitées.
 * @return Index interne du voisin non visité le plus proche.
 */
static std::size_t
find_nearest_unvisited_city(const Map &map, std::size_t current_city_index,
                            const std::vector<bool> &visited_cities,
                            int &distance_calls) {
  double nearest_city_distance = std::numeric_limits<double>::infinity();
  std::size_t nearest_city_index = 0;

  for (std::size_t candidate_index = 0; candidate_index < visited_cities.size();
       ++candidate_index) {
    if (visited_cities[candidate_index])
      continue;
    ++distance_calls;
    const double distance_to_candidate =
        map.distance(current_city_index, candidate_index);
    if (distance_to_candidate < nearest_city_distance) {
      nearest_city_distance = distance_to_candidate;
      nearest_city_index = candidate_index;
    }
  }

  return nearest_city_index;
}

SolveResult NearestNeighborSolver::solve(const Map &map,
                                         const SolveOptions &options) {
  const auto start_time = std::chrono::steady_clock::now();
  const std::size_t city_count = map.size();

  std::size_t current_city_index =
      options.start_city_id.has_value()
          ? find_start_index(map, options.start_city_id.value())
          : 0;

  std::vector<bool> visited_cities(city_count, false);
  std::vector<int> visit_order;
  visit_order.reserve(city_count);

  visited_cities[current_city_index] = true;
  visit_order.push_back(static_cast<int>(current_city_index));

  int distance_calls = 0;

  for (std::size_t step = 1; step < city_count; ++step) {
    const std::size_t nearest_city_index = find_nearest_unvisited_city(
        map, current_city_index, visited_cities, distance_calls);

    visited_cities[nearest_city_index] = true;
    visit_order.push_back(static_cast<int>(nearest_city_index));
    current_city_index = nearest_city_index;
  }

  Tour tour{visit_order};
  const double tour_cost = cost(map, tour);
  const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::steady_clock::now() - start_time);

  SolveResult result{"", "nearest_neighbor", std::move(tour), tour_cost,
                     elapsed, distance_calls, 0, 0};
  return result;
}
