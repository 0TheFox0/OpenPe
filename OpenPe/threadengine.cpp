#include "threadengine.h"
#include <QMessageBox>
#include <boost/thread.hpp>
ThreadEngine::ThreadEngine(QObject *parent)
	: QThread(parent)
{
	
	engine = new FoxEngine(this);
	bool connScript = true;
	connScript &= connect(engine,SIGNAL(error(QString)),this,SLOT(error(QString)));
	connScript &= connect(engine,SIGNAL(ended()),this,SLOT(ended()));
	connScript &= connect(engine,SIGNAL(smsBox(QString,QString)),this,SLOT(sms(QString,QString)));
	connScript &= connect(engine,SIGNAL(output(QString)),this,SLOT(output(QString)));	
	if(!connScript)
		QMessageBox::warning(NULL,"Internal Error","Can't connect with ScriptEngine",QMessageBox::Ok);	
}

ThreadEngine::~ThreadEngine()
{
	if(engine->isEvaluating())
		engine->abortEvaluation();		
}

void ThreadEngine::error(QString error)
{
	if (engine->isEvaluating())
		engine->abortEvaluation();
	emit errorOccurred(error);
}
void ThreadEngine::ended()
{
	emit endOccurred();
}

void ThreadEngine::run()
{
	engine->evaluate(script);
	if (engine->hasUncaughtException()) 
	{
		QStringList bt = engine->uncaughtExceptionBacktrace();
		emit error(bt.join("\n"));
	}
	else
		emit endOccurred();
}

void ThreadEngine::DoIt(WORD& header)
{
	bool needdoLater = false;
	if(isRunnable)
		this->start();
	else
	{
		engine->set_header(header);
		engine->evaluate(script);
		engine->needDolater = false;
		if (engine->hasUncaughtException()) 
		{
			QStringList bt = engine->uncaughtExceptionBacktrace();
			emit error(bt.join("\n"));
		}
		else
		{
			header = engine->out_Header;
			needdoLater = engine->needDolater;
			emit endOccurred();
		}
		if(needdoLater)
		{
			boost::thread * t = new boost::thread(&FoxEngine::doLater,engine);
		}
	}
}


void ThreadEngine::StopIt()
{
	if (engine->isEvaluating())
		engine->abortEvaluation();
}

bool ThreadEngine::setScript(QString newScript)
{
	FoxEngine * eng = new FoxEngine(this);
	if(newScript.isEmpty())
	{	
		emit errorOccurred("What script do u wanna check?  m(");
		return false;
	}
	QScriptSyntaxCheckResult isValid = eng->checkSyntax(newScript);
	bool script_valid;
	switch (isValid.state())
	{
	case QScriptSyntaxCheckResult::Intermediate:
		emit errorOccurred("Script not complete");
		script_valid = false;
		break;
	case QScriptSyntaxCheckResult::Error:
		emit errorOccurred("Script not valid");
		script_valid = false;
		break;
	default:
		script_valid = true;
	}
	//check for isRunnable
	bool b = newScript.contains("ThisPacket");
	isRunnable = !b;
	if(script_valid)
		script = newScript;
	return script_valid;
}

QString ThreadEngine::checkSyntax(QString toCheck)
{
	FoxEngine * eng = new FoxEngine(this);
	QString result;
	if (toCheck.isEmpty())
	{
		result = "What script do u wanna check?  m(";
		return result;
	}
	QScriptSyntaxCheckResult isValid = eng->checkSyntax(toCheck);
	result = isValid.errorMessage();
	if(result.isEmpty())
		result = "Script syntax valid.";
	return result;
}

void ThreadEngine::sms(QString title,QString body)
{
	emit smsBox(title,body);
}

void ThreadEngine::output(QString out)
{
	emit outputOcurred(out);
}

void ThreadEngine::set_Packet(CPacket pkt,bool isSend)
{
	engine->set_Packet(pkt,isSend);
}