#include <string.h>
#include "gorocksdb.h"
#include "gorocksdb_iter.h"
#include "_cgo_export.h"

int gorocksdb_iterator_valid_for_prefix(rocksdb_iterator_t *it, char *prefix, size_t prefix_len)
{
	if (rocksdb_iter_valid(it) == 0) {
		return -1;
	}

	size_t key_len;
	const char *key;

	key = rocksdb_iter_key(it, &key_len);
	if (key == NULL) {
		return -1;
	}

	if (prefix_len > key_len) {
		return -1;
	}

	int result = memcmp(prefix, key, prefix_len);
	if (result != 0) {
		return -1;
	}
	return 0;
}

#define DEFAULT_VALUES_ALLOC_NUM 1024

int gorocksdb_iterator_get_values_for_prefix(rocksdb_iterator_t *it, char *prefix, size_t prefix_len, gorocksdb_values_t **batch)
{
	*batch = (gorocksdb_values_t *)malloc(sizeof(gorocksdb_values_t));
	if (*batch == NULL) {
		return -1;
	}

	(*batch)->values = NULL;
	(*batch)->values_cnt = 0;
	(*batch)->values_alloc = DEFAULT_VALUES_ALLOC_NUM;

	(*batch)->values = (gorocksdb_value_t *)malloc(sizeof(gorocksdb_value_t) * (*batch)->values_alloc);
	if ((*batch)->values == NULL) {
		free(*batch);
		return -1;
	}

	for (rocksdb_iter_seek(it, prefix, prefix_len);
	     gorocksdb_iterator_valid_for_prefix(it, prefix, prefix_len) == 0;
	     rocksdb_iter_next(it)) {
		size_t current_value_len;
		const char *current_value;
		char *current_value_copy;

		current_value = rocksdb_iter_value(it, &current_value_len);
		if (current_value == NULL || current_value_len == 0) {
			continue;
		}

		current_value_copy = (char *)malloc(current_value_len + 1);
		if (current_value_copy == NULL) {
			goto cleanup;
		}

		memcpy(current_value_copy, current_value, current_value_len);
		current_value_copy[current_value_len] = '\0';

		(*batch)->values[(*batch)->values_cnt].val = current_value_copy;
		(*batch)->values[(*batch)->values_cnt].len = current_value_len;
		(*batch)->values_cnt++;

		if ((*batch)->values_cnt == (*batch)->values_alloc) {
			(*batch)->values_alloc += DEFAULT_VALUES_ALLOC_NUM;
			(*batch)->values = (gorocksdb_value_t *)realloc((*batch)->values, sizeof(gorocksdb_value_t) * (*batch)->values_alloc);
			if ((*batch)->values == NULL) {
				return -1;
			}
		}
	}
	return 0;

cleanup:
	gorocksdb_values_free(*batch);
	return 0;
}

char *gorocksdb_values_get(gorocksdb_values_t *batch, size_t value_num, size_t *value_len)
{
	if (value_num >= batch->values_cnt) {
		return NULL;
	}

	*value_len = batch->values[value_num].len;
	return batch->values[value_num].val;
}

void gorocksdb_values_free(gorocksdb_values_t *batch)
{
	for (size_t i = 0; i < batch->values_cnt; i++) {
		free(batch->values[i].val);
	}
	free(batch->values);
	free(batch);
}

size_t gorocksdb_values_length(gorocksdb_values_t *batch)
{
	return batch->values_cnt;
}
