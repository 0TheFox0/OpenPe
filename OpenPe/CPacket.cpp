#include "CPacket.h"

#undef NDEBUG
#include <cassert>
#include <random>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

CPacket::CPacket()
{

}

CPacket::CPacket(COutPacket* pPacket, DWORD dwRetnAddr)
{
	assert(pPacket != NULL);
	assert(pPacket->dwcbData >= 2);
	assert(pPacket->lpvData != NULL);

	this->Type = PktType::Out;
	this->dwReturn = dwRetnAddr;
	this->cbData = pPacket->dwcbData;
	this->bData.reset(new BYTE[this->cbData]);
	memcpy(this->bData.get(), pPacket->lpbData, this->cbData);
}

CPacket::CPacket(CInPacket* pPacket, DWORD dwRetnAddr)
{
	assert(pPacket != NULL);
	assert(pPacket->usDataLen >= 2);
	assert(pPacket->lpvData != NULL);

	this->Type = PktType::In;
	this->dwReturn = dwRetnAddr;

	this->cbData = pPacket->usDataLen;
	this->bData.reset(new BYTE[this->cbData]);
	memcpy(this->bData.get(), pPacket->pData->bData, this->cbData);
}

CPacket& CPacket::operator=(CPacket& Packet)
{
	this->Type = Packet.Type;
	this->dwReturn = Packet.dwReturn;
	this->cbData = Packet.cbData;
	this->bData.reset(new BYTE[this->cbData]);
	memcpy(this->bData.get(), Packet.bData.get(), this->cbData);

	return *this;
}

WORD CPacket::GetHeader()
{
	assert(this->cbData >= 2);
	assert(this->bData.get() != NULL);
	return (*reinterpret_cast<LPWORD>(this->bData.get()));
}

bool CPacket::SendPacket()
{
	assert(this->Type == PktType::Out);
	assert(this->cbData >= 2);
	assert(this->bData.get() != NULL);

	//copy packet data
	boost::scoped_array<BYTE> bTemp(new BYTE[this->cbData]);
	memcpy(bTemp.get(), this->bData.get(), this->cbData);

	//create object
	COutPacket Packet;
	SecureZeroMemory(&Packet, sizeof(COutPacket));
	Packet.lpbData = bTemp.get();
	Packet.dwcbData = this->cbData;

	try
	{
		InjectPacket(&Packet);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool CPacket::RecvPacket()
{
	assert(this->Type == PktType::In);
	assert(this->cbData >= 2);
	assert(this->bData.get() != NULL);

	//packet data
	std::vector<BYTE> vData;

	//generate random header
	static std::tr1::uniform_int<DWORD> gen;
	static std::tr1::mt19937 engine;
	DWORD dwHeader = gen(engine);

	//add header
	for (int i = 0; i < 32; i += 8)
		vData.push_back(static_cast<BYTE>((dwHeader >> i) & 0xFF));

	//add the packet data
	for (std::size_t index = 0; index < this->cbData; index++)
		vData.push_back(this->bData[index]);

	//construct packet object
	CInPacket Packet;
	SecureZeroMemory(&Packet, sizeof(CInPacket));
	Packet.iState = 2;
	Packet.lpvData = vData.data();
	Packet.usLength = vData.size();
	Packet.usRawSeq = dwHeader & 0xFFFF;
	Packet.usDataLen = Packet.usLength - sizeof(DWORD);
	Packet.usUnknown = 0; //0x00CC;
	Packet.uOffset = 4;

	//recv packet
	try
	{
		InjectPacket(&Packet);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool CPacket::operator()()
{
	if (this->Type == PktType::Out)
		return this->SendPacket();
	else
		return this->RecvPacket();
}

//Taken from StackOverflow
template <typename T>
struct HexTo
{
	T value;

	operator T() const
	{
		return value;
	}

	friend std::istream& operator>>(std::istream& in, HexTo& out)
	{
		in >> std::hex >> out.value;
		return in;
	}
};

void CPacket::TokenizePacket(std::string& strPacket, std::vector<BYTE>& vData)
{
	typedef boost::tokenizer<boost::char_separator<char>> Tokenizer;

	vData.clear();

	//generate random digits
	static std::tr1::uniform_int<BYTE> gen(0, 15);
	static std::tr1::mt19937 engine;

	static const char szDigits[] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F'
	};

	//replace wildcards with digits
	for (std::size_t index = 0; index < strPacket.length(); index++)
		if (strPacket[index] == '*')
			strPacket[index] = szDigits[gen(engine)];

	//convert tokens to bytes and add to vector
	Tokenizer Tokens(strPacket);
	for (Tokenizer::const_iterator iterate = Tokens.begin(), end = Tokens.end(); iterate != end; iterate++)
		vData.push_back(static_cast<BYTE>(boost::lexical_cast<HexTo<unsigned int>>(*iterate)));
}

bool CPacket::SendPacket(std::string& strPacket, std::string& strError)
{
	strError.clear();

	//remove useless spaces
	boost::trim(strPacket);

	//sanity check
	if (strPacket.empty())
	{
		strError = "Please enter a packet.";
		return false;
	}

	std::vector<BYTE> vData;

	//add packet data from tokenized string
	try
	{
		CPacket::TokenizePacket(strPacket, vData);
	}
	catch (boost::bad_lexical_cast& exc)
	{
		strError = exc.what();
		return false;
	}

	//construct packet object
	COutPacket Packet;
	SecureZeroMemory(&Packet, sizeof(COutPacket));
	Packet.dwcbData = vData.size();
	Packet.lpbData = vData.data();

	//send packet
	try
	{
		InjectPacket(&Packet);
		return true;
	}
	catch (...)
	{
		strError = "Internal error!";
		return false;
	}
}

bool CPacket::RecvPacket(std::string& strPacket, std::string& strError)
{
	strError.clear();

	//remove useless spaces
	boost::trim(strPacket);

	//sanity check
	if (strPacket.empty())
	{
		strError = "Please enter a packet.";
		return false;
	}

	std::vector<BYTE> vData;

	//add packet data from tokenized string
	try
	{
		CPacket::TokenizePacket(strPacket, vData);
	}
	catch (boost::bad_lexical_cast& exc)
	{
		strError = exc.what();
		return false;
	}

	//generate random header
	static std::tr1::uniform_int<DWORD> gen;
	static std::tr1::mt19937 engine;
	DWORD dwHeader = gen(engine);

	//add header
	for (int i = 0; i < sizeof(DWORD); i++)
		vData.insert(vData.begin(), static_cast<BYTE>(dwHeader >> (i * 8)));

	//construct packet object
	CInPacket Packet;
	SecureZeroMemory(&Packet, sizeof(CInPacket));
	Packet.iState = 2;
	Packet.lpvData = vData.data();
	Packet.usLength = vData.size();
	Packet.usRawSeq = dwHeader & 0xFFFF;
	Packet.usDataLen = Packet.usLength - sizeof(DWORD);
	Packet.usUnknown = 0; //0x00CC;
	Packet.uOffset = 4;

	//recv packet
	try
	{
		InjectPacket(&Packet);
		return true;
	}
	catch (...)
	{
		strError = "Internal error!";
		return false;
	}
}