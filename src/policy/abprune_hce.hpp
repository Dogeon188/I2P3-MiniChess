#pragma once
#include "../state/state.hpp"

/**
 * @brief alpha-beta prune + minimax + handcrafted evaluation
 */
class ABPruneHCE {
    static int _evaluate(State *state, int depth, int alpha, int beta, bool isMax);

public:
    static Move get_move(State *state, int depth);
};