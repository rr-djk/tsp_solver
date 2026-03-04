#include <fstream>
#include <gtest/gtest.h>

#include "evaluator.hpp"
#include "map.hpp"
#include "nearest_neighbor_solver.hpp"
#include "solve_options.hpp"
#include "two_opt_solver.hpp"

class TwoOptSolverBerlin52 : public ::testing::Test {
protected:
  static Map map;
  TwoOptSolver solver;

  static void SetUpTestSuite() {
    std::ifstream f("data/berlin52.tsp");
    ASSERT_TRUE(f.is_open()) << "Impossible d'ouvrir data/berlin52.tsp";
    f >> map;
  }
};

Map TwoOptSolverBerlin52::map;

TEST_F(TwoOptSolverBerlin52, TourValide) {
  const SolveResult result = solver.solve(map, SolveOptions{});

  EXPECT_TRUE(result.tour.is_valid(map.size()));
}

// 2-opt part du tour NN et n'applique que des améliorations strictes :
// le coût final est donc garanti <= coût NN (même ville de départ).
TEST_F(TwoOptSolverBerlin52, CoutInferieurOuEgalNN) {
  NearestNeighborSolver nn_solver;
  const SolveResult nn_result = nn_solver.solve(map, SolveOptions{});
  const SolveResult two_opt_result = solver.solve(map, SolveOptions{});

  EXPECT_LE(two_opt_result.cost, nn_result.cost);
}

TEST_F(TwoOptSolverBerlin52, AvecVilleDeDepart_TourValide) {
  SolveOptions options;
  options.start_city_id = 1;

  const SolveResult result = solver.solve(map, options);

  EXPECT_TRUE(result.tour.is_valid(map.size()));
}

// Avec time_limit=0, la limite est dépassée dès le premier check :
// aucune passe 2-opt n'est effectuée, le résultat doit être le tour NN brut.
TEST_F(TwoOptSolverBerlin52, TimeLimit_ZeroArretImmediat) {
  NearestNeighborSolver nn_solver;
  const SolveResult nn_result = nn_solver.solve(map, SolveOptions{});

  SolveOptions options_zero;
  options_zero.time_limit = std::chrono::microseconds{0};
  const SolveResult limited_result = solver.solve(map, options_zero);

  EXPECT_DOUBLE_EQ(limited_result.cost, nn_result.cost);
}
