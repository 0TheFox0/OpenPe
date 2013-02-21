#ifndef QSCRIPTFLAGS_H
#define QSCRIPTFLAGS_H

#include <QObject>
#include <QPair>
#include <QVector>
#include <QMutex>
class QScriptFlags : public QObject
{
	Q_OBJECT
public:
		QScriptFlags::QScriptFlags();
		~QScriptFlags();
public slots:
	void Create(QString,bool);
	void Create(QString);
	void Delete(QString);
	bool GetStatus(QString);
	void SetStatus(QString, bool);
	void Toggle(QString);
private:
	int find_flag(QString);
	QMutex Gl_mutex;
	static QVector<QPair<QString,bool>*> GlobalFlags;

	
};

#endif // QSCRIPTFLAGS_H
