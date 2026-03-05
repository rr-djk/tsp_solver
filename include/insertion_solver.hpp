#ifndef INSERTION_SOLVER_HPP
#define INSERTION_SOLVER_HPP

#include "isolver.hpp"
#include "solve_options.hpp"
#include "solve_result.hpp"

class InsertionSolver : public ISolver {
public:
  SolveResult solve(const Map &map, const SolveOptions &options) override;
};

#endif
