#include <chrono>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <print>
#include <string_view>
#include <vector>

#include "cli_parser.hpp"
#include "insertion_solver.hpp"
#include "isolver.hpp"
#include "json_writer.hpp"
#include "map.hpp"
#include "nearest_neighbor_solver.hpp"
#include "solve_options.hpp"
#include "solve_result.hpp"
#include "two_opt_solver.hpp"

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

static Map load_map_or_exit(const std::string &path) {
  std::ifstream file(path);
  if (!file) {
    std::println(stderr, "Erreur à l'ouverture : {}", path);
    std::exit(1);
  }
  Map map;
  try {
    file >> map;
  } catch (const std::exception &e) {
    std::println(stderr, "Erreur de lecture : {}", e.what());
    std::exit(1);
  }
  return map;
}

static std::unique_ptr<ISolver> make_solver(const std::string &algo) {
  if (algo == "nn")
    return std::make_unique<NearestNeighborSolver>();
  if (algo == "2opt")
    return std::make_unique<TwoOptSolver>();
  if (algo == "insertion")
    return std::make_unique<InsertionSolver>();
  std::println(stderr, "Algorithme inconnu : {}", algo);
  std::exit(1);
}

int main(int argc, char *argv[]) {
  if (argc >= 2 && std::string_view(argv[1]) == "help")
    print_help_and_exit();

  const CliOptions opts = parse_args_or_exit(argc, argv);

  const Map map = load_map_or_exit(opts.input_file);

  SolveOptions solve_opts;
  if (opts.time_limit_seconds.has_value())
    solve_opts.time_limit = std::chrono::seconds(*opts.time_limit_seconds);

  auto solver = make_solver(opts.algo);

  std::vector<std::optional<std::size_t>> starts;
  if (opts.all_start) {
    for (const auto &city : map.getCities())
      starts.push_back(static_cast<std::size_t>(city.getId()));
  } else {
    starts.push_back(std::nullopt);
  }

  std::optional<SolveResult> best;

  for (int rep = 0; rep < opts.repeat; ++rep) {
    for (const auto &start : starts) {
      solve_opts.start_city_id = start;
      SolveResult result = solver->solve(map, solve_opts);
      result.file_name = opts.input_file;
      if (!best.has_value() || result.cost < best->cost)
        best = std::move(result);
    }
  }

  if (!opts.quiet) {
    std::println("Algorithme : {}", best->algo_name);
    std::println("Coût       : {:.4f}", best->cost);
    std::println("Durée      : {}", best->duration);
  }

  try {
    write_solve_result_json(*best, opts.output_file);
  } catch (const std::exception &e) {
    std::println(stderr, "Erreur d'écriture JSON : {}", e.what());
    return 1;
  }

  return 0;
}
