#define MAX_SIZE 100

int hashmap_put(struct hashmap *h, char *key, const double value);
double hashmap_get(struct hashmap *h, char *key);
struct hashmap *hashmap_init(void);
void hashmap_deinit(struct hashmap *h);
void print_hashmap(struct hashmap *h);

struct hashmap {
	char *keys[MAX_SIZE];
	double values[MAX_SIZE];
	int curr_index;
};