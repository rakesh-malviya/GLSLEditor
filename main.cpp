#include <QtGui/QApplication>
#include "mainwindow.h"
#include <gl/freeglut.h>
int main(int argc, char *argv[])
{
    glutInit(&argc,argv);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
