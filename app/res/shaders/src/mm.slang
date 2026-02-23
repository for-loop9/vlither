import modules.common;

struct vs_input {
  [[vk::location(0)]] float2 pos;
};

struct vs_output {
  float4 clipspace : SV_Position;
  [[vk::location(0)]] float2 uv;
  [[vk::location(1)]] float2 uvp;
  [[vk::location(2)]] float2 luvp;
};

[[vk::binding(0, 0)]] ConstantBuffer<Global> global;
[[vk::binding(3, 0)]] Sampler2D minimap_tex;

static const float SHADOW = 0.9;

vs_output vs_main(vs_input input) {
  vs_output output;

  float2 p = (input.pos - 0.5) * (global.minimap_circ.z + EDGE_SOFTNESS);
  float2 r = p + 0.5 * (global.minimap_circ.z + EDGE_SOFTNESS * 0.5);

  float2 hv = global.viewport * 0.5;
  float2 wp = r + global.minimap_circ.xy;
  float2 o = (wp - hv) / hv;

  output.clipspace = float4(o, 1);
  output.uv = input.pos;

  output.uvp = ((global.view - global.grd) / global.bd_radius + 1.0f) * 0.5f;
  output.uvp = output.uvp * 2 - 1;
  output.uvp *= SHADOW;
  output.uvp = output.uvp * 0.5 + 0.5;
  
  output.luvp = ((global.lview - global.grd) / global.bd_radius + 1.0f) * 0.5f;
  output.luvp = output.luvp * 2 - 1;
  output.luvp *= SHADOW;
  output.luvp = output.luvp * 0.5 + 0.5;

  return output;
}

float4 fs_main(vs_output input) {
  float block_size = global.minimap_data_size / global.max_minimap_size;
  float zoom = 1 / SHADOW;
  float2 center = float2(block_size * 0.5, block_size * 0.5);
  float2 mpuv = input.uv * block_size;
  mpuv = (mpuv - center) * zoom + center;

  float d = length(input.uv - 0.5) * 2;
  float dd = length(input.uv - input.uvp) * 2;
  float dd2 = length(input.uv - input.luvp) * 2;
  float p = length(float2(ddx(d), ddy(d)));
  float pp = length(float2(ddx(dd), ddy(dd)));
  float pp2 = length(float2(ddx(dd2), ddy(dd2)));

  float a = smoothstep(0.0, EDGE_SOFTNESS, (0.045 - dd) / pp);
  float ap = max(0, smoothstep(0.0, EDGE_SOFTNESS, (0.035 - dd) / pp));
  float la = smoothstep(0.0, EDGE_SOFTNESS, (0.02 - dd2) / pp2);
  float lar = (smoothstep(0.0, EDGE_SOFTNESS, (0.062 - dd2) / pp2) - smoothstep(0.0, EDGE_SOFTNESS, (0.055 - dd2) / pp2));

  float b = max(0, smoothstep(0.0, EDGE_SOFTNESS, (SHADOW - d) / p) - (a + la));
  float s = max(0, smoothstep(0.0, 1.25 * (1 - SHADOW) * global.minimap_circ.z, (1 - d) / p) - (b + a + la + lar));
  float c = max(0, minimap_tex.Sample(mpuv).r - (a + la + lar));
  b -= c;

  float4 cc = float4(0.4, 0.4, 0.4, 0.7) * c;
  float4 bc = float4(0.1, 0.1, 0.1, 0.6) * b;
  float4 dc = float4(0, 0, 0, 1) * a;
  float4 pdc = float4(1, 0.5, 0.5, 1) * ap;
  float4 ldc = float4(0.5, 0.8, 0.5, 1) * (la + lar);
  float4 sc = float4(lerp(global.bd_color, float3(1, 1, 1), 0.25), 1.75) * s;

  return (sc + bc + cc + dc + pdc + ldc) * float4(1, 1, 1, global.minimap_opacity);
}