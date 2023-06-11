#include <cstdlib>

#include "../state/state.hpp"
#include "./greedy_pss.hpp"

/**
 * @brief Get a legal action w/ minimax policy
 *
 * @param state Now state
 * @param depth search depth
 * @return Move
 */
Move GreedyPSS::get_move(State *state, int depth) {
    if (!state->legal_actions.size())
        state->get_legal_actions();

    int max_value = depth - 100000;
    Move *max_action = nullptr;
    for (auto &action : state->legal_actions) {
        auto next_state = state->next_state(action);
        int value = next_state->evaluatePSS();
        if (value > max_value) {
            max_value = value;
            max_action = &action;
        }
    }
    return *max_action;
}