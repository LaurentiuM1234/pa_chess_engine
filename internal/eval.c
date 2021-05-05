#include "eval.h"
#include "models/board.h"

int eval(board_t *board, move_t move)
{
    if (to_piece(board, decode_move(move, M_SRC)) == ALL_KINGS) {
        if ((decode_move(move, M_SRC) == decode_move(move, M_TARGET) + 2) ||
            (decode_move(move, M_SRC) == decode_move(move, M_TARGET) - 2))
                return INT32_MAX; //max score for castling
    }

    return (int)(decode_move(move, M_FLAGS) + decode_move(move, M_PIECE));
}

move_t select_move(board_t *board, arraylist_t* moves, eval_t move_evaluator)
{
    move_t ref = 0U;
    int ref_score = 0;

    for (int i = 0; i < size(moves); i++) {
        move_t crt_move = *(move_t*)get(moves, i);
        int crt_score = (*move_evaluator)(board, crt_move);

        if (crt_score > ref_score) {
            ref_score = crt_score;
            ref = crt_move;
        }
    }

    return ref;
}