#ifndef QPACKET_H
#define QPACKET_H

#include <QObject>
#include "CPacket.h"
class QPacket : public QObject
{
	Q_OBJECT

public:
	QPacket(QObject *parent);
	~QPacket();
	void set_Packet(CPacket,bool);
signals:
	void header_changed();
public slots:
		int Size();
		int ReadByte(int);
		int ReadWord(int);
		unsigned long  ReadDWord(int);
		//Edit Functions
		void Block();
		bool CheckPattern(QString);
		void Mod(QString);
private:
	CPacket Packet;
	bool isSend;
};

#endif // QPACKET_H
