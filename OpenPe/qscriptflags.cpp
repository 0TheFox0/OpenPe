#include "qscriptflags.h"

QVector<QPair<QString,bool>*> QScriptFlags::GlobalFlags;


QScriptFlags::QScriptFlags()
{

}


QScriptFlags::~QScriptFlags()
{

}

void QScriptFlags::Create(QString name ,bool value)
{
	if(find_flag(name)==-1)
	{
		QPair<QString,bool>* nGL = new QPair<QString,bool>(name,value);
		GlobalFlags.append(nGL);
	}
}
void QScriptFlags::Create(QString name)
{
	Create(name,false);
}
void QScriptFlags::Delete(QString name)
{
	int index = find_flag(name);
	if (index!=-1)
		GlobalFlags.remove(index);
}
bool QScriptFlags::GetStatus(QString name)
{
	int index = find_flag(name);
	if (index!=-1)
	{
		return GlobalFlags.at(index)->second;
	}
	return false;
}
void QScriptFlags::Toggle(QString name)
{
	int index = find_flag(name);
	if (index!=-1)
	{
		 GlobalFlags.at(index)->second = !GlobalFlags.at(index)->second;
	}
}
int QScriptFlags::find_flag(QString name)
{
	QMutexLocker locker(&Gl_mutex);
	for (int i=0;i<GlobalFlags.size();i++)
		if (GlobalFlags.at(i)->first == name)
			return i;
	return -1;
}

void QScriptFlags::SetStatus(QString name, bool newValue)
{
	int index = find_flag(name);
	if (index!=-1)
	{
		GlobalFlags.at(index)->second = newValue;
	}
}

