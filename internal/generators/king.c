#include "king.h"


static void add_moves(board_t *board, arraylist_t *moves,
                           uint64_t king_moves, move_t source_square)
{
  while (king_moves) {
    // extract position of lsb
    move_t target_square = to_position(king_moves);

    // compute value of captured piece
    move_t captured_piece = to_piece(board, target_square);

    move_t move = 0U;

    // generate move
    if (captured_piece == 0U) {
      // quiet move
      move = encode_move(source_square, target_square, M_QUIET, 0U);
    } else {
      // capture
      move = encode_move(source_square, target_square, M_CAPTURE, captured_piece);
    }

    push(moves, &move);
    king_moves &= (king_moves - 1);
  }
}

uint64_t compute_king_moves(uint64_t king_board)
{
  uint64_t north = (king_board & ~RANK_8) << 8U;
  uint64_t south = (king_board & ~RANK_1) >> 8U;
  uint64_t west = (king_board & ~FILE_A) >> 1U;
  uint64_t east = (king_board & ~FILE_H) << 1U;
  uint64_t north_east = (king_board & ~RANK_8 & ~FILE_H) << 9U;
  uint64_t north_west = (king_board & ~RANK_8 & ~FILE_A) << 7U;
  uint64_t south_east = (king_board & ~RANK_1 & ~FILE_H) >> 9U;
  uint64_t south_west = (king_board & ~RANK_1 & ~FILE_A) >> 7U;

  return north | south | west | east
      | north_east | north_west | south_east | south_west;
}

uint64_t compute_castling_moves(board_t* board, side_t side) {
  uint64_t occ = get_occupancy(board);
  uint64_t check_bitboard = get_check_board(board);
  uint64_t king_bitboard = get_king(board, side);

  uint64_t moves = 0ULL;

  if (side == WHITE) {
    if (get_king_flags(board) & WHITE_KING_MOVED)
      return 0ULL;
    if ((get_king_flags(board) & WHITE_RR) && (get_king_flags(board) & WHITE_LR))
      return 0ULL;

    if (get_king_flags(board) & WHITE_KING_IN_CHECK) {
      return 0ULL;
    } else {
      if (~(get_king_flags(board)) & WHITE_RR) {
        if (((king_bitboard << 1U) & occ) == 0 && ((king_bitboard << 2U) & occ) == 0)
          if (((king_bitboard << 1U) & check_bitboard) == 0 && ((king_bitboard << 2U) & check_bitboard) == 0)
            moves |= (king_bitboard << 2U);
      }

      if (~(get_king_flags(board)) & WHITE_LR) {
        if (((king_bitboard >> 1U) & occ) == 0 && ((king_bitboard >> 2U) & occ) == 0)
          if (((king_bitboard >> 1U) & check_bitboard) == 0 && ((king_bitboard >> 2U) & check_bitboard) == 0)
            moves |= (king_bitboard >> 2U);
      }
    }
  } else {
    if (get_king_flags(board) & BLACK_KING_MOVED)
      return 0ULL;
    if ((get_king_flags(board) & BLACK_RR) && (get_king_flags(board) & BLACK_LR))
      return 0ULL;

    if (get_king_flags(board) & BLACK_KING_IN_CHECK) {
      return 0ULL;
    } else {
      if (~(get_king_flags(board)) & BLACK_RR) {
        if (((king_bitboard << 1U) & occ) == 0 && ((king_bitboard << 2U) & occ) == 0)
          if (((king_bitboard << 1U) & check_bitboard) == 0 && ((king_bitboard << 2U) & check_bitboard) == 0)
            moves |= (king_bitboard << 2U);
      }

      if (~(get_king_flags(board)) & BLACK_LR) {
        if (((king_bitboard >> 1U) & occ) == 0 && ((king_bitboard >> 2U) & occ) == 0)
          if (((king_bitboard >> 1U) & check_bitboard) == 0 && ((king_bitboard >> 2U) & check_bitboard) == 0)
            moves |= (king_bitboard >> 2U);
      }
    }
  }

  return moves;
}

void add_king_moves(arraylist_t *moves, board_t *board, side_t side)
{
  uint64_t king_board = get_king(board, side);
  uint64_t king_moves_board = compute_king_moves(king_board) & ~get_side(board, side);
  add_moves(board, moves, king_moves_board, to_position(king_board));

  uint64_t castling_moves = compute_castling_moves(board, side);
  add_moves(board, moves, castling_moves, to_position(king_board));

  printf("king moves castling\n");
  print_bitboard(castling_moves);
}

static uint64_t side_king_attack_board(board_t *board, unsigned int position, side_t side)
{
  uint64_t king_board = get_king(board, side);
  uint64_t position_board = to_bitboard(position);

  uint64_t north = (((king_board & ~RANK_8) << 8U) & position_board) >> 8U;
  uint64_t south = (((king_board & ~RANK_1) >> 8U) & position_board) << 8U;
  uint64_t west = (((king_board & ~FILE_A) >> 1U) & position_board) << 1U;
  uint64_t east = (((king_board & ~FILE_H) << 1U) & position_board) >> 1U;
  uint64_t north_east = (((king_board & ~RANK_8 & ~FILE_H) << 9U) & position_board) >> 9U;
  uint64_t north_west = (((king_board & ~RANK_8 & ~FILE_A) << 7U) & position_board) >> 7U;
  uint64_t south_east = (((king_board & ~RANK_1 & ~FILE_H) >> 9U) & position_board) << 9U;
  uint64_t south_west = (((king_board & ~RANK_1 & ~FILE_A) >> 7U) & position_board) << 7U;

  return north | south | west | east
      | north_east | north_west | south_east | south_west;
}


uint64_t king_attack_board(board_t *board, unsigned int position)
{
  return side_king_attack_board(board, position, WHITE)
  | side_king_attack_board(board, position, BLACK);
}