#ifndef __EVAL__
#define __EVAL__

#include "models/move.h"
#include "containers/arraylist.h"

typedef int (*eval_t)(move_t);

/*
 * Utility function used to compute the score of a pawn move.
 * @params: move -> given move for which the score is computed
 * @returns: the computed score
 */
int eval_pawns(move_t move);

int eval_knights(move_t move);


/*
 * Select a move which has the highest score from an arraylist of moves.
 * @params: moves -> pointer to arraylist object containing moves
 *          move_evaluator -> pointer to a function used to compute the
 *          score of a move
 * @returns: the move with the highest score
 */
move_t select_move(arraylist_t* moves, eval_t move_evaluator);

#endif