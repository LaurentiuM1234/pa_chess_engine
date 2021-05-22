#include "move.h"
#include "../../util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static move_t extract_prom_flags(char *buffer)
{
  move_t flags = 0U;

  if (is_promotion(buffer)) {
    if (buffer[4] == 'q')
      flags = M_QUEEN_PROM;
    else if (buffer[4] == 'n')
      flags = M_KNIGHT_PROM;
    else if (buffer[4] == 'b')
      flags = M_BISHOP_PROM;
    else if (buffer[4] == 'r')
      flags = M_ROOK_PROM;
    else
      flags = 0U;
  }

  return flags;
}

static void convert_prom_flags(char *buffer, move_t flags)
{
  if (flags == 0U)
    return;

  if (flags & M_QUEEN_PROM)
    buffer[9] = 'q';
  else if (flags & M_ROOK_PROM)
    buffer[9] = 'r';
  else if (flags & M_BISHOP_PROM)
    buffer[9] = 'b';
  else if (flags & M_KNIGHT_PROM)
    buffer[9] = 'n';
}

move_t encode_move(move_t source, move_t target, move_t flags, move_t captured_piece)
{
    return source | target << 8U | flags << 16U | captured_piece << 24U;
}

move_t decode_move(move_t move, unsigned int decode_key)
{
    return (move >> decode_key) & 255U;
}

move_t to_move(char *buffer)
{
    move_t src = (atoi(&buffer[1]) - 1) * 8 + (buffer[0] - 'a');
    move_t target = (atoi(&buffer[3]) - 1) * 8 + (buffer[2] - 'a');
    move_t prom_flags = extract_prom_flags(buffer);

    return encode_move(src, target, prom_flags, 0U);
}

void to_string(char *buffer, move_t move)
{
    move_t src = decode_move(move, M_SRC);
    move_t target = decode_move(move, M_TARGET);
    move_t prom_flags = decode_move(move, M_FLAGS);

    // adding "move" command to buffer
    memcpy(buffer, "move ", 5);

    // adding actual move to buffer
    buffer[5] = (char)((src % 8) + 'a');
    buffer[7] = (char)((target % 8) + 'a');
    buffer[6] = (char)((src / 8) + 49);
    buffer[8] = (char)((target / 8) + 49);

    // add promotion flags to move string
    convert_prom_flags(buffer, prom_flags);
}

void print_move(move_t move)
{
    printf("[SOURCE SQUARE]: %d\n", move & 255U);
    printf("[TARGET SQUARE]: %d\n", (move & (255U << 8U)) >> 8U);
    printf("[FLAG VALUE]: %d\n", (move & (255U << 16U)) >> 16U);
    printf("[CAPTURED PIECE CODE]: %d\n", (move & (255U << 24U)) >> 24U);
}