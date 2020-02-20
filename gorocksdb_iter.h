#include <stdlib.h>
#include "rocksdb/c.h"
#include "gorocksdb.h"

typedef struct gorocksdb_value_s {
	char   *val;
	size_t len;
} gorocksdb_value_t;

typedef struct gorocksdb_values_s {
	gorocksdb_value_t *values;
	size_t          values_cnt;
	size_t          values_alloc;
} gorocksdb_values_t;

int gorocksdb_iterator_get_values_for_prefix(rocksdb_iterator_t *it, char *prefix, size_t prefix_len, gorocksdb_values_t **batch);
char *gorocksdb_values_get(gorocksdb_values_t *batch, size_t value_num, size_t *value_len);
void gorocksdb_values_free(gorocksdb_values_t *batch);
size_t gorocksdb_values_length(gorocksdb_values_t *batch);
