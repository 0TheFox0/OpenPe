#ifndef THREADENGINE_H
#define THREADENGINE_H

#include <QThread>
#include "foxengine.h"
class ThreadEngine : public QThread
{
	Q_OBJECT

public:
	ThreadEngine(QObject *parent);
	~ThreadEngine();
	void run();
	void DoIt(WORD&);
	void StopIt();
	bool setScript(QString);
	QString checkSyntax(QString);
	
	void set_Packet(CPacket,bool);
private:
	bool isRunnable;
	QString script;
	FoxEngine * engine;
private slots:
	void sms (QString,QString);
	void output(QString);
	void error(QString);
	void ended();
signals:
	void smsBox(QString,QString);
	void outputOcurred(QString);
	void errorOccurred(QString);
	void endOccurred();
};

#endif // THREADENGINE_H
