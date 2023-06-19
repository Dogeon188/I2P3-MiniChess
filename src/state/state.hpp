#ifndef __STATE_H__
#define __STATE_H__

#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

#include "../config.hpp"
typedef std::pair<int64_t, int64_t> Point;
typedef std::pair<Point, Point> Move;
class Board {
public:
    Point king[2] = { { 5, 4 }, { 0, 0 } };
    char board[2][BOARD_H][BOARD_W] = {
        {
            // white
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 1, 1, 1, 1, 1 },
            { 2, 3, 4, 5, 6 },
        },
        {
            // black
            { 6, 5, 4, 3, 2 },
            { 1, 1, 1, 1, 1 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0 },
        }
    };
};

enum GameState {
    UNKNOWN = 0,
    WIN,
    DRAW,
    NONE
};

class State {
    int _evaluateKingThreat(int player);

public:
    // You may want to add more property for a state
    GameState game_state = UNKNOWN;
    Board board;
    // 0 -> white, 1 -> black
    int player = 0;
    std::vector<Move> legal_actions;

    State(){};
    State(int player) : player(player){};
    State(Board board) : board(board){};
    /**
     * @param board 5*6 piece id array
     * @param player 0 -> white, 1 -> black
     */
    State(Board board, int player) : board(board), player(player){};
    State(State *state) : board(state->board), player(state->player){};

    int evaluatePSS();
    float evaluateHCE(float mobilityWeight, float kingThreatWeight);
    State *next_state(Move move);
    void get_legal_actions();
    std::string encode_output();
    std::string encode_state();
};

#endif