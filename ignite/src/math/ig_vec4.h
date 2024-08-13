#ifndef IG_VEC4_H
#define IG_VEC4_H

typedef struct ig_vec4 {
	float x;
	float y;
	float z;
	float w;
} ig_vec4;

void ig_vec4_neg(ig_vec4* dst, const ig_vec4* a);
void ig_vec4_add(ig_vec4* dst, const ig_vec4* a, const ig_vec4* b);
void ig_vec4_sub(ig_vec4* dst, const ig_vec4* a, const ig_vec4* b);
void ig_vec4_mul(ig_vec4* dst, const ig_vec4* a, const ig_vec4* b);
void ig_vec4_div(ig_vec4* dst, const ig_vec4* a, const ig_vec4* b);

void ig_vec4_sadd(ig_vec4* dst, const ig_vec4* a, float b);
void ig_vec4_ssub(ig_vec4* dst, const ig_vec4* a, float b);
void ig_vec4_smul(ig_vec4* dst, const ig_vec4* a, float b);
void ig_vec4_sdiv(ig_vec4* dst, const ig_vec4* a, float b);

#endif