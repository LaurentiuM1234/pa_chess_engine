#include "util.h"


#include <math.h>
#include <string.h>
#include <stdlib.h>
#define SAN0_NORMAL 4
#define SAN0_PROMOTION 5

static int is_rank(char ref)
{
    int rank_index = atoi(&ref);

    if (rank_index >= 1 && rank_index <= 8)
        return 1;
    else
        return 0;
}

static int is_file(char ref)
{
    if (ref >= 'a' && ref <= 'h')
        return 1;
    else
        return 0;
}

static int is_valid(char *buffer)
{
  if (is_file(buffer[0]) && is_file(buffer[2])) {
    if (is_rank(buffer[1]) && is_rank(buffer[3]))
      return 1;
    else
      return 0;
  }
  return 0;
}

static int is_normal(char *buffer)
{
  if (strlen(buffer) != SAN0_NORMAL)
    return 0;

  return is_valid(buffer);
}

static int is_promotion(char *buffer)
{
  if (strlen(buffer) != SAN0_PROMOTION)
    return 0;

  return is_valid(buffer);
}

int is_move(char *buffer)
{
    return is_normal(buffer) || is_promotion(buffer);
}

uint64_t to_bitboard(unsigned int position)
{
    return 1ULL << position;
}

unsigned int to_position(uint64_t bitboard)
{
    return log2((bitboard | (bitboard - 1)) ^ (bitboard - 1));
}

uint64_t circular_left_shift(uint64_t target, unsigned int value)
{
    return (target << value) | (target >> (64 - value));
}

void print_bitboard(uint64_t bitboard)
{
    for (unsigned int i = 0; i < 8; i++) {
        for (unsigned int j = 0; j < 8; j++) {
            if (bitboard & (1ULL << (8 * i + j))) {
                printf("%d ", 1);
            } else {
                printf("%c ", '.');
            }
        }
        printf("\n");
    }
    printf("\n");
}

uint64_t get_rank_index(uint64_t square) {
    for (int i = 0; i <= 7; i++) {
      if (circular_left_shift(RANK_1, (8 * i)) & square) {
          return i;
      }
    }
    return 0;
}

uint64_t get_file_index(uint64_t square) {
    for (int i = 0; i <= 7; i++) {
        if (circular_left_shift(FILE_A, i) & square) {
            return i;
        }
    }
    return 0;
}

uint64_t add_move(int file, int rank) {
    if (file >= 0 && file <= 7 && rank >= 0 && rank <= 7) {
        return to_bitboard(rank * 8 + file);
    }
    return 0;
}

uint64_t get_bit(uint64_t n, int pos) {
	return (n >> pos) & 1;
}