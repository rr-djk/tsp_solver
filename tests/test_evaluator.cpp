#include <algorithm>
#include <fstream>
#include <gtest/gtest.h>
#include <numeric>
#include <print>
#include <random>

#include "evaluator.hpp"
#include "map.hpp"
#include "tour.hpp"

class EvaluatorBerlin52 : public ::testing::Test {
protected:
  static Map map;

  static void SetUpTestSuite() {
    std::ifstream f("data/berlin52.tsp");
    ASSERT_TRUE(f.is_open()) << "Impossible d'ouvrir data/berlin52.tsp";
    f >> map;
  }
};

Map EvaluatorBerlin52::map;

TEST_F(EvaluatorBerlin52, TourAleatoire_CoutPositif) {
  std::size_t n = map.size();
  std::vector<int> order(n);
  std::iota(order.begin(), order.end(), 0);
  std::shuffle(order.begin(), order.end(), std::mt19937{42});

  Tour tour(order);
  double total = cost(map, tour);

  EXPECT_GT(total, 0.0);
}

TEST_F(EvaluatorBerlin52, TourAleatoire_Affichage) {
  std::size_t n = map.size();
  std::vector<int> order(n);
  std::iota(order.begin(), order.end(), 0);
  std::shuffle(order.begin(), order.end(), std::mt19937{42});

  Tour tour(order);
  double total = cost(map, tour);

  const auto &cities = map.getCities();
  const auto &ord = tour.getOrder();

  for (std::size_t i = 0; i < n; ++i)
    std::print("{}{}", cities[static_cast<std::size_t>(ord[i])].getId(),
               i + 1 < n ? " -> " : "\n");

  std::println("Cout total : {:.2f}", total);

  SUCCEED();
}
