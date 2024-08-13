#ifndef IG_MAT4_H
#define IG_MAT4_H

typedef struct ig_mat4 {
	float data[16];
} ig_mat4;

void ig_mat4_zero(ig_mat4* a);
void ig_mat4_identity(ig_mat4* a);
void ig_mat4_ortho(ig_mat4* a, float left, float right, float top, float bottom, float near, float far);
void ig_mat4_transpose(ig_mat4* a);

#endif