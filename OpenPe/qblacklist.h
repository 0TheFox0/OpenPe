#ifndef QBLACKLIST_H
#define QBLACKLIST_H

#include <QObject>
#include <QVector>
#include <QMutex>
#include <QMutexLocker>
//#include "CPacket.h"
#include "threadengine.h"
#include <Windows.h>
enum edit_type
{
	NONE , IGNORED , BLOCK , MOD , SCRIPT  
};
struct blackType
{
	WORD header;
	edit_type edit;
	QString from;
	QString to;
	QString script;
};

class QBlackList : public QObject
{
	Q_OBJECT

public:
	QBlackList(QObject *parent);
	~QBlackList();
	bool addList(WORD, edit_type, bool, QString from, QString to, QString script);
	blackType find(WORD, bool,int&);
	bool remove(WORD, bool);
	 //for scripts => "removeAllBlocked()";
	void removeIgnored();
	void clear();
	void runScript(WORD&,CPacket,bool);
public slots:
	void UnBlockAll();
	void Block(QString,unsigned short);
	void UnBlock(QString,unsigned short);
signals:
	void added(unsigned short,int,bool);
	void removed(unsigned short,int,bool);
	void blocked_removed();
	void ignored_removed();
	void cleared();
private:
	int findScriptEngine(WORD,bool);
	QVector<blackType> v_recv_list;
	QVector<blackType> v_send_list;
	QVector <QPair<WORD,ThreadEngine*>*> send_engines;
	QVector <QPair<WORD,ThreadEngine*>*> recv_engines;
	QMutex mutex;
};

#endif // QBLACKLIST_H
