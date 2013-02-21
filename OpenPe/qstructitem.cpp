#include "qstructitem.h"

QStructItem::QStructItem(QObject *parent)
	: QObject(parent)
{

}

QStructItem::~QStructItem()
{

}
void QStructItem::add1(BYTE value,DWORD retAddy)
{
	member_struct mst;
	mst.m_type = IsByte;
	mst.b = value;
	mst.ret = retAddy;
	members.append(mst);
}
void QStructItem::add2(WORD value,DWORD retAddy)
{
	member_struct mst;
	mst.m_type = IsWord;
	mst.w = value;
	mst.ret = retAddy;
	members.append(mst);
}
void QStructItem::add4(DWORD value,DWORD retAddy)
{
	member_struct mst;
	mst.m_type = IsDword;
	mst.dw = value;
	mst.ret = retAddy;
	members.append(mst);
}
void QStructItem::SaddStr(LPCSTR value,DWORD retAddy)
{
	member_struct mst;
	mst.m_type = IsStr;
	mst.str = value;
	mst.ret = retAddy;
	members.append(mst);
}
void QStructItem::addStr(LPCSTR* value,DWORD retAddy)
{
	member_struct mst;
	mst.m_type = IsStr;
	mst.str = *value;
	mst.ret = retAddy;
	members.append(mst);
}
void QStructItem::addBuff(__in_bcount(uLength) PBYTE pb, __in UINT uLength,DWORD retAddy)
{
	member_struct mst;
	mst.m_type = IsBuff;
	for (int i=0 ; i<uLength ; i++)
	{
		mst.buff.append(pb[i]);
	}
	mst.ret = retAddy;
	members.append(mst);
}