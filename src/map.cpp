#include "map.hpp"
#include <cstdio>
#include <stdexcept>
#include <string>

const std::vector<City> &Map::getCities() const { return cities; }

std::istream &operator>>(std::istream &is, Map &map) {
  std::string line;
  unsigned int dimension = 0;

  while (std::getline(is, line)) {
    if (line.starts_with("DIMENSION")) {
      if (std::sscanf(line.c_str(), "DIMENSION : %u", &dimension) != 1)
        throw std::runtime_error("Ligne DIMENSION mal formatee");
    } else if (line == "NODE_COORD_SECTION") {
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

  return is;
}
