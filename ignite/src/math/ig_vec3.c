#include "ig_vec3.h"

void ig_vec3_neg(ig_vec3* dst, const ig_vec3* a) {
	dst->x = -a->x;
	dst->y = -a->y;
	dst->z = -a->z;
}

void ig_vec3_add(ig_vec3* dst, const ig_vec3* a, const ig_vec3* b) {
	dst->x = a->x + b->x;
	dst->y = a->y + b->y;
	dst->z = a->z + b->z;
}

void ig_vec3_sub(ig_vec3* dst, const ig_vec3* a, const ig_vec3* b) {
	dst->x = a->x - b->x;
	dst->y = a->y - b->y;
	dst->z = a->z - b->z;
}

void ig_vec3_mul(ig_vec3* dst, const ig_vec3* a, const ig_vec3* b) {
	dst->x = a->x * b->x;
	dst->y = a->y * b->y;
	dst->z = a->z * b->z;
}

void ig_vec3_div(ig_vec3* dst, const ig_vec3* a, const ig_vec3* b) {
	dst->x = a->x / b->x;
	dst->y = a->y / b->y;
	dst->z = a->z / b->z;
}

void ig_vec3_sadd(ig_vec3* dst, const ig_vec3* a, float b) {
	dst->x = a->x + b;
	dst->y = a->y + b;
	dst->z = a->z + b;
}

void ig_vec3_ssub(ig_vec3* dst, const ig_vec3* a, float b) {
	dst->x = a->x - b;
	dst->y = a->y - b;
	dst->z = a->z - b;
}

void ig_vec3_smul(ig_vec3* dst, const ig_vec3* a, float b) {
	dst->x = a->x * b;
	dst->y = a->y * b;
	dst->z = a->z * b;
}

void ig_vec3_sdiv(ig_vec3* dst, const ig_vec3* a, float b) {
	dst->x = a->x / b;
	dst->y = a->y / b;
	dst->z = a->z / b;
}
