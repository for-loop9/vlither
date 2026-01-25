import modules.common;

struct vs_input {
  [[vk::location(0)]] float2 pos;
};

struct vs_output {
  float4 clipspace : SV_Position;
  [[vk::location(0)]] float2 uv;
};

[[vk::binding(0, 0)]] ConstantBuffer<Global> global;

vs_output vs_main(vs_input input) {
  vs_output output;

  output.clipspace = float4(input.pos * 2 - 1, 0, 1);
  output.uv = input.pos;

  return output;
}

float4 fs_main(vs_output input) {
  float2 frag_pos = input.uv * global.viewport;
  float r = (global.zoom * (global.bd_radius)) + EDGE_SOFTNESS;
  float r2 = (global.zoom * (global.bd_radius + 11.5)) + EDGE_SOFTNESS;

  float2 c = global.viewport * 0.5 + (global.grd - global.view) * global.zoom;
  float d = length(frag_pos - c);
  float dd = r - d;
  float dd2 = r2 - d;
  float p = length(float2(ddx(d), ddy(d)));
  float a = 1 - smoothstep(0.0, EDGE_SOFTNESS, dd / p);
  float a2 = 1 - smoothstep(0.0, EDGE_SOFTNESS, dd2 / p);
  float b = a - a2;

  float4 a2c = float4(0.2 * global.bd_color, a2 * 0.7);
  float4 ac = b * float4(global.bd_color, 0.4);

  return global.bd_opacity * (a2c + ac);
}