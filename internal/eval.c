#include "eval.h"
#include "models/board.h"
#include "containers/arraylist.h"
#include "generators/generate.h"
#include "update.h"
#include "../util.h"
#define minus_inf -2147483647

#define PAWN_SCORE 20
#define KNIGHT_SCORE 200
#define ROOK_SCORE 300
#define BISHOP_SCORE 500
#define QUEEN_SCORE 10000
#define CHECK_SCORE 500000

int eval(board_t *board, side_t side)
{
	int side_score = 0;
	int enemy_score = 0;

	if (compute_check_board(board, !side)) {
		side_score += CHECK_SCORE;
	}

	if (compute_check_board(board, side)) {
		enemy_score += CHECK_SCORE;
	}

	for(int i = 0; i < 64; ++i) {
		if (to_bitboard(i) & get_pawns(board, side)) {
			side_score += PAWN_SCORE;
		} else if (to_bitboard(i) & get_knights(board, side)) {
			side_score += KNIGHT_SCORE;
		} else if (to_bitboard(i) & get_rooks(board, side)) {
			side_score += ROOK_SCORE;
		} else if (to_bitboard(i) & get_bishops(board, side)) {
			side_score += BISHOP_SCORE;
		} else if (to_bitboard(i) & get_queen(board, side)) {
			side_score += QUEEN_SCORE;
		}
	}

	for(int i = 0; i < 64; ++i) {
		if (to_bitboard(i) & get_pawns(board, !side)) {
			enemy_score += PAWN_SCORE;
		} else if (to_bitboard(i) & get_knights(board, !side)) {
			enemy_score += KNIGHT_SCORE;
		} else if (to_bitboard(i) & get_rooks(board, !side)) {
			enemy_score += ROOK_SCORE;
		} else if (to_bitboard(i) & get_bishops(board, !side)) {
			enemy_score += BISHOP_SCORE;
		} else if (to_bitboard(i) & get_queen(board, !side)) {
			enemy_score += QUEEN_SCORE;
		}
	}

	return (side_score - enemy_score);
}

static int game_over(board_t *board) {
	if (get_white_checks_num(board) == 3 ||
		get_black_checks_num(board) == 3) {

		return 1;
	}

	return 0;
}

static int generate_rand() {
	// returns 1 or -1
	srand(time(0));
	int r = rand() % 2;
	if (r == 0) {
		return -1;
	} else {
		return 1;
	}
}

static int compare_moves_dsc(const void* a, const void* b) {
	move_score first = *(move_score*)a;
	move_score second = *(move_score*)b;

	int rez = second.score - first.score;

	if (rez == 0) {
		generate_rand();
	}

	return rez;
}

arraylist_t *sort_moves(arraylist_t* moves, board_t* board, side_t side) {
	// sort the moves by score

	// aux vector used for sorting
	move_score* scores = calloc(size(moves), sizeof(move_score));

	board_t *board_cpy = create_board();
	for (int i = 0; i < size(moves); i++) {
		board_copy(board_cpy, board);
		update(board_cpy, side, 0U, *(move_t*)(get(moves, i)));
		scores[i].score = eval(board_cpy, side);
		scores[i].index = i;
	}

	qsort(scores, size(moves), sizeof(move_score), &compare_moves_dsc);

	arraylist_t* sorted_array = create_list(sizeof(move_t));

	for (int i = 0; i < size(moves); i++) {
		int index = scores[i].index;
		push(sorted_array, get(moves, index));
	}

	destroy_board(&board_cpy);
	free(scores);
	destroy_list(&moves);

	return sorted_array;
}

static int select_move_recv(board_t *board, eval_t move_evaluator, 
					  side_t side, int alpha, int beta, int depth)
{
	//arraylist_t *available_moves = generate_moves(board, side);

	arraylist_t *available_moves = sort_moves(generate_moves(board, side), board, side);

	/*printf("recv fnc checks:\n");
	printf("\n%d\n", get_white_checks_num(board));
	printf("%d\n\n", get_black_checks_num(board));*/

	if (game_over(board) || size(available_moves) == 0 || depth == 0) {
		return eval(board, side);
	}

	board_t *board_cpy = create_board();

	int best_score = minus_inf;

	int score;

	for (int i = 0; i < size(available_moves); ++i) {
		board_copy(board_cpy, board);

		update(board_cpy, side, 0U, *(move_t*)(get(available_moves, i)));
		// print_bitboard(get_occupancy(board_cpy));

		score = -(select_move_recv(board_cpy, eval, !side, -beta, -alpha, depth - 1));

		if (score > best_score) {
			best_score = score;
		}

		if (best_score > alpha) {
			alpha = best_score;
		}

		if (alpha >= beta) break;
	}

    return best_score;
}

Best_move select_move(board_t *board, eval_t move_evaluator, arraylist_t* available_moves,
					  side_t side, int alpha, int beta, int depth)
{	
	available_moves = sort_moves(available_moves, board, side);
	/*printf("primary fnc checks:\n");
	printf("\n%d\n", get_white_checks_num(board));
	printf("%d\n\n", get_black_checks_num(board));*/

	if (game_over(board) || size(available_moves) == 0) {
		Best_move bm = {.score = eval(board, side), .move = 0};
		destroy_list(&available_moves);
		return bm;
	}

	board_t *board_cpy = create_board();

	int best_score = minus_inf;
	move_t best_move;

	int score;

	for (int i = 0; i < size(available_moves); ++i) {
		board_copy(board_cpy, board);

		update(board_cpy, side, 0U, *(move_t*)(get(available_moves, i)));
		// print_bitboard(get_occupancy(board_cpy));

		score = -(select_move_recv(board_cpy, eval, !side, -beta, -alpha, depth - 1));

		if (score > best_score) {
			best_score = score;

			best_move = *(move_t*)(get(available_moves, i));
		}

		if (best_score > alpha) {
			alpha = best_score;
		}

		if (alpha >= beta) break;
	}

	//print_bitboard(compute_check_board(board,WHITE));

	destroy_board(&board_cpy);
	destroy_list(&available_moves);

	//printf("%d\n", best_move);
	Best_move bm = {.score = best_score, .move = best_move};

    return bm;
}