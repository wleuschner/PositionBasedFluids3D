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
    Particle p = particles[gId];
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
        particlesFront[gId] = particles[gId];
        p.vel = (1.0/timestep)*(p.tempPos-p.pos);
        particles[gId] = p;
        particlesFront[gId] = p;
        memoryBarrier();
        break;
    case 9:
        neighborInteraction(gId);
        memoryBarrier();
        break;
    case 10:
        updatePositions(gId);
        memoryBarrier();
        break;
    //Velocity update
    case 11:
        break;
    //Velocity correction
    case 12:
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
    Particle p = particlesFront[gId];
    float xPosF = (minOfs.x+p.pos.x)/kernelSupport;
    float yPosF = (minOfs.y+p.pos.y)/kernelSupport;
    float zPosF = (minOfs.z+p.pos.z)/kernelSupport;

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
    /*
    for(uint i=0;i<nParticles;i++)
    {
        Particle part = particlesFront[i];
        uint b = part.bucket;
        particles[histogram[b]+atomicAdd(ofs[b],1)] = part;
    }*/

    Particle part = particlesFront[gId];
    uint b = part.bucket;
    particles[histogram[b]+atomicAdd(ofs[b],1)] = part;
}

void updateDisplacement(uint gId)
{
    Particle p = particles[gId];
    if(dot((p.tempPos+p.displacement),vec3(0.0,1.0,0.0))+1.5f<0.0)
    {
        vec3 n1 = vec3(0.0,1.0,0.0);
        vec3 r = normalize((p.tempPos+p.displacement)-p.pos);
        float t = -(1.5+dot(n1,p.pos))/dot(n1,r);
        p.displacement = p.pos+(r*t)-p.tempPos;
    }
    if(dot((p.tempPos+p.displacement),vec3(0.0,-1.0,0.0))+1.5f<0.0)
    {
        vec3 n1 = vec3(0.0,-1.0,0.0);
        vec3 r = normalize((p.tempPos+p.displacement)-p.pos);
        float t = -(1.5+dot(n1,p.pos))/dot(n1,r);
        p.displacement = p.pos+(r*t)-p.tempPos;
    }
    if(dot((p.tempPos+p.displacement),vec3(-1.0,0.0,0.0))+1.0f<0.0)
    {
        vec3 n1 = vec3(-1.0,0.0,0.0);
        vec3 r = normalize((p.tempPos+p.displacement)-p.pos);
        float t = -(1.0+dot(n1,p.pos))/dot(n1,r);
        p.displacement = p.pos+(r*t)-p.tempPos;
    }
    if(dot((p.tempPos+p.displacement),vec3(1.0,0.0,0.0))+1.0f<0.0)
    {
        vec3 n1 = vec3(1.0,0.0,0.0);
        vec3 r = normalize((p.tempPos+p.displacement)-p.pos);
        float t = -(1.0+dot(n1,p.pos))/dot(n1,r);
        p.displacement = p.pos+(r*t)-p.tempPos;
    }
    if(dot((p.tempPos+p.displacement),vec3(0.0,0.0,-1.0))+1.0f<0.0)
    {
        vec3 n1 = vec3(0.0,0.0,-1.0);
        vec3 r = normalize((p.tempPos+p.displacement)-p.pos);
        float t = -(1.0+dot(n1,p.pos))/dot(n1,r);
        p.displacement = p.pos+(r*t)-p.tempPos;
    }
    if(dot((p.tempPos+p.displacement),vec3(0.0,0.0,1.0))+1.0f<0.0)
    {
        vec3 n1 = vec3(0.0,0.0,1.0);
        vec3 r = normalize((p.tempPos+p.displacement)-p.pos);
        float t = -(1.0+dot(n1,p.pos))/dot(n1,r);
        p.displacement = p.pos+(r*t)-p.tempPos;
    }
    particles[gId].displacement = p.displacement;
}

void updateTempPos(uint gId)
{
    Particle p = particles[gId];
    particles[gId].tempPos += p.displacement;
}

void updatePositions(uint gId)
{
    Particle p = particlesFront[gId];
    //particlesFront[gId].vel =
    particlesFront[gId].pos = p.tempPos;
}

void neighborInteraction(uint gId)
{
    Particle p = particles[gId];
    int xPos = int(clamp(int((floor((minOfs.x+p.tempPos.x)/kernelSupport))),0,dimSize.x-1));
    int yPos = int(clamp(int((floor((minOfs.y+p.tempPos.y)/kernelSupport))),0,dimSize.y-1));
    int zPos = int(clamp(int((floor((minOfs.z+p.tempPos.z)/kernelSupport))),0,dimSize.z-1));

    uint beginIdx[9];
    uint endIdx[9];

    int xPosBegin,xPosEnd;
    int yPosBegin,yPosEnd;
    int zPosBegin,zPosEnd;

    xPosBegin = max(xPos-1,0);
    xPosEnd   = min(xPos+1,dimSize.x-1);

    yPosBegin = max(yPos-1,0);
    yPosEnd   = min(yPos+1,dimSize.y-1);

    zPosBegin = max(zPos-1,0);
    zPosEnd   = min(zPos+1,dimSize.z-1);

    //Bottom
    uint bottomLeftBack   = xPosBegin+(dimSize.x*(yPosBegin+dimSize.y*zPosBegin));
    uint bottomRightBack   = xPosEnd+(dimSize.x*(yPosBegin+dimSize.y*zPosBegin));

    beginIdx[0] = histogram[bottomLeftBack];
    endIdx[0] = histogram[bottomRightBack+1];

    uint bottomLeftFront  = xPosBegin+(dimSize.x*(yPosBegin+dimSize.y*zPosEnd));
    uint bottomRightFront  = xPosEnd+(dimSize.x*(yPosBegin+dimSize.y*zPosEnd));

    beginIdx[1] = histogram[bottomLeftFront];
    if(bottomRightFront==nBuckets-1)
    {
        endIdx[1] = nParticles;
    }
    else
    {
        endIdx[1] = histogram[bottomRightFront+1];
    }
    uint bottomLeftCenter = xPosBegin+(dimSize.x*(yPosBegin+dimSize.y*zPos));
    uint bottomRightCenter = xPosEnd+(dimSize.x*(yPosBegin+dimSize.y*zPos));

    beginIdx[2] = histogram[bottomLeftCenter];
    endIdx[2] = histogram[bottomRightCenter+1];

    //Top
    uint topLeftBack = xPosBegin+(dimSize.x*(yPosEnd+dimSize.y*zPosBegin));
    uint topRightBack = xPosEnd+(dimSize.x*(yPosEnd+dimSize.y*zPosBegin));

    beginIdx[3] = histogram[topLeftBack];
    endIdx[3] = histogram[topRightBack+1];

    uint topLeftFront = xPosBegin+(dimSize.x*(yPosEnd+dimSize.y*zPosEnd));
    uint topRightFront = xPosEnd+(dimSize.x*(yPosEnd+dimSize.y*zPosEnd));

    beginIdx[4] = histogram[topLeftFront];
    if(topRightFront==nBuckets-1)
    {
        endIdx[4] = nParticles;
    }
    else
    {
        endIdx[4] = histogram[topRightFront+1];
    }
    uint topLeftCenter = xPosBegin+(dimSize.x*(yPosEnd+dimSize.y*zPos));
    uint topRightCenter = xPosEnd+(dimSize.x*(yPosEnd+dimSize.y*zPos));

    beginIdx[5] = histogram[topLeftCenter];
    if(topRightCenter==nBuckets-1)
    {
        endIdx[5] = nParticles;
    }
    else
    {
        endIdx[5] = histogram[topRightCenter+1];
    }

    //Center
    uint centerLeftBack = xPosBegin+(dimSize.x*(yPos+dimSize.y*zPosBegin));
    uint centerRightBack = xPosEnd+(dimSize.x*(yPos+dimSize.y*zPosBegin));

    beginIdx[6] = histogram[centerLeftBack];
    endIdx[6] = histogram[centerRightBack+1];

    uint centerLeftFront = xPosBegin+(dimSize.x*(yPos+dimSize.y*zPosEnd));
    uint centerRightFront = xPosEnd+(dimSize.x*(yPos+dimSize.y*zPosEnd));

    beginIdx[7] = histogram[centerLeftFront];
    endIdx[7] = histogram[centerRightFront+1];

    uint centerLeftCenter = xPosBegin+(dimSize.x*(yPos+dimSize.y*zPos));
    uint centerRightCenter = xPosEnd+(dimSize.x*(yPos+dimSize.y*zPos));
    beginIdx[8] = histogram[centerLeftCenter];
    endIdx[8] = histogram[centerRightCenter+1];

    //Check for neighborhood and dispatch
    float minDist = 100000.0f;
    float density = 0.0;
    vec3  gradSum1 = vec3(0.0,0.0,0.0);
    float gradSum2 = 0.0;
    vec3  curl = vec3(0.0,0.0,0.0);
    vec3  velAccum = vec3(0.0,0.0,0.0);
    float invRestDensity = 1.0/restDensity;
    for(uint i=0;i<9;i++)
    {
        for(uint j=beginIdx[i];j<endIdx[i];j++)
        {
            Particle n = particles[j];
            if(p.index!=n.index)
            {
                if(dot(p.tempPos-n.pos,p.tempPos-n.pos)<=kernelSupport*kernelSupport)
                {
                    switch(taskId)
                    {
                    case 4:
                        density += kernelPoly6(p.tempPos-n.pos);
                        vec3 grad = gradSpikey(p.tempPos-n.pos);
                        gradSum1 += grad;
                        grad = invRestDensity*-gradSpikey(p.tempPos-n.pos);
                        gradSum2 += dot(grad,grad);
                        break;
                    case 5:
                        float sCorr = -corrConst*pow(kernelPoly6(p.tempPos-n.pos)/kernelPoly6(vec3(kernelSupport,0.0,0.0)*corrDist),corrExp);
                        particles[gId].displacement += (p.lambda+n.lambda+sCorr)*gradSpikey(p.tempPos-n.pos);
                        break;
                    case 6:
                        minDist = checkParticleCollision(gId,n,minDist);
                        break;
                    case 7:
                        break;
                    case 8:
                        break;
                    case 9:
                        curl += cross(n.vel-p.vel,gradViscocity(p.tempPos-n.pos));
                        velAccum += (n.vel-p.vel)*kernelViscocity(p.tempPos-n.pos);
                        break;
                    }
                }
            }
        }
    }

    switch(taskId)
    {
    case 4:
        density = (invRestDensity*density)-1.0;
        particles[gId].density = density;
        gradSum1 = invRestDensity*gradSum1;
        float gradSum = gradSum2+dot(gradSum1,gradSum1);
        particles[gId].lambda = -density/(gradSum+cfmRegularization);
        break;
    case 5:
        particles[gId].displacement = invRestDensity*particles[gId].displacement;
        break;
    case 6:
        break;
    case 7:
        break;
    case 8:
        break;
    case 9:
        particlesFront[gId].vel += timestep*artVisc*velAccum;
        particlesFront[gId].pos  = p.tempPos;
        break;
    }
}

float checkParticleCollision(uint gId,Particle n,float minDist)
{
    Particle p = particles[gId];
    float ret = minDist;
    vec3 dVec = n.pos-(p.tempPos+p.displacement);
    float rSumSquared = 4*particleSize*particleSize;
    vec3 c = n.pos-p.pos;
    vec3 v = (p.tempPos+p.displacement)-p.pos;
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
            vec3 tempDispl = p.pos+(t*v)-p.tempPos;
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
