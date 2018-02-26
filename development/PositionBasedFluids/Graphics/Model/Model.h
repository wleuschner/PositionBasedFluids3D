#ifndef MODEL_H
#define MODEL_H
#include<string>
#include<vector>
#include<map>
#include<glm/glm.hpp>
#include"../../Graphics/Shader/Shader.h"
#include"../../Graphics/ShaderProgram/ShaderProgram.h"
#include"../../Graphics/VertexBuffer/VertexBuffer.h"
#include"../../Graphics/IndexBuffer/IndexBuffer.h"
#include"../../Graphics/Material/Material.h"
#include"../../Graphics/AABB/AABB.h"
#include"../../Graphics/ParticleBuffer/ParticleBuffer.h"

class Model
{
public:
    Model();
    ~Model();
    bool load(std::string path);
    bool release();
    void bind();
    void draw(ShaderProgram* shader);

    ParticleBuffer* voxelize(float particleSize,bool solid);

    Material getMaterial() const;
    void setMaterial(const Material &value);
    std::vector<Vertex>& getVertices();
    std::vector<unsigned int>& getIndices();

    void setModelMat(const glm::mat4& mat);
    const glm::mat4& getModelMat();


    //Primitives
    static Model* createPlaneXZ(float width,float height,int xPatches,int zPatches);
    static Model* createPlaneXY(float width,float height,int xPatches,int yPatches);
    static Model* createPlaneYZ(float width,float height,int yPatches,int zPatches);
    static Model* createCylinder(float radius,int stacks,int slices);
    static Model* createSphere(float radius,int stacks,int slices);
    static Model* createTorus(float inner,float outer,int stacks,int slices);

    void update();

private:
    bool createVBO();
    bool createIndex();

    std::string name;

    static ShaderProgram* voxelProgram;

    AABB aabb;
    glm::mat4 modelMat;

    Material material;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> position;
    std::vector<glm::vec3> normal;
    std::vector<glm::vec2> uv_coords;

    std::vector<Vertex> vertices;

    VertexBuffer* vbo;
    IndexBuffer* index;

};
#endif
