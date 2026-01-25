import modules.common;

struct vs_input {
  [[vk::location(0)]] float2 pos;
  [[vk::location(1)]] float4 rect;
  [[vk::location(2)]] float4 uv;
  [[vk::location(3)]] float4 color;
};

struct vs_output {
  float4 clipspace : SV_Position;
  [[vk::location(0)]] float2 uv;
  [[vk::location(1)]] float4 color;
};

[[vk::binding(0, 0)]] ConstantBuffer<Global> global;
[[vk::binding(1, 0)]] Sampler2D tex_atlas;

vs_output vs_main(vs_input input) {
  vs_output output;

  float2 r = input.pos * input.rect.zw;
  float2 wp = r + input.rect.xy - 0.5 * input.rect.zw;

  float2 hv = global.viewport * 0.5;
  float2 o = (wp - hv) / hv;

  output.clipspace = float4(o, 1);
  output.uv = input.uv.xy + input.pos * input.uv.zw;
  output.color = input.color;

  return output;
}

float4 fs_main(vs_output input) {
  float4 c = tex_atlas.Sample(input.uv).rgba;
  return c * input.color;
}