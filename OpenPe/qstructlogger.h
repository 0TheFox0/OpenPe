#ifndef QSTRUCTLOGGER_H
#define QSTRUCTLOGGER_H

#include <QObject>
#include "MapleHook.h"
#include "qstructitem.h"
#include <QMutex>
#include <QVector>
#include <QPair>
class QStructLogger : public QObject
{
	Q_OBJECT

public:
	QStructLogger(QObject *parent);
	~QStructLogger();
	void SaddItem(COutPacket* item,QStructItem*);
	void Sadd1(COutPacket * item,BYTE value,DWORD retAddy);
	void Sadd2(COutPacket * item,WORD value,DWORD retAddy);
	void Sadd4(COutPacket * item,DWORD value,DWORD retAddy);
	void SaddStr(COutPacket* item,LPCSTR value,DWORD retAddy);
	void SaddBuff(COutPacket* item,__in_bcount(uLength) PBYTE pb, __in UINT uLength,DWORD retAddy);
	void SdeleteItem(COutPacket* item);
	QStructItem* SgetItem(COutPacket*);

	void addItem(CInPacket* item,QStructItem*);
	void add1(CInPacket * item,BYTE value,DWORD retAddy);
	void add2(CInPacket  * item,WORD value,DWORD retAddy);
	void add4(CInPacket  * item,DWORD value,DWORD retAddy);
	void addStr(CInPacket * item,LPCSTR* value,DWORD retAddy);
	void addBuff(CInPacket * item,__in_bcount(uLength) PBYTE pb, __in UINT uLength,DWORD retAddy);
	void deleteItem(CInPacket* item);

	
private:
	int Sfind_Item(COutPacket*);
	int find_Item(CInPacket*);
	QMutex send_mutex;
	QMutex recv_mutex;
	QVector <QPair<COutPacket*,QStructItem*>*> send_items;
	QVector <QPair<CInPacket*,QStructItem*>*> recv_items;

};

#endif // QSTRUCTLOGGER_H
