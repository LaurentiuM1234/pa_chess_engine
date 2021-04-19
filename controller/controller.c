#include "controller.h"
#include "../util.h"
#include "../debug/fatal.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


#define ENGINE_FEATURES "feature sigint=0 san=0 myname=\"SURPY SOFTISTU CHESS ENGINE\"\n"

struct controller {
    char _in_buffer[IN_BUF_SIZE];
    char _out_buffer[OUT_BUF_SIZE];
    char _cmd_buffer[CMD_BUF_SIZE];
    uint16_t _flags;
};

controller_t *create_controller(void)
{
    controller_t *controller = calloc(1, sizeof(controller_t));

    if (!controller)
        return NULL;

    // by default, engine will play for blacks
    set_flag(controller, C_ES);

    return controller;
}

void destroy_controller(controller_t **controller)
{
    free(*controller);
    *controller = NULL;
}

static void read_input(char *buffer, int buffer_size)
{
    // reading input from stdin
    fgets(buffer, buffer_size, stdin);

    // removing trailing newline
    buffer[strlen(buffer) - 1] = '\0';
}

void set_flag(controller_t *controller, unsigned int position)
{
    controller->_flags |= (1U << position);
}

void clear_flag(controller_t *controller, unsigned int position)
{
    controller->_flags &= ~(1U << position);
}

int is_set(controller_t *controller, unsigned int position)
{
    return (controller->_flags & (1U << position)) >> position;
}

void toggle_flag(controller_t *controller, unsigned int position)
{
    controller->_flags ^= (1U << position);
}

static void clear_all(controller_t *controller)
{
    controller->_flags = 0U;
}

static void process_command(controller_t *controller)
{
    if (strcmp(controller->_cmd_buffer, "quit") == 0) {
        set_flag(controller, C_QUIT);
    } else if (strcmp(controller->_cmd_buffer, "force") == 0) {
        set_flag(controller, C_FORCE);
    } else if (strcmp(controller->_cmd_buffer, "go") == 0) {
        clear_flag(controller, C_FORCE);

        if (is_set(controller, C_STM))
            set_flag(controller, C_ES);
        else
            clear_flag(controller, C_ES);

    } else if (strcmp(controller->_cmd_buffer, "new") == 0) {
        // clearing all previously set flags
        clear_all(controller);
        // setting new game flag for board cleaning
        set_flag(controller, C_NEW);
        // setting engine side flag
        set_flag(controller, C_ES);
    }
}

void do_handshake(controller_t *controller)
{
    // read the xboard command
    read_input(controller->_cmd_buffer, CMD_BUF_SIZE);

    // check command validity
    if (strcmp(controller->_cmd_buffer, "xboard") != 0)
        FATAL_ERROR("Wrong connection command.");

    // read protocol command if there
    read_input(controller->_cmd_buffer, CMD_BUF_SIZE);

    // if the command is not 'protover N', then no handshake can be done
    if (strncmp(controller->_cmd_buffer, "protover", 8) != 0)
        return;

    // sending supported features
    printf(ENGINE_FEATURES);

    // flushing stdout buffer
    fflush(stdout);

    // cleaning the buffer
    memset(controller->_cmd_buffer, 0, CMD_BUF_SIZE);
}

void listen(controller_t *controller)
{
    // read next command only if the buffer is not occupied
    if (strlen(controller->_cmd_buffer) == 0) {
        read_input(controller->_cmd_buffer, CMD_BUF_SIZE);
    }

    if (is_move(controller->_cmd_buffer)) {

        // copying command to in move buffer
        memcpy(controller->_in_buffer, controller->_cmd_buffer, IN_BUF_SIZE);

        // after a move is made change side to move
        toggle_flag(controller, C_STM);

    } else {
        process_command(controller);
    }

    // cleaning command buffer
    memset(controller->_cmd_buffer, 0, CMD_BUF_SIZE);
}

void inform(controller_t *controller)
{
    if (is_set(controller, C_HAS_MOVE)) {
        // printing move if available
        printf("%s\n", controller->_out_buffer);

        // cleaning the move buffer
        memset(controller->_out_buffer, 0, OUT_BUF_SIZE);

        // turning off flag
        clear_flag(controller, C_HAS_MOVE);

        // flushing stdout buffer
        fflush(stdout);
    }
    if (is_set(controller, C_RESIGN)) {
        // informing xboard engine has resigned
        printf("resign\n");

        // flushing stdout buffer
        fflush(stdout);
    }
}

char *get_in_buffer(controller_t *controller)
{
    return controller->_in_buffer;
}

char *get_out_buffer(controller_t *controller)
{
    return controller->_out_buffer;
}
