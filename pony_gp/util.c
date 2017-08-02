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


/**
 * Instantiate srand. Can only be performed once per execution.
 */
void start_srand() {
	// srand must only be instantiated once.
	static bool first_time = true;

	if (first_time) {
		srand((unsigned int)time(NULL));
		first_time = false;
	}
}

/*
* Return a random int between min and max inclusive.
	min: The minimum value.
	max: The maxmium value.
*/
int get_randint(int min, int max) {
	start_srand();

	return (rand() % (max + 1 - min)) + min;

}


/**
 * Return a random probability. Values are 0.0 to 1.0 inclusive.
 */
double get_rand_probability() {
	start_srand();

	return (double)rand() / (double)RAND_MAX;
}


/*
* Get a random index of an array.
	length: The length of the array.
*/
int rand_index(int length) {
	return get_randint(0, length - 1);
}