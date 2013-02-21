#ifndef QCOUNTER_H
#define QCOUNTER_H

#include <QObject>
#include <QPair>
#include <QVector>
#include <QMutex>

class QCounter : public QObject
{
	Q_OBJECT

public:
	QCounter(QObject *parent);
	QCounter();
	~QCounter();
public slots:
	void Create(QString,int);
	void Create(QString);
	void Delete(QString);
	int GetValue(QString);
	void Inc(QString);
	void Dec(QString);
	void Set(QString,int);
	void Reset(QString);
private:
	int find_Gl(QString);
	QMutex Gl_mutex;
	static QVector<QPair<QString,int>*> GlobalCounters;
};

#endif // QCOUNTER_H
