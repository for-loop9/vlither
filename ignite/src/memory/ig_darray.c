#include "ig_darray.h"
#include <string.h>
#include <stdio.h>

size_t* _ig_darray_get_fields(void* darray) {
	return darray - (_IG_DARRAY_FIELD_LENGTH * sizeof(size_t));
}

void* _ig_darray_create(size_t stride) {
	void* r = malloc(_IG_DARRAY_FIELD_LENGTH * sizeof(size_t) + stride) + _IG_DARRAY_FIELD_LENGTH * sizeof(size_t);
	size_t* fields = _ig_darray_get_fields(r);
	fields[_IG_DARRAY_LENGTH] = 0;
	fields[_IG_DARRAY_STRIDE] = stride;
	fields[_IG_DARRAY_CAPACITY] = 1;

	return r;
}

void _ig_darray_insert(void** darray, size_t i, void* value_ptr) {
	size_t* fields = _ig_darray_get_fields(*darray);
	if (fields[_IG_DARRAY_LENGTH] >= fields[_IG_DARRAY_CAPACITY]) {
		void* beg = *darray - _IG_DARRAY_FIELD_LENGTH * sizeof(size_t);
		beg = realloc(beg, _IG_DARRAY_FIELD_LENGTH * sizeof(size_t) + (fields[_IG_DARRAY_CAPACITY] *= 2) * fields[_IG_DARRAY_STRIDE]);
		*darray = beg + _IG_DARRAY_FIELD_LENGTH * sizeof(size_t);
		fields = _ig_darray_get_fields(*darray);

	}
	memcpy(*darray + (i + 1) * fields[_IG_DARRAY_STRIDE], *darray + i * fields[_IG_DARRAY_STRIDE], (fields[_IG_DARRAY_LENGTH] - i) * fields[_IG_DARRAY_STRIDE]);
	fields[_IG_DARRAY_LENGTH]++;
	memcpy(*darray + i * fields[_IG_DARRAY_STRIDE], value_ptr, fields[_IG_DARRAY_STRIDE]);
}

void _ig_darray_push(void** darray, void* value_ptr) {
	size_t* fields = _ig_darray_get_fields(*darray);

	if (fields[_IG_DARRAY_LENGTH] >= fields[_IG_DARRAY_CAPACITY]) {
		void* beg = *darray - _IG_DARRAY_FIELD_LENGTH * sizeof(size_t);
		beg = realloc(beg, _IG_DARRAY_FIELD_LENGTH * sizeof(size_t) + (fields[_IG_DARRAY_CAPACITY] *= 2) * fields[_IG_DARRAY_STRIDE]);
		*darray = beg + _IG_DARRAY_FIELD_LENGTH * sizeof(size_t);
		fields = _ig_darray_get_fields(*darray);
	}

	memcpy(*darray + fields[_IG_DARRAY_LENGTH] * fields[_IG_DARRAY_STRIDE], value_ptr, fields[_IG_DARRAY_STRIDE]);
	fields[_IG_DARRAY_LENGTH]++;
}

void _ig_darray_pop(void* darray) {
	size_t* fields = _ig_darray_get_fields(darray);
	fields[_IG_DARRAY_LENGTH]--;
}

void _ig_darray_remove(void* darray, size_t i) {
	size_t* fields = _ig_darray_get_fields(darray);
	fields[_IG_DARRAY_LENGTH]--;
	memcpy(darray + i * fields[_IG_DARRAY_STRIDE], darray + (i + 1) * fields[_IG_DARRAY_STRIDE], (fields[_IG_DARRAY_LENGTH] - i) * fields[_IG_DARRAY_STRIDE]);
}

size_t _ig_darray_length(void* darray) {
	return _ig_darray_get_fields(darray)[_IG_DARRAY_LENGTH];
}

size_t _ig_darray_memory(void* darray) {
	size_t* fields = _ig_darray_get_fields(darray);
	return fields[_IG_DARRAY_CAPACITY] * fields[_IG_DARRAY_STRIDE];
}

int _ig_darray_find(void* darray, void* value_ptr) {
	size_t* fields = _ig_darray_get_fields(darray);
	for (int i = 0; i < fields[_IG_DARRAY_LENGTH]; i++) {
		if (memcmp(darray + i * fields[_IG_DARRAY_STRIDE], value_ptr, fields[_IG_DARRAY_STRIDE]) == 0)
			return  i;
	}
	return -1;
}

void _ig_darray_clear(void* darray) {
	size_t* fields = _ig_darray_get_fields(darray);
	fields[_IG_DARRAY_LENGTH] = 0;
}

void _ig_darray_destroy(void* darray) {
	size_t* fields = _ig_darray_get_fields(darray);
	free(darray - _IG_DARRAY_FIELD_LENGTH * sizeof(size_t));
}
