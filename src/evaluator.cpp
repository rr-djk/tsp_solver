#include "evaluator.hpp"
#include "map.hpp"
#include "tour.hpp"
#include <cstddef>
#include <stdexcept>

double cost(const Map &map, const Tour &tour) {
  const std::size_t n = map.size();

  if (n <= 1) {
    return 0.0;
  }

  if (!tour.is_valid(n)) {
    throw std::invalid_argument("cost: tour invalide pour cette Map");
  }

  const auto &ord = tour.getOrder();
  double sum = 0.0;

  for (std::size_t i = 0; i < n - 1; ++i) {
    sum += map.distance(static_cast<std::size_t>(ord[i]),
                        static_cast<std::size_t>(ord[i + 1]));
  }

  // retour implicite
  sum += map.distance(static_cast<std::size_t>(ord[n - 1]),
                      static_cast<std::size_t>(ord[0]));

  return sum;
}
