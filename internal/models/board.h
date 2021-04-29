#ifndef __BOARD__
#define __BOARD__


#include <stdint.h>

#include "move.h"

enum piece_code{
    ALL_WHITES,
    ALL_BLACKS,
    ALL_PAWNS,
    ALL_ROOKS,
    ALL_BISHOPS,
    ALL_KNIGHTS,
    ALL_QUEENS,
    ALL_KINGS
};

typedef enum side {
    WHITE,
    BLACK
}side_t;

typedef struct board board_t;

/*
 * Create a board.
 * @returns: newly allocated board object.
 */
board_t *create_board(void);

/*
 * Safe delete a board object.
 * @params: board -> reference of pointer to a board object
 * @returns: no return
 */
void destroy_board(board_t **board);

/*
 * Clear a board.
 * @params: board -> pointer to a board object
 * @returns: no return
 */
void clear_board(board_t *board);

// Retrieve occupancy bitboard for white or black
uint64_t get_side(board_t *board, side_t side);

/*
 * Retrieve the occupancy bitboard.
 * @params: board -> pointer to a board object
 * @returns: the occupancy bitboard
 */
uint64_t get_occupancy(board_t *board);

/*
 * Retrieve the pawns bitboard for a given side.
 * @params: board -> pointer to a board object
 *          side -> side for which the retrieval is done
 * @returns: the pawns bitboard for given side
 */
uint64_t get_pawns(board_t *board, side_t side);

/*
 * Retrieve the knights bitboard for a given side.
 * @params: board -> pointer to a board object
 *          side -> side for which the retrieval is done
 * @returns: the knights bitboard for given side
 */
uint64_t get_knights(board_t *board, side_t side);

/*
 * Retrieve the bishops bitboard for a given side.
 * @params: board -> pointer to a board object
 *          side -> side for which the retrieval is done
 * @returns: the bishops bitboard for given side
 */
uint64_t get_bishops(board_t *board, side_t side);

/*
 * Retrieve the queen bitboard for a given side
 * @params: board -> pointer to a board object
 *          side -> side for which the retrieval is done
 * @returns: the queen bitboard for given side
 */
uint64_t get_queen(board_t *board, side_t side);

/*
 * Retrieve the kings bitboard for a given side
 * @params: board -> pointer to a board object
 *          side -> side for which the retrieval is done
 * @returns: the kings bitboard for given side
 */
uint64_t get_king(board_t *board, side_t side);

/*
 * Retrieve a bitboard found at given position.
 * @params: board -> pointer to a board object
 *          index -> position of the bitboard to be retrieved
 * @returns: the searched bitboard
 */
uint64_t get_bitboard(board_t *board, int index);

/*
 * Update a bitboard found at given position
 * @params: board -> pointer to a board object
 *          index -> position of the searched bitboard
 *          new_value -> new value of the bitboard
 * @returns: no return
 */
void update_bitboard(board_t *board, int index, uint64_t *new_value);

/*
 * Retrieve the code of the piece currently found at given board position.
 * @params: board -> pointer to a board object
 *          position -> position of the searched piece
 * @returns: the piece code
 */
int to_piece(board_t *board, unsigned int position);

// lookup table / hash table generators

uint64_t* knight_lookup_table;
uint64_t* bishop_lookup_table;
uint64_t** bishop_hash_table;
uint64_t* precompute_knight_lookup_table();
void precompute_bishop_data();
void free_bishop_data();

// generates lookup tables and hash tables necessary
// for knight/bishop
void precompute_tables();

// deletes lookup tables and hash tables
void free_tables();

/*
 * Retrieve the value of the en-passant square.
 * @params: board -> pointer to board object
 * @returns: value of the en-passant square
 */
unsigned int get_ep_square(board_t *board);

/*
 * Set the value of the en-passant square.
 * @params: board -> pointer to board object
 *          new -> new value of the en-passant square
 * @returns: no return
 */
void set_ep_square(board_t *board, unsigned int new);

/*
 * Update the value of the check board.
 * @params: board -> pointer to board object
 *          new -> new value of the check board
 * @returns: no return
 */
void update_check_board(board_t *board, uint64_t new);

/*
 * Retrieve the value of the check board.
 * @params: board -> pointer to board object
 * @returns: value of the check board
 */
uint64_t get_check_board(board_t *board);

/*
 * Copy contents of src to dest.
 * @params: dest -> pointer to destination board object
 *          src -> pointer to source board object
 * @returns: no return
 */
void board_copy(board_t *dest, board_t *src);
#endif