#include "glsleditor.h"

#include <QtGui>
#include <QToolTip>
GLSLEditor::GLSLEditor(QWidget *parent) :
    QWidget(parent)
{
    setupEditor();
    QHBoxLayout *editorLayout = new QHBoxLayout;
    editorLayout->addWidget(editor);
    this->setLayout(editorLayout);

    signalMapper = new QSignalMapper(this);
    connect(editor,SIGNAL(textChanged()),signalMapper,SLOT(map()));
    signalMapper->setMapping(editor,(QObject*)this);
    connect(signalMapper, SIGNAL(mapped(QObject*)), this, SIGNAL(textChanged(QObject*)));
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
    //connect(editor,SIGNAL(selectionChanged()),this,SLOT(cursorMoveHandle()));

    highlighter = new Highlighter(editor->document());

    QFile file("../GLSLEditor/Basic.fsh");

    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        text = file.readAll();
        editor->setPlainText(text);
    }
    else
        printf_s("No file :(\n");
}

//void GLSLEditor::cursorMoveHandle()
//{
//    QTextCursor tc = editor->textCursor();
//    //tc.select(QTextCursor::WordUnderCursor);
//    QString word = tc.selectedText();
//    qDebug() << word;
//    qDebug() << tc.position();
//}


void GLSLEditor::keyPressEvent ( QKeyEvent * keyEvent )
{
    keysPressed+= keyEvent->key();


    if(keysPressed.contains(Qt::Key_Control)&& keysPressed.contains(Qt::Key_Comma))
    {
        handleValueChange("DEC");
    }
    else if(keysPressed.contains(Qt::Key_Control) && keysPressed.contains(Qt::Key_Period))
    {
        handleValueChange("INC");
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
        bool converted=false;
        float value = word.toFloat(&converted);
        if(converted)
            qDebug()<<value;
        else
        {
            qDebug()<<"Conversion fail!";
            return;
        }

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
            text=editor->toPlainText();
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
            text=editor->toPlainText();
        }
    }
}

void GLSLEditor::handleValueChange(QString mode)
{
    QTextCursor tc = editor->textCursor();
    emit sendValueChanged(tc.position(),mode);
}

void GLSLEditor::getValueChangedInfo(int start, int end, float newValueFloat, int newValueInt, bool isInt)
{
    QPoint offsetPoint(0,-30);
    QTextCursor tc = editor->textCursor();
    QString newWord;
    if(isInt)
        newWord= QString::number(newValueInt,10);
     else
        newWord= QString::number(newValueFloat,'f',2);
    QToolTip::showText( this->mapToGlobal(editor->cursorRect().topLeft() + offsetPoint), newWord);
    qDebug()<<newWord;
}

void GLSLEditor::getCode(QString code)
{
    editor->setPlainText(code);
}
