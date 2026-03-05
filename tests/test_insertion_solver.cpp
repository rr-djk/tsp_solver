#include <chrono>
#include <fstream>
#include <gtest/gtest.h>
#include <stdexcept>

#include "evaluator.hpp"
#include "insertion_solver.hpp"
#include "map.hpp"
#include "solve_options.hpp"

class InsertionSolverBerlin52 : public ::testing::Test {
protected:
  static Map map;
  InsertionSolver solver;

  static void SetUpTestSuite() {
    std::ifstream f("data/berlin52.tsp");
    ASSERT_TRUE(f.is_open()) << "Impossible d'ouvrir data/berlin52.tsp";
    f >> map;
  }
};

Map InsertionSolverBerlin52::map;

TEST_F(InsertionSolverBerlin52, SansVilleDeDepart_TourValide) {
  const SolveResult result = solver.solve(map, SolveOptions{});

  EXPECT_TRUE(result.tour.is_valid(map.size()));
}

TEST_F(InsertionSolverBerlin52, SansVilleDeDepart_CoutCoherent) {
  const SolveResult result = solver.solve(map, SolveOptions{});

  EXPECT_DOUBLE_EQ(result.cost, cost(map, result.tour));
}

TEST_F(InsertionSolverBerlin52, AvecVilleDeDepart_TourValide) {
  SolveOptions options;
  options.start_city_id = 1;

  const SolveResult result = solver.solve(map, options);

  EXPECT_TRUE(result.tour.is_valid(map.size()));
}

TEST_F(InsertionSolverBerlin52, AvecVilleDeDepart_PremiereVilleCorrecte) {
  SolveOptions options;
  options.start_city_id = 1;

  const SolveResult result = solver.solve(map, options);

  const int first_city_id = map.getCities()[static_cast<std::size_t>(
      result.tour.getOrder().front())].getId();
  EXPECT_EQ(first_city_id, 1);
}

TEST_F(InsertionSolverBerlin52, AvecVilleDeDepart_Invalide_LeveException) {
  SolveOptions options;
  options.start_city_id = 9999;

  EXPECT_THROW(solver.solve(map, options), std::invalid_argument);
}

TEST_F(InsertionSolverBerlin52, AvecTimeLimitDepasse_TourValide) {
  SolveOptions options;
  options.time_limit = std::chrono::microseconds{1};

  const SolveResult result = solver.solve(map, options);

  EXPECT_TRUE(result.tour.is_valid(map.size()));
}
