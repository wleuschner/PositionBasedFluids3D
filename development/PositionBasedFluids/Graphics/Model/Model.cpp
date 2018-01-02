#include"Model.h"
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <cmath>

Model::Model()
{
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
    shader->setAttribute("vertex",GL_FLOAT,0,3,sizeof(Vertex));
    shader->enableAttribute("vertex");

    shader->setAttribute("normal",GL_FLOAT,sizeof(float)*3,3,sizeof(Vertex));
    shader->enableAttribute("normal");

    shader->setAttribute("uv",GL_FLOAT,sizeof(float)*6,2,sizeof(Vertex));
    shader->enableAttribute("uv");

    glm::vec3 ambient = material.getAmbient();
    glm::vec3 diffuse = material.getDiffuse();
    glm::vec3 specular = material.getSpecular();
    float shininess = material.getShininess();

    shader->uploadVec3("material.amb",ambient);
    shader->uploadVec3("material.dif",diffuse);
    shader->uploadVec3("material.spec",specular);
    shader->uploadScalar("material.shininess",shininess);


    glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,(void*)0);
}

Model* Model::createSphere(float radius,int stacks,int slices)
{
    Model *model = new Model();
    float angle1_inc = (M_PI)/(stacks-1);
    float angle2_inc = (2*M_PI)/(slices-1);
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
