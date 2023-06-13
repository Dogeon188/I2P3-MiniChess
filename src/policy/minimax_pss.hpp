#pragma once
#include "../state/state.hpp"

/**
 * @brief Minimax + Piece Score Sum
 */
class MinimaxPSS {
    static std::pair<int, Move> _get_move(State *state, int depth, bool isMax = true);

public:
    static Move get_move(State *state, int depth);
};