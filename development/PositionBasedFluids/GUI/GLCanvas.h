#include<QOpenGLWidget>
#include<glm/glm.hpp>

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

    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
};
