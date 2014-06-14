#include "glsleditor.h"

#include <QtGui>
GLSLEditor::GLSLEditor(QWidget *parent) :
    QWidget(parent)
{
    setupEditor();
    QHBoxLayout *editorLayout = new QHBoxLayout;
    editorLayout->addWidget(editor);
    this->setLayout(editorLayout);
}


void GLSLEditor::setupEditor()
{

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    editor = new QTextEdit;
    editor->setFont(font);
    editor->setReadOnly(true);
    connect(editor,SIGNAL(selectionChanged()),this,SLOT(cursorMoveHandle()));

    highlighter = new Highlighter(editor->document());

    QFile file("../DistanceField/Basic.fsh");

    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        text = file.readAll();
        editor->setPlainText(text);
    }
    else
        printf_s("No file :(\n");
}

void GLSLEditor::cursorMoveHandle()
{
    QTextCursor tc = editor->textCursor();
    //tc.select(QTextCursor::WordUnderCursor);
    QString word = tc.selectedText();
    qDebug() << word;
    qDebug() << tc.position();
//    tc.removeSelectedText();
//    editor->insertPlainText("xxx");
//    QChar c = text.at(tc.position()-1);
//    qDebug() << c;
}

void GLSLEditor::mouseMoveEvent ( QMouseEvent * mouseEvent )
{
    qDebug()<<mouseEvent->pos();
}

void GLSLEditor::keyPressEvent ( QKeyEvent * keyEvent )
{
    keysPressed+= keyEvent->key();


    if(keysPressed.contains(Qt::Key_Control)&& keysPressed.contains(Qt::Key_Comma))
    {
        HandleText("DEC");
    }
    else if(keysPressed.contains(Qt::Key_Control) && keysPressed.contains(Qt::Key_Period))
    {
        HandleText("INC");
    }
}

void GLSLEditor::keyReleaseEvent ( QKeyEvent * keyEvent )
{
    keysPressed-= keyEvent->key();
}

void GLSLEditor::HandleText(QString mode)
{

    QTextCursor tc = editor->textCursor();
    //tc.select(QTextCursor::WordUnderCursor);
    QString word = tc.selectedText();

    if(!word.isEmpty())
    {
        qDebug() << word;
        qDebug() << tc.position();
        bool fEnd=false;
        if(word.at(word.length()-1)=='f')
        {
            fEnd=true;
            qDebug()<<"f at end processing";
            word.remove(word.length()-1,1);
            qDebug()<<"word after processing:";
            qDebug()<<word;
        }
        bool converted;
        float value = word.toFloat(&converted);
        if(converted)
            qDebug()<<value;
        else
            qDebug()<<"Conversion fail!";

        if(mode=="INC")
        {
            qDebug()<<"inc";
            value+=0.05f;
            QString newWord;
            newWord= QString::number(value,'f',2);
            if(fEnd)
                newWord.append('f');
            qDebug()<<newWord;
            tc.removeSelectedText();
            editor->insertPlainText(newWord);
            int startPos = tc.position();
            int endPos = tc.position()-newWord.length();
            tc.setPosition(startPos);
            tc.setPosition(endPos, QTextCursor::KeepAnchor);
            editor->setTextCursor(tc);
        }
        else if(mode=="DEC")
        {
            qDebug()<<"dec";
            value-=0.05f;
            QString newWord;
            newWord= QString::number(value,'f',2);
            if(fEnd)
                newWord.append('f');
            qDebug()<<newWord;
            tc.removeSelectedText();
            editor->insertPlainText(newWord);
            int startPos = tc.position();
            int endPos = tc.position()-newWord.length();
            tc.setPosition(startPos);
            tc.setPosition(endPos, QTextCursor::KeepAnchor);
            editor->setTextCursor(tc);
        }
    }
}
