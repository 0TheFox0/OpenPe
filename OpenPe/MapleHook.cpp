//#include "MapleHook.h"
#include <Windows.h>
#include "detours.h"
#include "openpe.h"
#pragma comment(lib, "detours")


const LPVOID* lppvCClientSocket = reinterpret_cast<const LPVOID*>(0x011C8A58);//(0x00EC3E88);//(0x00F15FF8);

//********************************************************************************************
//**************************************  Send hook  ******************************************
//*********************************************************************************************
lpfnSendPacket _SendPacket = reinterpret_cast<lpfnSendPacket>(0x004C8B90);//(0x004AE940);//(0x004ABE10);
const LPVOID lpvSomeRet = reinterpret_cast<const LPVOID>(0x00764fd7);//(0x004AE936);//(0x004ABE07);

//Decode & Encode Functions
/////
//SEND FUNCTIONS/////
VOID (__fastcall * _InitPacket)(__out COutPacket* oPacket, __in DWORD dwEDX, __in LONG nType) = NULL;
VOID (__fastcall * _Encode1)(__inout COutPacket* oPacket, __in DWORD dwEDX, __in BYTE b) = NULL;
VOID (__fastcall * _Encode2)(__inout COutPacket* oPacket, __in DWORD dwEDX, __in WORD w) = NULL;
VOID (__fastcall * _Encode4)(__inout COutPacket* oPacket, __in DWORD dwEDX, __in DWORD dw) = NULL;
VOID (__fastcall * _EncodeString)(__inout COutPacket* oPacket, __in DWORD dwEDX, __in LPCSTR lpcsz) = NULL;
VOID (__fastcall * _EncodeBuffer)(__inout COutPacket* oPacket, __in DWORD dwEDX, __in_bcount(uLength) LPBYTE pb, __in UINT uLength) = NULL;

WORD trace_opcode = 0x0;
bool bTrace = false;
bool bTrace_in = true;
VOID __fastcall InitPacket(__inout COutPacket* oPacket, __in DWORD dwEDX, __in LONG nType)
{
	if (nType != INT_MAX && thisPe->show_send)
	{
		WORD wHeader = *oPacket->lpwHeader;
		int i;
		blackType type = thisPe->black_list->find(wHeader, true , i);
		if ((type.edit == NONE)||(type.edit == SCRIPT))
		{
			QStructItem* st_item = new QStructItem(NULL);
			thisPe->struct_Logger->SaddItem(oPacket,st_item);
		}
	}
	_InitPacket(oPacket, dwEDX, nType);
}

VOID __fastcall Encode1(__inout COutPacket* oPacket, __in DWORD dwEDX, __in BYTE b)
{
	if(thisPe->show_send)
	{
		WORD wHeader = *oPacket->lpwHeader;
		int i;
		blackType type = thisPe->black_list->find(wHeader, true , i);
		if ((type.edit == NONE)||(type.edit == SCRIPT))
		{
			thisPe->struct_Logger->Sadd1(oPacket,b,reinterpret_cast<DWORD>(_ReturnAddress()));
		}
	}
	_Encode1(oPacket, dwEDX, b);
}

VOID __fastcall Encode2(__inout COutPacket* oPacket, __in DWORD dwEDX, __in WORD w)
{
	if(thisPe->show_send)
	{
		WORD wHeader = *oPacket->lpwHeader;
		int i;
		blackType type = thisPe->black_list->find(wHeader, true , i);
		if ((type.edit == NONE)||(type.edit == SCRIPT))
		{
			thisPe->struct_Logger->Sadd2(oPacket,w,reinterpret_cast<DWORD>(_ReturnAddress()));
		}
	}
	_Encode2(oPacket, dwEDX, w);
}

VOID __fastcall Encode4(__inout COutPacket* oPacket, __in DWORD dwEDX, __in DWORD dw)
{
	if(thisPe->show_send)
	{
		WORD wHeader = *oPacket->lpwHeader;
		int i;
		blackType type = thisPe->black_list->find(wHeader, true , i);
		if ((type.edit == NONE)||(type.edit == SCRIPT))
		{
			thisPe->struct_Logger->Sadd4(oPacket,dw,reinterpret_cast<DWORD>(_ReturnAddress()));
		}
	}
	_Encode4(oPacket, dwEDX, dw);
}

VOID __fastcall EncodeString(__inout COutPacket* oPacket, __in DWORD dwEDX, __in LPCSTR lpcsz)
{
	if(thisPe->show_send)
	{
		WORD wHeader = *oPacket->lpwHeader;
		int i;
		blackType type = thisPe->black_list->find(wHeader, true , i);
		if ((type.edit == NONE)||(type.edit == SCRIPT))
		{
			thisPe->struct_Logger->SaddStr(oPacket,lpcsz,reinterpret_cast<DWORD>(_ReturnAddress()));
		}
	}
	_EncodeString(oPacket, dwEDX, lpcsz);
}

VOID __fastcall EncodeBuffer(__inout COutPacket* oPacket, __in DWORD dwEDX, __in_bcount(uLength) PBYTE pb, __in UINT uLength)
{
	if(thisPe->show_send)
	{
		WORD wHeader = *oPacket->lpwHeader;
		int i;
		blackType type = thisPe->black_list->find(wHeader, true , i);
		if ((type.edit == NONE)||(type.edit == SCRIPT))
		{
			thisPe->struct_Logger->SaddBuff(oPacket,pb,uLength,reinterpret_cast<DWORD>(_ReturnAddress()));
		}
	}
	_EncodeBuffer(oPacket, dwEDX, pb, uLength);
}

void __stdcall HandleOutPacket(COutPacket* pPacket, DWORD dwReturn)
{
	if (pPacket != NULL)
	{
	//	if (pPacket->dwcbData >= 2)
	//	{
			bool display = true;
			 WORD& wHeader = *pPacket->lpwHeader;
			int i;
			blackType type = thisPe->black_list->find(wHeader, true , i);
			//black - white list switch
			if (thisPe->ignore_As_blackList)
			{
				if (type.edit == IGNORED)
					display = false;
			}
			else
			{
				if (type.edit == NONE)
					display=false;
			}

			if (type.edit == BLOCK)
			{
				display = false;
				wHeader = -1;
			}
			else if (type.edit == MOD)
			{				
				CPacket Pkt(pPacket, dwReturn);
				if(thisPe->pattern(Pkt,type.from))
				{
				//	MessageBox(NULL,L"Pattern accepted",L"Pattern",MB_OK);
					wHeader = -1;
					thisPe->injectModPacket(Pkt,type.to,true);
					display=false;
				}
			}
			else if (type.edit == SCRIPT)
			{
				CPacket Pkt(pPacket, dwReturn);
				thisPe->black_list->runScript(wHeader,Pkt,true);
				if (wHeader == -1)
				{
					display=false;
				}
			}
			if (display && thisPe->show_send)
			{
				try
				{
				CPacket Pkt(pPacket, dwReturn);
				//QStructItem* Item = thisPe->struct_Logger->SgetItem(pPacket);
				thisPe->AddPacket(Pkt/*,Item*/);
				//thisPe->struct_Logger->SdeleteItem(pPacket);
				}
				catch (...)
				{ }
			}
	//	}
	}
}

VOID __fastcall SendPacket(__in LPVOID thisSocket, __in DWORD dwEDX, __in COutPacket* oPacket)
{
	HandleOutPacket(oPacket, reinterpret_cast<DWORD>(_ReturnAddress()));
	return InjectPacket(oPacket);
}

void __declspec(naked) InjectPacket(COutPacket* pPacket)
{
	__asm
	{
		//set class ptr
		mov ecx,[lppvCClientSocket]
		mov ecx,[ecx]

		//push packet and fake return address
		push [esp+4]
		push [lpvSomeRet] //00764fd7

		//send packet
		jmp [_SendPacket]
	}
}

//****************************************************************************************
//************************  Receive hook  ************************************************
//****************************************************************************************
lpfnRecvPacket _ProcessPacket = reinterpret_cast<lpfnRecvPacket>(0x004C9310);//(0x004AC240);
void HandleInPacket(CInPacket* pPacket, DWORD dwReturn)
{
	if (pPacket != NULL)
	{
		if (pPacket->usDataLen >= 2)
		{
			bool display = true;
			WORD& wHeader = pPacket->pHeader->wHeader;
			int i;
			blackType type = thisPe->black_list->find(wHeader, false , i);
			//black - white list switch
			if (thisPe->ignore_As_blackList)
			{
				if (type.edit == IGNORED)
					display = false;
			}
			else
			{
				if (type.edit == NONE)
				   display=false;
			}

			if (type.edit == BLOCK)
			{
				display = false;
				wHeader = -1;
			}
			else if (type.edit == MOD)
			{
				CPacket Pkt(pPacket, dwReturn);
				if(thisPe->pattern(Pkt,type.from))
				{
					wHeader = -1;
					thisPe->injectModPacket(Pkt,type.to,false);
					display=false;
				}
			}
			else if (type.edit == SCRIPT)
			{
				CPacket Pkt(pPacket, dwReturn);
				thisPe->black_list->runScript(wHeader,Pkt,false);
				if (wHeader == -1)
				{
					display=false;
				}
			}
			if (display && thisPe->show_recv)
			{
				try
				{
				CPacket Pkt(pPacket, dwReturn);
				//QStructItem* st_item = new QStructItem(NULL);
				//thisPe->struct_Logger->addItem(pPacket,st_item);
				thisPe->AddPacket(Pkt/*,st_item*/);
				}
				catch (...)
				{ }
			}
		}
	}
}

VOID __fastcall ProcessPacket(__in LPVOID thisSocket, __in DWORD dwEDX, __in CInPacket* pPacket)
{
	HandleInPacket(pPacket, reinterpret_cast<DWORD>(_ReturnAddress()));
	return InjectPacket(pPacket);
}

void InjectPacket(CInPacket* pPacket)
{
	return _ProcessPacket(*lppvCClientSocket, NULL, pPacket);
}

BYTE (__fastcall * _Decode1)(__inout CInPacket* inPacket, __in DWORD dwEDX) = NULL;
WORD (__fastcall * _Decode2)(__inout CInPacket* inPacket, __in DWORD dwEDX) = NULL;
DWORD (__fastcall * _Decode4)(__inout CInPacket* inPacket, __in DWORD dwEDX) = NULL;
LPCSTR* (__fastcall * _DecodeString)(__inout CInPacket* inPacket, __in DWORD dwEDX, __in LPVOID lpUnknown) = NULL;
VOID (__fastcall * _DecodeBuffer)(__inout CInPacket* inPacket, __in DWORD dwEDX, __out_bcount(uLength) PBYTE pb, __in UINT uLength) = NULL;


BYTE __fastcall Decode1(__inout CInPacket* inPacket, __in DWORD dwEDX)
{

	BYTE          b = _Decode1(inPacket, dwEDX);

	if(thisPe->show_recv)
	{
		WORD wHeader = inPacket->pHeader->wHeader;
		int i;
		blackType type = thisPe->black_list->find(wHeader, false , i);
		if ((type.edit == NONE)||(type.edit == SCRIPT))
		{
			thisPe->struct_Logger->add1(inPacket,b,reinterpret_cast<DWORD>(_ReturnAddress()));
			if (inPacket->uOffset >= inPacket->usLength)
			{
				thisPe->struct_Logger->deleteItem(inPacket);
			}
		}
	}

	

	return b;
}

WORD __fastcall Decode2(__inout CInPacket* inPacket, __in DWORD dwEDX)
{
	
	WORD          w = _Decode2(inPacket, dwEDX);

	if(thisPe->show_recv)
	{
		WORD wHeader = inPacket->pHeader->wHeader;
		int i;
		blackType type = thisPe->black_list->find(wHeader, false , i);
		if ((type.edit == NONE)||(type.edit == SCRIPT))
		{
			thisPe->struct_Logger->add2(inPacket,w,reinterpret_cast<DWORD>(_ReturnAddress()));
			if (inPacket->uOffset >= inPacket->usLength)
			{
				thisPe->struct_Logger->deleteItem(inPacket);
			}
		}
	}
	return w;
}

DWORD __fastcall Decode4(__inout CInPacket* inPacket, __in DWORD dwEDX)
{
	
	DWORD         dw = _Decode4(inPacket, dwEDX);

	if(thisPe->show_recv)
	{
		WORD wHeader = inPacket->pHeader->wHeader;
		int i;
		blackType type = thisPe->black_list->find(wHeader, false , i);
		if ((type.edit == NONE)||(type.edit == SCRIPT))
		{
			thisPe->struct_Logger->add4(inPacket,dw,reinterpret_cast<DWORD>(_ReturnAddress()));
			if (inPacket->uOffset >= inPacket->usLength)
			{
				thisPe->struct_Logger->deleteItem(inPacket);
			}
		}
	}
	return dw;
}

LPCSTR* __fastcall DecodeString(__inout CInPacket* inPacket, __in DWORD dwEDX, __in LPVOID lpUnknown)
{
	LPCSTR*         lplpcsz = _DecodeString(inPacket, dwEDX, lpUnknown);

	if(thisPe->show_recv)
	{
		WORD wHeader = inPacket->pHeader->wHeader;
		int i;
		blackType type = thisPe->black_list->find(wHeader, false , i);
		if ((type.edit == NONE)||(type.edit == SCRIPT))
		{
			thisPe->struct_Logger->addStr(inPacket,lplpcsz,reinterpret_cast<DWORD>(_ReturnAddress()));
			if (inPacket->uOffset >= inPacket->usLength)
			{
				thisPe->struct_Logger->deleteItem(inPacket);
			}
		}
	}
	return lplpcsz;
}

VOID __fastcall DecodeBuffer(__inout CInPacket* inPacket, __in DWORD dwEDX, __out_bcount(uLength) PBYTE pb, __in UINT uLength)
{

	_DecodeBuffer(inPacket, dwEDX, pb, uLength);

 	if (inPacket->iState != 2)
 		return;

	if(thisPe->show_recv)
	{
		WORD wHeader = inPacket->pHeader->wHeader;
		int i;
		blackType type = thisPe->black_list->find(wHeader, false , i);
		if ((type.edit == NONE)||(type.edit == SCRIPT))
		{
			thisPe->struct_Logger->addBuff(inPacket,pb,uLength,reinterpret_cast<DWORD>(_ReturnAddress()));
			if (inPacket->uOffset >= inPacket->usLength)
			{
				thisPe->struct_Logger->deleteItem(inPacket);
			}
		}
	}
}



///hooking
bool setHooks(PVOID pTarget, DWORD dwLocation , PVOID pHook, bool enable)
{
	//int i;

	if (DetourTransactionBegin() == NO_ERROR)
	{
		if (DetourUpdateThread(GetCurrentThread()) == NO_ERROR)
		{
			if(enable)
				*(PVOID*)pTarget = (PVOID)dwLocation;

			if((enable?DetourAttach:DetourDetach)((PVOID*)pTarget, pHook) != NO_ERROR)
				return FALSE;

			if (DetourTransactionCommit() == NO_ERROR)
			{
				return TRUE;
			}
		}

	}
	MessageBox(NULL, L"Detour failed!!",L"Something's wrong!",MB_OK);
	return FALSE;
}


typedef struct _MAPLE_FUNCTION
{
	PVOID pTarget;
	DWORD dwLocation;
} MAPLE_FUNCTION;

typedef struct _MAPLE_HOOK
{
	PVOID           pHook;
	MAPLE_FUNCTION  Function;
} MAPLE_HOOK;

const MAPLE_HOOK encodeHooks[] =
{   //send ones
	{ InitPacket,				 { &_InitPacket,  0x006B7E10 } },
	{ Encode1,					 { &_Encode1,     0x0040B5A0 } },
	{ Encode2,					 { &_Encode2,     0x004265F0 } },
	{ Encode4,                   { &_Encode4,	  0x0040B5F0 } },
	{ EncodeString,              { &_EncodeString,0x00481D10 } },
	{EncodeBuffer,				 { &_EncodeBuffer,0x0047C380 } },
	//recv ones
	{ Decode1,					 { &_Decode1,     0x004062C0 } },
	{ Decode2,					 { &_Decode2,     0x00425010 } },
	{ Decode4,					 { &_Decode4,     0x00406360 } },
	{ DecodeString,              { &_DecodeString,0x004351C0 } },
	{ DecodeBuffer,              { &_DecodeBuffer,0x0042CCB0 } }
};


BOOL SetDecodeHooks(bool enable)
{

	int i;

	if (DetourTransactionBegin() == NO_ERROR)
	{
		if (DetourUpdateThread(GetCurrentThread()) == NO_ERROR)
		{
			for (i = 0; i < sizeof(encodeHooks) / sizeof(*encodeHooks); i++)
			{
				if(enable)
				{
				*(PVOID*)encodeHooks[i].Function.pTarget = (PVOID)encodeHooks[i].Function.dwLocation;
				}

				if((enable?DetourAttach:DetourDetach)((PVOID*)encodeHooks[i].Function.pTarget, encodeHooks[i].pHook) != NO_ERROR)
					return FALSE;
			}
			// 				for (i = 0; i < sizeof(LoginFunctions) / sizeof(*LoginFunctions); i++)
			// 					*(PVOID*)LoginFunctions[i].pTarget = (PVOID)LoginFunctions[i].dwLocation;
			if (DetourTransactionCommit() == NO_ERROR)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}