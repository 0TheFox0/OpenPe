#include "qcounter.h"

QVector<QPair<QString,int>*> QCounter::GlobalCounters;

QCounter::QCounter(QObject *parent)
	: QObject(parent)
{

}
QCounter::QCounter()
{

}
QCounter::~QCounter()
{

}
int QCounter::find_Gl(QString name)
{
	QMutexLocker locker(&Gl_mutex);
	for (int i=0;i<GlobalCounters.size();i++)
		if (GlobalCounters.at(i)->first == name)
			return i;
	return -1;
}
void QCounter::Create(QString name,int value)
{
	if(find_Gl(name)==-1)
	{
		QPair<QString,int>* nGL = new QPair<QString,int>(name,value);
		GlobalCounters.append(nGL);
	}
}
void QCounter::Create(QString name)
{
	Create(name,0);
}

int QCounter::GetValue(QString name)
{ 
	int index = find_Gl(name);
	if (index!=-1)
		return GlobalCounters.at(index)->second;
	else
		return 0;
}
void QCounter::Delete(QString name)
{
	int index = find_Gl(name);
	if (index!=-1)
		GlobalCounters.remove(index);
}
void QCounter::Inc(QString name)
{
	int index = find_Gl(name);
	if (index!=-1)
		GlobalCounters.at(index)->second = GlobalCounters.at(index)->second + 1;	
}
void QCounter::Dec(QString name)
{
	int index = find_Gl(name);
	if (index!=-1)
		GlobalCounters.at(index)->second = GlobalCounters.at(index)->second - 1;	
}
void QCounter::Set(QString name,int value)
{
	int index = find_Gl(name);
	if (index!=-1)
		GlobalCounters.at(index)->second = value;	
}
void QCounter::Reset(QString name)
{
	Set(name,0);
}