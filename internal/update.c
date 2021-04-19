#include "update.h"
#include "../util.h"
#include "../debug/fatal.h"


static int is_quiet(side_t side, uint64_t target_bitboard, int captured_piece, int moved_piece)
{	
	// moved piece is not a pawn
	if (captured_piece == 0 && moved_piece != ALL_PAWNS)
        	return 1;

    // moved piece is pawn
    if (side == WHITE) {
        if ((target_bitboard & ~RANK_8) && (captured_piece == 0) && (moved_piece == ALL_PAWNS))
            return 1;
        else
            return 0;
    } else {
        if ((target_bitboard & ~RANK_1) && (captured_piece == 0) &&  (moved_piece == ALL_PAWNS))
            return 1;
        else
            return 0;
    }
}

static int is_capture(side_t side, uint64_t target_bitboard, int captured_piece, int moved_piece)
{
    if (side == WHITE) {
        if (captured_piece != 0) {
            if (moved_piece == ALL_PAWNS) {
                return (target_bitboard & RANK_8) ? 0 : 1;
            } else {
                return 1;
            }
        } else {
            return 0;
        }
    } else {
        if (captured_piece != 0) {
            if (moved_piece == ALL_PAWNS) {
                return (target_bitboard & RANK_1) ? 0 : 1;
            } else {
                return 1;
            }
        } else {
            return 0;
        }

    }
}

static int is_prom_capture(side_t side, uint64_t target_bitboard, int captured_piece, int moved_piece)
{
    if (side == WHITE) {
        if ((target_bitboard & RANK_8) && (captured_piece != 0) && (moved_piece == ALL_PAWNS))
            return 1;
        else
            return 0;
    } else {
        if ((target_bitboard & RANK_1) && (captured_piece != 0) && (moved_piece == ALL_PAWNS))
            return 1;
        else
            return 0;
    }
}

static int is_quiet_prom(side_t side, uint64_t target_bitboard, int captured_piece, int moved_piece)
{
    if (side == WHITE) {
        if ((target_bitboard & RANK_8) && (captured_piece == 0) && (moved_piece == ALL_PAWNS))
            return 1;
        else
            return 0;
    } else {
        if ((target_bitboard & RANK_1) && (captured_piece == 0) && (moved_piece == ALL_PAWNS))
            return 1;
        else
            return 0;
    }
}

static void update_quiet(board_t *board, side_t side,
                         uint64_t src_bitboard, uint64_t target_bitboard, int moved_piece)
{
    // updating bitboard for moved piece
    uint64_t quiet_bitboard = (src_bitboard | target_bitboard) ^ get_bitboard(board, moved_piece);
    update_bitboard(board, moved_piece, &quiet_bitboard);


    // updating bitboard for opposite side
    uint64_t side_bitboard = (src_bitboard | target_bitboard) ^ get_bitboard(board, side);
    update_bitboard(board, side, &side_bitboard);
}

static void update_capture(board_t *board, side_t side, uint64_t src_bitboard,
                           uint64_t target_bitboard, int moved_piece, int captured_piece)
{
    // removing captured piece from its bitboard
    uint64_t captured_bitboard = target_bitboard ^ get_bitboard(board, captured_piece);
    update_bitboard(board, captured_piece, &captured_bitboard);

    // removing captured piece from opponent's board
    uint64_t op_bitboard = target_bitboard ^ get_bitboard(board, !side);
    update_bitboard(board, !side, &op_bitboard);

    // updating bitboard for moved piece
    uint64_t moved_bitboard = (src_bitboard | target_bitboard) ^ get_bitboard(board, moved_piece);
    update_bitboard(board, moved_piece, &moved_bitboard);

    // updating side's bitboard
    uint64_t side_bitboard = (src_bitboard | target_bitboard) ^ get_bitboard(board, side);
    update_bitboard(board, side, &side_bitboard);
}

static void update_prom_capture(board_t *board, side_t side, uint64_t src_bitboard,
                                uint64_t target_bitboard, int moved_piece, int captured_piece)
{
    // removing captured piece from its bitboard
    uint64_t captured_bitboard = target_bitboard ^ get_bitboard(board, captured_piece);
    update_bitboard(board, captured_piece, &captured_bitboard);

    // removing captured piece from opponent's board
    uint64_t op_bitboard = target_bitboard ^ get_bitboard(board, !side);
    update_bitboard(board, !side, &op_bitboard);

    // removing pawn
    uint64_t pawn_bitboard = src_bitboard ^ get_bitboard(board, ALL_PAWNS);
    update_bitboard(board, moved_piece, &pawn_bitboard);

    // adding queen
    uint64_t queen_bitboard = target_bitboard | get_bitboard(board, ALL_QUEENS);
    update_bitboard(board, ALL_QUEENS, &queen_bitboard);

    // updating side's bitboard
    uint64_t side_bitboard = (src_bitboard | target_bitboard) ^ get_bitboard(board, side);
    update_bitboard(board, side, &side_bitboard);
}

static void update_quiet_prom(board_t *board, side_t side, uint64_t src_bitboard,
                              uint64_t target_bitboard, int moved_piece)
{
    // removing pawn
    uint64_t pawn_bitboard = src_bitboard ^ get_bitboard(board, ALL_PAWNS);
    update_bitboard(board, moved_piece, &pawn_bitboard);

    // adding queen
    uint64_t queen_bitboard = target_bitboard | get_bitboard(board, ALL_QUEENS);
    update_bitboard(board, ALL_QUEENS, &queen_bitboard);

    // updating side's bitboard
    uint64_t side_bitboard = (src_bitboard | target_bitboard) ^ get_bitboard(board, side);
    update_bitboard(board, side, &side_bitboard);
}


static void update_by_in(board_t *board, side_t side, move_t in_move)
{
    // extracting information
    move_t src = decode_move(in_move, M_SRC);
    move_t target = decode_move(in_move, M_TARGET);

    // computing auxiliary information
    uint64_t src_bitboard = to_bitboard(src);
    uint64_t target_bitboard = to_bitboard(target);
    int captured_piece = to_piece(board, target);
    int moved_piece = to_piece(board, src);

    if (moved_piece == 0 || !(src_bitboard & get_bitboard(board, side)))
        FATAL_ERROR("Invalid move.");

    if (is_quiet(side, target_bitboard, captured_piece, moved_piece))
        update_quiet(board, side, src_bitboard, target_bitboard, moved_piece);
    else if (is_capture(side, target_bitboard, captured_piece, moved_piece))
        update_capture(board, side, src_bitboard, target_bitboard, moved_piece, captured_piece);
    else if (is_prom_capture(side, target_bitboard, captured_piece, moved_piece))
        update_prom_capture(board, side, src_bitboard, target_bitboard, moved_piece, captured_piece);
    else if (is_quiet_prom(side, target_bitboard, captured_piece, moved_piece))
        update_quiet_prom(board, side, src_bitboard, target_bitboard, moved_piece);
    else
        FATAL_ERROR("Invalid move.");
}

static void update_by_gen(board_t *board, side_t side, move_t gen_move)
{
    // extracting information
    move_t src = decode_move(gen_move, M_SRC);
    move_t target = decode_move(gen_move, M_TARGET);
    move_t flags = decode_move(gen_move, M_FLAGS);
    move_t captured_piece = decode_move(gen_move, M_PIECE);


    // computing auxiliary information
    uint64_t src_bitboard = to_bitboard(src);
    uint64_t target_bitboard = to_bitboard(target);
    int moved_piece = to_piece(board, src);

    if (moved_piece == 0 || !(src_bitboard & get_bitboard(board, side)))
        FATAL_ERROR("Invalid move.");

    if ((flags & M_QUIET) && !(flags & M_QUEEN_PROM))
        update_quiet(board, side, src_bitboard, target_bitboard, moved_piece);
    else if ((flags & M_CAPTURE) && !(flags & M_QUEEN_PROM))
        update_capture(board, side, src_bitboard, target_bitboard, moved_piece, captured_piece);
    else if ((flags & M_CAPTURE) && (flags & M_QUEEN_PROM))
        update_prom_capture(board, side, src_bitboard, target_bitboard, moved_piece, captured_piece);
    else if ((flags & M_QUIET) && (flags & M_QUEEN_PROM))
        update_quiet_prom(board, side, src_bitboard, target_bitboard, moved_piece);
    else
        FATAL_ERROR("Invalid move.");
}


void update(board_t *board, side_t side, move_t in_move, move_t gen_move)
{
    if ((in_move == 0U && gen_move == 0U) || (in_move != 0U && gen_move != 0U))
        FATAL_ERROR("Invalid parameters.");
    else if (in_move != 0U)
        update_by_in(board, !side, in_move);
    else
        update_by_gen(board, side, gen_move);
}
