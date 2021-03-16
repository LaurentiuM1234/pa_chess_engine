#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 25
#define PROTOVER_DEFAULT 1
#define INPUT_BUFFER_SIZE 30

// BIT_OPS

void set_bit(unsigned int* x, int position);

int is_set(unsigned int x, int position);

int get_bit(unsigned int x, int position);

void reset_bit(unsigned int* x, int position);

void print_binary(unsigned int c);

// STRING_OPS

void read_buffer(char* buffer);

int is_move(char* buffer);

// Functions to be used

int is_black();

int is_white();

void set_white();

void set_black();

int get_xboard_bit();

int get_protover_bit();

int get_new_bit();

int get_force_bit();

int get_go_bit();

int get_quit_bit();

int get_resign_bit();

int get_move_bit();

char* get_in_buffer();

int get_protover_setting();