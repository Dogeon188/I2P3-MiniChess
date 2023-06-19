#pragma once

#include "nnue_engine.hpp"
#include <fstream>

namespace NNUE {
    const int INPUT_SIZE = 20;
    const int LAYER1_SIZE = 64;
    const int LAYER2_SIZE = 64;
    const int LAYER3_SIZE = 10;
    const int OUTPUT_SIZE = 1;
    class Network {
        Layer layer1, layer2, layer3, layer4;
        float &output = layer4.output[0];

    public:
    // TODO incremental computation of layer 1
    // TODO 1st layer should have player & opponent split
        Network() : layer1(INPUT_SIZE, LAYER1_SIZE),
                    layer2(LAYER1_SIZE, LAYER2_SIZE),
                    layer3(LAYER2_SIZE, LAYER3_SIZE),
                    layer4(LAYER3_SIZE, OUTPUT_SIZE) {}

        void load(std::ifstream &filename);
        void write(std::ofstream &filename) const;
        float &evaluate(const std::vector<float> &input);
        void randomize();
    };
}