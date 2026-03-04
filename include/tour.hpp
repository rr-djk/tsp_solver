#ifndef TOUR_H
#define TOUR_H

#include <cstddef>
#include <vector>

class Tour {
public:
  /**
   * @brief Construit un tour à partir d’un ordre donné.
   * @param order_ Vecteur représentant l’ordre de visite des villes.
   */
  explicit Tour(std::vector<int> order_);

  /** @return Nombre de villes dans le tour. */
  std::size_t size() const;

  /** @return Référence constante vers le vecteur d'ordre de visite. */
  const std::vector<int> &getOrder() const;

  /**
   * @brief Vérifie que le tour est une permutation valide de n villes.
   * @param n Nombre attendu de villes.
   * @return true si le tour est valide, false sinon.
   * @note Conditions : taille == n, tous les indices dans [0, n-1], aucun doublon.
   */
  bool is_valid(std::size_t n) const;

private:
  std::vector<int> order;
};

#endif
