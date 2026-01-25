#include <stdio.h>
#include <string.h>

#include "tdarray.h"

size_t* _tdarray_get_fields(void* darray) {
  return darray - (_TDARRAY_FIELD_LENGTH * sizeof(size_t));
}

void* _tdarray_create(size_t stride) {
  void* r = malloc(_TDARRAY_FIELD_LENGTH * sizeof(size_t) + stride) +
            _TDARRAY_FIELD_LENGTH * sizeof(size_t);
  size_t* fields = _tdarray_get_fields(r);
  fields[_TDARRAY_LENGTH] = 0;
  fields[_TDARRAY_STRIDE] = stride;
  fields[_TDARRAY_CAPACITY] = 1;

  return r;
}

void _tdarray_insert(void** darray, size_t i, void* value_ptr) {
  size_t* fields = _tdarray_get_fields(*darray);
  if (fields[_TDARRAY_LENGTH] >= fields[_TDARRAY_CAPACITY]) {
    void* beg = *darray - _TDARRAY_FIELD_LENGTH * sizeof(size_t);
    beg = realloc(beg, _TDARRAY_FIELD_LENGTH * sizeof(size_t) +
                           (fields[_TDARRAY_CAPACITY] *= 2) *
                               fields[_TDARRAY_STRIDE]);
    *darray = beg + _TDARRAY_FIELD_LENGTH * sizeof(size_t);
    fields = _tdarray_get_fields(*darray);
  }
  memmove(*darray + (i + 1) * fields[_TDARRAY_STRIDE],
         *darray + i * fields[_TDARRAY_STRIDE],
         (fields[_TDARRAY_LENGTH] - i) * fields[_TDARRAY_STRIDE]);
  fields[_TDARRAY_LENGTH]++;
  memcpy(*darray + i * fields[_TDARRAY_STRIDE], value_ptr,
         fields[_TDARRAY_STRIDE]);
}

void _tdarray_push(void** darray, void* value_ptr) {
  size_t* fields = _tdarray_get_fields(*darray);

  if (fields[_TDARRAY_LENGTH] >= fields[_TDARRAY_CAPACITY]) {
    void* beg = *darray - _TDARRAY_FIELD_LENGTH * sizeof(size_t);
    beg = realloc(beg, _TDARRAY_FIELD_LENGTH * sizeof(size_t) +
                           (fields[_TDARRAY_CAPACITY] *= 2) *
                               fields[_TDARRAY_STRIDE]);
    *darray = beg + _TDARRAY_FIELD_LENGTH * sizeof(size_t);
    fields = _tdarray_get_fields(*darray);
  }

  memcpy(*darray + fields[_TDARRAY_LENGTH] * fields[_TDARRAY_STRIDE],
         value_ptr, fields[_TDARRAY_STRIDE]);
  fields[_TDARRAY_LENGTH]++;
}

void _tdarray_pop(void* darray) {
  size_t* fields = _tdarray_get_fields(darray);
  fields[_TDARRAY_LENGTH]--;
}

void _tdarray_remove(void* darray, size_t i) {
  size_t* fields = _tdarray_get_fields(darray);
  fields[_TDARRAY_LENGTH]--;
  memmove(darray + i * fields[_TDARRAY_STRIDE],
         darray + (i + 1) * fields[_TDARRAY_STRIDE],
         (fields[_TDARRAY_LENGTH] - i) * fields[_TDARRAY_STRIDE]);
}

size_t _tdarray_length(void* darray) {
  return _tdarray_get_fields(darray)[_TDARRAY_LENGTH];
}

size_t _tdarray_memory(void* darray) {
  size_t* fields = _tdarray_get_fields(darray);
  return fields[_TDARRAY_CAPACITY] * fields[_TDARRAY_STRIDE];
}

int _tdarray_find(void* darray, void* value_ptr) {
  size_t* fields = _tdarray_get_fields(darray);
  for (int i = 0; i < fields[_TDARRAY_LENGTH]; i++) {
    if (memcmp(darray + i * fields[_TDARRAY_STRIDE], value_ptr,
               fields[_TDARRAY_STRIDE]) == 0)
      return i;
  }
  return -1;
}

void _tdarray_clear(void* darray) {
  size_t* fields = _tdarray_get_fields(darray);
  fields[_TDARRAY_LENGTH] = 0;
}

void _tdarray_destroy(void* darray) {
  size_t* fields = _tdarray_get_fields(darray);
  free(darray - _TDARRAY_FIELD_LENGTH * sizeof(size_t));
}