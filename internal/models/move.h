#ifndef __MOVE__
#define __MOVE__

#include <stdint.h>

/*==========FLAG MASKS=============*/
#define M_QUIET 1U
#define M_DOUBLE_PUSH 2U
#define M_CAPTURE 4U
#define M_QUEEN_PROM 8U
/*=================================*/

/*===========DECODE KEYS===========*/
#define M_SRC 0
#define M_TARGET 8
#define M_FLAGS 16
#define M_PIECE 24
/*=================================*/

typedef uint32_t move_t;

/*
 * Encode given parameters into a move.
 * @params: source -> the source square
 *          target -> the target square
 *          flags -> flags associated with the move
 *          captured_piece -> code of the captured piece
 * @returns: move containing given information
 */
move_t encode_move(move_t source, move_t target,
                   move_t flags, move_t captured_piece);

/*
 * Decode given move using a key.
 * @params: move -> move object to be decoded
 *          decode_key -> value representing what info.
 *          to extract from the move object
 * @returns: the searched info
 */
move_t decode_move(move_t move, unsigned int decode_key);

/*
 * Convert a string to a move object.
 * @params: buffer -> string to be converted
 * @returns: the move resulting from conversion
 */
move_t to_move(char *buffer);

/*
 * Convert a move to a string.
 * @params: buffer -> buffer in which the converted
 *          move will be placed (has to be of size 5 at least)
 *          move -> move object to be converted
 * @returns: no return
 */
void to_string(char *buffer, move_t move);


void print_move(move_t move);



#endif