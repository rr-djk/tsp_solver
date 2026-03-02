#ifndef MAP_H
#define MAP_H

#include "city.hpp"
#include <istream>
#include <vector>

class Map {
public:
  const std::vector<City> &getCities() const;

private:
  std::vector<City> cities;
  friend std::istream &operator>>(std::istream &is, Map &map);
};

#endif
