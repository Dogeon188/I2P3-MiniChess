#include <fstream>
#include <iostream>

#include "../config.hpp"
#include "../policy/minimax_pss.hpp"
#include "../state/state.hpp"

static const int MINIMAX_DEPTHS[] = { 2, 4, 6 };
// depth 7 would likely TLE, depth 6 is the limit

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
    for (auto depth : MINIMAX_DEPTHS) {
        auto move = MinimaxPSS::get_move(root, depth);
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
