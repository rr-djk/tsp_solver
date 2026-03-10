#include "map.hpp"
#include <cstddef>
#include <cstdio>
#include <stdexcept>
#include <string>

const std::vector<City> &Map::getCities() const { return cities; }

/**
 * @brief Précalcule la matrice de distances en O(n²).
 * Pour chaque paire (i, j), calcule la distance euclidienne une seule fois
 * et la stocke dans dist_matrix_[i * n + j] et dist_matrix_[j * n + i]
 * (symétrie exploitée pour réduire le nombre d'appels à distanceTo).
 */
void Map::build_dist_matrix() {
  const std::size_t n = cities.size();
  dist_matrix_.assign(n * n, 0.0);
  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = i + 1; j < n; ++j) {
      const double d = cities[i].distanceTo(cities[j]);
      dist_matrix_[i * n + j] = d;
      dist_matrix_[j * n + i] = d;
    }
  }
}

double Map::distance(std::size_t index_city_a, std::size_t index_city_b) const {
  return dist_matrix_[index_city_a * cities.size() + index_city_b];
}

std::size_t Map::size() const { return cities.size(); }

bool Map::is_empty() const { return cities.empty(); }

std::istream &operator>>(std::istream &is, Map &map) {
  std::string line;
  unsigned int dimension = 0;

  while (std::getline(is, line)) {
    if (line.starts_with("DIMENSION")) {
      if (std::sscanf(line.c_str(), "DIMENSION : %u", &dimension) != 1)
        throw std::runtime_error("Ligne DIMENSION mal formatee");
    } else if (line.starts_with("NODE_COORD_SECTION")) {
      break;
    }
  }
  if (dimension == 0)
    throw std::runtime_error("DIMENSION invalide");
  map.cities.clear();
  map.cities.reserve(dimension);

  int id;
  double x, y;
  while (is >> id >> x >> y)
    map.cities.emplace_back(id, x, y);
  if (map.cities.size() != dimension)
    throw std::runtime_error("Nombre de villes != DIMENSION");

  map.build_dist_matrix();

  return is;
}
