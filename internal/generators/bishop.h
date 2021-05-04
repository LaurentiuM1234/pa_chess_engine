#include "../models/board.h"
#include "../../util.h"
#include "../containers/arraylist.h"

/*
 * Generate all possible bishop moves for current board's state and side and
 * put them in a given arraylist object.
 * @params: moves -> pointer to an arraylist object
 *          board -> pointer to a board object
 *          side -> side for which the generation is done
 *			lookup_table -> contains bishop moves for each square possible
 * @returns: no return
 */
void add_bishop_moves(arraylist_t *moves, board_t *board, side_t side);

uint64_t bishop_attack_board(board_t* board, unsigned int position);