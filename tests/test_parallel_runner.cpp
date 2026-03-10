#include <fstream>
#include <gtest/gtest.h>
#include <memory>

#include "map.hpp"
#include "nearest_neighbor_solver.hpp"
#include "parallel_runner.hpp"
#include "solve_options.hpp"
#include "two_opt_solver.hpp"

class ParallelRunnerBerlin52 : public ::testing::Test {
protected:
  static Map map;

  static void SetUpTestSuite() {
    std::ifstream f("data/berlin52.tsp");
    ASSERT_TRUE(f.is_open()) << "Impossible d'ouvrir data/berlin52.tsp";
    f >> map;
  }

  static auto nn_factory() {
    return []() -> std::unique_ptr<ISolver> {
      return std::make_unique<NearestNeighborSolver>();
    };
  }
};

Map ParallelRunnerBerlin52::map;

// --- Cas limites ---

TEST_F(ParallelRunnerBerlin52, ListeVideRetourneNullopt) {
  const auto result = run_parallel(nn_factory(), map, {}, 2);
  EXPECT_FALSE(result.has_value());
}

TEST_F(ParallelRunnerBerlin52, UneTacheUnThread_TourValide) {
  const std::vector<SolveOptions> tasks = {SolveOptions{}};
  const auto result = run_parallel(nn_factory(), map, tasks, 1);

  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(result->tour.is_valid(map.size()));
  EXPECT_GT(result->cost, 0.0);
}

TEST_F(ParallelRunnerBerlin52, UneTache_PlusDeThreadsQueTaches_NeCrachePas) {
  const std::vector<SolveOptions> tasks = {SolveOptions{}};
  const auto result = run_parallel(nn_factory(), map, tasks, 16);

  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(result->tour.is_valid(map.size()));
}

// --- Parallélisation de all-start ---

TEST_F(ParallelRunnerBerlin52, AllStart_1Thread_TourValide) {
  std::vector<SolveOptions> tasks;
  for (const auto &city : map.getCities()) {
    SolveOptions o;
    o.start_city_id = static_cast<std::size_t>(city.getId());
    tasks.push_back(o);
  }

  const auto result = run_parallel(nn_factory(), map, tasks, 1);

  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(result->tour.is_valid(map.size()));
}

TEST_F(ParallelRunnerBerlin52, AllStart_4Threads_TourValide) {
  std::vector<SolveOptions> tasks;
  for (const auto &city : map.getCities()) {
    SolveOptions o;
    o.start_city_id = static_cast<std::size_t>(city.getId());
    tasks.push_back(o);
  }

  const auto result = run_parallel(nn_factory(), map, tasks, 4);

  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(result->tour.is_valid(map.size()));
}

TEST_F(ParallelRunnerBerlin52, AllStart_1Thread_vs_4Threads_MemeResultat) {
  std::vector<SolveOptions> tasks;
  for (const auto &city : map.getCities()) {
    SolveOptions o;
    o.start_city_id = static_cast<std::size_t>(city.getId());
    tasks.push_back(o);
  }

  const auto r1 = run_parallel(nn_factory(), map, tasks, 1);
  const auto r4 = run_parallel(nn_factory(), map, tasks, 4);

  ASSERT_TRUE(r1.has_value());
  ASSERT_TRUE(r4.has_value());
  // NN est déterministe par ville de départ : le meilleur coût doit être identique
  EXPECT_DOUBLE_EQ(r1->cost, r4->cost);
}

// --- Parallélisation de repeat ---

TEST_F(ParallelRunnerBerlin52, Repeat_4Taches_2Threads_TourValide) {
  std::vector<SolveOptions> tasks(4, SolveOptions{});

  const auto result = run_parallel(nn_factory(), map, tasks, 2);

  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(result->tour.is_valid(map.size()));
}

// --- Avec 2-opt ---

TEST_F(ParallelRunnerBerlin52, TwoOpt_AllStart_4Threads_CoutInferieurNN) {
  auto twoopt_factory = []() -> std::unique_ptr<ISolver> {
    return std::make_unique<TwoOptSolver>();
  };

  std::vector<SolveOptions> tasks_nn, tasks_2opt;
  for (const auto &city : map.getCities()) {
    SolveOptions o;
    o.start_city_id = static_cast<std::size_t>(city.getId());
    tasks_nn.push_back(o);
    tasks_2opt.push_back(o);
  }

  const auto best_nn = run_parallel(nn_factory(), map, tasks_nn, 4);
  const auto best_2opt = run_parallel(twoopt_factory, map, tasks_2opt, 4);

  ASSERT_TRUE(best_nn.has_value());
  ASSERT_TRUE(best_2opt.has_value());
  EXPECT_LE(best_2opt->cost, best_nn->cost);
}
