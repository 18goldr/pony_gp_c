#define MAX_SIZE 100

int hashmap_put(struct hashmap *h, char *key, const double value);
double hashmap_get(struct hashmap *h, char *key);
struct hashmap *hashmap_init(void);
void hashmap_deinit(struct hashmap *h);
void print_hashmap(struct hashmap *h);
int hashmap_get_size(struct hashmap *h);
char *hashmap_get_key(struct hashmap *h, int i);
int hashmap_get_num_with_value(struct hashmap *h, int value);

struct hashmap {
	char *keys[MAX_SIZE];
	double values[MAX_SIZE];
	int curr_index;
};