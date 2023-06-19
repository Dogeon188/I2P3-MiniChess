#include <fstream>
#include <iostream>

#include "../config.hpp"
#include "../policy/abprune_hce.hpp"
#include "../policy/mcts.hpp"
#include "../state/state.hpp"

static const int MINIMAX_DEPTHS[] = { 6, 8, 10, 11 };

#define MCTS_ITERATION 500000
#define MCTS_GENERATION 20

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

/**
 * @brief randomly choose a move and then write it into output file
 *
 * @param fout
 */
void write_valid_spot(std::ofstream &fout) {
    int pieceCount = 0;
    for (int i = 0; i < BOARD_H; i++) {
        for (int j = 0; j < BOARD_W; j++) {
            if (root->board.board[root->player][i][j]) pieceCount++;
            if (root->board.board[1 - root->player][i][j]) pieceCount++;
        }
    }
    if (pieceCount > 16) {
        auto move = MCTS::get_move(root, MCTS_ITERATION, MCTS_GENERATION, fout);
        fout << move.first.first << " " << move.first.second << " "
             << move.second.first << " " << move.second.second << std::endl;
        fout.flush();
        return;
    }
    for (auto depth : MINIMAX_DEPTHS) {
        auto move = ABPruneHCE::get_move(root, depth);
        fout << move.first.first << " " << move.first.second << " "
             << move.second.first << " " << move.second.second << std::endl;
        fout.flush();
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
