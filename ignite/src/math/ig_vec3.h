#ifndef IG_VEC3_H
#define IG_VEC3_H

typedef struct ig_vec3 {
	float x;
	float y;
	float z;
} ig_vec3;

void ig_vec3_neg(ig_vec3* dst, const ig_vec3* a);
void ig_vec3_add(ig_vec3* dst, const ig_vec3* a, const ig_vec3* b);
void ig_vec3_sub(ig_vec3* dst, const ig_vec3* a, const ig_vec3* b);
void ig_vec3_mul(ig_vec3* dst, const ig_vec3* a, const ig_vec3* b);
void ig_vec3_div(ig_vec3* dst, const ig_vec3* a, const ig_vec3* b);

void ig_vec3_sadd(ig_vec3* dst, const ig_vec3* a, float b);
void ig_vec3_ssub(ig_vec3* dst, const ig_vec3* a, float b);
void ig_vec3_smul(ig_vec3* dst, const ig_vec3* a, float b);
void ig_vec3_sdiv(ig_vec3* dst, const ig_vec3* a, float b);

#endif