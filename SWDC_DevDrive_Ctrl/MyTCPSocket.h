#pragma once

#include "MySocket.h"
#include "DefineType.h"

class CMyTCPSocket : public CMySocket
{
public:
	CMyTCPSocket();
	CMyTCPSocket(char* DstIP, unsigned short CCPort, unsigned short FCPort, char* LocalIP);
	~CMyTCPSocket();

	int StartRevThread(void);
	int InitSock(char *DstIP, unsigned short CCPort, unsigned short FCPort, char * LocalIP);
	int Send2DevCmd_CC(std::shared_ptr<unsigned char> && Cmd, unsigned long CmdLen = 128);
	int Send2DevCmd_FC(std::shared_ptr<unsigned char> && Cmd, unsigned long CmdLen = 128);
	int Send2DevCmd_FC_X(std::pair<std::shared_ptr<unsigned char>, int > && t);
	
private:
	SOCKET m_Socket_CC;
	SOCKET m_Socket_FC;
	int RecvDataFunc();
};