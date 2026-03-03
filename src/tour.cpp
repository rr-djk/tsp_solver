#include "tour.hpp"
#include <cstddef>

Tour::Tour(std::vector<int> order_) : order(std::move(order_)) {}

std::size_t Tour::size() const { return order.size(); }

const std::vector<int> &Tour::getOrder() const { return order; }

bool Tour::is_valid(std::size_t n) const {
  if (size() != n)
    return false;

  std::vector<bool> seen(n, false);
  for (int city : order) {
    if (city < 0 || static_cast<std::size_t>(city) >= n)
      return false;
    if (seen[static_cast<std::size_t>(city)])
      return false;
    seen[static_cast<std::size_t>(city)] = true;
  }

  return true;
}
