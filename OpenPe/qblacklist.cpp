#include "qblacklist.h"
#include "openpe.h"
#include <QMessageBox>
QBlackList::QBlackList(QObject *parent)
	: QObject(parent)
{

}

QBlackList::~QBlackList()
{

}

bool QBlackList::addList(WORD header, edit_type type, bool isSend, QString from, QString to, QString script)
{
	blackType isEdited;
	int index;
	isEdited = find(header,isSend,index);
	if(isEdited.edit == NONE)
	{
		blackType b;
		b.header = header;
		b.edit = type;
		b.from = from;
		b.to = to;
		b.script = script;
		if(type == SCRIPT)
		{
			ThreadEngine * eng = new ThreadEngine(this);
			if(!eng->setScript(script))
			{
			   QMessageBox::critical(thisPe,"Error","Can't add this script, something's wrong!");
			   return true;
			}
			bool connScript = true;
			connScript &= connect(eng,SIGNAL(errorOccurred(QString)),thisPe,SLOT(edit_engine_error(QString)));
		//	connScript &= connect(eng,SIGNAL(endOccurred()),thisPe,SLOT(sender_engine_ended()));
			connScript &= connect(eng,SIGNAL(smsBox(QString,QString)),thisPe,SLOT(script_sms_box(QString,QString)));
			connScript &= connect(eng,SIGNAL(outputOcurred(QString)),thisPe,SLOT(script_output(QString)));
			if(!connScript)
				QMessageBox::warning(thisPe,"Internal Error","Can't connect with ScriptEngine",QMessageBox::Ok);
			QPair<WORD,ThreadEngine*>* pair = new QPair<WORD,ThreadEngine*>(header,eng);
			isSend ? send_engines.append(pair) : recv_engines.append(pair);
		}
		isSend ? v_send_list.append(b) : v_recv_list.append(b);		
		emit added(header,type,isSend);
		return true;
	}
	return false;
}

blackType QBlackList::find(WORD header, bool isSend , int& index)
{
	QMutexLocker locker(&mutex);
	blackType ret;
	ret.edit= NONE;
	index = -1;
	if (isSend)
	{
		for (int i= 0; i< v_send_list.size(); i++)
		{
			if (v_send_list.at(i).header == header)
			{
				index = i;
				ret = v_send_list.at(i);
				break;		
			}
		}
	}
	else
	{
		for (int i= 0; i< v_recv_list.size(); i++)
		{
			if (v_recv_list.at(i).header == header)
			{
				index = i;
				ret = v_recv_list.at(i);
				break;		
			}
		}
	}
	return ret;
}

bool QBlackList::remove(WORD header, bool isSend)
{
	int index;
	find(header,isSend,index);
	if (index>=0)
	{
		QMutexLocker locker(&mutex);
		edit_type a = isSend ? v_send_list.at(index).edit : v_recv_list.at(index).edit;
		if (a == SCRIPT)
		{
			int idx = findScriptEngine(header,isSend);
			isSend ? send_engines.remove(idx) : recv_engines.remove(idx);
		}
		emit removed(header,a,isSend);
		isSend ? v_send_list.remove(index) : v_recv_list.remove(index);
		return true;
	}
	return false;
}

void QBlackList::removeIgnored()
{
	QMutexLocker locker(&mutex);
	for (int i= 0; i< v_send_list.size(); i++)
	{
		if (v_send_list.at(i).edit == IGNORED)
		{
			emit removed(v_send_list.at(i).header,IGNORED, true);
			v_send_list.remove(i);			
		}
	}
	for (int i= 0; i< v_recv_list.size(); i++)
	{
		if (v_recv_list.at(i).edit == IGNORED)
		{
			emit removed(v_recv_list.at(i).header,IGNORED, false);
			v_recv_list.remove(i);			
		}
	}
	emit ignored_removed();
}

void QBlackList::UnBlockAll()
{
	QMutexLocker locker(&mutex);
	for (int i= 0; i< v_send_list.size(); i++)
	{
		if (v_send_list.at(i).edit == BLOCK)
		{
			emit removed(v_send_list.at(i).header,BLOCK, true);
			v_send_list.remove(i);			
		}
	}
	for (int i= 0; i< v_recv_list.size(); i++)
	{
		if (v_recv_list.at(i).edit == BLOCK)
		{
			emit removed(v_recv_list.at(i).header,BLOCK, false);
			v_recv_list.remove(i);			
		}
	}
	emit blocked_removed();
}

void QBlackList::Block(QString direction,unsigned short header)
{
	bool isSend = (direction == "Out");
	addList(header,BLOCK,isSend,"","","");
}

void QBlackList::UnBlock(QString direction,unsigned short header)
{
	bool isSend = (direction == "Out");
	remove(header,isSend);
}

void QBlackList::clear()
{
	QMutexLocker locker(&mutex);
	v_send_list.clear();
	v_recv_list.clear();
	emit cleared();
}

int QBlackList::findScriptEngine(WORD header,bool isSend)
{
	if(isSend)
	{
		for (int i=0;i<send_engines.size();i++)
		{
			if(send_engines.at(i)->first == header)
				return i;
		}
	}
	else
	{
		for (int i=0;i<recv_engines.size();i++)
		{
			if(recv_engines.at(i)->first == header)
				return  i;
		}
	}
	return -1;
}

void QBlackList::runScript(WORD& header,CPacket pkt,bool isSend)
{
	int i = findScriptEngine(header,isSend);
	if (isSend)
	{
		send_engines.at(i)->second->set_Packet(pkt,isSend);
		send_engines.at(i)->second->DoIt(header);
	}
	else
	{
		recv_engines.at(i)->second->set_Packet(pkt,isSend);
		recv_engines.at(i)->second->DoIt(header);
	}	
}