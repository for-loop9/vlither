#include "ig_vec2.h"
#include <math.h>

void ig_vec2_norm(ig_vec2* dst) {
	float w = sqrtf(dst->x * dst->x + dst->y * dst->y);
    dst->x /= w;
    dst->y /= w;
}

void ig_vec2_neg(ig_vec2* dst, const ig_vec2* a) {
	dst->x = -a->x;
	dst->y = -a->y;
}

void ig_vec2_add(ig_vec2* dst, const ig_vec2* a, const ig_vec2* b) {
	dst->x = a->x + b->x;
	dst->y = a->y + b->y;
}

void ig_vec2_sub(ig_vec2* dst, const ig_vec2* a, const ig_vec2* b) {
	dst->x = a->x - b->x;
	dst->y = a->y - b->y;
}

void ig_vec2_mul(ig_vec2* dst, const ig_vec2* a, const ig_vec2* b) {
	dst->x = a->x * b->x;
	dst->y = a->y * b->y;
}

void ig_vec2_div(ig_vec2* dst, const ig_vec2* a, const ig_vec2* b) {
	dst->x = a->x / b->x;
	dst->y = a->y / b->y;
}

void ig_vec2_sadd(ig_vec2* dst, const ig_vec2* a, float b) {
	dst->x = a->x + b;
	dst->y = a->y + b;
}

void ig_vec2_ssub(ig_vec2* dst, const ig_vec2* a, float b) {
	dst->x = a->x - b;
	dst->y = a->y - b;
}

void ig_vec2_smul(ig_vec2* dst, const ig_vec2* a, float b) {
	dst->x = a->x * b;
	dst->y = a->y * b;
}

void ig_vec2_sdiv(ig_vec2* dst, const ig_vec2* a, float b) {
	dst->x = a->x / b;
	dst->y = a->y / b;
}
