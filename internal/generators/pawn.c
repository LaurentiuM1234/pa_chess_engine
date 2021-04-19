#include "pawn.h"
#include "../../util.h"


static void add_quiets(arraylist_t *move_list, uint64_t pawn_board, int offset, uint32_t flags)
{
    while(pawn_board) {
        // extract position of lsb
        move_t target_square = to_position(pawn_board);

        // compute position of source square using given offset
        move_t source_square = target_square - offset;

        // generate move
        move_t move = encode_move(source_square, target_square, flags, 0U);
        print_move(move);

        // add move to the move list
        push(move_list, &move);

        // removing lsb
        pawn_board &= (pawn_board - 1);
    }
}


static void add_captures(arraylist_t *move_list, uint64_t pawn_board, int offset, board_t *board, move_t flags)
{
    while(pawn_board) {
        // extract position of lsb
        move_t target_square = to_position(pawn_board);

        // compute position of source square using given offset
        move_t source_square = target_square - offset;

        // compute value of captured piece
        move_t captured_piece = to_piece(board, target_square);

        // generate move
        move_t move = encode_move(source_square, target_square, flags, captured_piece);

        // add move to the move list
        push(move_list, &move);

        // removing lsb
        pawn_board &= (pawn_board - 1);
    }
}

static void white_pawn_moves(board_t *board, arraylist_t *moves)
{
    uint64_t pawns = get_pawns(board, WHITE);
    uint64_t free_squares = ~get_occupancy(board);

    // computing move boards
    uint64_t single_push = circular_left_shift(pawns, 8) & free_squares;
    uint64_t double_push = circular_left_shift(single_push & RANK_3, 8)
                           & free_squares;
    uint64_t quiet_promos = single_push & RANK_8;
    uint64_t left_captures = circular_left_shift(pawns & ~FILE_H, 9)
                             & get_bitboard(board, ALL_BLACKS) & ~get_king(board, BLACK);
    uint64_t right_captures = circular_left_shift(pawns & ~FILE_A, 7)
                              & get_bitboard(board, ALL_BLACKS)  & ~get_king(board, BLACK);
    uint64_t left_capture_promos = left_captures & RANK_8;
    uint64_t right_capture_promos = right_captures & RANK_8;

    // adding moves
    add_quiets(moves, single_push, 8, M_QUIET);
    add_quiets(moves, double_push, 16, M_DOUBLE_PUSH | M_QUIET);
    add_quiets(moves, quiet_promos, 8, M_QUEEN_PROM | M_QUIET);
    add_captures(moves, left_captures & ~RANK_8, 9, board, M_CAPTURE);
    add_captures(moves, right_captures & ~RANK_8, 7, board, M_CAPTURE);
    add_captures(moves, left_capture_promos, 9, board, M_CAPTURE | M_QUEEN_PROM);
    add_captures(moves, right_capture_promos, 7, board, M_CAPTURE | M_QUEEN_PROM);
}

static void black_pawn_moves(board_t *board, arraylist_t *moves)
{
    uint64_t pawns = get_pawns(board, BLACK);
    uint64_t free_squares = ~get_occupancy(board);

    // computing move boards
    uint64_t single_push = circular_left_shift(pawns, 56) & free_squares;
    uint64_t double_push = circular_left_shift(single_push & RANK_6, 56)
                           & free_squares;
    uint64_t quiet_promos = single_push & RANK_1;
    uint64_t left_captures = circular_left_shift(pawns & ~FILE_A, 55)
                             & get_bitboard(board, ALL_WHITES) & ~get_king(board, WHITE);
    uint64_t right_captures = circular_left_shift(pawns & ~FILE_H, 57)
                              & get_bitboard(board, ALL_WHITES) & ~get_king(board, WHITE);
    uint64_t left_capture_promos = left_captures & RANK_1;
    uint64_t right_capture_promos = right_captures & RANK_1;

    // adding moves
    add_quiets(moves, single_push, -8, M_QUIET);
    add_quiets(moves, double_push, -16, M_DOUBLE_PUSH | M_QUIET);
    add_quiets(moves, quiet_promos, -8, M_QUIET | M_QUEEN_PROM);
    add_captures(moves, left_captures & ~RANK_1, -9, board, M_CAPTURE);
    add_captures(moves, right_captures & ~RANK_1, -7, board, M_CAPTURE);
    add_captures(moves, left_capture_promos, -9, board, M_CAPTURE | M_QUEEN_PROM);
    add_captures(moves, right_capture_promos, -7, board, M_CAPTURE | M_QUEEN_PROM);
}


//NOTICE: CAPTURING THE KING IS NO LONGER A MOVE
void add_pawn_moves(arraylist_t *moves, board_t *board, side_t side)
{
    if (side == WHITE)
        white_pawn_moves(board, moves);
    else
        black_pawn_moves(board, moves);
}