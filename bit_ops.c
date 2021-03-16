#include "header.h"

#define BIT_COUNT 8 * sizeof(unsigned int)

static int in_range(int position){
	if(position < 0 || position > (BIT_COUNT - 1)){
		return 0;
	}
	return 1;
}

// verifies if the bit at position "position" is set;
// position can have values between 0 and BIT_COUNT - 1
// 0 -> LSB; BIT_COUNT - 1 -> MSB
int is_set(unsigned int x, int position){
	if(!in_range(position)){
		return -1;
	}
	unsigned int mask = 1 << position;
	if((mask & x) == 0){
		return 0;
	}
	return 1;
}

// sets bit to 1
void set_bit(unsigned int* x, int position){
	if(!in_range(position)){
		return;
	}

	unsigned int mask = 1 << position;
	*x |= mask;
	return;
}

// gets a bit by position
int get_bit(unsigned int x, int position){
	return is_set(x, position);
}

// sets bit to 0
void reset_bit(unsigned int* x, int position){
	if(!in_range(position)){
		return;
	}

	unsigned int mask = ~(1 << position);
	*x &= mask;
	return;
}

// debug function
void print_binary(unsigned int c){
	for(int pos = BIT_COUNT - 1; pos >= 0; pos--){
		printf("%d", is_set(c, pos));
	}
	printf("\n");
}