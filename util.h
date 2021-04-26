#ifndef __UTIL__
#define __UTIL__


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*==========FILE MASKS============*/
#define FILE_A 72340172838076673ULL
#define FILE_H 9259542123273814144ULL
/*================================*/

/*==========RANK MASKS=============*/
#define RANK_1 255ULL
#define RANK_2 65280ULL
#define RANK_3 16711680ULL
#define RANK_6 280375465082880ULL
#define RANK_7 71776119061217280ULL
#define RANK_8 18374686479671623680ULL
/*=================================*/


/*
 * Convert given board position to a bitboard.
 * @params: position -> given position to be converted
 * @returns: bitboard for given position
 */
uint64_t to_bitboard(unsigned int position);

/*
 * Check if given string is a move
 * @params: buffer -> given string
 * @returns -> 1 if the string is a move, 0 otherwise
 */
int is_move(char *buffer);

/*
 * Convert given bitboard to a board position.
 * @params: bitboard -> bitboard to be converted
 * @returns: board position for given bitboard
 */
unsigned int to_position(uint64_t bitboard);

/*
 * Apply a circular left shift of given value to given target
 * @params: target -> target of the shift operation
 *          value -> value of the shift
 * @returns: shifted target
 */
uint64_t circular_left_shift(uint64_t target, unsigned int value);

uint64_t circular_right_shift(uint64_t target, unsigned int value);

void print_bitboard(uint64_t bitboard);

uint64_t get_rank_index(uint64_t square);

uint64_t get_file_index(uint64_t square);

uint64_t add_move(int file, int rank);

uint64_t generate_magic_number();

uint64_t get_bit(uint64_t n, int pos);

int is_promotion(char *buffer);

#endif