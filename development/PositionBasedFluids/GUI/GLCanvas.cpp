#include"GLCanvas.h"
#include<QKeyEvent>
#include<QMouseEvent>

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

    std::vector<Particle> pTest;
    pTest.push_back(Particle(glm::vec3(0.0,0.0,0.0)));
    pTest.push_back(Particle(glm::vec3(-2.0,0.0,0.0)));
    pTest.push_back(Particle(glm::vec3(+2.0,0.0,0.0)));
    pTest.push_back(Particle(glm::vec3(0.0,2.0,0.0)));
    pTest.push_back(Particle(glm::vec3(-2.0,2.0,0.0)));
    pTest.push_back(Particle(glm::vec3(+2.0,2.0,0.0)));
    pTest.push_back(Particle(glm::vec3(0.0,-2.0,0.0)));
    pTest.push_back(Particle(glm::vec3(-2.0,-2.0,0.0)));
    pTest.push_back(Particle(glm::vec3(+2.0,-2.0,0.0)));

    std::cout<<sizeof(Vertex)<<std::endl;
    vbo = new VertexBuffer();
    vbo->bind();
    vbo->upload(test);

    //Enable Vertex Attrib Arrays
    Vertex::setVertexAttribs();
    Vertex::enableVertexAttribs();


    particles = new ParticleBuffer();
    particles->bind();
    particles->upload(pTest);

    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)sizeof(unsigned int));
    glEnableVertexAttribArray(2);

    glVertexAttribDivisor(0,0);
    glVertexAttribDivisor(1,0);
    glVertexAttribDivisor(2,1);
}

void GLCanvas::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    program->bind();
    view = camera.getView();
    glm::mat4 pvm = projection*view*model;
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(glm::mat3(view*model)));
    program->uploadMat4("pvm",pvm);
    program->uploadMat3("normalMatrix",normalMatrix);
    vbo->bind();
    glDrawArraysInstanced(GL_TRIANGLES,0,3,9);
}

void GLCanvas::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    projection = glm::perspectiveFov(45.0f,(float)w,(float)h,0.1f,100.0f);
}

void GLCanvas::mousePressEvent(QMouseEvent *event)
{
    std::cout<<"MOUSEPRESS"<<std::endl;
    switch(event->button())
    {
    case Qt::LeftButton:
        break;
    case Qt::MiddleButton:
        mouseCoords = event->pos();
        break;
    case Qt::RightButton:
        break;
    }
}

void GLCanvas::mouseMoveEvent(QMouseEvent *event)
{
    std::cout<<"MOUSEMOVE"<<std::endl;
    switch(event->buttons())
    {
    case Qt::LeftButton:
        break;
    case Qt::MiddleButton:
        camera.rotate(0.01*(event->pos().x()-mouseCoords.x()),camera.getUpVector());
        camera.rotate(0.01*(event->pos().y()-mouseCoords.y()),camera.getStrafeVec());
        mouseCoords = event->pos();
        break;
    case Qt::RightButton:
        break;
    }
}

void GLCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    std::cout<<"MOUSERELEASE"<<std::endl;
}

void GLCanvas::keyPressEvent(QKeyEvent *event)
{
    std::cout<<"KEYPRESS"<<std::endl;
    switch(event->key())
    {
    case Qt::Key_W:
        camera.translate(camera.getForwardVec());
        break;
    case Qt::Key_S:
        camera.translate(-camera.getForwardVec());
        break;
    case Qt::Key_D:
        camera.translate(camera.getStrafeVec());
        break;
    case Qt::Key_A:
        camera.translate(-camera.getStrafeVec());
        break;
    case Qt::Key_PageUp:
        camera.translate(camera.getUpVector());
        break;
    case Qt::Key_PageDown:
        camera.translate(-camera.getUpVector());
        break;
    case Qt::Key_Left:
        camera.rotate(0.1,camera.getUpVector());
        break;
    case Qt::Key_Right:
        camera.rotate(-0.1,camera.getUpVector());
        break;
    case Qt::Key_Up:
        camera.rotate(0.1,camera.getStrafeVec());
        break;
    case Qt::Key_Down:
        camera.rotate(-0.1,camera.getStrafeVec());
        break;
    }
}

void GLCanvas::keyReleaseEvent(QKeyEvent *event)
{
    std::cout<<"KEYRELEASE"<<std::endl;
}

