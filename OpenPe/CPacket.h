#pragma once


#include <vector>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>

#include "MapleHook.h"

class CPacket
{
public:
	CPacket::CPacket();
	explicit CPacket(COutPacket* pPacket, DWORD dwRetnAddr);
	explicit CPacket(CInPacket* pPacket, DWORD dwRetnAddr);
	CPacket& operator=(CPacket& Packet);

	WORD GetHeader();

	bool operator()();

	static bool SendPacket(std::string& strPacket, std::string& strError = std::string());
	static bool RecvPacket(std::string& strPacket, std::string& strError = std::string());

	PktType Type;
	DWORD dwReturn;
	boost::shared_array<BYTE> bData;
	std::size_t cbData;

private:
	static void TokenizePacket(std::string& strPacket, std::vector<BYTE>& vData);

	bool SendPacket();
	bool RecvPacket();
};
