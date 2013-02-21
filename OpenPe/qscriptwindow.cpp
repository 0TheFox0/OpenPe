#include "qscriptwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QAction>
#include <QMenu>
QScriptWindow::QScriptWindow(QWidget * parent, Qt::WindowFlags  flags)
	:  QDialog(parent, flags )
{
	ui.setupUi(this);
	sender_engine = new ThreadEngine(this);
	bool connScript = true;
	connScript &= connect(sender_engine,SIGNAL(errorOccurred(QString)),this,SLOT(sender_engine_error(QString)));
	connScript &= connect(sender_engine,SIGNAL(endOccurred()),this,SLOT(sender_engine_ended()));
	connScript &= connect(sender_engine,SIGNAL(smsBox(QString,QString)),this,SLOT(script_sms_box(QString,QString)));
	//connScript &= connect(sender_engine,SIGNAL(outputOcurred(QString)),this,SLOT(script_output(QString)));
	if(!connScript)
		QMessageBox::warning(this,"Internal Error","Can't connect with ScriptEngine",QMessageBox::Ok);

	QString base = qApp->applicationDirPath()+"/Scripts";
	QDir myDir(base);
	if(!myDir.exists())
	{
		myDir.mkdir(base);
		myDir.mkdir(base+"/Send");
		myDir.mkdir(base+"/Recv");
	}
	QStringList filters;
	filters << "*.txt";
	myDir.setNameFilters(filters);
	QStringList list = myDir.entryList();
	list.prepend("Select your saved scripts here...");
	ui.script_Box->addItems(list);

	QAction* atc = ui.sender_code->currPkt->menuAction();
	ui.sender_code->GlobalMenu->removeAction(atc);
}

QScriptWindow::~QScriptWindow()
{

}
void QScriptWindow::loadScript(QString newScript)
{
	if (newScript == "Select your saved scripts here...")
		return;
	/*QString a = newScript;*/
	QString base = qApp->applicationDirPath()+"/Scripts";
	QDir myDir(base);
	QStringList filters;
	filters << "*.txt";
	myDir.setNameFilters(filters);
	QStringList list = myDir.entryList();	
	int itemcount = ui.script_Box->count();
	if(itemcount != list.length())
	{
		ui.script_Box->clear();
		ui.script_Box->addItems(list);
	}
	newScript.prepend(base+"/");
	QFile file(newScript);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream in(&file);
	QString line = in.readAll();
	ui.sender_code->clear();
	ui.sender_code->insertPlainText(line);
	file.close();
}
void QScriptWindow::saveSlot()
{
	QString base = qApp->applicationDirPath()+"/Scripts";
	QFileDialog *  savedlg = new QFileDialog(this);
	savedlg->setFilter("*.txt");
	savedlg->setWindowTitle("Save File...");
	savedlg->setDirectory(base);
	savedlg->exec();
	QString sfile= savedlg->selectedFiles()[0];
	sfile.append(".txt");
	QFile file(sfile);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream out(&file);
	out << ui.sender_code->toPlainText();
	file.close();
	QDir myDir(base);
	QStringList filters;
	filters << "*.txt";
	myDir.setNameFilters(filters);
	QStringList list = myDir.entryList();	
	
	
		ui.script_Box->clear();
		ui.script_Box->addItems(list);
	
}
void QScriptWindow::testSlot()
{
	QString toCheck = ui.sender_code->toPlainText();
	QString result = sender_engine->checkSyntax(toCheck);
	QMessageBox::information(this,"Syntax result",result);
}
void QScriptWindow::runSlot()
{
	if (ui.run_script_btn->text() == "Run")
	{
		QString toCheck = ui.sender_code->toPlainText();
		if (sender_engine->setScript(toCheck))
		{
			QIcon icon7;
			icon7.addFile(QString::fromUtf8(":/OpenPE/Resources/script_delete.png"), QSize(), QIcon::Normal, QIcon::Off);
			ui.run_script_btn->setText("Stop");
			ui.run_script_btn->setIcon(icon7);
			sender_engine->start();
		}
	}
	else
	{
		QIcon icon7;
		icon7.addFile(QString::fromUtf8(":/OpenPE/Resources/script_gear.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.run_script_btn->setText("Run");
		ui.run_script_btn->setIcon(icon7);
		sender_engine->StopIt();
	}
}
void QScriptWindow::sender_engine_error(QString error)
{
	QIcon icon7;
	icon7.addFile(QString::fromUtf8(":/OpenPE/Resources/script_gear.png"), QSize(), QIcon::Normal, QIcon::Off);
	ui.run_script_btn->setText("Run");
	ui.run_script_btn->setIcon(icon7);
	QMessageBox::critical(this,"Script Error",error);
}
void QScriptWindow::sender_engine_ended()
{
	QIcon icon7;
	icon7.addFile(QString::fromUtf8(":/OpenPE/Resources/script_gear.png"), QSize(), QIcon::Normal, QIcon::Off);
	ui.run_script_btn->setText("Run");
	ui.run_script_btn->setIcon(icon7);
}
void QScriptWindow::script_sms_box(QString title,QString body)
{
	QMessageBox::information(this,title,body);
}