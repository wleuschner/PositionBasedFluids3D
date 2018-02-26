#include"GLCanvas.h"
#include<QKeyEvent>
#include<QMouseEvent>
#include<QOpenGLFramebufferObject>

#include<glm/gtc/matrix_transform.hpp>
#include<iostream>

#include"../Solver/SolverImpl/PBFSolver.h"
#include"../Solver/SolverImpl/PBFSolverGPU.h"
#include"../Solver/Kernel/KernelImpl/Poly6Kernel.h"
#include"../Solver/Kernel/KernelImpl/SpikyKernel.h"
#include"../Solver/Kernel/KernelImpl/ViscocityKernel.h"
#include"../Graphics/Texture/Texture.h"
#include"../Graphics/FrameBufferObject/FrameBufferObject.h"

#include"PlacementDialog.h"

GLCanvas::GLCanvas(QWidget* parent) : QOpenGLWidget(parent)
{
    surface = false;
    smoothIter = 1;
    particleSize = 0.03;
    //particleSize = 0.005;
    screenshotNo = 0;
    running = false;
    step = false;
    record = false;
    gpu = false;
    format = QSurfaceFormat::defaultFormat();
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setMajorVersion(4);
    format.setMinorVersion(5);
    setFormat(format);
    makeCurrent();


    connect(&updateTimer,SIGNAL(timeout()),this,SLOT(simulate()));
    //connect(&updateTimer,SIGNAL(timeout()),this,SLOT(update()));
}

void GLCanvas::simulate()
{
    updateTimer.stop();
    if(running||step)
    {
        makeCurrent();
        particles->bind();
        solver->solve();

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
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glEnable(GL_CULL_FACE);

    glEnable(GL_PROGRAM_POINT_SIZE);

    //Load Skybox Shader
    Shader skyBoxVert(GL_VERTEX_SHADER,"Resources/Effects/skybox.vert");
    if(!skyBoxVert.compile())
    {
        std::cout<<skyBoxVert.compileLog().c_str()<<std::endl;
    }

    Shader skyBoxFrag(GL_FRAGMENT_SHADER,"Resources/Effects/skybox.frag");
    if(!skyBoxFrag.compile())
    {
        std::cout<<skyBoxFrag.compileLog().c_str()<<std::endl;
    }

    skyBoxProgram = new ShaderProgram();
    skyBoxProgram->attachShader(skyBoxVert);
    skyBoxProgram->attachShader(skyBoxFrag);
    if(!skyBoxProgram->link())
    {
        std::cout<<skyBoxProgram->linkLog().c_str()<<std::endl;
    }
    skyBoxProgram->bind();

    //Load Depth Shaders
    Shader depthVert(GL_VERTEX_SHADER,"Resources/Effects/Surface/depth.vert");
    if(!depthVert.compile())
    {
        std::cout<<depthVert.compileLog().c_str()<<std::endl;
    }

    Shader depthFrag(GL_FRAGMENT_SHADER,"Resources/Effects/Surface/depth.frag");
    if(!depthFrag.compile())
    {
        std::cout<<depthFrag.compileLog().c_str()<<std::endl;
    }

    depthProgram = new ShaderProgram();
    depthProgram->attachShader(depthVert);
    depthProgram->attachShader(depthFrag);
    if(!depthProgram->link())
    {
        std::cout<<depthProgram->linkLog().c_str()<<std::endl;
    }
    depthProgram->bind();

    //Load Smoothing Shader
    Shader smoothVert(GL_VERTEX_SHADER,"Resources/Effects/Surface/smooth.vert");
    if(!smoothVert.compile())
    {
        std::cout<<smoothVert.compileLog().c_str()<<std::endl;
    }

    Shader smoothFrag(GL_FRAGMENT_SHADER,"Resources/Effects/Surface/smooth.frag");
    if(!smoothFrag.compile())
    {
        std::cout<<smoothFrag.compileLog().c_str()<<std::endl;
    }

    smoothProgram = new ShaderProgram();
    smoothProgram->attachShader(smoothVert);
    smoothProgram->attachShader(smoothFrag);
    if(!smoothProgram->link())
    {
        std::cout<<smoothProgram->linkLog().c_str()<<std::endl;
    }
    smoothProgram->bind();

    //Load Thickness Shader
    Shader thicknessVert(GL_VERTEX_SHADER,"Resources/Effects/Surface/thickness.vert");
    if(!thicknessVert.compile())
    {
        std::cout<<thicknessVert.compileLog().c_str()<<std::endl;
    }

    Shader thicknessFrag(GL_FRAGMENT_SHADER,"Resources/Effects/Surface/thickness.frag");
    if(!thicknessFrag.compile())
    {
        std::cout<<thicknessFrag.compileLog().c_str()<<std::endl;
    }

    thicknessProgram = new ShaderProgram();
    thicknessProgram->attachShader(thicknessVert);
    thicknessProgram->attachShader(thicknessFrag);
    if(!thicknessProgram->link())
    {
        std::cout<<thicknessProgram->linkLog().c_str()<<std::endl;
    }
    thicknessProgram->bind();

    //Load Surface Shader
    Shader surfaceVert(GL_VERTEX_SHADER,"Resources/Effects/Surface/surface.vert");
    if(!surfaceVert.compile())
    {
        std::cout<<surfaceVert.compileLog().c_str()<<std::endl;
    }

    Shader surfaceFrag(GL_FRAGMENT_SHADER,"Resources/Effects/Surface/surface.frag");
    if(!surfaceFrag.compile())
    {
        std::cout<<surfaceFrag.compileLog().c_str()<<std::endl;
    }

    surfaceProgram = new ShaderProgram();
    surfaceProgram->attachShader(surfaceVert);
    surfaceProgram->attachShader(surfaceFrag);
    if(!surfaceProgram->link())
    {
        std::cout<<surfaceProgram->linkLog().c_str()<<std::endl;
    }
    surfaceProgram->bind();

    //Load Particle Shaders
    Shader particleVert(GL_VERTEX_SHADER,"Resources/Effects/Particles/particles.vert");
    if(!particleVert.compile())
    {
        std::cout<<particleVert.compileLog().c_str()<<std::endl;
    }

    Shader particleFrag(GL_FRAGMENT_SHADER,"Resources/Effects/Particles/particles.frag");
    if(!particleFrag.compile())
    {
        std::cout<<particleFrag.compileLog().c_str()<<std::endl;
    }

    particleProgram = new ShaderProgram();
    particleProgram->attachShader(particleVert);
    particleProgram->attachShader(particleFrag);
    if(!particleProgram->link())
    {
        std::cout<<particleProgram->linkLog().c_str()<<std::endl;
    }
    particleProgram->bind();

    //Load Solid Shaders
    Shader solidVert(GL_VERTEX_SHADER,"Resources/Effects/Solid/solid.vert");
    if(!solidVert.compile())
    {
        std::cout<<solidVert.compileLog().c_str()<<std::endl;
    }

    Shader solidFrag(GL_FRAGMENT_SHADER,"Resources/Effects/Solid/solid.frag");
    if(!solidFrag.compile())
    {
        std::cout<<solidFrag.compileLog().c_str()<<std::endl;
    }

    solidProgram = new ShaderProgram();
    solidProgram->attachShader(solidVert);
    solidProgram->attachShader(solidFrag);
    if(!solidProgram->link())
    {
        std::cout<<solidProgram->linkLog().c_str()<<std::endl;
    }
    solidProgram->bind();

    //Create Light
    light = Light(glm::vec3(20.0,20.0,0.0));

    //Create Screenquad
    std::vector<Vertex> screenQuadVerts(6);
    Vertex s1,s2,s3,s4,s5,s6;
    s1.pos = glm::vec3(-1.0f,1.0f,0.0f);
    s1.uv = glm::vec2(0.0f,1.0f);
    s2.pos = glm::vec3(-1.0f,-1.0f,0.0f);
    s2.uv = glm::vec2(0.0f,0.0f);
    s3.pos = glm::vec3(1.0f,-1.0f,0.0f);
    s3.uv = glm::vec2(1.0f,0.0f);
    s4.pos = glm::vec3(-1.0f,1.0f,0.0f);
    s4.uv = glm::vec2(0.0f,1.0f);
    s5.pos = glm::vec3(1.0f,1.0f,0.0f);
    s5.uv = glm::vec2(1.0f,1.0f);
    s6.pos = glm::vec3(1.0f,-1.0f,0.0f);
    s6.uv = glm::vec2(1.0f,1.0f);

    screenQuadVerts[0] = s1;
    screenQuadVerts[1] = s2;
    screenQuadVerts[2] = s3;
    screenQuadVerts[3] = s4;
    screenQuadVerts[5] = s5;
    screenQuadVerts[4] = s6;
    screenQuad = new VertexBuffer();
    screenQuad->bind();
    screenQuad->upload(screenQuadVerts);

    //Create Sphere Model
    sphere = new Model();
    sphere = Model::createSphere(1.0,16,16);

    sphere->bind();

    //Load Armadillo Model and voxelize
    //armadillo = new Model();
    //armadillo->load("Resources/sphere.obj");
    //armadillo->load("Resources/Models/Armadillo.obj");
    //armadillo->bind();
    //particles = armadillo->voxelize(particleSize);
    QOpenGLFramebufferObject::bindDefault();



    //Load SkyBox Vertices and Cubemap
    std::vector<Vertex> cubeVerts = {
      Vertex(glm::vec3(-1.0f,  1.0f, -1.0f)),
      Vertex(glm::vec3(-1.0f, -1.0f, -1.0f)),
      Vertex(glm::vec3(1.0f, -1.0f, -1.0f)),
      Vertex(glm::vec3(1.0f, -1.0f, -1.0f)),
      Vertex(glm::vec3(1.0f,  1.0f, -1.0f)),
      Vertex(glm::vec3(-1.0f,  1.0f, -1.0f)),

      Vertex(glm::vec3(-1.0f, -1.0f,  1.0f)),
      Vertex(glm::vec3(-1.0f, -1.0f, -1.0f)),
      Vertex(glm::vec3(-1.0f,  1.0f, -1.0f)),
      Vertex(glm::vec3(-1.0f,  1.0f, -1.0f)),
      Vertex(glm::vec3(-1.0f,  1.0f,  1.0f)),
      Vertex(glm::vec3(-1.0f, -1.0f,  1.0f)),

      Vertex(glm::vec3(1.0f, -1.0f, -1.0f)),
      Vertex(glm::vec3(1.0f, -1.0f,  1.0f)),
      Vertex(glm::vec3(1.0f,  1.0f,  1.0f)),
      Vertex(glm::vec3(1.0f,  1.0f,  1.0f)),
      Vertex(glm::vec3(1.0f,  1.0f, -1.0f)),
      Vertex(glm::vec3(1.0f, -1.0f, -1.0f)),

      Vertex(glm::vec3(-1.0f, -1.0f,  1.0f)),
      Vertex(glm::vec3(-1.0f,  1.0f,  1.0f)),
      Vertex(glm::vec3(1.0f,  1.0f,  1.0f)),
      Vertex(glm::vec3(1.0f,  1.0f,  1.0f)),
      Vertex(glm::vec3(1.0f, -1.0f,  1.0f)),
      Vertex(glm::vec3(-1.0f, -1.0f,  1.0f)),

      Vertex(glm::vec3(-1.0f,  1.0f, -1.0f)),
      Vertex(glm::vec3(1.0f,  1.0f, -1.0f)),
      Vertex(glm::vec3(1.0f,  1.0f,  1.0f)),
      Vertex(glm::vec3(1.0f,  1.0f,  1.0f)),
      Vertex(glm::vec3(-1.0f,  1.0f,  1.0f)),
      Vertex(glm::vec3(-1.0f,  1.0f, -1.0f)),

      Vertex(glm::vec3(-1.0f, -1.0f, -1.0f)),
      Vertex(glm::vec3(-1.0f, -1.0f,  1.0f)),
      Vertex(glm::vec3(1.0f, -1.0f, -1.0f)),
      Vertex(glm::vec3(1.0f, -1.0f, -1.0f)),
      Vertex(glm::vec3(-1.0f, -1.0f,  1.0f)),
      Vertex(glm::vec3(1.0f, -1.0f,  1.0f))
    };

    QImage negX,negY,negZ,posX,posY,posZ;
    negX.load("Resources/Textures/Yokohama2/negx.jpg");
    negY.load("Resources/Textures/Yokohama2/negy.jpg");
    negZ.load("Resources/Textures/Yokohama2/negz.jpg");
    posX.load("Resources/Textures/Yokohama2/posx.jpg");
    posY.load("Resources/Textures/Yokohama2/posy.jpg");
    posZ.load("Resources/Textures/Yokohama2/posz.jpg");

    cube = new VertexBuffer();
    cube->bind();
    cube->upload(cubeVerts);

    skyBox = new CubeMap();
    skyBox->bind(0);
    skyBox->upload_side(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,negX.width(),negX.height(),negX.bits());
    skyBox->upload_side(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,negY.width(),negY.height(),negY.bits());
    skyBox->upload_side(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,negZ.width(),negZ.height(),negZ.bits());
    skyBox->upload_side(GL_TEXTURE_CUBE_MAP_POSITIVE_X,posX.width(),posX.height(),posX.bits());
    skyBox->upload_side(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,posY.width(),posY.height(),posY.bits());
    skyBox->upload_side(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,posZ.width(),posZ.height(),posZ.bits());



    //Enable Vertex Attrib Arrays
    Vertex::setVertexAttribs();
    Vertex::enableVertexAttribs();


    particles = new ParticleBuffer();
    //ParticleBuffer* armadilloParts;
    //armadilloParts = armadillo->voxelize(particleSize);
    reset();
    //particles->merge(*armadilloParts);

    //delete armadilloParts;

    pbf = new PBFSolver(particles->getParticles(),(AbstractKernel*)densityKernel,(AbstractKernel*)gradKernel,(AbstractKernel*)viscKernel,0.08,4);
    pbfGpu = new PBFSolverGPU(particles->getParticles(),(AbstractKernel*)densityKernel,(AbstractKernel*)gradKernel,(AbstractKernel*)gradKernel,0.08,4);
    solver = (AbstractSolver*)pbf;
    //particles->addParticle(Particle(0,glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0),1.0,1.0));
}

void GLCanvas::paintGL()
{
    glViewport(0,0,width(),height());
    if(surface)
    {
        renderSurface();
    }
    else
    {
        renderParticles();
    }
    //delete particles;
}

void GLCanvas::renderParticles()
{
    glDepthMask(GL_FALSE);
    cube->bind();
    Vertex::setVertexAttribs();
    Vertex::enableVertexAttribs();
    glm::mat4 viewCube = camera.getRotMat();
    glm::mat4 modelViewCube = viewCube*model;
    glm::mat4 pvmCube = projection*modelViewCube;

    skyBoxProgram->bind();
    skyBoxProgram->uploadMat4("pvm",pvmCube);
    skyBoxProgram->uploadInt("cube_texture",0);
    skyBox->bind(0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);

    sphere->bind();
    Vertex::setVertexAttribs();
    Vertex::enableVertexAttribs();

    particles->bind();
    glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)32);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4,1,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)16);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(5,1,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)20);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(6,1,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)24);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(7,1,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)28);
    glEnableVertexAttribArray(7);

    glVertexAttribDivisor(0,0);
    glVertexAttribDivisor(1,0);
    glVertexAttribDivisor(3,1);
    glVertexAttribDivisor(4,1);
    glVertexAttribDivisor(5,1);
    glVertexAttribDivisor(6,1);
    glVertexAttribDivisor(7,1);

    particleProgram->bind();
    view = camera.getView();
    glm::mat4 modelView = view*model;
    glm::mat4 pvm = projection*modelView;
    glm::mat4 normalMatrix = glm::mat3(glm::transpose(glm::inverse((view*model))));
    particleProgram->uploadScalar("particleSize",particleSize);
    particleProgram->uploadMat4("modelView",modelView);
    particleProgram->uploadMat4("pvm",pvm);
    particleProgram->uploadMat4("view",view);
    particleProgram->uploadMat3("normalMatrix",normalMatrix);
    particleProgram->uploadVec3("cPos",camera.getPosition());
    particleProgram->uploadLight("light0",light,view);
    glDrawElementsInstanced(GL_TRIANGLES,sphere->getIndices().size(),GL_UNSIGNED_INT,0,particles->getNumParticles());

}

void GLCanvas::renderSurface()
{

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    //Render Background
    Texture bgImage;
    Texture bgDepthImage;
    bgImage.bind(0);
    bgImage.createRenderImage(this->width(),this->height());
    bgDepthImage.bind(1);
    bgDepthImage.createDepthImage(this->width(),this->height());
    FrameBufferObject fbo4;
    fbo4.bind();
    fbo4.attachColorImage(bgImage,0);
    fbo4.attachDepthImage(bgDepthImage);
    fbo4.setRenderBuffer({GL_COLOR_ATTACHMENT0});
    if(!fbo4.isComplete())
    {
        std::cout<<"FBO incomplete"<<std::endl;
    }

    glm::mat4 viewCube = camera.getRotMat();
    glm::mat4 modelViewCube = viewCube*model;
    glm::mat4 pvmCube = projection*modelViewCube;

    skyBoxProgram->bind();
    skyBoxProgram->uploadMat4("pvm",pvmCube);
    skyBoxProgram->uploadInt("cube_texture",0);
    skyBox->bind(0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    cube->bind();
    glVertexAttribDivisor(0,0);
    glVertexAttribDivisor(1,0);
    glVertexAttribDivisor(3,0);
    glVertexAttribDivisor(4,0);
    glVertexAttribDivisor(5,0);
    glVertexAttribDivisor(6,0);
    glVertexAttribDivisor(7,0);
    Vertex::setVertexAttribs();
    Vertex::enableVertexAttribs();
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
    glDisableVertexAttribArray(6);
    glDisableVertexAttribArray(7);
    glDepthMask(GL_FALSE);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
    //cube->bind();
    //glClear();
    CubeMap::unbind(0);
    for(unsigned int i=0;i<models.size();i++)
    {
        glm::mat4 view = camera.getView();
        glm::mat4 modelView = view*models[i]->getModelMat();
        glm::mat4 pvm = projection*modelView;

        glm::mat4 normalMatrix = glm::mat3(glm::transpose(glm::inverse((view*models[i]->getModelMat()))));

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

        solidProgram->bind();
        models[i]->bind();
        Vertex::setVertexAttribs();
        Vertex::enableVertexAttribs();

        solidProgram->uploadMat4("modelView",modelView);
        solidProgram->uploadMat4("projection",projection);
        solidProgram->uploadMat4("pvm",pvm);
        solidProgram->uploadMat4("view",view);
        solidProgram->uploadMat3("normalMatrix",normalMatrix);
        solidProgram->uploadLight("light0",light,view);
        glDrawElements(GL_TRIANGLES,models[i]->getIndices().size(),GL_UNSIGNED_INT,(void*)0);
        //models[i]->draw(solidProgram);
        glFinish();
    }
    fbo4.unbind();

    //sphere->bind();
    //Vertex::setVertexAttribs();
    //Vertex::enableVertexAttribs();

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    view = camera.getView();
    glm::mat4 modelView = view*model;
    glm::mat4 pvm = projection*modelView;
    glm::mat4 normalMatrix = glm::mat3(glm::transpose(glm::inverse((view*model))));
    FrameBufferObject fbo;
    Texture depthImage;
    depthImage.bind(0);
    depthImage.createDepthImage(this->width(),this->height());
    fbo.bind();
    fbo.attachDepthImage(depthImage);
    fbo.setRenderBuffer({GL_NONE});
    if(!fbo.isComplete())
    {
        std::cout<<"FBO incomplete"<<std::endl;
    }
    depthProgram->bind();
    depthProgram->uploadScalar("particleSize",particleSize);
    depthProgram->uploadMat4("modelView",modelView);
    depthProgram->uploadMat4("projection",projection);
    depthProgram->uploadMat4("pvm",pvm);
    depthProgram->uploadMat4("view",view);
    depthProgram->uploadMat3("normalMatrix",normalMatrix);
    depthProgram->uploadLight("light0",light,view);
    glClear(GL_DEPTH_BUFFER_BIT);
    particles->bind();
    glVertexAttribDivisor(0,0);
    glVertexAttribDivisor(1,0);
    glVertexAttribDivisor(3,0);
    glVertexAttribDivisor(4,0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)32);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4,1,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)16);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(5,1,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)20);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(6,1,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)24);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(7,1,GL_FLOAT,GL_FALSE,sizeof(Particle),(void*)28);
    glEnableVertexAttribArray(7);
    glDrawArrays(GL_POINTS,0,particles->getNumParticles());
    //glDrawElementsInstanced(GL_TRIANGLES,sphere->getIndices().size(),GL_UNSIGNED_INT,0,particles->getNumParticles());
    fbo.unbind();
/*
    fbo2.attachDepthImage(smoothDepthImage);
    fbo2.setRenderBuffer({GL_NONE});
    fbo2.bind();*/


    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    //Smooth Depthimage
    Texture smoothDepthImage;
    smoothDepthImage.bind(0);
    smoothDepthImage.createFloatRenderImage(this->width(),this->height());
    FrameBufferObject fbo2;
    glVertexAttribDivisor(0,0);
    glVertexAttribDivisor(1,0);
    glVertexAttribDivisor(3,0);
    glVertexAttribDivisor(4,0);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    smoothProgram->bind();
    {
        Texture tempImage;
        tempImage.bind(0);
        tempImage.createFloatRenderImage(this->width(),this->height());
        smoothProgram->uploadInt("depthMap",0);
        fbo2.bind();
        depthImage.bind(0);
        fbo2.attachColorImage(tempImage,0);
        fbo2.setRenderBuffer({GL_COLOR_ATTACHMENT0});
        if(!fbo2.isComplete())
        {
            std::cout<<"FBO incomplete"<<std::endl;
        }

        //Blur Horizontally
        smoothProgram->uploadVec2("blurDir",glm::vec2(1.0,0.0));
        glClear(GL_COLOR_BUFFER_BIT);
        screenQuad->bind();
        glDrawArrays(GL_TRIANGLES,0,6);

        //Blur Vertically
        smoothProgram->uploadInt("depthMap",0);
        fbo2.bind();
        tempImage.bind(0);
        fbo2.attachColorImage(smoothDepthImage,0);
        fbo2.setRenderBuffer({GL_COLOR_ATTACHMENT0});
        if(!fbo2.isComplete())
        {
            std::cout<<"FBO incomplete"<<std::endl;
        }
        smoothProgram->uploadVec2("blurDir",glm::vec2(0.0,1.0));
        glClear(GL_COLOR_BUFFER_BIT);
        screenQuad->bind();
        glDrawArrays(GL_TRIANGLES,0,6);
    }
    for(unsigned i=1;i<25;i++)
    {
        Texture tempImage;
        tempImage.bind(0);
        tempImage.createFloatRenderImage(this->width(),this->height());
        smoothProgram->uploadInt("depthMap",0);

        fbo2.bind();
        smoothDepthImage.bind(0);
        fbo2.attachColorImage(tempImage,0);
        fbo2.setRenderBuffer({GL_COLOR_ATTACHMENT0});
        if(!fbo2.isComplete())
        {
            std::cout<<"FBO incomplete"<<std::endl;
        }

        //Blur Horizontally
        smoothProgram->uploadVec2("blurDir",glm::vec2(1.0,0.0));
        glClear(GL_COLOR_BUFFER_BIT);
        screenQuad->bind();
        glDrawArrays(GL_TRIANGLES,0,6);

        //Blur Vertically
        smoothProgram->uploadInt("depthMap",0);
        fbo2.bind();
        tempImage.bind(0);
        fbo2.attachColorImage(smoothDepthImage,0);
        fbo2.setRenderBuffer({GL_COLOR_ATTACHMENT0});
        if(!fbo2.isComplete())
        {
            std::cout<<"FBO incomplete"<<std::endl;
        }
        smoothProgram->uploadVec2("blurDir",glm::vec2(0.0,1.0));
        glClear(GL_COLOR_BUFFER_BIT);
        screenQuad->bind();
        glDrawArrays(GL_TRIANGLES,0,6);

    }
    fbo2.unbind();

    //Calculate Thickness
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    Texture thicknessImage;
    Texture thicknessDepthImage;
    thicknessImage.bind(0);
    thicknessImage.createFloatRenderImage(this->width(),this->height());
    thicknessDepthImage.bind(1);
    thicknessDepthImage.createDepthImage(this->width(),this->height());
    FrameBufferObject fbo3;
    fbo3.bind();
    fbo3.attachColorImage(thicknessImage,0);
    fbo3.attachDepthImage(thicknessDepthImage);
    fbo3.setRenderBuffer({GL_COLOR_ATTACHMENT0});
    if(!fbo3.isComplete())
    {
        std::cout<<"FBO incomplete"<<std::endl;
    }
    thicknessProgram->bind();
    thicknessProgram->uploadScalar("particleSize",particleSize);
    thicknessProgram->uploadMat4("modelView",modelView);
    thicknessProgram->uploadMat4("projection",projection);
    thicknessProgram->uploadMat4("pvm",pvm);
    thicknessProgram->uploadMat4("view",view);
    thicknessProgram->uploadMat3("normalMatrix",normalMatrix);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE);

    glClear(GL_COLOR_BUFFER_BIT);
    particles->bind();
    glVertexAttribDivisor(0,0);
    glVertexAttribDivisor(1,0);
    glVertexAttribDivisor(3,0);
    glVertexAttribDivisor(4,0);
    glVertexAttribDivisor(5,0);
    glVertexAttribDivisor(6,0);
    glVertexAttribDivisor(7,0);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glEnableVertexAttribArray(6);
    glEnableVertexAttribArray(7);
    glDrawArrays(GL_POINTS,0,particles->getNumParticles());

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    fbo3.unbind();

    //Qt5 Hack to restore framebuffer
    QOpenGLFramebufferObject::bindDefault();

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    screenQuad->bind();
    Vertex::setVertexAttribs();
    Vertex::enableVertexAttribs();
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    //depthImage.bind(0);
    //smoothDepthImage.bind(0);
    surfaceProgram->bind();
    surfaceProgram->uploadScalar("vpWidth",this->width());
    surfaceProgram->uploadScalar("vpHeight",this->height());
    surfaceProgram->uploadVec3("cPos",camera.getPosition());
    surfaceProgram->uploadScalar("fx",projection[0][0]);
    surfaceProgram->uploadScalar("fy",projection[1][1]);
    surfaceProgram->uploadInt("depthMap",0);
    surfaceProgram->uploadInt("thicknessMap",1);
    surfaceProgram->uploadInt("background",2);
    surfaceProgram->uploadInt("skybox",3);
    surfaceProgram->uploadLight("light0",light,view);
    smoothDepthImage.bind(0);
    thicknessImage.bind(1);
    bgImage.bind(2);
    skyBox->bind(3);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES,0,6);
}

void GLCanvas::resizeGL(int w, int h)
{
    projection = glm::perspectiveFov(45.0f,(float)w,(float)h,0.1f,10.0f);
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

void GLCanvas::reset()
{
    particles->clear();
    particles->bind();
    unsigned int cc = 0;
    //int ppu = 15;
    int ppu = 20;
    for( int z=-ppu;z!=ppu;z++)
    {
        for( int y=-ppu;y!=ppu;y++)
        {
            for( int x=-ppu;x!=ppu+1;x++)
            {
                /*particles->addParticle(Particle(cc,glm::vec3(x/10.0,y/10.0,z/10.0),glm::vec3(0.0,0.0,0.0),1.0,1.0));
                cc++;*/
                //particles->addParticle(Particle(cc,glm::vec3(x/5.0-10,y/5.0,z/5.0),glm::vec3(0.0,0.0,0.0),1.0,1.0));
                //cc++;

                particles->addParticle(Particle(cc,glm::vec3(-0.5+((x+ppu)/(ppu*2.0)),-0.5+((y+ppu)/(ppu*2.0)),-0.5+((z+ppu)/(ppu*2.0))),glm::vec3(0.0,0.0,0.0),1.0,0.0,false));
                cc++;
            }
        }
    }
    particles->upload();
}

void GLCanvas::loadModel(QString fileName)
{
    PlacementDialog dialog;
    if(dialog.exec()==QDialog::Accepted)
    {
        makeCurrent();
        Model* model=new Model();
        model->load(fileName.toStdString());
        glm::vec3 transl = dialog.getDisplacement();
        bool solid = dialog.isSolid();
        model->setModelMat(glm::translate(glm::mat4(),transl));
        ParticleBuffer *parts = model->voxelize(particleSize,solid);
        QOpenGLFramebufferObject::bindDefault();
        if(gpu)
        {
            particles->syncGPU();
        }
        particles->merge(*parts);
        if(solid)
        {
            models.push_back(model);
        }
        else
        {
            delete model;
        }
        delete parts;
    }
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
    unsigned int iter = solver->getNumIterations();
    float particleSize = solver->getPartSize();
    float kernelSupport = solver->getKernelSupport();
    float timestep = solver->getTimestep();
    float restDensity = solver->getRestDensity();
    float artVisc = solver->getArtVisc();
    float artVort = solver->getArtVort();
    float cfm = solver->getCfmRegularization();
    float corrConst = solver->getCorrConst();
    float corrDist = solver->getCorrDist();
    float corrExp = solver->getCorrExp();
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
    solver->setNumIterations(iter);
    solver->setPartSize(particleSize);
    solver->setKernelSupport(kernelSupport);
    solver->setTimestep(timestep);
    solver->setRestDensity(restDensity);
    solver->setArtVisc(artVisc);
    solver->setArtVort(artVort);
    solver->setCfmRegularization(cfm);
    solver->setCorrConst(corrConst);
    solver->setCorrDist(corrDist);
    solver->setCorrExp(corrExp);
}

void GLCanvas::setSurface(int state)
{
    this->surface = state;
}
