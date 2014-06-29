#ifndef QGLFRAME_H
#define QGLFRAME_H

#include <QGLWidget>

#include <QTimer>
#include "qglrenderthread.h"
#include "glsleditor.h"

class QGLFrame : public QGLWidget
{
    Q_OBJECT
public:
    explicit QGLFrame(QWidget *parent = 0);
    ~QGLFrame();
    void initRenderThread(void);
    void stopRenderThread(void);
    QGLRenderThread RenderThread;
    QString code;


signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);
    void sendBackValueInfo(int start,int end,float newValueFloat,int newValueInt,bool isInt);
    void sendCode(QString code);
    void sendFPS(QString FPS);

public slots:
   void prevModel();
   void nextModel();
   void setXRotation(int angle);
   void setYRotation(int angle);
   void setZRotation(int angle);
   void handleCodeChange(QObject* glslwptr);
   void getValueChanged(int position,QString mode);
   void getCodeChanged(QString newCode);
   void openFileDialog();



protected:
    void closeEvent(QCloseEvent *evt);
    void resizeEvent(QResizeEvent *evt);
    void paintEvent(QPaintEvent *);


private:
    int xRot;
    int yRot;
    int zRot;
    QBasicTimer timer;

private slots:
    void animate();
};


#endif // QGLFRAME_H
