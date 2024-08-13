#include "callback.h"
#include "util.h"
#include "../game/game.h"
#include "../game/food.h"
#include "../game/oef.h"
#include "../game/redraw.h"
#include "../game/prey.h"

void client_callback(struct mg_connection* c, int ev, void* ev_data) {
	game* g = (game*) c->fn_data;

	if (ev == MG_EV_WS_MSG) {
		struct mg_ws_message* wm = (struct mg_ws_message*) ev_data;
		const uint8_t* packet = (const uint8_t*) wm->data.buf;

		int p = 2;
		uint8_t packet_type = packet[p++];
		int packet_len = wm->data.len;

		if (packet_type == '6') {
			printf("recieved pre-init, sent decrypted message and nickname/skin data\n");

			uint8_t ping_packet = 251;
			uint8_t decoded_secret[24] = {};
			int nickname_skin_data_len = 0;
			uint8_t* nickname_skin_data = make_nickname_skin_data(g, &nickname_skin_data_len);

			decode_secret(packet, decoded_secret);

			mg_ws_send(c, decoded_secret, 24, WEBSOCKET_OP_BINARY);
			mg_ws_send(c, nickname_skin_data, nickname_skin_data_len, WEBSOCKET_OP_BINARY);
			mg_ws_send(c, &ping_packet, 1, WEBSOCKET_OP_BINARY);
			// printf("first ping\n");

			free(nickname_skin_data);
		} else if (packet_type == 'a') {
			printf("recieved a\n");
			g->config.grd = packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]; p += 3;
			g->config.mscps = packet[p] << 8 | packet[p + 1]; p += 2;
			g->config.sector_size = packet[p] << 8 | packet[p + 1];
			g->config.ssd256 = g->config.sector_size / 256.0f; p += 4;
			g->config.spangdv = packet[p] / 10.0f; p++;
			g->config.nsp1 = (packet[p] << 8 | packet[p + 1]) / 100.0f; p += 2;
			g->config.nsp2 = (packet[p] << 8 | packet[p + 1]) / 100.0f; p += 2;
			g->config.nsp3 = (packet[p] << 8 | packet[p + 1]) / 100.0f; p += 2;
			g->config.mamu = (packet[p] << 8 | packet[p + 1]) / 1e3; p += 2;
			g->config.mamu2 = (packet[p] << 8 | packet[p + 1]) / 1e3; p += 2;
			g->config.cst = (packet[p] << 8 | packet[p + 1]) / 1e3; p += 2;

			if (p < packet_len) {
				if (packet[p] != PROTOCOL_VERSION) {
					printf("unsupported protocol version: expected %d, got %d\n", PROTOCOL_VERSION, packet[p]);
					g->network_done = 1;
					g->frame_write = 1;
				}
			}

			set_mscps_fmlts_fpsls(g);

			printf("protocol version = %d\n", packet[p]);
			printf("grd = %d\n", g->config.grd);
			printf("mscps = %d\n", g->config.mscps);
			printf("sector_size = %.3f\n", g->config.sector_size);
			printf("ssd256 = %.3f\n", g->config.ssd256);
			printf("spangdv = %.3f\n", g->config.spangdv);
			printf("nsp1 = %.3f\n", g->config.nsp1);
			printf("nsp2 = %.3f\n", g->config.nsp2);
			printf("nsp3 = %.3f\n", g->config.nsp3);
			printf("mamu = %.3f\n", g->config.mamu);
			printf("mamu2 = %.3f\n", g->config.mamu2);
			printf("cst = %.3f\n", g->config.cst);
			
			g->config.kills_display = 0;
		} else if (packet_type == 'e' || packet_type == 'E' || packet_type == '3' || packet_type == '4' || packet_type == '5' || packet_type == 'd' || packet_type == '7') {
			snake* o = NULL;
			if (packet_type == 'd' || packet_type == '7' || packet_len <= 5 && (packet_type == 'e' || packet_type == 'E' || packet_type == '3' || packet_type == '4' || packet_type == '5')) {
				o = g->os.snakes + 0;
			}
			else {
				int id = packet[p] << 8 | packet[p + 1]; p += 2;
				o = snake_map_get(&g->os, id);
			}

			int dir = -1;
			float ang = -1;
			float wang = -1;
			float speed = -1;

			if (packet_len == 8) {
				if (packet_type == 'e') dir = 1;
				else dir = 2;
				ang = packet[p] * 2 * PI / 256.0f;
				p++;
				wang = packet[p] * 2 * PI / 256.0f;
				p++;
				speed = packet[p] / 18.0f;
				p++;
			} else if (packet_len == 7 || packet_len == 5) {
				if (packet_type == 'e') {
					ang = packet[p] * 2 * PI / 256.0f;
					p++;
					speed = packet[p] / 18.0f;
					p++;
				}
				else if (packet_type == 'E') {
					dir = 1;
					wang = packet[p] * 2 * PI / 256.0f;
					p++;
					speed = packet[p] / 18.0f;
					p++;
				}
				else if (packet_type == '4') {
					dir = 2;
					wang = packet[p] * 2 * PI / 256.0f;
					p++;
					speed = packet[p] / 18.0f;
					p++;
				}
				else if (packet_type == '3') {
					dir = 1;
					ang = packet[p] * 2 * PI / 256.0f;
					p++;
					wang = packet[p] * 2 * PI / 256.0f;
					p++;
				}
				else {
					if (packet_type == '5') {
						dir = 2;
						ang = packet[p] * 2 * PI / 256.0f;
						p++;
						wang = packet[p] * 2 * PI / 256.0f;
						p++;
					}
				}
			} else {
				if (packet_len == 6 || packet_len == 4) {
					if (packet_type == 'e') {
						ang = packet[p] * 2 * PI / 256.0f;
						p++;
					}
					else if (packet_type == 'E') {
						dir = 1;
						wang = packet[p] * 2 * PI / 256.0f;
						p++;
					}
					else if (packet_type == '4') {
						dir = 2;
						wang = packet[p] * 2 * PI / 256.0f;
						p++;
					}
					else if (packet_type == '3') {
						speed = packet[p] / 18.0f;
						p++;
					}
					else if (packet_type == 'd') {
						dir = 1;
						ang = packet[p] * 2 * PI / 256.0f;
						p++;
						wang = packet[p] * 2 * PI / 256.0f;
						p++;
						speed = packet[p] / 18.0f;
						p++;
					}
					else if (packet_type == '7') {
						dir = 2;
						ang = packet[p] * 2 * PI / 256.0f;
						p++;
						wang = packet[p] * 2 * PI / 256.0f;
						p++;
						speed = packet[p] / 18.0f;
						p++;
					}
				}
			}

			if (o) {
				if (dir != -1) o->dir = dir;
				if (ang != -1) {
					float da = fmodf(ang - o->ang, X2PI);
					if (da < 0) da += X2PI;
					if (da > PI) da -= X2PI;
					int k = o->fapos;
					for (int j = 0; j < AFC; j++) {
						o->fas[k] -= da * g->config.afas[j];
						k++;
						if (k >= AFC) k = 0;
					}
					o->fatg = AFC;
					o->ang = ang;
				}
				if (wang != -1) {
					o->wang = wang;
					if (g->snake_null || o != g->os.snakes + 0) {
						o->eang = wang;
					}
				}
				if (speed != -1) {
					o->sp = speed;
					o->spang = o->sp / g->config.spangdv;
					if (o->spang > 1) o->spang = 1;
				}
			}

		} else if (packet_type == 'p') { // pong packet
			// printf("pong\n");
			g->config.wfpr = 0;
			if (g->config.lagging) {
				g->config.lagging = 0;
			}

		} else if (packet_type == 's') { // add/remove snake
			int id = packet[p] << 8 | packet[p + 1]; p += 2;

			if (packet_len > 9) {
				float ang = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) * 2.0f * PI / 16777215.0f; p += 3;
				int dir = packet[p] - 48; p++;
				float wang = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) * 2.0f * PI / 16777215.0f; p += 3;
				float speed = (packet[p] << 8 | packet[p + 1]) / 1e3; p += 2;
				float fam = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) / 16777215.0f; p += 3;
				int cv = (int) fmaxf(0, fminf(packet[p], 65)); p++;
				body_part* pts = ig_darray_create(body_part);
				float snx = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) / 5.0f; p += 3;
				float sny = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) / 5.0f; p += 3;
				uint8_t nl, anl; nl = anl = packet[p]; p++; if (nl > 24) nl = 24;
				char nk[24] = {};
				int skin_data_len = g->config.default_skins[cv][0];
				uint8_t* skin_data = g->config.default_skins[cv] + 1;
				int cusk = 0;

				for (int j = 0; j < nl; j++) {
					nk[j] = packet[p + j];
				}
				p += anl;
				int skl = packet[p]; p++;
				if (skl > 0) {
					cusk = 1;
					skin_data_len = 0;
					for (int j = 8; j < skl; j += 2) {
						uint8_t cg_len = packet[p + j];
						skin_data_len += cg_len;
					}
					
					skin_data = malloc(sizeof(uint8_t) * skin_data_len);
					int i = 0;
					for (int j = 8; j < skl; j += 2) {
						uint8_t cg_len = packet[p + j];
						uint8_t cg_id = packet[p + j + 1];
						for (int k = 0; k < cg_len; k++) {
							skin_data[i++] = cg_id;
						}
					}
				}
				p += skl;
				p++;

				float xx = 0;
				float yy = 0;
				float lx = 0;
				float ly = 0;

				lx = xx;
				ly = yy;
				xx = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) / 5.0f; p += 3;
				yy = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) / 5.0f; p += 3;
				lx = xx;
				ly = yy;

				ig_darray_push(&pts, (&(body_part) {
					.eiu = 0,
					.xx = xx,
					.yy = yy,
					.fx = 0,
					.fy = 0,
					.da = 0,
					.ebx = xx - lx,
					.eby = yy - ly
				}));

				while (p < packet_len) {
					lx = xx;
					ly = yy;
					xx += (packet[p] - 127) / 2.0f; p++;
					yy += (packet[p] - 127) / 2.0f; p++;
					ig_darray_push(&pts, (&(body_part) {
						.eiu = 0,
						.xx = xx,
						.yy = yy,
						.fx = 0,
						.fy = 0,
						.da = 0,
						.ebx = xx - lx,
						.eby = yy - ly
					}));
				}
				snake o = {
					.id = id,
					.xx = snx,
					.yy = sny,
					.ang = ang,
					.ehang = ang,
					.wehang = ang,
					.eang = ang,
					.wang = ang,
					.scang = 1,
					.skin_data_len = skin_data_len,
					.skin_data = skin_data,
					.cusk = cusk,
					.ehl = 1
				};

				int pts_len = ig_darray_length(pts);
				if (pts_len) {
					o.pts = pts;
					o.sct = pts_len;
					if (pts[0].dying) o.sct--;
				} else {
					ig_darray_destroy(pts);
					o.pts = ig_darray_create(body_part);
					o.sct = 0;
				}

				o.tl = o.sct + o.fam;
				o.cfl = o.tl;

				if (snake_map_get_total(&g->os) == 0) {
					g->snake_null = 0;
					g->config.view_xx = xx;
					g->config.view_yy = yy;
					// snake.md = false;
					// snake.wmd = false;
					g->config.lfsx = -1;
					g->config.lfsy = -1;
					g->config.lfcv = 0;
					g->config.lfvsx = -1;
					g->config.lfvsy = -1;
					g->config.lfesid = -1;

					pthread_mutex_lock(&g->connecting_mutex);
					g->connected = 1;
					pthread_cond_signal(&g->connecting_cond);
					pthread_mutex_unlock(&g->connecting_mutex);
				}

				strcpy(o.nk, nk);
				o.eang = o.wang = wang;
				o.sp = speed;
				o.spang = o.sp / g->config.spangdv;	
				if (o.spang > 1) o.spang = 1;
				o.fam = fam;
				o.sc = fminf(6, 1 + (o.sct - 2) / 106.0f);
				o.scang = 0.13f + 0.87f * powf((7 - o.sc) / 6.0f, 2);
				o.ssp = g->config.nsp1 + g->config.nsp2 * o.sc;
				o.fsp = o.ssp + 0.1f;
				o.wsep = 6 * o.sc;
				float mwsep = 4.5f / g->config.gsc;
				if (o.wsep < mwsep) o.wsep = mwsep;
				o.sep = o.wsep;
				snake_update_length(&o, g);
				snake_map_put(&g->os, id, &o);
			}
			else {
				int is_kill = packet[p] == 1; p++;
				int snakes_len = snake_map_get_total(&g->os);
				for (int i = snakes_len - 1; i >= 0; i--) {
					if (g->os.snakes[i].id == id) {
						g->os.snakes[i].id = -1234;
						if (is_kill) {
							g->os.snakes[i].dead = 1;
							g->os.snakes[i].dead_amt = 0;
						} else
							snake_map_remove_idx(&g->os, i);
						break;
					}
				}
			}
		} else if (packet_type == 'k') {
			int killed_id = packet[p] << 8 | packet[p + 1]; p += 2;
			g->config.kills_display = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]); p += 3;

			if (g->settings_instance.notify_kills) {
				message msg = {
					.message = "Kill",
					.tt = 1,
					.color = { .x = 1, .y = 0.6f, .z = 0.3f, .w = 1 }
				};

				pthread_mutex_lock(&g->msg_mutex);
				message_queue_push(&g->msg_queue, &msg);
				pthread_mutex_unlock(&g->msg_mutex);
			}
		} else if (packet_type == 'G' || packet_type == 'N' || packet_type == 'g' || packet_type == 'n') { // update snake length
			snake* o = NULL;
			if (packet_type == 'g' && packet_len == 7 || packet_type == 'G' && packet_len == 5 || packet_type == 'n' && packet_len == 10 || packet_type == 'N' && packet_len == 8)
				o = g->os.snakes + 0;
			else {
				int id = packet[p] << 8 | packet[p + 1]; p += 2;
				o = snake_map_get(&g->os, id);
			}

			if (o) {
				int adding_only = packet_type == 'N' || packet_type == 'n';

				int pts_len = ig_darray_length(o->pts);
				if (adding_only) {
					// if (o == g->os.snakes) printf("adding\n");
					o->sct++;
				}
				else {
					for (int j = 0; j < pts_len; j++) {
						if (!o->pts[j].dying) {
							o->pts[j].dying = 1;
							break;
						}
					}
				}

				body_part* po = o->pts + (pts_len - 1);
				int lpo_i = pts_len - 1;
				body_part* lmpo;
				body_part* mpo;
				float dx, dy;

				float xx = 0;
				float yy = 0;

				if (packet_type == 'g' || packet_type == 'n') {
					xx = packet[p] << 8 | packet[p + 1]; p += 2;
					yy = packet[p] << 8 | packet[p + 1]; p += 2;
				} else {
					xx = po->xx + packet[p] - 128; p++;
					yy = po->yy + packet[p] - 128; p++;
				}

				if (adding_only) {
					o->fam = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) / 16777215.0f; p += 3;
				}

				ig_darray_push(&o->pts, (&(body_part) {
					.xx = xx,
					.yy = yy
				}));

				pts_len = ig_darray_length(o->pts);
				po = o->pts + (pts_len - 1);

				po->ebx = po->xx - o->pts[lpo_i].xx;
				po->eby = po->yy - o->pts[lpo_i].yy;

				body_part* lpo = po;

				if (o->iiv) {
					dx = o->xx + o->fx - po->xx;
					dy = o->yy + o->fy - po->yy;
					po->fx += dx;
					po->fy += dy;
					po->exs[po->eiu] = dx;
					po->eys[po->eiu] = dy;
					po->efs[po->eiu] = 0;
					po->ems[po->eiu] = 1;
					po->eiu++;
				}

				pts_len = ig_darray_length(o->pts);
				int k = pts_len - 3;
				if (k >= 1) {
					lmpo = o->pts + k;
					int n = 0;
					float mv = 0;
					for (int j = k - 1; j >= 0; j--) {
						mpo = o->pts + j;
						n++;
						dx = mpo->xx;
						dy = mpo->yy;
						if (n <= 4) mv = g->config.cst * n / 4.0f;
						mpo->xx += (lmpo->xx - mpo->xx) * mv;
						mpo->yy += (lmpo->yy - mpo->yy) * mv;
						if (o->iiv) {
							dx -= mpo->xx;
							dy -= mpo->yy;
							mpo->fx += dx;
							mpo->fy += dy;
							mpo->exs[mpo->eiu] = dx;
							mpo->eys[mpo->eiu] = dy;
							mpo->efs[mpo->eiu] = 0;
							mpo->ems[mpo->eiu] = 2;
							mpo->eiu++;
						}
						lmpo = mpo;
					}
				}

				o->sc = fminf(6, 1 + (o->sct - 2) / 106.0f);
				o->scang = 0.13f + 0.87f * powf((7 - o->sc) / 6.0f, 2);
				o->ssp = g->config.nsp1 + g->config.nsp2 * o->sc;
				o->fsp = o->ssp + 0.1f;
				o->wsep = 6.0f * o->sc;
				float mwsep = 4.5f / g->config.gsc;
				if (o->wsep < mwsep) o->wsep = mwsep;
				if (adding_only) snake_update_length(o, g);

				float ovxx;
				float ovyy;

				if (o == g->os.snakes + 0 && !g->snake_null) {
					ovxx = o->xx + o->fx;
					ovyy = o->yy + o->fy;
				}
				float csp = o->sp * (0 / 8.0f) / 4;
				csp *= g->config.lag_mult;
				float ochl = o->chl - 1;
				o->chl = csp / 42.0f;
				float ox = o->xx;
				float oy = o->yy;
				o->xx = xx + cosf(o->ang) * csp;
				o->yy = yy + sinf(o->ang) * csp;
				dx = o->xx - ox;
				dy = o->yy - oy;
				float dchl = o->chl - ochl;
				k = o->fpos;
				for (int j = 0; j < RFC; j++) {
					o->fxs[k] -= dx * g->config.rfas[j];
					o->fys[k] -= dy * g->config.rfas[j];
					o->fchls[k] -= dchl * g->config.rfas[j];
					k++;
					if (k >= RFC) k = 0;
				}
				o->fx = o->fxs[o->fpos];
				o->fy = o->fys[o->fpos];
				o->fchl = o->fchls[o->fpos];
				o->ftg = RFC;
				o->ehl = 0;
				if (o == g->os.snakes + 0 && !g->snake_null) {
					float lvx = g->config.view_xx;
					float lvy = g->config.view_yy;
					g->config.view_xx = o->xx + o->fx;
					g->config.view_yy = o->yy + o->fy;
					g->config.bgx2 -= (g->config.view_xx - lvx) * 1 / g->config.bgw2;
					g->config.bgy2 -= (g->config.view_yy - lvy) * 1 / g->config.bgh2;
					// g->config.bgx2 = fmod(g->config.bgx2, 1.0f);
					// if (g->config.bgx2 < 0) g->config.bgx2 += 1;
					// g->config.bgy2 = fmodf(g->config.bgy2, 1.0f);
					// if (g->config.bgy2 < 0) g->config.bgy2 += 1;
					float dx = g->config.view_xx - ovxx;
					float dy = g->config.view_yy - ovyy;
					k = g->config.fvpos;
					for (int j = 0; j < VFC; j++) {
						g->config.fvxs[k] -= dx * g->config.vfas[j];
						g->config.fvys[k] -= dy * g->config.vfas[j];
						k++;
						if (k >= VFC) k = 0;
					}
					g->config.fvtg = VFC;
				}
			}
		} else if (packet_type == 'r') { // remove last body part (tail)
			int id = packet[p] << 8 | packet[p + 1]; p += 2;
			snake* o = snake_map_get(&g->os, id);

			if (o) {
				if (packet_len >= 7) {
					o->fam = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) / 16777215.0f; p += 3;
				}
				int pts_len = ig_darray_length(o->pts);
				for (int j = 0; j < pts_len; j++) {
					if (!o->pts[j].dying) {
						o->pts[j].dying = 1;
						o->sct--;
						o->sc = fminf(6, 1 + (o->sct - 1) / 106.0f);
						o->scang = 0.13 + 0.87 * powf((7 - o->sc) / 6.0f, 2);
						o->ssp = g->config.nsp1 + g->config.nsp2 * o->sc;
						o->fsp = o->ssp + 0.1;
						o->wsep = 6 * o->sc;
						float mwsep = 4.5f / g->config.gsc;
                        if (o->wsep < mwsep) o->wsep = mwsep;
						break;
					}
				}
				snake_update_length(o, g);
			}
		} else if (packet_type == 'h') { // update fam
			int id = packet[p] << 8 | packet[p + 1]; p += 2;
			snake* o = snake_map_get(&g->os, id);
			if (o) {
				o->fam = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) / 16777215.0f; p += 3;
				snake_update_length(o, g);
			}
		} else if (packet_type == 'F') { // FOOD
			int sx = packet[p]; p++;
			int sy = packet[p]; p++;
			float axx = sx * g->config.sector_size;
			float ayy = sy * g->config.sector_size;
			float xx, yy;
			int rx, ry;
			int cv, id;
			float rad;

			while (p < packet_len) {
				cv = packet[p]; p++;
				rx = packet[p]; p++;
				ry = packet[p]; p++;
				xx = axx + rx * g->config.ssd256;
				yy = ayy + ry * g->config.ssd256;
				rad = packet[p] / 5.0f; p++;
				id = sx << 24 | sy << 16 | rx << 8 | ry;
				int cv2 = floorf(FOOD_SIZES * g->config.gsc * rad / 16.5f);
				if (cv2 < 0) cv2 = 0;
				if (cv2 >= FOOD_SIZES) cv2 = FOOD_SIZES - 1;

				food fo = {
					.sx = sx,
					.sy = sy,
					.id = id,
					.xx = xx,
					.yy = yy,
					.rx = xx,
					.ry = yy,
					.rsp = 2,
					.cv = cv,
					.rad = 1e-5,
					.sz = rad,
					.lrrad = 1e-5,
					.f = g->config.fs[cv2],
					.f2 = g->config.f2s[cv2],
					.fr = 0,
					.gfr = (float) rand() / (float) (RAND_MAX) * 64.0f,
					.gr = 0.65f + 0.1f * rad,
					.wsp = (2.0 * ((float) rand() / RAND_MAX) - 1.0) * 0.0225f,
					.eaten_fr = 0,
					.eaten_by = -1
				};

				ig_darray_push(&g->foods, &fo);
			}
		} else if (packet_type == 'b' || packet_type == 'f') { // FOOD
			int sx, sy;
			if (packet_len >= 8) {
				sx = packet[p]; p++;
				sy = packet[p]; p++;
				g->config.lfsx = sx;
				g->config.lfsy = sy;
			}
			else {
				sx = g->config.lfsx;
				sy = g->config.lfsy;
			}
			int rx = packet[p]; p++;
			int ry = packet[p]; p++;
			float xx = sx * g->config.sector_size + rx * g->config.ssd256;
			float yy = sy * g->config.sector_size + ry * g->config.ssd256;

			int id = sx << 24 | sy << 16 | rx << 8 | ry;
			int cv;
			if (packet_len == 7 || packet_len == 9) {
				cv = packet[p]; p++;
				g->config.lfcv = cv;
			}
			else cv = g->config.lfcv;
			float rad = packet[p] / 5.0f; p++;
			int cv2 = floorf(FOOD_SIZES * g->config.gsc * rad / 16.5f);
			if (cv2 < 0) cv2 = 0;
			if (cv2 >= FOOD_SIZES) cv2 = FOOD_SIZES - 1;
			
			food fo = {
				.sx = sx,
				.sy = sy,
				.id = id,
				.xx = xx,
				.yy = yy,
				.rx = xx,
				.ry = yy,
				.rsp = (packet_type == 'b') + 1,
				.cv = cv,
				.rad = 1e-5,
				.sz = rad,
				.lrrad = 1e-5,
				.f = g->config.fs[cv2],
				.f2 = g->config.f2s[cv2],
				.fr = 0,
				.gfr = (float) rand() / (float) (RAND_MAX) * 64.0f,
				.gr = 0.65f + 0.1f * rad,
				.wsp = (2.0 * ((float) rand() / RAND_MAX) - 1.0) * 0.0225f,
				.eaten_fr = 0,
				.eaten_by = -1
			};

			ig_darray_push(&g->foods, &fo);
		} else if (packet_type == 'C' || packet_type == '<') { // FOOD
			int id;
			int ebid = -1;
			int sx, sy, rx, ry;

			if (packet_type == '<' && packet_len == 7 || packet_type == 'C' && packet_len == 5) {
				sx = g->config.lfvsx;
				sy = g->config.lfvsy;
			}
			else {
				sx = packet[p]; p++;
				sy = packet[p]; p++;
				g->config.lfvsx = sx;
				g->config.lfvsy = sy;
			}
			rx = packet[p]; p++;
			ry = packet[p]; p++;
			id = sx << 24 | sy << 16 | rx << 8 | ry;

			if (packet_type == '<') {
				ebid = packet[p] << 8 | packet[p + 1]; p += 2;
				g->config.lfesid = ebid;
			} else if (packet_type == 'C')
				ebid = g->config.lfesid;

			int cm1 = ig_darray_length(g->foods) - 1;
			for (int i = cm1; i >= 0; i--) {
				food* fo = g->foods + i;
				if (fo->id == id) {
					fo->eaten = true;
					if (ebid >= 0) {
						fo->eaten_by = ebid;
						fo->eaten_fr = 0;
					}
					else {
						ig_darray_remove(g->foods, i);
					}
					id = -1;
					break;
				}
			}
		} else if (packet_type == 'w') { // FOOD
			int xx = packet[p]; p++;
			int yy = packet[p]; p++;

			int cm1 = ig_darray_length(g->foods) - 1;
			for (int i = cm1; i >= 0; i--) {
				food* fo = g->foods + i;
				if (fo->sx == xx) {
					if (fo->sy == yy) {
						ig_darray_remove(g->foods, i);
					}
				}
			}
		} else if (packet_type == 'v') { // died
			// g->os.snakes[0].id = -1;
			printf("died\n");
			int final_score = floorf((g->config.fpsls[g->os.snakes[0].sct] + g->os.snakes[0].fam / g->config.fmlts[g->os.snakes[0].sct] - 1) * 15 - 5) / 1;
			printf("score was %d\n", final_score);

			// g->network_done = 1;
			// g->frame_write = 1;

		} else if (packet_type == 'l') { // leaderboard
			memset(g->leaderboard, 0, sizeof(g->leaderboard));
			// printf("leaderboard packet:\n");
			g->config.my_pos = packet[p]; p++;
			p += 2;
			p += 2;

			int curr_n = 0;
			while (p < packet_len) {
				int sct = packet[p] << 8 | packet[p + 1]; p += 2;
				float fam = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) / 16777215.0f; p += 3;
				int cv = packet[p] % 9; p++;
				uint8_t nl, anl; nl = anl = packet[p]; p++; if (nl > 24) nl = 24;
				leaderboard_entry* entry = g->leaderboard + curr_n;
				
				if (g->config.my_pos == curr_n + 1 && !g->snake_null) {
					strcpy(entry->nk, g->os.snakes[0].nk);
				} else {
					for (uint8_t j = 0; j < nl; j++) {
						entry->nk[j] = (char) packet[p + j];
					}
				}
				p += anl;

				entry->score = (int) (floorf((g->config.fpsls[sct] + fam / g->config.fmlts[sct] - 1) * 15 - 5) / 1.0f);
				entry->cv = cv;
				curr_n++;
			}
		} else if (packet_type == 'M') { // minimap
			p += 2;
			int xx = 136 - 1;
			int yy = 136 - 1;
			int u_m[] = { 64, 32, 16, 8, 4, 2, 1 };
			memset(g->config.mmdata, 0, 136 * 136);

			while (p < packet_len) {
				if (yy < 0) break;
				int k = packet[p++];
				if (k >= 128) {
					if (k == 255) k = 126 * packet[p++];
					else k -= 128;
					for (int i = 0; i < k; i++) {
						xx--;
						if (xx < 0) {
							xx = 136 - 1;
							yy--;
							if (yy < 0) break;
						}
					}
				}
				else {
					for (int i = 0; i < 7; i++) {
						if ((k & u_m[i]) > 0) {
							g->config.mmdata[yy * 136 + xx] = 255;
						}
						xx--;
						if (xx < 0) {
							xx = 136 - 1;
							yy--;
							if (yy < 0) break;
						}
					}
				}
			}
		} else if (packet_type == 'V') { // minimap
			int xx = 136 - 1;
			int yy = 136 - 1;
			int u_m[] = { 64, 32, 16, 8, 4, 2, 1 };

			while (p < packet_len) {
				if (yy < 0) break;
				int k = packet[p++];
				if (k >= 128) {
					if (k == 255) k = 126 * packet[p++];
					else k -= 128;
					for (int i = 0; i < k; i++) {
						xx--;
						if (xx < 0) {
							xx = 136 - 1;
							yy--;
							if (yy < 0) break;
						}
					}
				}
				else {
					for (int i = 0; i < 7; i++) {
						if ((k & u_m[i]) > 0) {
							int j = yy * 136 + xx;
							if (g->config.mmdata[j]) {
								g->config.mmdata[j] = 0;
								// ctx.clearRect(xx, yy, 1, 1)
							}
							else {
								g->config.mmdata[j] = 255;
								// ctx.fillRect(xx, yy, 1, 1)
							}
						}
						xx--;
						if (xx < 0) {
							xx = 136 - 1;
							yy--;
							if (yy < 0) break;
						}
					}
				}
			}
		} else if (packet_type == 'y') { // new prey
			int id = packet[p] << 8 | packet[p + 1]; p += 2;
			if (packet_len == 5) {
				int preys_len = ig_darray_length(g->preys);
				for (int i = preys_len - 1; i >= 0; i--) {
					prey* pr = g->preys + i;
					if (pr->id == id) {
						ig_darray_remove(g->preys, i);
						break;
					}
				}
			} else if (packet_len == 7) {
				int snake_id = packet[p] << 8 | packet[p + 1]; p += 2;
				int preys_len = ig_darray_length(g->preys);
				for (int i = preys_len - 1; i >= 0; i--) {
					prey* pr = g->preys + i;
					if (pr->id == id) {
						pr->eaten = 1;
						pr->eaten_by = snake_id;
						if (snake_map_get(&g->os, snake_id)) pr->eaten_fr = 0;
						else ig_darray_remove(g->preys, i);
						break;
					}
				}
			} else {
				int cv = packet[p]; p++;
				float xx = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) / 5.0f; p += 3;
				float yy = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) / 5.0f; p += 3;
				float rad = packet[p] / 5.0f; p++;
				int dir = packet[p] - 48; p++;
				float wang = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) * 2 * PI / 16777215.0f; p += 3;
				float ang = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) * 2 * PI / 16777215.0f; p += 3;
				float speed = (packet[p] << 8 | packet[p + 1]) / 1e3; p += 2;

				int cv2 = floorf(PREY_SIZES * g->config.gsc * rad / 9.0f);
				if (cv2 < 0) cv2 = 0;
				if (cv2 >= PREY_SIZES) cv2 = PREY_SIZES - 1;
				
				ig_darray_push(&g->preys, (&(prey) {
					.id = id,
					.xx = xx,
					.yy = yy,
					.rad = 1e-5,
					.sz = rad,
					.cv = cv,
					.dir = dir,
					.wang = wang,
					.ang = ang,
					.sp = speed,
					.gfr = (float) rand() / RAND_MAX * 64.0f,
					.gr = 0.5f + (float) rand() / RAND_MAX * 0.15f + 0.1f * rad,
					.f = g->config.pr_fs[cv2],
					.f2 = g->config.pr_f2s[cv2],
				}));
			}
		} else if (packet_type == 'j') { // prey move
			int id = packet[p] << 8 | packet[p + 1]; p += 2;
			float xx = 1 + (packet[p] << 8 | packet[p + 1]) * 3; p += 2;
			float yy = 1 + (packet[p] << 8 | packet[p + 1]) * 3; p += 2;
			prey* pr = NULL;
			int preys_len = ig_darray_length(g->preys);
			for (int i = preys_len - 1; i >= 0; i--) {
				if (g->preys[i].id == id) {
					pr = g->preys + i;
					break;
				}
			}
			if (pr) {
				float csp = 0;
				float ox = pr->xx;
				float oy = pr->yy;
				if (packet_len == 18) {
					pr->dir = packet[p] - 48; p++;
					pr->ang = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) * 2.0f * PI / 16777215.0f; p += 3;
					pr->wang = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) * 2.0f * PI / 16777215.0f; p += 3;
					pr->sp = (packet[p] << 8 | packet[p + 1]) / 1e3; p += 2;
				}
				else if (packet_len == 14) {
					pr->ang = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) * 2.0f * PI / 16777215.0f; p += 3;
					pr->sp = (packet[p] << 8 | packet[p + 1]) / 1e3; p += 2;
				}
				else if (packet_len == 15) {
					pr->dir = packet[p] - 48; p++;
					pr->wang = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) * 2.0f * PI / 16777215.0f; p += 3;
					pr->sp = (packet[p] << 8 | packet[p + 1]) / 1e3; p += 2;
				}
				else if (packet_len == 16) {
					pr->dir = packet[p] - 48; p++;
					pr->ang = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) * 2.0f * PI / 16777215.0f; p += 3;
					pr->wang = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) * 2.0f * PI / 16777215.0f; p += 3;
				}
				else if (packet_len == 12) {
					pr->ang = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) * 2.0f * PI / 16777215.0f; p += 3;
				}
				else if (packet_len == 13) {
					pr->dir = packet[p] - 48; p++;
					pr->wang = (packet[p] << 16 | packet[p + 1] << 8 | packet[p + 2]) * 2.0f * PI / 16777215.0f; p += 3;
				}
				else if (packet_len == 11) {
					pr->sp = (packet[p] << 8 | packet[p + 1]) / 1e3; p += 2;
				}
				pr->xx = xx + cosf(pr->ang) * csp;
				pr->yy = yy + sinf(pr->ang) * csp;
				float dx = pr->xx - ox;
				float dy = pr->yy - oy;
				int k = pr->fpos;
				for (int j = 0; j < RFC; j++) {
					pr->fxs[k] -= dx * g->config.rfas[j];
					pr->fys[k] -= dy * g->config.rfas[j];
					k++;
					if (k >= RFC) k = 0;
				}
				pr->fx = pr->fxs[pr->fpos];
				pr->fy = pr->fys[pr->fpos];
				pr->ftg = RFC;
			}
		}
	} else if (ev == MG_EV_WAKEUP) { // ypdate & render packet
		struct mg_ws_message* wm = (struct mg_ws_message*) ev_data;
		const input_data* inp_data = (const input_data*) wm->data.buf;
		oef(g, c, inp_data);
		g->woke_up = true;

		pthread_mutex_lock(&g->render_mutex);
		redraw(g, inp_data->fps_display);
		g->frame_write = 1;
		pthread_cond_signal(&g->render_cond);
		pthread_mutex_unlock(&g->render_mutex);

	} else if (ev == MG_EV_OPEN) {
		printf("opening connection\n");
	} else if (ev == MG_EV_ERROR) {
		MG_ERROR(("error: %p %s", c->fd, (char*) ev_data));
	} else if (ev == MG_EV_WS_OPEN) {
		printf("connection opened, sent startlogin\n");
		mg_ws_send(c, "c", 1, WEBSOCKET_OP_BINARY);
	} else if (ev == MG_EV_CLOSE) {
		printf("connection closed\n");
		if (!g->woke_up && g->connected) {
			pthread_mutex_lock(&g->render_mutex);
			redraw(g, -1);
			g->frame_write = 1;
			pthread_cond_signal(&g->render_cond);
			pthread_mutex_unlock(&g->render_mutex);
		}
		pthread_mutex_lock(&g->connecting_mutex);
		g->network_done = 1;
		g->connected = 0;
		// printf("frame_write is %d\n", g->frame_write);
		pthread_mutex_unlock(&g->connecting_mutex);
	}
}