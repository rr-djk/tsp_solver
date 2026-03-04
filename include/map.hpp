#ifndef MAP_H
#define MAP_H

#include "city.hpp"
#include <cstddef>
#include <istream>
#include <vector>

class Map {
public:
  /** @return Référence constante vers le vecteur de villes chargées. */
  const std::vector<City> &getCities() const;

  /**
   * @brief Calcule la distance euclidienne entre deux villes par leur index.
   * @param i Index de la première ville dans cities.
   * @param j Index de la deuxième ville dans cities.
   * @return Distance euclidienne entre les villes i et j.
   * NOTE: Lève std::out_of_range si i ou j sont hors des bornes de cities.
   */
  double distance(std::size_t i, std::size_t j) const;

  /** @return Nombre de villes chargées dans la carte. */
  std::size_t size() const;

  /** @return true si aucune ville n'a été chargée, false sinon. */
  bool is_empty() const;

private:
  std::vector<City> cities;

  /**
   * @brief Lit et parse un fichier TSPLIB dans la carte.
   * @param is Flux d'entrée positionné au début du fichier .tsp.
   * @param map Carte à remplir avec les villes lues.
   * @return Référence au flux après lecture.
   * @note Lève std::runtime_error si DIMENSION est absent ou malformé,
   *       ou si le nombre de villes lues ne correspond pas à DIMENSION.
   */
  friend std::istream &operator>>(std::istream &is, Map &map);
};

#endif
