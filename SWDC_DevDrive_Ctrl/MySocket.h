#pragma once

#include <WS2tcpip.h>
#include <iostream>
#include <memory>
#include "DefineType.h"
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

class CMySocket
{
public:
	CMySocket();
	virtual ~CMySocket();
	virtual int StartRevThread(void) = 0;
	virtual int InitSock(char* DstIP, unsigned short CCPort, unsigned short FCPort, char* LocalIP) = 0;
	virtual int Send2DevCmd_CC(std::shared_ptr<unsigned char>&& Cmd, unsigned long CmdLen = 128) = 0;
	virtual int Send2DevCmd_FC(std::shared_ptr<unsigned char>&& Cmd, unsigned long CmdLen = 128) = 0;
	virtual int Send2DevCmd_FC_X(std::pair<std::shared_ptr<unsigned char>, int>&& t) = 0;

	virtual void RegisterTemperatureCallbackFunc(Func func);
	virtual void RegisterADOverloadAlarmCallbackFunc(Func1 AdCallBack);
	virtual void RegisterHeartBeatCallbackFunc(Func2 HeartBeatCallBack);
	virtual void SetSharedCondition(std::condition_variable* cond, std::atomic<bool>& Con_Flag);
	virtual std::shared_ptr<unsigned char> GetDataPtr();

	static int HeartVersion;
	static int HeartDevTypeID;
	
protected:
	WSADATA m_wsaDATA;
	SOCKADDR_IN RecvADDR;
	SOCKADDR_IN m_DestinationADDR_CC;
	SOCKADDR_IN m_DestinationADDR_FC;

	std::condition_variable *m_Condition = nullptr;
	std::atomic<bool> *m_ConditonFlag = nullptr;
	std::atomic<bool> m_ThreadFlag;
	std::mutex m_Mutex4StopThread;
	std::condition_variable m_Condition4StopThread;
	std::shared_ptr<unsigned char> m_RecData;
	std::mutex m_Mutex4Data;

	//温度回调函数指针
	Func m_TemperatureCallbackFunc = nullptr;
	//AD过载报警回调函数
	Func1 m_ADOverloadAlarmCallback = nullptr;
	//心跳包回调函数
	Func2 m_HeartBeatCallbackFunc = nullptr;

	LPCRITICAL_SECTION locksection;

	virtual void AdOverProcess(std::shared_ptr<unsigned char> Cmd);
	virtual void HeartBeatProcess(std::shared_ptr<unsigned char> Cmd);
};
