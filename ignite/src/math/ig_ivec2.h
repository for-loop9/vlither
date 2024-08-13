#ifndef IG_IVEC2_H
#define IG_IVEC2_H

typedef struct ig_ivec2 {
	int x;
	int y;
} ig_ivec2;

void ig_ivec2_neg(ig_ivec2* dst, const ig_ivec2* a);

void ig_ivec2_add(ig_ivec2* dst, const ig_ivec2* a, const ig_ivec2* b);
void ig_ivec2_sub(ig_ivec2* dst, const ig_ivec2* a, const ig_ivec2* b);
void ig_ivec2_mul(ig_ivec2* dst, const ig_ivec2* a, const ig_ivec2* b);
void ig_ivec2_div(ig_ivec2* dst, const ig_ivec2* a, const ig_ivec2* b);

void ig_ivec2_sadd(ig_ivec2* dst, const ig_ivec2* a, int b);
void ig_ivec2_ssub(ig_ivec2* dst, const ig_ivec2* a, int b);
void ig_ivec2_smul(ig_ivec2* dst, const ig_ivec2* a, int b);
void ig_ivec2_sdiv(ig_ivec2* dst, const ig_ivec2* a, int b);

#endif