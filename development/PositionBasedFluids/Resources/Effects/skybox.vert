#version 400

layout (location=0) in vec3 pos;
layout (location=1) in vec3 normal;
layout (location=3) in vec3 ofs;
layout (location=4) in float dens;
uniform mat4 pvm;
out vec3 texcoords;

void main() {
  texcoords = pos;
  gl_Position = pvm * vec4(pos, 1.0);
}
