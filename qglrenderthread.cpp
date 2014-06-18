#include "qglrenderthread.h"

#include <QGLShader>
#include <QtOpenGL>
#include <QFileInfo>
#include <gl/freeglut.h>
#include "qglframe.h"
#include <math.h>

static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };

QGLRenderThread::QGLRenderThread(QGLFrame *parent) :
    QThread(),
    GLFrame(parent)
{
    model = 0;
    xRot = 0;
    yRot = 0;
    zRot = 0;
    doRendering = true;
    doResize = false;
    FrameCounter=0;
    timerCount =0;
    ShaderProgram = NULL;
    VertexShader = FragmentShader = NULL;
}

void QGLRenderThread::resizeViewport(const QSize &size)
{
    w = size.width();
    h = size.height();
    doResize = true;
}

void QGLRenderThread::stop()
{
    doRendering = false;
}


void QGLRenderThread::run()
{
    qDebug()<<"running";
    GLFrame->makeCurrent();
    GLInit();
    LoadShader("../DistanceField/Basic.vsh", "../DistanceField/Basic.fsh");

    while (doRendering)
        {
        if(doResize)
            {
            GLResize(w, h);
            doResize = false;
            }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
        glLoadIdentity();

        paintGL(); // render actual frame

        FrameCounter++;
        GLFrame->swapBuffers();

        msleep(16); // wait 16ms => about 60 FPS
        }
}


void QGLRenderThread::GLInit(void)
{
    glClearColor(0.25f, 0.25f, 0.4f, 0.0f);
}


void QGLRenderThread::GLResize(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluPerspective(45.,((GLfloat)width)/((GLfloat)height),0.1f,1000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void QGLRenderThread::paintGL(void)
{
    //qDebug()<<"PAinting";
    ShaderProgram->setUniformValue("timerCount",timerCount);
    ShaderProgram->setUniformValue("modelNo",model);

    lightPosition[0]=10.0*sin((double)(zRot*22.0)/(7.0*180.0));
    lightPosition[1]=5.0;
    lightPosition[2]=10.0*cos((double)(zRot*22.0)/(7.0*180.0));
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glTranslatef(0.0f, 0.0f, -5.0f);            // move 5 units into the screen
    //glRotatef(FrameCounter,1.0f,1.0f,1.5f);     // rotate z-axis
    glRotatef(xRot, 1.0, 0.0, 0.0);
    glRotatef(yRot, 0.0, 1.0, 0.0);
    //glRotatef(100*timerCount, 0.0, 1.0, 1.0);

    //glRotatef(zRot, 0.0, 0.0, 1.0);
    glColor3f(1.0,0.6,0.0);
    //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
//    glutSolidTeapot(0.6);
    float qSize=2.0;
    glCullFace(GL_BACK);

    glBegin(GL_QUADS);


            glNormal3f(0,0,1);
            glVertex3f(qSize,qSize,0);
            glNormal3f(0,0,1);
            glVertex3f(qSize,-qSize,0);
            glNormal3f(0,0,1);
            glVertex3f(-qSize,-qSize,0);
            glNormal3f(0,0,1);
            glVertex3f(-qSize,qSize,0);

    glEnd();

    glCullFace(GL_FRONT);


    glBegin(GL_QUADS);


            glNormal3f(0,0,1);
            glVertex3f(qSize,qSize,0);
            glNormal3f(0,0,1);
            glVertex3f(qSize,-qSize,0);
            glNormal3f(0,0,1);
            glVertex3f(-qSize,-qSize,0);
            glNormal3f(0,0,1);
            glVertex3f(-qSize,qSize,0);

    glEnd();


}

void QGLRenderThread::sleepOne()
{
    this->sleep(1);
}

void QGLRenderThread::updateFragShader(QString code)
{
    GLInit();
    if(FragmentShader)
        {
        delete FragmentShader;
        FragmentShader = NULL;
        }

    FragmentShader = new QGLShader(QGLShader::Fragment);

    if(FragmentShader->compileSourceFile("../DistanceField/Basic.fsh"))
        ShaderProgram->addShader(FragmentShader);
    else qWarning() << "Fragment Shader compile Error" << FragmentShader->log();



    if(!ShaderProgram->link())
        {
        qWarning() << "Shader Program Linker Error" << ShaderProgram->log();
        }
    else
        ShaderProgram->bind();
}

void QGLRenderThread::LoadShader(QString vshader, QString fshader)
{
    if(ShaderProgram)
        {
        ShaderProgram->release();
        ShaderProgram->removeAllShaders();
        }
    else ShaderProgram = new QGLShaderProgram;

    if(VertexShader)
        {
        delete VertexShader;
        VertexShader = NULL;
        }

    if(FragmentShader)
        {
        delete FragmentShader;
        FragmentShader = NULL;
        }

    // load and compile vertex shader
    QFileInfo vsh(vshader);
    if(vsh.exists())
        {
        VertexShader = new QGLShader(QGLShader::Vertex);

        if(VertexShader->compileSourceFile(vshader))
        {
            ShaderProgram->addShader(VertexShader);
        }
        else qWarning() << "Vertex Shader Error" << VertexShader->log();
        }
    else qWarning() << "Vertex Shader source file " << vshader << " not found.";


    // load and compile fragment shader
    if(GLFrame->code.isEmpty())
    {
        qDebug()<<"No code reading from file!";
        QFileInfo fsh(fshader);
        if(fsh.exists())
            {
            FragmentShader = new QGLShader(QGLShader::Fragment);
            if(FragmentShader->compileSourceFile(fshader))
                ShaderProgram->addShader(FragmentShader);
            else qWarning() << "Fragment Shader Error" << FragmentShader->log();
            }
        else qWarning() << "Fragment Shader source file " << fshader << " not found.";
    }
    else
    {
        qDebug()<<"code reading";
        qDebug()<<GLFrame->code;
        FragmentShader = new QGLShader(QGLShader::Fragment);
        if(FragmentShader->compileSourceCode(GLFrame->code))
            ShaderProgram->addShader(FragmentShader);
        else qWarning() << "Fragment Shader Error" << FragmentShader->log();

    }

    if(!ShaderProgram->link())
        {
        qWarning() << "Shader Program Linker Error" << ShaderProgram->log();
        }
    else ShaderProgram->bind();
}
