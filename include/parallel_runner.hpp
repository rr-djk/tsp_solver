#pragma once

#include "isolver.hpp"
#include "map.hpp"
#include "solve_options.hpp"
#include "solve_result.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <vector>

/**
 * @brief Exécute une liste de tâches TSP en parallèle et retourne le meilleur
 *        résultat trouvé.
 *
 * Chaque tâche est décrite par un SolveOptions distinct. Les tâches sont
 * distribuées sur @p num_threads workers via un index atomique partagé.
 * Chaque worker crée sa propre instance de solveur via @p make_solver, ce
 * qui garantit l'absence de contention sur l'état interne des solveurs.
 *
 * @param make_solver  Fabrique appelée une fois par thread pour créer un
 *                     ISolver indépendant.
 * @param map          Carte TSP partagée en lecture seule.
 * @param tasks        Liste des SolveOptions à exécuter (une par combinaison
 *                     repeat × start_city).
 * @param num_threads  Nombre de threads workers à lancer (>= 1).
 * @return             Le SolveResult de coût minimal, ou std::nullopt si
 *                     @p tasks est vide.
 */
std::optional<SolveResult>
run_parallel(std::function<std::unique_ptr<ISolver>()> make_solver,
             const Map &map, const std::vector<SolveOptions> &tasks,
             int num_threads);
