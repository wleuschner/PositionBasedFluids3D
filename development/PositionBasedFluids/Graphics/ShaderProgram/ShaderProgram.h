#ifndef __SHADER_PROGRAM_H
#define __SHADER_PROGRAM_H
#include<vector>
#include<string>
#include<glm/glm.hpp>
#include"../Shader/Shader.h"
#include"../Light/Light.h"

class ShaderProgram
{
public:
    ShaderProgram();
    void attachShader(const Shader& shader);
    void bind();
    bool link();
    std::string linkLog();

    //Attrib Access
    unsigned int getAttribLocation(const std::string& name);
    void setAttribute(unsigned int loc,unsigned int type,unsigned int offset,unsigned int n,unsigned int size);
    void setAttribute(const std::string& name,unsigned int type,unsigned int offset,unsigned int n,unsigned int size);
    void enableAttribute(unsigned int loc);
    void enableAttribute(const std::string& name);

    //Uniform Access
    void uploadUnsignedInt(const std::string& var,unsigned int val);
    void uploadScalar(const std::string& var,float val);
    void uploadVec2(const std::string& var,glm::vec2 val);
    void uploadVec3(const std::string& var,glm::vec3 val);
    void uploadVec4(const std::string& var,glm::vec4 val);
    void uploadMat2(const std::string& var,glm::mat2 val);
    void uploadMat3(const std::string& var,glm::mat3 val);
    void uploadMat4(const std::string& var,glm::mat4 val);
    void uploadLight(const std::string& var,const Light& val,const glm::mat4& view);

    //Compute Shader
    void dispatch(unsigned int wx,unsigned int wy,unsigned wz);
private:
    unsigned int id;
};

#endif
