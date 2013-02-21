#include "qpacket.h"
#include "openpe.h"
QPacket::QPacket(QObject *parent)
	: QObject(parent)
{

}

QPacket::~QPacket()
{

}

void QPacket::set_Packet(CPacket pkt,bool isSend)
{
	this->Packet = pkt;
	this->isSend = isSend;
}

int QPacket::Size()
{
	return this->Packet.cbData;
}
int  QPacket::ReadByte(int offset)
{
	BYTE ret = 0;
	ret = this->Packet.bData[offset];
	return ret;
}
int  QPacket::ReadWord(int offset)
{
	WORD ret = 0;
	ret = *(WORD*)&this->Packet.bData[offset];		
	return ret;
}
unsigned long  QPacket::ReadDWord(int offset)
{
	DWORD ret = 0;
	ret = *(DWORD*)&this->Packet.bData[offset];		
	return ret;
}
//Edit Functions
void  QPacket::Block()
{
	emit header_changed();
}
bool  QPacket::CheckPattern(QString pat)
{
	return thisPe->pattern(this->Packet,pat);
}
void  QPacket::Mod(QString pat)
{
	emit header_changed();
	return thisPe->injectModPacket(this->Packet,pat,this->isSend);
}
