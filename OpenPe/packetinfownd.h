#ifndef PACKETINFOWND_H
#define PACKETINFOWND_H

#include <QDialog>
#include "ui_packetinfownd.h"
#include "qstructitem.h"
class PacketInfoWnd : public QDialog
{
	Q_OBJECT

public:
	PacketInfoWnd(QWidget *parent = 0);
	PacketInfoWnd(QWidget *parent = 0 ,QStructItem* info = NULL, QString data = "");
	~PacketInfoWnd();

private:
	Ui::PacketInfoWnd ui;
};

#endif // PACKETINFOWND_H
