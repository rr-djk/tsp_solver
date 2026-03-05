#include <gtest/gtest.h>
#include <thread>
#include <vector>

#include "cli_parser.hpp"

/**
 * @brief Construit un argv depuis une liste littérale et appelle parse_args_or_exit.
 */
static CliOptions run(std::initializer_list<const char *> args) {
  std::vector<char *> argv;
  argv.reserve(args.size());
  for (const char *a : args)
    argv.push_back(const_cast<char *>(a));
  return parse_args_or_exit(static_cast<int>(argv.size()), argv.data());
}

// ---------------------------------------------------------------------------
// Cas valides
// ---------------------------------------------------------------------------

TEST(CliParserValid, Minimal_AlgoNn_DefaultsCorrects) {
  const CliOptions opts =
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json"});

  EXPECT_EQ(opts.input_file, "in.tsp");
  EXPECT_EQ(opts.algo, "nn");
  EXPECT_EQ(opts.output_file, "out.json");
  EXPECT_FALSE(opts.all_start);
  EXPECT_FALSE(opts.quiet);
  EXPECT_EQ(opts.repeat, 1);
  EXPECT_EQ(opts.threads, 1);
  EXPECT_FALSE(opts.time_limit_seconds.has_value());
}

TEST(CliParserValid, Algo2opt_Accepte) {
  const CliOptions opts =
      run({"tsp", "--input-file", "in.tsp", "--algo", "2opt", "--output-file",
           "out.json"});

  EXPECT_EQ(opts.algo, "2opt");
}

TEST(CliParserValid, AlgoInsertion_Accepte) {
  const CliOptions opts =
      run({"tsp", "--input-file", "in.tsp", "--algo", "insertion",
           "--output-file", "out.json"});

  EXPECT_EQ(opts.algo, "insertion");
}

TEST(CliParserValid, FlagAllStart_EstVrai) {
  const CliOptions opts =
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--all-start"});

  EXPECT_TRUE(opts.all_start);
}

TEST(CliParserValid, FlagQuiet_EstVrai) {
  const CliOptions opts =
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--quiet"});

  EXPECT_TRUE(opts.quiet);
}

TEST(CliParserValid, Repeat5_ParseCorrectement) {
  const CliOptions opts =
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--repeat", "5"});

  EXPECT_EQ(opts.repeat, 5);
}

TEST(CliParserValid, Threads1_ParseCorrectement) {
  const CliOptions opts =
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--threads", "1"});

  EXPECT_EQ(opts.threads, 1);
}

TEST(CliParserValid, TimeLimit10s_ParseCorrectement) {
  const CliOptions opts =
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--time-limit", "10s"});

  ASSERT_TRUE(opts.time_limit_seconds.has_value());
  EXPECT_EQ(opts.time_limit_seconds.value(), 10);
}

TEST(CliParserValid, TimeLimit120s_ParseCorrectement) {
  const CliOptions opts =
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--time-limit", "120s"});

  ASSERT_TRUE(opts.time_limit_seconds.has_value());
  EXPECT_EQ(opts.time_limit_seconds.value(), 120);
}

TEST(CliParserValid, OrdreDifferent_ParseCorrectement) {
  const CliOptions opts =
      run({"tsp", "--algo", "2opt", "--output-file", "out.json", "--input-file",
           "in.tsp"});

  EXPECT_EQ(opts.algo, "2opt");
  EXPECT_EQ(opts.input_file, "in.tsp");
  EXPECT_EQ(opts.output_file, "out.json");
}

TEST(CliParserValid, ToutesOptionsEnsemble_ParseCorrectement) {
  const CliOptions opts =
      run({"tsp", "--input-file", "data/berlin52.tsp", "--algo", "nn",
           "--output-file", "out.json", "--all-start", "--quiet", "--repeat",
           "3", "--time-limit", "30s"});

  EXPECT_EQ(opts.input_file, "data/berlin52.tsp");
  EXPECT_EQ(opts.algo, "nn");
  EXPECT_TRUE(opts.all_start);
  EXPECT_TRUE(opts.quiet);
  EXPECT_EQ(opts.repeat, 3);
  ASSERT_TRUE(opts.time_limit_seconds.has_value());
  EXPECT_EQ(opts.time_limit_seconds.value(), 30);
}

// ---------------------------------------------------------------------------
// Cas invalides – arguments obligatoires manquants
// ---------------------------------------------------------------------------

TEST(CliParserInvalid, AucunArgument_Exit1) {
  EXPECT_EXIT(run({"tsp"}), ::testing::ExitedWithCode(1), "");
}

TEST(CliParserInvalid, ManqueInputFile_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--algo", "nn", "--output-file", "out.json"}),
      ::testing::ExitedWithCode(1), "");
}

TEST(CliParserInvalid, ManqueAlgo_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--output-file", "out.json"}),
      ::testing::ExitedWithCode(1), "");
}

TEST(CliParserInvalid, ManqueOutputFile_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn"}),
      ::testing::ExitedWithCode(1), "");
}

// ---------------------------------------------------------------------------
// Cas invalides – algorithme
// ---------------------------------------------------------------------------

TEST(CliParserInvalid, AlgoInconnu_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo", "bfs", "--output-file",
           "out.json"}),
      ::testing::ExitedWithCode(1), "");
}

TEST(CliParserInvalid, AlgoCassesDifferentes_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo", "NN", "--output-file",
           "out.json"}),
      ::testing::ExitedWithCode(1), "");
}

// ---------------------------------------------------------------------------
// Cas invalides – option inconnue
// ---------------------------------------------------------------------------

TEST(CliParserInvalid, OptionInconnue_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--verbose"}),
      ::testing::ExitedWithCode(1), "");
}

TEST(CliParserInvalid, OptionAbregee_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--alg", "nn", "--output-file",
           "out.json"}),
      ::testing::ExitedWithCode(1), "");
}

// ---------------------------------------------------------------------------
// Cas invalides – options dupliquées
// ---------------------------------------------------------------------------

TEST(CliParserInvalid, InputFileDuplique_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "a.tsp", "--input-file", "b.tsp", "--algo",
           "nn", "--output-file", "out.json"}),
      ::testing::ExitedWithCode(1), "");
}

TEST(CliParserInvalid, AlgoDuplique_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--algo", "2opt",
           "--output-file", "out.json"}),
      ::testing::ExitedWithCode(1), "");
}

TEST(CliParserInvalid, AllStartDuplique_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--all-start", "--all-start"}),
      ::testing::ExitedWithCode(1), "");
}

// ---------------------------------------------------------------------------
// Cas invalides – valeur absente pour option à valeur
// ---------------------------------------------------------------------------

TEST(CliParserInvalid, InputFileSansValeur_Exit1) {
  EXPECT_EXIT(run({"tsp", "--input-file"}), ::testing::ExitedWithCode(1), "");
}

TEST(CliParserInvalid, AlgoSuiviOptionSuivante_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo", "--output-file",
           "out.json"}),
      ::testing::ExitedWithCode(1), "");
}

TEST(CliParserInvalid, RepeatSansValeur_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--repeat"}),
      ::testing::ExitedWithCode(1), "");
}

// ---------------------------------------------------------------------------
// Cas invalides – format avec '='
// ---------------------------------------------------------------------------

TEST(CliParserInvalid, FormatEgalPourThreads_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--threads=4"}),
      ::testing::ExitedWithCode(1), "");
}

TEST(CliParserInvalid, FormatEgalPourAlgo_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo=nn", "--output-file",
           "out.json"}),
      ::testing::ExitedWithCode(1), "");
}

// ---------------------------------------------------------------------------
// Cas invalides – --repeat
// ---------------------------------------------------------------------------

TEST(CliParserInvalid, RepeatZero_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--repeat", "0"}),
      ::testing::ExitedWithCode(1), "");
}

TEST(CliParserInvalid, RepeatNegatif_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--repeat", "-1"}),
      ::testing::ExitedWithCode(1), "");
}

TEST(CliParserInvalid, RepeatNonNumerique_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--repeat", "abc"}),
      ::testing::ExitedWithCode(1), "");
}

// ---------------------------------------------------------------------------
// Cas invalides – --time-limit
// ---------------------------------------------------------------------------

TEST(CliParserInvalid, TimeLimitZeroS_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--time-limit", "0s"}),
      ::testing::ExitedWithCode(1), "");
}

TEST(CliParserInvalid, TimeLimitMajusculeS_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--time-limit", "10S"}),
      ::testing::ExitedWithCode(1), "");
}

TEST(CliParserInvalid, TimeLimitSansSuffixe_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--time-limit", "10"}),
      ::testing::ExitedWithCode(1), "");
}

TEST(CliParserInvalid, TimeLimitSeulementS_Exit1) {
  EXPECT_EXIT(
      run({"tsp", "--input-file", "in.tsp", "--algo", "nn", "--output-file",
           "out.json", "--time-limit", "s"}),
      ::testing::ExitedWithCode(1), "");
}
