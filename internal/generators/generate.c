#include "generate.h"
#include "pawn.h"
#include "knight.h"
#include "bishop.h"
#include "king.h"

arraylist_t *generate_moves(board_t *board, side_t side)
{
    arraylist_t *available_moves = create_list(sizeof(move_t));

    if (!available_moves)
        return NULL;

    // add pawn moves
    add_pawn_moves(available_moves, board, side);

    // add knight moves
    add_knight_moves(available_moves, board, side);

    // add bishop_moves
    add_bishop_moves(available_moves, board, side);

    // add king moves
    add_king_moves(available_moves, board, side);

    // TODO: add moves for other pieces

    return available_moves;
}