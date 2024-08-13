#include "ig_mat4.h"

void ig_mat4_zero(ig_mat4* a) {
	for (int i = 0; i < 16; i++) {
		a->data[i] = 0;
	}
}

void ig_mat4_identity(ig_mat4* a) {
	ig_mat4_zero(a);
	a->data[0] = 1;
	a->data[5] = 1;
	a->data[10] = 1;
	a->data[15] = 1;
}

void ig_mat4_ortho(ig_mat4* a, float left, float right, float top, float bottom, float near, float far) {
	a->data[0] = 2 / (right - left);
	a->data[1] = 0;
	a->data[2] = 0;
	a->data[3] = 0;
	a->data[4] = 0;
	a->data[5] = 2 / (top - bottom);
	a->data[6] = 0;
	a->data[7] = 0;
	a->data[8] = 0;
	a->data[9] = 0;
	a->data[10] = -2 / (far - near);
	a->data[11] = 0;
	a->data[12] = -((right + left) / (right - left));
	a->data[13] = -((top + bottom) / (top - bottom));
	a->data[14] = -((far + near) / (far - near));
	a->data[15] = 1;
}

void ig_mat4_transpose(ig_mat4* a) {
    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) {
            float temp = a->data[i * 4 + j];
            a->data[i * 4 + j] = a->data[j * 4 + i];
            a->data[j * 4 + i] = temp;
        }
    }
}

