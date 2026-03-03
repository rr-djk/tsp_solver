#include <fstream>
#include <gtest/gtest.h>

#include "map.hpp"

class MapBerlin52 : public ::testing::Test {
protected:
  static Map map;

  static void SetUpTestSuite() {
    std::ifstream f("data/berlin52.tsp");
    ASSERT_TRUE(f.is_open()) << "Impossible d'ouvrir data/berlin52.tsp";
    f >> map;
  }
};

Map MapBerlin52::map;

TEST_F(MapBerlin52, TailleEgale52) { EXPECT_EQ(map.getCities().size(), 52u); }

struct CityData {
  int id;
  double x, y;
};

TEST_F(MapBerlin52, CinqPremieresCities) {
  const CityData expected[] = {
      {1, 565.0, 575.0},
      {2, 25.0, 185.0},
      {3, 345.0, 750.0},
      {4, 945.0, 685.0},
      {5, 845.0, 655.0},
  };
  const auto &cities = map.getCities();
  for (size_t i = 0; i < 5; ++i) {
    EXPECT_EQ(cities[i].getId(), expected[i].id);
    EXPECT_DOUBLE_EQ(cities[i].getCoordX(), expected[i].x);
    EXPECT_DOUBLE_EQ(cities[i].getCoordY(), expected[i].y);
  }
}

TEST_F(MapBerlin52, CinqDernieresCities) {
  const CityData expected[] = {
      {48, 830.0, 610.0},
      {49, 605.0, 625.0},
      {50, 595.0, 360.0},
      {51, 1340.0, 725.0},
      {52, 1740.0, 245.0},
  };
  const auto &cities = map.getCities();
  size_t n = cities.size();
  for (size_t i = 0; i < 5; ++i) {
    EXPECT_EQ(cities[n - 5 + i].getId(), expected[i].id);
    EXPECT_DOUBLE_EQ(cities[n - 5 + i].getCoordX(), expected[i].x);
    EXPECT_DOUBLE_EQ(cities[n - 5 + i].getCoordY(), expected[i].y);
  }
}
