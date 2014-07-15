#include "qglframe.h"

#include <QResizeEvent>
#include <QDebug>
#include <QFileDialog>

QGLFrame::QGLFrame(QWidget *parent) :
    QGLWidget(parent),
    RenderThread(this)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;
    setAutoBufferSwap(false);
    QTimer *myTimer = new QTimer;
    connect(myTimer,SIGNAL(timeout()),SLOT(animate()));
    myTimer->start(10);
    initRenderThread();
}

QGLFrame::~QGLFrame()
{
}

void QGLFrame::initRenderThread(void)
{
    doneCurrent();
    RenderThread.start();
}

void QGLFrame::stopRenderThread(void)
{
    RenderThread.stop();
    RenderThread.wait();
}

void QGLFrame::resizeEvent(QResizeEvent *evt)
{
    RenderThread.resizeViewport(evt->size());
}

void QGLFrame::paintEvent(QPaintEvent *)
{
    // Do nothing. Let the thread do the work

}

void QGLFrame::closeEvent(QCloseEvent *evt)
{
    stopRenderThread();
    QGLWidget::closeEvent(evt);
}

void QGLFrame::prevModel(){
    RenderThread.model-=1;
    if(RenderThread.model<0)
        RenderThread.model=0;
}

void QGLFrame::nextModel(){
    RenderThread.model+=1;
    if(RenderThread.model>20)
        RenderThread.model=0;
}

void QGLFrame::getValueChanged(int position, QString mode)
{
    //qDebug() << "Pos:" << position << "  Mode:" << mode;
    int start;
    int end;
    float newValueFloat;
    int newValueInt;
    bool isInt;
    if(RenderThread.handleValueChanged(position,mode,&start,&end,&newValueFloat,&newValueInt,&isInt)!=-1)
    {
        qDebug() << isInt << "  " << QString::number(newValueFloat,'f',2) << "  " << newValueInt;
        emit sendBackValueInfo(start,end,newValueFloat,newValueInt,isInt);
    }
}

void QGLFrame::getCodeChanged(QString newCode)
{
    this->code =  newCode;
    stopRenderThread();
    initRenderThread();
    RenderThread.doRendering=true;
}

void QGLFrame::openFileDialog()
{
    QString filePath = QFileDialog::getOpenFileName(this,tr("Fragment Shader File"),"./../Shadertoy",tr("Files (*.fsh)"));
    qDebug()<<filePath;
    QFile fShaderFile(filePath);

    if(fShaderFile.open(QFile::ReadOnly | QFile::Text))
    {
        this->code =  fShaderFile.readAll();
        stopRenderThread();
        initRenderThread();
        RenderThread.doRendering=true;
        emit sendCode(this->code);
    }
    else
        qDebug() <<"Error unable to open file:" << filePath;
}

void QGLFrame::handleCodeChange(QObject *glslwptr)
{
    this->code=((GLSLEditor*)glslwptr)->editor->toPlainText();
    stopRenderThread();
    initRenderThread();
    RenderThread.doRendering=true;
}

void QGLFrame::setXRotation(int angle)
 {
     if (angle != xRot) {
         xRot = angle;
         emit xRotationChanged(angle);
         RenderThread.xRot = xRot;
         RenderThread.paintGL();
     }
 }

 void QGLFrame::setYRotation(int angle)
 {

     if (angle != yRot) {
         yRot = angle;
         emit yRotationChanged(angle);
         RenderThread.yRot = yRot;
         RenderThread.paintGL();
     }
 }

 void QGLFrame::setZRotation(int angle)
 {

     if (angle != zRot) {
         zRot = angle;
         emit zRotationChanged(angle);
         RenderThread.zRot = zRot;
         RenderThread.paintGL();
     }

 }



 void QGLFrame::animate()
 {
    RenderThread.timerCount += 0.01;
     emit sendFPS(RenderThread.FPS);
    //updateGL();
 }
