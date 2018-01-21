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


uniform mat4 view;
uniform LightSource light;

in vec2 fragTexCoord;
out vec4 fragColor;

void main()
{
    fragColor = DataIn.color;
}
