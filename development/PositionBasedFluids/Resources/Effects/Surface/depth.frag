#version 330
struct LightSource
{
    vec3 pos;
    vec3 ldir;
    vec3 amb;
    vec3 dif;
    vec3 spec;
};

in Data
{
    vec4 color;
} DataIn;

uniform float smoothTimestep;
uniform sampler2D depthMap;

uniform mat4 view;
uniform LightSource light;

in vec2 fragTexCoord;
out vec4 fragColor;

void main()
{
    //float z = texture2D(depthMap,fragTexCoord);
    float cx;
    float cy;
    float dx;
    float dy;
    float dx2;
    float dy2;
    float d;
    fragColor = DataIn.color;
}
