#include "oef.h"

#include "../user.h"

void time_step(tenv* env) {
  tuser_data* usr = env->usr;
  tcontext* ctx = env->ctx;
  game_data* gdata = &usr->gdata;

  double time_sec = glfwGetTime();
  gdata->data.ctm = time_sec * 1000;

  if (gdata->data.follow_view) gdata->data.play_etm = time_sec;

  gdata->data.fps_etm = (gdata->data.ctm - gdata->data.fps_ltm);
  gdata->data.vfr = (gdata->data.ctm - gdata->data.ltm) / 8.0f;
  if (gdata->data.vfr > 5) gdata->data.vfr = 5;
  if (gdata->data.vfr < 0) gdata->data.vfr = 0;
  gdata->data.avfr = gdata->data.vfr;
  gdata->data.ltm = gdata->data.ctm;

  if (!gdata->data.lagging) {
    if (gdata->data.wfpr && gdata->data.ctm - gdata->data.last_ping_mtm > 750) {
      gdata->data.lagging = true;
    }
  }

  if (gdata->data.lagging) {
    gdata->data.lag_mult -= 0.05f * gdata->data.vfr;
    if (gdata->data.lag_mult < .2) gdata->data.lag_mult = .2;
  } else if (gdata->data.lag_mult < 1) {
    gdata->data.lag_mult += 0.05f * gdata->data.vfr;
    if (gdata->data.lag_mult >= 1) gdata->data.lag_mult = 1;
  }

  if (gdata->data.vfr > 120) gdata->data.vfr = 120;

  gdata->data.vfr *= gdata->data.lag_mult;
  float lfr = gdata->data.fr;
  gdata->data.fr += gdata->data.vfr;
  gdata->data.vfrb = (int)(floorf(gdata->data.fr) - floorf(lfr));
}

void oef(tenv* env) {
  tuser_data* usr = env->usr;
  tcontext* ctx = env->ctx;
  game_data* gdata = &usr->gdata;
  user_settings* usrs = &usr->usrs;
  gameplay_mode* mode = usrs->modes + usrs->hotkeys[HOTKEY_ASSIST].active;

  gdata->data.gsc = usrs->smooth_zoom
                        ? glm_lerp(gdata->data.gsc, gdata->data.ms_zoom,
                                   0.25f * gdata->data.vfr)
                        : gdata->data.ms_zoom;

  // update flux:
  if (gdata->data.vfrb > 0) {
    if (gdata->data.flx_tg > 0) {
      int ki = gdata->data.vfrb;
      if (ki > gdata->data.flx_tg) ki = gdata->data.flx_tg;
      gdata->data.flx_tg -= ki;
      for (int qq = 1; qq <= ki; qq++) {
        if (qq == ki)
          gdata->data.flux_grd =
              gdata->data.flux_grds[gdata->data.flux_grd_pos];
        gdata->data.flux_grds[gdata->data.flux_grd_pos] =
            gdata->data.real_flux_grd;
        gdata->data.flux_grd_pos++;
        if (gdata->data.flux_grd_pos >= GD_FLXC) gdata->data.flux_grd_pos = 0;
      }
    } else if (gdata->data.flx_tg == 0)
      gdata->data.flx_tg = -1;
  }

  if (gdata->data.play_etm >= LAST_POSITION_DURATION) {
    usr->r->global.lview[0] = -1;
    usr->r->global.lview[1] = -1;
  }

  // update snakes:
  int snakes_len = _tdarray_length(gdata->data.snakes);
  for (int i = snakes_len - 1; i >= 0; i--) {
    snake* o = gdata->data.snakes + i;
    float mang = gdata->data.mamu * gdata->data.vfr * o->scang * o->spang;
    float csp = o->sp * gdata->data.vfr / 4.0f;
    if (csp > o->msl) csp = o->msl;
    if (o->sep != o->wsep) {
      if (o->sep < o->wsep) {
        o->sep += 0.002f * gdata->data.vfr;
        if (o->sep >= o->wsep) o->sep = o->wsep;
      } else if (o->sep > o->wsep) {
        o->sep -= 0.002f * gdata->data.vfr;
        if (o->sep <= o->wsep) o->sep = o->wsep;
      }
    }
    if (!o->dead) {
      float rate_up = 0.05f;
      float rate_down = rate_up * 2;

      float diff = o->sp - o->tsp;

      if (diff > 0.0f) {
        float factor = 1.0f - expf(-rate_up * gdata->data.vfr);
        o->tsp += diff * factor;
      } else if (diff < 0.0f) {
        float factor = 1.0f - expf(-rate_down * gdata->data.vfr);
        o->tsp += diff * factor;
      }

      if (o->tsp > o->fsp) o->sfr += (o->tsp - o->fsp) * gdata->data.vfr * .021;
      if (o->fltg > 0) {
        int k = gdata->data.vfrb;
        if (k > o->fltg) k = o->fltg;
        o->fltg -= k;
        for (int qq = 0; qq < k; qq++) {
          o->fl = o->fls[o->flpos];
          o->fls[o->flpos] = 0;
          o->flpos++;
          if (o->flpos >= GD_EEZ) o->flpos = 0;
        }
      } else if (o->fltg == 0) {
        o->fltg = -1;
        o->fl = 0;
      }
      o->cfl = o->tl + o->fl - .6;
    }
    if (o->dir == 1) {
      o->ang -= mang;
      if (o->ang < 0 || o->ang >= PI2) o->ang = fmodf(o->ang, PI2);
      if (o->ang < 0) o->ang += PI2;
      float vang = fmodf(o->wang - o->ang, PI2);
      if (vang < 0) vang += PI2;
      if (vang > PI) vang -= PI2;
      if (vang > 0) {
        o->ang = o->wang;
        o->dir = 0;
      }
    } else if (o->dir == 2) {
      o->ang += mang;
      if (o->ang < 0 || o->ang >= PI2) o->ang = fmodf(o->ang, PI2);
      if (o->ang < 0) o->ang += PI2;
      float vang = fmodf(o->wang - o->ang, PI2);
      if (vang < 0) vang += PI2;
      if (vang > PI) vang -= PI2;
      if (vang < 0) {
        o->ang = o->wang;
        o->dir = 0;
      }
    } else
      o->ang = o->wang;

    if (!o->dead)
      if (o->ehang != o->wehang) {
        float vang = fmodf(o->wehang - o->ehang, PI2);
        if (vang < 0) vang += PI2;
        if (vang > PI) vang -= PI2;
        if (vang < 0)
          o->edir = 1;
        else if (vang > 0)
          o->edir = 2;
      }
    if (o->edir == 1) {
      float tang = fmodf(o->wehang - o->ehang, PI2);
      if (tang < 0) tang += PI2;
      if (tang > PI) tang -= PI2;
      o->ehang += tang * gdata->data.p12[gdata->data.vfrb];
      if (o->ehang < 0 || o->ehang >= PI2) o->ehang = fmodf(o->ehang, PI2);
      if (o->ehang < 0) o->ehang += PI2;
      float vang = fmodf(o->wehang - o->ehang, PI2);
      if (vang < 0) vang += PI2;
      if (vang > PI) vang -= PI2;
      if (vang > 0) {
        o->ehang = o->wehang;
        o->edir = 0;
      }
    } else if (o->edir == 2) {
      float tang = fmodf(o->wehang - o->ehang, PI2);
      if (tang < 0) tang += PI2;
      if (tang > PI) tang -= PI2;
      o->ehang += tang * gdata->data.p12[gdata->data.vfrb];
      if (o->ehang < 0 || o->ehang >= PI2) o->ehang = fmodf(o->ehang, PI2);
      if (o->ehang < 0) o->ehang += PI2;
      float vang = fmodf(o->wehang - o->ehang, PI2);
      if (vang < 0) vang += PI2;
      if (vang > PI) vang -= PI2;
      if (vang < 0) {
        o->ehang = o->wehang;
        o->edir = 0;
      }
    }
    if (!o->dead) {
      o->xx += cosf(o->ang) * csp;
      o->yy += sinf(o->ang) * csp;
      o->chl += csp / o->msl;
    }
    if (gdata->data.vfrb > 0) {
      int k = 0;
      int pts_len = tdarray_length(o->pts);
      for (int j = pts_len - 1; j >= 0; j--) {
        int po_i = j;
        if (o->pts[po_i].dying) {
          k++;
          o->pts[po_i].da += .0015 * gdata->data.vfrb;
          if (o->pts[po_i].da >= 1) {
            o->pts[po_i].da = 1;
            if (k >= 4) {
              tdarray_remove(o->pts, j);
            }
          }
        }
      }

      pts_len = tdarray_length(o->pts);
      for (int j = pts_len - 1; j >= 0; j--) {
        int po_i = j;
        if (o->pts[po_i].ftg > 0) {
          int k = gdata->data.vfrb;
          if (k > o->pts[po_i].ftg) k = o->pts[po_i].ftg;
          o->pts[po_i].ftg -= k;
          for (int qq = 0; qq < k; qq++) {
            o->pts[po_i].fx = o->pts[po_i].fxs[o->pts[po_i].fpos];
            o->pts[po_i].fy = o->pts[po_i].fys[o->pts[po_i].fpos];
            o->pts[po_i].fltn = o->pts[po_i].fltns[o->pts[po_i].fpos];
            o->pts[po_i].fsmu = o->pts[po_i].fsmus[o->pts[po_i].fpos];
            o->pts[po_i].fxs[o->pts[po_i].fpos] = 0;
            o->pts[po_i].fys[o->pts[po_i].fpos] = 0;
            o->pts[po_i].fltns[o->pts[po_i].fpos] = 0;
            o->pts[po_i].fsmus[o->pts[po_i].fpos] = 0;
            o->pts[po_i].fpos++;
            if (o->pts[po_i].fpos >= GD_EEZ) o->pts[po_i].fpos = 0;
          }
        } else if (o->pts[po_i].ftg == 0) {
          o->pts[po_i].ftg = -1;
          o->pts[po_i].fx = 0;
          o->pts[po_i].fy = 0;
          o->pts[po_i].fltn = 0;
          o->pts[po_i].fsmu = 0;
        }
      }
    }
    float wx = cosf(o->eang) * 2.3f;  // o.pma = 2.3
    float wy = sinf(o->eang) * 2.3f;  // o.pma = 2.3
    if (o->rex < wx) {
      o->rex += gdata->data.vfr / 6.0f;
      if (o->rex >= wx) o->rex = wx;
    }
    if (o->rey < wy) {
      o->rey += gdata->data.vfr / 6.0f;
      if (o->rey >= wy) o->rey = wy;
    }
    if (o->rex > wx) {
      o->rex -= gdata->data.vfr / 6.0f;
      if (o->rex <= wx) o->rex = wx;
    }
    if (o->rey > wy) {
      o->rey -= gdata->data.vfr / 6.0f;
      if (o->rey <= wy) o->rey = wy;
    }
    if (gdata->data.vfrb > 0) {
      if (o->ftg > 0) {
        int k = gdata->data.vfrb;
        if (k > o->ftg) k = o->ftg;
        o->ftg -= k;
        for (int qq = 0; qq < k; qq++) {
          o->fx = o->fxs[o->fpos];
          o->fy = o->fys[o->fpos];
          o->fchl = o->fchls[o->fpos];
          o->fxs[o->fpos] = 0;
          o->fys[o->fpos] = 0;
          o->fchls[o->fpos] = 0;
          o->fpos++;
          if (o->fpos >= GD_EEZ) o->fpos = 0;
        }
      } else if (o->ftg == 0) {
        o->ftg = -1;
        o->fx = 0;
        o->fy = 0;
        o->fchl = 0;
      }
      if (o->fatg > 0) {
        int k = gdata->data.vfrb;
        if (k > o->fatg) k = o->fatg;
        o->fatg -= k;
        for (int qq = 0; qq < k; qq++) {
          o->fa = o->fas[o->fapos];
          o->fas[o->fapos] = 0;
          o->fapos++;
          if (o->fapos >= GD_AFC) o->fapos = 0;
        }
      } else if (o->fatg == 0) {
        o->fatg = -1;
        o->fa = 0;
      }
    }
    if (o->dead) {
      o->dead_amt += gdata->data.vfr * (mode->death_effect ? 0.02f : 0.03f);
      if (o->dead_amt >= 1) {
        tdarray_push(&gdata->data.pts_dp, &o->pts);
        tdarray_push(&gdata->data.gptz_dp, &o->gptz);

        tdarray_remove(gdata->data.snakes, i);
      }
    } else if (o->alive_amt != 1) {
      o->alive_amt += gdata->data.vfr * 0.02f;
      if (o->alive_amt >= 1) o->alive_amt = 1;
    }
  }

  // update preys:
  int preys_len = tdarray_length(gdata->data.preys);
  for (int i = preys_len - 1; i >= 0; i--) {
    prey* pr = gdata->data.preys + i;
    float mang = gdata->data.mamu2 * gdata->data.vfr;
    float csp = pr->sp * gdata->data.vfr / 4;
    if (gdata->data.vfrb > 0) {
      if (pr->ftg > 0) {
        int k = gdata->data.vfrb;
        if (k > pr->ftg) k = pr->ftg;
        pr->ftg -= k;
        for (int qq = 1; qq <= k; qq++) {
          if (qq == k) {
            pr->fx = pr->fxs[pr->fpos];
            pr->fy = pr->fys[pr->fpos];
          }
          pr->fxs[pr->fpos] = 0;
          pr->fys[pr->fpos] = 0;
          pr->fpos++;
          if (pr->fpos >= GD_EEZ) pr->fpos = 0;
        }
      } else if (pr->ftg == 0) {
        pr->fx = 0;
        pr->fy = 0;
        pr->ftg = -1;
      }
    }
    if (pr->dir == 1) {
      pr->ang -= mang;
      if (pr->ang < 0 || pr->ang >= PI2) pr->ang = fmodf(pr->ang, PI2);
      if (pr->ang < 0) pr->ang += PI2;
      float vang = fmodf(pr->wang - pr->ang, PI2);
      if (vang < 0) vang += PI2;
      if (vang > PI) vang -= PI2;
      if (vang > 0) {
        pr->ang = pr->wang;
        pr->dir = 0;
      }
    } else if (pr->dir == 2) {
      pr->ang += mang;
      if (pr->ang < 0 || pr->ang >= PI2) pr->ang = fmodf(pr->ang, PI2);
      if (pr->ang < 0) pr->ang += PI2;
      float vang = fmodf(pr->wang - pr->ang, PI2);
      if (vang < 0) vang += PI2;
      if (vang > PI) vang -= PI2;
      if (vang < 0) {
        pr->ang = pr->wang;
        pr->dir = 0;
      }
    } else
      pr->ang = pr->wang;
    pr->xx += cosf(pr->ang) * csp;
    pr->yy += sinf(pr->ang) * csp;
    pr->gfr += gdata->data.vfr * pr->gr;
    if (pr->eaten) {
      if (pr->fr != 1.5f) {
        pr->fr += gdata->data.vfr / 150;
        if (pr->fr >= 1.5f) pr->fr = 1.5f;
      }
      pr->eaten_fr += gdata->data.vfr / 47;
      pr->gfr += gdata->data.vfr;
      snake* o = get_snake(gdata, pr->ebid);
      if (pr->eaten_fr >= 1 || !o) {
        tdarray_remove(gdata->data.preys, i);
      } else
        pr->rad = 1 - powf(pr->eaten_fr, 3);
    } else if (pr->fr != 1) {
      pr->fr += gdata->data.vfr / 150;
      if (pr->fr >= 1) {
        pr->fr = 1;
        pr->rad = 1;
      } else {
        pr->rad = .5 * (1 - cosf(PI * pr->fr));
        pr->rad += (.5 * (1 - cosf(PI * pr->rad)) - pr->rad) * .66;
      }
    }
  }

  // update foods:
  int cm1 = (int)tdarray_length(gdata->data.foods) - 1;
  for (int i = cm1; i >= 0; i--) {
    food* fo = gdata->data.foods + i;
    fo->gfr += gdata->data.vfr * fo->gr;
    if (fo->eaten) {
      fo->eaten_fr += gdata->data.vfr / 41;
      snake* o = get_snake(gdata, fo->ebid);
      if (fo->eaten_fr >= 1 || !o) {
        if (i != cm1) {
          gdata->data.foods[i] = gdata->data.foods[cm1];
        }
        tdarray_pop(gdata->data.foods);
        cm1--;
      } else {
        float k = fo->eaten_fr * fo->eaten_fr;
        fo->rad = fo->lrrad * (1 - fo->eaten_fr * k);
        fo->rx =
            fo->xx + (o->xx + o->fx +
                      cosf(o->ang + o->fa) * (43 - k * 24) * (1 - k) - fo->xx) *
                         k;
        fo->ry =
            fo->yy + (o->yy + o->fy +
                      sinf(o->ang + o->fa) * (43 - k * 24) * (1 - k) - fo->yy) *
                         k;
        if (mode->food_float) {
          fo->rx += cosf(fo->wsp * fo->gfr) * 6 * (1 - fo->eaten_fr);
          fo->ry += sinf(fo->wsp * fo->gfr) * 6 * (1 - fo->eaten_fr);
        }
      }
    } else {
      if (fo->fr != 1) {
        fo->fr += fo->rsp * gdata->data.vfr / 150;
        if (fo->fr >= 1) {
          fo->fr = 1;
          fo->rad = 1;
        } else {
          fo->rad = .5 * (1 - cosf(PI * fo->fr));
          fo->rad += (.5 * (1 - cosf(PI * fo->rad)) - fo->rad) * .66;
        }
        fo->lrrad = fo->rad;
      }
      fo->rx = fo->xx;
      fo->ry = fo->yy;
      if (mode->food_float) {
        fo->rx += mode->food_float * cosf(fo->wsp * fo->gfr) * 6;
        fo->ry += mode->food_float * sinf(fo->wsp * fo->gfr) * 6;
      }
    }
  }

  if (usrs->hotkeys[HOTKEY_BOT].active && gdata->data.follow_view)
    sbot_go(env);
}