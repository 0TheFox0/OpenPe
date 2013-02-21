#include "openpe.h"
#include <QMessageBox>
#include <QClipboard>
#include <QInputDialog>
#include<QCompleter>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include "qscriptwindow.h"
#include "qspammerwindow.h"
#include "foxtreeitem.h"
#include "packetinfownd.h"
#include "spamerthread.h"

OpenPE * thisPe;

OpenPE::OpenPE(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	thisPe = this;
	black_list = new QBlackList(this);
	struct_Logger = new QStructLogger(this);
	spammer = new SpamerThread(this);
	ignore_As_blackList = true;

	bool connBlckLt = true;
	connBlckLt &= connect(black_list, SIGNAL(added(unsigned short,int,bool)), this , SLOT(fromBLack_added(unsigned short,int,bool)));
	connBlckLt &= connect(black_list,SIGNAL (removed(unsigned short,int,bool)), this, SLOT(fromBLack_removed(unsigned short,int,bool)));
	connBlckLt &= connect(black_list, SIGNAL(cleared()), this ,SLOT(fromBlack_cleared()));
	connBlckLt &= connect(black_list, SIGNAL(blocked_removed()),this,SLOT(fromBlack_blocked_removed()));
	connBlckLt &= connect(black_list, SIGNAL(ignored_removed()),this,SLOT(fromBlack_ignored_removed()));
	if(!connBlckLt)
		QMessageBox::warning(this,"Internal Error","Can't connect with BlackList",QMessageBox::Ok);

	
	sender_engine = new ThreadEngine(this);
	bool connScript = true;
	connScript &= connect(sender_engine,SIGNAL(errorOccurred(QString)),this,SLOT(sender_engine_error(QString)));
	connScript &= connect(sender_engine,SIGNAL(endOccurred()),this,SLOT(sender_engine_ended()));
	connScript &= connect(sender_engine,SIGNAL(smsBox(QString,QString)),this,SLOT(script_sms_box(QString,QString)));
	connScript &= connect(sender_engine,SIGNAL(outputOcurred(QString)),this,SLOT(script_output(QString)));
	if(!connScript)
		QMessageBox::warning(this,"Internal Error","Can't connect with ScriptEngine",QMessageBox::Ok);
	
	QList<QAction*> context_actions;	
	
	context_actions.append(ui.actionIgnore_opcode);
	context_actions.append(ui.actionBlock_opcode);
	context_actions.append(ui.actionCopy_packet);
	context_actions.append(ui.actionPacket_Info);
	context_actions.append(ui.actionDelete_Selected);
	context_actions.append(ui.actionClear_this_log);
	context_actions.append(ui.actionClear_log);

	ui.flow_tree->addActions(context_actions);
	ui.send_tree->addActions(context_actions);
	ui.recv_tree->addActions(context_actions);

	ui.edits_tree->addAction(ui.actionDelete_Seleted_Edit);

	ui.ignored_tree->addAction(ui.actionAdd_Ignore_opcode);
	ui.ignored_tree->addAction(ui.actionDelete_Selected);

	QAction* atc = ui.sender_code->currPkt->menuAction();
	ui.sender_code->GlobalMenu->removeAction(atc);

	//QDir myDir(qApp->applicationDirPath()+"/Scripts");
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
	

	QDir send_opcodes(base+"/Send");
	if(!send_opcodes.exists())
	{
			myDir.mkdir(base+"/Send");
	}
	QStringList sfilters;
	sfilters << "*.txt";
	send_opcodes.setNameFilters(sfilters);
	QStringList send_list = send_opcodes.entryList();
	for each (QString s in send_list)
	{
		ProcessFile(base+"/Send",s,true);
	}

	QDir recv_opcodes(base+"/Recv");
	if(!recv_opcodes.exists())
	{
		myDir.mkdir(base+"/Recv");
	}
	QStringList rfilters;
	rfilters << "*.txt";
	send_opcodes.setNameFilters(rfilters);
	QStringList recv_list = recv_opcodes.entryList();
	for each (QString s in recv_list)
	{
		ProcessFile(base+"/Recv",s,false);
	}
}

OpenPE::~OpenPE()
{

}
void OpenPE::closeEvent(QCloseEvent *event)
{
	if(ui.hook_chk->isChecked())
	{
		setHooks(reinterpret_cast<PVOID*>(&_SendPacket),0x004C8B90,&SendPacket,false);
		setHooks(reinterpret_cast<PVOID*>(&_ProcessPacket),0x004C9310,&ProcessPacket,false);
		//FIXME: uncomment
		//SetDecodeHooks(false);
	}
	QMessageBox::StandardButton ret;
		ret = QMessageBox::warning(this, tr("Renovatio is closing..."),
			tr("Want to close MapleStory too?"),QMessageBox::Yes | QMessageBox::No);

		if( ret == QMessageBox::Yes )
			TerminateProcess(GetCurrentProcess(),0);
}
QColor OpenPE::GetPacketColor(PktType type)
{
	//blue for send, orange for recv
	static const QColor colors[] =
	{
		QColor::fromRgb(127, 170, 230), //blue
		QColor::fromRgb(255, 120, 40) //orange
	};

	return colors[(type == PktType::Out ? 0 : 1)];
}

QColor OpenPE::GetEditColor(edit_type type)
{
	//blue for send, orange for recv
	static const QColor Ecolors[] =
	{
		QColor::fromRgb(255,69,0), //block
		QColor::fromRgb(153,50,204), //patter
		QColor::fromRgb(34,139,34), //script
	};

	switch (type)
	{
	case BLOCK:
		return Ecolors[0];
	case MOD:
		return Ecolors[1];
	default:
		return Ecolors[2];
	}
}

//black list slots
void OpenPE::fromBLack_added(unsigned short header,int type,bool isSend)
{
	QTreeWidgetItem* item = new QTreeWidgetItem();
	QString sWord = QString().sprintf("0x%04X",header);
	QString Direction = isSend ? "Send" : "Recv";
	QString edit;
	QTreeWidget* tree = ui.edits_tree;
	edit_type typ;
	switch (type)
	{
		case BLOCK:
			edit = "Block";
			typ = BLOCK;
		break;
		case MOD:
			edit = "Pattern Mod";
			typ = MOD;
		break;
		case SCRIPT:
			edit = "Script";
			typ = SCRIPT;
		break;
		default://case IGNORED
			tree = ui.ignored_tree;
			edit = "Ignored";
			typ = IGNORED;
	}
	item->setText(0,sWord);
	item->setText(1,Direction);
	item->setText(2,edit);
	item->setTextColor(0,GetPacketColor(isSend ? Out : In));
	item->setTextColor(1,GetPacketColor(isSend ? Out : In));
	item->setTextColor(2,GetEditColor(typ));

	tree->addTopLevelItem(item);
}

void OpenPE::fromBLack_removed(unsigned short header,int type,bool isSend)
{
	QString sWord = QString().sprintf("0x%04X",header);
	QString Direction = isSend ? "Send" : "Recv";
	if (type == IGNORED)
	{
		QList<QTreeWidgetItem*> match = ui.ignored_tree->findItems(sWord,Qt::MatchFlag::MatchCaseSensitive ,0);
		for each (QTreeWidgetItem* item in match)
		{
			if (item->text(1) == Direction)
			{
				delete item;
			}
		}
	}
	else
	{
		QList<QTreeWidgetItem*> match = ui.edits_tree->findItems(sWord,Qt::MatchFlag::MatchCaseSensitive ,0);
		for each (QTreeWidgetItem* item in match)
		{
			if (item->text(1) == Direction)
			{
				delete item;
			}
		}
	}
}

void OpenPE::fromBlack_blocked_removed()
{
	QList<QTreeWidgetItem*> match = ui.edits_tree->findItems("Block",Qt::MatchFlag::MatchCaseSensitive ,2);
	for each (QTreeWidgetItem* item in match)
		delete item;
}

void OpenPE::fromBlack_ignored_removed()
{
   ui.ignored_tree->clear();
}

void OpenPE::fromBlack_cleared()
{
	ui.ignored_tree->clear();
	ui.edits_tree->clear();
}

void  OpenPE::addEdit()
{
	QString sWord = ui.opcode_edit_txt->text();
	bool isSend = ui.edit_send_chk->isChecked();
	bool canBeHex;
	WORD header = sWord.toLong(&canBeHex,16);
	if(!canBeHex)
	{
		QMessageBox::warning(this,"Error in Opcode","Invalid hex value",QMessageBox::Ok);
		return;
	}
	QString from; QString to; QString script;
	edit_type edit;
	if (ui.edit_block_chk->isChecked())
	{
		edit = BLOCK;
		from="";to=""; script="";
	}
	else if(ui.edit_pattern_chk->isChecked())
	{
		QString Error;
		edit = MOD;
		to = ui.patt_to_txt->text();
		from = ui.patt_from_txt->text();
		if(!checkPattern(from,Error))
		{
			QMessageBox::warning(this,"Error in Pattern => \"From\"",Error,QMessageBox::Ok);
			return;
		}
		if(!checkPattern(to,Error))
		{
			QMessageBox::warning(this,"Error in Pattern => \"To\"",Error,QMessageBox::Ok);
			return;
		}
		script ="";
	}
	else
	{
		edit = SCRIPT;
		from="";to=""; 
		script=ui.edit_code->toPlainText();
	}
	
	if(!black_list->addList(header,edit,isSend,from,to,script))
		QMessageBox::warning(this,"Error","This opcode is already in the edit list.\nPlease delete it first.");
}

void OpenPE::editChanged(bool state)
{
	QObject* pSender = sender();
	if (state)
	{
		if (pSender == ui.edit_block_chk)
		{
			ui.pattern_group->setEnabled(false);
			ui.script_group->setEnabled(false);
		}
		else if (pSender == ui.edit_pattern_chk)
		{
			ui.pattern_group->setEnabled(true);
			ui.script_group->setEnabled(false);
		} 
		else
		{
			ui.pattern_group->setEnabled(false);
			ui.script_group->setEnabled(true);
		}
	}
}

void OpenPE::editActivated(QTreeWidgetItem* item ,int colum)
{
	QString sHeader = item->text(0).mid(2);
	WORD header = sHeader.toLong(0,16);
	bool isSend = item->text(1) == "Send";
	int index;
	blackType blackItem = black_list->find(header,isSend,index);
	switch (blackItem.edit)
	{
	case BLOCK:
		ui.patt_to_txt->setText("");
		ui.patt_from_txt->setText("");
		//ui.pattern_group->setEnabled(false);
		ui.edit_code->clear();
		//ui.script_group->setEnabled(false);
		ui.edit_block_chk->setChecked(true);
	break;
	case MOD:
		ui.patt_to_txt->setText(blackItem.to);
		ui.patt_from_txt->setText(blackItem.from);
		//ui.pattern_group->setEnabled(false);
		ui.edit_code->clear();
		//ui.script_group->setEnabled(false);
		ui.edit_pattern_chk->setChecked(true);
	break;
	default:
		ui.patt_to_txt->setText("");
		ui.patt_from_txt->setText("");
		//ui.pattern_group->setEnabled(false);
		ui.edit_code->clear();
		ui.edit_code->insertPlainText(blackItem.script);
		//ui.script_group->setEnabled(false);
		ui.edit_script_chk->setChecked(true);
	}
	ui.opcode_edit_txt->setText(sHeader);
}

void OpenPE::deleteEdit()
{
	if(ui.edits_tree->topLevelItemCount() == 0)
		return;
	QList<QTreeWidgetItem*> items = ui.edits_tree->selectedItems();
	if(items.isEmpty())
		return;
	QTreeWidgetItem* item = items[0];
	QString sHeader = item->text(0).mid(2);
	WORD header = sHeader.toLong(0,16);
	bool isSend = item->text(1) == "Send";
	int index;
	black_list->remove(header,isSend);
}

bool OpenPE::checkPattern(QString pattern, QString &Error)
{
	QString aux = pattern.remove("?");
	aux.remove("...");
	Error = "No error found";
	aux = aux.remove(" ");
	if ( (aux.length()%2) != 0)
	{
		Error="Check pattern length!";
		return false;
	}
	for (int i=0;i<aux.length();i++)
	{
		if (aux.at(i) >= QChar('a') && aux.at(i) <= QChar('f'))
			continue;
		if (aux.at(i) >= QChar('A') && aux.at(i) <= QChar('F'))
			continue;
		if (aux.at(i) >= QChar('0') && aux.at(i) <= QChar('9'))
			continue;
		Error="Found invalid character in pattern!";
		return false;
	}
	return true;
}

bool OpenPE::preparePacket(QString& input, QString& Error)
{
	QString aux = input.remove(" ");
	//timestamp here?
	if ( (aux.length()%2) != 0)
	{
		Error="Check packet length!";
		return false;
	}
	for (int i=0;i<aux.length();i++)
	{
		if (aux.at(i) >= QChar('a') && aux.at(i) <= QChar('f'))
			continue;
		if (aux.at(i) >= QChar('A') && aux.at(i) <= QChar('F'))
			continue;
		if (aux.at(i) >= QChar('0') && aux.at(i) <= QChar('9'))
			continue;
		if (aux.at(i) == '*')
			continue;
		Error="Found invalid character in packet!";
		return false;
	}

	int ilength = (aux.length()-2)/2;
	for(int n=0, i=2 ; n<ilength; i+=3 , n++)
	{
		aux.insert(i," ");
	}
	input = aux;
	return true;
}

void OpenPE::senderSlot()
{
	QObject * pSender = sender();
	//Send single packet
	//***************************************************************************************
	if (pSender == ui.send_single_btn)
	{
		std::string strError;
		QString error;
		QString mss = ui.single_pkt_txt->text();
		if(!preparePacket(mss,error))
			QMessageBox::critical(this,"Failed to prepare the packet",error,QMessageBox::Ok);
		if (!CPacket::SendPacket(mss.toStdString(), strError))
			QMessageBox::critical(this, "Failed to send packet.", strError.c_str());
	} 
	//*****************************************************************************************
	//Send packets in multiline
	else if (pSender == ui.send_multi_btn)
	{
		QStringList s_list= ui.multi_pkt_txt->toPlainText().split("\n");
		QString error;
		std::string strError;
		for each (QString mss in s_list)
		{
			if(!preparePacket(mss,error))
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
	//********************************************************************************************
	//Spam packets in multiline
	else if (pSender == ui.spam_multi_btn)
	{
		//Not running already
		if (ui.spam_multi_btn->text() == "Spam it!")
		{
			QString error;
			QStringList Packets = this->ui.multi_pkt_txt->toPlainText().split('\n');
			for each (QString s in Packets)
			{
				if(!preparePacket(s,error))
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
	//******************************************************************************************
	
}

void OpenPE::globalSettings(bool state)
{
	QObject* pSender = sender();
	if (pSender == ui.hook_chk)
	{
		setHooks(reinterpret_cast<PVOID*>(&_SendPacket),0x004C8B90,&SendPacket,state);
		setHooks(reinterpret_cast<PVOID*>(&_ProcessPacket),0x004C9310,&ProcessPacket,state);

		//SetDecodeHooks(state);
		ui.show_Recv_chk->setEnabled(state);
		ui.show_Send_chk->setEnabled(state);
		//enable/disable tabs here
	} 
	else if (pSender == ui.show_Send_chk)
		show_send = state;
	else if (pSender == ui.show_Recv_chk)
		show_recv = state;
	else if (pSender == ui.ignore_AsBlack_chk)
		ignore_As_blackList = state;
		
}

int OpenPE::alreayAnode(QTreeWidget* tree, QString header)
{
	for (int i=0;i<tree->topLevelItemCount();i++)
	{
		if (tree->topLevelItem(i)->text(0) == header)
		{
			return i;
		}
	}
	return -1;
}

void OpenPE::AddPacket(CPacket& pkt/*,QStructItem* st_item*/)

{
	//create item
	FoxTreeItem* pItem = new FoxTreeItem();//(st_item);
	FoxTreeItem* foxItem = new FoxTreeItem();//(st_item);
	FoxTreeItem* foxItemChild = new FoxTreeItem();//(st_item);
	QTreeWidget* tree;
	//print Type
	if (pkt.Type == PktType::Out)
	{
		pItem->setText(0, "Out");
		tree = this->ui.send_tree;
	}
	else
	{
		pItem->setText(0, "In");
		tree = this->ui.recv_tree;
	}

	//set color
	pItem->setTextColor(0, this->GetPacketColor(pkt.Type));
	foxItem->setTextColor(0, this->GetPacketColor(pkt.Type));
	foxItemChild->setTextColor(0, this->GetPacketColor(pkt.Type));
	//print Address
	pItem->setText(1, QString().sprintf("%08X", pkt.dwReturn));

	//print Size
	pItem->setText(2, QString().sprintf("%i", pkt.cbData));

	//format Packet Data
	QString qstrPacket;
	for (std::size_t index = 0; index < pkt.cbData; index++)
	{
		if (index != 0)
			qstrPacket += ' ';

		qstrPacket += QString().sprintf("%02X", pkt.bData[index]);
	}

	//print Packet Data
	pItem->setText(3, qstrPacket);

	//set tooltip for Packet Data
	pItem->setToolTip(3, qstrPacket);

	//Foxlogger tree item

	QString allData = pItem->text(3);
	QString currentHeader = allData.left(5);
	QString currentData = allData.right(allData.length()-5);

	int posibleNode = alreayAnode(tree, currentHeader);
	if(posibleNode >= 0)
	{

		foxItem->setText(0,currentData);
		tree->topLevelItem(posibleNode)->addChild(foxItem);
	}
	else
	{

		foxItem->setText(0,currentHeader);
		if(currentData.length()>0)
		{
			foxItemChild->setText(0,currentData);
			foxItem->addChild(foxItemChild);
		}
		tree->addTopLevelItem(foxItem);
	}
	//dispatch to GUI
	this->ui.flow_tree->addTopLevelItem(pItem);
}

void OpenPE::contextMenuSlot_delete()
{
	QObject* pSender = sender();
	QTreeWidget * tree;
	if(ui.flow_tree->hasFocus())
		tree = ui.flow_tree;
	else if (ui.send_tree->hasFocus())
		tree = ui.send_tree;
	else if (ui.recv_tree->hasFocus())
		tree = ui.recv_tree;
	else
		tree= ui.ignored_tree;

	if (pSender == ui.actionClear_log)
	{
		ui.flow_tree->clear();
		ui.send_tree->clear();
		ui.recv_tree->clear();
	}
	if (pSender == ui.actionClear_this_log)
	{
		tree->clear();
	}
	if (pSender == ui.actionDelete_Selected)
	{
		 QTreeWidgetItem* pItem = tree->currentItem();
		 if (tree == ui.ignored_tree)
		 {
			 QColor colors[] =
			 {
				 QColor::fromRgb(127, 170, 230), //blue
				 QColor::fromRgb(255, 120, 40) //orange
			 };
			 bool isSend = pItem->textColor(0) == colors[0];
			 QString sHeader = pItem->text(0).mid(2);
			 WORD wHeader = sHeader.toLong(0,16);
			 if(!black_list->remove(wHeader,isSend))
				 QMessageBox::warning(this,"Error","OMG!! Error deleting ignore opcode!");
		 }
		 else if (pItem != NULL)
		 {
			 delete pItem;
		 }
	}
	
}

void OpenPE::contextMenuSlot_packet()
{
	QObject* pSender = sender();

	QTreeWidget * tree;
	if(ui.flow_tree->hasFocus())
		tree = ui.flow_tree;
	else if (ui.send_tree->hasFocus())
		tree = ui.send_tree;
	else if (ui.recv_tree->hasFocus())
		tree = ui.recv_tree;
	else
		tree= ui.ignored_tree;

	if (pSender ==ui.actionCopy_packet)
	{
		QTreeWidgetItem* pItem = tree->currentItem();
		if (pItem != NULL)
		{
			QString pck;
			if (tree == ui.flow_tree)
			{
				pck = pItem->text(3);
			}
			else
			{
				pck = pItem->text(0);
				pck.append(" ");
				pck.append(pItem->text(1));
			}
			QClipboard* pClipboard = QApplication::clipboard();
			pClipboard->clear();
			pClipboard->setText(pck);
		}
	}
	if ((pSender ==ui.actionIgnore_opcode) || (pSender == ui.actionBlock_opcode))
	{
		QString sHeader;
		WORD wHeader;
		bool isSend;
		edit_type edit = (pSender ==ui.actionIgnore_opcode) ? IGNORED : BLOCK;
		QTreeWidgetItem* pItem = tree->currentItem();
		if (pItem != NULL)
		{
			if (tree == ui.flow_tree)
			{
				sHeader = pItem->text(3).left(5);
				isSend = pItem->text(0) == "Out";
			}
			else
			{
				isSend = tree == ui.send_tree;
				if (!(pItem->childCount()>0))
					return;
				else
				{
					sHeader = pItem->text(0);
				}
			}
			sHeader.remove(" ");
			QString aux1 = sHeader.left(2);
			QString aux2 = sHeader.mid(2);
			sHeader = aux2+ aux1;
			wHeader = sHeader.toLong(0,16);
			if(!black_list->addList(wHeader,edit,isSend,"","",""))
				QMessageBox::warning(this,"Error","This opcode is already in the ignore or edit list.\nPlease delete it first.");
		}
	}
	
	if (pSender ==ui.actionAdd_Ignore_opcode)
	{
		boost::scoped_ptr<QMessageBox> mb(new QMessageBox(this));
		boost::scoped_ptr<QPushButton> pb1 (mb->addButton("Out", QMessageBox::ButtonRole::ActionRole));
		boost::scoped_ptr<QPushButton> pb2 (mb->addButton("In", QMessageBox::ButtonRole::ActionRole));
		mb->setWindowTitle("Packet Type");
		mb->setText("Please select the packet type (In or Out).");
		mb->setIcon(QMessageBox::Icon::Information);
		mb->exec();

		bool isSend = mb->clickedButton() == pb1.get();

		QString& qstr = QInputDialog::getText(this, "Header", "Please enter a header (WORD in hex).\nNote this is reverse from logging!");
		bool fSucceeded = false;
		WORD wHeader = qstr.toUShort(&fSucceeded, 16);

		if (fSucceeded)
		{
			if(!this->black_list->addList(wHeader,IGNORED,isSend,"","",""))
				QMessageBox::warning(this,"Error","This opcode is already in the ignore list.\nPlease delete it first.");
		}
		else
		{
			QMessageBox::critical(this,"U silly?","This isn't a valid hex number!");
		}
	}

}

bool OpenPE::pattern(CPacket& pkt , QString patter)
{
	patter.remove(" ");
	patter.toUpper();
	bool isPatterIncomplete = patter.indexOf("...") >= 0;

	int patterLength = patter.length();	

	int patterI = patter.count('?');


	int patterLengthInBytes = (patterLength+patterI) / 2;

	if(!isPatterIncomplete)  //if from isn't a incomplete pattern (aka check all nº of bytes)
		if(patterLengthInBytes != pkt.cbData) //then if size of packet != size of pattern
			return false;                     //Do nothing.

	int pattOfset = 0;
	int dataIndex = 0;

	while ((pattOfset < patter.length()) && (patter[pattOfset]!='.'))
	{
		if (dataIndex >pkt.cbData) //pattern is longer than packet
		{
			return false;
		}
		if (patter[pattOfset] == '?')
			pattOfset ++;
		else
		{
			QString aux = patter.mid(pattOfset,2);
			QString data = QString().sprintf("%02X", pkt.bData[dataIndex]);
			if(aux == data)
			{
				pattOfset = pattOfset + 2;
			}
			else
				return false;
		}
		dataIndex ++;
	}
return true;
}

void OpenPE::injectModPacket (CPacket& pkt , QString patter, bool isSend)
{
	patter.remove(" ");
	patter.toUpper();

	bool isPatterIncomplete = patter.indexOf("...") >= 0;

	int patterLength = patter.length();	

	int patterI = patter.count('?');

	int patterLengthInBytes = (patterLength+patterI) / 2;

	int pattOfset = 0;
	int dataIndex = 0;

	QString aux ="";

	while ((pattOfset < patter.length()) && (patter[pattOfset]!='.'))
	{
		if(patter[pattOfset] == '?')
		{
			if (dataIndex > pkt.cbData)//internal error, cant copy this byte from original packet
			{
				MessageBox(NULL,L"Internal error - cant copy byte from original packet",L"Error",MB_OK);
				return;
			}
			else
			{
				aux += QString().sprintf("%02X", pkt.bData[dataIndex]);
				pattOfset ++;
			}
			
		}
		else
		{
			aux += patter.mid(pattOfset,2);
			pattOfset = pattOfset + 2;
		}
		dataIndex ++;
	}
	if(isPatterIncomplete)
	{
		while(dataIndex < pkt.cbData)
		{
			aux += QString().sprintf("%02X", pkt.bData[dataIndex]);
			dataIndex ++;
		}
	}
	std::string error;
	QString qError;
	preparePacket(aux,qError);
//	MessageBoxA(NULL,aux.toStdString().c_str(),"Packet",MB_OK);
	if(isSend)
	{
	 if(!CPacket::SendPacket(aux.toStdString(),error))
		 MessageBox(NULL,L"Internal error - cant send this packet",L"Error",MB_OK);
	}
	else
	{
	 if(!CPacket::RecvPacket(aux.toStdString(),error))
		  MessageBox(NULL,L"Internal error - cant recv this packet",L"Error",MB_OK);
	}
	// MessageBox(NULL,L"Injected the packet",L"Done",MB_OK);
}



void OpenPE::scriptSlot()
{
	if (sender()== ui.test_script_btn)
	{
		QString toCheck = ui.sender_code->toPlainText();
		QString result = sender_engine->checkSyntax(toCheck);
		QMessageBox::information(this,"Syntax result",result);
	//	ui.script_output_txt->setText(result);
	}
	else
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
}

void OpenPE::sender_engine_error(QString error)
{
	QIcon icon7;
	icon7.addFile(QString::fromUtf8(":/OpenPE/Resources/script_gear.png"), QSize(), QIcon::Normal, QIcon::Off);
	ui.run_script_btn->setText("Run");
	ui.run_script_btn->setIcon(icon7);
	QMessageBox::critical(this,"Script Error",error);
}

void OpenPE::sender_engine_ended()
{
	QIcon icon7;
	icon7.addFile(QString::fromUtf8(":/OpenPE/Resources/script_gear.png"), QSize(), QIcon::Normal, QIcon::Off);
	ui.run_script_btn->setText("Run");
	ui.run_script_btn->setIcon(icon7);
//	QMessageBox::information(this,"Script End","end");
}

void OpenPE::script_sms_box(QString title,QString body)
{
	QMessageBox::information(this,title,body);
}
void OpenPE::edit_engine_error(QString error)
{
	QMessageBox::critical(this,"Script Error",error);
}
void OpenPE::script_output(QString output)
{
	//ui.script_output_txt->setText(output);
	//QMessageBox::information(this,"Sc",result);
}

void OpenPE::expandSlot()
{
	if(sender() == ui.more_script_btn)
	{
		QScriptWindow * w = new QScriptWindow(this,Qt::WindowMinimizeButtonHint);
		w->show();
	}
	else
	{
		QSpammerWindow * w = new QSpammerWindow(this,Qt::WindowMinimizeButtonHint);
		w->show();
	}
}
void OpenPE::loadScript(QString newScript)
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
	
	
		ui.script_Box->clear();
		ui.script_Box->addItems(list);
	
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
	
void OpenPE::save_senderCode()
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
	int itemcount = ui.script_Box->count();
	if(itemcount != list.length())
	{
		ui.script_Box->clear();
		ui.script_Box->addItems(list);
	}
}

void OpenPE::ProcessFile(QString dir, QString file ,bool isSend)
{
	QString abPath = dir +"/"+ file;
	QString aux = file.left(4);
	bool fsucces;
	WORD header = aux.toUShort(&fsucces,16);
		if(!fsucces)
			return;
	QFile qfile(abPath);
	if (!qfile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream in(&qfile);
	QString from = "";
	QString to="";
	QString script = "";
	QString line = in.readLine();
	if (line.toUpper().startsWith("BLO"))
	{
		black_list->addList(header,BLOCK,isSend,from,to,script);
	}
	else if (line.toUpper().startsWith("PAT"))
	{
	from = in.readLine();
	to = in.readLine();
	black_list->addList(header,MOD,isSend,from,to,script);
	}
	else if (line.toUpper().startsWith("IGN"))
	{
	black_list->addList(header,IGNORED,isSend,from,to,script);
	}
	else
	{
	script = in.readAll();
	black_list->addList(header,SCRIPT,isSend,from,to,script);
	}
}

void OpenPE::itemActivated(QTreeWidgetItem* item,int)
{
	if(item->childCount()!= 0)
		return;
	QTreeWidget * tree;
	if(ui.send_tree->hasFocus())
		tree = ui.send_tree;
	else if(ui.recv_tree->hasFocus())
		tree = ui.recv_tree;
	else
		tree = ui.flow_tree;
		
	FoxTreeItem * fItem = (FoxTreeItem*)item;
	QStructItem * st_item = fItem->st_item;
	QString sStruct;
	QString aux;
	QString data_str;
	for (int i=0; i<st_item->members.size();i++)
	{
		member_type typ = st_item->members.at(i).m_type;
		switch (typ)
		{
		case IsByte:
			aux = QString().sprintf("<font color=#003399>[%02X] </font>",st_item->members.at(i).b);
			sStruct += aux;
		break;
		case IsWord:
			aux = QString().sprintf("<font color=#339966>[%04X] </font>",st_item->members.at(i).w);
			sStruct += aux;
		break;
		case IsDword:
			aux = QString().sprintf("<font color=#996666>[%08X] </font>",st_item->members.at(i).dw);
			sStruct += aux;
		break;
		case IsStr:
			data_str = QString().fromStdString(st_item->members.at(i).str);
			if(data_str.isEmpty())
			{
				data_str = "Empty";
			}
			aux = "<font color=#993399>["+data_str+"] </font>";
			sStruct += aux;
		break;
		case IsBuff:
			aux = "";
			for (int l=0 ; l <st_item->members.at(i).buff.size() ; l++)
			{
				aux+= QString().sprintf("%02X ",st_item->members.at(i).buff.at(l));
			}
			aux.prepend("<font color=#ccff99>[");
			aux.append("]</font>");
			sStruct += aux;
		break;
		}
	}
	ui.fomatted_text->clear();
	ui.data_txt->clear();
	ui.fomatted_text->insertHtml(sStruct);
	if (tree == ui.flow_tree)
	{
    ui.data_txt->insertPlainText(item->text(3));
	}
	else
	{
	QString opc = item->parent()->text(0);
	QString dat = item->text(0);
	ui.data_txt->insertPlainText(opc+" "+dat);
	}

}

void OpenPE::pktInfoSlot()
{
	QTreeWidget * tree;
	if(ui.send_tree->hasFocus())
		tree = ui.send_tree;
	else if(ui.recv_tree->hasFocus())
		tree = ui.recv_tree;
	else
		tree = ui.flow_tree;

	QTreeWidgetItem* pitem = tree->currentItem();
	if (pitem == NULL)
		return;
	if(pitem->childCount() !=0)
		return;
	FoxTreeItem * fitem = (FoxTreeItem*)pitem;
	QString data;
	if (tree == ui.flow_tree)
	{
		data = pitem->text(3);
	} 
	else
	{
		QString opc = pitem->parent()->text(0);
		QString dat = pitem->text(0);
		data =(opc+" "+dat);
	}
	PacketInfoWnd * w = new PacketInfoWnd(this,fitem->st_item,data);
	w->show();
}