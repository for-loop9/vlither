#include "sbot.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "../user.h"

#define ARC_SIZE (M_PI / 8.0f)
#define MAXARC 16
#define SPEED_BASE 5.78f
#define FRONT_ANGLE (M_PI / 2.0f)
#define ENCIRCLE_THRESH 0.5625f
#define ENCIRCLE_ALL_THRESH 0.5625f
#define ENCIRCLE_DIST_MULT 20.0f
#define BORDER_PT_RADIUS 20.0f
#define COLLISION_DELAY 10
#define ACTION_FRAMES 2

#define MAX_BODY_PTS 8192
#define MAX_HULL_PTS 32
#define MAX_INSIDE_PTS 512
#define MAX_COLL_PTS 512

typedef struct {
  float x, y;
} v2;
typedef struct {
  float x, y, r;
} circ;
typedef struct {
  float x, y, a;
} isect;

typedef struct {
  v2 pts[MAX_HULL_PTS];
  int n;
  float minx, maxx, miny, maxy;
} poly_box;

typedef struct {
  v2 pts[MAX_INSIDE_PTS];
  int n;
  float minx, maxx, miny, maxy;
} poly_box_large;

typedef struct {
  float x, y, d2, r;
  int si, type;
} coll_pt;
typedef struct {
  float x, y, ang, d2, r;
  int si, aIndex;
} coll_ang;
typedef struct {
  float x, y, ang, da, d2, sz, score;
} food_ang;
typedef struct {
  float x, y, len;
} body_pt;

typedef struct {
  int stage;
  int delay_frame;
  int default_accel;
  int circle_dir;
  bool enable_encircle;
  bool enable_follow_circle;

  int id;
  float x, y, ang, ca, sa;
  float speed_mult, width, radius;
  int snake_len;

  circ head_circle, side_l, side_r;

  coll_ang coll_angles[MAXARC];
  bool coll_angles_set[MAXARC];
  food_ang food_angles[MAXARC];
  bool food_angles_set[MAXARC];

  food_ang current_food;
  bool has_food;

  coll_pt coll_pts[MAX_COLL_PTS];
  int coll_pts_n;

  v2 goal;

  body_pt bpts[MAX_BODY_PTS];
  int bpts_n;
  float bpts_len;

  float radius_mult;
  float follow_circle_length;
} bot_state;

static bot_state B;

static inline float dist2(float ax, float ay, float bx, float by) {
  float dx = ax - bx, dy = ay - by;
  return dx * dx + dy * dy;
}

static inline float ang_between(float a, float b) {
  float r1 = fmodf(a - b, (float)M_PI);
  float r2 = fmodf(b - a, (float)M_PI);
  return r1 < r2 ? -r1 : r2;
}

static inline int ang_index(float angle) {
  float a = angle < 0 ? angle + PI2 : angle;
  int i = (int)roundf(a * (1.0f / ARC_SIZE));
  return i == MAXARC ? 0 : i;
}

static inline float snake_width(float sc) { return roundf(sc * 29.0f); }

static inline v2 unit_vec(v2 v) {
  float l = sqrtf(v.x * v.x + v.y * v.y);
  return l > 0 ? (v2){v.x / l, v.y / l} : (v2){0, 0};
}

static inline float cross2(v2 o, v2 a, v2 b) {
  return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

static int hull_cmp(const void* a, const void* b) {
  v2 pa = *(v2*)a, pb = *(v2*)b;
  if (pa.x != pb.x) return pa.x < pb.x ? -1 : 1;
  return pa.y < pb.y ? -1 : pa.y > pb.y ? 1 : 0;
}

static int convex_hull(v2* pts, int n, v2* out) {
  if (n < 2) {
    for (int i = 0; i < n; i++) out[i] = pts[i];
    return n;
  }
  qsort(pts, n, sizeof(v2), hull_cmp);

  v2 stk[MAX_HULL_PTS * 2];
  int k = 0;
  /* lower hull */
  for (int i = 0; i < n; i++) {
    while (k >= 2 && cross2(stk[k - 2], stk[k - 1], pts[i]) <= 0) k--;
    stk[k++] = pts[i];
  }
  /* upper hull */
  int lo = k + 1;
  for (int i = n - 2; i >= 0; i--) {
    while (k >= lo && cross2(stk[k - 2], stk[k - 1], pts[i]) <= 0) k--;
    stk[k++] = pts[i];
  }
  k--; /* remove last point (duplicate of first) */
  for (int i = 0; i < k; i++) out[i] = stk[i];
  return k;
}

static bool point_in_poly(v2 p, const poly_box* pb) {
  if (p.x < pb->minx || p.x > pb->maxx || p.y < pb->miny || p.y > pb->maxy)
    return false;
  bool c = false;
  int l = pb->n;
  for (int i = 0, j = l - 1; i < l; j = i++) {
    if ((pb->pts[i].y > p.y) != (pb->pts[j].y > p.y) &&
        p.x < (pb->pts[j].x - pb->pts[i].x) * (p.y - pb->pts[i].y) /
                      (pb->pts[j].y - pb->pts[i].y) +
                  pb->pts[i].x)
      c = !c;
  }
  return c;
}

static bool point_in_poly_large(v2 p, const poly_box_large* pb) {
  if (p.x < pb->minx || p.x > pb->maxx || p.y < pb->miny || p.y > pb->maxy)
    return false;
  bool c = false;
  int l = pb->n;
  for (int i = 0, j = l - 1; i < l; j = i++) {
    if ((pb->pts[i].y > p.y) != (pb->pts[j].y > p.y) &&
        p.x < (pb->pts[j].x - pb->pts[i].x) * (p.y - pb->pts[i].y) /
                      (pb->pts[j].y - pb->pts[i].y) +
                  pb->pts[i].x)
      c = !c;
  }
  return c;
}

static void poly_box_aabb(poly_box* pb) {
  pb->minx = pb->maxx = pb->pts[0].x;
  pb->miny = pb->maxy = pb->pts[0].y;
  for (int i = 1; i < pb->n; i++) {
    if (pb->pts[i].x < pb->minx) pb->minx = pb->pts[i].x;
    if (pb->pts[i].x > pb->maxx) pb->maxx = pb->pts[i].x;
    if (pb->pts[i].y < pb->miny) pb->miny = pb->pts[i].y;
    if (pb->pts[i].y > pb->maxy) pb->maxy = pb->pts[i].y;
  }
}

static void poly_box_large_aabb(poly_box_large* pb) {
  pb->minx = pb->maxx = pb->pts[0].x;
  pb->miny = pb->maxy = pb->pts[0].y;
  for (int i = 1; i < pb->n; i++) {
    if (pb->pts[i].x < pb->minx) pb->minx = pb->pts[i].x;
    if (pb->pts[i].x > pb->maxx) pb->maxx = pb->pts[i].x;
    if (pb->pts[i].y < pb->miny) pb->miny = pb->pts[i].y;
    if (pb->pts[i].y > pb->maxy) pb->maxy = pb->pts[i].y;
  }
}

static bool circle_intersect(circ c1, circ c2, isect* out) {
  float br = c1.r + c2.r;
  if (c1.x + br <= c2.x || c2.x + br <= c1.x) return false;
  if (c1.y + br <= c2.y || c2.y + br <= c1.y) return false;
  if (dist2(c1.x, c1.y, c2.x, c2.y) >= br * br) return false;
  if (out) {
    out->x = (c1.x * c2.r + c2.x * c1.r) / br;
    out->y = (c1.y * c2.r + c2.y * c1.r) / br;
    out->a = atan2f(out->y - B.y, out->x - B.x);
  }
  return true;
}

static v2 heading_abs(float angle) {
  return (v2){roundf(B.x + 500.0f * cosf(angle)),
              roundf(B.y + 500.0f * sinf(angle))};
}

static v2 heading_rel(float angle) {
  float hx = B.x + 500.0f * B.ca, hy = B.y + 500.0f * B.sa;
  float c = cosf(-angle), s = sinf(-angle);
  return (v2){roundf(c * (hx - B.x) - s * (hy - B.y) + B.x),
              roundf(s * (hx - B.x) + c * (hy - B.y) + B.y)};
}

static inline bool is_left(v2 start, v2 end, v2 p) {
  return (end.x - start.x) * (p.y - start.y) -
             (end.y - start.y) * (p.x - start.x) >
         0;
}

static void add_coll_angle(coll_pt* sp) {
  float ang = atan2f(roundf(sp->y - B.y), roundf(sp->x - B.x));
  int ai = ang_index(ang);
  float adj = sqrtf(sp->d2) - sp->r;
  float ad2 = roundf(adj * adj);
  if (!B.coll_angles_set[ai] || B.coll_angles[ai].d2 > ad2) {
    B.coll_angles[ai] =
        (coll_ang){roundf(sp->x), roundf(sp->y), ang, ad2, sp->r, sp->si, ai};
    B.coll_angles_set[ai] = true;
  }
}

static void add_food_angle(float fx, float fy, float fd2, float fsz) {
  float ang = atan2f(roundf(fy - B.y), roundf(fx - B.x));
  int ai = ang_index(ang);
  if (B.coll_angles_set[ai]) {
    float cd = sqrtf(B.coll_angles[ai].d2);
    float fd = sqrtf(fd2);
    if (cd <= fd + (B.radius * B.radius_mult * B.speed_mult) / 2.0f) return;
  }
  float score = fsz * fsz / fd2;
  if (!B.food_angles_set[ai]) {
    B.food_angles[ai] =
        (food_ang){roundf(fx), roundf(fy), ang,  fabsf(ang_between(ang, B.ang)),
                   fd2,        fsz,        score};
    B.food_angles_set[ai] = true;
  } else {
    food_ang* fa = &B.food_angles[ai];
    fa->sz += roundf(fsz);
    fa->score += score;
    if (fa->d2 > fd2) {
      fa->x = roundf(fx);
      fa->y = roundf(fy);
      fa->d2 = fd2;
    }
  }
}

static int coll_pt_cmp(const void* a, const void* b) {
  float da = ((coll_pt*)a)->d2, db = ((coll_pt*)b)->d2;
  return (da > db) - (da < db);
}

static void get_collision_points(game_data* gdata) {
  B.coll_pts_n = 0;
  memset(B.coll_angles_set, 0, sizeof(B.coll_angles_set));

  float far_d2 = (B.width * 50.0f) * (B.width * 50.0f);
  int ns = tdarray_length(gdata->data.snakes);

  for (int i = 0; i < ns; i++) {
    snake* s = gdata->data.snakes + i;
    if (s->id == B.id) continue;

    float sr = snake_width(s->sc) / 2.0f;
    float smul = fminf(1.0f, s->sp / SPEED_BASE - 1.0f);
    float srm = sr * smul * B.radius_mult;

    float hx = s->xx + cosf(s->ang) * srm / 2.0f;
    float hy = s->yy + sinf(s->ang) * srm / 2.0f;
    float hd2 = dist2(B.x, B.y, hx, hy);

    coll_pt head = {hx, hy, hd2, B.head_circle.r, i, 0};
    add_coll_angle(&head);
    if (B.coll_pts_n < MAX_COLL_PTS) B.coll_pts[B.coll_pts_n++] = head;

    int pn = tdarray_length(s->pts);
    for (int j = 0; j < pn; j++) {
      body_part* po = s->pts + j;
      if (po->dying) continue;
      float pd2 = dist2(B.x, B.y, po->xx, po->yy);
      if (pd2 > far_d2) continue;
      coll_pt bp = {po->xx, po->yy, pd2, sr, i, 1};
      add_coll_angle(&bp);
      float tr = B.head_circle.r + sr;
      if (pd2 <= tr * tr && B.coll_pts_n < MAX_COLL_PTS)
        B.coll_pts[B.coll_pts_n++] = bp;
    }
  }

  float view_ang = atan2f(gdata->data.view_yy - gdata->data.grd,
                          gdata->data.view_xx - gdata->data.grd);
  float dist_to_ctr = sqrtf(dist2(B.x, B.y, gdata->data.grd, gdata->data.grd));
  bool near_wall = (gdata->data.flux_grd - dist_to_ctr) < 1000.0f;

  if (near_wall) {
    float bpw = BORDER_PT_RADIUS * 2.0f;
    float brad = gdata->data.flux_grd + BORDER_PT_RADIUS;
    for (int k = -3; k <= 3; k++) {
      float wa = view_ang + (k * bpw) / gdata->data.flux_grd;
      float wx = gdata->data.grd + brad * cosf(wa);
      float wy = gdata->data.grd + brad * sinf(wa);
      float wd2 = dist2(B.x, B.y, wx, wy);
      coll_pt wp = {wx, wy, wd2, BORDER_PT_RADIUS, -1, 2};
      if (B.coll_pts_n < MAX_COLL_PTS) B.coll_pts[B.coll_pts_n++] = wp;
      add_coll_angle(&wp);
    }
  }

  qsort(B.coll_pts, B.coll_pts_n, sizeof(coll_pt), coll_pt_cmp);
}

static bool in_front(float px, float py) {
  float ang = atan2f(roundf(py - B.y), roundf(px - B.x));
  return fabsf(ang_between(ang, B.ang)) < FRONT_ANGLE;
}

static void avoid_point(isect* ip) {
  v2 head = {B.x, B.y};
  v2 end = {B.x + 2000.0f * B.ca, B.y + 2000.0f * B.sa};
  v2 pt = {ip->x, ip->y};
  float new_ang =
      is_left(head, end, pt) ? ip->a - (float)M_PI : ip->a + (float)M_PI;
  B.goal = heading_abs(new_ang);
}

static void heading_best_angle(void) {
  typedef struct {
    int s, e, sz;
  } oa;
  oa opens[MAXARC];
  int no = 0, open_start = -1;
  int best_ai = -1;
  float best_d = -1.0f;

  for (int i = 0; i < MAXARC; i++) {
    if (!B.coll_angles_set[i]) {
      if (open_start < 0) open_start = i;
    } else {
      float d = B.coll_angles[i].d2;
      if (best_ai < 0 || (d > best_d && d != 0.0f)) {
        best_d = d;
        best_ai = i;
      }
      if (open_start >= 0) {
        opens[no++] = (oa){open_start, i - 1, i - 1 - open_start};
        open_start = -1;
      }
    }
  }
  if (open_start >= 0) {
    if (no > 0) {
      opens[0].s = open_start;
      opens[0].sz = opens[0].e - open_start;
      if (opens[0].sz < 0) opens[0].sz += MAXARC;
    } else {
      opens[no++] = (oa){open_start, open_start, 0};
    }
  }

  if (no > 0) {
    for (int a = 0; a < no - 1; a++)
      for (int b = a + 1; b < no; b++)
        if (opens[b].sz > opens[a].sz) {
          oa t = opens[a];
          opens[a] = opens[b];
          opens[b] = t;
        }
    float mid = (opens[0].e - opens[0].sz / 2.0f) * ARC_SIZE;
    B.goal = heading_abs(mid);
  } else if (best_ai >= 0) {
    B.goal = heading_abs(best_ai * ARC_SIZE);
  }
}

static bool check_collision(game_data* gdata) {
  get_collision_points(gdata);
  if (B.coll_pts_n == 0) return false;
  for (int i = 0; i < B.coll_pts_n; i++) {
    coll_pt* cp = B.coll_pts + i;
    circ col = {cp->x, cp->y, cp->r};
    isect ip;
    if (circle_intersect(B.head_circle, col, &ip) && in_front(ip.x, ip.y)) {
      if (cp->type == 0) {
        snake* s = gdata->data.snakes + cp->si;
        gdata->bot.output.accel = s->sp > 10.0f;
      } else {
        gdata->bot.output.accel = (bool)B.default_accel;
      }
      avoid_point(&ip);
      return true;
    }
  }
  gdata->bot.output.accel = (bool)B.default_accel;
  return false;
}

static bool check_encircle(game_data* gdata) {
  if (!B.enable_encircle) return false;
  int en[512];
  memset(en, 0, sizeof(en));
  int high = 0, high_si = 0, en_all = 0;
  for (int i = 0; i < MAXARC; i++) {
    if (!B.coll_angles_set[i]) continue;
    coll_ang* ca = &B.coll_angles[i];
    if (ca->si >= 0 && ca->si < 512) {
      if (++en[ca->si] > high) {
        high = en[ca->si];
        high_si = ca->si;
      }
    }
    float ed = B.radius * ENCIRCLE_DIST_MULT;
    if (ca->d2 < ed * ed) en_all++;
  }
  if (high > (int)(MAXARC * ENCIRCLE_THRESH)) {
    heading_best_angle();
    int ns = tdarray_length(gdata->data.snakes);
    if (high != MAXARC && high_si < ns)
      gdata->bot.output.accel = gdata->data.snakes[high_si].sp > 10.0f;
    else
      gdata->bot.output.accel = (bool)B.default_accel;
    return true;
  }
  if (en_all > (int)(MAXARC * ENCIRCLE_ALL_THRESH)) {
    heading_best_angle();
    gdata->bot.output.accel = (bool)B.default_accel;
    return true;
  }
  gdata->bot.output.accel = (bool)B.default_accel;
  return false;
}

static void populate_pts(game_data* gdata) {
  B.bpts_n = 0;
  B.bpts_len = 0.0f;
  int ns = tdarray_length(gdata->data.snakes);
  snake* me = NULL;
  for (int i = 0; i < ns; i++)
    if (gdata->data.snakes[i].id == B.id) {
      me = gdata->data.snakes + i;
      break;
    }
  if (!me) return;

  B.bpts[B.bpts_n++] = (body_pt){B.x, B.y, 0.0f};
  float lx = B.x, ly = B.y, l = 0.0f;
  int pn = tdarray_length(me->pts);
  /* Iterate oldest→newest (tail→head direction matches JS pts iteration) */
  for (int p = pn - 1; p >= 0 && B.bpts_n < MAX_BODY_PTS; p--) {
    body_part* po = me->pts + p;
    if (po->dying) continue;
    l += sqrtf(dist2(lx, ly, po->xx, po->yy));
    B.bpts[B.bpts_n++] = (body_pt){po->xx, po->yy, l};
    lx = po->xx;
    ly = po->yy;
  }
  B.bpts_len = l;
}

static v2 smooth_point(float t) {
  if (t <= 0.0f) return (v2){B.bpts[0].x, B.bpts[0].y};
  if (t >= B.bpts_len) {
    int e = B.bpts_n - 1;
    return (v2){B.bpts[e].x, B.bpts[e].y};
  }
  int lo = 0, hi = B.bpts_n - 1;
  while (hi - lo > 1) {
    int m = (lo + hi) / 2;
    if (t > B.bpts[m].len)
      lo = m;
    else
      hi = m;
  }
  float wp = B.bpts[hi].len - t, wq = t - B.bpts[lo].len, w = wp + wq;
  return (v2){(wp * B.bpts[lo].x + wq * B.bpts[hi].x) / w,
              (wp * B.bpts[lo].y + wq * B.bpts[hi].y) / w};
}

static float closest_body_point(void) {
  if (B.bpts_n < 3) return B.bpts_len;
  int start_n = 0;
  float start_d2 = 0.0f;
  for (;;) {
    float prev = start_d2;
    start_n++;
    if (start_n >= B.bpts_n) break;
    start_d2 = dist2(B.x, B.y, B.bpts[start_n].x, B.bpts[start_n].y);
    if (start_d2 < prev || start_n == B.bpts_n - 1) break;
  }
  if (start_n >= B.bpts_n || start_n <= 1) return B.bpts_len;

  int min_n = start_n;
  float min_d2 = start_d2;
  for (int n = min_n + 1; n < B.bpts_n; n++) {
    float d2 = dist2(B.x, B.y, B.bpts[n].x, B.bpts[n].y);
    if (d2 < min_d2) {
      min_n = n;
      min_d2 = d2;
    }
  }

  int next_n;
  float next_d2;
  if (min_n == B.bpts_n - 1) {
    next_n = min_n - 1;
    next_d2 = dist2(B.x, B.y, B.bpts[next_n].x, B.bpts[next_n].y);
  } else {
    float dm = dist2(B.x, B.y, B.bpts[min_n - 1].x, B.bpts[min_n - 1].y);
    float dp = dist2(B.x, B.y, B.bpts[min_n + 1].x, B.bpts[min_n + 1].y);
    if (dm < dp) {
      next_n = min_n - 1;
      next_d2 = dm;
    } else {
      next_n = min_n + 1;
      next_d2 = dp;
    }
  }

  float t2 = B.bpts[min_n].len - B.bpts[next_n].len;
  t2 *= t2;
  if (t2 == 0.0f) return B.bpts[min_n].len;
  float min_w = t2 - (min_d2 - next_d2);
  float next_w = t2 + (min_d2 - next_d2);
  return (B.bpts[min_n].len * min_w + B.bpts[next_n].len * next_w) /
         (2.0f * t2);
}

static void determine_circle_dir(void) {
  float cx = 0, cy = 0;
  for (int i = 0; i < B.bpts_n; i++) {
    cx += B.bpts[i].x;
    cy += B.bpts[i].y;
  }
  cx /= B.bpts_n;
  cy /= B.bpts_n;
  float dx = B.x - cx, dy = B.y - cy;
  B.circle_dir = (-dy * B.ca + dx * B.sa > 0.0f) ? -1 : +1;
}

static void body_danger_zone(float offset, v2 target_pt,
                             v2 target_norm,   /* closePointNormal */
                             float close_dist, /* closePointDist   */
                             v2 past_target, v2 close_pt, poly_box* out) {
  int o = B.circle_dir;
  v2 raw[10];
  raw[0] = (v2){B.x - o * offset * B.sa, B.y + o * offset * B.ca};
  raw[1] = (v2){B.x + B.width * B.ca + offset * (B.ca - o * B.sa),
                B.y + B.width * B.sa + offset * (B.sa + o * B.ca)};
  raw[2] = (v2){B.x + 1.75f * B.width * B.ca + o * 0.3f * B.width * B.sa +
                    offset * (B.ca - o * B.sa),
                B.y + 1.75f * B.width * B.sa - o * 0.3f * B.width * B.ca +
                    offset * (B.sa + o * B.ca)};
  raw[3] = (v2){B.x + 2.5f * B.width * B.ca + o * 0.7f * B.width * B.sa +
                    offset * (B.ca - o * B.sa),
                B.y + 2.5f * B.width * B.sa - o * 0.7f * B.width * B.ca +
                    offset * (B.sa + o * B.ca)};
  raw[4] = (v2){
      B.x + 3.0f * B.width * B.ca + o * 1.2f * B.width * B.sa + offset * B.ca,
      B.y + 3.0f * B.width * B.sa - o * 1.2f * B.width * B.ca + offset * B.sa};
  float cpd_half = 0.5f * fmaxf(close_dist, 0.0f);
  raw[5] = (v2){target_pt.x + target_norm.x * (offset + cpd_half),
                target_pt.y + target_norm.y * (offset + cpd_half)};
  raw[6] = (v2){past_target.x + target_norm.x * offset,
                past_target.y + target_norm.y * offset};
  raw[7] = past_target;
  raw[8] = target_pt;
  raw[9] = close_pt;

  v2 scratch[10];
  memcpy(scratch, raw, sizeof(raw));
  out->n = convex_hull(scratch, 10, out->pts);
  poly_box_aabb(out);
}

static void construct_inside_polygon(float close_t, poly_box_large* out) {
  float start_t = 5.0f * B.width;
  float end_t = close_t + 5.0f * B.width;

  out->n = 0;
  out->pts[out->n++] = smooth_point(end_t);
  out->pts[out->n++] = smooth_point(start_t);
  for (float t = start_t; t < end_t && out->n < MAX_INSIDE_PTS; t += B.width)
    out->pts[out->n++] = smooth_point(t);

  poly_box_large_aabb(out);
}

static void follow_circle_self(game_data* gdata) {
  populate_pts(gdata);
  determine_circle_dir();
  int o = B.circle_dir;

  if (B.bpts_len < 9.0f * B.width) return;

  float close_t = closest_body_point();
  v2 close_pt = smooth_point(close_t);
  v2 close_next = smooth_point(close_t - B.width);
  v2 close_tang =
      unit_vec((v2){close_next.x - close_pt.x, close_next.y - close_pt.y});
  v2 close_norm = (v2){-o * close_tang.y, o * close_tang.x};

  float cur_course = asinf(
      fmaxf(-1.0f, fminf(1.0f, B.ca * close_norm.x + B.sa * close_norm.y)));

  float close_dist =
      (B.x - close_pt.x) * close_norm.x + (B.y - close_pt.y) * close_norm.y;

  poly_box_large inside_poly;
  construct_inside_polygon(close_t, &inside_poly);

  float target_t = close_t;
  float target_far = 0.0f;
  float step = B.width / 64.0f;
  for (float h = close_dist, a = cur_course; h >= 0.125f * B.width;) {
    target_t -= step;
    target_far += step * cosf(a);
    h += step * sinf(a);
    a = fmaxf(-(float)M_PI / 4.0f, a - step / B.width);
  }
  v2 target_pt = smooth_point(target_t);
  v2 past_target = smooth_point(target_t - 3.0f * B.width);

  float offset_incr = 0.0625f * B.width;

  float enemy_body_od = 0.25f * B.width;
  float enemy_head_d2 = 64.0f * 64.0f * B.width * B.width;

  int ns = tdarray_length(gdata->data.snakes);
  for (int i = 0; i < ns; i++) {
    snake* sk = gdata->data.snakes + i;
    if (sk->id == B.id) continue;

    float ew = snake_width(sk->sc);
    v2 eh = {sk->xx, sk->yy};
    v2 ea = {eh.x + cosf(sk->ang) * B.width, eh.y + sinf(sk->ang) * B.width};

    if (!point_in_poly_large(eh, &inside_poly)) {
      float d1 = dist2(eh.x, eh.y, target_pt.x, target_pt.y);
      float d2_ = dist2(ea.x, ea.y, target_pt.x, target_pt.y);
      float dm = d1 < d2_ ? d1 : d2_;
      if (dm < enemy_head_d2) enemy_head_d2 = dm;
    }

    bool offset_set = false;
    float offset = 0.0f;
    poly_box cpolbody;
    float body_offset = 0.5f * (B.width + ew);

    int pn = tdarray_length(sk->pts);
    for (int j = 0; j < pn; j++) {
      body_part* po = sk->pts + j;
      if (po->dying) continue;
      v2 pt = {po->xx, po->yy};

      while (!offset_set ||
             (enemy_body_od >= -B.width && point_in_poly(pt, &cpolbody))) {
        if (!offset_set) {
          offset_set = true;
        } else {
          enemy_body_od -= offset_incr;
        }
        offset = body_offset + enemy_body_od;
        body_danger_zone(offset, target_pt, close_norm, close_dist, past_target,
                         close_pt, &cpolbody);
      }
    }
  }

  float dist_to_ctr = sqrtf(dist2(B.x, B.y, gdata->data.grd, gdata->data.grd));
  bool near_wall = (gdata->data.flux_grd - dist_to_ctr) < 1000.0f;
  float wall_od = 0.0f;

  if (near_wall) {
    float view_ang = atan2f(gdata->data.view_yy - gdata->data.grd,
                            gdata->data.view_xx - gdata->data.grd);
    float bpw = BORDER_PT_RADIUS * 2.0f;
    float brad = gdata->data.flux_grd + BORDER_PT_RADIUS;
    float wall_off = 0.5f * (B.width + BORDER_PT_RADIUS);
    bool offset_set = false;
    float offset = 0.0f;
    poly_box cpolbody;

    for (int k = -2; k <= 2; k++) {
      float wa = view_ang + (k * bpw) / gdata->data.flux_grd;
      v2 wp = {gdata->data.grd + brad * cosf(wa),
               gdata->data.grd + brad * sinf(wa)};

      while (!offset_set ||
             (wall_od >= -B.width && point_in_poly(wp, &cpolbody))) {
        if (!offset_set) {
          offset_set = true;
        } else {
          wall_od -= offset_incr;
        }
        offset = wall_off + wall_od;
        body_danger_zone(offset, target_pt, close_norm, close_dist, past_target,
                         close_pt, &cpolbody);
      }
    }
  }

  float enemy_head_dist = sqrtf(enemy_head_d2);

  float target_course = cur_course + 0.25f;

  float head_prox = -1.0f - (2.0f * target_far - enemy_head_dist) / B.width;
  if (head_prox > 0)
    head_prox = 0.125f * head_prox * head_prox;
  else
    head_prox = -0.500f * head_prox * head_prox;
  target_course = fminf(target_course, head_prox);

  float adj_body = (enemy_body_od - 0.0625f * B.width) / B.width;
  target_course = fminf(target_course, target_course + adj_body);

  if (near_wall) {
    float adj_wall = (wall_od - 0.0625f * B.width) / B.width;
    target_course = fminf(target_course, target_course + adj_wall);
  }

  float tail_behind = B.bpts_len - close_t;
  float allow_tail = B.width * 2.0f;
  target_course =
      fminf(target_course,
            (tail_behind - allow_tail + (B.width - close_dist)) / B.width);

  target_course =
      fminf(target_course, (-0.5f * (close_dist - 4.0f * B.width)) / B.width);

  target_course = fmaxf(target_course, (-0.75f * close_dist) / B.width);
  target_course = fminf(target_course, 1.0f);

  v2 goal_dir = {close_tang.x * cosf(target_course) -
                     o * close_tang.y * sinf(target_course),
                 close_tang.y * cosf(target_course) +
                     o * close_tang.x * sinf(target_course)};
  v2 goal = {B.x + goal_dir.x * 4.0f * B.width,
             B.y + goal_dir.y * 4.0f * B.width};

  /* smooth update */
  if (fabsf(goal.x - B.goal.x) < 1000.0f &&
      fabsf(goal.y - B.goal.y) < 1000.0f) {
    B.goal.x = roundf(goal.x * 0.25f + B.goal.x * 0.75f);
    B.goal.y = roundf(goal.y * 0.25f + B.goal.y * 0.75f);
  } else {
    B.goal = (v2){roundf(goal.x), roundf(goal.y)};
  }
}

static void to_circle(game_data* gdata) {
  if (!B.enable_follow_circle) return;
  int ns = tdarray_length(gdata->data.snakes);
  snake* me = NULL;
  for (int i = 0; i < ns; i++)
    if (gdata->data.snakes[i].id == B.id) {
      me = gdata->data.snakes + i;
      break;
    }
  if (!me) return;

  int pn = tdarray_length(me->pts), checked = 0;
  for (int i = 0; i < pn && checked < 20; i++) {
    body_part* po = me->pts + i;
    if (po->dying) continue;
    checked++;
    circ tc = {po->xx, po->yy, B.radius};
    if (circle_intersect(B.head_circle, tc, NULL)) {
      B.stage = 2;
      return;
    }
  }
  gdata->bot.output.accel = (bool)B.default_accel;
  B.goal = heading_rel((B.circle_dir * (float)M_PI) / 32.0f);
}

static void compute_food_goal(game_data* gdata) {
  memset(B.food_angles_set, 0, sizeof(B.food_angles_set));
  int fn = tdarray_length(gdata->data.foods);
  for (int i = 0; i < fn; i++) {
    food* fo = gdata->data.foods + i;
    if (fo->eaten) continue;
    float fd2 = dist2(B.x, B.y, fo->xx, fo->yy);
    circ fc = {fo->xx, fo->yy, 2.0f};
    if (circle_intersect(fc, B.side_l, NULL)) continue;
    if (circle_intersect(fc, B.side_r, NULL)) continue;
    add_food_angle(fo->xx, fo->yy, fd2, fo->sz);
  }
  B.has_food = false;
  float best = -1.0f;
  for (int i = 0; i < MAXARC; i++) {
    if (!B.food_angles_set[i] || B.food_angles[i].sz <= 0.0f) continue;
    if (B.food_angles[i].score > best) {
      best = B.food_angles[i].score;
      B.current_food = B.food_angles[i];
      B.has_food = true;
    }
  }
}

static void delay_action(game_data* gdata) {
  if (B.delay_frame == -1) return;
  if (B.delay_frame > 0) {
    B.delay_frame--;
    return;
  }

  int ns = tdarray_length(gdata->data.snakes);
  bool playing = false;
  for (int i = 0; i < ns; i++)
    if (gdata->data.snakes[i].id == B.id && !gdata->data.snakes[i].dead) {
      playing = true;
      break;
    }

  if (playing) {
    if (B.stage == 0) {
      compute_food_goal(gdata);
      B.goal = B.has_food ? (v2){B.current_food.x, B.current_food.y}
                          : (v2){gdata->data.grd, gdata->data.grd};
    } else if (B.stage == 1) {
      to_circle(gdata);
    }
  }
  B.delay_frame = -1;
}

static int snake_score(game_data* gdata, snake* sk) {
  if (sk->sct < 0 || sk->fam < 0 || sk->rsc < 0) return 0;
  int sct = sk->sct + sk->rsc;
  return (int)(15.0f * (gdata->data.fpsls[sct] +
                        sk->fam / gdata->data.fmlts[sct] - 1) -
               5);
}

static void every(game_data* gdata) {
  int ns = tdarray_length(gdata->data.snakes);
  if (ns == 0) return;
  snake* me = NULL;
  for (int i = 0; i < ns; i++)
    if (gdata->data.snakes[i].id == B.id) {
      me = gdata->data.snakes + i;
      break;
    }
  if (!me) {
    me = gdata->data.snakes + (ns - 1);
    B.id = me->id;
  }

  B.x = me->xx;
  B.y = me->yy;
  B.ang = me->ang;
  B.ca = cosf(B.ang);
  B.sa = sinf(B.ang);
  B.speed_mult = me->sp / SPEED_BASE;
  B.width = snake_width(me->sc);
  B.radius = B.width / 2.0f;
  B.snake_len = snake_score(gdata, me);

  float spf = fminf(1.0f, B.speed_mult - 1.0f) * B.radius_mult;
  B.head_circle = (circ){B.x + (B.ca * spf / 2.0f) * B.radius,
                         B.y + (B.sa * spf / 2.0f) * B.radius,
                         (B.radius_mult / 2.0f) * B.radius};
  B.side_r = (circ){B.x - (B.y + B.sa * B.width - B.y),
                    B.y + (B.x + B.ca * B.width - B.x), B.width * B.speed_mult};
  B.side_l = (circ){B.x + (B.y + B.sa * B.width - B.y),
                    B.y - (B.x + B.ca * B.width - B.x), B.width * B.speed_mult};
}

void sbot_init(tenv* env) {
  tuser_data* usr = env->usr;
  game_data* gdata = &usr->gdata;
  memset(&B, 0, sizeof(B));
  B.stage = 0;
  B.delay_frame = -1;
  B.default_accel = 0;
  B.circle_dir = +1;
  B.enable_encircle = true;
  B.enable_follow_circle = true;
  gdata->bot.output.accel = false;
  gdata->bot.output.xm = 0;
  gdata->bot.output.ym = 0;
}

void sbot_go(tenv* env) {
  tuser_data* usr = env->usr;
  user_settings* usrs = &usr->usrs;
  game_data* gdata = &usr->gdata;
  sbot* bot = &gdata->bot;
  B.radius_mult = usrs->bot_radius_mult;
  B.follow_circle_length = usrs->bot_follow_circle_score;

  if (tdarray_length(gdata->data.snakes) == 0) return;

  every(gdata);

  if (B.snake_len < B.follow_circle_length) B.stage = 0;
  if (B.has_food && B.stage != 0) B.has_food = false;

  if (B.stage == 2) {
    bot->output.accel = (bool)B.default_accel;
    follow_circle_self(gdata);
  } else if (check_collision(gdata) || check_encircle(gdata)) {
    if (B.delay_frame != -1) B.delay_frame = COLLISION_DELAY;
  } else {
    if (B.snake_len > B.follow_circle_length) B.stage = 1;
    if (B.delay_frame == -1) B.delay_frame = ACTION_FRAMES;
    bot->output.accel = (bool)B.default_accel;
  }

  delay_action(gdata);

  bot->output.xm = (B.goal.x - gdata->data.view_xx) * gdata->data.gsc;
  bot->output.ym = (B.goal.y - gdata->data.view_yy) * gdata->data.gsc;
}

void sbot_destroy(tenv* env) { (void)env; }