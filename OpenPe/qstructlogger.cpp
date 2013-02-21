#include "qstructlogger.h"

QStructLogger::QStructLogger(QObject *parent)
	: QObject(parent)
{

}

QStructLogger::~QStructLogger()
{

}
int QStructLogger::find_Item(CInPacket* item)
{
	for (int i=0;i<recv_items.size();i++)
	{
		if (recv_items.at(i)->first == item)
		{
			return i;
		}
	}
	return -1;
}
int QStructLogger::Sfind_Item(COutPacket* item)
{
	for (int i=0;i<send_items.size();i++)
	{
		if (send_items.at(i)->first == item)
		{
			return i;
		}
	}
	return -1;
}
void QStructLogger::SaddItem(COutPacket* item, QStructItem* st_item)
{
	QMutexLocker locker(&send_mutex);
	int index= Sfind_Item(item);
	if (index == -1)
	{
		QPair <COutPacket*,QStructItem*>* pair = new QPair <COutPacket*,QStructItem*>(item,st_item);
		send_items.append(pair);
	}
}
void QStructLogger::Sadd1(COutPacket * item,BYTE value,DWORD retAddy)
{
	QMutexLocker locker(&send_mutex);
	int index= Sfind_Item(item);
	if (index != -1)
	{
		send_items[index]->second->add1(value,retAddy);
	}
}
void QStructLogger::Sadd2(COutPacket * item,WORD value,DWORD retAddy)
{
	QMutexLocker locker(&send_mutex);
	int index= Sfind_Item(item);
	if (index != -1)
	{
		send_items[index]->second->add2(value,retAddy);
	}
}
void QStructLogger::Sadd4(COutPacket * item,DWORD value,DWORD retAddy)
{
	QMutexLocker locker(&send_mutex);
	int index= Sfind_Item(item);
	if (index != -1)
	{
		send_items[index]->second->add4(value,retAddy);
	}
}
void QStructLogger::SaddStr(COutPacket* item,LPCSTR value,DWORD retAddy)
{
	QMutexLocker locker(&send_mutex);
	int index= Sfind_Item(item);
	if (index != -1)
	{
		send_items[index]->second->SaddStr(value,retAddy);
	}
}
void QStructLogger::SaddBuff(COutPacket* item,__in_bcount(uLength) PBYTE pb, __in UINT uLength,DWORD retAddy)
{
	QMutexLocker locker(&send_mutex);
	int index= Sfind_Item(item);
	if (index != -1)
	{
		send_items[index]->second->addBuff(pb,uLength,retAddy);
	}
}
void QStructLogger::SdeleteItem(COutPacket* item)
{
	QMutexLocker locker(&send_mutex);
	int index= Sfind_Item(item);
	if (index != -1)
	{
		send_items.remove(index);
	}
}

void QStructLogger::addItem(CInPacket* item,QStructItem* st_item)
{
	QMutexLocker locker(&recv_mutex);
	int index= find_Item(item);
	if (index == -1)
	{
		QPair <CInPacket*,QStructItem*>* pair = new QPair <CInPacket*,QStructItem*>(item,st_item);
		recv_items.append(pair);
	}
}
void QStructLogger::add1(CInPacket * item,BYTE value,DWORD retAddy)
{
	QMutexLocker locker(&recv_mutex);
	int index= find_Item(item);
	if (index != -1)
	{
		recv_items[index]->second->add1(value,retAddy);
	}
}
void QStructLogger::add2(CInPacket * item,WORD value,DWORD retAddy)
{
	QMutexLocker locker(&recv_mutex);
	int index= find_Item(item);
	if (index != -1)
	{
		recv_items[index]->second->add2(value,retAddy);
	}
}
void QStructLogger::add4(CInPacket  * item,DWORD value,DWORD retAddy)
{
	QMutexLocker locker(&recv_mutex);
	int index= find_Item(item);
	if (index != -1)
	{
		recv_items[index]->second->add4(value,retAddy);
	}
}
void QStructLogger::addStr(CInPacket * item,LPCSTR* value,DWORD retAddy)
{
	QMutexLocker locker(&recv_mutex);
	int index= find_Item(item);
	if (index != -1)
	{
		recv_items[index]->second->addStr(value,retAddy);
	}
}
void QStructLogger::addBuff(CInPacket * item,__in_bcount(uLength) PBYTE pb, __in UINT uLength,DWORD retAddy)
{
	QMutexLocker locker(&recv_mutex);
	int index= find_Item(item);
	if (index != -1)
	{
		recv_items[index]->second->addBuff(pb,uLength,retAddy);
	}
}
void QStructLogger::deleteItem(CInPacket* item)
{
	QMutexLocker locker(&recv_mutex);
	int index= find_Item(item);
	if (index != -1)
	{
		recv_items.remove(index);
	}
}


QStructItem* QStructLogger::SgetItem(COutPacket* pkt)
{
	QMutexLocker locker(&send_mutex);
	int index= Sfind_Item(pkt);
	if (index != -1)
	{
		return send_items[index]->second;
	}
	return NULL;
}