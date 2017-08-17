#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "util.h"

// the seed to pass to the random function
// default to 0
double SEED = 0;

/**
* Set the seed for the random number generator.
* Useful for testing purposes.
*	 s: Value that SEED will be set to.
*/
void set_seed(double s) {
	SEED = s;
}

/**
* Get the length of a character array/string
* Array must end in Null character.
*	arr: The array to parse.
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
* Get the length of a double array.
* Array must end in the value NAN defined in <math.h>
*	arr: The array to parse.
*/
int get_double_arr_length(double *arr) {
	int length = 0;

	while (!isnan(*arr)) {
		length++;
		arr++;
	}

	return length;
}

/**
* Get the length of a 2 dimensional array of doubles. Each outer array must end
* with a NULL pointer. Each inner array must end with the NAN value
* defined in <math.h>
*	 arr: The 2D array to parse.
*/
int get_2d_arr_length(double **arr) {
	int length = 0;

	while (*arr) {
		length++;
		arr++;
	}

	return length;
}


/**
* Call srand. Can only be performed once per execution.
*/
void start_srand() {
	// srand must only be called once for different results each time.
	static bool first_time = true;

	if (first_time) {
		srand((unsigned int)(SEED ? SEED : time(NULL)));
		first_time = false;
	}
}

/*
* Return a random int between min and max inclusive.
*	min: The minimum value.
*	max: The maxmium value.
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
*	length: The length of the array.
*/
int rand_index(int length) {
	return get_randint(0, length-1);
}


/**
* Return an array of random indexes.
*	 length: Length of the array.
*/
int *random_indexes(int length) {
	int *indexes = fill_array_indexes(length);

	shuffle(indexes, length);

	return indexes;
}

/**
* Return an array of a random sample of indexes.
*	      length: The length of the array.
*	 sample_size: The size of the sample array to return.
*/
int *random_sample(int length, int sample_size) {
	int *all_indexes = fill_array_indexes(length);

	int *random_indexes = malloc(sizeof(int) * sample_size);

	for (int i = 0; i < sample_size; i++) {
		int rand_i = rand_index(length);
		random_indexes[i] = rand_i;

		// Swap last value with random value
		int tmp = all_indexes[rand_i];
		all_indexes[rand_i] = all_indexes[length - 1];
		all_indexes[length - 1] = tmp;
		
		// Make the last value unavailable
		length--;
	}
	free(all_indexes);

	return random_indexes;
}

/**
* Return an array filled with numbers 1 through length-1.
*	 length: The length of the array to return.
*/
int *fill_array_indexes(int length) {
	int *indexes = malloc(sizeof(int) * length);

	for (int i = 0; i < length; i++) {
		indexes[i] = i;
	}

	return indexes;
}


/**
* Arrange the `n` elements of the array in random order.
* `n` must be must smaller than RAND_MAX to be effective.
*	 array: The array to shuffle.
*		 n: The size of the array.
*/
void shuffle(int *array, int n) {
	start_srand();

	for (int i = n - 1; i > 0; i--) {
		int j = rand() % (i + 1);

		swap(&array[i], &array[j]);
	}
}

/**
* Utility function to swap two integers.
*	a, b: The integers to swap.
*/
void swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}


/**
* Seperate a string by the delimeter.
*	   str: String to modify
*	 delim: The character to seperate the string by.
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
*	 str: The string to modify.
*/
void remove_spaces(char *str) {
	if (!str) return;

	char* i = str;
	char* j = str;

	while (*j) {
		*i = *j++;
		if (*i != ' ') i++;
	}

	*i = 0;
}

/**
* Print a 2 dimensional array of doubles. Each outer array must end
* with a NULL pointer. Each inner array must end with the NAN value
* defined in <math.h>
	 arr: The 2D array to print.
*/
void print_2d_array(double **arr) {
	while (*arr) {
		while (!isnan(**arr)) {
			printf("%f\n", *(*arr)++);
		}

		arr++;
	}
}

/**
* Return if a string is in an array.
*	 strings: The array of strings to parse.
*	     str: The string to parse for.
*		size: The size of the array of strings.
*/
bool str_in_arr(char **strings, char *str, int size) {
	
	for (int i = 0; i < size; i++) {
		if (!strcmp(strings[i], str)) {
			return true;
		}
	}

	return false;
}

/**
* Return average and standard deviation.
*	 values: Values to calculate on.
*	   size: The number of elements in values.
*/
double *get_ave_and_std(double *values, int size) {
	double ave = sum_doubles(values, size) / size;
	double std = get_std(values, size, ave);

	double *results = malloc(sizeof(double) * 2);
	results[0] = ave;
	results[1] = std;

	return results;
}

/**
* Return the sum of the doubles in an array.
*	 values: The array.
*	   size: The size of the array.
*/
double sum_doubles(double *values, int size) {
	double total = 0;

	for (int i = 0; i < size; i++) {
		total += values[i];
	}

	return total;
}

/**
* Return the standard deviation of a set of values.
*	 values: The array of values.
*	   size: The size of the array.
*		ave: The average of the values.
*/
double get_std(double *values, int size, double ave) {
	double std = 0.0;

	for (int i = 0; i < size; ++i) {
		std += pow(values[i] - ave, 2);
	}

	return sqrt(std / 10);
}

/**
* Get the length of a double, provided the user knows
* the  number of digits after the decimal point to use.
*	 d: The double.
*	 d_places: The number of digits after the decimal point
*/
int double_length(double d, int d_places) {
	int d_int = (int)d;

	if (!d_int) return 1 + d_places;

	int n_digs = (int)floor(log10(abs(d_int))) + 1;

	return n_digs + d_places;
}


/**
* Get the max value of all doubles in an array.
*	 values: The array to parse.
*	   size: The size of the array.
*/
double max_value(double *values, int size) {
	double max = values[0];

	for (int i = 1; i < size; i++) {
		if (values[i] > max) max = values[i];
	}

	return max;
}
