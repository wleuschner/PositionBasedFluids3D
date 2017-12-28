#include<GL/glew.h>
#include<QOpenGLWidget>
#include<glm/glm.hpp>
#include"../Graphics/Shader/Shader.h"
#include"../Graphics/ShaderProgram/ShaderProgram.h"
#include"../Graphics/VertexBuffer/VertexBuffer.h"

class GLCanvas : public QOpenGLWidget
{
    Q_OBJECT
public:
    GLCanvas(QWidget* parent=0);
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
private:
    QSurfaceFormat format;
    ShaderProgram* program;
    VertexBuffer* vbo;
    unsigned int particles;
    unsigned int vao;

    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
};
