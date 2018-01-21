#version 330
#extension GL_ARB_explicit_uniform_location : require
struct LightSource
{
    vec3 pos;
    vec3 ldir;
    vec3 amb;
    vec3 dif;
    vec3 spec;
};

uniform float smoothTimestep;
layout(location=10) uniform sampler2D depthMap;

uniform mat4 view;
uniform LightSource light;

in vec2 fragTexCoord;
out vec4 fragColor;

void main()
{
    float dxTex = textureSize(depthMap,0).x;
    float dyTex = textureSize(depthMap,0).y;
    float z = texture(depthMap,fragTexCoord).x;
    float cx;
    float cy;
    float dx;
    float dy;
    float dx2;
    float dy2;
    float d;

    float cz=(2.0 * 0.1) / (100.0 + 0.1 - z * (100.0 - 0.1));
    fragColor = vec4(cz,cz,cz,1.0);
}
