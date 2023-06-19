#include <chrono>
#include <cstdlib>
#include <iostream>

#include "../src/config.hpp"
#include "../src/state/state.hpp"

#define OUTER_LOOP 500
#define INNER_LOOP 20

int main() {
    int64_t sum = 0;

    for (int i = 0; i < OUTER_LOOP; i++) {
        srand(RANDOM_SEED);
        State *state = new State();
        for (int i = 0; i < INNER_LOOP; i++) {
            if (state->legal_actions.size() == 0)
                state->get_legal_actions();
            int maxscore = 0;
            for (auto &action : state->legal_actions) {
                auto next_state = state->next_state(action);

                auto start = std::chrono::high_resolution_clock::now();

                maxscore = std::max((float)maxscore, next_state->evaluateHCE());

                auto end = std::chrono::high_resolution_clock::now();
                sum += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

                delete next_state;
            }
            auto next_state = state->next_state(state->legal_actions[rand() % state->legal_actions.size()]);
            delete state;
            state = next_state;
        }
        delete state;
    }
    std::cout << "HCE: " << sum / OUTER_LOOP << "ns" << std::endl;

    sum = 0;
    for (int i = 0; i < OUTER_LOOP; i++) {
        srand(RANDOM_SEED);
        State *state = new State();
        for (int i = 0; i < INNER_LOOP; i++) {
            if (state->legal_actions.size() == 0)
                state->get_legal_actions();
            int maxscore = 0;
            for (auto &action : state->legal_actions) {
                auto next_state = state->next_state(action);

                auto start = std::chrono::high_resolution_clock::now();

                maxscore = std::max(maxscore, next_state->evaluatePSS());

                auto end = std::chrono::high_resolution_clock::now();
                sum += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

                delete next_state;
            }
            auto next_state = state->next_state(state->legal_actions[rand() % state->legal_actions.size()]);
            delete state;
            state = next_state;
        }
        delete state;
    }

    std::cout << "PSS: " << sum / OUTER_LOOP << "ns" << std::endl;

    return 0;
}