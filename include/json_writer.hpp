#pragma once
#include <filesystem>

struct SolveResult;

/**
 * @brief Écrit le résultat d'un solver dans un fichier JSON.
 * @param result Résultat à sérialiser.
 * @param out_path Chemin du fichier de sortie.
 * NOTE: Si @p out_path n'existe pas, le fichier est créé dans results/
 *       (le dossier est créé automatiquement si nécessaire).
 * NOTE: Lève std::runtime_error si le fichier ne peut pas être ouvert.
 *
 * Champs écrits : file_name, algo_name, cost, time_ms,
 *                 distance_calls, best_tour.
 */
void write_solve_result_json(const SolveResult &result,
                             const std::filesystem::path &out_path);
