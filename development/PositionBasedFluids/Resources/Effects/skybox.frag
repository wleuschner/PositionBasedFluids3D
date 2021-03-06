#version 400

in vec3 texcoords;
uniform samplerCube cube_texture;
layout(location=0) out vec4 frag_colour;

void main() {
  //frag_colour = vec4(1.0,0.0,0.0,1.0);
  frag_colour = texture(cube_texture, normalize(texcoords));
}
