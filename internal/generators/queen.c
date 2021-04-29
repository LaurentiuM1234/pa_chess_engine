#include "queen.h"
#include <stdlib.h>
#include <string.h>

extern uint64_t get_attack_set_bishop(board_t* board, side_t side, uint64_t bishop_bitboard);

uint64_t get_attack_set_queen(board_t* board, side_t side, uint64_t queen_bitboard) {
	// TODO: ADD ROOK ATTACK SET WHEN AVAILABLE
    return get_attack_set_bishop(board, side, queen_bitboard);
}

static void add_quiet(arraylist_t *moves, board_t *board, side_t side, uint64_t queen_bitboard, uint64_t move) {
    move_t source_square = to_position(queen_bitboard);
    move_t target_square = to_position(move);

    // generate move
    move_t encoded_move = encode_move(source_square, target_square, M_QUIET, 0U);

    // add move to the move list
    push(moves, &encoded_move);
}

static void add_capture(arraylist_t *moves, board_t *board, side_t side, uint64_t queen_bitboard, uint64_t move) {
    move_t source_square = to_position(queen_bitboard);
    move_t target_square = to_position(move);
    move_t captured_piece = to_piece(board, target_square);

    // generate move
    move_t encoded_move = encode_move(source_square, target_square, M_CAPTURE, captured_piece);

    // add move to the move list
    push(moves, &encoded_move);
}

void add_queen_moves(arraylist_t* moves, board_t* board, side_t side) {
    uint64_t queen = get_queen(board, side);

    uint64_t enemies;
    if(side == WHITE) {
        enemies = get_side(board, BLACK);
    } else {
        enemies = get_side(board, WHITE);
    }

   if (queen == 0) {
        return; // queen is dead
    }

    uint64_t attack_set = get_attack_set_queen(board, side, queen);

    for (int j = 0; j < 64; j++) {
        uint64_t move = circular_left_shift(1, j);

        if (move & attack_set) {
            if (move & enemies) {
                add_capture(moves, board, side, queen, move);
            } else {
                add_quiet(moves, board, side, queen, move);
            }
        }
    }
}

static uint64_t side_queen_attack_board(board_t* board, side_t side, unsigned int position) {
    uint64_t bitboard = to_bitboard(position);
    uint64_t queen = get_queen(board, side);
    
    if (queen != 0 && (get_attack_set_queen(board, side, queen) & bitboard)) {
        return queen;
    }

    return 0;
}

uint64_t queen_attack_board(board_t* board, unsigned int position) {
    return side_queen_attack_board(board, WHITE, position) |
        side_queen_attack_board(board, BLACK, position);
}