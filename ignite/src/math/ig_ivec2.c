#include "ig_ivec2.h"

void ig_ivec2_neg(ig_ivec2* dst, const ig_ivec2* a) {
	dst->x = -a->x;
	dst->y = -a->y;
}

void ig_ivec2_add(ig_ivec2* dst, const ig_ivec2* a, const ig_ivec2* b) {
	dst->x = a->x + b->x;
	dst->y = a->y + b->y;
}

void ig_ivec2_sub(ig_ivec2* dst, const ig_ivec2* a, const ig_ivec2* b) {
	dst->x = a->x - b->x;
	dst->y = a->y - b->y;
}

void ig_ivec2_mul(ig_ivec2* dst, const ig_ivec2* a, const ig_ivec2* b) {
	dst->x = a->x * b->x;
	dst->y = a->y * b->y;
}

void ig_ivec2_div(ig_ivec2* dst, const ig_ivec2* a, const ig_ivec2* b) {
	dst->x = a->x / b->x;
	dst->y = a->y / b->y;
}

void ig_ivec2_sadd(ig_ivec2* dst, const ig_ivec2* a, int b) {
	dst->x = a->x + b;
	dst->y = a->y + b;
}

void ig_ivec2_ssub(ig_ivec2* dst, const ig_ivec2* a, int b) {
	dst->x = a->x - b;
	dst->y = a->y - b;
}

void ig_ivec2_smul(ig_ivec2* dst, const ig_ivec2* a, int b) {
	dst->x = a->x * b;
	dst->y = a->y * b;
}

void ig_ivec2_sdiv(ig_ivec2* dst, const ig_ivec2* a, int b) {
	dst->x = a->x / b;
	dst->y = a->y / b;
}
