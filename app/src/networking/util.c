#include "util.h"
#include <string.h>
#include <stdlib.h>
#include "../game/game.h"
#include <math.h>

uint8_t* reduce_skin(game* g) {
	uint8_t* reduced = ig_darray_create(uint8_t);
	uint8_t sequence_count = 0;

	for (int i = 0; i < g->settings_instance.exp_ptr; i++) {
		sequence_count++;

		uint8_t data_i = g->settings_instance.cusk_skin_data_exp[i];

		if (g->settings_instance.cusk_skin_data_exp[i + 1] != data_i) {
			ig_darray_push(&reduced, &sequence_count);
			ig_darray_push(&reduced, &data_i);
			sequence_count = 0;
		}
	}
	return reduced;
}

void decode_secret(const uint8_t* packet, uint8_t* result) {
	int int_packet[165] = {};
	for (int i = 0; i < 165; i++) {
		int_packet[i] = (int) packet[i];
	}

	int global_value = 0;
	for (int i = 0; i < 24; i++) {
		int value1 = int_packet[17 + i * 2];
		if (value1 <= 96) {
			value1 += 32;
		}
		value1 = (value1 - 98 - i * 34) % 26;
		if (value1 < 0) {
			value1 += 26;
		}
		int value2 = int_packet[18 + i * 2];
		if (value2 <= 96) {
			value2 += 32;
		}
		value2 = (value2 - 115 - i * 34) % 26;
		if (value2 < 0) {
			value2 += 26;
		}
		int interim_result = (value1 << 4) | value2;
		int offset = interim_result >= 97 ? 97 : 65;
		interim_result -= offset;
		if (i == 0) {
			global_value = 2 + interim_result;
		}
		result[i] = (uint8_t) ((interim_result + global_value) % 26) + offset;
		global_value += 3 + interim_result;
	}
}

uint8_t* make_nickname_skin_data(game* g, int* nickname_skin_data_len) {
	int nickname_len = (int) strlen(g->settings_instance.nickname);

	uint8_t* reduced_skin_data = reduce_skin(g);
	int reduced_length = g->settings_instance.cusk ? ig_darray_length(reduced_skin_data) : 0;
	uint8_t* result = malloc(*nickname_skin_data_len = 8 + nickname_len + 8 + reduced_length);

	result[0] = 115;
	result[1] = 31;
	result[2] = 333 >> 8 & 255;
	result[3] = 333 & 255;
	result[4] = g->settings_instance.cv;
	result[5] = nickname_len;

	int j = 6;
	for (int i = 0; i < nickname_len; i++) {
		result[j++] = (uint8_t) g->settings_instance.nickname[i];
	}

	result[j++] = 0;
	result[j++] = 255;

	// custom skin data:
	result[j++] = 255;
	result[j++] = 255;
	result[j++] = 255;
	result[j++] = 0;
	result[j++] = 0;
	result[j++] = 0;
	result[j++] = (int) floorf(rand() % 256);
	result[j++] = (int) floorf(rand() % 256);

	for (int i = 0; i < reduced_length; i++) {
		result[j++] = reduced_skin_data[i];
	}
	
	ig_darray_destroy(reduced_skin_data);

	return result;
}

void set_mscps_fmlts_fpsls(game* g) {
	for (int i = 0; i <= g->config.mscps; i++) {
		if (i >= g->config.mscps) ig_darray_push(&(g->config.fmlts), &(g->config.fmlts[i - 1]));
		else ig_darray_push(&(g->config.fmlts), (float[]) { (powf(1 - (i / (float) (g->config.mscps)), 2.25f)) });
		if (i == 0) ig_darray_push(&(g->config.fpsls), (float[]) { 0 });
		else ig_darray_push(&(g->config.fpsls), (float[]) { g->config.fpsls[i - 1] + 1 / g->config.fmlts[i - 1] });
	}
	float t_fmlt = g->config.fmlts[ig_darray_length(g->config.fmlts) - 1];
	float t_fpsl = g->config.fpsls[ig_darray_length(g->config.fpsls) - 1];
	for (int i = 0; i < 2048; i++) {
		ig_darray_push(&(g->config.fmlts), &t_fmlt);
		ig_darray_push(&(g->config.fpsls), &t_fpsl);
	}
}

void reset_game(game* g) {
	// glfwSetTime(0);

	// g->config.last_ping_mtm = 0;
	// g->config.last_accel_mtm = 0;
	// g->config.last_e_mtm = 0;

	for (int i = 0; i < 10; i++) {
		strcpy(g->leaderboard[i].nk, "LOADING...");
		g->leaderboard[i].cv = 11;
		g->leaderboard[i].score = 0;
	}

	g->config.wfpr = 0;
	for (int j = VFC - 1; j >= 0; j--) {
		g->config.fvxs[j] = 0;
		g->config.fvys[j] = 0;
	}
	g->config.fvtg = 0;
	g->config.fvx = 0;
	g->config.fvy = 0;
	g->config.lag_mult = 1;
	g->network_done = 0;
	g->config.my_pos = 0;
	g->config.rank = 0;
	g->config.total_players = 0;

	snake_map_clear(&g->os);
	ig_darray_clear(g->foods);
	ig_darray_clear(g->preys);
	memset(g->config.mmdata, 0, 136 * 136);
}

void save_settings(game* g) {
	FILE *file = fopen("settings.dat", "wb");
	if (!file) {
		perror("Failed to open settings file for writing");
		return;
	}
	fwrite(&g->settings_instance, sizeof(settings), 1, file);
	fclose(file);
}

void load_settings(game* g) {
	FILE* file = fopen("settings.dat", "rb");
	if (!file) {
		return;
	}
	fread(&g->settings_instance, sizeof(settings), 1, file);
	g->config.qsm = g->settings_instance.hq ? 1 : 1.7f;
	fclose(file);
}
