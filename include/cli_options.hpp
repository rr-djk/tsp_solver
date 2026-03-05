#pragma once

#include <optional>
#include <string>

/**
 * @brief Paramètres CLI parsés et validés.
 */
struct CliOptions {
  /** Chemin vers le fichier .tsp TSPLIB à charger. */
  std::string input_file;

  /** Chemin vers le fichier JSON de sortie. */
  std::string output_file;

  /** Nom de l'algorithme à exécuter (nn | 2opt | insertion). */
  std::string algo;

  /** Si true, exécute l'algorithme depuis chaque ville de départ. */
  bool all_start = false;

  /** Si true, réduit l'affichage terminal au strict minimum. */
  bool quiet = false;

  /** Nombre de répétitions de l'exécution complète (>= 1). */
  int repeat = 1;

  /** Nombre de threads pour la parallélisation (1..hardware_concurrency). */
  int threads = 1;

  /** Limite de temps en secondes (absent = pas de limite). */
  std::optional<int> time_limit_seconds;
};
