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

uniform vec3 cPos;
uniform mat4 view;
uniform LightSource light;

out vec4 fragColor;

void main()
{
    fragColor = DataIn.color;
}
