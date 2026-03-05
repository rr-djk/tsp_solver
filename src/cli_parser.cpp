#include "cli_parser.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_set>

namespace {

[[noreturn]] void die(std::string_view error_message) {
  std::cerr << "error: " << error_message << "\n";
  std::exit(1);
}

[[noreturn]] void die_unknown_option(std::string_view option_name) {
  std::cerr << "error: option inconnue '" << option_name << "'\n";
  std::cerr << "hint: utilise --help\n";
  std::exit(1);
}

[[noreturn]] void die_duplicate_option(std::string_view option_name) {
  std::cerr << "error: option dupliquee '" << option_name << "'\n";
  std::exit(1);
}

[[noreturn]] void die_missing_value(std::string_view option_name) {
  std::cerr << "error: '" << option_name << "' attend une valeur\n";
  std::exit(1);
}

bool is_supported_algo(std::string_view algo_name) {
  static const std::unordered_set<std::string_view> supported_algos{
      "nn", "2opt", "insertion"};
  return supported_algos.contains(algo_name);
}

/**
 * @brief Refuse le format --option=valeur.
 * @param argument Argument CLI à inspecter.
 * NOTE: Termine avec exit(1) si un '=' est détecté.
 */
void reject_equals_form(std::string_view argument) {
  if (argument.find('=') != std::string_view::npos)
    die(std::string("format non supporte: '") + std::string(argument) +
        "' (utilise '--option valeur')");
}

unsigned int compute_max_threads() {
  const unsigned int hardware_thread_count =
      std::thread::hardware_concurrency();
  return hardware_thread_count > 0 ? hardware_thread_count : 1u;
}

/**
 * @brief Parse une chaîne de chiffres en entier strictement positif.
 * @param digits Chaîne à parser (uniquement des chiffres, sans signe).
 * @param option_name Nom de l'option, utilisé dans les messages d'erreur.
 * @return Entier parsé, >= 1.
 * NOTE: Termine avec exit(1) si la chaîne est vide, contient un non-chiffre,
 *       dépasse 2 000 000 000 ou vaut 0.
 */
int parse_strictly_positive_int(std::string_view digits,
                                std::string_view option_name) {
  if (digits.empty())
    die(std::string("'") + std::string(option_name) + "' valeur vide");

  long long parsed_value = 0;
  for (char digit_char : digits) {
    if (digit_char < '0' || digit_char > '9')
      die(std::string("'") + std::string(option_name) +
          "' attend un entier positif (ex: 4)");
    parsed_value = parsed_value * 10 + (digit_char - '0');
    if (parsed_value > 2'000'000'000LL)
      die(std::string("'") + std::string(option_name) + "' valeur trop grande");
  }

  if (parsed_value < 1)
    die(std::string("'") + std::string(option_name) + "' doit etre >= 1");

  return static_cast<int>(parsed_value);
}

/**
 * @brief Parse une valeur de limite de temps au format strict Ns.
 * @param raw_value Valeur brute lue depuis argv (ex: "10s").
 * @return Nombre de secondes (>= 1).
 * NOTE: Termine avec exit(1) si le format est invalide (manque 's',
 *       majuscule, espace, N <= 0, etc.).
 */
int parse_time_limit_seconds(std::string_view raw_value) {
  if (raw_value.size() < 2 || raw_value.back() != 's')
    die(std::string("--time-limit invalide '") + std::string(raw_value) +
        "' (attendu: Ns, ex: 10s)");

  return parse_strictly_positive_int(raw_value.substr(0, raw_value.size() - 1),
                                     "--time-limit");
}

} // namespace

CliOptions parse_args_or_exit(int argc, char **argv) {
  CliOptions parsed_options;
  std::unordered_set<std::string_view> seen_options;

  auto mark_seen_or_die = [&](std::string_view option_name) {
    if (seen_options.contains(option_name))
      die_duplicate_option(option_name);
    seen_options.insert(option_name);
  };

  if (argc <= 1)
    die("arguments manquants (utilise --help)");

  const unsigned int max_thread_count = compute_max_threads();

  for (int arg_index = 1; arg_index < argc; ++arg_index) {
    const std::string_view current_arg = argv[arg_index];

    reject_equals_form(current_arg);

    if (current_arg == "--all-start") {
      mark_seen_or_die(current_arg);
      parsed_options.all_start = true;
      continue;
    }

    if (current_arg == "--quiet") {
      mark_seen_or_die(current_arg);
      parsed_options.quiet = true;
      continue;
    }

    auto consume_next_value =
        [&](std::string_view option_name) -> std::string_view {
      if (arg_index + 1 >= argc)
        die_missing_value(option_name);
      const std::string_view next_arg = argv[arg_index + 1];
      if (next_arg.starts_with("--"))
        die_missing_value(option_name);
      ++arg_index;
      return next_arg;
    };

    if (current_arg == "--input-file") {
      mark_seen_or_die(current_arg);
      parsed_options.input_file = std::string(consume_next_value(current_arg));
      continue;
    }

    if (current_arg == "--output-file") {
      mark_seen_or_die(current_arg);
      parsed_options.output_file = std::string(consume_next_value(current_arg));
      continue;
    }

    if (current_arg == "--algo") {
      mark_seen_or_die(current_arg);
      const std::string_view algo_name = consume_next_value(current_arg);
      if (!is_supported_algo(algo_name))
        die(std::string("algo inconnu '") + std::string(algo_name) +
            "' (supportes: nn, 2opt, insertion)");
      parsed_options.algo = std::string(algo_name);
      continue;
    }

    if (current_arg == "--repeat") {
      mark_seen_or_die(current_arg);
      parsed_options.repeat = parse_strictly_positive_int(
          consume_next_value(current_arg), "--repeat");
      continue;
    }

    if (current_arg == "--threads") {
      mark_seen_or_die(current_arg);
      const int requested_thread_count = parse_strictly_positive_int(
          consume_next_value(current_arg), "--threads");
      if (static_cast<unsigned int>(requested_thread_count) > max_thread_count)
        die("--threads doit etre dans [1.." + std::to_string(max_thread_count) +
            "], recu: " + std::to_string(requested_thread_count));
      parsed_options.threads = requested_thread_count;
      continue;
    }

    if (current_arg == "--time-limit") {
      mark_seen_or_die(current_arg);
      parsed_options.time_limit_seconds =
          parse_time_limit_seconds(consume_next_value(current_arg));
      continue;
    }

    die_unknown_option(current_arg);
  }

  if (parsed_options.input_file.empty())
    die("option obligatoire manquante: --input-file");
  if (parsed_options.algo.empty())
    die("option obligatoire manquante: --algo");
  if (parsed_options.output_file.empty())
    die("option obligatoire manquante: --output-file");

  return parsed_options;
}
