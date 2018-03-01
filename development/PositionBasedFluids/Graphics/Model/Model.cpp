#include"Model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <cmath>
#include <iostream>
#include "../FrameBufferObject/FrameBufferObject.h"
ShaderProgram* Model::voxelProgram = NULL;

Model::Model()
{
    this->modelMat = glm::mat4();
    if(voxelProgram==NULL)
    {
        voxelProgram = new ShaderProgram();
        Shader vert(GL_VERTEX_SHADER,"Resources/Voxel/voxel.vert");
        if(!vert.compile())
        {
            std::cout<<vert.compileLog()<<std::endl;
        }
        Shader frag(GL_FRAGMENT_SHADER,"Resources/Voxel/voxel.frag");
        if(!frag.compile())
        {
            std::cout<<frag.compileLog()<<std::endl;
        }
        Shader geom(GL_GEOMETRY_SHADER,"Resources/Voxel/voxel.geom");
        if(!geom.compile())
        {
            std::cout<<geom.compileLog()<<std::endl;
        }
        voxelProgram->attachShader(vert);
        voxelProgram->attachShader(geom);
        voxelProgram->attachShader(frag);
        if(!voxelProgram->link())
        {
            std::cout<<voxelProgram->linkLog()<<std::endl;
        }
        voxelProgram->bind();
    }
}

Model::~Model()
{
}

bool Model::load(std::string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,aiProcessPreset_TargetRealtime_Quality);
    for(unsigned int j=0;j</*scene->mNumMeshes*/1;j++)
    {
        const aiMesh* mesh = scene->mMeshes[j];
        aiColor3D mat_ambient;
        aiColor3D mat_diffuse;
        aiColor3D mat_specular;

        int mat_index = mesh->mMaterialIndex;
        const aiMaterial* mat = scene->mMaterials[mat_index];

        float mat_shininess;

        mat->Get(AI_MATKEY_COLOR_AMBIENT,mat_ambient);
        mat->Get(AI_MATKEY_COLOR_DIFFUSE,mat_diffuse);
        mat->Get(AI_MATKEY_COLOR_SPECULAR,mat_specular);
        mat->Get(AI_MATKEY_SHININESS,mat_shininess);
        position.reserve(mesh->mNumVertices);
        for(unsigned int i=0;i<mesh->mNumVertices;i++)
        {
            aiVector3D v = mesh->mVertices[i];
            glm::vec3 pos = glm::vec3(v.x,v.y,v.z);
            aabb.max.x = glm::max(pos.x,aabb.max.x);
            aabb.max.y = glm::max(pos.y,aabb.max.y);
            aabb.max.z = glm::max(pos.z,aabb.max.z);
            aabb.min.x = glm::min(pos.x,aabb.min.x);
            aabb.min.y = glm::min(pos.y,aabb.min.y);
            aabb.min.z = glm::min(pos.z,aabb.min.z);
            position.push_back(pos);
            if(mesh->HasNormals())
            {
                aiVector3D n = mesh->mNormals[i];
                normal.push_back(glm::vec3(n.x,n.y,n.z));
            }
            //if(mesh->HasTextureCoords())
            //{
            //    aiVector2D uv = mesh->mTextureCoords[i];
            //    uv_coords.push_back(glm::vec2(uv[0],uv[1]));
            //}
            Vertex vert;
            vert.pos = position[i];
            vert.normal = normal[i];
            vert.uv = glm::vec2(0,0);
            vertices.push_back(vert);
        }
        for(int i=0;i<mesh->mNumFaces;i++)
        {
            indices.push_back(mesh->mFaces[i].mIndices[0]);
            indices.push_back(mesh->mFaces[i].mIndices[1]);
            indices.push_back(mesh->mFaces[i].mIndices[2]);
        }
    }
    createVBO();
    createIndex();
    return true;
}

bool Model::release()
{
    delete vbo;
    delete index;
    indices.clear();
    vertices.clear();
    uv_coords.clear();
    normal.clear();
    return true;
}

void Model::bind()
{
    vbo->bind();
    index->bind();
}

void Model::update()
{
    vbo->bind();
    vbo->upload(vertices);
    bind();
}

std::vector<Vertex>& Model::getVertices()
{
    return vertices;
}

std::vector<unsigned int>& Model::getIndices()
{
    return indices;
}

void Model::setModelMat(const glm::mat4& mat)
{
    this->modelMat = mat;
}

const glm::mat4& Model::getModelMat()
{
    return this->modelMat;
}

bool Model::createVBO()
{
    vbo = new VertexBuffer();
    vbo->bind();
    vbo->upload(vertices);
    return true;
}

bool Model::createIndex()
{
    index = new IndexBuffer();
    index->bind();
    index->upload(indices);
    return true;
}

Material Model::getMaterial() const
{
    return material;
}

void Model::setMaterial(const Material &value)
{
    material = value;
}

void Model::draw(ShaderProgram* shader)
{
    bind();

    glm::vec3 ambient = material.getAmbient();
    glm::vec3 diffuse = material.getDiffuse();
    glm::vec3 specular = material.getSpecular();
    float shininess = material.getShininess();

    //shader->uploadVec3("material.amb",ambient);
    //shader->uploadVec3("material.dif",diffuse);
    //shader->uploadVec3("material.spec",specular);
    //shader->uploadScalar("material.shininess",shininess);


    glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,(void*)0);
}

ParticleBuffer* Model::voxelize(float particleSize,bool solid)
{
    glm::vec3 ext = aabb.getExtent();

    unsigned int layers = ceil(ext.z/(particleSize*2*32*4));
    unsigned int width = ceil(ext.x/(particleSize*2));
    unsigned int height = ceil(ext.y/(particleSize*2));

    glm::mat4 orthProj = glm::ortho(-ext.x/2,ext.x/2,-ext.y/2,ext.y/2);
    glm::mat4 pv = orthProj*glm::translate(glm::mat4(),glm::vec3(-aabb.getCenter().x,-aabb.getCenter().y,aabb.min.z));


    FrameBufferObject fbo;
    TextureArray texArray;
    texArray.bind(0);
    /*ext.x/particleSize,ext.y/particleSize,((ext.z/32)/particleSize)*/
    texArray.createRenderArray(width,height,layers);
    texArray.unbind(0);
    //texArray.createRenderArray(10,10,512);

    fbo.bind();
    fbo.attachColorArray(texArray,0);
    //fbo.attachDepthArray(depthArray);
    fbo.setRenderBuffer({GL_COLOR_ATTACHMENT0});
    if(!fbo.isComplete())
    {
        std::cout<<"FBO incomplete"<<std::endl;
    }
    fbo.bind();
    bind();
    Vertex::setVertexAttribs();
    Vertex::enableVertexAttribs();
    voxelProgram->bind();
    voxelProgram->uploadMat4("projection",pv);
    voxelProgram->uploadScalar("particleSize",particleSize);
    texArray.bind(0);

    glViewport(0,0,width,height);
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_COLOR_LOGIC_OP);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glLogicOp(GL_XOR);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,(void*)0);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_LOGIC_OP);
    glDisable(GL_DEPTH_CLAMP);
    glFlush();
    fbo.unbind();

    //Read Voxels from Texture Array
    unsigned int* buffer = new unsigned int[4*width*height*layers];
    //glReadBuffer(GL_COLOR_ATTACHMENT0);
    //glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_INT,buffer);
    texArray.bind(0);
    glGetTexImage(GL_TEXTURE_2D_ARRAY,0,GL_RGBA_INTEGER,GL_UNSIGNED_INT,buffer);
    ParticleBuffer* voxelParticles = new ParticleBuffer();
    voxelParticles->bind();

    unsigned int part=0;
    for(unsigned int l=0;l<layers;l++)
    {
        //glGetTextureSubImage(GL_TEXTURE_2D_ARRAY,0,0,0,l,width,height,0,GL_RGBA,GL_UNSIGNED_INT,4*width*height*layers,buffer);
        for(unsigned int y=0;y<height;y++)
        {
            for(unsigned int x=0;x<width;x++)
            {
                //std::cout<<"("<<(unsigned int)buffer[(x*4)+width*4*y+(width*4*height)*l]<<","<<(unsigned int)buffer[1+(x*4)+width*4*y+(width*4*height)*l]<<","<<(unsigned int)buffer[2+(x*4)+width*4*y+(width*4*height)*l]<<","<<(unsigned int)buffer[3+(x*4)+width*4*y+(width*4*height)*l]<<")"<<std::endl;
                //Red Channel
                for(unsigned int d=0;d<32;d++)
                {
                    if(buffer[(x*4)+width*4*y+(width*4*height)*l]&1)
                    {
                        float xp = aabb.min.x+x*particleSize*2;
                        float yp = aabb.min.y+y*particleSize*2;
                        float zp = aabb.min.z+l*32*4*particleSize+d*particleSize*2;
                        voxelParticles->addParticle(Particle(part,modelMat*glm::vec4((glm::vec3(xp,yp,zp)-glm::vec3(-aabb.getCenter().x,-aabb.getCenter().y,aabb.min.z+ext.z/2)),1.0),glm::vec3(0.0,0.0,0.0),1.0,0.0,solid));
                        part++;
                    }
                    buffer[(x*4)+width*4*y+(width*4*height)*l]>>=1;
                }
                //Green Channel
                for(unsigned int d=0;d<32;d++)
                {

                    if(buffer[1+(x*4)+width*4*y+(width*4*height)*l]&1)
                    {
                        float xp = aabb.min.x+x*particleSize*2;
                        float yp = aabb.min.y+y*particleSize*2;
                        float zp = aabb.min.z+l*32*4*particleSize+(d+32)*particleSize*2;
                        voxelParticles->addParticle(Particle(part,modelMat*glm::vec4((glm::vec3(xp,yp,zp)-glm::vec3(-aabb.getCenter().x,-aabb.getCenter().y,aabb.min.z+ext.z/2)),1.0),glm::vec3(0.0,0.0,0.0),1.0,0.0,solid));
                        part++;
                    }
                    buffer[1+(x*4)+width*4*y+(width*4*height)*l]>>=1;
                }
                //Blue Channel
                for(unsigned int d=0;d<32;d++)
                {

                    if(buffer[2+(x*4)+width*4*y+(width*4*height)*l]&1)
                    {
                        float xp = aabb.min.x+x*particleSize*2;
                        float yp = aabb.min.y+y*particleSize*2;
                        float zp = aabb.min.z+l*32*4*particleSize+(d+64)*particleSize*2;
                        voxelParticles->addParticle(Particle(part,modelMat*glm::vec4((glm::vec3(xp,yp,zp)-glm::vec3(-aabb.getCenter().x,-aabb.getCenter().y,aabb.min.z+ext.z/2)),1.0),glm::vec3(0.0,0.0,0.0),1.0,0.0,solid));
                        part++;
                    }
                    buffer[2+(x*4)+width*4*y+(width*4*height)*l]>>=1;
                }
                //Alpha Channel
                for(unsigned int d=0;d<32;d++)
                {

                    if(buffer[3+(x*4)+width*4*y+(width*4*height)*l]&1)
                    {
                        float xp = aabb.min.x+x*particleSize*2;
                        float yp = aabb.min.y+y*particleSize*2;
                        float zp = aabb.min.z+l*32*4*particleSize+(d+96)*particleSize*2;
                        voxelParticles->addParticle(Particle(part,modelMat*glm::vec4((glm::vec3(xp,yp,zp)-glm::vec3(-aabb.getCenter().x,-aabb.getCenter().y,aabb.min.z+ext.z/2)),1.0),glm::vec3(0.0,0.0,0.0),1.0,0.0,solid));
                        part++;
                    }
                    buffer[3+(x*4)+width*4*y+(width*4*height)*l]>>=1;
                }
            }
        }
    }
    voxelParticles->upload();
    texArray.unbind(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    //std::cout<<"---------------------------------------------------------------------"<<std::endl;
    delete[] buffer;
    return voxelParticles;
}

Model* Model::createSphere(float radius,int stacks,int slices)
{
    Model *model = new Model();
    float angle1_inc = (2*M_PI)/(slices-1);
    float angle2_inc = (M_PI)/(stacks-1);
    float angle1 = 0.0;
    for(int y=0;y<stacks;y++,angle1+=angle1_inc)
    {
        float angle2 = 0.0;
        float a1_sinf = sinf(angle1);
        float a1_cosf = cosf(angle1);
        for(int x=0;x<slices;x++,angle2+=angle2_inc)
        {
            model->position.push_back(glm::vec3(radius*a1_cosf*sinf(angle2),radius*cosf(angle2),radius*a1_sinf*sinf(angle2)));
            model->normal.push_back(glm::vec3(a1_cosf*sinf(angle2),cosf(angle2),a1_sinf*sinf(angle2)));
            model->uv_coords.push_back(glm::vec2(x/((float)slices),y/((float)stacks)));
        }
    }
    for(int i=0;i<model->position.size();i++)
    {
        Vertex v;
        v.pos = model->position[i];
        v.normal = model->normal[i];
        v.uv = model->uv_coords[i];

        model->vertices.push_back(v);
    }
    for(int y=0;y<stacks-1;y++)
    {
        int x;
        for(x=0;x<slices-1;x++)
        {
            model->indices.push_back(y*slices+x);
            model->indices.push_back((y+1)*slices+x);
            model->indices.push_back(y*slices+(x+1));

            model->indices.push_back((y+1)*slices+x);
            model->indices.push_back((y+1)*slices+(x+1));
            model->indices.push_back(y*slices+(x+1));

        }

        //Face 1
        model->indices.push_back(y*slices+x);
        model->indices.push_back((y+1)*slices+x);
        model->indices.push_back(y*slices);

        //Face 2
        model->indices.push_back((y+1)*slices+x);
        model->indices.push_back((y+1)*slices);
        model->indices.push_back(y*slices);

    }
    model->createVBO();
    model->createIndex();
    return model;
}
