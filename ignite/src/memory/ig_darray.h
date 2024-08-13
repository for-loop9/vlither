#ifndef IG_DARRAY_H
#define IG_DARRAY_H

#include <stdlib.h>

typedef enum _ig_darray_field {
	_IG_DARRAY_LENGTH,
	_IG_DARRAY_STRIDE,
	_IG_DARRAY_CAPACITY,
	_IG_DARRAY_FIELD_LENGTH
} _ig_darray_field;

size_t* _ig_darray_get_fields(void* darray);
void* _ig_darray_create(size_t stride);
void _ig_darray_insert(void** darray, size_t i, void* value_ptr);
void _ig_darray_push(void** darray_ptr, void* value_ptr);
void _ig_darray_pop(void* darray);
void _ig_darray_remove(void* darray, size_t i);
size_t _ig_darray_length(void* darray);
size_t _ig_darray_memory(void* darray);
int _ig_darray_find(void* darray, void* value_ptr);
void _ig_darray_clear(void* darray);
void _ig_darray_destroy(void* darray);

#define ig_darray_create(type) (type*) _ig_darray_create(sizeof(type))
#define ig_darray_insert(darray_ptr, i, value_ptr) _ig_darray_insert((void**) darray_ptr, i, value_ptr)
#define ig_darray_push(darray_ptr, value_ptr) _ig_darray_push((void**) darray_ptr, value_ptr)
#define ig_darray_pop(darray) _ig_darray_pop(darray)
#define ig_darray_remove(darray, i) _ig_darray_remove(darray, i)
#define ig_darray_length(darray) _ig_darray_length(darray)
#define ig_darray_memory(darray) _ig_darray_memory(darray)
#define ig_darray_find(darray, value_ptr) _ig_darray_find(darray, value_ptr)
#define ig_darray_clear(darray) _ig_darray_clear(darray)
#define ig_darray_destroy(darray) _ig_darray_destroy(darray)

size_t get_tot_mem_alloc();
size_t get_tot_mem_freed();
void print_mem_info();

#endif