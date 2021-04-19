#ifndef __LOGIC__
#define __LOGIC__

#include "controller/controller.h"
#include "internal/models/board.h"

/*
 * Check the state of the controller's flags, generate moves if possible
 * and update the state of the board accordingly.
 * @params: controller -> pointer to a controller object
 *          board -> pointer to a board object
 * @returns: no return
 */
void do_internal_logic(controller_t *controller, board_t *board);


#endif