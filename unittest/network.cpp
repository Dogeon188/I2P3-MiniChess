#include "../src/config.hpp"
#include "../src/policy/nnue_network.hpp"
#include <cstdlib>
#include <iostream>
#include <chrono>

int main() {
    srand(time(NULL));

    // generate a random network
    NNUE::Network network0;
    network0.randomize();
    std::ofstream fout("log/sus.nnue", std::ios::binary);
    network0.write(fout);
    fout.close();

    NNUE::Network network;
    std::ifstream fin("log/sus.nnue", std::ios::binary);
    network.load(fin);
    fin.close();

    std::vector<float> input(NNUE::INPUT_SIZE);
    for (int i = 0; i < NNUE::INPUT_SIZE; i++) {
        input[i] = ((float)rand() / RAND_MAX) > 0.5;
    }
    std::cout << "Input: ";
    for (int i = 0; i < NNUE::INPUT_SIZE; i++) {
        std::cout << input[i] << " ";
    }
    std::cout << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    auto output = network.evaluate(input);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Output: " << output << " " << std::endl;
    auto output0 = network0.evaluate(input);
    std::cout << "Output0: " << output0 << " " << std::endl;
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns" << std::endl;


    return 0;
}