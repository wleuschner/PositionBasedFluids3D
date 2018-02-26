#version 330

const vec2 madd=vec2(0.5,0.5);
layout (location=0) in vec3 pos;
layout (location=2) in vec2 uv;
layout (location=5) in float fluid;
layout (location=6) in float solid;
layout (location=7) in float gas;

flat out float solidFrag;

out vec2 fragTexCoord;

void main() {
   solidFrag = solid;
   fragTexCoord = pos.xy*madd+madd; // scale vertex attribute to [0-1] range
   gl_Position = vec4(pos.xy,0.0,1.0);
}
