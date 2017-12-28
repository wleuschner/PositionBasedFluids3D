#include"ShaderProgram.h"
#include<GL/glew.h>
#include<glm/gtc/type_ptr.hpp>

ShaderProgram::ShaderProgram()
{
    id = glCreateProgram();
}

void ShaderProgram::attachShader(const Shader &shader)
{
    glAttachShader(id,shader.id);
}

bool ShaderProgram::link()
{
    glLinkProgram(id);
    GLint linked = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &linked);
    return linked==GL_TRUE;
}

void ShaderProgram::bind()
{
    glUseProgram(id);
}

std::string ShaderProgram::linkLog()
{
    GLint maxLength = 0;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);

    //The maxLength includes the NULL character
    std::vector<GLchar> infoLog(maxLength);
    glGetProgramInfoLog(id, maxLength, &maxLength, &infoLog[0]);
    std::string ret(infoLog.begin(),infoLog.end());
    return ret;
}

unsigned int ShaderProgram::getAttribLocation(const std::string& name)
{
    return glGetAttribLocation(id,name.c_str());
}

void ShaderProgram::uploadScalar(const std::string& var,float val)
{
    unsigned int loc = glGetUniformLocation(id,var.c_str());
    glUniform1f(loc,val);
}

void ShaderProgram::uploadVec2(const std::string& var,glm::vec2 val)
{
    unsigned int loc = glGetUniformLocation(id,var.c_str());
    glUniform2fv(loc,1,glm::value_ptr(val));
}

void ShaderProgram::uploadVec3(const std::string& var,glm::vec3 val)
{
    unsigned int loc = glGetUniformLocation(id,var.c_str());
    glUniform3fv(loc,1,glm::value_ptr(val));
}

void ShaderProgram::uploadVec4(const std::string& var,glm::vec4 val)
{
    unsigned int loc = glGetUniformLocation(id,var.c_str());
    glUniform4fv(loc,1,glm::value_ptr(val));
}

void ShaderProgram::uploadMat2(const std::string& var,glm::mat2 val)
{
    unsigned int loc = glGetUniformLocation(id,var.c_str());
    glUniformMatrix2fv(loc,1,GL_FALSE,glm::value_ptr(val));
}

void ShaderProgram::uploadMat3(const std::string& var,glm::mat3 val)
{
    unsigned int loc = glGetUniformLocation(id,var.c_str());
    glUniformMatrix3fv(loc,1,GL_FALSE,glm::value_ptr(val));
}

void ShaderProgram::uploadMat4(const std::string& var,glm::mat4 val)
{
    unsigned int loc = glGetUniformLocation(id,var.c_str());
    glUniformMatrix4fv(loc,1,GL_FALSE,glm::value_ptr(val));
}

void ShaderProgram::uploadLight(const std::string& var,const Light& val)
{
    unsigned int loc = glGetUniformBlockIndex(id,"Light[0]");
   // glUniformBlockBinding(id,val)
}
