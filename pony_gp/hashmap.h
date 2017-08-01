#define MAX_SIZE 100

int hashmap_put(struct hashmap *h, const char key, const int values);
int hashmap_get(struct hashmap *h, const char key);
void hashmap_init(struct hashmap *h);
void print_hashmap(struct hashmap *h);

struct hashmap {
	char keys[MAX_SIZE];
	int values[MAX_SIZE];
	int curr_index;
};