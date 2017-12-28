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

    //AttribAccess
    unsigned int getAttribLocation(const std::string& name);

    //Uniform Access
    void uploadScalar(const std::string& var,float val);
    void uploadVec2(const std::string& var,glm::vec2 val);
    void uploadVec3(const std::string& var,glm::vec3 val);
    void uploadVec4(const std::string& var,glm::vec4 val);
    void uploadMat2(const std::string& var,glm::mat2 val);
    void uploadMat3(const std::string& var,glm::mat3 val);
    void uploadMat4(const std::string& var,glm::mat4 val);
    void uploadLight(const std::string& var,const Light& val);

private:
    unsigned int id;
};

#endif
