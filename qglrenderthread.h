#ifndef QGLRENDERTHREAD_H
#define QGLRENDERTHREAD_H

#include <QThread>

#include <QGLWidget>

#include "constantfinder.h"


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
    void run();
    void stop(void);
    void LoadShader(QString vshader, QString fshader);
    void paintGL(void);
    int xRot;
    int yRot;
    int zRot;
    int model;
    float timerCount;
    void sleepOne();
    bool doRendering;
    void updateFragShader(QString code);    
    int handleValueChanged(int position,QString mode,int* start,int* end, float* newValueFloat,int* newValueInt,bool* isInt);
    int findIndex(int position,bool* isInt);

protected:
    void GLInit(void);
    void GLResize(int width, int height);


private:
    ConstantFinder* constFind;
    void createNewCode(QString code);
    int* intConstants;
    float* floatConstants;

    void setUniformFloat();
    void setUniformInt();

    bool  doResize;
    int w, h, FrameCounter;
    QString fShaderModified;
    QGLFrame *GLFrame;

    QGLShaderProgram *ShaderProgram;
    QGLShader *VertexShader, *FragmentShader;

signals:

public slots:

};


#endif // QGLRENDERTHREAD_H
