#include "qglrenderthread.h"

#include <QGLShader>
#include <QtOpenGL>
#include <QFileInfo>
#include <QFile>
#include <gl/freeglut.h>
#include "qglframe.h"
#include <math.h>
#include <QByteArray>

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
    constFind = NULL;
    intConstants = NULL;
    floatConstants = NULL;
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
    LoadShader("../GLSLEditor/Basic.vsh", "../GLSLEditor/Basic.fsh");

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

    setUniformInt();
    setUniformFloat();

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

int QGLRenderThread::handleValueChanged(int position, QString mode, int *start, int *end, float *newValueFloat, int *newValueInt, bool *isIntBoolPtr)
{
    bool isInt;
    int index=findIndex(position,&isInt);
    if(index!=-1)
    {
        if(isInt)
        {
            if(mode=="DEC")
                intConstants[index]-=1;
            else
                intConstants[index]+=1;
            //qDebug()<<"IntValue:"<<intConstants[index];

            *start = constFind->intArray[index].start;
            *end   = constFind->intArray[index].end;
            *newValueInt = intConstants[index];
            *isIntBoolPtr = true;
        }
        else
        {
            if(mode=="DEC")
                floatConstants[index]-=0.05;
            else
                floatConstants[index]+=0.05;
            //qDebug()<<"FloatValue:"<<floatConstants[index];
            *start = constFind->floatArray[index].start;
            *end   = constFind->floatArray[index].end;
            *newValueFloat = floatConstants[index];
            *isIntBoolPtr = false;
        }
    }

    return index;
}

int QGLRenderThread::findIndex(int position,bool* isInt)
{
    int intStartPos,intEndPos,floatStartPos,floatEndPos;
    int intStartPosIndex,intEndPosIndex,floatStartPosIndex,floatEndPosIndex;
    //Binary search to find data at postion in intArray
    int high=constFind->intArraySize-1;
    int low=0;
    int mid=0;
    while(high>=low)
    {
        mid = (low+high)/2;
        if(constFind->intArray[mid].start == position)
            break;
        else if(constFind->intArray[mid].start < position)
            low = mid+1;
        else
            high = mid -1;
    }

    if(constFind->intArray[mid].start>position && mid!=0)
        mid--;
    qDebug() << mid <<" "<<constFind->intArray[mid].data<<" " << position << " " <<constFind->intArray[mid].start;
    intStartPos = constFind->intArray[mid].start;
    intStartPosIndex = mid;

    high=constFind->intArraySize-1;
    low=0;
    mid=0;
    while(high>=low)
    {
        mid = (low+high)/2;
        if((constFind->intArray[mid].end+1) == position)
            break;
        else if((constFind->intArray[mid].end+1) < position)
            low = mid+1;
        else
            high = mid -1;
    }

    if((constFind->intArray[mid].end+1) < position  && mid!=(constFind->intArraySize - 1))
        mid++;

    qDebug() << mid <<" "<<constFind->intArray[mid].data<<" " << position << " " <<(constFind->intArray[mid].end+1);
    intEndPos = constFind->intArray[mid].end+1;
    intEndPosIndex = mid;

    if((intEndPosIndex==intStartPosIndex) && (intStartPos <=position) && (intEndPos >= position))
    {
        *isInt=true;
        return intEndPosIndex;
    }

    //Binary search to find data at postion in floatArray
    high=constFind->floatArraySize-1;
    low=0;
    mid=0;
    while(high>=low)
    {
        mid = (low+high)/2;
        if(constFind->floatArray[mid].start == position)
            break;
        else if(constFind->floatArray[mid].start < position)
            low = mid+1;
        else
            high = mid - 1;
    }

    if(constFind->floatArray[mid].start>position && mid!=0)
        mid--;

    qDebug() << mid <<" "<<constFind->floatArray[mid].data<<" " << position << " " <<constFind->floatArray[mid].start;
    floatStartPos = constFind->floatArray[mid].start;
    floatStartPosIndex = mid;


    high=constFind->floatArraySize-1;
    low=0;
    mid=0;
    while(high>=low)
    {
        mid = (low+high)/2;
        if((constFind->floatArray[mid].end+1) == position && mid!=(constFind->floatArraySize - 1))
            break;
        else if((constFind->floatArray[mid].end+1) < position)
            low = mid+1;
        else
            high = mid - 1;
    }

    if((constFind->floatArray[mid].end+1) < position)
        mid++;
    qDebug() << mid <<" "<<constFind->floatArray[mid].data<<" " << position << " " <<constFind->floatArray[mid].end+1;
    floatEndPos = constFind->floatArray[mid].end+1;
    floatEndPosIndex = mid;

    if((floatEndPosIndex==floatStartPosIndex) && (floatStartPos <=position) && (floatEndPos >= position))
    {
        *isInt=false;
        return floatEndPosIndex;
    }

    qDebug() << "___________________________________________________";
    return -1;
}


void QGLRenderThread::createNewCode(QString code)
{
    if(intConstants)
    {
        delete intConstants;
        intConstants = NULL;
    }
    intConstants = new int[constFind->intArraySize];

    if(floatConstants)
    {
        delete floatConstants;
        floatConstants = NULL;
    }
    floatConstants = new float[constFind->floatArraySize];


    //Copy data from constFind->int/floatArray to int/floatConstants
    //data in int/floatConstants may change
    //but constFind->int/floatArray will have same unless code is change

    for(int i=0;i<constFind->intArraySize;i++)
        intConstants[i]=constFind->intArray[i].data;

    for(int i=0;i<constFind->floatArraySize;i++)
        floatConstants[i]=constFind->floatArray[i].data;

    //Now create the code
    QString intCode="uniform int ";
    for(int i=0;i<constFind->intArraySize;i++)
    {
        QString constName="intConst";
        QString intStr;
        intStr.setNum(i);
        constName+=intStr;
        constName.append(',');
        //qDebug()<<constName;
        intCode+=constName;
    }
    intCode[intCode.length()-1]=';';
    //qDebug()<<intCode;

    QString floatCode="uniform float ";
    for(int i=0;i<constFind->floatArraySize;i++)
    {
        QString constName="floatConst";
        QString floatStr;
        floatStr.setNum(i);
        constName+=floatStr;
        constName.append(',');
        //qDebug()<<constName;
        floatCode+=constName;
    }
    floatCode[floatCode.length()-1]=';';
    //qDebug()<<floatCode;

    QString mergedConstCode = intCode + "\n" + floatCode;

    //qDebug()<<mergedConstCode;

    int insertPos=0;
    int offset=0;

    insertPos=code.indexOf(';')+1;
    code.insert(insertPos,'\n');
    offset++;
    code.insert(insertPos+1,mergedConstCode);
    offset+=mergedConstCode.length();
//    qDebug() << code;

    for(int i=0,j=0;i<constFind->intArraySize||j<constFind->floatArraySize;)
    {
        int intStartPoint = constFind->intArray[i].start;
        int floatStartPoint = constFind->floatArray[j].start;

        if(intStartPoint < floatStartPoint && i < constFind->intArraySize)
        {
            int removeLength = constFind->intArray[i].end + 1 - intStartPoint;
            qDebug()<<"INT "<<constFind->intArray[i].data << "  "<<intStartPoint << "  " << constFind->intArray[i].end<< "  "<<removeLength;
            code.remove(offset+intStartPoint,removeLength);

            QString constName="intConst";
            QString intStr;
            intStr.setNum(i);
            constName+=intStr;
            code.insert(offset+intStartPoint,constName);
            offset+=(constName.length()-removeLength);
            i++;
        }
        else if(j < constFind->floatArraySize)
        {
            int removeLength = constFind->floatArray[j].end + 1 - floatStartPoint;
            qDebug()<<"FLO "<<constFind->floatArray[j].data << "  " << floatStartPoint << "  " << constFind->floatArray[j].end<< "  "<<removeLength;
            code.remove(offset+floatStartPoint,removeLength);
            QString constName="floatConst";
            QString floatStr;
            floatStr.setNum(j);
            constName+=floatStr;
            code.insert(offset+floatStartPoint,constName);
            offset+=(constName.length()-removeLength);
            j++;
        }

    }

    qDebug() << code;

    fShaderModified = code;
}

void QGLRenderThread::setUniformFloat()
{
    for(int i=0;i<constFind->floatArraySize;i++)
    {
        QString constName="floatConst";
        QString floatStr;
        floatStr.setNum(i);
        constName+=floatStr;
        QByteArray ba = constName.toLocal8Bit();
        const char* charConstName = ba.data();
        ShaderProgram->setUniformValue(charConstName,floatConstants[i]);
    }
}

void QGLRenderThread::setUniformInt()
{
    for(int i=0;i<constFind->intArraySize;i++)
    {
        QString constName="intConst";
        QString intStr;
        intStr.setNum(i);
        constName+=intStr;
        QByteArray ba = constName.toLocal8Bit();
        const char* charConstName = ba.data();
        ShaderProgram->setUniformValue(charConstName,intConstants[i]);
    }
}




void QGLRenderThread::LoadShader(QString vshader, QString fshader)
{
    if(constFind)
    {
        delete constFind;
        constFind=NULL;
    }
    QFile fShaderFile(fshader);

    if(GLFrame->code.isEmpty())
    {
        if(fShaderFile.open(QFile::ReadOnly | QFile::Text))
        {
            QString code = fShaderFile.readAll();
            constFind = new ConstantFinder(code);
            fShaderFile.close();
            if(constFind)
                createNewCode(code);
        }
    }
    else
    {
        constFind = new ConstantFinder(GLFrame->code);
        if(constFind)
            createNewCode(GLFrame->code);
    }


    if(ShaderProgram)
    {
        ShaderProgram->release();
        ShaderProgram->removeAllShaders();
        ShaderProgram = NULL;
    }
    ShaderProgram = new QGLShaderProgram;

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
//    if(GLFrame->code.isEmpty())
    if(fShaderModified.isEmpty())
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
        //qDebug()<<GLFrame->code;
        FragmentShader = new QGLShader(QGLShader::Fragment);
//        if(FragmentShader->compileSourceCode(GLFrame->code))
        if(FragmentShader->compileSourceCode(fShaderModified))
            ShaderProgram->addShader(FragmentShader);
        else qWarning() << "Fragment Shader Error" << FragmentShader->log();

    }

    if(!ShaderProgram->link())
        {
        qWarning() << "Shader Program Linker Error" << ShaderProgram->log();
        }
    else ShaderProgram->bind();
}
