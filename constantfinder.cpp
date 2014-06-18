#include "constantfinder.h"
#include <QString>
#include <QChar>
#include <QDebug>
ConstantFinder::ConstantFinder(QString textSent)
{
    this->text = textSent;
    bool isCommentOld=false;
    bool isCommentNew=false;
    bool isMacro=false;
    bool isConst=false;
    bool isFloat=false;
    int startOfData=-1;
    QString data="";
    for(int i=0;i<text.length();i++)
    {
        QChar pChar = text.at(i);


        if(isCommentOld)
        {
            if((pChar=='*')&&(text.at(i+1)=='/'))
            {
                isCommentOld=false;
                i++;
            }
            continue;
        }
        else if(isCommentNew)
        {
            if(pChar=='\n')
            {
                isCommentNew=false;
            }
            else
                continue;
        }
        else if(isMacro)
        {
            if(pChar=='\n')
            {
                isMacro=false;
            }
            else
                continue;
        }
        else if(isConst)
        {
            if(pChar=='f')
            {
                isFloat=true;
            }
            else if(pChar=='.')
            {
                isFloat=true;
            }
            else if(pChar.isDigit())
            {

            }
            else
            {
                if(startOfData==-1)
                {
                    qDebug() <<data << ": startOfData = -1, Error in constantFinder class";
                    return;
                }

                processData(data,isFloat,startOfData,i-1); //create list since amount of elements are unknown now
                isFloat=false;
                isConst=false;
                startOfData = -1;
                //qDebug()<<data;
                data.clear();
                //printf_s("\n");
            }
        }



        if(pChar=='#')
        {
            isMacro=true;
            continue;
        }
        else if(pChar=='/')
        {
            if(text.at(i+1)=='/')
            {
                isCommentNew=true;
                i++;
                continue;
            }
            else if(text.at(i+1)=='*')
            {
                isCommentOld=true;
                i++;
                continue;
            }
        }
        else if(pChar.isDigit() && !isConst)
        {
            if(!(text.at(i-1).isLetter()))
            {
                startOfData = i;
                isConst=true;
            }
            else
                continue;
        }
        else if(pChar=='-' && !isConst && text.at(i+1).isDigit())
        {
            if(!(text.at(i-1).isLetter()))
            {
                startOfData = i;
                isConst=true;
            }
            else
                continue;
        }

        if(isConst)
        {
            data.append(pChar);
            //printf_s("%c",pChar);
        }
    }

    //printListData();
    processListData(); //send data of list to array for faster access
    deleteLists();     // delete list
    //printArrayData();

}

ConstantFinder::~ConstantFinder()
{
    delete intArray;
    delete floatArray;
}

void ConstantFinder::processListData()
{
    intArraySize = intList.size();
    floatArraySize = floatList.size();

    intArray = new ConstData<int> [intArraySize];
    floatArray = new ConstData<float> [floatArraySize];

    int index=0;
    for(auto it=intList.cbegin();it!=intList.cend();it++)
    {
        ConstData<int> constantData = *it;
        intArray[index]=constantData;
        index++;
        //qDebug() << constantData.start << " " << constantData.end << " " << constantData.data;
    }

    qDebug()<<"ints inserted:" << index;

    index = 0;

    for(auto it=floatList.cbegin();it!=floatList.cend();it++)
    {
        ConstData<float> constantData = *it;
        floatArray[index]=constantData;
        index++;
        //qDebug() << constantData.start << " " << constantData.end << " " << constantData.data;
    }

    qDebug()<<"floats inserted:" << index;
}


void ConstantFinder::printListData()
{
    qDebug() << "Int list size:" << intList.size();
    qDebug() << "Float list size:" << floatList.size();
    qDebug() << "\nInt data:";
    for(auto it=intList.cbegin();it!=intList.cend();it++)
    {
        ConstData<int> constantData = *it;
        qDebug() << constantData.start << " " << constantData.end << " " << constantData.data;
    }

    qDebug() << "\nFloat data:";

    for(auto it=floatList.cbegin();it!=floatList.cend();it++)
    {
        ConstData<float> constantData = *it;
        qDebug() << constantData.start << " " << constantData.end << " " << constantData.data;
    }
}

void ConstantFinder::deleteLists()
{
    intList.clear();
    floatList.clear();
}

void ConstantFinder::printArrayData()
{
    qDebug() << "\nint data:";
    for(int i=0;i<intArraySize;i++)
    {
        ConstData<int> constantData = intArray[i];
        qDebug() << constantData.start << " " << constantData.end << " " << constantData.data;
    }

    qDebug() << "\nFloat data:";
    for(int i=0;i<floatArraySize;i++)
    {
        ConstData<float> constantData = floatArray[i];
        qDebug() << constantData.start << " " << constantData.end << " " << constantData.data;
    }
}

void ConstantFinder::processData(QString data, bool isFloat,int start,int end)
{
    if(isFloat)
    {
        //qDebug()<<data << " " << "float";
        if(data.at(data.length()-1)=='f')
        {
            data.remove(data.length()-1,1);
        }
        bool converted=false;
        float value = data.toFloat(&converted);
        if(converted)
        {
            ConstData<float> constantData(start,end,value);
            floatList.push_back(constantData);
            //qDebug() << constantData.start << " " << constantData.end << " " << constantData.data;
        }
        else
        {
            qDebug()<<"Conversion failed in processData func() of constantFinder class!";
            return;
        }
    }
    else
    {
        //qDebug()<<data << " " << "int";
        bool converted=false;
        int value = data.toInt(&converted);
        if(converted)
        {
            ConstData<int> constantData(start,end,value);
            intList.push_back(constantData);
            //qDebug() << constantData.start << " " << constantData.end << " " << constantData.data;
        }
        else
        {
            qDebug()<<"Conversion failed in processData func() of constantFinder class!";
            return;
        }
    }
}





//bool ConstantFinder::isConstOperator(QChar q)
//{

//    if( (q=='/')||
//        (q=='*')||
//        (q=='+')||
//        (q=='-')||
//        (q=='&')||
//        (q=='|')||
//        (q=='%')||
//        (q=='<')||
//        (q=='>')||
//        (q=='=')||
//        (q=='!')||
//        (q=='~')||
//        (q==':')||
//        (q=='^') )
//        return true;
//    else
//        return false;

//}

