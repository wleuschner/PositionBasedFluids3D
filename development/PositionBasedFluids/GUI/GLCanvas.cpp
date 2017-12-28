#include"GLCanvas.h"

#include<glm/gtc/matrix_transform.hpp>
#include<iostream>

GLCanvas::GLCanvas(QWidget* parent) : QOpenGLWidget(parent)
{
    format = QSurfaceFormat::defaultFormat();
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setMajorVersion(4);
    format.setMinorVersion(2);
    setFormat(format);
}

void GLCanvas::initializeGL()
{
    glewInit();

    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    glClearColor(0.0,0.0,0.0,1.0);
    glClearDepth(1.0);
    glDepthFunc(GL_LEQUAL);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);


    glEnableClientState(GL_VERTEX_ARRAY);

    Shader vert(GL_VERTEX_SHADER,"Resources/shader.vert");
    if(!vert.compile())
    {
        std::cout<<vert.compileLog().c_str()<<std::endl;
    }

    Shader frag(GL_FRAGMENT_SHADER,"Resources/shader.frag");
    if(!frag.compile())
    {
        std::cout<<frag.compileLog().c_str()<<std::endl;
    }

    program = new ShaderProgram();
    program->attachShader(vert);
    program->attachShader(frag);
    if(!program->link())
    {
        std::cout<<program->linkLog().c_str()<<std::endl;
    }
    program->bind();

    Vertex a1,a2,a3;
    a1.pos = glm::vec3(0.0f,0.5f,-30.0f);
    a2.pos = glm::vec3(1.0f,-0.5f,-30.0f);
    a3.pos = glm::vec3(-1.0f,-0.5f,-30.0f);
    std::vector<Vertex> test;
    test.push_back(a1);
    test.push_back(a2);
    test.push_back(a3);

    std::vector<glm::vec3> pTest;
    pTest.push_back(glm::vec3(0.0,0.0,0.0));
    pTest.push_back(glm::vec3(-2.0,0.0,0.0));
    pTest.push_back(glm::vec3(+2.0,0.0,0.0));
    pTest.push_back(glm::vec3(0.0,2.0,0.0));
    pTest.push_back(glm::vec3(-2.0,2.0,0.0));
    pTest.push_back(glm::vec3(+2.0,2.0,0.0));

    std::cout<<sizeof(Vertex)<<std::endl;
    vbo = new VertexBuffer();
    vbo->bind();
    vbo->upload(test);

    //Enable Vertex Attrib Arrays
    Vertex::setVertexAttribs();
    Vertex::enableVertexAttribs();

    glGenBuffers(1,&particles);
    glBindBuffer(GL_ARRAY_BUFFER,particles);
    glBufferData(GL_ARRAY_BUFFER,pTest.size()*sizeof(glm::vec3),(void*)pTest.data(),GL_STREAM_DRAW);

    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,12,(void*)0);
    glEnableVertexAttribArray(2);

    glVertexAttribDivisor(0,0);
    glVertexAttribDivisor(1,0);
    glVertexAttribDivisor(2,1);
}

void GLCanvas::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    program->bind();
    glm::mat4 pvm = projection*view*model;
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(glm::mat3(view*model)));
    program->uploadMat4("pvm",pvm);
    program->uploadMat3("normalMatrix",normalMatrix);
    vbo->bind();
    glDrawArraysInstanced(GL_TRIANGLES,0,3,6);
}

void GLCanvas::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    projection = glm::perspectiveFov(45.0f,(float)w,(float)h,0.1f,100.0f);
}
