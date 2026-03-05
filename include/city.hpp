#ifndef CITY_H
#define CITY_H

class City {
public:
  /**
   * @brief Construit une ville par défaut (id=0, coordonnées nulles).
   */
  City();

  /**
   * @brief Construit une ville avec un identifiant et des coordonnées.
   * @param id Identifiant unique de la ville (issu du fichier TSPLIB).
   * @param coord_x Coordonnée x de la ville.
   * @param coord_y Coordonnée y de la ville.
   */
  City(int id, double coord_x, double coord_y);

  /** @return Identifiant entier de la ville. */
  int getId() const;

  /** @return Coordonnée x de la ville. */
  double getCoordX() const;

  /** @return Coordonnée y de la ville. */
  double getCoordY() const;

  /**
   * @brief Affiche l'identifiant et les coordonnées de la ville sur stdout.
   */
  void display() const;

  /**
   * @brief Calcule la distance euclidienne vers une autre ville.
   * @param other Ville cible.
   * @return Distance euclidienne entre cette ville et @p other.
   */
  double distanceTo(const City &other) const;

private:
  int id;
  double coord_x;
  double coord_y;
};

#endif
