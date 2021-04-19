#include "move.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

    return encode_move(src, target, 0U, 0U);
}

void to_string(char *buffer, move_t move)
{
    move_t src = decode_move(move, M_SRC);
    move_t target = decode_move(move, M_TARGET);

    // adding "move" command to buffer
    memcpy(buffer, "move ", 5);

    // adding actual move to buffer
    buffer[5] = (char)((src % 8) + 'a');
    buffer[7] = (char)((target % 8) + 'a');
    buffer[6] = (char)((src / 8) + 49);
    buffer[8] = (char)((target / 8) + 49);
}

void print_move(move_t move)
{
    printf("[SOURCE SQUARE]: %d\n", move & 255U);
    printf("[TARGET SQUARE]: %d\n", (move & (255U << 8U)) >> 8U);
    printf("[FLAG VALUE]: %d\n", (move & (255U << 16U)) >> 16U);
    printf("[CAPTURED PIECE CODE]: %d\n", (move & (255U << 24U)) >> 24U);
}