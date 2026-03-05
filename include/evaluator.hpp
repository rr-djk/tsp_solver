#pragma once

class Map;
class Tour;

/**
 * @brief Calcule le coût total d'un tour TSP (retour au départ implicite).
 * @param map Instance contenant les villes.
 * @param tour Tournée (permutation des indices internes [0..n-1]).
 * @return Coût total du tour (double).
 * @note Si la carte contient 0 ou 1 ville, retourne 0.0.
 * @note Lève std::invalid_argument si le tour n'est pas valide pour cette carte.
 */
double cost(const Map &map, const Tour &tour);
