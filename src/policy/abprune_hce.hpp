#pragma once
#include "../state/state.hpp"

/**
 * @brief alpha-beta prune + minimax + handcrafted evaluation
 */
class ABPruneHCE {
    static std::pair<int, Move> _get_move(State *state, int depth, int alpha, int beta, bool isMax);

public:
    static Move get_move(State *state, int depth);
};