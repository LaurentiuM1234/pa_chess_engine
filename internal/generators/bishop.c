#include "bishop.h"
#include <stdlib.h>

// bishop magic numbers brute-force generated
// bishop magic[i] is the magic number for square 'i'
const uint64_t bishop_magic[64] = {
  0x410010204040124ULL, 0x88a00418808d1010ULL, 0xa208020404200800ULL,
  0x90908200800860ULL, 0x41104008000400ULL, 0x800880c42000280ULL,
  0x141420820083000ULL, 0x82010088142200ULL, 0x200401009010122ULL,
  0x1008a801040222ULL, 0x1000900082204972ULL, 0x100d480600440808ULL,
  0x4001011040020400ULL, 0x220410406c10004ULL, 0x8900228208a04c02ULL,
  0x1001408c04060200ULL, 0x140001102020410ULL, 0x202004080e1748ULL,
  0x50000448c20040ULL,0x8021405411010ULL, 0x1001020820082a0cULL,
  0x802000458020801ULL, 0x22000b08420210ULL, 0x1022000032020248ULL,
  0x12090040102c80ULL, 0x1008081004010805ULL, 0x42670000a040240ULL,
  0x140c0020410200ULL, 0x4840008802020ULL, 0x404410000900810ULL,
  0x1004406321000ULL, 0x1414441040200ULL, 0x41103008c8050811ULL,
  0x8020600080800ULL, 0x4080180400080040ULL, 0x1002220080080080ULL,
  0x1204140400001010ULL, 0x810100060021200ULL, 0x2009040100028800ULL,
  0x841100108188ULL, 0x4080814d0200404ULL, 0x82100a029000ULL,
  0x700050119003080cULL, 0x42091000803ULL, 0xa44480104001040ULL,
  0x2009002800300ULL, 0xc2280801200280ULL, 0x1010008100c40100ULL,
  0x80a6008208409000ULL, 0x9006004424348000ULL, 0x2014404040006ULL, 
  0x808000884040010ULL, 0x980001810640000ULL, 0x295037020090ULL,
  0x284002060c010009ULL, 0x1042700202204204ULL, 0x182208210a02040ULL,
  0x80028401481200ULL, 0x201000008048080aULL, 0x2001000000209800ULL,
  0x800110010c20224ULL, 0x800242004010a00ULL, 0x204280208420404ULL,
  0x902200401820040ULL
};


// BBits[i] = log(2, n) where n is the total number of 
// masked blockers for square 'i'
int BBits[64] = {
  6, 5, 5, 5, 5, 5, 5, 6,
  5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  6, 5, 5, 5, 5, 5, 5, 6
};

// returns bitboard for each bishop
static void get_each_bishop(board_t *board, side_t side, uint64_t* b1, uint64_t* b2) {
    uint64_t bishops = get_bishops(board, side);

    // find first bishop
    for (int i = 0; i < 64; i++) {
        if (circular_left_shift(1, i) & bishops) {
            *b1 = circular_left_shift(1, i);
            break;
        }
    }

    *b2 = bishops & (~(*b1));
}

// generates all possible moves a bishop can make
// from a specific position, ignoring blocking pieces
static uint64_t get_bishop_moves(uint64_t position) {
    uint64_t bitboard = 0;
    int rank = get_rank_index(position);
    int file = get_file_index(position);

    // NW
    int next_rank = rank + 1;
    int next_file = file + 1;

    while (next_rank < 7 && next_file < 7) {
        bitboard |= add_move(next_file, next_rank);
        next_file++, next_rank++;
    }

    // NE
    next_rank = rank + 1;
    next_file = file - 1;

    while (next_rank < 7 && next_file > 0) {
        bitboard |= add_move(next_file, next_rank);
        next_file--, next_rank++;
    }

    // SW
    next_rank = rank - 1;
    next_file = file + 1;

    while (next_rank > 0 && next_file < 7) {
        bitboard |= add_move(next_file, next_rank);
        next_file++, next_rank--;
    }

    // SE
    next_rank = rank - 1;
    next_file = file - 1;

    while (next_rank > 0 && next_file > 0) {
        bitboard |= add_move(next_file, next_rank);
        next_file--, next_rank--;
    }

    return bitboard;
}

// returns a bitboard containing blocking pieces (pieces on margin not included)
static uint64_t get_masked_blockers(board_t* board, uint64_t bishop_bitboard) {
    uint64_t m = get_occupancy(board) & bishop_lookup_table[to_position(bishop_bitboard)];
    m &= (~RANK_1) & (~RANK_8) & (~FILE_A) & (~FILE_H);
    return m;
}

// all possible moves a bishop can make from each square (ignoring blocking pieces)
static void precompute_bishop_lookup_table() {
    bishop_lookup_table = malloc (64 * sizeof(uint64_t));
    if (!bishop_lookup_table) {
        return;
    }

    for (int i = 0; i < 64 ; i++) {
        bishop_lookup_table[i] = get_bishop_moves(to_bitboard(i));
    }
}

// computes the attack set given the mask of blockers and the bishop's bitboard
static uint64_t masked_blocker_to_attack_set(uint64_t bishop_bitboard, uint64_t blocker) {
    int rank = get_rank_index(bishop_bitboard);
    int file = get_file_index(bishop_bitboard);
    uint64_t bitboard = 0;

    // go in all 4 directions

    // NW
    int next_rank = rank + 1;
    int next_file = file + 1; 
    while (next_rank <= 7 && next_file <= 7) {
        uint64_t next = add_move(next_file, next_rank);
        bitboard |= next;
        if (next & blocker) {
            break;
        }
        next_rank++; next_file++;
    }

    // NE
    next_rank = rank + 1;
    next_file = file - 1; 
    while (next_rank <= 7 && next_file >= 0) {
        uint64_t next = add_move(next_file, next_rank);
        bitboard |= next;
        if (next & blocker) {
            break;
        }
        next_rank++; next_file--;
    }

    // SW
    next_rank = rank - 1;
    next_file = file + 1; 
    while (next_rank >= 0 && next_file <= 7) {
        uint64_t next = add_move(next_file, next_rank);
        bitboard |= next;
        if (next & blocker) {
            break;
        }
        next_rank--; next_file++;
    }

    // SE
    next_rank = rank - 1;
    next_file = file - 1; 
    while (next_rank >= 0 && next_file >=0) {
        uint64_t next = add_move(next_file, next_rank);
        bitboard |= next;
        if (next & blocker) {
            break;
        }
        next_rank--; next_file--;
    }

    return bitboard;
}

uint64_t generate_key(int square, uint64_t mask) {
    return (mask * bishop_magic[square]) >> (64 - BBits[square]);
}

uint64_t get_mask(uint64_t NW, uint64_t NE, uint64_t SW, uint64_t SE) {
    return to_bitboard(NW) | to_bitboard(NE) | to_bitboard (SW) | to_bitboard(SE);
}

int in_range_NW(uint64_t NW) {
    return !((to_bitboard(NW) & FILE_A) || (to_bitboard(NW) & RANK_8));
}

int in_range_NE(uint64_t NE) {
    return !((to_bitboard(NE) & FILE_H) || (to_bitboard(NE) & RANK_8));
}

int in_range_SW(uint64_t SW) {
    return !((to_bitboard(SW) & FILE_A) || (to_bitboard(SW) & RANK_1));
}

int in_range_SE(uint64_t SE) {
    return !((to_bitboard(SE) & FILE_H) || (to_bitboard(SE) & RANK_1));
}

// number of squares on the NW/NE/SW/SE diagonal
int distance_NW (uint64_t pos) {
    uint64_t d = -1;
    while (in_range_NW(pos)) {
        d++;
        pos += 7; 
    }
    if (d == -1) {
        return 0;
    }
    return d;
}

int distance_NE (uint64_t pos) {
    uint64_t d = -1;
    while (in_range_NE(pos)) {
        d++;
        pos += 9; 
    }
    if (d == -1) {
        return 0;
    }
    return d;
}

int distance_SW (uint64_t pos) {
    uint64_t d = -1;
    while (in_range_SW(pos)) {
        d++;
        pos -= 9; 
    }
    if (d == -1) {
        return 0;
    }
    return d;
}

int distance_SE (uint64_t pos) {
    uint64_t d = -1;
    while (in_range_SE(pos)) {
        d++;
        pos -= 7; 
    }
    if (d == -1) {
        return 0;
    }
    return d;
}

////////////////////////////////////////

// generates NW/NE/SW/SE occupancy which is represented by the bits of n
// example: n = 3 (011 in binary) => NW diagonal occupancy will be 011
uint64_t fill_NW(uint64_t pos, int n) {
    uint64_t mask = 0;
    for (int i = 1; i <= distance_NW(pos); i++) {
        mask |= get_bit(n, i - 1) << (pos + 7 * i);
    }
    return mask;
}

uint64_t fill_NE(uint64_t pos, int n) {
    uint64_t mask = 0;
    for (int i = 1; i <= distance_NE(pos); i++) {
        mask |= get_bit(n, i - 1) << (pos + 9 * i);
    }
    return mask;
}

uint64_t fill_SW(uint64_t pos, int n) {
    uint64_t mask = 0;
    for (int i = 1; i <= distance_SW(pos); i++) {
        mask |= get_bit(n, i - 1) << (pos - 9 * i);
    }
    return mask;
}

uint64_t fill_SE(uint64_t pos, int n) {
    uint64_t mask = 0;
    for (int i = 1; i <= distance_SE(pos); i++) {
        mask |= get_bit(n, i - 1) << (pos - 7 * i);
    }
    return mask;
}

////////////////////////////////////

static void array_of_consecutives(int n, int* v) {
    for (int i = 0; i < (1 << n); i++) {
        v[i] = i;
    }
}

void generate_bishop_hash_table_square(int position) {
    int NW[64], NE[64], SW[64], SE[64];
    int nw_dist = distance_NW(position);
    int ne_dist = distance_NE(position);
    int sw_dist = distance_SW(position);
    int se_dist = distance_SE(position);

    uint64_t bitboard = to_bitboard(position);

    array_of_consecutives(nw_dist, NW);
    array_of_consecutives(ne_dist, NE);
    array_of_consecutives(sw_dist, SW);
    array_of_consecutives(se_dist, SE);

    int nw, ne, sw, se;

    for (nw = 0; ; nw++) {
        for (ne = 0; ; ne++) {
            for (sw = 0; ; sw++) {
                for (se = 0; ; se++) {
                    uint64_t mask = fill_NW(position, NW[nw]) | fill_NE(position, NE[ne])
                                  | fill_SW(position, SW[sw]) | fill_SE(position, SE[se]);
                    uint64_t key = generate_key(position, mask);
                    bishop_hash_table[position][key] = masked_blocker_to_attack_set(bitboard, mask);

                    if (se == (1 << se_dist) - 1) {
                        break;
                    }
                }
                if (sw == (1 << sw_dist) - 1) {
                    break;
                }
            }
            if (ne == (1 << ne_dist) - 1) {
                break;
            }      
        }
        if (nw == (1 << nw_dist) - 1) {
            break;
        }
    }
}


void alloc_hash_table() {
    bishop_hash_table = calloc(64, sizeof(uint64_t*));
    for (int i = 0; i < 64; i++) {
        bishop_hash_table[i] = calloc(1 << BBits[i], sizeof(uint64_t));
    }
}

void generate_bishop_hash_table() {
    alloc_hash_table();

    for (int i = 0; i < 64; i++) {
        generate_bishop_hash_table_square(i);
    }
}

void precompute_bishop_data() {
    precompute_bishop_lookup_table();
    generate_bishop_hash_table();
}

void free_hash_table() {
    for (int i = 0; i < 64; i++) {
        free(bishop_hash_table[i]);
    }
    free(bishop_hash_table);
}

void free_bishop_data() {
    free(bishop_lookup_table);
    free_hash_table();
}

uint64_t get_attack_set_bishop(board_t* board, side_t side, uint64_t bishop_bitboard) {
    int position = to_position(bishop_bitboard);
    uint64_t mask = get_masked_blockers(board, bishop_bitboard);
    return bishop_hash_table[position][generate_key(position, mask)] & (~get_side(board, side));
}

static void add_quiet(arraylist_t *moves, board_t *board, side_t side, uint64_t bishop_bitboard, uint64_t move) {
    move_t source_square = to_position(bishop_bitboard);
    move_t target_square = to_position(move);

    // generate move
    move_t encoded_move = encode_move(source_square, target_square, M_QUIET, 0U);

    // add move to the move list
    push(moves, &encoded_move);
}

static void add_capture(arraylist_t *moves, board_t *board, side_t side, uint64_t bishop_bitboard, uint64_t move) {
    move_t source_square = to_position(bishop_bitboard);
    move_t target_square = to_position(move);
    move_t captured_piece = to_piece(board, target_square);

    // generate move
    move_t encoded_move = encode_move(source_square, target_square, M_CAPTURE, captured_piece);

    // add move to the move list
    push(moves, &encoded_move);
}

void add_bishop_moves(arraylist_t* moves, board_t* board, side_t side) {
    uint64_t b1 = 0, b2 = 0;
    get_each_bishop(board, side, &b1, &b2);
    uint64_t bishop_list[2]; bishop_list[0] = b1; bishop_list[1] = b2;

    uint64_t enemies;
    if(side == WHITE) {
        enemies = get_side(board, BLACK);
    } else {
        enemies = get_side(board, WHITE);
    }

    for (int i = 0; i < 2; i++) {
        if (bishop_list[i] == 0) {
            continue; // current bishop is dead
        }

        //uint64_t attack_set = get_attack_set_bishop(board, bishop_list[i]) & (~get_side(board, side));
        uint64_t attack_set = get_attack_set_bishop(board, side, bishop_list[i]);

        for (int j = 0; j < 64; j++) {
            uint64_t move = circular_left_shift(1, j);

            if (move & attack_set) {
                if (move & enemies) {
                    add_capture(moves, board, side, bishop_list[i], move);
                } else {
                    add_quiet(moves, board, side, bishop_list[i], move);
                }
            }
        }
    }
}

static uint64_t side_bishops_attack_board(board_t* board, side_t side, unsigned int position) {
    uint64_t bitboard = to_bitboard(position);
    uint64_t b1 = 0;
    uint64_t b2 = 0;
    get_each_bishop(board, side, &b1, &b2);

    uint64_t attack_board = 0;
   
    if (b1 != 0 && (get_attack_set_bishop(board, side, b1) & bitboard)) {
        attack_board |= b1;
    }

    if (b2 != 0 && (get_attack_set_bishop(board, side, b2) & bitboard)) {
        attack_board |= b2;
    }

    return attack_board;
}

uint64_t bishop_attack_board(board_t* board, unsigned int position) {
    return side_bishops_attack_board(board, WHITE, position) |
        side_bishops_attack_board(board, BLACK, position);
}