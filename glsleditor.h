#ifndef GLSLEDITOR_H
#define GLSLEDITOR_H

#include <QWidget>
#include <QTextEdit>
#include <QSet>
#include "highlighter.h"
#include <Qt>
#include <QSignalMapper>

class GLSLEditor : public QWidget
{
    Q_OBJECT
public:
    explicit GLSLEditor(QWidget *parent = 0);
    QTextEdit *editor;
    QSignalMapper *signalMapper;
    QString text;
signals:
    void textChanged(QObject* glslwptr);
public slots:
    void cursorMoveHandle();

private:
     void setupEditor();
     float getFloatUnderCursor(QTextCursor tc);
//     void setupFileMenu();
//     void setupHelpMenu();


     Highlighter *highlighter;

     QSet<int> keysPressed;

     void HandleText(QString mode);
protected:

     void mouseMoveEvent ( QMouseEvent * mouseEvent );
     void keyPressEvent ( QKeyEvent * keyEvent );
     void keyReleaseEvent ( QKeyEvent * keyEvent );
};

#endif // GLSLEDITOR_H
