#include "update.h"
#include "../util.h"
#include "../debug/fatal.h"
#include "generators/generate.h"

#define EP_DISPLACEMENT 16


static int in_is_quiet(side_t side, uint64_t target_bitboard, int captured_piece, int moved_piece)
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

static int in_is_capture(side_t side, uint64_t target_bitboard, int captured_piece, int moved_piece)
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

static int in_is_prom_capture(side_t side, uint64_t target_bitboard, int captured_piece, int moved_piece)
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

static int in_is_quiet_prom(side_t side, uint64_t target_bitboard, int captured_piece, int moved_piece)
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

static int gen_is_prom(move_t flags)
{
  if (flags & M_QUEEN_PROM || flags & M_ROOK_PROM|| flags & M_BISHOP_PROM || flags & M_KNIGHT_PROM)
    return 1;
  else
    return 0;
}

static int gen_is_quiet(move_t flags)
{
  if ((flags & M_QUIET) && !gen_is_prom(flags))
    return 1;
  else
    return 0;
}

static int gen_is_capture(move_t flags)
{
  if ((flags & M_CAPTURE) && !gen_is_prom(flags))
    return 1;
  else
    return 0;
}

static int gen_is_prom_capture(move_t flags)
{
  if ((flags & M_CAPTURE) && gen_is_prom(flags))
    return 1;
  else
    return 0;
}

static int gen_is_quiet_prom(move_t flags)
{
  if ((flags & M_QUIET) && gen_is_prom(flags))
    return 1;
  else
    return 0;
}

static void update_ep_square(board_t *board, uint64_t src_bitboard, side_t side,
                             uint64_t target_bitboard, int moved_piece)
{
  // check to see if ep square can be updated
  if (moved_piece == ALL_PAWNS) {
    int displacement = abs((int)to_position(target_bitboard) - (int)to_position(src_bitboard));

    if (displacement == EP_DISPLACEMENT) {
      if (side == WHITE)
        set_ep_square(board, to_position(src_bitboard) + 8);
      else
        set_ep_square(board, to_position(src_bitboard) - 8);
    }
  }
}

static void update_castling_rook_flags(board_t *board, uint64_t rook_src, side_t side) {
  if (side == WHITE) {
    if (rook_src & WHITE_INITIAL_LR) set_king_flags(board, WHITE_LR);
    else if (rook_src & WHITE_INITIAL_RR) set_king_flags(board, WHITE_RR);
  } else {
    if (rook_src & BLACK_INITIAL_LR) set_king_flags(board, BLACK_LR);
    else if (rook_src & BLACK_INITIAL_RR) set_king_flags(board, BLACK_RR);   
  }
}

static void update_castling_king_flags(board_t *board, uint64_t king_src, side_t side) {
  if (side == WHITE) {
    if (king_src & WHITE_INITIAL_KING) set_king_flags(board, WHITE_KING_MOVED);
  } else {
    if (king_src & BLACK_INITIAL_KING) set_king_flags(board, BLACK_KING_MOVED);
  }
}

static void add_promoted_piece(board_t *board, uint64_t target_bitboard, move_t flags)
{
  if (flags & M_QUEEN_PROM) {
    // adding queen
    uint64_t queen_bitboard = target_bitboard | get_bitboard(board, ALL_QUEENS);
    update_bitboard(board, ALL_QUEENS, &queen_bitboard);
  } else if (flags & M_KNIGHT_PROM) {
    // adding knight
    uint64_t knight_bitboard = target_bitboard | get_bitboard(board, ALL_KNIGHTS);
    update_bitboard(board, ALL_KNIGHTS, &knight_bitboard);
  } else if (flags & M_BISHOP_PROM) {
    // adding bishop
    uint64_t bishop_bitboard = target_bitboard | get_bitboard(board, ALL_BISHOPS);
    update_bitboard(board, ALL_BISHOPS, &bishop_bitboard);
  } else if (flags & M_ROOK_PROM) {
    // adding rook
    uint64_t rook_bitboard = target_bitboard | get_bitboard(board, ALL_ROOKS);
    update_bitboard(board, ALL_ROOKS, &rook_bitboard);
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

    // updating ep square if need be
    update_ep_square(board, src_bitboard, side, target_bitboard, moved_piece);

    // check if it's a rook move; update castling flags if needed
    if (moved_piece == ALL_ROOKS) {
      update_castling_rook_flags(board, src_bitboard, side);
    }

    // castling can only be a quiet move
    // check for king movement and treat the case in which it is a castling move
    if (moved_piece == ALL_KINGS) {
      uint64_t qb_cs = 0U;
      uint64_t sb_cs = 0U;

      if (target_bitboard == (src_bitboard << 2)) {
        // king-side castling

        if (side == WHITE) {
          qb_cs = (WHITE_INITIAL_RR | WHITE_INITIAL_RR >> 2) ^ get_bitboard(board, ALL_ROOKS);
          update_bitboard(board, ALL_ROOKS, &qb_cs);

          sb_cs = (WHITE_INITIAL_RR | WHITE_INITIAL_RR >> 2) ^ get_bitboard(board, side);
          update_bitboard(board, side, &sb_cs);
        } else {
          qb_cs = (BLACK_INITIAL_RR | BLACK_INITIAL_RR >> 2) ^ get_bitboard(board, ALL_ROOKS);
          update_bitboard(board, ALL_ROOKS, &qb_cs);

          sb_cs = (BLACK_INITIAL_RR | BLACK_INITIAL_RR >> 2) ^ get_bitboard(board, side);
          update_bitboard(board, side, &sb_cs);
        }

      } else if (target_bitboard == (src_bitboard >> 2)) {
        //queen-side castling

        if (side == WHITE) {
          qb_cs = (WHITE_INITIAL_LR | WHITE_INITIAL_LR << 3) ^ get_bitboard(board, ALL_ROOKS);
          update_bitboard(board, ALL_ROOKS, &qb_cs);

          sb_cs = (WHITE_INITIAL_LR | WHITE_INITIAL_LR << 3) ^ get_bitboard(board, side);
          update_bitboard(board, side, &sb_cs);
        } else {
          qb_cs = (BLACK_INITIAL_LR | BLACK_INITIAL_LR << 3) ^ get_bitboard(board, ALL_ROOKS);
          update_bitboard(board, ALL_ROOKS, &qb_cs);

          sb_cs = (BLACK_INITIAL_LR | BLACK_INITIAL_LR << 3) ^ get_bitboard(board, side);
          update_bitboard(board, side, &sb_cs);
        }
      }

      update_castling_king_flags(board, src_bitboard, side);
    }
}

static void update_ep_capture(board_t *board, side_t side, uint64_t src_bitboard,
                              uint64_t target_bitboard, int moved_piece)
{
  uint64_t captured_bitboard = 0U;
  uint64_t op_bitboard = 0U;

  if (side == WHITE) {
    captured_bitboard = (target_bitboard >> 8U) ^ get_bitboard(board, ALL_PAWNS);
    op_bitboard = (target_bitboard >> 8U) ^ get_bitboard(board, !side);

  } else {
    captured_bitboard = (target_bitboard << 8U) ^ get_bitboard(board, ALL_PAWNS);
    op_bitboard = (target_bitboard << 8U) ^ get_bitboard(board, !side);
  }

  // removing captured piece from its bitboard
  update_bitboard(board, ALL_PAWNS, &captured_bitboard);

  // removing captured piece from opponent's board
  update_bitboard(board, !side, &op_bitboard);

  // updating bitboard for moved piece
  uint64_t moved_bitboard = (src_bitboard | target_bitboard) ^ get_bitboard(board, moved_piece);
  update_bitboard(board, moved_piece, &moved_bitboard);

  // updating side's bitboard
  uint64_t side_bitboard = (src_bitboard | target_bitboard) ^ get_bitboard(board, side);
  update_bitboard(board, side, &side_bitboard);

  // updating board's ep square
  set_ep_square(board, 0U);

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

    // check if it's a rook move; update castling flags if needed
    if (moved_piece == ALL_ROOKS) {
      update_castling_rook_flags(board, src_bitboard, side);
    }

    // same for the king
    if (moved_piece == ALL_KINGS) {
      update_castling_king_flags(board, src_bitboard, side);
    }
}

static void update_prom_capture(board_t *board, side_t side, uint64_t src_bitboard,
                                uint64_t target_bitboard, int moved_piece, int captured_piece,
                                move_t prom_flags)
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

    // adding promoted piece
    add_promoted_piece(board, target_bitboard, prom_flags);

    // updating side's bitboard
    uint64_t side_bitboard = (src_bitboard | target_bitboard) ^ get_bitboard(board, side);
    update_bitboard(board, side, &side_bitboard);
}

static void update_quiet_prom(board_t *board, side_t side, uint64_t src_bitboard,
                              uint64_t target_bitboard, int moved_piece, move_t prom_flags)
{
    // removing pawn
    uint64_t pawn_bitboard = src_bitboard ^ get_bitboard(board, ALL_PAWNS);
    update_bitboard(board, moved_piece, &pawn_bitboard);

    // adding promoted piece
    add_promoted_piece(board, target_bitboard, prom_flags);

    // updating side's bitboard
    uint64_t side_bitboard = (src_bitboard | target_bitboard) ^ get_bitboard(board, side);
    update_bitboard(board, side, &side_bitboard);
}

static void update_by_in(board_t *board, side_t side, move_t in_move)
{
    // extracting information
    move_t src = decode_move(in_move, M_SRC);
    move_t target = decode_move(in_move, M_TARGET);
    move_t prom_flags = decode_move(in_move, M_FLAGS);

    // computing auxiliary information
    uint64_t src_bitboard = to_bitboard(src);
    uint64_t target_bitboard = to_bitboard(target);
    int captured_piece = to_piece(board, target);
    int moved_piece = to_piece(board, src);

    // check if move is en-passant and handle
    if (target == get_ep_square(board) && moved_piece == ALL_PAWNS) {
      update_ep_capture(board, side, src_bitboard, target_bitboard, moved_piece);
      return;
    }

    // clearing ep square
    set_ep_square(board, 0U);

    if (moved_piece == 0 || !(src_bitboard & get_bitboard(board, side)))
        FATAL_ERROR("Invalid move.");

    if (in_is_quiet(side, target_bitboard, captured_piece, moved_piece)) {
      update_quiet(board, side, src_bitboard, target_bitboard, moved_piece);
    } else if (in_is_capture(side, target_bitboard, captured_piece, moved_piece)) {
        update_capture(board, side, src_bitboard,
                       target_bitboard, moved_piece, captured_piece);
    } else if (in_is_prom_capture(side, target_bitboard, captured_piece, moved_piece)) {
      update_prom_capture(board, side, src_bitboard,
                          target_bitboard, moved_piece, captured_piece,
                          prom_flags);
    } else if (in_is_quiet_prom(side, target_bitboard, captured_piece, moved_piece)) {
      update_quiet_prom(board, side, src_bitboard, target_bitboard, moved_piece, prom_flags);
    } else {
      FATAL_ERROR("Invalid move.");
    }
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

    // check if move is en-passant and handle
    if ((flags & M_EP) && moved_piece == ALL_PAWNS) {
      update_ep_capture(board, side, src_bitboard, target_bitboard, moved_piece);
      return;
    }

    // clearing ep square
    set_ep_square(board, 0U);

    if (moved_piece == 0 || !(src_bitboard & get_bitboard(board, side)))
        FATAL_ERROR("Invalid move.");

    if (gen_is_quiet(flags)) {
      update_quiet(board, side, src_bitboard, target_bitboard, moved_piece);
    } else if (gen_is_capture(flags)) {
        update_capture(board, side, src_bitboard,
                       target_bitboard, moved_piece, captured_piece);
    } else if (gen_is_prom_capture(flags)) {
      update_prom_capture(board, side, src_bitboard,
                          target_bitboard, moved_piece, captured_piece,
                          flags);
    } else if (gen_is_quiet_prom(flags)) {
      update_quiet_prom(board, side, src_bitboard, target_bitboard, moved_piece, flags);
    } else {
      FATAL_ERROR("Invalid move.");
    }
}

static void update_check_num(board_t *board) {
  if (compute_check_board(board, WHITE)) {
    add_white_check(board);
  }

  if (compute_check_board(board, BLACK)) {
    add_black_check(board);
  }
}

void update(board_t *board, side_t side, move_t in_move, move_t gen_move)
{
    if ((in_move == 0U && gen_move == 0U) || (in_move != 0U && gen_move != 0U))
        FATAL_ERROR("Invalid parameters.");
    else if (in_move != 0U)
        update_by_in(board, !side, in_move);
    else
        update_by_gen(board, side, gen_move);

    // update the check board for given side
    update_check_board(board, compute_check_board(board, side));
    update_check_num(board);
}
