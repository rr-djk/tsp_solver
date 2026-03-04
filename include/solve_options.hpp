#ifndef SOLVE_OPTIONS_HPP
#define SOLVE_OPTIONS_HPP

#include <chrono>
#include <cstdint>
#include <optional>

/**
 * @brief Paramètres optionnels transmis à un solveur TSP.
 */
struct SolveOptions {
  /** Identifiant TSPLIB de la ville de départ (absente = choix par l'algorithme). */
  std::optional<std::size_t> start_city_id;

  /** Durée maximale allouée à la résolution (absente = pas de limite). */
  std::optional<std::chrono::microseconds> time_limit;

  /** Graine pour l'initialisation du générateur aléatoire (absente = non déterministe). */
  std::optional<std::uint32_t> seed;
};

#endif
