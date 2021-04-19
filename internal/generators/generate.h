#ifndef __GENERATE__
#define __GENERATE__

#include "../containers/arraylist.h"
#include "../models/board.h"

/*
 * Generate all possible moves for current board's state and side.
 * @params: board -> pointer to a board object
 *          side -> side for which the moves are generated
 * @returns: newly allocated arraylist containing the generated moves
 */
arraylist_t *generate_moves(board_t *board, side_t side);

#endif