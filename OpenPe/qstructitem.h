#ifndef QSTRUCTITEM_H
#define QSTRUCTITEM_H

#include <QObject>
#include <QVector>
#include <Windows.h>
enum member_type
{
	IsByte , IsWord , IsDword , IsStr , IsBuff
};
struct member_struct
{
	member_type m_type;
	BYTE b;
	WORD w;
	DWORD dw;
	std::string str;
	QVector<BYTE>buff;	

	DWORD ret;
};
class QStructItem : public QObject
{
	Q_OBJECT

public:
	QStructItem(QObject *parent);
	~QStructItem();
	void add1(BYTE value,DWORD retAddy);
	void add2(WORD value,DWORD retAddy);
	void add4(DWORD value,DWORD retAddy);
	void SaddStr(LPCSTR value,DWORD retAddy);
	void addStr(LPCSTR* value,DWORD retAddy);
	void addBuff(__in_bcount(uLength) PBYTE pb, __in UINT uLength,DWORD retAddy);

	QVector<member_struct> members;
private:
	
};

#endif // QSTRUCTITEM_H
