#pragma once
#include "../state/state.hpp"

/**
 * @brief alpha-beta prune + minimax + NNUE evaluation
 */
class ABPruneNNUE {
    static float _evaluate(State *state, int depth, int alpha, int beta, bool isMax);

public:
    static Move get_move(State *state, int depth);
};