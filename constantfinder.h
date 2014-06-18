#ifndef CONSTANTFINDER_H
#define CONSTANTFINDER_H

#include <QString>
#include <vector>

template <class T>
class ConstData
{
public:
    int start;
    int end;
    T data;
    //0 int
    //1 float
    ConstData()
    {
        start=0;
        end=0;
        data=0;
    }

    ConstData(int s,int e,T d)
    {
        start=s;
        end = e;
        data = d;
    }

    ConstData(const ConstData &copy)
    {
        start = copy.start;
        end = copy.end;
        data = copy.data;
    }
};

using namespace std;

class ConstantFinder
{
public:
    ConstantFinder(QString text);
    ConstData<int>* intArray;
    ConstData<float>* floatArray;
    int intArraySize;
    int floatArraySize;
    ~ConstantFinder();

private:
    vector < ConstData<int> > intList;
    vector < ConstData<float> > floatList;
    void processListData();
    void printListData();
    void deleteLists();
    void printArrayData();
    void processData(QString data,bool isFloat,int start,int end);
    QString text;
    //bool isConstOperator(QChar q);
};

#endif // CONSTANTFINDER_H
