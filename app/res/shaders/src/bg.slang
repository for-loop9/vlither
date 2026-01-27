import modules.common;

struct vs_input {
  [[vk::location(0)]] float2 pos;
};

struct vs_output {
  float4 clipspace : SV_Position;
  [[vk::location(0)]] float2 uv;
};

[[vk::binding(0, 0)]] ConstantBuffer<Global> global;
[[vk::binding(2, 0)]] Sampler2D bg_tex;

vs_output vs_main(vs_input input) {
  vs_output output;
  output.clipspace = float4(input.pos * 2 - 1, 0.0, 1.0);
  output.uv = input.pos;
  return output;
}

float4 fs_main(vs_output input) {
  float2 sx = input.uv * global.viewport;
  float2 c = global.viewport * 0.5;
  float2 wp = (sx - c) / (global.zoom) + global.view;
  float2 uv = wp / (global.bg_size * global.bg_scale);
  float4 bg_col = bg_tex.Sample(uv);
  return float4(bg_col.rgb * global.bg_color, global.bg_opacity);
}