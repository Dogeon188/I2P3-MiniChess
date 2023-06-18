#include "../state/state.hpp"
#include "./abprune_hce.hpp"

#define NEGINF -1000000
#define POSINF 1000000

/**
 * @brief Internal search function for minimax
 *
 * @param state Now state
 * @param depth search depth limit
 * @param isMax is maximizing player aka our AI
 * @param alpha alpha, best value for maximizing player
 * @param beta beta, best value for minimizing player
 *
 * @return pair(stateValue, move)
 */
std::pair<int, Move> ABPruneHCE::_get_move(State *state, int depth, int alpha, int beta, bool isMax) {
    if (!state->legal_actions.size())
        state->get_legal_actions();

    if (state->game_state == WIN) {
        return std::make_pair(isMax ? POSINF : NEGINF, state->legal_actions.back());
    }
    // we don't quite want draw
    if (state->game_state == DRAW) {
        return std::make_pair(isMax ? -5000 : 5000, state->legal_actions.back());
    }
    if (depth == 0) {
        // negative when calculation is based on opponent
        // use HCE to evaluate
        return std::make_pair(state->evaluateHCE() * (isMax ? 1 : -1), state->legal_actions.back());
    }

    int best_value = isMax ? NEGINF : POSINF;
    Move best_move = state->legal_actions[0];   // avoid output 0 0 0 0
    if (isMax) {
        // reverse iterate since win move is put at last
        for (auto it = state->legal_actions.rbegin(); it != state->legal_actions.rend(); it++) {
            auto next_state = state->next_state(*it);
            auto value = _get_move(next_state, depth - 1, alpha, beta, !isMax).first;
            delete next_state;
            if (value > best_value) {
                best_value = value;
                best_move = *it;
            }
            if (best_value > alpha)
                alpha = best_value;
            if (alpha >= beta)
                return std::make_pair(best_value, best_move);   // alpha cut-off
        }
    } else {
        // non-reverse iterate
        for (auto &action : state->legal_actions) {
            auto next_state = state->next_state(action);
            auto value = _get_move(next_state, depth - 1, alpha, beta, !isMax).first;
            delete next_state;
            if (value < best_value) {
                best_value = value;
                best_move = action;
            }
            if (best_value < beta)
                beta = best_value;
            if (beta <= alpha)
                return std::make_pair(best_value, best_move);   // beta cut-off
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
Move ABPruneHCE::get_move(State *state, int depth) {
    return _get_move(state, depth, NEGINF, POSINF, true).second;
}