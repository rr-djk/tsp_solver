#pragma once

#include "cli_options.hpp"

/**
 * @brief Parse et valide les arguments passés en ligne de commande.
 * @param argc Nombre d'arguments.
 * @param argv Tableau d'arguments.
 * @return Options parsées et validées.
 * NOTE: Termine le programme avec exit(1) si une option est inconnue,
 *       dupliquée, manquante ou hors contrainte.
 */
CliOptions parse_args_or_exit(int argc, char **argv);
