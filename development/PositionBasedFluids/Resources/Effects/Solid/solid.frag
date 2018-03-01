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

uniform LightSource light;

layout(location=0) out vec4 frag_colour;

void main()
{
    frag_colour = DataIn.color;
}
