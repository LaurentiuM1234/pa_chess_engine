#include "controller/controller.h"
#include "internal/models/board.h"
#include "debug/fatal.h"
#include "logic.h"

int main()
{
    board_t *board = create_board();

    precompute_tables();

    if (!board)
        FATAL_ERROR("Unable to create board.");

    controller_t *controller = create_controller();

    if (!controller) {
        destroy_board(&board);
        FATAL_ERROR("Unable to create controller.");
    }

    do_handshake(controller);

    while (!is_set(controller, C_QUIT)) {
        listen(controller);
        do_internal_logic(controller, board);
        inform(controller);
    }

    destroy_board(&board);
    destroy_controller(&controller);

    free_tables();
}