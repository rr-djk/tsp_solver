#ifndef TWO_OPT_SOLVER_HPP
#define TWO_OPT_SOLVER_HPP

#include "isolver.hpp"
#include "solve_options.hpp"
#include "solve_result.hpp"

class TwoOptSolver : public ISolver {
public:
  SolveResult solve(const Map &map, const SolveOptions &options) override;
};

#endif
