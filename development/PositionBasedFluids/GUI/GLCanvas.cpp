#include"GLCanvas.h"
#include<QKeyEvent>
#include<QMouseEvent>

#include<glm/gtc/matrix_transform.hpp>
#include<iostream>

#include"../Solver/SolverImpl/PBFSolver.h"
#include"../Solver/Kernel/KernelImpl/Poly6Kernel.h"
#include"../Solver/Kernel/KernelImpl/SpikyKernel.h"

GLCanvas::GLCanvas(QWidget* parent) : QOpenGLWidget(parent)
{
    screenshotNo = 0;
    running = false;
    format = QSurfaceFormat::defaultFormat();
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setMajorVersion(4);
    format.setMinorVersion(2);
    setFormat(format);

    connect(&updateTimer,SIGNAL(timeout()),this,SLOT(simulate()));
    //connect(&updateTimer,SIGNAL(timeout()),this,SLOT(update()));
    updateTimer.setInterval(1000.0/30);
    updateTimer.setSingleShot(false);
    updateTimer.start();

    Poly6Kernel* densityKernel = new Poly6Kernel(1.5f);
    SpikyKernel* gradKernel = new SpikyKernel(1.5f);
    PBFSolver* pbf = new PBFSolver((AbstractKernel*)densityKernel,(AbstractKernel*)gradKernel,(AbstractKernel*)gradKernel,0.08,4);
    solver = (AbstractSolver*)pbf;
}

void GLCanvas::simulate()
{
    updateTimer.stop();
    if(running)
    {
        solver->solve(particles->getParticles());
        particles->bind();
        particles->upload();
        QImage screenshot = grabFramebuffer();
        screenshot.save(QString("Screenshot")+QString::number(screenshotNo)+QString(".png"),"png");
        screenshotNo++;

    }
    update();
    updateTimer.start();
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

    Vertex a1,a2,a3,a4,a5,a6;
    a1.pos = 0.1f*glm::vec3(0.0f,1.0f,0.0f);
    a2.pos = 0.1f*glm::vec3(1.0f,0.0f,0.0f);
    a3.pos = 0.1f*glm::vec3(-1.0f,0.0f,0.0f);
    a4.pos = 0.1f*glm::vec3(0.0f,-1.0f,0.0f);
    a5.pos = 0.1f*glm::vec3(1.0f,0.0f,0.0f);
    a6.pos = 0.1f*glm::vec3(-1.0f,0.0f,0.0f);
    std::vector<Vertex> test;
    test.push_back(a1);
    test.push_back(a2);
    test.push_back(a3);
    test.push_back(a4);
    test.push_back(a5);
    test.push_back(a6);

    std::cout<<sizeof(Vertex)<<std::endl;
    vbo = new VertexBuffer();
    vbo->bind();
    vbo->upload(test);

    //Enable Vertex Attrib Arrays
    Vertex::setVertexAttribs();
    Vertex::enableVertexAttribs();


    particles = new ParticleBuffer();
    particles->bind();
    unsigned int cc = 0;
    for( int z=-5;z!=6;z++)
    {
        for( int y=-5;y!=6;y++)
        {
            for( int x=-5;x!=6;x++)
            {
                /*particles->addParticle(Particle(cc,glm::vec3(x/10.0,y/10.0,z/10.0),glm::vec3(0.0,0.0,0.0),1.0,1.0));
                cc++;*/
                //particles->addParticle(Particle(cc,glm::vec3(x/5.0-10,y/5.0,z/5.0),glm::vec3(0.0,0.0,0.0),1.0,1.0));
                //cc++;
                particles->addParticle(Particle(cc,glm::vec3(-0.5+((x+5)/10.0),-0.5+((y+5)/10.0),-0.5+((z+5)/10.0)),glm::vec3(0.0,0.0,0.0),1.0,1.0));
                cc++;
            }
        }
    }
    particles->upload();

    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)sizeof(unsigned int));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)36);
    glEnableVertexAttribArray(3);

    glVertexAttribDivisor(0,0);
    glVertexAttribDivisor(1,0);
    glVertexAttribDivisor(2,1);
    glVertexAttribDivisor(3,1);
    //solver->init(particles->getParticles());

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
    glDrawArraysInstanced(GL_TRIANGLES,0,6,particles->getNumParticles());
}

void GLCanvas::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    projection = glm::perspectiveFov(45.0f,(float)w,(float)h,0.1f,100.0f);
}

void GLCanvas::mousePressEvent(QMouseEvent *event)
{
    switch(event->button())
    {
    case Qt::LeftButton:
        /*particles->addParticle(Particle(camera.getPosition()));
        particles->bind();
        particles->upload();*/
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
    case Qt::Key_Space:
        if(!running)
        {
            screenshotNo = 0;
            running = true;
        }
        else
        {
            running = false;
        }
        //simulate();
        break;
    }
}

void GLCanvas::keyReleaseEvent(QKeyEvent *event)
{
    std::cout<<"KEYRELEASE"<<std::endl;
}

void GLCanvas::setNumIterations(int val)
{

}

void GLCanvas::setKernelSupport(double val)
{

}

void GLCanvas::setRestDensity(double val)
{

}

void GLCanvas::setArtVisc(double val)
{

}

void GLCanvas::setArtVort(double val)
{

}

void GLCanvas::setCfmRegularization(double val)
{

}

void GLCanvas::setCorrConst(double val)
{

}

void GLCanvas::setCorrDist(double val)
{

}

void GLCanvas::setCorrExp(double val)
{

}
