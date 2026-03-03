#ifndef TOUR_H
#define TOUR_H

#include <cstddef>
#include <vector>

class Tour {
public:
  /**
   * Construit un tour à partir d’un ordre donné.
   *
   * @param order Vecteur représentant l’ordre de visite des villes.
   */
  explicit Tour(std::vector<int> order_);

  std::size_t size() const;

  const std::vector<int> &getOrder() const;

  /**
   * Vérifie que le tour est une permutation valide
   *
   * Conditions :
   * - Taille == n
   * - Tous les indices sont dans [0, n-1]
   * - Aucun doublon
   *
   * @param n Nombre attendu de villes.
   * @return true si le tour est valide, false sinon.
   */
  bool is_valid(std::size_t n) const;

private:
  std::vector<int> order;
};

#endif
