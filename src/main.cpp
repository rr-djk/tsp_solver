#include <chrono>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <numeric>
#include <print>
#include <random>
#include <string_view>

#include "evaluator.hpp"
#include "map.hpp"
#include "tour.hpp"

[[noreturn]] static void print_help_and_exit() {
  std::cout
      << "Usage:\n"
      << "  tsp_solver --input-file <path> --algo <name> --output-file <path>"
         " [options]\n\n"
      << "Obligatoires:\n"
      << "  --input-file <path>\n"
      << "  --algo <name>         (nn | 2opt | insertion)\n"
      << "  --output-file <path>\n\n"
      << "Optionnels:\n"
      << "  --all-start\n"
      << "  --quiet\n"
      << "  --repeat <k>          (entier >= 1)\n"
      << "  --threads <t>         (1 <= t <= hardware_concurrency)\n"
      << "  --time-limit <Ns>     (N entier > 0, suffixe 's', ex: 10s)\n\n"
      << "Aide:\n"
      << "  tsp_solver help\n";
  std::exit(0);
}

int main(int argc, char *argv[]) {
  if (argc >= 2 && std::string_view(argv[1]) == "help")
    print_help_and_exit();

  if (argc < 2) {
    std::println(stderr, "Parametres manquants");
    return 1;
  }

  const std::string filename = argv[1];
  std::println("Lecture du fichier : {}", filename);

  Map map;
  std::ifstream fichier_tsp(filename);
  if (fichier_tsp.fail()) {
    std::println(stderr, "Erreur a l'ouverture du fichier : {}", filename);
    return 1;
  }
  try {
    fichier_tsp >> map;
  } catch (const std::exception &e) {
    std::println(stderr, "Erreur a la creation de la carte : {}", e.what());
    return 1;
  }

  const std::size_t map_size = map.size();
  std::vector<int> order(map_size);
  std::iota(order.begin(), order.end(), 0);
  std::shuffle(order.begin(), order.end(),
               std::mt19937{std::random_device{}()});
  Tour tour(order);

  using steady_clock = std::chrono::steady_clock;
  const auto start = steady_clock::now();
  const double total_cost = cost(map, tour);
  const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
      steady_clock::now() - start);

  std::println("=== Évaluation TSP ===");
  std::println("Nombre de villes    : {}", map_size);
  std::println("Coût du tour        : {:.4f}", total_cost);
  std::println("Temps d'évaluation  : {}", elapsed);

  return 0;
}
