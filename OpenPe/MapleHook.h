#ifndef MAPLEHOOK_H
#define MAPLEHOOK_H
#include <Windows.h>

#pragma pack(push, 1)
struct COutPacket
{
	BOOL fLoopback;
	union
	{
		LPBYTE lpbData;
		LPVOID lpvData;
		LPWORD lpwHeader;
	};
	DWORD dwcbData;
	UINT uOffset;
	BOOL fEncryptedByShanda;
};

struct CInPacket
{
	BOOL fLoopback; //0
	INT iState; //2
	union
	{
		LPVOID lpvData;
		struct 
		{
			DWORD dw;
			WORD wHeader;
		} *pHeader;
		struct 
		{
			DWORD dw;
			BYTE bData[0];
		} *pData;
	};
	USHORT usLength; //size of preceding struct
	USHORT usRawSeq; //pData->dw & 0xffff
	//should be DWORD, then SIZE_T, according to jony
	USHORT usDataLen; //usLength - 4
	USHORT usUnknown; //0xcc
	UINT uOffset; //sizeof(DWORD) == 4
	LPVOID lpv; //idk; 1238e0?
};


#pragma pack(pop)

enum PktType
{
	Out,
	In
};

//MapleStory Socket
extern const LPVOID* lppvCClientSocket;

//Receive packets hook
typedef void (__fastcall* lpfnRecvPacket)(LPVOID lpvClass, LPVOID lpvEDX, CInPacket* pPacket);
extern lpfnRecvPacket _ProcessPacket;
VOID __fastcall ProcessPacket(__in LPVOID thisSocket, __in DWORD dwEDX, __in CInPacket* pPacket);

//Send packet hook
typedef void (__fastcall* lpfnSendPacket)(LPVOID lpvClass, LPVOID lpvEDX, COutPacket* pPacket);
extern lpfnSendPacket _SendPacket;
VOID __fastcall SendPacket(__in LPVOID thisSocket, __in DWORD dwEDX, __in COutPacket* pPacket);

//Taken from 21st century PE, thanks to Phi!
void InjectPacket(COutPacket* pPacket);
void InjectPacket(CInPacket* pPacket);

bool setHooks(PVOID pTarget, DWORD dwLocation , PVOID pHook, bool enable);
BOOL SetDecodeHooks(bool enable);
#endif




