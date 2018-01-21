#version 330
struct LightSource
{
    vec3 pos;
    vec3 ldir;
    vec3 amb;
    vec3 dif;
    vec3 spec;
};

layout (location=2) in vec3 pos;
layout (location=3) in float dens;
uniform float particleSize;
uniform mat4 pvm;
uniform mat4 modelView;
uniform mat3 normalMatrix;
uniform LightSource light;

out Data
{
    vec4 color;
} DataOut;


void main()
{
    vec4 diff;
    //More Than Resting Density RED
    if(dens>0.0)
    {
        diff = vec4(clamp(+dens/2.0,0.0,1.0),0.0,clamp(1.0-dens/2.0,0.0,1.0),1.0);
    }
    //Less Than Resting Density GREEN
    else
    {
        diff = vec4(0.0,clamp(-dens/2.0,0.0,1.0),clamp(1.0+dens/2.0,0.0,1.0),1.0);
    }

    DataOut.color = diff;

    gl_PointSize = 200.0*particleSize;
    gl_Position = pvm * vec4(pos,1.0);
}
