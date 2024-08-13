#include "ig_vec4.h"

void ig_vec4_neg(ig_vec4* dst, const ig_vec4* a) {
	dst->x = -a->x;
	dst->y = -a->y;
	dst->z = -a->z;
	dst->w = -a->w;
}

void ig_vec4_add(ig_vec4* dst, const ig_vec4* a, const ig_vec4* b) {
	dst->x = a->x + b->x;
	dst->y = a->y + b->y;
	dst->z = a->z + b->z;
	dst->w = a->w + b->w;
}

void ig_vec4_sub(ig_vec4* dst, const ig_vec4* a, const ig_vec4* b) {
	dst->x = a->x - b->x;
	dst->y = a->y - b->y;
	dst->z = a->z - b->z;
	dst->w = a->w - b->w;
}

void ig_vec4_mul(ig_vec4* dst, const ig_vec4* a, const ig_vec4* b) {
	dst->x = a->x * b->x;
	dst->y = a->y * b->y;
	dst->z = a->z * b->z;	
	dst->w = a->w * b->w;
}

void ig_vec4_div(ig_vec4* dst, const ig_vec4* a, const ig_vec4* b) {
	dst->x = a->x / b->x;
	dst->y = a->y / b->y;
	dst->z = a->z / b->z;
	dst->w = a->w / b->w;
}

void ig_vec4_sadd(ig_vec4* dst, const ig_vec4* a, float b) {
	dst->x = a->x + b;
	dst->y = a->y + b;
	dst->z = a->z + b;
	dst->w = a->w + b;
}

void ig_vec4_ssub(ig_vec4* dst, const ig_vec4* a, float b) {
	dst->x = a->x - b;
	dst->y = a->y - b;
	dst->z = a->z - b;
	dst->w = a->w - b;
}

void ig_vec4_smul(ig_vec4* dst, const ig_vec4* a, float b) {
	dst->x = a->x * b;
	dst->y = a->y * b;
	dst->z = a->z * b;
	dst->w = a->w * b;
}

void ig_vec4_sdiv(ig_vec4* dst, const ig_vec4* a, float b) {
	dst->x = a->x / b;
	dst->y = a->y / b;
	dst->z = a->z / b;
	dst->w = a->w / b;
}
