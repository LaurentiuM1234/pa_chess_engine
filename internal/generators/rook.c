#include "rook.h"
#include <stdlib.h>
#include <string.h>


//get the rooks from the boards
static void get_each_rook(board_t *board, side_t side, uint64_t* r1, uint64_t* r2) {
    uint64_t rooks = get_rooks(board, side);

    // find first rook
    for (int i = 0; i < 64; i++) {
        if (circular_left_shift(1, i) & rooks) {
            *r1 = circular_left_shift(1, i);
            break;
        }
    }

    *r2 = rooks & (~(*r1));
}

uint64_t generate_rook_moves(board_t *board, side_t side, uint64_t rook_bitboard) {
    if (rook_bitboard == 0) {
        return 0ULL;
    }

    uint64_t bitboard = 0ULL;
    uint64_t next_move = 0ULL;

    uint64_t occ = get_occupancy(board);
    uint64_t enemy_occ = get_side(board, side) ^ occ;

    int i;
    int rank = get_rank_index(rook_bitboard);
    int file = get_file_index(rook_bitboard);

    //going up
    for(i = file + 1; i < 8; ++i) {
    	next_move = add_move(i, rank);

    	if(next_move & occ) {
    		if(next_move & enemy_occ) {
    			bitboard |= next_move;
    		}

    		break;
    	} else {
    		bitboard |= next_move;
    	}
    }

    //going down
    for(i = file - 1; i >= 0; --i) {
    	next_move = add_move(i, rank);

    	if(next_move & occ) {
    		if(next_move & enemy_occ) {
    			bitboard |= next_move;
    		}

    		break;
    	} else {
    		bitboard |= next_move;
    	}
    }

    //going left
    for(i = rank - 1; i >= 0; --i) {
    	next_move = add_move(file, i);

    	if(next_move & occ) {
    		if(next_move & enemy_occ) {
    			bitboard |= next_move;
    		}

    		break;
    	} else {
    		bitboard |= next_move;
    	}
    }

    //going right
    for(i = rank + 1; i < 8; ++i) {
    	next_move = add_move(file, i);

    	if(next_move & occ) {
    		if(next_move & enemy_occ) {
    			bitboard |= next_move;
    		}

    		break;
    	} else {
    		bitboard |= next_move;
    	}
    }

    return bitboard;
}

static arraylist_t* generate_rook_moves_arr(uint64_t bitboard) {
    arraylist_t* moves = create_list(sizeof(uint64_t));
    if(!moves) {
        return NULL;
    }

    for(int i = 0; i < 64; i++) {
        uint64_t move = circular_left_shift(1, i);
        if (move & bitboard) {
            push(moves, &move);
        }
    }

    return moves;
}

// adds a quiet move to the arraylist
static void add_quiet(arraylist_t *moves, board_t *board, side_t side, uint64_t rook_bitboard, uint64_t move) {
    move_t source_square = to_position(rook_bitboard);
    move_t target_square = to_position(move);

    // generate move
    move_t encoded_move = encode_move(source_square, target_square, M_QUIET, 0U);

    // add move to the move list
    push(moves, &encoded_move);
}

static void add_capture(arraylist_t *moves, board_t *board, side_t side, uint64_t rook_bitboard, uint64_t move) {
    move_t source_square = to_position(rook_bitboard);
    move_t target_square = to_position(move);
    move_t captured_piece = to_piece(board, target_square);

    // generate move
    move_t encoded_move = encode_move(source_square, target_square, M_CAPTURE, captured_piece);

    // add move to the move list
    push(moves, &encoded_move);
}

static void push_moves(arraylist_t *moves, board_t *board, side_t side, uint64_t rook_bitboard, arraylist_t* move_list) {
    uint32_t enemy_side;
    if (side == WHITE) {
        enemy_side = BLACK;
    } else {
        enemy_side = WHITE;
    }

    uint64_t enemy = get_side(board, enemy_side);

    // iterate through the move list
    for (int i = 0; i < size(move_list); i++) {
        // check if the move is a capture
        uint64_t current_move;
        memcpy(&current_move, get(move_list, i), sizeof(uint64_t));

        if (enemy & current_move) {
            add_capture(moves, board, side, rook_bitboard, current_move);
        } else {
            // quiet move
            add_quiet(moves, board, side, rook_bitboard, current_move);
        }
    }

}

void add_rook_moves(arraylist_t *moves, board_t *board, side_t side) {
    uint64_t r1 = 0;
    uint64_t r2 = 0;

    get_each_rook(board, side, &r1, &r2);

    // first rook
    arraylist_t* move_list = generate_rook_moves_arr(generate_rook_moves(board, side, r1));
    if (move_list) {
        push_moves(moves, board, side, r1, move_list);
        destroy_list(&move_list);
    }

    // second rook
    move_list = generate_rook_moves_arr(generate_rook_moves(board, side, r2));
    if (move_list) {
        push_moves(moves, board, side, r2, move_list);
        destroy_list(&move_list);
    }
}

static uint64_t side_rook_attack_board(board_t *board, unsigned int position, side_t side) {
    uint64_t eligible_rooks = 0ULL;

    uint64_t attacked_position = to_bitboard(position);

    uint64_t occ = get_occupancy(board);
    uint64_t enemy_occ = get_side(board, side) ^ occ;

    uint64_t r1, r2;
    get_each_rook(board, side, &r1, &r2);

    if (!r1 && !r2) return 0ULL;

    int i, rank, file;

    uint64_t next_move;

    uint64_t attack_bitboard = 0ULL;

    if (r1) {
        rank = get_rank_index(r1);
        file = get_file_index(r1);

        for(i = file + 1; i < 8; ++i) {
            next_move = add_move(i, rank);

            if(next_move & occ) {
                if(next_move & enemy_occ) {
                    attack_bitboard |= next_move;
                }

                break;
            } else {
                attack_bitboard |= next_move;
            }
        }

        //going down
        for(i = file - 1; i >= 0; --i) {
            next_move = add_move(i, rank);

            if(next_move & occ) {
                if(next_move & enemy_occ) {
                    attack_bitboard |= next_move;
                }

                break;
            } else {
                attack_bitboard |= next_move;
            }
        }

        //going left
        for(i = rank - 1; i >= 0; --i) {
            next_move = add_move(file, i);

            if(next_move & occ) {
                if(next_move & enemy_occ) {
                    attack_bitboard |= next_move;
                }

                break;
            } else {
                attack_bitboard |= next_move;
            }
        }

        //going right
        for(i = rank + 1; i < 8; ++i) {
            next_move = add_move(file, i);

            if(next_move & occ) {
                if(next_move & enemy_occ) {
                    attack_bitboard |= next_move;
                }

                break;
            } else {
                attack_bitboard |= next_move;
            }
        }

        if (attack_bitboard & attacked_position) eligible_rooks |= r1;
    }

    attack_bitboard = 0ULL;

    if (r2) {
        rank = get_rank_index(r2);
        file = get_file_index(r2);

        for(i = file + 1; i < 8; ++i) {
            next_move = add_move(i, rank);

            if(next_move & occ) {
                if(next_move & enemy_occ) {
                    attack_bitboard |= next_move;
                }

                break;
            } else {
                attack_bitboard |= next_move;
            }
        }

        //going down
        for(i = file - 1; i >= 0; --i) {
            next_move = add_move(i, rank);

            if(next_move & occ) {
                if(next_move & enemy_occ) {
                    attack_bitboard |= next_move;
                }

                break;
            } else {
                attack_bitboard |= next_move;
            }
        }

        //going left
        for(i = rank - 1; i >= 0; --i) {
            next_move = add_move(file, i);

            if(next_move & occ) {
                if(next_move & enemy_occ) {
                    attack_bitboard |= next_move;
                }

                break;
            } else {
                attack_bitboard |= next_move;
            }
        }

        //going right
        for(i = rank + 1; i < 8; ++i) {
            next_move = add_move(file, i);

            if(next_move & occ) {
                if(next_move & enemy_occ) {
                    attack_bitboard |= next_move;
                }

                break;
            } else {
                attack_bitboard |= next_move;
            }
        }

        if (attack_bitboard & attacked_position) eligible_rooks |= r2;
    }

    return eligible_rooks;
}

uint64_t rook_attack_board(board_t *board, unsigned int position) {
    return side_rook_attack_board(board, position, WHITE) | 
           side_rook_attack_board(board, position, BLACK);
}

