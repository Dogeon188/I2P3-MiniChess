#include <cstdlib>
#include <iostream>

#include "../src/config.hpp"
#include "../src/state/state.hpp"

int main() {
    srand(RANDOM_SEED);

    State state;
    std::cout << state.encode_output() << std::endl;
    state.get_legal_actions();
    for (auto move : state.legal_actions) {
        std::cout << move.first.first << " " << move.first.second << " " << move.second.first << " " << move.second.second << "\n";
        auto temp = state.next_state(move);
        if (temp != NULL) {
            std::cout << temp->encode_output();
        } else {
            std::cout << "NULL\n";
        }
    }
    return 0;
}