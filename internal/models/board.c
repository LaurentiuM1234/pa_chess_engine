#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "../containers/arraylist.h"
#include "../../util.h"


struct board {
    arraylist_t *_pieces;
    uint64_t _check_board;
    unsigned int _ep_square;
};


static board_t *alloc_board(void)
{
    board_t *board = calloc(1, sizeof(board_t));

    if (!board)
        return NULL;

    board->_pieces = create_list(sizeof(uint64_t));
    board->_ep_square = 0U;
    board->_check_board = 0U;

    if (!board->_pieces) {
        free(board);
        return NULL;
    }

    return board;
}

static void init_pieces(arraylist_t *pieces)
{
    //defining variables for each of the bitboards
    uint64_t all_pawns = 71776119061282560ULL;
    uint64_t all_rooks = 9295429630892703873ULL;
    uint64_t all_bishops = 2594073385365405732ULL;
    uint64_t all_knights = 4755801206503243842ULL;
    uint64_t all_queens = 576460752303423496ULL;
    uint64_t all_kings = 1152921504606846992ULL;
    uint64_t all_whites = 65535ULL;
    uint64_t all_blacks = 18446462598732840960ULL;


    //adding the variables to the piece list
    push(pieces, &all_whites);
    push(pieces, &all_blacks);
    push(pieces, &all_pawns);
    push(pieces, &all_rooks);
    push(pieces, &all_bishops);
    push(pieces, &all_knights);
    push(pieces, &all_queens);
    push(pieces, &all_kings);
}

board_t *create_board(void)
{
    board_t *board = alloc_board();

    if (!board)
        return NULL;

    init_pieces(board->_pieces);

    return board;
}

void destroy_board(board_t **board)
{
    destroy_list(&(*board)->_pieces);
    free(*board);
    *board = NULL;
}

uint64_t get_occupancy(board_t *board)
{
    return *(uint64_t*)get(board->_pieces, ALL_WHITES) |
           *(uint64_t*)get(board->_pieces, ALL_BLACKS);
}

uint64_t get_side(board_t *board, side_t side) {
    return *(uint64_t*)get(board->_pieces, side);
}

uint64_t get_pawns(board_t *board, side_t side)
{
    return *(uint64_t*)get(board->_pieces, ALL_PAWNS) &
           *(uint64_t*)get(board->_pieces, side);
}

uint64_t get_knights(board_t *board, side_t side)
{
    return *(uint64_t*)get(board->_pieces, ALL_KNIGHTS) &
           *(uint64_t*)get(board->_pieces, side);
}

uint64_t get_bishops(board_t *board, side_t side)
{
    return *(uint64_t*)get(board->_pieces, ALL_BISHOPS) &
           *(uint64_t*)get(board->_pieces, side);
}

uint64_t get_queen(board_t *board, side_t side)
{
    return *(uint64_t*)get(board->_pieces, ALL_QUEENS)
           & *(uint64_t*)get(board->_pieces, side);
}

uint64_t get_king(board_t *board, side_t side)
{
    return *(uint64_t*)get(board->_pieces, ALL_KINGS)
           & *(uint64_t*)get(board->_pieces, side);
}

int to_piece(board_t *board, unsigned int position)
{
    uint64_t piece_board = to_bitboard(position);

    for (int i = 2; i < size(board->_pieces); i++) {
        if ((piece_board & (*(uint64_t*)get(board->_pieces, i))))
            return i;
    }

    return 0ULL;
}

uint64_t get_bitboard(board_t *board, int index)
{
    return *(uint64_t*)get(board->_pieces, index);
}

void update_bitboard(board_t *board, int index, uint64_t *new_value)
{
    set(board->_pieces, index, new_value);
}

void clear_board(board_t *board)
{
    // cleaning the array list
    flush(board->_pieces);

    // clearning the check board
    board->_check_board = 0U;

    // initializing the array list
    init_pieces(board->_pieces);
}

void precompute_tables() {
    knight_lookup_table = precompute_knight_lookup_table();
    precompute_bishop_data();
}

void free_tables() {
    free(knight_lookup_table);
    free_bishop_data();
}

unsigned int get_ep_square(board_t *board)
{
  return board->_ep_square;
}

void set_ep_square(board_t *board, unsigned int new)
{
  board->_ep_square = new;
}

void update_check_board(board_t *board, uint64_t new)
{
  board->_check_board = new;
}

uint64_t get_check_board(board_t *board)
{
  return board->_check_board;
}

void board_copy(board_t *dest, board_t *src)
{
  dest->_check_board = src->_check_board;
  dest->_ep_square = src->_ep_square;

  for (int i = 0; i < 8; i++) {
    uint64_t crt_board = get_bitboard(src, i);
    update_bitboard(dest, i, &crt_board);
  }
}