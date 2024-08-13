#include "ig_ivec3.h"

void ig_ivec3_neg(ig_ivec3* dst, const ig_ivec3* a) {
	dst->x = -a->x;
	dst->y = -a->y;
	dst->z = -a->z;
}

void ig_ivec3_add(ig_ivec3* dst, const ig_ivec3* a, const ig_ivec3* b) {
	dst->x = a->x + b->x;
	dst->y = a->y + b->y;
	dst->z = a->z + b->z;
}

void ig_ivec3_sub(ig_ivec3* dst, const ig_ivec3* a, const ig_ivec3* b) {
	dst->x = a->x - b->x;
	dst->y = a->y - b->y;
	dst->z = a->z - b->z;
}

void ig_ivec3_mul(ig_ivec3* dst, const ig_ivec3* a, const ig_ivec3* b) {
	dst->x = a->x * b->x;
	dst->y = a->y * b->y;
	dst->z = a->z * b->z;
}

void ig_ivec3_div(ig_ivec3* dst, const ig_ivec3* a, const ig_ivec3* b) {
	dst->x = a->x / b->x;
	dst->y = a->y / b->y;
	dst->z = a->z / b->z;
}

void ig_ivec3_sadd(ig_ivec3* dst, const ig_ivec3* a, int b) {
	dst->x = a->x + b;
	dst->y = a->y + b;
	dst->z = a->z + b;
}

void ig_ivec3_ssub(ig_ivec3* dst, const ig_ivec3* a, int b) {
	dst->x = a->x - b;
	dst->y = a->y - b;
	dst->z = a->z - b;
}

void ig_ivec3_smul(ig_ivec3* dst, const ig_ivec3* a, int b) {
	dst->x = a->x * b;
	dst->y = a->y * b;
	dst->y = a->y * b;
}

void ig_ivec3_sdiv(ig_ivec3* dst, const ig_ivec3* a, int b) {
	dst->x = a->x / b;
	dst->y = a->y / b;
	dst->y = a->z / b;
}
