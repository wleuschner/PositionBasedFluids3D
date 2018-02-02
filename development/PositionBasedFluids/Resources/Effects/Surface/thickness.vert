#version 330
layout (location=0) in vec3 pos;
layout (location=1) in vec3 normal;
layout (location=3) in vec3 ofs;
uniform float particleSize;
uniform mat4 pvm;
uniform mat4 modelView;

out vec3 eyeSpacePos;

void main()
{
    eyeSpacePos = vec3(modelView * vec4(ofs,1.0));
    float dist = length(eyeSpacePos);
    gl_PointSize = 1000.0*(particleSize/dist);
    gl_Position = pvm * vec4(ofs,1.0);
}
