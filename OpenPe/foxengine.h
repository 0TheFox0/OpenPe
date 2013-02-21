#ifndef FOXENGINE_H
#define FOXENGINE_H

#include <QScriptEngine>
#include <QtScript>
#include <QThread>
#include <QPair>
#include <QVector>
#include <QMutex>
#include "MapleHook.h"
#include "CPacket.h"
#include "qcounter.h"
#include "qpacket.h"
#include "qscriptflags.h"
#include "stringconverter.h"

class FoxEngine : public QScriptEngine
{
	Q_OBJECT

public:
	FoxEngine(QObject *parent);
	~FoxEngine();
	/// Packet  ******************************************
	bool isSend;
	WORD out_Header;
	bool needDolater;
	QString later_Script;
	CPacket CurrentPacket;
	
	void set_Packet(CPacket,bool isSend);
	void set_header(WORD&);
	
	void doLater();

	void emitError(QString);
	void emitOutput(QString);
	void message (QString,QString);
signals:
	void smsBox (QString,QString);
	void output(QString);
	void error(QString);
	void ended();
public slots:
	void header_Changed();
private:
	static QCounter* counter;
	static QScriptFlags* flags;
	static StringConverter * converter;
	QPacket * pkt;
	bool check_runnable(QString);	
};

#endif // FOXENGINE_H
