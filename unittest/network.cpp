#include "../src/config.hpp"
#include "../src/policy/nnue_network.hpp"
#include <cstdlib>
#include <iostream>
#include <chrono>

int main() {
    srand(time(NULL));

    // generate a random network
    NNUE::Network::getInstance().randomize();

    State state;

    std::cout << state.encode_output() << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    auto output = NNUE::Network::getInstance().evaluate(&state);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Output: " << output << " " << std::endl;
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns" << std::endl;
    return 0;
}