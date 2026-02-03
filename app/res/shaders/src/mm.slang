import modules.common;

struct vs_input {
  [[vk::location(0)]] float2 pos;
};

struct vs_output {
  float4 clipspace : SV_Position;
  [[vk::location(0)]] float2 uv;
  [[vk::location(1)]] float2 uvp;
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
  float p = length(float2(ddx(d), ddy(d)));
  float pp = length(float2(ddx(dd), ddy(dd)));

  float a = smoothstep(0.0, EDGE_SOFTNESS, (0.035 - dd) / pp);
  float a2 = smoothstep(0.0, EDGE_SOFTNESS, (0.04 - dd) / pp);
  float b = max(0, smoothstep(0.0, EDGE_SOFTNESS, (SHADOW - d) / p) - a);
  float s = max(0, smoothstep(0.0, 1.25 * (1 - SHADOW) * global.minimap_circ.z, (1 - d) / p) - (b + a));
  float c = max(0, minimap_tex.Sample(mpuv).r - a2);
  b -= c;

  float4 cc = float4(0.4, 0.4, 0.4, 0.7) * c;
  float4 bc = float4(0.1, 0.1, 0.1, 0.6) * b;
  float4 dc = float4(1, 0.5, 0.5, 1) * a;
  float4 sc = float4(lerp(global.bd_color, float3(1, 1, 1), 0.25), 1.75) * s;

  return (sc + bc + cc + dc) * float4(1, 1, 1, global.minimap_opacity);
}