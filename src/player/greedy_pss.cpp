#include <fstream>
#include <iostream>

#include "../config.hpp"
#include "../policy/greedy_pss.hpp"
#include "../state/state.hpp"

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
    auto move = GreedyPSS::get_move(root, 0);
    fout << move.first.first << " " << move.first.second << " "
         << move.second.first << " " << move.second.second << std::endl;

    // Remember to flush the output to ensure the last action is written to file.
    fout.flush();
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
