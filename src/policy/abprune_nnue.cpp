#include "./abprune_nnue.hpp"
#include "../state/state.hpp"

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
 * @return stateValue
 */
float ABPruneNNUE::_evaluate(State *state, int depth, int alpha, int beta, bool isMax) {
    if (!state->legal_actions.size()) state->get_legal_actions();

    if (state->game_state == WIN)
        return isMax ? POSINF : NEGINF;
    // we don't quite want draw
    if (state->game_state == DRAW)
        return isMax ? -5000 : 5000;
    // negative when calculation is based on opponent
    // use HCE to evaluate
    if (depth == 0)
        return state->evaluateNNUE() * (isMax ? 1 : -1);

    int best_value = isMax ? NEGINF : POSINF;
    // Move best_move = state->legal_actions[0];   // avoid output 0 0 0 0
    if (isMax) {
        // reverse iterate since win move is put at last
        for (auto it = state->legal_actions.rbegin(); it != state->legal_actions.rend(); it++) {
            auto next_state = state->next_state(*it);
            auto value = _evaluate(next_state, depth - 1, alpha, beta, !isMax);
            delete next_state;
            if (value > best_value) best_value = value;
            if (best_value > alpha) alpha = best_value;
            if (alpha >= beta)
                return best_value;   // alpha cut-off
        }
    } else {
        // non-reverse iterate
        for (auto &action : state->legal_actions) {
            auto next_state = state->next_state(action);
            auto value = _evaluate(next_state, depth - 1, alpha, beta, !isMax);
            delete next_state;
            if (value < best_value) best_value = value;
            if (best_value < beta) beta = best_value;
            if (beta <= alpha)
                return best_value;   // beta cut-off
        }
    }
    return best_value;
}

/**
 * @brief Get a legal action w/ minimax policy
 *
 * @param state Now state
 * @param depth search depth limit
 * @return Move
 */
Move ABPruneNNUE::get_move(State *state, int depth) {
    if (!state->legal_actions.size())
        state->get_legal_actions();

    if (state->game_state == WIN) return state->legal_actions.back();
    if (state->game_state == DRAW) return state->legal_actions.back();

    int best_value = NEGINF;
    int alpha = NEGINF;
    int beta = POSINF;

    Move best_move = state->legal_actions[0];   // avoid output 0 0 0 0
    // reverse iterate since win move is put at last
    for (auto it = state->legal_actions.rbegin(); it != state->legal_actions.rend(); it++) {
        auto next_state = state->next_state(*it);
        auto value = _evaluate(next_state, depth - 1, alpha, beta, false);
        delete next_state;
        if (value > best_value) {
            best_value = value;
            best_move = *it;
        }
        if (best_value > alpha) alpha = best_value;
        if (alpha >= beta)
            return best_move;
    }
    return best_move;
}