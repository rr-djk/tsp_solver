#include <chrono>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>
#include <string>

#include "json_writer.hpp"
#include "solve_result.hpp"
#include "tour.hpp"

namespace fs = std::filesystem;

static SolveResult make_test_result() {
  Tour tour({0, 1, 2});
  return SolveResult{"berlin52.tsp",
                     "nearest_neighbor",
                     std::move(tour),
                     1234.5,
                     std::chrono::microseconds{2500},
                     42};
}

static std::string read_file(const fs::path &path) {
  std::ifstream f(path);
  std::ostringstream ss;
  ss << f.rdbuf();
  return ss.str();
}

class JsonWriterTest : public ::testing::Test {
protected:
  void TearDown() override {
    fs::remove("test_output.json");
    fs::remove("results/test_auto.json");
    fs::remove("test_error_dir.json");
    if (fs::exists("results") && fs::is_empty("results"))
      fs::remove("results");
  }
};

TEST_F(JsonWriterTest, FichierExistant_ChampsCorrects) {
  const fs::path output_path = "test_output.json";
  std::ofstream(output_path).close();

  write_solve_result_json(make_test_result(), output_path);

  const std::string content = read_file(output_path);
  EXPECT_NE(content.find("\"file_name\": \"berlin52.tsp\""), std::string::npos);
  EXPECT_NE(content.find("\"algo_name\": \"nearest_neighbor\""),
            std::string::npos);
  EXPECT_NE(content.find("\"cost\": 1234.500000"), std::string::npos);
  EXPECT_NE(content.find("\"time_ms\": 2.500000"), std::string::npos);
  EXPECT_NE(content.find("\"distance_calls\": 42"), std::string::npos);
  EXPECT_NE(content.find("[0,1,2]"), std::string::npos);
}

TEST_F(JsonWriterTest, FichierInexistant_CreeDossierResultsEtEcrit) {
  fs::remove("results/test_auto.json");
  if (fs::exists("results") && fs::is_empty("results"))
    fs::remove("results");

  ASSERT_FALSE(fs::exists("results/test_auto.json"));

  write_solve_result_json(make_test_result(), "test_auto.json");

  EXPECT_TRUE(fs::exists("results/test_auto.json"));
  const std::string content = read_file("results/test_auto.json");
  EXPECT_NE(content.find("\"algo_name\": \"nearest_neighbor\""),
            std::string::npos);
}

TEST_F(JsonWriterTest, CheminInaccessible_LeveException) {
  fs::create_directory("test_error_dir.json");

  EXPECT_THROW(
      write_solve_result_json(make_test_result(), "test_error_dir.json"),
      std::runtime_error);
}

TEST_F(JsonWriterTest, CaracteresSpeciauxDansNom_EchappesCorrectement) {
  const fs::path output_path = "test_output.json";
  std::ofstream(output_path).close();

  Tour tour({0, 1});
  SolveResult r{"file\"quoted\".tsp",
                "algo\\slash",
                std::move(tour),
                0.0,
                std::chrono::microseconds{0},
                0};
  write_solve_result_json(r, output_path);

  const std::string content = read_file(output_path);
  EXPECT_NE(content.find("file\\\"quoted\\\""), std::string::npos);
  EXPECT_NE(content.find("algo\\\\slash"), std::string::npos);
}
