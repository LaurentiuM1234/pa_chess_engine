#ifndef __FILTER__
#define __FILTER__

#include "containers/arraylist.h"
#include "models/board.h"

arraylist_t *filter_moves(board_t *board, arraylist_t *moves, side_t side);


#endif