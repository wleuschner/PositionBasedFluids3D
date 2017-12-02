#include"GLCanvas.h"
#include<glm/gtc/matrix_transform.hpp>

GLCanvas::GLCanvas(QWidget* parent) : QOpenGLWidget(parent)
{
}

void GLCanvas::initializeGL()
{
    glClearColor(0.0,0.0,0.0,1.0);
    glClearDepth(1.0);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

}

void GLCanvas::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void GLCanvas::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    projection = glm::perspectiveFov(45.0f,(float)w,(float)h,0.1f,100.0f);
}
