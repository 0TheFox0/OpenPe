#include "qspammerwindow.h"
#include <QMessageBox>
#include "CPacket.h"
#include "openpe.h"

QSpammerWindow::QSpammerWindow(QWidget * parent, Qt::WindowFlags  flags)
	: QDialog(parent, flags )
{
	ui.setupUi(this);
	spammer = new SpamerThread(this);
}

QSpammerWindow::~QSpammerWindow()
{

}

void QSpammerWindow::sendSlot()
{
	QStringList s_list= ui.multi_pkt_txt->toPlainText().split("\n");
	QString error;
	std::string strError;
	for each (QString mss in s_list)
	{
		if(!thisPe->preparePacket(mss,error))
		{
			QMessageBox::critical(this,"Failed to prepare the packet",error,QMessageBox::Ok);
			return;
		}			
	}
	SpamerThread * spam = new SpamerThread(this);
	spam->listItems = s_list;
	spam->SpamDelay = 0;
	spam->continueSpaming=false;
	spam->start();
}
void QSpammerWindow::spamSlot()
{
	if (ui.spam_multi_btn->text() == "Spam it!")
	{
		QString error;
		QStringList Packets = this->ui.multi_pkt_txt->toPlainText().split('\n');
		for each (QString s in Packets)
		{
			if(!thisPe->preparePacket(s,error))
			{
				QMessageBox::critical(this,"Failed to prepare the packets",error,QMessageBox::Ok);
				return;
			}
		}
		ui.spam_multi_btn->setText("Stop!");
		QIcon icon7;
		icon7.addFile(QString::fromUtf8(":/OpenPE/Resources/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.spam_multi_btn->setIcon(icon7);

		int delay = ui.spam_delay->value();

		spammer->listItems = Packets;
		spammer->continueSpaming = true;
		spammer->SpamDelay = delay;
		spammer->start();
	}
	else // already running
	{
		spammer->continueSpaming = false;
		ui.spam_multi_btn->setText("Spam it!");
		QIcon icon7;
		icon7.addFile(QString::fromUtf8(":/OpenPE/Resources/spam.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.spam_multi_btn->setIcon(icon7);			
	}
}
