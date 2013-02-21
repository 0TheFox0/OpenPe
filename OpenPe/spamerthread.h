#ifndef SPAMERTHREAD_H
#define SPAMERTHREAD_H

#include <QThread>
#include <QStringList>
class SpamerThread : public QThread
{
	Q_OBJECT

public:
	SpamerThread(QObject *parent);
	~SpamerThread();
	QStringList listItems;
	int SpamDelay;
	bool continueSpaming;
	void run();
private:
	
};

#endif // SPAMERTHREAD_H
