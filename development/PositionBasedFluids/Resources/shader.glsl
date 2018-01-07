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
uniform float particleSize;

uniform vec3 minOfs;
uniform ivec3 dimSize;
uniform uint nParticles;
uniform uint nBuckets;

layout (std430,binding=0) buffer frontBuffer
{
    Particle particlesFront[];
};

layout (std430,binding=1) buffer backBuffer
{
    Particle particles[];
};

layout (std430,binding=2) buffer histBuffer
{
    uint histogram[];
};

layout (std430,binding=3) buffer ofsBuffer
{
    uint ofs[];
};

layout( local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;

void main()
{
    uint gId = gl_GlobalInvocationID.x;
    switch(taskId)
    {
    //Apply external forces
    case 0:
        particlesFront[gId].vel += vec3(0.0,-9.81,0.0);
        particlesFront[gId].tempPos = particlesFront[gId].pos + timestep*particlesFront[gId].vel;
        break;
    //Insert and Count Buckets
    case 1:
        float xPosF = (minOfs.x+particlesFront[gId].pos.x)/kernelSupport;
        float yPosF = (minOfs.y+particlesFront[gId].pos.y)/kernelSupport;
        float zPosF = (minOfs.z+particlesFront[gId].pos.z)/kernelSupport;

        int xPos = clamp(int(floor(xPosF)),0,dimSize.x-1);
        int yPos = clamp(int(floor(yPosF)),0,dimSize.y-1);
        int zPos = clamp(int(floor(zPosF)),0,dimSize.z-1);

        int b = xPos+(dimSize.y*(yPos+dimSize.x*zPos));
        particlesFront[gId].bucket = b;
        atomicAdd(histogram[b],1);

        break;
    //Compute Histogram
    case 2:
        uint acc=0;
        for(uint i=0;i<nBuckets;i++)
        {
            uint tVal = histogram[i];
            histogram[i] = acc;
            acc+=tVal;
        }
        break;
    //Reoder Particles
    case 3:
        for(uint i=0;i<nParticles;i++)
        {
            Particle part = particlesFront[i];
            uint b = part.bucket;
            particles[histogram[b]+ofs[b]] = part;
            ofs[b]++;
        }
        break;
    case 4:
        particles[gId].vel = (1.0/timestep)*(particles[gId].tempPos-particles[gId].pos);
        particles[gId].pos = particles[gId].tempPos;
        break;
    case 5:
        particles[gId].displacement = vec3(0.0,0.0,0.0);
        if(dot((particles[gId].tempPos+particles[gId].displacement),vec3(0.0,1.0,0.0))+0.5f<0.0)
        {
            vec3 n1 = vec3(0.0,1.0,0.0);
            vec3 r = normalize((particles[gId].tempPos)-particles[gId].pos);
            float t = -(0.5+dot(n1,particles[gId].pos))/dot(n1,r);
            particles[gId].displacement = particles[gId].pos+(r*t)-particles[gId].tempPos;
        }
        if(dot((particles[gId].tempPos+particles[gId].displacement),vec3(0.0,-1.0,0.0))+1.5f<0.0)
        {
            vec3 n1 = vec3(0.0,-1.0,0.0);
            vec3 r = normalize((particles[gId].tempPos)-particles[gId].pos);
            float t = -(1.5+dot(n1,particles[gId].pos))/dot(n1,r);
            particles[gId].displacement = particles[gId].pos+(r*t)-particles[gId].tempPos;
        }
        if(dot((particles[gId].tempPos+particles[gId].displacement),vec3(-1.0,0.0,0.0))+1.0f<0.0)
        {
            vec3 n1 = vec3(-1.0,0.0,0.0);
            vec3 r = normalize((particles[gId].tempPos)-particles[gId].pos);
            float t = -(1.0+dot(n1,particles[gId].pos))/dot(n1,r);
            particles[gId].displacement = particles[gId].pos+(r*t)-particles[gId].tempPos;
        }
        if(dot((particles[gId].tempPos+particles[gId].displacement),vec3(1.0,0.0,0.0))+1.0f<0.0)
        {
            vec3 n1 = vec3(1.0,0.0,0.0);
            vec3 r = normalize((particles[gId].tempPos)-particles[gId].pos);
            float t = -(1.0+dot(n1,particles[gId].pos))/dot(n1,r);
            particles[gId].displacement = particles[gId].pos+(r*t)-particles[gId].tempPos;
        }
        if(dot((particles[gId].tempPos+particles[gId].displacement),vec3(0.0,0.0,-1.0))+1.0f<0.0)
        {
            vec3 n1 = vec3(0.0,0.0,-1.0);
            vec3 r = normalize((particles[gId].tempPos)-particles[gId].pos);
            float t = -(1.0+dot(n1,particles[gId].pos))/dot(n1,r);
            particles[gId].displacement = particles[gId].pos+(r*t)-particles[gId].tempPos;
        }
        if(dot((particles[gId].tempPos+particles[gId].displacement),vec3(0.0,0.0,1.0))+1.0f<0.0)
        {
            vec3 n1 = vec3(0.0,0.0,1.0);
            vec3 r = normalize((particles[gId].tempPos)-particles[gId].pos);
            float t = -(1.0+dot(n1,particles[gId].pos))/dot(n1,r);
            particles[gId].displacement = particles[gId].pos+(r*t)-particles[gId].tempPos;
        }
        break;
    case 6:
        particles[gId].tempPos = particles[gId].tempPos + particles[gId].displacement;
        break;
    case 7:
        break;
    case 8:
        break;
    //Velocity update
    case 9:
        break;
    //Velocity correction
    case 10:
        break;
    }
}
