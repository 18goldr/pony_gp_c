#define MAX_SIZE 100

void hashmap_put(struct hashmap *h, char key, int value);
int hashmap_get(struct hashmap *h, char key);
void hashmap_init(struct hashmap *h);
void print_hashmap(struct hashmap *h);

struct hashmap {
	char keys[MAX_SIZE];
	int values[MAX_SIZE];
	int curr_index;
};