#version 430

struct Particle
{
    uint index;
    uint bucket;
    float lambda;
    float mass;
    float density;

    vec3 pos;
    vec3 vel;

    vec3 curl;
    vec3 tempPos;
    vec3 displacement;
};

uniform uint taskId;
uniform float timestep;
uniform uint iterations;
uniform float kernelSupport;
uniform float restDensity;
uniform float artVisc;
uniform float artVort;
uniform float cfmRegularization;
uniform float corrConst;
uniform float corrDist;
uniform float corrExp;

layout (std430,binding=0) buffer destBuffer
{
    Particle particles[];
} outBuffer;

layout( local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;

void main()
{
    uint gId = gl_GlobalInvocationID.x;
    switch(taskId)
    {
    //Apply external forces
    case 0:
        outBuffer.particles[gId].vel += vec3(0.0,-9.81,0.0);
        outBuffer.particles[gId].tempPos = outBuffer.particles[gId].pos + timestep*outBuffer.particles[gId].vel;
        break;
    case 1:
        outBuffer.particles[gId].vel += (1.0/timestep)*(outBuffer.particles[gId].tempPos-outBuffer.particles[gId].pos);
        outBuffer.particles[gId].pos = outBuffer.particles[gId].tempPos;
        break;
    case 2:
        if(dot((outBuffer.particles[gId].tempPos),vec3(0.0,1.0,0.0))+0.5f<0.0)
        {
            vec3 n1 = vec3(0.0,1.0,0.0);
            vec3 r = normalize((outBuffer.particles[gId].tempPos)-outBuffer.particles[gId].pos);
            float t = -(0.5+dot(n1,outBuffer.particles[gId].pos))/dot(n1,r);
            outBuffer.particles[gId].displacement = outBuffer.particles[gId].pos+(r*t)-outBuffer.particles[gId].tempPos;
        }
        if(dot((outBuffer.particles[gId].tempPos),vec3(0.0,-1.0,0.0))+1.5f<0.0)
        {
            vec3 n1 = vec3(0.0,-1.0,0.0);
            vec3 r = normalize((outBuffer.particles[gId].tempPos)-outBuffer.particles[gId].pos);
            float t = -(1.5+dot(n1,outBuffer.particles[gId].pos))/dot(n1,r);
            outBuffer.particles[gId].displacement = outBuffer.particles[gId].pos+(r*t)-outBuffer.particles[gId].tempPos;
        }
        if(dot((outBuffer.particles[gId].tempPos),vec3(-1.0,0.0,0.0))+1.0f<0.0)
        {
            vec3 n1 = vec3(-1.0,0.0,0.0);
            vec3 r = normalize((outBuffer.particles[gId].tempPos)-outBuffer.particles[gId].pos);
            float t = -(1.0+dot(n1,outBuffer.particles[gId].pos))/dot(n1,r);
            outBuffer.particles[gId].displacement = outBuffer.particles[gId].pos+(r*t)-outBuffer.particles[gId].tempPos;
        }
        if(dot((outBuffer.particles[gId].tempPos),vec3(1.0,0.0,0.0))+1.0f<0.0)
        {
            vec3 n1 = vec3(1.0,0.0,0.0);
            vec3 r = normalize((outBuffer.particles[gId].tempPos)-outBuffer.particles[gId].pos);
            float t = -(1.0+dot(n1,outBuffer.particles[gId].pos))/dot(n1,r);
            outBuffer.particles[gId].displacement = outBuffer.particles[gId].pos+(r*t)-outBuffer.particles[gId].tempPos;
        }
        if(dot((outBuffer.particles[gId].tempPos),vec3(0.0,0.0,-1.0))+1.0f<0.0)
        {
            vec3 n1 = vec3(0.0,0.0,-1.0);
            vec3 r = normalize((outBuffer.particles[gId].tempPos)-outBuffer.particles[gId].pos);
            float t = -(1.0+dot(n1,outBuffer.particles[gId].pos))/dot(n1,r);
            outBuffer.particles[gId].displacement = outBuffer.particles[gId].pos+(r*t)-outBuffer.particles[gId].tempPos;
        }
        if(dot((outBuffer.particles[gId].tempPos),vec3(0.0,0.0,1.0))+1.0f<0.0)
        {
            vec3 n1 = vec3(0.0,0.0,1.0);
            vec3 r = normalize((outBuffer.particles[gId].tempPos)-outBuffer.particles[gId].pos);
            float t = -(1.0+dot(n1,outBuffer.particles[gId].pos))/dot(n1,r);
            outBuffer.particles[gId].displacement = outBuffer.particles[gId].pos+(r*t)-outBuffer.particles[gId].tempPos;
        }
        break;
    case 3:
        outBuffer.particles[gId].tempPos = outBuffer.particles[gId].tempPos + outBuffer.particles[gId].displacement;
        break;
    case 4:
        break;
    case 5:
        break;
    //Velocity update
    case 6:
        break;
    //Velocity correction
    case 7:
        break;
    }
}
