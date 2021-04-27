#ifndef __KING__
#define __KING__

#include "../models/board.h"
#include "../../util.h"
#include "../containers/arraylist.h"


/*
 * Generate all possible king moves for current board's state and side and
 * put them in a given arraylist object.
 * @params: moves -> pointer to an arraylist object
 *          board -> pointer to a board object
 *          side -> side for which the generation is done
 * @returns: no return
 */
void add_king_moves(arraylist_t *moves, board_t *board, side_t side);

#endif