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
layout (location=3) in vec3 ofs;
layout (location=4) in float dens;
layout (location=5) in float fluid;
layout (location=6) in float solid;
layout (location=7) in float gas;

flat out float density;
uniform float particleSize;
uniform mat4 pvm;
uniform mat4 modelView;
uniform mat3 normalMatrix;
uniform vec3 cPos;
uniform LightSource light;

out Data
{
    vec4 color;
} DataOut;


void main()
{
    vec4 diff;
    //More Than Resting Density RED
    if(true/*solid==0.0*/)
    {
        if(dens>0.0)
        {
            diff = vec4(clamp(+dens/2.0,0.0,1.0),0.0,clamp(1.0-dens/2.0,0.0,1.0),1.0);
        }
        //Less Than Resting Density GREEN
        else
        {
            diff = vec4(0.0,clamp(-dens/2.0,0.0,1.0),clamp(1.0+dens/2.0,0.0,1.0),1.0);
        }
    }
    else
    {
        diff = vec4(0.0,0.0,0.0,1.0);
    }
    density = dens;

    vec4 spec = vec4(0.0);
    vec3 n = normalize(normalMatrix * normal);
    float intensity = max(dot(n,light.ldir),0.0);
    if(intensity>0.0)
    {
        vec3 p = vec3(normalize(modelView*vec4(pos+ofs,1.0)));
        vec3 e = normalize(-p);
        vec3 h = normalize(light.ldir + e);
        float intSpec = max(dot(h,n),0.0);
        spec = vec4(0.5,0.5,0.5,1.0) * pow(intSpec,1.2);
    }
    DataOut.color = max(intensity*diff+spec,diff);

    gl_Position = pvm * vec4((pos*particleSize)+ofs,1.0);
}
