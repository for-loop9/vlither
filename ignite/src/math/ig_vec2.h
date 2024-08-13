#ifndef IG_VEC2_H
#define IG_VEC2_H

typedef struct ig_vec2 {
	float x;
	float y;
} ig_vec2;

void ig_vec2_neg(ig_vec2* dst, const ig_vec2* a);
void ig_vec2_norm(ig_vec2* dst);

void ig_vec2_add(ig_vec2* dst, const ig_vec2* a, const ig_vec2* b);
void ig_vec2_sub(ig_vec2* dst, const ig_vec2* a, const ig_vec2* b);
void ig_vec2_mul(ig_vec2* dst, const ig_vec2* a, const ig_vec2* b);
void ig_vec2_div(ig_vec2* dst, const ig_vec2* a, const ig_vec2* b);

void ig_vec2_sadd(ig_vec2* dst, const ig_vec2* a, float b);
void ig_vec2_ssub(ig_vec2* dst, const ig_vec2* a, float b);
void ig_vec2_smul(ig_vec2* dst, const ig_vec2* a, float b);
void ig_vec2_sdiv(ig_vec2* dst, const ig_vec2* a, float b);

#endif