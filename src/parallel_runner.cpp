#include "parallel_runner.hpp"

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <thread>
#include <vector>

std::optional<SolveResult>
run_parallel(std::function<std::unique_ptr<ISolver>()> make_solver,
             const Map &map, const std::vector<SolveOptions> &tasks,
             int num_threads) {
  if (tasks.empty())
    return std::nullopt;

  const std::size_t n = tasks.size();
  std::vector<std::optional<SolveResult>> results(n);
  std::atomic<std::size_t> next_task{0};

  /**
   * @brief Chaque worker récupère la prochaine tâche disponible via l'index
   *        atomique, exécute le solveur et écrit dans son slot dédié.
   *        Aucun mutex nécessaire : chaque index est alloué à un seul thread.
   */
  auto worker = [&]() {
    auto solver = make_solver();
    while (true) {
      const std::size_t idx =
          next_task.fetch_add(1, std::memory_order_relaxed);
      if (idx >= n)
        break;
      results[idx] = solver->solve(map, tasks[idx]);
    }
  };

  const int actual_threads =
      std::min(num_threads, static_cast<int>(n));
  std::vector<std::jthread> threads;
  threads.reserve(static_cast<std::size_t>(actual_threads));
  for (int i = 0; i < actual_threads; ++i)
    threads.emplace_back(worker);
  threads.clear(); // join implicite via ~jthread

  std::optional<SolveResult> best;
  for (auto &r : results) {
    if (r.has_value() && (!best.has_value() || r->cost < best->cost))
      best = std::move(r);
  }
  return best;
}
