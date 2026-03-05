#ifndef NEAREST_NEIGHBOR_SOLVER_HPP
#define NEAREST_NEIGHBOR_SOLVER_HPP

#include "isolver.hpp"
#include "solve_options.hpp"
#include "solve_result.hpp"

class NearestNeighborSolver : public ISolver {
public:
  SolveResult solve(const Map &map, const SolveOptions &options) override;
};

#endif
