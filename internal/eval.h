#ifndef __EVAL__
#define __EVAL__

#include "models/move.h"
#include "containers/arraylist.h"
#include "models/board.h"

typedef struct best_move {
	int score;
	move_t move;
} Best_move;

typedef struct move_score {
	int score;
	int index; // position in arraylist
} move_score;

typedef int (*eval_t)(board_t*, move_t);

/*
 * Utility function used to compute the score of a pawn move.
 * @params: move -> given move for which the score is computed
 * @returns: the computed score
 */
int eval(board_t *board, side_t side);


/*
 * Select a move which has the highest score from an arraylist of moves.
 * @params: moves -> pointer to arraylist object containing moves
 *          move_evaluator -> pointer to a function used to compute the
 *          score of a move
 * @returns: the move with the highest score
 */
Best_move select_move(board_t *board, eval_t move_evaluator, arraylist_t* moves,
					  side_t side, int alpha, int beta, int depth);

#endif