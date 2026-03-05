#include "json_writer.hpp"
#include "solve_result.hpp"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace json {

// Échappe une string pour JSON.
static std::string escape(std::string_view s) {
  std::string out;
  out.reserve(s.size() + 8);

  for (char raw : s) {
    const unsigned char c = static_cast<unsigned char>(raw);
    switch (c) {
    case '\"':
      out += "\\\"";
      break;
    case '\\':
      out += "\\\\";
      break;
    case '\b':
      out += "\\b";
      break;
    case '\f':
      out += "\\f";
      break;
    case '\n':
      out += "\\n";
      break;
    case '\r':
      out += "\\r";
      break;
    case '\t':
      out += "\\t";
      break;
    default:
      if (c < 0x20) {
        static constexpr char hex[] = "0123456789ABCDEF";
        out += "\\u00";
        out += hex[(c >> 4) & 0xF];
        out += hex[c & 0xF];
      } else {
        out += static_cast<char>(c);
      }
    }
  }
  return out;
}

// Écrit un tableau JSON d'entiers
static void write_int_array(std::ostream &os, const std::vector<int> &v) {
  os << '[';
  for (std::size_t i = 0; i < v.size(); ++i) {
    if (i)
      os << ',';
    os << v[i];
  }
  os << ']';
}

// Petit writer JSON
class ObjectWriter {
public:
  explicit ObjectWriter(std::ostream &os, int indent = 2)
      : os_(os), indent_(indent) {}

  void begin_object() {
    os_ << "{\n";
    first_ = true;
  }

  void end_object() { os_ << "\n}\n"; }

  void key_string(std::string_view key, std::string_view value) {
    write_key(key);
    os_ << '\"' << escape(value) << '\"';
  }

  void key_number(std::string_view key, double value) {
    write_key(key);
    os_ << value;
  }

  void key_int(std::string_view key, long long value) {
    write_key(key);
    os_ << value;
  }

  void key_int_array(std::string_view key, const std::vector<int> &v) {
    write_key(key);
    write_int_array(os_, v);
  }

private:
  void write_key(std::string_view key) {
    if (!first_)
      os_ << ",\n";
    first_ = false;

    os_ << std::string(static_cast<std::size_t>(indent_), ' ') << '\"' << key
        << "\": ";
  }

  std::ostream &os_;
  int indent_;
  bool first_ = true;
};

} // namespace json

// ---------------------------------------------------------------------------

// Résout le chemin de sortie
static fs::path resolve_output_path(const fs::path &user_path) {
  if (fs::exists(user_path)) {
    return user_path;
  }

  // créer le dossier results si nécessaire
  fs::path results_dir = "results";
  if (!fs::exists(results_dir)) {
    fs::create_directories(results_dir);
  }

  // placer le fichier dans results/
  return results_dir / user_path.filename();
}

static std::ofstream open_or_throw(const fs::path &out_path) {
  std::ofstream out(out_path);
  if (!out.is_open()) {
    throw std::runtime_error("Impossible d'ouvrir le fichier JSON: " +
                             out_path.string());
  }
  return out;
}

static double micros_to_ms(std::chrono::microseconds us) {
  return static_cast<double>(us.count()) / 1000.0;
}

// ---------------------------------------------------------------------------

void write_solve_result_json(const SolveResult &r,
                             const fs::path &user_output_path) {

  fs::path out_path = resolve_output_path(user_output_path);

  auto out = open_or_throw(out_path);

  out << std::fixed << std::setprecision(6);

  json::ObjectWriter w(out);
  w.begin_object();

  w.key_string("file_name", r.file_name);
  w.key_string("algo_name", r.algo_name);

  w.key_number("cost", r.cost);
  w.key_number("time_ms", micros_to_ms(r.duration));
  w.key_int("distance_calls", r.distance_calls);

  w.key_int_array("best_tour", r.tour.getOrder());

  w.end_object();
}
