#ifndef IG_IVEC3_H
#define IG_IVEC3_H

typedef struct ig_ivec3 {
	int x;
	int y;
	int z;
} ig_ivec3;

void ig_ivec3_neg(ig_ivec3* dst, const ig_ivec3* a);
void ig_ivec3_add(ig_ivec3* dst, const ig_ivec3* a, const ig_ivec3* b);
void ig_ivec3_sub(ig_ivec3* dst, const ig_ivec3* a, const ig_ivec3* b);
void ig_ivec3_mul(ig_ivec3* dst, const ig_ivec3* a, const ig_ivec3* b);
void ig_ivec3_div(ig_ivec3* dst, const ig_ivec3* a, const ig_ivec3* b);

void ig_ivec3_sadd(ig_ivec3* dst, const ig_ivec3* a, int b);
void ig_ivec3_ssub(ig_ivec3* dst, const ig_ivec3* a, int b);
void ig_ivec3_smul(ig_ivec3* dst, const ig_ivec3* a, int b);
void ig_ivec3_sdiv(ig_ivec3* dst, const ig_ivec3* a, int b);

#endif