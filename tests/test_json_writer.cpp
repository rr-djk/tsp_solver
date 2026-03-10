#include <chrono>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>
#include <string>

#include "cli_options.hpp"
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
                     42, 0, 0};
}

static CliOptions make_test_opts() {
  CliOptions opts;
  opts.algo = "nn";
  opts.threads = 1;
  opts.repeat = 1;
  opts.all_start = false;
  return opts;
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

  write_solve_result_json(make_test_result(), make_test_opts(), output_path);

  const std::string content = read_file(output_path);
  EXPECT_NE(content.find("\"status\": \"ok\""), std::string::npos);
  EXPECT_NE(content.find("\"algo\": \"nn\""), std::string::npos);
  EXPECT_NE(content.find("\"file_name\": \"berlin52.tsp\""), std::string::npos);
  EXPECT_NE(content.find("\"algo_name\": \"nearest_neighbor\""),
            std::string::npos);
  EXPECT_NE(content.find("\"cost\": 1234.500000"), std::string::npos);
  EXPECT_NE(content.find("\"time_ms\": 2.500000"), std::string::npos);
  EXPECT_NE(content.find("\"distance_calls\": 42"), std::string::npos);
  EXPECT_NE(content.find("[0,1,2]"), std::string::npos);
}

TEST_F(JsonWriterTest, StatusTimeout_EcritDansJSON) {
  const fs::path output_path = "test_output.json";
  std::ofstream(output_path).close();

  SolveResult r = make_test_result();
  r.status = SolveStatus::timeout;
  write_solve_result_json(r, make_test_opts(), output_path);

  const std::string content = read_file(output_path);
  EXPECT_NE(content.find("\"status\": \"timeout\""), std::string::npos);
}

TEST_F(JsonWriterTest, StatusError_EcritDansJSON) {
  const fs::path output_path = "test_output.json";
  std::ofstream(output_path).close();

  SolveResult r = make_test_result();
  r.status = SolveStatus::error;
  write_solve_result_json(r, make_test_opts(), output_path);

  const std::string content = read_file(output_path);
  EXPECT_NE(content.find("\"status\": \"error\""), std::string::npos);
}

TEST_F(JsonWriterTest, ConfigSection_ContientChampsCLI) {
  const fs::path output_path = "test_output.json";
  std::ofstream(output_path).close();

  CliOptions opts = make_test_opts();
  opts.threads = 4;
  opts.repeat = 3;
  opts.all_start = true;
  opts.time_limit_seconds = 10;
  write_solve_result_json(make_test_result(), opts, output_path);

  const std::string content = read_file(output_path);
  EXPECT_NE(content.find("\"threads\": 4"), std::string::npos);
  EXPECT_NE(content.find("\"repeat\": 3"), std::string::npos);
  EXPECT_NE(content.find("\"all_start\": true"), std::string::npos);
  EXPECT_NE(content.find("\"time_limit_s\": 10"), std::string::npos);
}

TEST_F(JsonWriterTest, ConfigSection_SansTimeLimitAbsent) {
  const fs::path output_path = "test_output.json";
  std::ofstream(output_path).close();

  write_solve_result_json(make_test_result(), make_test_opts(), output_path);

  const std::string content = read_file(output_path);
  EXPECT_EQ(content.find("\"time_limit_s\""), std::string::npos);
}

TEST_F(JsonWriterTest, FichierInexistant_CreeDossierResultsEtEcrit) {
  fs::remove("results/test_auto.json");
  if (fs::exists("results") && fs::is_empty("results"))
    fs::remove("results");

  ASSERT_FALSE(fs::exists("results/test_auto.json"));

  write_solve_result_json(make_test_result(), make_test_opts(), "test_auto.json");

  EXPECT_TRUE(fs::exists("results/test_auto.json"));
  const std::string content = read_file("results/test_auto.json");
  EXPECT_NE(content.find("\"algo_name\": \"nearest_neighbor\""),
            std::string::npos);
}

TEST_F(JsonWriterTest, CheminInaccessible_LeveException) {
  fs::create_directory("test_error_dir.json");

  EXPECT_THROW(
      write_solve_result_json(make_test_result(), make_test_opts(), "test_error_dir.json"),
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
                0, 0, 0};
  write_solve_result_json(r, make_test_opts(), output_path);

  const std::string content = read_file(output_path);
  EXPECT_NE(content.find("file\\\"quoted\\\""), std::string::npos);
  EXPECT_NE(content.find("algo\\\\slash"), std::string::npos);
}
