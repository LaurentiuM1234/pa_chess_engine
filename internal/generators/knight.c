#include "knight.h"
#include <stdlib.h>
#include <string.h>

// generates all possible moves a knight can make
// from a specific position
static uint64_t get_knight_moves(uint64_t position) {
    // 2 moves NW
    // 2 moves NE
    // 2 moves SW
    // 2 moves SE

    uint64_t bitboard = 0;
    int rank = get_rank_index(position);
    int file = get_file_index(position);

    bitboard = (add_move(file + 2, rank + 1)) | (add_move(file + 1, rank + 2)) |
            (add_move(file + 2, rank - 1)) | (add_move(file + 1, rank - 2)) |
            (add_move(file - 2, rank + 1)) | (add_move(file - 1, rank + 2)) |
            (add_move(file - 2, rank - 1)) | (add_move(file - 1, rank - 2));

    return bitboard;
}

// generates a lookup table for the knight piece, where
// lookup_table[i] represents all possible moves a knight
// can make from position 'i'
uint64_t* precompute_knight_lookup_table() {
    uint64_t* lookup_table = malloc(64 * sizeof(uint64_t));
    if(!lookup_table) {
        return NULL;
    }

    for (int i = 0; i < 64; i++) {
        lookup_table[i] = get_knight_moves(to_bitboard(i));
    }

    return lookup_table;
}

// returns bitboard representation for each knight (k1 and k2)
static void get_each_knight(board_t *board, side_t side, uint64_t* k1, uint64_t* k2) {
    uint64_t knights = get_knights(board, side);

    // find first knight
    for (int i = 0; i < 64; i++) {
        if (circular_left_shift(1, i) & knights) {
            *k1 = circular_left_shift(1, i);
            break;
        }
    }

    *k2 = knights & (~(*k1));
}

// DOESN'T TAKE INTO ACCOUNT LEAVING THE KING IN CHECK (SO FAR)
// STORE THE MOVES INTO AN ARRAY
// returns an array of moves if there exists at least one possible move
// returns NULL if no moves are possible
static arraylist_t* generate_knight_moves(board_t *board, side_t side, uint64_t knight_bitboard, uint64_t* lookup_table) {
    // check if knight is alive
    if (knight_bitboard == 0) {
        return NULL;
    }

    uint64_t move_bitboard = lookup_table[to_position(knight_bitboard)] & (~get_side(board, side));

    arraylist_t* moves = create_list(sizeof(uint64_t));
    if(!moves) {
        return NULL;
    }

    for(int i = 0; i < 64; i++) {
        uint64_t move = circular_left_shift(1, i);
        if (move & move_bitboard) {
            //TODO: ALSO CHECK IF THE MOVE LEAVES THE KING IN CHECK
            push(moves, &move);
        }
    }

    return moves;
}

// adds a quiet move to the arraylist
static void add_quiet(arraylist_t *moves, board_t *board, side_t side, uint64_t knight_bitboard, uint64_t move) {
    move_t source_square = to_position(knight_bitboard);
    move_t target_square = to_position(move);

    // generate move
    move_t encoded_move = encode_move(source_square, target_square, M_QUIET, 0U);
    print_move(encoded_move);

    // add move to the move list
    push(moves, &encoded_move);
}

static void add_capture(arraylist_t *moves, board_t *board, side_t side, uint64_t knight_bitboard, uint64_t move) {
    move_t source_square = to_position(knight_bitboard);
    move_t target_square = to_position(move);
    move_t captured_piece = to_piece(board, target_square);

    // generate move
    move_t encoded_move = encode_move(source_square, target_square, M_CAPTURE, captured_piece);

    // add move to the move list
    push(moves, &encoded_move);
}


/*
 * add moves to the arraylist
 * @params: moves -> the arraylist containing all the moves
 *          board -> full chess board
 *          side -> current side
 *          knight_bitboard -> bitboard containing the location of a knight
 *          move_list -> list of moves the knight can make
 */
static void push_moves(arraylist_t *moves, board_t *board, side_t side, uint64_t knight_bitboard, arraylist_t* move_list) {
    uint32_t enemy_side;
    if (side == WHITE) {
        enemy_side = BLACK;
    } else {
        enemy_side = WHITE;
    }

    uint64_t enemy = get_side(board, enemy_side);

    // iterate through the move list
    for (int i = 0; i < size(move_list); i++) {
        // check if the move is a capture
        uint64_t current_move;
        memcpy(&current_move, get(move_list, i), sizeof(uint64_t));

        if (enemy & current_move) {
            add_capture(moves, board, side, knight_bitboard, current_move);
        } else {
            // quiet move
            add_quiet(moves, board, side, knight_bitboard, current_move);
        }
    }

}

void add_knight_moves(arraylist_t *moves, board_t *board, side_t side) {
    uint64_t k1 = 0;
    uint64_t k2 = 0;

    get_each_knight(board, side, &k1, &k2);

    // first knight
    arraylist_t* move_list = generate_knight_moves(board, side, k1, knight_lookup_table);
    if (move_list) {
        push_moves(moves, board, side, k1, move_list);
        destroy_list(&move_list);
    }

    // second knight
    move_list = generate_knight_moves(board, side, k2, knight_lookup_table);
    if (move_list) {
        push_moves(moves, board, side, k2, move_list);
        destroy_list(&move_list);
    }
}