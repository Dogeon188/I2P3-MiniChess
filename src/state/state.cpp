#include <cstdint>
#include <iostream>
#include <sstream>

#include "../config.hpp"
#include "./state.hpp"

enum Piece : int8_t {
    EMPTY = 0,
    PAWN = 1,
    ROOK = 2,
    KNIGHT = 3,
    BISHOP = 4,
    QUEEN = 5,
    KING = 6,
    MAX_PIECE
};
static const int move_table_rook_bishop[8][7][2] = {
    { { 0, 1 }, { 0, 2 }, { 0, 3 }, { 0, 4 }, { 0, 5 }, { 0, 6 }, { 0, 7 } },
    { { 0, -1 }, { 0, -2 }, { 0, -3 }, { 0, -4 }, { 0, -5 }, { 0, -6 }, { 0, -7 } },
    { { 1, 0 }, { 2, 0 }, { 3, 0 }, { 4, 0 }, { 5, 0 }, { 6, 0 }, { 7, 0 } },
    { { -1, 0 }, { -2, 0 }, { -3, 0 }, { -4, 0 }, { -5, 0 }, { -6, 0 }, { -7, 0 } },
    { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 } },
    { { 1, -1 }, { 2, -2 }, { 3, -3 }, { 4, -4 }, { 5, -5 }, { 6, -6 }, { 7, -7 } },
    { { -1, 1 }, { -2, 2 }, { -3, 3 }, { -4, 4 }, { -5, 5 }, { -6, 6 }, { -7, 7 } },
    { { -1, -1 }, { -2, -2 }, { -3, -3 }, { -4, -4 }, { -5, -5 }, { -6, -6 }, { -7, -7 } },
};
static const int move_table_knight[8][2] = {
    { 1, 2 },
    { 1, -2 },
    { -1, 2 },
    { -1, -2 },
    { 2, 1 },
    { 2, -1 },
    { -2, 1 },
    { -2, -1 },
};
static const int move_table_king[8][2] = {
    { 1, 0 },
    { 0, 1 },
    { -1, 0 },
    { 0, -1 },
    { 1, 1 },
    { 1, -1 },
    { -1, 1 },
    { -1, -1 },
};

/**
 * @brief 1st iteration: naive approach,
 * different piece has different score,
 * sum them up to get state score.
 *
 * @return sum of piece score
 */
//                                    P♟R♜ N♞B♝ Q♛  K♚
static const int piece_value[MAX_PIECE] = { 0, 2, 6, 7, 8, 20, 300 };
int State::evaluatePSS() {
    int value = 0;
    for (int i = 0; i < BOARD_H; i += 1) {
        for (int j = 0; j < BOARD_W; j += 1) {
            // player
            int now_piece = this->board.board[this->player][i][j];
            if (now_piece) {
                value += piece_value[now_piece];
            }
            // opponent
            now_piece = this->board.board[1 - this->player][i][j];
            if (now_piece) {
                value -= piece_value[now_piece];
            }
        }
    }
    return value;
}

int State::_evaluateKingThreat(int player) {
    int value = 0;
    const auto &kingPos = this->board.king[player];
    for (auto move : move_table_king) {
        int64_t p[2] = { kingPos.first + move[0], kingPos.second + move[1] };

        if (p[0] >= BOARD_H || p[0] < 0 || p[1] >= BOARD_W || p[1] < 0) continue;
        int now_piece = this->board.board[1 - player][p[0]][p[1]];
        if (now_piece == KING) {
            ++value;
            break;
        }
    }
    for (auto move : move_table_knight) {
        int64_t p[2] = { kingPos.first + move[0], kingPos.second + move[1] };

        if (p[0] >= BOARD_H || p[0] < 0 || p[1] >= BOARD_W || p[1] < 0) continue;
        int now_piece = this->board.board[1 - player][p[0]][p[1]];
        if (now_piece == KNIGHT) {
            ++value;
            break;
        }
    }
    for (int i = 0; i < 4; ++i) {
        for (auto move : move_table_rook_bishop[i]) {
            int64_t p[2] = { kingPos.first + move[0], kingPos.second + move[1] };

            if (p[0] >= BOARD_H || p[0] < 0 || p[1] >= BOARD_W || p[1] < 0) break;
            int now_piece = this->board.board[1 - player][p[0]][p[1]];
            if (now_piece == ROOK || now_piece == QUEEN) {
                ++value;
                break;
            }
        }
    }
    for (int i = 4; i < 8; ++i) {
        for (auto move : move_table_rook_bishop[i]) {
            int64_t p[2] = { kingPos.first + move[0], kingPos.second + move[1] };

            if (p[0] >= BOARD_H || p[0] < 0 || p[1] >= BOARD_W || p[1] < 0) break;
            int now_piece = this->board.board[1 - player][p[0]][p[1]];
            if (now_piece == BISHOP || now_piece == QUEEN) {
                ++value;
                break;
            }
        }
    }
    return value;
}

#define HCE_MOBILITY_WEIGHT 16   // divided instead of multiplied
#define HCE_KINGTHREAT_WEIGHT 1

/**
 * @brief 2nd iteration: heuristic approach,
 * added mobility and king threat.
 * @return PSS + (MOB / MOB_WEIGHT) + (KT * KT_WEIGHT)
 */
int State::evaluateHCE() {
    int value = 0;
    // piece score
    value += this->evaluatePSS();
#if HCE_MOBILITY_WEIGHT   // mobility
    value += this->legal_actions.size() / HCE_MOBILITY_WEIGHT;
#endif
#if HCE_KINGTHREAT_WEIGHT   // is king threatened
    if (this->board.king[this->player].first != -1) {
        value -= this->_evaluateKingThreat(this->player) * HCE_KINGTHREAT_WEIGHT;
    }
    if (this->board.king[1 - this->player].first != -1) {
        value += this->_evaluateKingThreat(1 - this->player) * HCE_KINGTHREAT_WEIGHT;
    }
#endif
    return value;
}

/**
 * @brief return next state after the move
 *
 * @param move
 * @return State*
 */
State *State::next_state(Move move) {
    Board next = this->board;
    Point from = move.first, to = move.second;

    int8_t moved = next.board[this->player][from.first][from.second];
    // promotion for pawn
    if (moved == PAWN && (to.first == BOARD_H - 1 || to.first == 0)) {
        moved = QUEEN;
    }
    // update king position
    if (moved == KING) {
        next.king[this->player] = to;
    }
    if (next.board[1 - this->player][to.first][to.second]) {
        if (next.board[1 - this->player][to.first][to.second] == KING) {
            next.king[1 - this->player] = { -1, -1 };
        }
        next.board[1 - this->player][to.first][to.second] = EMPTY;
    }

    next.board[this->player][from.first][from.second] = EMPTY;
    next.board[this->player][to.first][to.second] = moved;

    State *next_state = new State(next, 1 - this->player);

    if (this->game_state != WIN)
        next_state->get_legal_actions();
    return next_state;
}

/**
 * @brief get all legal actions of now state
 *
 */
void State::get_legal_actions() {
    // [Optional]
    // This method is not very efficient
    // You can redesign it
    this->game_state = NONE;
    std::vector<Move> all_actions;
    auto self_board = this->board.board[this->player];
    auto oppn_board = this->board.board[1 - this->player];

    int now_piece, oppn_piece;
    for (int i = 0; i < BOARD_H; i += 1) {
        for (int j = 0; j < BOARD_W; j += 1) {
            if (!(now_piece = self_board[i][j])) continue;

            // std::cout << this->player << "," << now_piece << ' ';
            switch (now_piece) {
                case PAWN:
                    if (this->player && i < BOARD_H - 1) {
                        // black
                        if (!oppn_board[i + 1][j] && !self_board[i + 1][j])
                            all_actions.push_back(Move(Point(i, j), Point(i + 1, j)));
                        if (j < BOARD_W - 1 && (oppn_piece = oppn_board[i + 1][j + 1]) > 0) {
                            all_actions.push_back(Move(Point(i, j), Point(i + 1, j + 1)));
                            if (oppn_piece == KING) {
                                this->game_state = WIN;
                                this->legal_actions = all_actions;
                                return;
                            }
                        }
                        if (j > 0 && (oppn_piece = oppn_board[i + 1][j - 1]) > 0) {
                            all_actions.push_back(Move(Point(i, j), Point(i + 1, j - 1)));
                            if (oppn_piece == KING) {
                                this->game_state = WIN;
                                this->legal_actions = all_actions;
                                return;
                            }
                        }
                    } else if (!this->player && i > 0) {
                        // white
                        if (!oppn_board[i - 1][j] && !self_board[i - 1][j])
                            all_actions.push_back(Move(Point(i, j), Point(i - 1, j)));
                        if (j < BOARD_W - 1 && (oppn_piece = oppn_board[i - 1][j + 1]) > 0) {
                            all_actions.push_back(Move(Point(i, j), Point(i - 1, j + 1)));
                            if (oppn_piece == KING) {
                                this->game_state = WIN;
                                this->legal_actions = all_actions;
                                return;
                            }
                        }
                        if (j > 0 && (oppn_piece = oppn_board[i - 1][j - 1]) > 0) {
                            all_actions.push_back(Move(Point(i, j), Point(i - 1, j - 1)));
                            if (oppn_piece == KING) {
                                this->game_state = WIN;
                                this->legal_actions = all_actions;
                                return;
                            }
                        }
                    }
                    break;

                case ROOK:     // rook
                case BISHOP:   // bishop
                case QUEEN:    // queen
                    int st, end;
                    switch (now_piece) {
                        case ROOK:
                            st = 0;
                            end = 4;
                            break;   // rook
                        case BISHOP:
                            st = 4;
                            end = 8;
                            break;   // bishop
                        case QUEEN:
                            st = 0;
                            end = 8;
                            break;   // queen
                        default: st = 0; end = -1;
                    }
                    for (int part = st; part < end; part += 1) {
                        auto move_list = move_table_rook_bishop[part];
                        for (int k = 0; k < std::max(BOARD_H, BOARD_W); k += 1) {
                            int p[2] = { move_list[k][0] + i, move_list[k][1] + j };

                            if (p[0] >= BOARD_H || p[0] < 0 || p[1] >= BOARD_W || p[1] < 0) break;
                            now_piece = self_board[p[0]][p[1]];
                            if (now_piece) break;

                            all_actions.push_back(Move(Point(i, j), Point(p[0], p[1])));

                            oppn_piece = oppn_board[p[0]][p[1]];
                            if (oppn_piece) {
                                if (oppn_piece == KING) {
                                    this->game_state = WIN;
                                    this->legal_actions = all_actions;
                                    return;
                                } else
                                    break;
                            };
                        }
                    }
                    break;

                case KNIGHT:   // knight
                    for (auto move : move_table_knight) {
                        int x = move[0] + i;
                        int y = move[1] + j;

                        if (x >= BOARD_H || x < 0 || y >= BOARD_W || y < 0) continue;
                        now_piece = self_board[x][y];
                        if (now_piece) continue;
                        all_actions.push_back(Move(Point(i, j), Point(x, y)));

                        oppn_piece = oppn_board[x][y];
                        if (oppn_piece == KING) {
                            this->game_state = WIN;
                            this->legal_actions = all_actions;
                            return;
                        }
                    }
                    break;

                case KING:   // king
                    for (auto move : move_table_king) {
                        int p[2] = { move[0] + i, move[1] + j };

                        if (p[0] >= BOARD_H || p[0] < 0 || p[1] >= BOARD_W || p[1] < 0) continue;
                        now_piece = self_board[p[0]][p[1]];
                        if (now_piece) continue;

                        all_actions.push_back(Move(Point(i, j), Point(p[0], p[1])));

                        oppn_piece = oppn_board[p[0]][p[1]];
                        if (oppn_piece == KING) {
                            this->game_state = WIN;
                            this->legal_actions = all_actions;
                            return;
                        }
                    }
                    break;
            }
        }
    }
    this->legal_actions = all_actions;
}

const char piece_table[2][MAX_PIECE][5] = {
    { " ", "♙", "♖", "♘", "♗", "♕", "♔" },
    { " ", "♟", "♜", "♞", "♝", "♛", "♚" }
};
/**
 * @brief encode the output for command line output
 *
 * @return std::string
 */
std::string State::encode_output() {
    std::stringstream ss;
    int now_piece;
    for (int i = 0; i < BOARD_H; i += 1) {
        for (int j = 0; j < BOARD_W; j += 1) {
            if ((now_piece = this->board.board[0][i][j])) {
                ss << std::string(piece_table[0][now_piece]);
            } else if ((now_piece = this->board.board[1][i][j])) {
                ss << std::string(piece_table[1][now_piece]);
            } else {
                ss << " ";
            }
            ss << " ";
        }
        ss << "\n";
    }
    return ss.str();
}

/**
 * @brief encode the state to the format for player
 *
 * @return std::string
 */
std::string State::encode_state() {
    std::stringstream ss;
    ss << this->player;
    ss << "\n";
    for (int pl = 0; pl < 2; pl += 1) {
        for (int i = 0; i < BOARD_H; i += 1) {
            for (int j = 0; j < BOARD_W; j += 1) {
                ss << int(this->board.board[pl][i][j]);
                ss << " ";
            }
            ss << "\n";
        }
        ss << "\n";
    }
    return ss.str();
}
