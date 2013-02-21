#include "stringconverter.h"

StringConverter::StringConverter(QObject *parent)
	: QObject(parent)
{

}
StringConverter::StringConverter()
{

}
StringConverter::~StringConverter()
{

}
QString StringConverter::ToHexString(int input, QString type)
{
	if (type.toLower() == "byte")
	{
		return QString().sprintf("%02X",input);
	}
	else if (type.toLower() =="word")
	{
		return QString().sprintf("%04X",input);
	}
	else if (type.toLower() =="dword")
	{
		return QString().sprintf("%08X",input);
	}
	else
		return "Error";
}

QString toAcsiiString(QString in)
{
	bool ok;
	QString out;
	QString aux = in.replace(" ","",Qt::CaseSensitive);
	for(int i=0;i<aux.length();i+=2)
	{
		QString c = aux.mid(i,2);	
		int a = c.toInt(&ok, 16);
		QChar d;
		if(a>32 && a<136)
		{
			out += d.fromAscii(a);
		}
		else
		{
			out +=".";
		}
	}
	return out;
}

QString StringConverter::ToMapleString(QString input)
{
	input.remove(" ");
	QString length_byte = QString().sprintf("%02X",input.length());
	length_byte.append(" 00");
	for (int i=0 ; i<input.length() ; i++)
	{
		QChar aux = input[i];
		QString aux1 = QString().sprintf("%02X",aux.toAscii());
		length_byte.append(" "+aux1);
	}
	return length_byte;
}
QString StringConverter::ToLittleEndian(QString input)
{
	input.remove(" ");
	QString out;
	for (int i=0;i<input.length();i=i+2)
	{
		out.prepend(input.mid(i,2)+" ");
	}
	return out;
}
bool StringConverter::isHex(QString input)
{
	return true;
}
