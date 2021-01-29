#include "stdafx.h"
#include "MyUDPSocket.h"
#include <thread>
#include <condition_variable>
#include <stdexcept>
#include <iostream>
#include <string>

CMyUDPSocket::CMyUDPSocket() : CMySocket()
{

}

CMyUDPSocket::CMyUDPSocket(char* DstIP, unsigned short CCPort, unsigned short FCPort, char* LocalIP) : CMySocket()
{
	InitSock(DstIP, CCPort, FCPort, LocalIP);
}

CMyUDPSocket::~CMyUDPSocket()
{
	m_ThreadFlag.store(false);
	std::unique_lock<std::mutex> lock(m_Mutex4StopThread);
	m_Condition4StopThread.wait(lock);
	::closesocket(m_Socket);
}

int CMyUDPSocket::InitSock(char *DstIP, unsigned short CCPort, unsigned short FCPort, char *LocalIP)
{
	int ret = WSAStartup(MAKEWORD(2, 2), &m_wsaDATA);
	if (ret != 0)
	{
		std::cerr << "ERROR: Net Initialize Error. CODE: -1" << std::endl;
		return -1;
	}
	if ((m_Socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
	{
		std::cerr << "ERROR: Net Initialize Error. CODE: -2" << std::endl;
		return -2;
	}
	m_DestinationADDR_CC.sin_family = AF_INET;
	m_DestinationADDR_CC.sin_port = ::htons(CCPort);
	//m_DestinationADDR_CC.sin_addr.s_addr = inet_addr(DstIP);
	inet_pton(AF_INET, DstIP, &m_DestinationADDR_CC.sin_addr.s_addr);

	m_DestinationADDR_FC.sin_family = AF_INET;
	m_DestinationADDR_FC.sin_port = ::htons(FCPort);
	//m_DestinationADDR_FC.sin_addr.s_addr = inet_addr(DstIP);
	inet_pton(AF_INET, DstIP, &m_DestinationADDR_FC.sin_addr.s_addr);

	RecvADDR.sin_family = AF_INET;
	RecvADDR.sin_port = ::htons(4000);
	//RecvADDR.sin_addr.s_addr = inet_addr(LocalIP);
	inet_pton(AF_INET, LocalIP, &RecvADDR.sin_addr.s_addr);

	int optval = 1024 * 1024 * 10;
	::setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (char*)&optval, sizeof(optval));
	if (::bind(m_Socket, (sockaddr *)&RecvADDR, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		std::cerr << "ERROR: Net Initialize Error. CODE: -3" << std::endl;
		return -3;
	}
	return 0;
}

int CMyUDPSocket::Send2DevCmd_CC(std::shared_ptr<unsigned char> && Cmd, unsigned long CmdLen)
{
	int len = sendto(m_Socket, (char*)(Cmd.get()), CmdLen, 0, (sockaddr*)&m_DestinationADDR_CC, sizeof(SOCKADDR_IN));
	if (len < 0)
		std::cerr << "Socket Error: SendError;" << std::endl;
	return len;
}

int CMyUDPSocket::Send2DevCmd_FC(std::shared_ptr<unsigned char> && Cmd, unsigned long CmdLen)
{
	int len = sendto(m_Socket, (char*)(Cmd.get()), CmdLen, 0, (sockaddr*)&m_DestinationADDR_FC, sizeof(SOCKADDR_IN));
	if (len < 0)
		std::cerr << "Socket Error: SendError;" << std::endl;
	return len;
}

int CMyUDPSocket::Send2DevCmd_FC_X(std::pair<std::shared_ptr<unsigned char>, int > && t)
{
	int len = sendto(m_Socket, (char*)(t.first.get()), t.second, 0, (sockaddr*)&m_DestinationADDR_FC, sizeof(SOCKADDR_IN));
	if (len < 0)
		std::cerr << "Socket Error: SendError;" << std::endl;
	return len;
}

int CMyUDPSocket::StartRevThread(void)
{
	std::thread Thread(&CMyUDPSocket::RecvDataFunc, this);
	Thread.detach();
	return 0;
}

int CMyUDPSocket::RecvDataFunc()
{
	int RevLen = sizeof(SOCKADDR_IN);
	while (m_ThreadFlag.load())
	{
		std::shared_ptr<unsigned char> Cmd(new unsigned char[1024], [](unsigned char *p){ delete[] p; });
		int ret = ::recvfrom(m_Socket, (char*)(Cmd.get()), 1024, 0, (sockaddr*)&RecvADDR, &RevLen);
		int ErrorNo = 0;
		if (ret < 0)
		{
			ErrorNo = WSAGetLastError();
			throw std::exception("ERROR: RecvFrom ERROR!!");
			continue;
		}
		if (Cmd.get()[2] == 0x7d)
		{
			int sign = 0;
			if (Cmd.get()[4] == 0)
			{
				sign = 1;
			}
			else if (Cmd.get()[4] == 1)
			{
				sign = -1;
			}
			float temperature_h = static_cast<float>(Cmd.get()[7] & 0xff);
			float temperature_l = static_cast<float>(Cmd.get()[6] & 0xff);
			float t = sign*(temperature_h + temperature_l / 100);

			if (m_TemperatureCallbackFunc != nullptr)
				m_TemperatureCallbackFunc(t);
		}
		
		//心跳包
		if (Cmd.get()[0] == 0xBC && Cmd.get()[1] == 0xBC && Cmd.get()[2] == 0xBC && Cmd.get()[3] == 0xBC)
		{
			HeartBeatProcess(Cmd);
		}
		//AD过载报警回调函数
		if (Cmd.get()[1] == 0xca && (Cmd.get()[4] & 0xf0) == 0x10 && Cmd.get()[5] == 0xcc)
		{
			AdOverProcess(Cmd);
		}
		if (Cmd.get()[1] == 0xca || Cmd.get()[0] == 0x77 || Cmd.get()[1] == 0x07 || ((Cmd.get()[0] == 0x5A) && Cmd.get()[1] == 0x58 && Cmd.get()[2] == 0x4C && Cmd.get()[3] == 0x43))
		{
			std::lock_guard<std::mutex> lock(m_Mutex4Data);
			m_RecData = std::move(Cmd);
			m_ConditonFlag->store(true);
			m_Condition->notify_all();
		}
	}

	m_Condition4StopThread.notify_all();
	return 0;
}