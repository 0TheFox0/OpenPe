#include "foxengine.h"
#include "ScriptFunctions.h"
#include <QMutexLocker>
#include "openpe.h"
#include <QMessageBox>

//QVector<QPair<QString,int>*> FoxEngine::GlobalCounters;
QCounter * FoxEngine::counter = new QCounter();
QScriptFlags* FoxEngine::flags = new QScriptFlags();
StringConverter* FoxEngine::converter = new StringConverter();

FoxEngine::FoxEngine(QObject *parent)
	: QScriptEngine(parent)
{	
	
	pkt = new QPacket(this);

	if(!connect(pkt,SIGNAL(header_changed()),this,SLOT(header_Changed())))
		QMessageBox::critical(thisPe,"Internal Error","Can't connect with Qpacket");

	// OBJECTS ********************************************************************
	QScriptValue scripCounter = this->newQObject(counter);
	this->globalObject().setProperty("Counters", scripCounter);

	QScriptValue scripPacket = this->newQObject(pkt);
	this->globalObject().setProperty("ThisPacket", scripPacket);

	QScriptValue scripFlags = this->newQObject(flags);
	this->globalObject().setProperty("Flags",scripFlags);

	QScriptValue scripBlackList = this->newQObject(thisPe->black_list);
	this->globalObject().setProperty("BlackList",scripBlackList);

	QScriptValue scripconverter = this->newQObject(converter);
	this->globalObject().setProperty("Converter",scripconverter);
	
	 
	// FUNCTIONS *************************************************************
	QScriptValue fun = this->newFunction(ShowMessage);
	this->globalObject().setProperty("ShowMessage",fun);

	QScriptValue fun2 = this->newFunction(ScriptPktSend);
	this->globalObject().setProperty("SendPacket",fun2);

	QScriptValue fun3 = this->newFunction(ScriptPktRecv);
	this->globalObject().setProperty("RecvPacket",fun3);

	QScriptValue fun4 = this->newFunction(ScriptSleep);
	this->globalObject().setProperty("Sleep",fun4);

	QScriptValue fun5 = this->newFunction( ScriptReadPointer);
	this->globalObject().setProperty("ReadPointer",fun5);

	QScriptValue fun6 = this->newFunction(ScriptWritePointer);
	this->globalObject().setProperty("WritePointer",fun6);	

	QScriptValue fun7 = this->newFunction(ScriptFormat);
	this->globalObject().setProperty("Format",fun7);	

	QScriptValue fun8 = this->newFunction(ScriptDoLater);
	this->globalObject().setProperty("DoLater",fun8);	
	
}

FoxEngine::~FoxEngine()
{
	
}

void FoxEngine::set_Packet(CPacket packet,bool isSend)
{
	pkt->set_Packet(packet,isSend);
}


void FoxEngine::emitError(QString err)
{
	emit error(err);
}

void FoxEngine::message(QString title, QString body)
{
	emit smsBox(title,body);
}
void FoxEngine::emitOutput(QString out)
{
	emit output(out);
}
void FoxEngine::set_header(WORD& header)
{
	out_Header = header;
}

void FoxEngine::header_Changed()
{
	out_Header = -1;
}
void FoxEngine::doLater()
{
	this->evaluate(later_Script);
}
