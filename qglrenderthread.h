#ifndef QGLRENDERTHREAD_H
#define QGLRENDERTHREAD_H

#include <QThread>

#include <QGLWidget>

class QGLFrame;
class QSize;
class QGLShaderProgram;
class QGLShader;

class QGLRenderThread : public QThread
{
    Q_OBJECT
public:
    explicit QGLRenderThread(QGLFrame *parent = 0);
    void resizeViewport(const QSize &size);
    void run(void);
    void stop(void);
    void LoadShader(QString vshader, QString fshader);
    void paintGL(void);
    int xRot;
    int yRot;
    int zRot;
    int model;
    float timerCount;
protected:
    void GLInit(void);
    void GLResize(int width, int height);


private:
    bool doRendering, doResize;
    int w, h, FrameCounter;

    QGLFrame *GLFrame;

    QGLShaderProgram *ShaderProgram;
    QGLShader *VertexShader, *FragmentShader;

signals:

public slots:

};


#endif // QGLRENDERTHREAD_H
