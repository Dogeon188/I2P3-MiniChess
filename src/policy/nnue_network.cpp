#include "nnue_network.hpp"
#include <iostream>

NNUE::Network NNUE::Network::instance = NNUE::Network();

void NNUE::Network::load(std::ifstream &fin) {
    layer1a.load(fin);
    layer1b.load(fin);
    layer2.load(fin);
    layer3.load(fin);
    layer4.load(fin);
}

void NNUE::Network::write(std::ofstream &fout) const {
    layer1a.write(fout);
    layer1b.write(fout);
    layer2.write(fout);
    layer3.write(fout);
    layer4.write(fout);
}

float &NNUE::Network::_evaluate() {
    // TODO implement incremental computation
    layer1a.feed(input1a);
    layer1b.feed(input1b);
    static Vector layer2Input(LAYER1_SIZE * 2);
    for (int i = 0; i < LAYER1_SIZE; ++i) {
        layer2Input[i] = layer1a.output[i];
        layer2Input[i + LAYER1_SIZE] = layer1b.output[i];
    }
    layer2.feed(layer2Input);
    layer3.feed(layer2.output);
    layer4.feed(layer3.output);
    return output;
}

void NNUE::Network::randomize() {
    layer1a.randomize();
    layer1b.randomize();
    layer2.randomize();
    layer3.randomize();
    layer4.randomize();
}

inline int orient(int player, int x, int y) {
    if (player) return (4 - x) * 5 + (4 - y);
    else return x * 5 + y;
}
inline int getId(int player, int kx, int ky, int px, int py, int pType, int pSide) {
    int pIdx = (pType - 1) * 2 + pSide;
    return orient(player, kx, ky) * 30 + orient(player, px, py) * 6 + pIdx;
}

float NNUE::Network::evaluate(const State *state) {
    auto &ours = state->board.board[state->player];
    auto &oursK = state->board.king[state->player];
    auto &theirs = state->board.board[1 - state->player];
    auto &theirsK = state->board.king[1 - state->player];

    Vector newInput1a(INPUT_SIZE), newInput1b(INPUT_SIZE);
    int nowPiece;
    for (int i = 0; i < BOARD_H; ++i) {
        for (int j = 0; j < BOARD_W; ++j) {
            if ((nowPiece = ours[i][j]))
                newInput1a[getId(state->player, i, j, oursK.first, oursK.second, nowPiece, 0)] = 1;
            if ((nowPiece = theirs[i][j]))
                newInput1b[getId(state->player, i, j, theirsK.first, theirsK.second, nowPiece, 1)] = 1;
        }
    }
    for (int i = 0; i < INPUT_SIZE; ++i) {
        inputDiff1a[i] = (newInput1a[i] != input1a[i]);
        inputDiff1b[i] = (newInput1b[i] != input1b[i]);
    }
    input1a = newInput1a;
    input1b = newInput1b;
    return _evaluate();
}