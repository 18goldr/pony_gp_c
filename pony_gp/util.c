#include "util.h"

/*
* Array must end in Null character.
	*arr: The array to parse.
*/
int get_char_arr_length(char *arr) {
	int length = 1;

	while (*arr) {
		length++;
		arr++;
	}

	return length;

}

/*
* Return a random int between min and max inclusive.
	min: The minimum value.
	max: The maxmium value.
*/
int get_rand(int min, int max) {
	// srand must only be instantiated once.
	static bool first_time = true;
	if (first_time) srand((unsigned int)time(NULL));

	first_time = false;

	return (rand() % (max + 1 - min)) + min;

}

/*
* Get a random index of an array.
	length: The length of the array.
*/
int rand_index(int length) {
	return get_rand(0, length - 1);
}