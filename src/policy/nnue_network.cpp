#include "nnue_network.hpp"
#include <iostream>

void NNUE::Network::load(std::ifstream &fin) {
    layer1.load(fin);
    layer2.load(fin);
    layer3.load(fin);
    layer4.load(fin);
}

void NNUE::Network::write(std::ofstream &fout) const {
    layer1.write(fout);
    layer2.write(fout);
    layer3.write(fout);
    layer4.write(fout);
}

float& NNUE::Network::evaluate(const std::vector<float> &input) {
    layer1.feed(input);
    layer2.feed(layer1.output);
    layer3.feed(layer2.output);
    layer4.feed(layer3.output);
    return output;
}

void NNUE::Network::randomize() {
    layer1.randomize();
    layer2.randomize();
    layer3.randomize();
    layer4.randomize();
}