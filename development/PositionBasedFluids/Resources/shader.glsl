#version 430
#define M_PI 3.1415926535897932384626433832795

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

void applyExternalForces(uint gId);
void insertCountBuckets(uint gId);
void computeHistogram(uint gId);
void reoderParticles(uint gId);
void updateDisplacement(uint gId);
void updateTempPos(uint gId);
void updatePositions(uint gId);

void neighborInteraction(uint gId);
float checkParticleCollision(uint gId,Particle n,float minDist);

float kernelExecute(vec3 r,uint kernelId);
float kernelPoly6(vec3 r);
float kernelSpikey(vec3 r);
float kernelViscocity(vec3 r);

vec3 gradExecute(vec3 r,uint kernelId);
vec3 gradPoly6(vec3 r);
vec3 gradSpikey(vec3 r);
vec3 gradViscocity(vec3 r);



void main()
{
    uint gId = gl_GlobalInvocationID.x;
    switch(taskId)
    {
    //Apply external forces
    case 0:
        applyExternalForces(gId);
        break;
    //Insert and Count Buckets
    case 1:
        insertCountBuckets(gId);
        break;
    //Compute Histogram
    case 2:
        computeHistogram(gId);
        break;
    //Reoder Particles
    case 3:
        reoderParticles(gId);
        break;
    //Update Lambda
    case 4:
        neighborInteraction(gId);
        memoryBarrier();
        break;
    case 5:
        particles[gId].displacement = vec3(0.0,0.0,0.0);
        neighborInteraction(gId);
        memoryBarrier();
        break;
    case 6:
        neighborInteraction(gId);
        updateDisplacement(gId);
        memoryBarrier();
        break;
    case 7:
        updateTempPos(gId);
        memoryBarrier();
        break;
    case 8:
        updatePositions(gId);
        memoryBarrier();
        break;
    case 9:
        break;
    //Velocity update
    case 10:
        break;
    //Velocity correction
    case 11:
        break;
    }
}

void applyExternalForces(uint gId)
{
    particlesFront[gId].vel += vec3(0.0,-9.81,0.0);
    particlesFront[gId].tempPos = particlesFront[gId].pos + timestep*particlesFront[gId].vel;
}

void insertCountBuckets(uint gId)
{
    float xPosF = (minOfs.x+particlesFront[gId].pos.x)/kernelSupport;
    float yPosF = (minOfs.y+particlesFront[gId].pos.y)/kernelSupport;
    float zPosF = (minOfs.z+particlesFront[gId].pos.z)/kernelSupport;

    int xPos = clamp(int(floor(xPosF)),0,dimSize.x-1);
    int yPos = clamp(int(floor(yPosF)),0,dimSize.y-1);
    int zPos = clamp(int(floor(zPosF)),0,dimSize.z-1);

    int b = xPos+(dimSize.y*(yPos+dimSize.x*zPos));
    particlesFront[gId].bucket = b;
    atomicAdd(histogram[b],1);
}

void computeHistogram(uint gId)
{
    uint acc=0;
    for(uint i=0;i<nBuckets;i++)
    {
        uint tVal = histogram[i];
        histogram[i] = acc;
        acc+=tVal;
    }
}

void reoderParticles(uint gId)
{
    for(uint i=0;i<nParticles;i++)
    {
        Particle part = particlesFront[i];
        uint b = part.bucket;
        particles[histogram[b]+ofs[b]] = part;
        ofs[b]++;
    }
}

void updateDisplacement(uint gId)
{
    if(dot((particles[gId].tempPos+particles[gId].displacement),vec3(0.0,1.0,0.0))+1.5f<0.0)
    {
        vec3 n1 = vec3(0.0,1.0,0.0);
        vec3 r = normalize((particles[gId].tempPos)-particles[gId].pos);
        float t = -(1.5+dot(n1,particles[gId].pos))/dot(n1,r);
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
}

void updateTempPos(uint gId)
{
    particles[gId].tempPos = particles[gId].tempPos + particles[gId].displacement;
}

void updatePositions(uint gId)
{
    particles[gId].vel = (1.0/timestep)*(particles[gId].tempPos-particles[gId].pos);
    particles[gId].pos = particles[gId].tempPos;
}

void neighborInteraction(uint gId)
{
    Particle p = particles[gId];
    uint xPos = clamp(uint((floor((minOfs.x+p.tempPos.x)/kernelSupport))),0,dimSize.x-1);
    uint yPos = clamp(uint((floor((minOfs.y+p.tempPos.y)/kernelSupport))),0,dimSize.y-1);
    uint zPos = clamp(uint((floor((minOfs.z+p.tempPos.z)/kernelSupport))),0,dimSize.z-1);

    uint beginIdx[9];
    uint endIdx[9];

    //Bottom
    if(yPos>0)
    {
        if(zPos>0)
        {
            uint bottomLeftBack   = max(xPos-1,0)+(dimSize.x*(yPos-1+dimSize.y*(zPos-1)));
            uint bottomRightBack   = min(xPos+1,dimSize.x-1)+(dimSize.x*(yPos-1+dimSize.y*(zPos-1)));

            beginIdx[0] = histogram[bottomLeftBack];
            endIdx[0] = histogram[bottomRightBack+1];
        }
        if(zPos<dimSize.z-1)
        {
            uint bottomLeftFront  = max(xPos-1,0)+(dimSize.x*(yPos-1+dimSize.y*(zPos+1)));
            uint bottomRightFront  = min(xPos+1,dimSize.x-1)+(dimSize.x*(yPos-1+dimSize.y*(zPos+1)));

            beginIdx[1] = histogram[bottomLeftFront];
            if(bottomRightFront==nBuckets-1)
            {
                endIdx[1] = nParticles;
            }
            else
            {
                endIdx[1] = histogram[bottomRightFront+1];
            }

        }
        uint bottomLeftCenter = max(xPos-1,0)+(dimSize.x*(yPos-1+dimSize.y*zPos));
        uint bottomRightCenter = min(xPos+1,dimSize.x-1)+(dimSize.x*(yPos-1+dimSize.y*zPos));

        beginIdx[2] = histogram[bottomLeftCenter];
        endIdx[2] = histogram[bottomRightCenter+1];
    }

    //Top
    if(yPos<dimSize.y-1)
    {
        if(zPos>0)
        {
            uint topLeftBack = max(xPos-1,0)+(dimSize.x*(yPos+1+dimSize.y*(zPos-1)));
            uint topRightBack = min(xPos+1,dimSize.x-1)+(dimSize.x*(yPos+1+dimSize.y*(zPos-1)));

            beginIdx[3] = histogram[topLeftBack];
            endIdx[3] = histogram[topRightBack+1];
        }
        if(zPos<dimSize.z-1)
        {
            uint topLeftFront = max(xPos-1,0)+(dimSize.x*(yPos+1+dimSize.y*(zPos+1)));
            uint topRightFront = min(xPos+1,dimSize.x-1)+(dimSize.x*(yPos+1+dimSize.y*(zPos+1)));

            beginIdx[4] = histogram[topLeftFront];
            if(topRightFront==nBuckets-1)
            {
                endIdx[4] = nParticles;
            }
            else
            {
                endIdx[4] = histogram[topRightFront+1];
            }
        }
        uint topLeftCenter = max(xPos-1,0)+(dimSize.x*(yPos+1+dimSize.y*zPos));
        uint topRightCenter = min(xPos+1,dimSize.x-1)+(dimSize.x*(yPos+1+dimSize.y*zPos));

        beginIdx[5] = histogram[topLeftCenter];
        if(topRightCenter==nBuckets-1)
        {
            endIdx[5] = nParticles;
        }
        else
        {
            endIdx[5] = histogram[topRightCenter+1];
        }


    }

    //Center
    if(zPos>0)
    {
        uint centerLeftBack = max(xPos-1,0)+(dimSize.x*(yPos+dimSize.y*(zPos-1)));
        uint centerRightBack = min(xPos+1,dimSize.x-1)+(dimSize.x*(yPos+dimSize.y*(zPos-1)));

        beginIdx[6] = histogram[centerLeftBack];
        endIdx[6] = histogram[centerRightBack+1];
    }
    if(zPos<dimSize.z-1)
    {
        uint centerLeftFront = max(xPos-1,0)+(dimSize.x*(yPos+dimSize.y*(zPos+1)));
        uint centerRightFront = min(xPos+1,dimSize.x-1)+(dimSize.x*(yPos+dimSize.y*(zPos+1)));

        beginIdx[7] = histogram[centerLeftFront];
        endIdx[7] = histogram[centerRightFront+1];
    }
    uint centerLeftCenter = max(xPos-1,0)+(dimSize.x*(yPos+dimSize.y*zPos));
    uint centerRightCenter = min(xPos+1,dimSize.x-1)+(dimSize.x*(yPos+dimSize.y*zPos));
    beginIdx[8] = histogram[centerLeftCenter];
    endIdx[8] = histogram[centerRightCenter+1];

    //Check for neighborhood and dispatch
    float minDist = 100000.0f;
    float density = 0.0;
    vec3  gradSum1 = vec3(0.0,0.0,0.0);
    float gradSum2 = 0.0;
    float invRestDensity = 1.0/restDensity;
    for(uint i=0;i<9;i++)
    {
        for(uint j=beginIdx[i];j<endIdx[i];j++)
        {
            Particle n = particles[j];
            if(particles[gId].index!=n.index)
            {
                if(dot(p.tempPos-n.pos,p.tempPos-n.pos)<=kernelSupport*kernelSupport)
                {
                    switch(taskId)
                    {
                    case 4:
                        density += kernelPoly6(p.pos-n.pos);
                        vec3 grad = gradSpikey(p.pos-n.pos);
                        gradSum1 += grad;
                        grad = invRestDensity*-grad;
                        gradSum2 += dot(grad,grad);
                        break;
                    case 5:
                        float sCorr = -corrConst*pow(kernelPoly6(p.pos-n.pos)/kernelPoly6(vec3(kernelSupport,0.0,0.0)*corrDist),corrExp);
                        particles[gId].displacement += (p.lambda+n.lambda+sCorr)*gradSpikey(p.tempPos-n.pos);
                        break;
                    case 6:
                        minDist = checkParticleCollision(gId,n,minDist);
                        break;
                    case 7:
                        break;
                    }
                }
            }
        }
    }

    switch(taskId)
    {
    case 4:
        particles[gId].density = (invRestDensity*density)-1.0;
        gradSum1 = invRestDensity*gradSum1;
        float gradSum = gradSum2+dot(gradSum1,gradSum1);
        particles[gId].lambda = -particles[gId].density/(gradSum+cfmRegularization);
        break;
    case 5:
        break;
    case 6:
        break;
    case 7:
        break;
    }
}

float checkParticleCollision(uint gId,Particle n,float minDist)
{
    float ret = minDist;
    vec3 dVec = n.pos-(particles[gId].tempPos+particles[gId].displacement);
    float rSumSquared = 4*particleSize*particleSize;
    vec3 c = n.pos-particles[gId].pos;
    vec3 v = (particles[gId].tempPos+particles[gId].displacement)-particles[gId].pos;
    vec3 n1 = normalize(v);
    float d = dot(n1,c);
    float d1 = d*d-(dot((c),(c)))+particleSize*particleSize;
    if(d1>=0.0)
    {
        float f = dot(c,c)-(d*d);
        float t = sqrt(rSumSquared-f);
        float d = dot(t*v,n1);
        if(d<minDist)
        {
            float corr = (d-t);
            vec3 tempDispl = particles[gId].pos+(t*v)-particles[gId].tempPos;
            particles[gId].displacement = tempDispl;
            minDist=d;
        }
    }
    return minDist;
}

float kernelExecute(vec3 r,uint kernelId)
{
    switch(kernelId)
    {
    //Poly6 Kernel
    case 0:
        return kernelPoly6(r);
        break;
    //Spikey Kernel
    case 1:
        return kernelSpikey(r);
        break;
    //Viscocity Kernel
    case 2:
        return kernelViscocity(r);
        break;
    }
    return 0;
}

float kernelPoly6(vec3 r)
{
    if(dot(r,r)>kernelSupport*kernelSupport)
    {
        return 0;
    }
    return (315.0/(64*M_PI*pow(kernelSupport,9)))*pow((kernelSupport*kernelSupport-dot(r,r)),3);
}

float kernelSpikey(vec3 r)
{
    if(dot(r,r)>kernelSupport*kernelSupport)
    {
        return 0;
    }
    return (15.0/(M_PI*pow(kernelSupport,6)))*pow((kernelSupport-length(r)),3);
}

float kernelViscocity(vec3 r)
{
    if(dot(r,r)>kernelSupport*kernelSupport)
    {
        return 0;
    }
    return 0;
}

vec3 gradExecute(vec3 r,uint kernelId)
{
    switch(kernelId)
    {
    //Poly6 Kernel
    case 0:
        return gradPoly6(r);
        break;
    //Spikey Kernel
    case 1:
        return gradSpikey(r);
        break;
    //Viscocity Kernel
    case 2:
        return gradViscocity(r);
        break;
    }
    return vec3(0.0,0.0,0.0);
}

vec3 gradPoly6(vec3 r)
{
    if(dot(r,r)>kernelSupport*kernelSupport)
    {
        return vec3(0.0,0.0,0.0);
    }
    return vec3(-(945*(r.x)*pow((-pow((r.x),2)+pow((r.y),2)+pow((r.z),2)+kernelSupport*kernelSupport),2))/(32*M_PI*pow(kernelSupport,9)),
                     -(945*(r.y)*pow((-pow((r.x),2)+pow((r.y),2)+pow((r.z),2)+kernelSupport*kernelSupport),2))/(32*M_PI*pow(kernelSupport,9)),
                     -(945*(r.z)*pow((-pow((r.x),2)+pow((r.y),2)+pow((r.z),2)+kernelSupport*kernelSupport),2))/(32*M_PI*pow(kernelSupport,9)));


}

vec3 gradSpikey(vec3 r)
{
    if(dot(r,r)>kernelSupport*kernelSupport || dot(r,r)==0)
    {
        return vec3(0.0,0.0,0.0);
    }
    return ((-45.0f/(M_PI*(pow(kernelSupport,6))))*(pow((kernelSupport-length(r)),2))*normalize(r));
}


vec3 gradViscocity(vec3 r)
{
    if(dot(r,r)>kernelSupport*kernelSupport)
    {
        return 0;
    }
    return -(pow(length(r),3)/(2*pow(kernelSupport,3)))-(pow(length(r),2)/(pow(kernelSupport,2)))+(kernelSupport/(2*length(r)))-1;
}
