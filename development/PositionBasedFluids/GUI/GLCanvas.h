#include<GL/glew.h>
#include<QTimer>
#include<QOpenGLWidget>
#include<glm/glm.hpp>
#include"../Graphics/Shader/Shader.h"
#include"../Graphics/ShaderProgram/ShaderProgram.h"
#include"../Graphics/VertexBuffer/VertexBuffer.h"
#include"../Graphics/ParticleBuffer/ParticleBuffer.h"
#include"../Graphics/Camera/Camera.h"
#include"../Solver/AbstractSolver.h"

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
    void simulate();
private:
    bool running;
    QTimer simulationTimer;
    QTimer updateTimer;
    QPoint mouseCoords;

    AbstractSolver* solver;

    QSurfaceFormat format;
    Camera camera;
    ShaderProgram* program;
    VertexBuffer* vbo;
    ParticleBuffer* particles;
    unsigned int vao;

    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
};
