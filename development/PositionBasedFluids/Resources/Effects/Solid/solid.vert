#version 330
struct LightSource
{
    vec3 pos;
    vec3 ldir;
    vec3 amb;
    vec3 dif;
    vec3 spec;
};

layout (location=0) in vec3 pos;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 uv;

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
    diff = vec4(0.2,0.2,0.2,1.0);

    vec4 spec = vec4(0.0);
    vec3 n = normalize(normalMatrix * normal);
    float intensity = max(dot(n,light.ldir),0.0);
    if(intensity>0.0)
    {
        vec3 p = vec3(normalize(modelView*vec4(pos,1.0)));
        vec3 e = normalize(-p);
        vec3 h = normalize(light.ldir + e);
        float intSpec = max(dot(h,n),0.0);
        spec = vec4(0.5,0.5,0.5,1.0) * pow(intSpec,1.2);
    }
    DataOut.color = max(intensity*diff+spec,diff);

    gl_Position = pvm * vec4(pos,1.0);
}
