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

    // find min instead of max since the state evaluation is of opponent
    int min_value = depth + 100000;
    Move *min_action = &state->legal_actions[0];
    for (auto &action : state->legal_actions) {
        auto next_state = state->next_state(action);
        int value = next_state->evaluatePSS();
        delete next_state;
        if (value < min_value) {
            min_value = value;
            min_action = &action;
        }
    }
    return *min_action;
}