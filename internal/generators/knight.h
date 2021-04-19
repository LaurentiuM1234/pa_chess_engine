#include "../models/board.h"
#include "../../util.h"
#include "../containers/arraylist.h"

uint64_t* knight_lookup_table;

/*
 * Generate all possible knight moves for current board's state and side and
 * put them in a given arraylist object.
 * @params: moves -> pointer to an arraylist object
 *          board -> pointer to a board object
 *          side -> side for which the generation is done
 *			lookup_table -> contains knight moves for each square possible
 * @returns: no return
 */
void add_knight_moves(arraylist_t *moves, board_t *board, side_t side);