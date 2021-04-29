#include "filter.h"
#include "../debug/fatal.h"
#include "models/move.h"
#include "update.h"


static int in_check(board_t *board)
{
  if (get_check_board(board))
    return 1;
  else
    return 0;
}

arraylist_t *filter_moves(board_t *board, arraylist_t *moves, side_t side)
{
  board_t *dummy = create_board();

  if (!dummy)
    FATAL_ERROR("Failed to create board.");

  arraylist_t *filtered_moves = create_list(sizeof(move_t));

  if (!filtered_moves) {
    destroy_board(&dummy);
    FATAL_ERROR("Failed to create arraylist.");
  }

  for (int i = 0; i < size(moves); i++) {
    // saving board state
    board_copy(dummy, board);

    // extracting current move
    move_t crt_move = *((move_t *)get(moves, i));

    //TODO: check if current move is castling and if it is check to see
    // if king is in check before adding it to @filtered_moves

    // update board
    update(board, side, 0U, crt_move);

    // add move if king is not in check after making it
    if (!in_check(board))
      push(filtered_moves, &crt_move);

    // unmake the move
    board_copy(board, dummy);
  }

  destroy_board(&dummy);

  return filtered_moves;
}