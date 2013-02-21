#ifndef OPENPE_H
#define OPENPE_H

#include <QtGui/QMainWindow>
#include "ui_openpe.h"
#include "qblacklist.h"
#include "threadengine.h"
#include "CPacket.h"
#include "qstructlogger.h"
#include "spamerthread.h"
class OpenPE : public QMainWindow
{
	Q_OBJECT

public:
	OpenPE(QWidget *parent = 0, Qt::WFlags flags = 0);
	~OpenPE();
	QBlackList* black_list;
	QStructLogger* struct_Logger;
	bool ignore_As_blackList;
	bool show_send;
	bool show_recv;
	void AddPacket(CPacket& pkt/*,QStructItem* st_item*/);
	bool pattern(CPacket& pkt , QString patter);
	void injectModPacket (CPacket& pkt , QString patter,bool isSend);
	bool preparePacket(QString&,QString&);
private slots:
	void globalSettings(bool);
	void contextMenuSlot_delete();
	void contextMenuSlot_packet();
	//***********************************************
	void fromBLack_added(unsigned short,int,bool);
	void fromBLack_removed(unsigned short,int,bool);
	void fromBlack_blocked_removed();
	void fromBlack_ignored_removed();
	void fromBlack_cleared();
	//*********************************
	void addEdit();
	void editChanged(bool);
	void editActivated(QTreeWidgetItem*,int);
	void deleteEdit();
	//********************************
	void senderSlot();
	void scriptSlot();
	void sender_engine_error(QString);
	void edit_engine_error(QString);
	void sender_engine_ended();
	void script_sms_box(QString,QString);
	void script_output(QString);
	//**********************************
	void expandSlot();
	void loadScript(QString);
	void save_senderCode();

	void itemActivated(QTreeWidgetItem*,int);
	void pktInfoSlot();
private:
	void closeEvent(QCloseEvent *event);
	Ui::OpenPEClass ui;
	ThreadEngine * sender_engine;
	void ProcessFile(QString dir, QString file, bool isSend);
	int alreayAnode(QTreeWidget* tree, QString header);
	
	//*****************************************
	QColor GetPacketColor(PktType);
	QColor GetEditColor(edit_type);
	//*****************************************
	bool checkPattern(QString,QString &);
	

	//*****************************************
	SpamerThread * spammer;
};
extern OpenPE * thisPe;
#endif // OPENPE_H
