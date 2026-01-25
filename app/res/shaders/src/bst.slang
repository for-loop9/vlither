import modules.common;

struct vs_input {
  [[vk::location(0)]] float2 pos;
  [[vk::location(1)]] float3 circ;
  [[vk::location(2)]] float4 color;
};

struct vs_output {
  float4 clipspace : SV_Position;
  [[vk::location(0)]] float2 uv;
  [[vk::location(2)]] float4 color;
};

[[vk::binding(0, 0)]] ConstantBuffer<Global> global;

vs_output vs_main(vs_input input) {
  vs_output output;

  float2 p = (input.pos - 0.5) * input.circ.z;
  float2 r = p + 0.5 * input.circ.z;

  float2 hv = global.viewport * 0.5;
  float2 wp = r + input.circ.xy;
  float2 o = (wp - hv) / hv;

  output.clipspace = float4(o, 1);
  output.uv = input.pos;
  output.color = input.color;

  return output;
}

float4 fs_main(vs_output input) {
  float2 p = input.uv * 2.0 - 1.0;
  float d = length(p);
  float radius = 0.8;
  float v = 1.0 - d / radius;
  v = saturate(v);
  v = 0.2 * (1.0 - cos(PI * v));
  return float4(input.color.rgb, input.color.a * v);
}