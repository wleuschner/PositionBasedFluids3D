#version 430

struct Particle
{
    vec3 pos;
    vec3 vel;
    float lambda;
    float mass;
    float density;
    vec3 curl;
    vec3 tempPos;
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

layout (packed,binding=0) buffer destBuffer
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
        outBuffer.particles[gId].vel += timestep*vec3(0.0,-9.81,0.0);
        outBuffer.particles[gId].tempPos = outBuffer.particles[gId].pos + timestep*outBuffer.particles[gId].vel;
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        break;
    //Velocity update
    case 6:
        outBuffer.particles[gId].vel += (1.0/timestep)*(outBuffer.particles[gId].tempPos-outBuffer.particles[gId].pos);
        break;
    //Velocity correction
    case 7:
        outBuffer.particles[gId].vel += timestep*vec3(0.0,-9.81,0.0);
        outBuffer.particles[gId].pos = outBuffer.particles[gId].tempPos;
        break;
    }
}
