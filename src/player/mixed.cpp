#include <fstream>
#include <iostream>

#include "../config.hpp"
#include "../policy/abprune_hce.hpp"
#include "../policy/mcts.hpp"
#include "../state/state.hpp"

static const int MINIMAX_DEPTHS[] = { 6, 8, 10, 11 };

#define MCTS_ITERATION 60000
#define MCTS_GENERATION 10

State *root = nullptr;

/**
 * @brief Read the board from the file
 *
 * @param fin
 */
void read_board(std::ifstream &fin) {
    Board board;
    int player;
    fin >> player;

    for (int pl = 0; pl < 2; pl++) {
        for (int i = 0; i < BOARD_H; i++) {
            for (int j = 0; j < BOARD_W; j++) {
                int c;
                fin >> c;
                board.board[pl][i][j] = c;
            }
        }
    }
    root = new State(board, player);
    root->get_legal_actions();
}

static bool isInitialBoard(State *state) {
    int pieceCount = 0;
    for (int i = 0; i < BOARD_H; i++) {
        for (int j = 0; j < BOARD_W; j++) {
            if (state->board.board[state->player][i][j]) pieceCount++;
            if (state->board.board[1 - state->player][i][j]) pieceCount++;
        }
    }
    return pieceCount > 14;
}

/**
 * @brief randomly choose a move and then write it into output file
 *
 * @param fout
 */
void write_valid_spot(std::ofstream &fout) {
    if (isInitialBoard(root)) {
        for (auto depth : MINIMAX_DEPTHS) {
            auto move = ABPruneHCE::get_move(root, depth);
            fout << move.first.first << " " << move.first.second << " "
                 << move.second.first << " " << move.second.second << std::endl;
            fout.flush();
        }
    } else {
        MCTS::get_move(root, MCTS_ITERATION, MCTS_GENERATION, fout);
    }
}

/**
 * @brief Main function for player
 *
 * @param argv
 * @return int
 */
int main(int, char **argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);

    read_board(fin);
    write_valid_spot(fout);

    fin.close();
    fout.close();
    delete root;
    return 0;
}
