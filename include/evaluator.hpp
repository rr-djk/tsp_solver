#pragma once

class Map;
class Tour;

/**
 * Calcule le coût total d'un tour TSP (retour au départ implicite).
 *
 * Hypothèses :
 * - Tour contient des indices internes [0..n-1] (pas les ids TSPLIB).
 * - Le retour à la ville de départ est implicite (dernier -> premier).
 *
 * Comportement :
 * - Si la carte contient 0 ou 1 ville, retourne 0.0.
 * - Si le tour n'est pas valide pour cette carte, lève std::invalid_argument.
 *
 * @param map Instance contenant les villes
 * @param tour Tournée (permutation des indices).
 * @return Coût total (double).
 */
double cost(const Map &map, const Tour &tour);
