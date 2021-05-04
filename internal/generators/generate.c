#include "generate.h"
#include "pawn.h"
#include "knight.h"
#include "bishop.h"
#include "rook.h"
#include "queen.h"
#include "king.h"
#include "../filter.h"

arraylist_t *generate_moves(board_t *board, side_t side)
{
    arraylist_t *pseudo_legal_moves = create_list(sizeof(move_t));

    if (!pseudo_legal_moves)
        return NULL;

    // add pawn moves
    add_pawn_moves(pseudo_legal_moves, board, side);

    // add knight moves
    add_knight_moves(pseudo_legal_moves, board, side);

    // add bishop moves
    add_bishop_moves(pseudo_legal_moves, board, side);

   	// add rook moves
   	add_rook_moves(pseudo_legal_moves, board, side);

    // add queen moves
    add_queen_moves(pseudo_legal_moves, board, side);

    // add king moves
    add_king_moves(pseudo_legal_moves, board, side);

    // TODO: add moves for other pieces
    // ^ we have done it woohoo

    // filter the moves
    arraylist_t *filtered_moves = filter_moves(board, pseudo_legal_moves, side);

    // free memory from initial arraylist
    destroy_list(&pseudo_legal_moves);

    return filtered_moves;
}

uint64_t compute_check_board(board_t *board, side_t side)
{
  unsigned int king_position = to_position(get_king(board, side));

  // defining variables for each of the check boards
  // TODO: compute check boards for rooks
  uint64_t pawn_check_board = pawn_attack_board(board, king_position);
  uint64_t rook_check_board = 0U;
  uint64_t bishop_check_board = bishop_attack_board(board, king_position);
  uint64_t knight_check_board = knight_attack_board(board, king_position);
  uint64_t queen_check_board = queen_attack_board(board, king_position);
  uint64_t king_check_board = king_attack_board(board, king_position);

  uint64_t all_pieces_check_board = pawn_check_board | rook_check_board
      | bishop_check_board | knight_check_board
      | queen_check_board | king_check_board;

  uint64_t side_check_board = all_pieces_check_board
      & get_bitboard(board, !side);

  return side_check_board;
}