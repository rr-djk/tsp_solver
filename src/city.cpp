#include "city.hpp"
#include <cmath>
#include <print>

City::City() : id(0), coord_x(0.0), coord_y(0.0) {}

City::City(int id_, double a, double b) : id(id_), coord_x(a), coord_y(b) {}

int City::getId() const { return id; }

double City::getCoordX() const { return coord_x; }

double City::getCoordY() const { return coord_y; }

void City::display() const {
  std::println("Ville {} : {:^10.4}, {:^10.4}", id, coord_x, coord_y);
}

double City::distanceTo(const City &other) const {
  return std::hypot(coord_x - other.coord_x, coord_y - other.coord_y);
}
