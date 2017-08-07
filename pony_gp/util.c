#include "util.h"

double SEED;


/**
 * Set the seed for the random number generator.
 * Useful for testing purposes.
	 s: Value that SEED will be set to.
 */
void set_seed(double s) {
	SEED = s;
}

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
	// srand must only be instantiated once for different results each time.
	static bool first_time = true;

	if (SEED != 0) {
		srand((unsigned int)SEED);
	}

	else if (first_time) {
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


/**
 * Return an array of random indixes.
	 length: Length of the array.
 */
int *random_indexes(int length) {
	int *indexes = malloc(sizeof(int) * length);

	for (int i = 0; i < length; i++) {
		indexes[i] = i;
	}


	shuffle(indexes, length);

	return indexes;
}


/**
 * Arrange the `n` elements of the array in random order.
 * N must be must smaller than RAND_MAX to be effective.
	 *array: The array to shuffle.
		  n: The size of the array.
 */
void shuffle(int *array, int n) {
	start_srand();

	if (n > 1) {

		for (int i = 0; i < n; i++) {
			int j = 1 + rand() / (RAND_MAX / (n - i) + 1);

			int temp = array[j];

			array[j] = array[i];
			array[i] = temp;
		}
	}
}

/**
 * Seperate a string by the delimeter.
	   **str: String to modify
	  *delim: The character to seperate the string by.
 */
char *str_sep(char **str, const char* delim) {

	char* start = *str;
	char* p;

	p = (start != NULL) ? strpbrk(start, delim) : NULL;

	if (!p) {
		*str = NULL;
	}
	else {
		*p = '\0';
		*str = p + 1;
	}

	return start;
}


/**
 * Remove the spaces from a string.
	 *str: The string to modify.
 */
void remove_spaces(char *str) {
	if (!str) return;

	char* i = str;
	char* j = str;

	while (*j != 0) {
		*i = *j++;
		if (*i != ' ') i++;
	}

	*i = 0;
}
