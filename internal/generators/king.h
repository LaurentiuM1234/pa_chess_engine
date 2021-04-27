#ifndef __KING__
#define __KING__

#include "../models/board.h"
#include "../../util.h"
#include "../containers/arraylist.h"

void add_king_moves(arraylist_t *moves, board_t *board, side_t side);

#endif