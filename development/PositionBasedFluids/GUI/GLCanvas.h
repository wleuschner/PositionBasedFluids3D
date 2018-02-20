#include<GL/glew.h>
#include<QTimer>
#include<QOpenGLWidget>
#include<glm/glm.hpp>
#include"../Graphics/Shader/Shader.h"
#include"../Graphics/ShaderProgram/ShaderProgram.h"
#include"../Graphics/VertexBuffer/VertexBuffer.h"
#include"../Graphics/ParticleBuffer/ParticleBuffer.h"
#include"../Graphics/Camera/Camera.h"
#include"../Graphics/Model/Model.h"
#include"../Graphics/CubeMap/CubeMap.h"
#include"../Solver/AbstractSolver.h"
#include"../Solver/SolverImpl/PBFSolver.h"
#include"../Solver/SolverImpl/PBFSolverGPU.h"



class GLCanvas : public QOpenGLWidget
{
    Q_OBJECT
public:
    GLCanvas(QWidget* parent=0);
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
protected slots:
    void reset();

    void setNumIterations(int val);
    void setTimestep(double val);
    void setKernelSupport(double val);
    void setParticleSize(double val);
    void setRestDensity(double val);
    void setArtVisc(double val);
    void setArtVort(double val);
    void setCfmRegularization(double val);
    void setCorrConst(double val);
    void setCorrDist(double val);
    void setCorrExp(double val);
    void setDensityKernel(int index);
    void setGradKernel(int index);
    void setViscKernel(int index);
    void setGPU(int state);
    void setSurface(int state);

    void simulate();
private:
    void renderParticles();
    void renderSurface();

    unsigned int screenshotNo;
    bool step;
    bool running;
    bool record;
    bool gpu;
    bool surface;
    unsigned int smoothIter;
    QTimer simulationTimer;
    QTimer updateTimer;
    QPoint mouseCoords;

    PBFSolver* pbf;
    PBFSolverGPU* pbfGpu;
    AbstractSolver* solver;
    std::vector<AbstractKernel*> kernels;

    QSurfaceFormat format;
    Camera camera;
    ShaderProgram* skyBoxProgram;
    ShaderProgram* particleProgram;
    ShaderProgram* depthProgram;
    ShaderProgram* smoothProgram;
    ShaderProgram* thicknessProgram;
    ShaderProgram* surfaceProgram;
    ParticleBuffer* particles;
    unsigned int vao;

    float particleSize;

    Light light;
    Model* sphere;
    Model* armadillo;
    CubeMap* skyBox;
    VertexBuffer* cube;
    VertexBuffer* screenQuad;

    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
};
