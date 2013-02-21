#include "packetinfownd.h"

PacketInfoWnd::PacketInfoWnd(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

PacketInfoWnd::~PacketInfoWnd()
{

}
PacketInfoWnd::PacketInfoWnd(QWidget *parent ,QStructItem* st_item,QString data)
	:QDialog(parent)
{
	ui.setupUi(this);
	ui.line->setText(data);
	QString aux_data = data.remove(" ");
	QString sStruct;
	for(int i = 0;i<st_item->members.size();i++)
	{
		QTreeWidgetItem * type = new QTreeWidgetItem;
		QTreeWidgetItem * value = new QTreeWidgetItem;
		QTreeWidgetItem * ret = new QTreeWidgetItem;
		QString aux;
		QString dat;
		QString retAddy;

		member_type typ = st_item->members.at(i).m_type;
		switch (typ)
		{
		case IsByte:
			sStruct += "[Byte]";
			type->setText(0,"[Byte]");
			aux = QString().sprintf("0x%02X = %d",st_item->members.at(i).b,st_item->members.at(i).b);
			value->setText(0,aux);

			retAddy = QString().sprintf("Return Address : 0x%08X",st_item->members.at(i).ret);
			ret->setText(0,retAddy);			

			type->addChild(value);
			type->addChild(ret);

			ui.tree->addTopLevelItem(type);
			break;
		case IsWord:
			sStruct += "[Word]";
			type->setText(0,"[Word]");

			aux = QString().sprintf("%04X = %d",st_item->members.at(i).w,st_item->members.at(i).w);
			value->setText(0,aux);

			retAddy = QString().sprintf("Return Address : 0x%08X",st_item->members.at(i).ret);
			ret->setText(0,retAddy);			
			
			type->addChild(value);
			type->addChild(ret);

			ui.tree->addTopLevelItem(type);
			break;
		case IsDword:
			sStruct += "[DWord]";
			type->setText(0,"[DWord]");

			aux = QString().sprintf("%08X = %d",st_item->members.at(i).dw,st_item->members.at(i).dw);
			value->setText(0,aux);
			

			retAddy = QString().sprintf("Return Address : 0x%08X",st_item->members.at(i).ret);
			ret->setText(0,retAddy);			
			
			
			type->addChild(value);
			type->addChild(ret);

			ui.tree->addTopLevelItem(type);
			break;
		case IsStr:
			sStruct += "[MapleString]";
			type->setText(0,"[MapleString]");

			aux = QString().fromStdString(st_item->members.at(i).str);
			if (aux.isEmpty())
			{
				aux="<Empty>";
			}
			value->setText(0,aux);

			retAddy = QString().sprintf("Return Address : 0x%08X",st_item->members.at(i).ret);
			ret->setText(0,retAddy);			
			
			type->addChild(value);
			type->addChild(ret);

			ui.tree->addTopLevelItem(type);
			break;
		case IsBuff:
			sStruct += "[Buffer]";
			type->setText(0,"[Buffer]");

			retAddy = QString().sprintf("Return Address : 0x%08X",st_item->members.at(i).ret);
			ret->setText(0,retAddy);

			for (int l=0 ; l <st_item->members.at(i).buff.size() ; l++)
			{
				aux+= QString().sprintf("%02X ",st_item->members.at(i).buff.at(l));
			}
			value->setText(0,aux);			
			
			type->addChild(value);
			type->addChild(ret);

			ui.tree->addTopLevelItem(type);
			break;
		}
	}
	ui.line_stc->setText(sStruct);
}