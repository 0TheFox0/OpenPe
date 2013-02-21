#ifndef QSCRIPTWINDOW_H
#define QSCRIPTWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "ui_qscriptwindow.h"
#include "threadengine.h"
#include <QDialog>

class QScriptWindow : public QDialog
{
	Q_OBJECT

public:
	QScriptWindow(QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~QScriptWindow();
private slots:
	void loadScript(QString);
	void saveSlot();
	void testSlot();
	void runSlot();
	void sender_engine_error(QString);
	void sender_engine_ended();
	void script_sms_box(QString,QString);
private:
	Ui::QScriptWindow ui;
	ThreadEngine * sender_engine;
};

#endif // QSCRIPTWINDOW_H
