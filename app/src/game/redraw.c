#include "redraw.h"
#include "game.h"
#include "prey.h"
#include "../networking/util.h"
#include "hud.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "../external/cimgui/cimgui.h"

void redraw(game* g, int fps_display) {
	renderer_start_imgui_frame(g->renderer);

	float mww = g->icontext->default_frame.resolution.x;
	float mhh = g->icontext->default_frame.resolution.y;
	float mww2 = mww / 2;
	float mhh2 = mhh / 2;
	float mwwp50 = mww + 50;
	float mhhp50 = mhh + 50;

	igSetNextWindowPos((ImVec2) { .x = 0, .y = 0 }, ImGuiCond_None, (ImVec2) {});
	igSetNextWindowSize((ImVec2) { .x = g->icontext->default_frame.resolution.x, .y = g->icontext->default_frame.resolution.y }, ImGuiCond_None);
	igBegin("hud", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs);
	ImDrawList* draw_list = igGetWindowDrawList();

	igPushFont(g->renderer->small_font);
	if (!g->snake_null) { // snake != NULL
		if (!g->settings_instance.enable_zoom) {
			float dgsc = 0.64285f + 0.514285714f / fmaxf(1, (g->os.snakes[0].sct + 16.0f) / 36.0f);
			if (g->config.gsc != dgsc) {
				if (g->config.gsc < dgsc) {
					g->config.gsc += 2e-4;
					if (g->config.gsc >= dgsc) g->config.gsc = dgsc;
				} else {
					g->config.gsc -= 2e-4;
					if (g->config.gsc <= dgsc) g->config.gsc = dgsc;
				}
			}
		}
	}

	float lvx = g->config.view_xx;
	float lvy = g->config.view_yy;

	if (!g->snake_null) { // snake != NULL
		if (g->config.fvtg > 0) {
			g->config.fvtg--;
			g->config.fvx = g->config.fvxs[g->config.fvpos];
			g->config.fvy = g->config.fvys[g->config.fvpos];
			g->config.fvxs[g->config.fvpos] = 0;
			g->config.fvys[g->config.fvpos] = 0;
			g->config.fvpos++;
			if (g->config.fvpos >= VFC) g->config.fvpos = 0;
		}
		g->config.view_xx = g->os.snakes[0].xx + g->os.snakes[0].fx + g->config.fvx;
		g->config.view_yy = g->os.snakes[0].yy + g->os.snakes[0].fy + g->config.fvy;
		g->config.bpx1 = g->config.view_xx - (mww2 / g->config.gsc + 84);
		g->config.bpy1 = g->config.view_yy - (mhh2 / g->config.gsc + 84);
		g->config.bpx2 = g->config.view_xx + (mww2 / g->config.gsc + 84);
		g->config.bpy2 = g->config.view_yy + (mhh2 / g->config.gsc + 84);
		g->config.fpx1 = g->config.view_xx - (mww2 / g->config.gsc + 24);
		g->config.fpy1 = g->config.view_yy - (mhh2 / g->config.gsc + 24);
		g->config.fpx2 = g->config.view_xx + (mww2 / g->config.gsc + 24);
		g->config.fpy2 = g->config.view_yy + (mhh2 / g->config.gsc + 24);
		g->config.apx1 = g->config.view_xx - (mww2 / g->config.gsc + 210);
		g->config.apy1 = g->config.view_yy - (mhh2 / g->config.gsc + 210);
		g->config.apx2 = g->config.view_xx + (mww2 / g->config.gsc + 210);
		g->config.apy2 = g->config.view_yy + (mhh2 / g->config.gsc + 210);
	}

	g->config.bgx2 -= (g->config.view_xx - lvx) * 1 / g->config.bgw2;
	g->config.bgy2 -= (g->config.view_yy - lvy) * 1 / g->config.bgh2;

	float grd = g->config.grd;
	
	renderer_push_bd(g->renderer, &(bd_instance) {
		.circ = {
			.x = mww2 + (g->config.grd - g->config.view_xx) * g->config.gsc,
			.y = mhh2 + (g->config.grd - g->config.view_yy) * g->config.gsc,
			.z = (g->config.grd * 0.98f) * g->config.gsc },
		.color = { .x = 0.186f, .y = 0.11f, .z = 0.13f }
	});

	renderer_push_bg(g->renderer, &(sprite_instance) {
		.rect = {
			.x = 0,
			.y = 0,
			.z = mww,
			.w = mhh,
		},
		.ratios = { .x = 0, .y = 1 },
		.uv_rect = {
			.x = (((-mww / g->config.bgw2) / 2) / g->config.gsc) - g->config.bgx2,
			.y = (((-mhh / g->config.bgh2) / 2) / g->config.gsc) - g->config.bgy2,
			.z = (mww / g->config.bgw2) / g->config.gsc,
			.w = (mhh / g->config.bgh2) / g->config.gsc },
		.color = { .x = 1, .y = 1, .z = 1 }
	});

	// foods
	for (int i = ig_darray_length(g->foods) - 1; i >= 0; i--) {
		// FOOD RENDER
		food* fo = g->foods + i;
		ig_vec3* col = g->config.color_groups + fo->cv;

		if (fo->rx >= g->config.fpx1 && fo->ry >= g->config.fpy1 && fo->rx <= g->config.fpx2 && fo->ry <= g->config.fpy2) {
			if (fo->rad == 1) {
				float fx = mww2 + g->config.gsc * (fo->rx - g->config.view_xx) - ((fo->rad * fo->f2) / 4.0f);
				float fy = mhh2 + g->config.gsc * (fo->ry - g->config.view_yy) - ((fo->rad * fo->f2) / 4.0f);
				
				renderer_push_circle(g->renderer, &(circle_instance) {
					.circ = { .x = fx, .y = fy, .z = (fo->rad * fo->f) / 4.0f },
					.ratios = { .x = 0, .y = 1 },
					.color = { .x = col->x, .y = col->y, .z = col->z, .w = 1 }
				});
			}
			else {
				float fx = mww2 + g->config.gsc * (fo->rx - g->config.view_xx) - ((fo->rad * fo->f2) / 4.0f);
				float fy = mhh2 + g->config.gsc * (fo->ry - g->config.view_yy) - ((fo->rad * fo->f2) / 4.0f);

				renderer_push_circle(g->renderer, &(circle_instance) {
					.circ = { .x = fx, .y = fy, .z = (fo->rad * fo->f) / 4.0f },
					.ratios = { .x = 0, .y = 1 },
					.color = { .x = col->x, .y = col->y, .z = col->z, .w = 1 }
				});
			}
		}
	}

	// preys
	for (int i = ig_darray_length(g->preys) - 1; i >= 0; i--) {
		prey* pr = g->preys + i;
		float tx = pr->xx + pr->fx;
		float ty = pr->yy + pr->fy;
		float px = mww2 + g->config.gsc * (tx - g->config.view_xx);
		float py = mhh2 + g->config.gsc * (ty - g->config.view_yy);
		ig_vec3* col = g->config.color_groups + pr->cv;

		if (px >= -50 && py >= -50 && px <= mwwp50 && py <= mhhp50) {
			if (pr->eaten) {
				snake* o = snake_map_get(&g->os, pr->eaten_by);
				if (o) {
					float k = powf(pr->eaten_fr, 2);
					tx += (o->xx + o->fx + cosf(o->ang + o->fa) * (43 - k * 24) * (1 - k) - tx) * k;
					ty += (o->yy + o->fy + sinf(o->ang + o->fa) * (43 - k * 24) * (1 - k) - ty) * k;
					px = mww2 + g->config.gsc * (tx - g->config.view_xx);
					py = mhh2 + g->config.gsc * (ty - g->config.view_yy);
				}
			}
			if (pr->rad == 1) {
				float fx = px - pr->f2 / 4.0f;
				float fy = py - pr->f2 / 4.0f;
				renderer_push_circle(g->renderer, &(circle_instance) {
					.circ = { .x = fx, .y = fy, .z = pr->f / 4.0f },
					.ratios = { .x = 0, .y = 1 },
					.color = { .x = col->x + pr->blink * (1 - col->x), .y = col->y + pr->blink * (1 - col->y), .z = col->z + pr->blink * (1 - col->z), .w = pr->fr }
				});
			} else {
				float fx = px - (pr->f2 * pr->rad) / 4.0f;
				float fy = py - (pr->f2 * pr->rad) / 4.0f;
				
				renderer_push_circle(g->renderer, &(circle_instance) {
					.circ = { .x = fx, .y = fy, .z = (pr->f * pr->rad) / 4.0f },
					.ratios = { .x = 0, .y = 1 },
					.color = { .x = col->x + pr->blink * (1 - col->x), .y = col->y + pr->blink * (1 - col->y), .z = col->z + pr->blink * (1 - col->z), .w = pr->fr }
				});
			}
		}
	}

	// snake is in view
	int snakes_len = snake_map_get_total(&g->os);
	for (int i = 0; i < snakes_len; i++) {
		snake* o = g->os.snakes + i;
		o->iiv = 0;
		int pts_len = ig_darray_length(o->pts);
		for (int j = pts_len - 1; j >= 0; j--) {
			body_part* po = o->pts + j;
			float px = po->xx + po->fx;
			float py = po->yy + po->fy;
			if (px >= g->config.bpx1 && py >= g->config.bpy1 && px <= g->config.bpx2 && py <= g->config.bpy2) {
				o->iiv = true;
				break;
			}
		}
	}
	// snakes render:
	snakes_len = snake_map_get_total(&g->os);
	for (int i = 0; i < snakes_len; i++) {
		snake* o = g->os.snakes + i;
		if (o->iiv) {
			float hx = o->xx + o->fx;
			float hy = o->yy + o->fy;
			float px = hx;
			float py = hy;
			float fang = o->ehang;
			float ssc = o->sc;
			float lsz = 29 * ssc;
			float rl = o->cfl;

			int pts_len = ig_darray_length(o->pts);
			body_part* po = o->pts + (pts_len - 1);

			lsz *= 0.5f;
			float ix1, iy1, ix2, iy2, ax1, ay1, ax2, ay2, cx2, cy2;
			int bp = 0;
			px = hx;
			py = hy;
			ax2 = px;
			ay2 = py;
			if (ax2 >= g->config.bpx1 && ay2 >= g->config.bpy1 && ax2 <= g->config.bpx2 && ay2 <= g->config.bpy2) {
				g->config.pbx[0] = ax2;
				g->config.pby[0] = ay2;
				g->config.pba[0] = atan2f(hy - (po->yy + po->fy), hx - (po->xx + po->fx)) + PI;
				g->config.pbu[0] = 2;
			} else g->config.pbu[0] = 0;
			bp = 1;
			float rezc = 0;
			float km = 0.25f;
			float n = fmodf(o->chl + o->fchl, km);
			if (n < 0) n += km;
			n = km - n;
			rl += 1 - km * ceilf((o->chl + o->fchl) / km);
			float ax = px;
			float ay = py;
			if (o->sep != o->wsep) {
				if (o->sep < o->wsep) {
					o->sep += 0.002f;
					if (o->sep >= o->wsep) o->sep = o->wsep;
				}
				else if (o->sep > o->wsep) {
					o->sep -= 0.002f;
					if (o->sep <= o->wsep) o->sep = o->wsep;
				}
			}
			float sep = o->sep * g->config.qsm;

			float rmr = 0;
			float lax = 0;
			float lay = 0;

			pts_len = ig_darray_length(o->pts);
			for (int j = pts_len - 1; j >= 0; j--) {
				po = o->pts + j;
				float lpx = px;
				float lpy = py;
				px = po->xx + po->fx;
				py = po->yy + po->fy;
				if (rl > -km) {
					ax1 = ax2;
					ay1 = ay2;
					ax2 = (px + lpx) / 2;
					ay2 = (py + lpy) / 2;
					cx2 = lpx;
					cy2 = lpy;
					for (float k = 0; k < 1; k += km) {
						float m = n + k;
						ix1 = ax1 + (cx2 - ax1) * m;
						iy1 = ay1 + (cy2 - ay1) * m;
						ix2 = cx2 + (ax2 - cx2) * m;
						iy2 = cy2 + (ay2 - cy2) * m;
						lax = ax;
						lay = ay;
						ax = ix1 + (ix2 - ix1) * m;
						ay = iy1 + (iy2 - iy1) * m;
						if (rl < 0) {
							ax += -(lax - ax) * rl / km;
							ay += -(lay - ay) * rl / km;
						}
						float d = sqrtf(powf(ax - lax, 2) + powf(ay - lay, 2));
						if (rmr + d < sep) rmr += d;
						else {
							rmr = -rmr;
							for (m = (d - rmr) / sep; m >= 1; m--) {
								rmr += sep;
								float pax = lax + (ax - lax) * rmr / d;
								float pay = lay + (ay - lay) * rmr / d;
								if (pax >= g->config.bpx1 && pay >= g->config.bpy1 && pax <= g->config.bpx2 && pay <= g->config.bpy2) {
									g->config.pbx[bp] = pax;
									g->config.pby[bp] = pay;
									g->config.pbu[bp] = 2;
									float tx = ax - lax;
									float ty = ay - lay;
									if (tx >= -4 && ty >= -4 && tx < 4 && ty < 4) g->config.pba[bp] = g->config.at2lt[((int) (ty * 32 + 128) << 8) | (int) (tx * 32 + 128)];
									else if (tx >= -8 && ty >= -8 && tx < 8 && ty < 8) g->config.pba[bp] = g->config.at2lt[((int)(ty * 16 + 128) << 8) | (int) (tx * 16 + 128)];
									else if (tx >= -16 && ty >= -16 && tx < 16 && ty < 16) g->config.pba[bp] = g->config.at2lt[((int)(ty * 8 + 128) << 8) | (int) (tx * 8 + 128)];
									else if (tx >= -127 && ty >= -127 && tx < 127 && ty < 127) g->config.pba[bp] = g->config.at2lt[((int) (ty + 128)) << 8 | (int) (tx + 128)];
									else g->config.pba[bp] = atan2f(ty, tx);
								}
								else g->config.pbu[bp] = 0;
								bp++;
							}
							rmr = d - rmr;
						}
						if (rl < 1) {
							rl -= km;
							if (rl <= -km) break;
						}
					}
					if (rl >= 1) rl--;
				}
			}

			if (ax >= g->config.bpx1 && ay >= g->config.bpy1 && ax <= g->config.bpx2 && ay <= g->config.bpy2) {
				g->config.pbu[bp] = 2;
				g->config.pbx[bp] = ax;
				g->config.pby[bp] = ay;
				g->config.pba[bp] = atan2f(ay - lay, ax - lax);
			} else g->config.pbu[bp] = 0;
			bp++;

			float v = (1 - o->dead_amt) * o->alive_amt;

			for (int j = bp - 1; j >= 0; j--) {
				if (g->config.pbu[j] >= 1) {
					if (j >= 1 && g->config.pbu[j - 1] == 2) {
						float shsz = (g->config.gsc * lsz * 62.0f / 32.0f) * 0.6f;
						px = (mww2 + ((g->config.pbx[j - 1] - g->config.view_xx) * g->config.gsc));
						py = (mhh2 + ((g->config.pby[j - 1] - g->config.view_yy) * g->config.gsc));

						renderer_push_circle(g->renderer, &(circle_instance) {
							.circ = { .x = px - shsz, .y = py - shsz, .z = shsz * 2 },
							.ratios = { .x = 0, .y = 1 },
							.color = { .x = 0, .y = 0, .z = 0, .w = 0.2f * v }
						});
					}

					ig_vec3* col = g->config.color_groups + o->skin_data[j % o->skin_data_len];

					px = mww2 + ((g->config.pbx[j] - g->config.view_xx) * g->config.gsc);
					py = mhh2 + ((g->config.pby[j] - g->config.view_yy) * g->config.gsc);

					renderer_push_circle(g->renderer, &(circle_instance) {
						.circ = { .x = px + (-g->config.gsc * lsz), .y = py + (-g->config.gsc * lsz), .z = g->config.gsc * 2 * lsz },
						.ratios = { .x = 0, .y = 1 },
						.color = { .x = col->x, .y = col->y, .z = col->z, .w = v }
					});
				}
			}

			// eyes
			float ed = 6 * ssc;
			float esp = 6 * ssc;

			float ex = cosf(fang) * ed + cosf(fang - PI / 2) * (esp + 0.5f);
			float ey = sinf(fang) * ed + sinf(fang - PI / 2) * (esp + 0.5f);

			float rd2 = (6 * ssc * g->config.gsc) * 2;

			renderer_push_circle(g->renderer, &(circle_instance) {
				.circ = {
					.x = (mww2 + (ex + hx - g->config.view_xx) * g->config.gsc) - rd2 / 2,
					.y = (mhh2 + (ey + hy - g->config.view_yy) * g->config.gsc) - rd2 / 2,
					.z = rd2
				},
				.ratios = { .x = 0, .y = 1 },
				.color = { .x = 1, .y = 1, .z = 1, .w = v }
			});

			ex = cosf(fang) * (ed + 0.5f) + o->rex * ssc + cosf(fang - PI / 2) * esp;
			ey = sinf(fang) * (ed + 0.5f) + o->rey * ssc + sinf(fang - PI / 2) * esp;
			rd2 = (3.5f * ssc * g->config.gsc) * 2;

			renderer_push_circle(g->renderer, &(circle_instance) {
				.circ = {
					.x = (mww2 + (ex + hx - g->config.view_xx) * g->config.gsc) - rd2 / 2,
					.y = (mhh2 + (ey + hy - g->config.view_yy) * g->config.gsc) - rd2 / 2,
					.z = rd2
				},
				.ratios = { .x = 0, .y = 1 },
				.color = { .x = 0, .y = 0, .z = 0, .w = v }
			});

			ex = cosf(fang) * ed + cosf(fang + PI / 2) * (esp + 0.5f);
			ey = sinf(fang) * ed + sinf(fang + PI / 2) * (esp + 0.5f);
			rd2 = (6 * ssc * g->config.gsc) * 2;

			// if (!g->snake_null && o == g->os.snakes)
			// 	printf("iris size = %d, bp size = %d, pupil size = %d\n", (int) rd2, (int) (g->config.gsc * 2 * lsz), (int) ((3.5f * ssc * g->config.gsc) * 2));

			renderer_push_circle(g->renderer, &(circle_instance) {
				.circ = {
					.x = (mww2 + (ex + hx - g->config.view_xx) * g->config.gsc) - rd2 / 2,
					.y = (mhh2 + (ey + hy - g->config.view_yy) * g->config.gsc) - rd2 / 2,
					.z = rd2
				},
				.ratios = { .x = 0, .y = 1 },
				.color = { .x = 1, .y = 1, .z = 1, .w = v }
			});

			ex = cosf(fang) * (ed + 0.5f) + o->rex * ssc + cosf(fang + PI / 2) * esp;
			ey = sinf(fang) * (ed + 0.5f) + o->rey * ssc + sinf(fang + PI / 2) * esp;
			rd2 = (3.5f * ssc * g->config.gsc) * 2;

			renderer_push_circle(g->renderer, &(circle_instance) {
				.circ = {
					.x = (mww2 + (ex + hx - g->config.view_xx) * g->config.gsc) - rd2 / 2,
					.y = (mhh2 + (ey + hy - g->config.view_yy) * g->config.gsc) - rd2 / 2,
					.z = rd2
				},
				.ratios = { .x = 0, .y = 1 },
				.color = { .x = 0, .y = 0, .z = 0, .w = v }
			});

			if (g->snake_null || o != g->os.snakes + 0) {
				float ntx = o->xx + o->fx;
				float nty = o->yy + o->fy;
				ntx = mww2 + (ntx - g->config.view_xx) * g->config.gsc;
				nty = mhh2 + (nty - g->config.view_yy) * g->config.gsc;
				
				int len; ImVec2 nick_txt_size; igCalcTextSize(&nick_txt_size, o->nk, NULL, 0, 0); len = nick_txt_size.x;

				ImDrawList_AddText_Vec2(draw_list,
					(ImVec2) {
					.x = ntx - len / 2, .y = nty + 32 + 11 * o->sc * g->config.gsc
				}, igColorConvertFloat4ToU32((ImVec4) { .x = 0.8f, .y = 0.8f, .z = 0.8f, .w = v }), o->nk, NULL);
			}
		}
	}
	igPopFont();

	hud(g, fps_display);
	igEnd();
}
