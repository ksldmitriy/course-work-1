#version 450
layout(location = 0) in vec2 inPosition;
layout(binding = 0) uniform inScale_t {
  vec2 scale;
  vec2 pos;
}
uCamera;

void main() {
  vec2 pos = inPosition;

  pos.x *= uCamera.scale.x;
  pos.y *= uCamera.scale.y;

  pos -= uCamera.pos;

  gl_Position = vec4(pos, 0.0, 1.0);
}
