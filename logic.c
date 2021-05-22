#include "logic.h"
#include "internal/containers/arraylist.h"
#include "internal/generators/generate.h"
#include "internal/update.h"
#include "internal/eval.h"
#include <string.h>

#define minus_inf -2147483647
#define plus_inf 2147483647

#define DEPTH 4

void do_internal_logic(controller_t *controller, board_t *board)
{
    if (is_set(controller, C_NEW)) {
        // if a new game has begun, clear the board
        clear_board(board);

        // turn off the flag
        clear_flag(controller, C_NEW);
    }
    if (!is_set(controller, C_FORCE)) {
        if (strlen(get_in_buffer(controller)) != 0) {
            // a move has been given to the engine
            move_t in_move = to_move(get_in_buffer(controller));
            update(board, is_set(controller, C_ES), in_move, 0U);

            // cleaning in buffer after move is made
            memset(get_in_buffer(controller), 0, IN_BUF_SIZE);
        }

        if (!is_set(controller, C_FORCE) && (is_set(controller, C_STM) == is_set(controller, C_ES))) {
            arraylist_t *available_moves = generate_moves(board, is_set(controller, C_ES));
            if (size(available_moves) == 0) {
                // no move to make, set resign flag
                set_flag(controller, C_RESIGN);
                return;
            }
            // move_t best_move = select_move(board, eval, is_set(controller, C_ES), alpha=minus_inf, beta=plus_inf, depth, &best_move);

            //move_t best_move = select_move(board, eval, is_set(controller, C_ES), minus_inf, plus_inf, DEPTH, &best_move);

            // decides best move
            move_t best_move;
            // best_move = (select_move()) -> move;
            best_move = (select_move(board, eval, available_moves, is_set(controller, C_ES),
                         minus_inf, plus_inf, DEPTH)).move;

            /*print_move(best_move);
            printf("primary fnc checks:\n");
            printf("\n%d\n", get_white_checks_num(board));
            printf("%d\n\n", get_black_checks_num(board));*/

            // updating board
            update(board, is_set(controller, C_ES), 0U, best_move);

            // setting has move flag
            set_flag(controller, C_HAS_MOVE);

            // writing move to output buffer
            to_string(get_out_buffer(controller), best_move);

            // changing side to move
            toggle_flag(controller, C_STM);

            // destroying list of moves
            //destroy_list(&available_moves);
        }
    } else {
        if (strlen(get_in_buffer(controller)) != 0) {
            // a move has been given to the engine
            move_t in_move = to_move(get_in_buffer(controller));
            update(board, is_set(controller, C_STM), in_move, 0U);

            // cleaning in buffer after move is made
            memset(get_in_buffer(controller), 0, IN_BUF_SIZE);
        }
    }
}