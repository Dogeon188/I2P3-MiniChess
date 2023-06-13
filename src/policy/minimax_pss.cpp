#include <cstdlib>

#include "../state/state.hpp"
#include "./minimax_pss.hpp"

/**
 * @brief Internal search function for minimax
 *
 * @param state Now state
 * @param depth search depth limit
 * @param isMax is maximizing player
 *
 * @return pair(stateValue, move)
 */
std::pair<int, Move> MinimaxPSS::_get_move(State *state, int depth, bool isMax) {
    if (depth == 0) {
        return std::make_pair(state->evaluatePSS(), Move());
    }

    if (!state->legal_actions.size())
        state->get_legal_actions();
    
    if (state->game_state == WIN) {
        if (isMax)
            return std::make_pair(100000, state->legal_actions.back());
        else
            return std::make_pair(-100000, state->legal_actions.back());
    }

    int best_value = isMax ? -100000 : 100000;
    Move best_move;
    for (auto &action : state->legal_actions) {
        auto next_state = state->next_state(action);
        auto value = _get_move(next_state, depth - 1, !isMax).first;
        if (isMax) {
            if (value > best_value) {
                best_value = value;
                best_move = action;
            }
        } else {
            if (value < best_value) {
                best_value = value;
                best_move = action;
            }
        }
    }
    return std::make_pair(best_value, best_move);
}

/**
 * @brief Get a legal action w/ minimax policy
 *
 * @param state Now state
 * @param depth search depth limit
 * @return Move
 */
Move MinimaxPSS::get_move(State *state, int depth) {
    return _get_move(state, depth, true).second;
}