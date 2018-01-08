#include"GLCanvas.h"
#include<QKeyEvent>
#include<QMouseEvent>

#include<glm/gtc/matrix_transform.hpp>
#include<iostream>

#include"../Solver/SolverImpl/PBFSolver.h"
#include"../Solver/SolverImpl/PBFSolverGPU.h"
#include"../Solver/Kernel/KernelImpl/Poly6Kernel.h"
#include"../Solver/Kernel/KernelImpl/SpikyKernel.h"
#include"../Solver/Kernel/KernelImpl/ViscocityKernel.h"

GLCanvas::GLCanvas(QWidget* parent) : QOpenGLWidget(parent)
{
    particleSize = 0.005;
    screenshotNo = 0;
    running = false;
    step = false;
    record = false;
    gpu = false;
    format = QSurfaceFormat::defaultFormat();
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setMajorVersion(4);
    format.setMinorVersion(4);
    setFormat(format);


    connect(&updateTimer,SIGNAL(timeout()),this,SLOT(simulate()));
    //connect(&updateTimer,SIGNAL(timeout()),this,SLOT(update()));
}

void GLCanvas::simulate()
{
    updateTimer.stop();
    if(running||step)
    {
        solver->solve();
        particles->bind();
        if(!gpu)
        {
            particles->upload();
        }

        if(record)
        {
            QImage screenshot = grabFramebuffer();
            screenshot.save(QString("Screenshot")+QString::number(screenshotNo)+QString(".png"),"png");
            screenshotNo++;
        }
        step = false;
    }
    update();
    updateTimer.start();
}

void GLCanvas::initializeGL()
{
    glewInit();

    updateTimer.setInterval(1000.0/60);
    updateTimer.setSingleShot(false);
    updateTimer.start();

    Poly6Kernel* densityKernel = new Poly6Kernel(0.5f);
    SpikyKernel* gradKernel = new SpikyKernel(0.5f);
    ViscocityKernel* viscKernel = new ViscocityKernel(0.5f);

    kernels.push_back((AbstractKernel*)densityKernel);
    kernels.push_back((AbstractKernel*)gradKernel);
    kernels.push_back((AbstractKernel*)viscKernel);

    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    glClearColor(0.0,0.0,0.0,1.0);
    glClearDepth(1.0);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    //glDisable(GL_CULL_FACE);


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

    //Create Light
    light = Light(glm::vec3(-10.0,10.0,5.0));

    //Create Sphere Model
    sphere = new Model();
    //sphere->load("Resources/sphere.obj");
    sphere = Model::createSphere(1.0,16,16);

    sphere->bind();

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

                particles->addParticle(Particle(cc,glm::vec3(-0.5+((x+5.0)/10.0),-0.5+((y+5.0)/10.0),-0.5+((z+5.0)/10.0)),glm::vec3(0.0,0.0,0.0),1.0,0.0));
                cc++;
            }
        }
    }
    particles->upload();
    pbf = new PBFSolver(particles->getParticles(),(AbstractKernel*)densityKernel,(AbstractKernel*)gradKernel,(AbstractKernel*)viscKernel,0.08,4);
    pbfGpu = new PBFSolverGPU(particles->getParticles(),(AbstractKernel*)densityKernel,(AbstractKernel*)gradKernel,(AbstractKernel*)gradKernel,0.08,4);
    solver = (AbstractSolver*)pbf;
    //particles->addParticle(Particle(0,glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0),1.0,1.0));
}

void GLCanvas::paintGL()
{
    Vertex::enableVertexAttribs();
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)32);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3,1,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)16);
    glEnableVertexAttribArray(3);

    glVertexAttribDivisor(0,0);
    glVertexAttribDivisor(1,0);
    glVertexAttribDivisor(2,1);
    glVertexAttribDivisor(3,1);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    program->bind();
    view = camera.getView();
    glm::mat4 modelView = view*model;
    glm::mat4 pvm = projection*modelView;
    glm::mat4 normalMatrix = glm::mat3(glm::transpose(glm::inverse((view*model))));
    program->uploadScalar("particleSize",particleSize);
    program->uploadMat4("modelView",modelView);
    program->uploadMat4("pvm",pvm);
    program->uploadMat4("view",view);
    program->uploadMat3("normalMatrix",normalMatrix);
    program->uploadVec3("cPos",camera.getPosition());
    program->uploadLight("light0",light,view);
    glDrawElementsInstanced(GL_TRIANGLES,sphere->getIndices().size(),GL_UNSIGNED_INT,0,particles->getNumParticles());
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
    case Qt::Key_V:
        if(!record)
        {
            screenshotNo = 0;
            record = true;
        }
        else
        {
            record = false;
        }
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
    case Qt::Key_Plus:
        std::cout<<"STEP"<<std::endl;
        step = true;
        updateTimer.stop();
        simulate();
        break;
    }
}

void GLCanvas::keyReleaseEvent(QKeyEvent *event)
{
    std::cout<<"KEYRELEASE"<<std::endl;
}

void GLCanvas::setNumIterations(int val)
{
    solver->setNumIterations(val);
}

void GLCanvas::setTimestep(double val)
{
    solver->setTimestep(val);
}

void GLCanvas::setKernelSupport(double val)
{
    solver->setKernelSupport(val);
}

void GLCanvas::setParticleSize(double val)
{
    solver->setPartSize(val);
    this->particleSize = val;
}

void GLCanvas::setRestDensity(double val)
{
    solver->setRestDensity(val);
}

void GLCanvas::setArtVisc(double val)
{
    solver->setArtVisc(val);
}

void GLCanvas::setArtVort(double val)
{
    solver->setArtVort(val);
}

void GLCanvas::setCfmRegularization(double val)
{
    solver->setCfmRegularization(val);
}

void GLCanvas::setCorrConst(double val)
{
    solver->setCorrConst(val);
}

void GLCanvas::setCorrDist(double val)
{
    solver->setCorrDist(val);
}

void GLCanvas::setCorrExp(double val)
{
    solver->setCorrExp(val);
}

void GLCanvas::setDensityKernel(int index)
{
    solver->setDensityKernel(index);
}

void GLCanvas::setGradKernel(int index)
{
    solver->setDensityKernel(index);
}

void GLCanvas::setViscKernel(int index)
{
    solver->setViscKernel(index);
}

void GLCanvas::setGPU(int state)
{
    if(state)
    {
        this->gpu = true;
        solver = (AbstractSolver*)pbfGpu;
    }
    else
    {
        this->gpu = false;
        solver = (AbstractSolver*)pbf;
    }
}
