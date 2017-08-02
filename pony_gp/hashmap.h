#define MAX_SIZE 100

int hashmap_put(struct hashmap *h, char *key, const double value);
double hashmap_get(struct hashmap *h, char *key);
void hashmap_init(struct hashmap *h);
void print_hashmap(struct hashmap *h);

struct hashmap {
	char *keys[MAX_SIZE];
	double values[MAX_SIZE];
	int curr_index;
};