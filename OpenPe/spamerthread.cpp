#include "spamerthread.h"
#include "CPacket.h"
SpamerThread::SpamerThread(QObject *parent)
	: QThread(parent)
{
	listItems.clear();	
	SpamDelay =0;
	continueSpaming = false;
}

SpamerThread::~SpamerThread()
{

}

void SpamerThread::run()
{
	std::string strError;
	do
	{
		for (int i=0;i<listItems.length();i++)
		{				
			if(!CPacket::SendPacket(listItems[i].toStdString(), strError))
			{
				continueSpaming = false;
			}			
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(SpamDelay));
	}while(continueSpaming);
}