#include "eval.h"
#include "models/board.h"

int eval_pawns(move_t move)
{
    return (int)(decode_move(move, M_FLAGS) + decode_move(move, M_PIECE));
}

int eval_knight(move_t move) {
    return (int)(decode_move(move, M_FLAGS) + decode_move(move, M_PIECE));
}

move_t select_move(arraylist_t* moves, eval_t move_evaluator)
{
    move_t ref = 0U;
    int ref_score = 0;

    for (int i = 0; i < size(moves); i++) {
        move_t crt_move = *(move_t*)get(moves, i);
        int crt_score = (*move_evaluator)(crt_move);

        if (crt_score > ref_score) {
            ref_score = crt_score;
            ref = crt_move;
        }
    }

    return ref;
}