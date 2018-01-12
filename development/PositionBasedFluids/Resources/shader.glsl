#version 430
#extension GL_ARB_compute_variable_group_size: enable

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

layout( local_size_variable ) in;


void updateDisplacement(uint gId);
void neighborInteraction(uint gId,Particle p);
Particle checkBBoxCollision(Particle p);

float kernelExecute(vec3 r,uint kernelId);
float kernelPoly6(vec3 r);
float kernelSpikey(vec3 r);
float kernelViscocity(vec3 r);

vec3 gradExecute(vec3 r,uint kernelId);
vec3 gradPoly6(vec3 r);
vec3 gradSpikey(vec3 r);
vec3 gradViscocity(vec3 r);

uint beginIdx[9];
uint endIdx[9];

void main()
{
    uint gId = gl_GlobalInvocationID.x;
    if(gId>=nParticles)
    {
        return;
    }

    float minDist = 100000.0f;
    float density = 0.0;
    vec3  gradSum1 = vec3(0.0,0.0,0.0);
    float gradSum2 = 0.0;
    vec3  curl = vec3(0.0,0.0,0.0);
    vec3  velAccum = vec3(0.0,0.0,0.0);
    float invRestDensity = 1.0/restDensity;

    switch(taskId)
    {
    //Apply external forces
    case 0:
        {
            particlesFront[gId].vel += vec3(0.0,-9.81,0.0);
            particlesFront[gId].tempPos = particlesFront[gId].pos + timestep*particlesFront[gId].vel;
            break;
        }
    //Insert and Count Buckets
    case 1:
        {
            Particle pf = particlesFront[gId];
            float xPosF = (minOfs.x+pf.pos.x)/kernelSupport;
            float yPosF = (minOfs.y+pf.pos.y)/kernelSupport;
            float zPosF = (minOfs.z+pf.pos.z)/kernelSupport;

            int xPos = clamp(int(floor(xPosF)),0,dimSize.x-1);
            int yPos = clamp(int(floor(yPosF)),0,dimSize.y-1);
            int zPos = clamp(int(floor(zPosF)),0,dimSize.z-1);

            uint b = xPos+(dimSize.y*(yPos+dimSize.x*zPos));
            particlesFront[gId].bucket = b;
            atomicAdd(histogram[b],1);
            break;
        }
    //Compute Histogram
    case 2:
        {
            uint acc=0;
            for(uint i=0;i<=nBuckets;i++)
            {
                uint tVal = histogram[i];
                histogram[i] = acc;
                acc+=tVal;
            }
            break;
        }
    //Reoder Particles
    case 3:
        {
            Particle pf = particlesFront[gId];
            uint b = pf.bucket;
            particles[histogram[b]+atomicAdd(ofs[b],1)] = pf;
            break;
        }
    //Update Lambda
    case 4:
        {
            Particle p = particles[gId];
            neighborInteraction(gId,p);
            for(uint i=0;i<9;i++)
            {
                for(uint j=beginIdx[i];j<endIdx[i];j++)
                {
                    Particle n = particles[j];
                    if(p.index!=n.index)
                    {
                        if(dot(p.tempPos-n.pos,p.tempPos-n.pos)<=kernelSupport*kernelSupport)
                        {
                            density += kernelPoly6(p.tempPos-n.pos);
                            vec3 grad = gradSpikey(p.tempPos-n.pos);
                            gradSum1 += grad;
                            grad = invRestDensity*-gradSpikey(p.tempPos-n.pos);
                            gradSum2 += dot(grad,grad);
                        }
                    }
                }
            }
            density = (invRestDensity*density)-1.0;
            particles[gId].density = density;
            gradSum1 = invRestDensity*gradSum1;
            float gradSum = gradSum2+dot(gradSum1,gradSum1);
            particles[gId].lambda = -density/(gradSum+cfmRegularization);
            break;
        }
    case 5:
        {
            Particle p = particles[gId];
            vec3 displacement = vec3(0.0,0.0,0.0);
            neighborInteraction(gId,p);
            for(uint i=0;i<9;i++)
            {
                for(uint j=beginIdx[i];j<endIdx[i];j++)
                {
                    Particle n = particles[j];
                    if(p.index!=n.index)
                    {
                        if(dot(p.tempPos-n.pos,p.tempPos-n.pos)<=kernelSupport*kernelSupport)
                        {
                            float sCorr = -corrConst*pow(kernelPoly6(p.tempPos-n.pos)/kernelPoly6(vec3(kernelSupport*corrDist,0.0,0.0)),corrExp);
                            displacement += (p.lambda+n.lambda+sCorr)*gradSpikey(p.tempPos-n.pos);
                        }
                    }
                }
            }
            particles[gId].displacement = invRestDensity*displacement;
            break;
        }
    case 6:
        {
            Particle p = particles[gId];
            float minDist = 1000000.0f;
            float rSumSquared = 4*particleSize*particleSize;
            vec3 v = (p.tempPos+p.displacement)-p.pos;
            vec3 n1 = normalize(v);
            vec3 tempDispl = p.displacement;



            /*neighborInteraction(gId,p);

            for(uint i=0;i<9;i++)
            {
                for(uint j=beginIdx[i];j<endIdx[i];j++)
                {
                    Particle n = particles[j];
                    if(p.index!=n.index)
                    {
                        if(dot(p.tempPos-n.pos,p.tempPos-n.pos)<=kernelSupport*kernelSupport)
                        {
                            vec3 c = n.pos-p.pos;
                            float d = dot(n1,c);
                            if(d>0.0)
                            {
                                float f = dot(c,c)-(d*d);
                                if(f<rSumSquared)
                                {
                                    float t = sqrt(rSumSquared-f);
                                    float corr = (d-t);
                                    corr = min(corr,minDist);
                                    tempDispl = p.pos+(corr*n1)-p.tempPos;
                                    minDist=corr;
                                }
                            }
                        }
                    }
                }
            }
            p.displacement = tempDispl;*/
            particles[gId] = checkBBoxCollision(p);
            break;
        }
    case 7:
        {
            Particle p = particles[gId];
            particles[gId].tempPos += p.displacement;
            break;
        }
    case 8:
        {
            Particle p = particles[gId];
            p.vel = (1.0/timestep)*(p.tempPos-p.pos);
            //particles[gId] = p;
            particlesFront[gId] = p;
            break;
        }
    case 9:
        {
            Particle pf = particlesFront[gId];
            neighborInteraction(gId,pf);
            for(uint i=0;i<9;i++)
            {
                for(uint j=beginIdx[i];j<endIdx[i];j++)
                {
                    Particle n = particlesFront[j];
                    if(pf.index!=n.index)
                    {
                        if(dot(pf.tempPos-n.pos,pf.tempPos-n.pos)<=kernelSupport*kernelSupport)
                        {
                            curl += cross(n.vel-pf.vel,gradViscocity(pf.tempPos-n.pos));
                            velAccum += (n.vel-pf.vel)*kernelViscocity(pf.tempPos-n.pos);
                        }
                    }
                }
            }
            if(dot(curl,curl)>0.0)
            {
                particlesFront[gId].vel += artVort*cross(normalize(curl),curl);
            }
            particlesFront[gId].vel += artVisc*velAccum;
            particlesFront[gId].pos  = pf.tempPos;
            break;
        }
    }
    groupMemoryBarrier();
    memoryBarrier();
}

Particle checkBBoxCollision(Particle p)
{
    if(dot((p.tempPos+p.displacement),vec3(0.0,1.0,0.0))+1.5f<0.0)
    {
        vec3 n1 = vec3(0.0,1.0,0.0);
        vec3 r = normalize((p.tempPos+p.displacement)-p.pos);
        float t = -(1.5f+dot(n1,-1.5*n1))/dot(n1,r);
        p.displacement = p.pos+(r*t)-p.tempPos;
    }
    if(dot((p.tempPos+p.displacement),vec3(0.0,-1.0,0.0))+1.5f<0.0)
    {
        vec3 n1 = vec3(0.0,-1.0,0.0);
        vec3 r = normalize((p.tempPos+p.displacement)-p.pos);
        float t = -(1.5+dot(n1,-1.5*n1))/dot(n1,r);
        p.displacement = p.pos+(r*t)-p.tempPos;
    }
    if(dot((p.tempPos+p.displacement),vec3(-1.0,0.0,0.0))+1.5f<0.0)
    {
        vec3 n1 = vec3(-1.0,0.0,0.0);
        vec3 r = normalize((p.tempPos+p.displacement)-p.pos);
        float t = -(1.5+dot(n1,-1.5*n1))/dot(n1,r);
        p.displacement = p.pos+(r*t)-p.tempPos;
    }
    if(dot((p.tempPos+p.displacement),vec3(1.0,0.0,0.0))+1.5f<0.0)
    {
        vec3 n1 = vec3(1.0,0.0,0.0);
        vec3 r = normalize((p.tempPos+p.displacement)-p.pos);
        float t = -(1.5+dot(n1,-1.5*n1))/dot(n1,r);
        p.displacement = p.pos+(r*t)-p.tempPos;
    }
    if(dot((p.tempPos+p.displacement),vec3(0.0,0.0,-1.0))+1.5f<0.0)
    {
        vec3 n1 = vec3(0.0,0.0,-1.0);
        vec3 r = normalize((p.tempPos+p.displacement)-p.pos);
        float t = -(1.5+dot(n1,-1.5*n1))/dot(n1,r);
        p.displacement = p.pos+(r*t)-p.tempPos;
    }
    if(dot((p.tempPos+p.displacement),vec3(0.0,0.0,1.0))+1.5f<0.0)
    {
        vec3 n1 = vec3(0.0,0.0,1.0);
        vec3 r = normalize((p.tempPos+p.displacement)-p.pos);
        float t = -(1.5+dot(n1,-1.5*n1))/dot(n1,r);
        p.displacement = p.pos+(r*t)-p.tempPos;
    }
    return p;
}

void neighborInteraction(uint gId, Particle p)
{
    int xPos = int(clamp(int((floor((minOfs.x+p.tempPos.x)/kernelSupport))),0,dimSize.x-1));
    int yPos = int(clamp(int((floor((minOfs.y+p.tempPos.y)/kernelSupport))),0,dimSize.y-1));
    int zPos = int(clamp(int((floor((minOfs.z+p.tempPos.z)/kernelSupport))),0,dimSize.z-1));

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
    endIdx[1] = histogram[bottomRightFront+1];

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
    endIdx[4] = histogram[topRightFront+1];
    uint topLeftCenter = xPosBegin+(dimSize.x*(yPosEnd+dimSize.y*zPos));
    uint topRightCenter = xPosEnd+(dimSize.x*(yPosEnd+dimSize.y*zPos));

    beginIdx[5] = histogram[topLeftCenter];
    endIdx[5] = histogram[topRightCenter+1];

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
        minDist = min(d,minDist);
        float corr = (minDist-t);
        vec3 tempDispl = p.pos+(t*v)-p.tempPos;
        particles[gId].displacement = tempDispl;
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
    return (315.0/(64*M_PI*pow(kernelSupport,9)))*pow((kernelSupport*kernelSupport-dot(r,r)),3);
}

float kernelSpikey(vec3 r)
{
    return (15.0/(M_PI*pow(kernelSupport,6)))*pow((kernelSupport-length(r)),3);
}

float kernelViscocity(vec3 r)
{
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
    return vec3(-(945*(r.x)*pow((-pow((r.x),2)+pow((r.y),2)+pow((r.z),2)+kernelSupport*kernelSupport),2))/(32*M_PI*pow(kernelSupport,9)),
                     -(945*(r.y)*pow((-pow((r.x),2)+pow((r.y),2)+pow((r.z),2)+kernelSupport*kernelSupport),2))/(32*M_PI*pow(kernelSupport,9)),
                     -(945*(r.z)*pow((-pow((r.x),2)+pow((r.y),2)+pow((r.z),2)+kernelSupport*kernelSupport),2))/(32*M_PI*pow(kernelSupport,9)));


}

vec3 gradSpikey(vec3 r)
{
    /*if(length(r)==0)
    {
        r = vec3(0.0,1.0,0.0);
    }*/
    return ((-45.0f/(M_PI*(pow(kernelSupport,6))))*(pow((kernelSupport-length(r)),2))*normalize(r));
}


vec3 gradViscocity(vec3 r)
{
    return -(pow(length(r),3)/(2*pow(kernelSupport,3)))-(pow(length(r),2)/(pow(kernelSupport,2)))+(kernelSupport/(2*length(r)))-1;
}
