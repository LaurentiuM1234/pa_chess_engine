#include "header.h"

// this structure is used to pass information
// to the next layers
typedef struct info{
	char move[BUFFER_SIZE]; // stores the latest move; e.g. 'e3e4'
	int setting; // protover parameter
} input_info;

// define macros for commands
#define OTHER -1
#define XBOARD 0
#define PROTOVER 1
#define NEW 2
#define FORCE 3
#define GO 4
#define QUIT 5
#define RESIGN 6
#define MOVE 7
#define COLOR 8

#define BLACK 100
#define WHITE 101

static input_info information = {.setting = PROTOVER_DEFAULT};

char* get_in_buffer(){
	return information.move;
}

int get_protover_setting(){
	return information.setting;
}

void set_protover_setting(char* buffer){
	// buffer has format: 'protover N'
	strtok(buffer, " ");
	information.setting = atoi(strtok(NULL, " "));
}

static unsigned int command = 0;

static void reset_command(){
	// reset all bits apart from color bit
	command &= (1 << COLOR);
}

void set_black();
void set_white();

// sets bit corresponding to position
static void set(int position){
	if(position == BLACK){
		set_black();
		return;
	}
	if(position == WHITE){
		set_white();
		return;
	}

	if(position == NEW){
		set_black();
	}

	set_bit(&command, position);
}

static void reset(int position){
	reset_bit(&command, position);
}

// Color bit is 1 for Black
int is_black(){
	return get_bit(command, COLOR);
}

// Color bit is 0 for White
int is_white(){
	return !is_black();
}

void set_white(){
	reset(COLOR);
}

void set_black(){
	set_bit(&command, COLOR);
}

int get_xboard_bit(){
	return get_bit(command, XBOARD);
}

int get_protover_bit(){
	return get_bit(command, PROTOVER);
}

int get_new_bit(){
	return get_bit(command, NEW);
}

int get_force_bit(){
	return get_bit(command, FORCE);
}

int get_go_bit(){
	return get_bit(command, GO);
}

int get_quit_bit(){
	return get_bit(command, QUIT);
}

int get_resign_bit(){
	return get_bit(command, RESIGN);
}

int get_move_bit(){
	return get_bit(command, MOVE);
}

static int get_command_code(char* string){
	if(!strcmp(string, "xboard")){
		return XBOARD;
	}
	if(strstr(string, "protover")){
		return PROTOVER;
	}
	if(!strcmp(string, "new")){
		return NEW;
	}
	if(!strcmp(string, "force")){
		return FORCE;
	}
	if(!strcmp(string, "go")){
		return GO;
	}
	if(!strcmp(string, "quit")){
		return QUIT;
	}
	if(!strcmp(string, "resign")){
		return RESIGN;
	}
	if(is_move(string)){
		return MOVE;
	}
	if(!strcmp(string, "black")){
		return BLACK;
	}
	if(!strcmp(string, "white")){
		return WHITE;
	}
	return OTHER; // other command
}

static void process_command(char* buffer){
	// get input
	int code = get_command_code(buffer);
	if(code == OTHER){
		return;
	}
	// sets the bit corresponding to the code
	set(code);

	//sets protover setting
	if(code == PROTOVER){
		set_protover_setting(buffer);
		return;
	}
	// updates move buffer
	if(code == MOVE){
		strcpy(information.move, buffer);
	}
}

void read_input(){
	reset_command();
	char input[INPUT_BUFFER_SIZE];
	read_buffer(input);
	process_command(input);
}