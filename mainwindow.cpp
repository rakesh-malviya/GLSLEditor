#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //connect(GLFrame, SIGNAL(xRotationChanged(int)), xSlider, SLOT(setValue(int)));
    //connect(ui->editorTab, SIGNAL(textChanged(QObject*)), ui->glWidget, SLOT(handleCodeChange(QObject*)));
    connect(ui->editorTab, SIGNAL(sendValueChanged(int,QString)), ui->glWidget, SLOT(getValueChanged(int,QString)));
    connect(ui->glWidget,SIGNAL(sendBackValueInfo(int,int,float,int,bool)),ui->editorTab,SLOT(getValueChangedInfo(int,int,float,int,bool)));
    connect(ui->actionOpen,SIGNAL(triggered()),ui->glWidget,SLOT(openFileDialog()));
    connect(ui->glWidget,SIGNAL(sendCode(QString)),ui->editorTab,SLOT(getCode(QString)));

}

MainWindow::~MainWindow()
{
    ui->glWidget->stopRenderThread();
    delete ui;
}
