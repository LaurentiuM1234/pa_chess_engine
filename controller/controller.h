#ifndef __CONTROLLER__
#define __CONTROLLER__

#define IN_BUF_SIZE 100
#define OUT_BUF_SIZE 100
#define CMD_BUF_SIZE 100

/*=========FLAG CONSTANTS========*/
#define C_RESIGN 0U
#define C_QUIT 1U
#define C_FORCE 2U
#define C_NEW 3U
#define C_ES 4U   // engine's side
#define C_STM 5U  // side to move flag position
#define C_HAS_MOVE 6U
/*===============================*/


typedef struct controller controller_t;

/*
 * Create a controller object.
 * @returns: newly allocated controller object
 */
controller_t *create_controller(void);

/*
 * Safe delete a controller object.
 * @params: controller -> reference of pointer to a controller object
 * @returns: no return
 */
void destroy_controller(controller_t **controller);

/*
 * Handle initial handshake between engine and XBoard.
 * @params: controller -> pointer to a controller object
 * @returns: no return
 */
void do_handshake(controller_t *controller);

/*
 * Parse the commands given by XBoard and set flags and buffers accordingly.
 * @params: controller -> pointer to a controller object
 * @returns: no return
 */
void listen(controller_t *controller);

/*
 * Give information to XBoard based on the state of the controller's flags and output buffer.
 * @params: controller -> pointer to a controller object
 * @returns: no return
 */
void inform(controller_t *controller);

/*
 * Retrieve controller's input buffer.
 * @params: controller -> pointer to a controller object
 * @returns: controller's input buffer
 */
char *get_in_buffer(controller_t *controller);

/*
 * Retrieve controller's output buffer.
 * @params: controller -> pointer to a controller object
 * @returns: controller's output buffer
 */
char *get_out_buffer(controller_t *controller);

/*====================FLAG OPERATIONS========================*/

/*
 * Set a controller flag found at given position.
 * @params: controller -> pointer to a controller object
 *          position -> the position of the flag to be set
 * @returns: no return
 */
void set_flag(controller_t *controller, unsigned int position);

/*
 * Clear a controller flag found at given position.
 * @params: controller -> pointer to a controller object
 *          position -> the position of the flag to be set
 * @returns: no return
 */
void clear_flag(controller_t *controller, unsigned int position);

/*
 * Check if a controller flag found at a given position is set.
 * @params: controller -> pointer to a controller object
 *          position -> the position of the flag to be set
 * @returns: 1 if the flag is set, 0 otherwise
 */
int is_set(controller_t *controller, unsigned int position);

/*
 * Toggle a controller flag found at given position.
 * @params: controller -> pointer to a controller object
 *          position -> the position of the flag to be set
 * @returns: no return
 */
void toggle_flag(controller_t *controller, unsigned int position);

/*===========================================================*/



#endif