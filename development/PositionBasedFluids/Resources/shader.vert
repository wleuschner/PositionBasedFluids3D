#version 330
//uniform  mat4 pvm;
layout (location=0) in vec3 pos;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 ofs;
layout (location=3) in float dens;
flat out float density;
uniform mat4 pvm;
uniform mat3 normalMatrix;

void main()
{
    density = dens;
    gl_Position = pvm * vec4(pos+ofs,1.0);
}
