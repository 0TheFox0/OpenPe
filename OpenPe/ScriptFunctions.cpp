#include "ScriptFunctions.h"
#include "foxengine.h"
#include "openpe.h"
#include "CPacket.h"
#include <boost/thread.hpp>
QScriptValue ShowMessage(QScriptContext *context, QScriptEngine *engine)
{
	FoxEngine * eng = (FoxEngine *)engine;
	int args = context->argumentCount();
	QString title,body;
	switch (args)
	{
	case 2:
		title = context->argument(0).toString();
		body = context->argument(1).toString();
		eng->message(title,body);
		#if _DEBUG
		eng->emitOutput("MessageBox Showed");
		#endif
		break;	
	default:
		eng->emitError("ShowMessage Function Error - Invalid Argument Number");		
		return 0;
	}
	return 1;
}

QScriptValue ScriptPktSend(QScriptContext *context, QScriptEngine *engine)
{
	FoxEngine * eng = (FoxEngine *)engine;
	int args = context->argumentCount();
	QString pkt,ppacket, Qerror;
	int delay , loops;
	std::string error;
	switch (args)
	{
	case 1:
		pkt = context->argument(0).toString();
		thisPe->preparePacket(pkt,Qerror) ;
		if (!CPacket::SendPacket(pkt.toStdString(),error))
		{
			eng->emitError(QString().fromStdString(error));
		}
		break;
	case 2:
		pkt = context->argument(0).toString();
		thisPe->preparePacket(pkt,Qerror) ;
		delay = context->argument(1).toInt32();
		thisPe->preparePacket(pkt,Qerror);
		if (!CPacket::SendPacket(pkt.toStdString(),error))
		{
			eng->emitError(QString().fromStdString(error));
		}

		boost::this_thread::sleep(boost::posix_time::milliseconds(delay)); 	
		break;
	case 3:
		pkt = context->argument(0).toString();
		thisPe->preparePacket(pkt,Qerror) ;
		delay = context->argument(1).toInt32();
		loops = context->argument(2).toInt32();
		for (int i=0;i<loops;i++)
		{
			thisPe->preparePacket(pkt,Qerror);
			if (!CPacket::SendPacket(pkt.toStdString(),error))
			{
				eng->emitError(QString().fromStdString(error));
			}
			boost::this_thread::sleep(boost::posix_time::milliseconds(delay)); 	
		}
		break;
	}
	return 1;
}
QScriptValue ScriptPktRecv(QScriptContext *context, QScriptEngine *engine)
{
	FoxEngine * eng = (FoxEngine *)engine;
	int args = context->argumentCount();
	QString pkt, Qerror, ppacket;
	int delay , loops;
	std::string error;
	switch (args)
	{
	case 1:
		pkt = context->argument(0).toString();
		thisPe->preparePacket(pkt,Qerror) ;
		if (!CPacket::RecvPacket(pkt.toStdString(),error))
		{
			eng->emitError(QString().fromStdString(error));
		}
		break;
	case 2:
		pkt = context->argument(0).toString();
		thisPe->preparePacket(pkt,Qerror) ;
		delay = context->argument(1).toInt32();
		thisPe->preparePacket(pkt,Qerror);
		if (!CPacket::RecvPacket(pkt.toStdString(),error))
		{
			eng->emitError(QString().fromStdString(error));
		}

		boost::this_thread::sleep(boost::posix_time::milliseconds(delay)); 	
		break;
	case 3:
		pkt = context->argument(0).toString();
		thisPe->preparePacket(pkt,Qerror) ;
		delay = context->argument(1).toInt32();
		loops = context->argument(2).toInt32();
		for (int i=0;i<loops;i++)
		{
			thisPe->preparePacket(pkt,Qerror);
			if (!CPacket::RecvPacket(pkt.toStdString(),error))
			{
				eng->emitError(QString().fromStdString(error));
			}
			boost::this_thread::sleep(boost::posix_time::milliseconds(delay)); 	
		}
		break;
	}
	return 1;
}

QScriptValue ScriptSleep(QScriptContext *context, QScriptEngine *engine)
{
	FoxEngine * eng = (FoxEngine *)engine;
	QScriptValue a;
	int args = context->argumentCount();
	if(args == 1)
	{
		a = context->argument(0);
		boost::this_thread::sleep(boost::posix_time::milliseconds(a.toInt32())); 	
	}
	else
	{
	eng->emitError("Sleep Function Error - Invalid Argument Number");	
	}
	return a;
}


void MakePageWritable(void* lpvAddress, const int cbSize)
{
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery(lpvAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
	if (mbi.Protect != PAGE_EXECUTE_READWRITE)
	{
		unsigned long ulProtect;
		VirtualProtect(lpvAddress, cbSize, PAGE_EXECUTE_READWRITE, &ulProtect);
	}
}
unsigned long ReadPointer(unsigned long ulBase, int iOffset)
{
	__try { return *(unsigned long*)(*(unsigned long*)ulBase + iOffset); }
	__except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

void WriteMemory(unsigned long dwAddress,unsigned char* bBYTES,int sz_Write_Lenght)
{
	__try
	{
		MakePageWritable((LPVOID)dwAddress,sz_Write_Lenght);
		for (size_t index = 0; index < sz_Write_Lenght; index++)
		{
			*(BYTE*)(dwAddress + index) = bBYTES[index];
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER){ }
}

bool WritePointer(unsigned long ulBase, int iOffset, int iValue)
{
	__try { *(int*)(*(unsigned long*)ulBase + iOffset) = iValue; return true; }
	__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

QScriptValue ScriptReadPointer(QScriptContext *context, QScriptEngine *engine)
{
	FoxEngine * eng = (FoxEngine *)engine;
	QScriptValue a,b,result;
	int args = context->argumentCount();
	bool fSucceeded;
	if(args == 2)
	{
		a = context->argument(0);
		b = context->argument(1);
		QString sbase = a.toString();
		sbase.toUpper();
		sbase.replace("0X","");
		unsigned long base = sbase.toLong(&fSucceeded,16);
		QString soff = b.toString();
		soff.toUpper();
		soff.replace("0X","");
		int off = soff.toInt(&fSucceeded,16);
		if(fSucceeded)
		{
			result= (int)ReadPointer(base, off);
		}
		else
		{
			eng->emitError("ReadPointer Function Error - Invalid Argument");	
		}
	}
	else
	{
		eng->emitError("ReadPointer Function Error - Invalid Argument Number");	
	}
	return result;
}
QScriptValue ScriptWritePointer(QScriptContext *context, QScriptEngine *engine)
{
	FoxEngine * eng = (FoxEngine *)engine;
	QScriptValue a,b,c,result;
	int args = context->argumentCount();
	bool fSucceeded;
	if(args == 3)
	{
		a = context->argument(0);
		b = context->argument(1);
		c = context->argument(2);
		QString sbase = a.toString();
		sbase.toUpper();
		sbase.replace("0X","");
		unsigned long base = sbase.toLong(&fSucceeded,16);
		QString soff = b.toString();
		soff.toUpper();
		soff.replace("0X","");
		int off = soff.toInt(&fSucceeded,16);
		if(fSucceeded)
		{
			WritePointer(base,off,c.toInt32());
		}
		else
		{
			eng->emitError("WritePointer Function Error - Invalid Argument");	
		}
	}
	else
	{
		eng->emitError("WritePointer Function Error - Invalid Argument Number");	
	}
	return result;
}

QScriptValue ScriptFormat(QScriptContext *context, QScriptEngine *engine)
{
	FoxEngine * eng = (FoxEngine *)engine;
	QScriptValue a,b,c,result;
	QString in,out;
	int args = context->argumentCount();
	bool fSucceeded;
	if(args < 2)
	{
		eng->emitError("Format Function Error - Invalid Argument Number");	
	}
	else
	{
		a = context->argument(0);
		QString formato = a.toString();
		QString aux;
		int j = 0;
		int valid_args = formato.count("%");
		while ((j = formato.indexOf("%", j)) != -1) 
		{
			formato.insert(j," ");
			j=j+2;
		}
		int last_split = 0;
		QStringList list;
		bool needSplit=false;
		for(int c=0; c<formato.length() ; c++)
		{
		//%X %X
			if(formato[c]=='%')
			{
				needSplit = true;
			}
			if (needSplit)
			{
				if (formato[c]==' ')
				{
					needSplit = false;
					list.append(formato.mid(last_split,c-last_split));
					last_split = c;
				}
			}
		}
		list.append(formato.mid(last_split));
		int index = 0;
		for(int i = 0; ((i+1 < args)||(i<valid_args)) ; i++)
		{

		if (context->argument(i+1).isNumber())
		{
			list[i] = QString().sprintf(list[i].toStdString().c_str(),context->argument(i+1).toInt32());
		}		
		else if (context->argument(i+1).isString())
		{		
			list[i] = QString().sprintf(list[i].toStdString().c_str(),context->argument(i+1).toString().toStdString().c_str());
		}
		
		}
		for(int z=0;z<list.length();z++)
		{
			if (list[z][0] == ' ')
			{
				list[z].remove(0,1);
			}
		}
	QString list_joined = list.join("");
	out = list_joined;
	}
	
	return out;
}
QScriptValue ScriptDoLater(QScriptContext *context, QScriptEngine *engine)
{
	FoxEngine * eng = (FoxEngine *)engine;
	QScriptValue a;
	QString s;
	a = context->argument(0);
	s = a.toString();
	eng->later_Script = s;
	eng->needDolater=true;
	return a;
}