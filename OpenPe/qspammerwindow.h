#ifndef QSPAMMERWINDOW_H
#define QSPAMMERWINDOW_H

#include <QDialog>
#include "ui_qspammerwindow.h"
#include <boost/thread.hpp>
#include "spamerthread.h"
class QSpammerWindow : public QDialog
{
	Q_OBJECT

public:
	QSpammerWindow(QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~QSpammerWindow();
private slots:
	void sendSlot();
	void spamSlot();
private:
	SpamerThread * spammer;
	Ui::QSpammerWindow ui;
};

#endif // QSPAMMERWINDOW_H
