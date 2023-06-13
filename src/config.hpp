#pragma once

/*Board Size, Don't change!*/
#define BOARD_H 6
#define BOARD_W 5

/*You can modify these things in development for fast testing*/
/*When TA run your program, we will use default settings (123, 10, 50)*/
#define RANDOM_SEED 123
#define timeout 10
#define MAX_STEP 50

/*Which character/words for pieces*/
/* By default, the pieces are '♟', '♜', '♞', '♝', '♛', '♚' from unicode*/
#define PIECE_STR_LEN 2
const char PIECE_TABLE[2][7][5] = {
#if PIECE_STR_LEN == 1
    { " ", "♟", "♜", "♞", "♝", "♛", "♚" },
    { " ", "♙", "♖", "♘", "♗", "♕", "♔" },
#elif PIECE_STR_LEN == 2
    { "  ", "wP", "wR", "wN", "wB", "wQ", "wK" },
    { "  ", "bP", "bR", "bN", "bB", "bQ", "bK" },
#endif
};
