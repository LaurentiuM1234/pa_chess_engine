#include "../models/board.h"
#include "../../util.h"
#include "../containers/arraylist.h"

/*
 * Generate all possible rook moves for current board's state and side and
 * put them in a given arraylist object.
 * @params: moves -> pointer to an arraylist object
 *          board -> pointer to a board object
 *          side -> side for which the generation is done
 *			lookup_table -> contains rook moves for each square possible
 * @returns: no return
 */
void add_rook_moves(arraylist_t *moves, board_t *board, side_t side);

uint64_t generate_rook_moves(board_t *board, side_t side, uint64_t rook_bitboard);

// rook attack board
uint64_t rook_attack_board(board_t *board, unsigned int position);