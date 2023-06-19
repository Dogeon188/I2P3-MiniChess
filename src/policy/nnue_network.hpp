#pragma once

#include "../state/state.hpp"
#include "nnue_engine.hpp"
#include <fstream>

namespace NNUE {
    class Network {
        static NNUE::Network instance;

        // a -> ours, b -> theirs
        Vector input1a, input1b;
        Vector inputDiff1a, inputDiff1b;
        Layer layer1a, layer1b;
        Layer layer2, layer3, layer4;
        float &output = layer4.output[0];
        float &_evaluate();

        Network() : input1a(INPUT_SIZE), input1b(INPUT_SIZE),
                    inputDiff1a(INPUT_SIZE), inputDiff1b(INPUT_SIZE),
                    layer1a(INPUT_SIZE, LAYER1_SIZE),
                    layer1b(INPUT_SIZE, LAYER1_SIZE),
                    layer2(LAYER1_SIZE * 2, LAYER2_SIZE),
                    layer3(LAYER2_SIZE, LAYER3_SIZE),
                    layer4(LAYER3_SIZE, OUTPUT_SIZE) {}

    public:
        static const int INPUT_SIZE = 9000;   // two input part
        static const int LAYER1_SIZE = 256;
        static const int LAYER2_SIZE = 32;
        static const int LAYER3_SIZE = 32;
        static const int OUTPUT_SIZE = 1;

        void load(std::ifstream &filename);
        void write(std::ofstream &filename) const;
        void randomize();

        float evaluate(const State *state);

        static NNUE::Network &getInstance() { return instance; }
    };
}