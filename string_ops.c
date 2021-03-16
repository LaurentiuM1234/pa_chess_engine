#include "header.h"

#define SAN0 22
#define SAN1 18

static void reset_buffer(char* buffer){
	for(int i = 0; i < INPUT_BUFFER_SIZE; i++){
		buffer[i] = '\0';
	}
}

static void remove_newline(char* buffer){
	if(buffer[0] == '\n'){
		return;
	}
	for(int i = 1; i < INPUT_BUFFER_SIZE; i++){
		if(buffer[i] == '\n'){
			buffer[i] = '\0';
			return;
		}
	}
}

void read_buffer(char* buffer){
	reset_buffer(buffer);
	read(0, buffer, INPUT_BUFFER_SIZE);
	//fgets(buffer, INPUT_BUFFER_SIZE, stdin);
	remove_newline(buffer);
}

static int is_in_range_a_h(char c){
	if(c >= 'a' && c <= 'h'){
		return 1;
	}
	return 0;
}

static int is_in_range_1_8(char c){
	if(c >= '1' && c <= '8'){
		return 1;
	}
	return 0;
}

static int is_uppercase(char c){
	if(c >= 'A' && c <= 'Z'){
		return 1;
	}
	return 0;
}

// checks if a move is in san=0 format
static int check_san_0(char* buffer){
	if(strlen(buffer) < 4){
		return 0;
	}
	if(is_in_range_a_h(buffer[0]) && is_in_range_1_8(buffer[1]) 
		&& is_in_range_a_h(buffer[2]) && is_in_range_1_8(buffer[3])){
		return 1;
	}
	return 0;
}

// checks if a move is in san=1 format
static int check_san_1(char* buffer){
	if(strlen(buffer) < 3){
		return 0;
	}

	if(is_uppercase(buffer[0]) && is_in_range_a_h(buffer[1]) 
		&& is_in_range_1_8(buffer[2])){
		return 1;
	}
	return 0;
}

static int check_castling(char* buffer){
	if((strlen(buffer) != SAN0 && strlen(buffer) != SAN1)){
		return 0;
	}
	int (*check)(char*);
	int format;

	if(strlen(buffer) == SAN0){
		format = 4;
		check = &check_san_0;
	}

	if(strlen(buffer) == SAN1){
		format = 3;
		check = &check_san_1;
	}
	char* s = buffer;
	while(*s != '\0'){
		if(!check(s)){
			return 0;
		}
		s += (format + 2);
	}
	return 1;
}

static int is_normal_move(char* buffer){
	if(strlen(buffer) < 3){
		return 0;
	}
	if(strlen(buffer) == 3 && check_san_1(buffer)){
		return 1;
	}
	if(strlen(buffer) == 4 && check_san_0(buffer)){
		return 1;
	}
	return 0;
}

static int check_pawn_promotion(char* buffer){
	if(strlen(buffer) < 4){
		return 0;
	}
	if(strlen(buffer) == 4 && check_san_1(buffer)){
		return 1;
	}
	if(strlen(buffer) == 5 && check_san_0(buffer)){
		return 1;
	}
	return 0;
}

// checks if a string is a valid move
int is_move(char* buffer){
	if(is_normal_move(buffer)){
		return 1;
	}
	if(check_pawn_promotion(buffer)){
		return 1;
	}
	return check_castling(buffer);
}