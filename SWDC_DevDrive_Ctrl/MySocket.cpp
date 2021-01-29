#include "stdafx.h"
#include "MySocket.h"
#include <thread>
#include <condition_variable>
#include <stdexcept>
#include <iostream>
#include <string>

CMySocket::CMySocket(): m_TemperatureCallbackFunc(nullptr), m_ADOverloadAlarmCallback(nullptr), m_HeartBeatCallbackFunc(nullptr)
{
	m_ThreadFlag.store(true);
	locksection = new CRITICAL_SECTION();
}

CMySocket::~CMySocket()
{

}

//12 AD过载报警回调函数
void CMySocket::RegisterADOverloadAlarmCallbackFunc(Func1 AdCallBack)
{
	m_ADOverloadAlarmCallback = AdCallBack;
}

//15 心跳包
void CMySocket::RegisterHeartBeatCallbackFunc(Func2 HeartBeatCallBack)
{
	m_HeartBeatCallbackFunc = HeartBeatCallBack;
}

//温度信息回调函数
void CMySocket::RegisterTemperatureCallbackFunc(Func func)
{
	m_TemperatureCallbackFunc = func;
}

std::shared_ptr<unsigned char> CMySocket::GetDataPtr()
{
	std::shared_ptr<unsigned char> ptr;
	{
		std::lock_guard<std::mutex> lock(m_Mutex4Data);
		ptr = std::move(m_RecData);
	}
	m_Condition->notify_all();

	return ptr;
}

void CMySocket::SetSharedCondition(std::condition_variable *cond, std::atomic<bool> &Con_Flag)
{
	m_Condition = cond;
	m_ConditonFlag = &Con_Flag;
}

void CMySocket::AdOverProcess(std::shared_ptr<unsigned char> Cmd)
{
	_DEV_ADOverloadAlarm_RESULT Result;
	Result.DevID = static_cast<int>(Cmd.get()[2] & 0xff);
	Result.alarm[0] = static_cast<int>(((Cmd.get()[6]) & 1) >> 0);
	Result.alarm[1] = static_cast<int>(((Cmd.get()[6]) & 2) >> 1);
	Result.alarm[2] = static_cast<int>(((Cmd.get()[6]) & 4) >> 2);
	Result.alarm[3] = static_cast<int>(((Cmd.get()[6]) & 8) >> 3);
	Result.alarm[4] = static_cast<int>(((Cmd.get()[6]) & 16) >> 4);
	Result.alarm[5] = static_cast<int>(((Cmd.get()[6]) & 32) >> 5);
	Result.alarm[6] = static_cast<int>(((Cmd.get()[6]) & 64) >> 6);
	Result.alarm[7] = static_cast<int>(((Cmd.get()[6]) & 128) >> 7);

	Result.alarm[8] = static_cast<int>(((Cmd.get()[7]) & 1) >> 0);
	Result.alarm[9] = static_cast<int>(((Cmd.get()[7]) & 2) >> 1);
	Result.alarm[10] = static_cast<int>(((Cmd.get()[7]) & 4) >> 2);
	Result.alarm[11] = static_cast<int>(((Cmd.get()[7]) & 8) >> 3);
	Result.alarm[12] = static_cast<int>(((Cmd.get()[7]) & 16) >> 4);
	Result.alarm[13] = static_cast<int>(((Cmd.get()[7]) & 32) >> 5);
	Result.alarm[14] = static_cast<int>(((Cmd.get()[7]) & 64) >> 6);
	Result.alarm[15] = static_cast<int>(((Cmd.get()[7]) & 128) >> 7);

	Result.alarm[16] = static_cast<int>(((Cmd.get()[4]) & 1) >> 0);
	Result.alarm[17] = static_cast<int>(((Cmd.get()[4]) & 2) >> 1);
	Result.alarm[18] = static_cast<int>(((Cmd.get()[4]) & 4) >> 2);
	Result.alarm[19] = static_cast<int>(((Cmd.get()[4]) & 8) >> 3);

	if (m_ADOverloadAlarmCallback != NULL)
		m_ADOverloadAlarmCallback(Result);
}

int CMySocket::HeartVersion = 0;
int CMySocket::HeartDevTypeID = 0;

void CMySocket::HeartBeatProcess(std::shared_ptr<unsigned char> Cmd)
{
	_DEV_HEARTBEAT_RESULT heart;
	int sign1 = 0;
	if (HeartDevTypeID == 2)
	{
#pragma region 01 

		heart.DevID = static_cast<int>(Cmd.get()[5 + 16] & 0xff);
		heart.CC_Switch_State[0] = static_cast<int>(Cmd.get()[4 + 16] & 0xff);
		heart.CC_Clock_State[0] = static_cast<int>(Cmd.get()[7 + 16] & 0xff);

		if (Cmd.get()[16 + 16] == 0)
		{
			sign1 = 1;
		}
		else if (Cmd.get()[16 + 16] == 1)
		{
			sign1 = -1;
		}
		heart.CC_Temperature_State[0] = sign1 * ((static_cast<float>(Cmd.get()[6 + 16] & 0xff) + static_cast<float>(Cmd.get()[9 + 16] & 0xff) / 100));

		heart.CC_FPGA_State[0] = static_cast<int>(Cmd.get()[8 + 16] & 0xff);
		heart.RC_Net_State[0] = static_cast<int>(Cmd.get()[11 + 16] & 0xff);
		heart.RF_Module_State[0] = static_cast<int>(Cmd.get()[13 + 16] & 0x01);
		heart.GS_WorkMode[0] = static_cast<int>(Cmd.get()[12 + 16] & 0x01);
		heart.GS_Status[0] = static_cast<int>(Cmd.get()[15 + 16] & 0x01);

		float x1 = (Cmd.get()[14 + 16] & 0x0f) << 8;
		float y1 = (Cmd.get()[17 + 16] & 0xff);

		if (HeartVersion == 0)
		{
			heart.RF_Temperature_State[0] = (x1 + y1) / 4;
		}
		else
		{
			heart.RF_Temperature_State[0] = (x1 + y1) / 16;
		}

#pragma endregion

#pragma region 02

		heart.DevID = static_cast<int>(Cmd.get()[21 + 16] & 0xff);
		heart.CC_Switch_State[1] = static_cast<int>(Cmd.get()[20 + 16] & 0xff);
		heart.CC_Clock_State[1] = static_cast<int>(Cmd.get()[23 + 16] & 0xff);

		if (Cmd.get()[32 + 16] == 0)
		{
			sign1 = 1;
		}
		else if (Cmd.get()[32 + 16] == 1)
		{
			sign1 = -1;
		}
		heart.CC_Temperature_State[1] = sign1 * ((static_cast<float>(Cmd.get()[22 + 16] & 0xff) + static_cast<float>(Cmd.get()[25 + 16] & 0xff) / 100));

		heart.CC_FPGA_State[1] = static_cast<int>(Cmd.get()[24 + 16] & 0xff);
		heart.RC_Net_State[1] = static_cast<int>(Cmd.get()[27 + 16] & 0xff);
		heart.RF_Module_State[1] = static_cast<int>(Cmd.get()[29 + 16] & 0x01);
		heart.GS_WorkMode[1] = static_cast<int>(Cmd.get()[28 + 16] & 0x01);
		heart.GS_Status[1] = static_cast<int>(Cmd.get()[31 + 16] & 0x01);

		float x2 = (Cmd.get()[30 + 16] & 0x0f) << 8;
		float y2 = (Cmd.get()[33 + 16] & 0xff);

		if (HeartVersion == 0)
		{
			heart.RF_Temperature_State[1] = (x2 + y2) / 4;
		}
		else
		{
			heart.RF_Temperature_State[1] = (x2 + y2) / 16;
		}

#pragma endregion

#pragma region 03

		heart.DevID = static_cast<int>(Cmd.get()[5 + 16] & 0xff);
		heart.CC_Switch_State[2] = static_cast<int>(Cmd.get()[4 + 16] & 0xff);
		heart.CC_Clock_State[2] = static_cast<int>(Cmd.get()[7 + 16] & 0xff);

		if (Cmd.get()[16 + 16] == 0)
		{
			sign1 = 1;
		}
		else if (Cmd.get()[16 + 16] == 1)
		{
			sign1 = -1;
		}
		heart.CC_Temperature_State[2] = sign1 * ((static_cast<float>(Cmd.get()[6 + 16] & 0xff) + static_cast<float>(Cmd.get()[9 + 16] & 0xff) / 100));
		heart.CC_FPGA_State[2] = static_cast<int>(Cmd.get()[8 + 16] & 0xff);
		heart.RC_Net_State[2] = static_cast<int>(Cmd.get()[11 + 16] & 0Xff);
		heart.RF_Module_State[2] = static_cast<int>(Cmd.get()[13 + 16] >> 1);
		heart.GS_WorkMode[2] = static_cast<int>(Cmd.get()[12 + 16] >> 1);
		heart.GS_Status[2] = static_cast<int>(Cmd.get()[15 + 16] >> 1);

		float x3 = (Cmd.get()[19 + 16] & 0x0f) << 8;
		float y3 = (Cmd.get()[18 + 16] & 0xff);

		if (HeartVersion == 0)
		{
			heart.RF_Temperature_State[2] = (x3 + y3) / 4;
		}
		else
		{
			heart.RF_Temperature_State[2] = (x3 + y3) / 16;
		}

#pragma endregion

#pragma region 04

		heart.DevID = static_cast<int>(Cmd.get()[21 + 16] & 0xff);
		heart.CC_Switch_State[3] = static_cast<int>(Cmd.get()[20 + 16] & 0xff);
		heart.CC_Clock_State[3] = static_cast<int>(Cmd.get()[23 + 16] & 0xff);

		if (Cmd.get()[32 + 16] == 0)
		{
			sign1 = 1;
		}
		else if (Cmd.get()[32 + 16] == 1)
		{
			sign1 = -1;
		}
		heart.CC_Temperature_State[3] = sign1 * ((static_cast<float>(Cmd.get()[22 + 16] & 0xff) + static_cast<float>(Cmd.get()[25 + 16] & 0xff) / 100));
		heart.CC_FPGA_State[3] = static_cast<int>(Cmd.get()[24 + 16] & 0xff);
		heart.RC_Net_State[3] = static_cast<int>(Cmd.get()[27 + 16] & 0xff);
		heart.RF_Module_State[3] = static_cast<int>(Cmd.get()[29 + 16] >> 1);
		heart.GS_WorkMode[3] = static_cast<int>(Cmd.get()[28 + 16] >> 1);
		heart.GS_Status[3] = static_cast<int>(Cmd.get()[31 + 16] >> 1);

		float x4 = (Cmd.get()[35 + 16] & 0x0f) << 8;
		float y4 = (Cmd.get()[34 + 16] & 0xff);

		if (HeartVersion == 0)
		{
			heart.RF_Temperature_State[3] = (x4 + y4) / 4;
		}
		else
		{
			heart.RF_Temperature_State[3] = (x4 + y4) / 16;
		}

#pragma endregion

#pragma region 05

		heart.DevID = static_cast<int>(Cmd.get()[69] & 0xff);
		heart.CC_Switch_State[4] = 0x00;
		heart.CC_Clock_State[4] = 0x00;
		heart.CC_Temperature_State[4] = 0x00;
		heart.CC_FPGA_State[4] = 0x00;
		heart.RC_Net_State[4] = 0x00;
		heart.RF_Module_State[4] = 0x00;
		heart.GS_WorkMode[4] = 0x00;
		heart.GS_Status[4] = 0x00;
		heart.RF_Temperature_State[4] = 0x00;

#pragma endregion

		if (m_HeartBeatCallbackFunc != NULL)
			m_HeartBeatCallbackFunc(heart);
	}
	if (HeartDevTypeID == 3)
	{
#pragma region 01 

		heart.DevID = static_cast<int>(Cmd.get()[5] & 0xff);
		heart.CC_Switch_State[0] = static_cast<int>(Cmd.get()[4] & 0xff);
		heart.CC_Clock_State[0] = static_cast<int>(Cmd.get()[7] & 0xff);

		if (Cmd.get()[16] == 0)
		{
			sign1 = 1;
		}
		else if (Cmd.get()[16] == 1)
		{
			sign1 = -1;
		}
		heart.CC_Temperature_State[0] = sign1 * ((static_cast<float>(Cmd.get()[6] & 0xff) + static_cast<float>(Cmd.get()[9] & 0xff) / 100));
		heart.CC_FPGA_State[0] = static_cast<int>(Cmd.get()[8] & 0xff);
		heart.RC_Net_State[0] = static_cast<int>(Cmd.get()[11] & 0xff);
		heart.RF_Module_State[0] = static_cast<int>(Cmd.get()[13] & 0xff);
		heart.GS_WorkMode[0] = static_cast<int>(Cmd.get()[12] & 0xff);
		heart.GS_Status[0] = static_cast<int>(Cmd.get()[15] & 0xff);

		float x1 = (Cmd.get()[14] & 0x0f) << 8;
		float y1 = (Cmd.get()[17] & 0xff);

		if (HeartVersion == 0)
		{
			heart.RF_Temperature_State[0] = (x1 + y1) / 4;
		}
		else
		{
			heart.RF_Temperature_State[0] = (x1 + y1) / 16;
		}

#pragma endregion

#pragma region 02

		heart.DevID = static_cast<int>(Cmd.get()[21] & 0xff);
		heart.CC_Switch_State[1] = static_cast<int>(Cmd.get()[20] & 0xff);
		heart.CC_Clock_State[1] = static_cast<int>(Cmd.get()[23] & 0xff);

		if (Cmd.get()[32] == 0)
		{
			sign1 = 1;
		}
		else if (Cmd.get()[32] == 1)
		{
			sign1 = -1;
		}
		heart.CC_Temperature_State[1] = sign1 * ((static_cast<float>(Cmd.get()[22] & 0xff) + static_cast<float>(Cmd.get()[25] & 0xff) / 100));
		heart.CC_FPGA_State[1] = static_cast<int>(Cmd.get()[24] & 0xff);
		heart.RC_Net_State[1] = static_cast<int>(Cmd.get()[27] & 0xff);
		heart.RF_Module_State[1] = static_cast<int>(Cmd.get()[29] & 0xff);
		heart.GS_WorkMode[1] = static_cast<int>(Cmd.get()[28] & 0xff);
		heart.GS_Status[1] = static_cast<int>(Cmd.get()[31] & 0xff);

		float x2 = (Cmd.get()[30] & 0x0f) << 8;
		float y2 = (Cmd.get()[33] & 0xff);

		if (HeartVersion == 0)
		{
			heart.RF_Temperature_State[1] = (x2 + y2) / 4;
		}
		else
		{
			heart.RF_Temperature_State[1] = (x2 + y2) / 16;
		}
#pragma endregion

#pragma region 03
		heart.DevID = static_cast<int>(Cmd.get()[37] & 0xff);
		heart.CC_Switch_State[2] = static_cast<int>(Cmd.get()[36] & 0xff);
		heart.CC_Clock_State[2] = static_cast<int>(Cmd.get()[39] & 0xff);

		if (Cmd.get()[48] == 0)
		{
			sign1 = 1;
		}
		else if (Cmd.get()[48] == 1)
		{
			sign1 = -1;
		}
		heart.CC_Temperature_State[2] = sign1 * ((static_cast<float>(Cmd.get()[38] & 0xff) + static_cast<float>(Cmd.get()[41] & 0xff) / 100));
		heart.CC_FPGA_State[2] = static_cast<int>(Cmd.get()[40] & 0xff);
		heart.RC_Net_State[2] = static_cast<int>(Cmd.get()[43] & 0xff);
		heart.RF_Module_State[2] = static_cast<int>(Cmd.get()[45] & 0xff);
		heart.GS_WorkMode[2] = static_cast<int>(Cmd.get()[44] & 0xff);
		heart.GS_Status[2] = static_cast<int>(Cmd.get()[47] & 0xff);

		float x3 = (Cmd.get()[46] & 0x0f) << 8;
		float y3 = (Cmd.get()[49] & 0xff);

		if (HeartVersion == 0)
		{
			heart.RF_Temperature_State[2] = (x3 + y3) / 4;
		}
		else
		{
			heart.RF_Temperature_State[2] = (x3 + y3) / 16;
		}
#pragma endregion

#pragma region 04
		heart.DevID = static_cast<int>(Cmd.get()[53] & 0xff);
		heart.CC_Switch_State[3] = static_cast<int>(Cmd.get()[52] & 0xff);
		heart.CC_Clock_State[3] = static_cast<int>(Cmd.get()[55] & 0xff);

		if (Cmd.get()[64] == 0)
		{
			sign1 = 1;
		}
		else if (Cmd.get()[64] == 1)
		{
			sign1 = -1;
		}
		heart.CC_Temperature_State[3] = sign1 * ((static_cast<float>(Cmd.get()[54] & 0xff) + static_cast<float>(Cmd.get()[57] & 0xff) / 100));
		heart.CC_FPGA_State[3] = static_cast<int>(Cmd.get()[56] & 0xff);
		heart.RC_Net_State[3] = static_cast<int>(Cmd.get()[59] & 0xff);
		heart.RF_Module_State[3] = static_cast<int>(Cmd.get()[61] & 0xff);
		heart.GS_WorkMode[3] = static_cast<int>(Cmd.get()[60] & 0xff);
		heart.GS_Status[3] = static_cast<int>(Cmd.get()[63] & 0xff);

		float x4 = (Cmd.get()[62] & 0x0f) << 8;
		float y4 = (Cmd.get()[65] & 0xff);

		if (HeartVersion == 0)
		{
			heart.RF_Temperature_State[3] = (x4 + y4) / 4;
		}
		else
		{
			heart.RF_Temperature_State[3] = (x4 + y4) / 16;
		}
#pragma endregion

#pragma region 05

		heart.DevID = static_cast<int>(Cmd.get()[69] & 0xff);
		heart.CC_Switch_State[4] = 0x00;
		heart.CC_Clock_State[4] = 0x00;
		heart.CC_Temperature_State[4] = 0x00;
		heart.CC_FPGA_State[4] = 0x00;
		heart.RC_Net_State[4] = 0x00;
		heart.RF_Module_State[4] = 0x00;
		heart.GS_WorkMode[4] = 0x00;
		heart.GS_Status[4] = 0x00;
		heart.RF_Temperature_State[4] = 0x00;

#pragma endregion

		if (m_HeartBeatCallbackFunc != NULL)
			m_HeartBeatCallbackFunc(heart);
	}

	if (HeartDevTypeID == 0)
	{
#pragma region 01 

		heart.DevID = static_cast<int>(Cmd.get()[5] & 0xff);
		heart.CC_Switch_State[0] = static_cast<int>(Cmd.get()[4] & 0xff);
		heart.CC_Clock_State[0] = static_cast<int>(Cmd.get()[7] & 0xff);

		if (Cmd.get()[16] == 0)
		{
			sign1 = 1;
		}
		else if (Cmd.get()[16] == 1)
		{
			sign1 = -1;
		}
		heart.CC_Temperature_State[0] = sign1 * ((static_cast<float>(Cmd.get()[6] & 0xff) + static_cast<float>(Cmd.get()[9] & 0xff) / 100));
		heart.CC_FPGA_State[0] = static_cast<int>(Cmd.get()[8] & 0xff);
		heart.RC_Net_State[0] = static_cast<int>(Cmd.get()[11] & 0xff);
		heart.RF_Module_State[0] = static_cast<int>(Cmd.get()[13] & 0xff);
		heart.GS_WorkMode[0] = static_cast<int>(Cmd.get()[12] & 0xff);
		heart.GS_Status[0] = static_cast<int>(Cmd.get()[15] & 0xff);

		float x1 = (Cmd.get()[14] & 0x0f) << 8;
		float y1 = (Cmd.get()[17] & 0xff);

		if (HeartVersion == 0)
		{
			heart.RF_Temperature_State[0] = (x1 + y1) / 4;
		}
		else
		{
			heart.RF_Temperature_State[0] = (x1 + y1) / 16;
		}

#pragma endregion

#pragma region 02

		heart.DevID = static_cast<int>(Cmd.get()[21] & 0xff);
		heart.CC_Switch_State[1] = static_cast<int>(Cmd.get()[20] & 0xff);
		heart.CC_Clock_State[1] = static_cast<int>(Cmd.get()[23] & 0xff);

		if (Cmd.get()[32] == 0)
		{
			sign1 = 1;
		}
		else if (Cmd.get()[32] == 1)
		{
			sign1 = -1;
		}
		heart.CC_Temperature_State[1] = sign1 * ((static_cast<float>(Cmd.get()[22] & 0xff) + static_cast<float>(Cmd.get()[25] & 0xff) / 100));
		heart.CC_FPGA_State[1] = static_cast<int>(Cmd.get()[24] & 0xff);
		heart.RC_Net_State[1] = static_cast<int>(Cmd.get()[27] & 0xff);
		heart.RF_Module_State[1] = static_cast<int>(Cmd.get()[29] & 0xff);
		heart.GS_WorkMode[1] = static_cast<int>(Cmd.get()[28] & 0xff);
		heart.GS_Status[1] = static_cast<int>(Cmd.get()[31] & 0xff);

		float x2 = (Cmd.get()[30] & 0x0f) << 8;
		float y2 = (Cmd.get()[33] & 0xff);

		if (HeartVersion == 0)
		{
			heart.RF_Temperature_State[1] = (x2 + y2) / 4;
		}
		else
		{
			heart.RF_Temperature_State[1] = (x2 + y2) / 16;
		}
#pragma endregion

#pragma region 03
		heart.DevID = static_cast<int>(Cmd.get()[37] & 0xff);
		heart.CC_Switch_State[2] = static_cast<int>(Cmd.get()[36] & 0xff);
		heart.CC_Clock_State[2] = static_cast<int>(Cmd.get()[39] & 0xff);

		if (Cmd.get()[48] == 0)
		{
			sign1 = 1;
		}
		else if (Cmd.get()[48] == 1)
		{
			sign1 = -1;
		}
		heart.CC_Temperature_State[2] = sign1 * ((static_cast<float>(Cmd.get()[38] & 0xff) + static_cast<float>(Cmd.get()[41] & 0xff) / 100));
		heart.CC_FPGA_State[2] = static_cast<int>(Cmd.get()[40] & 0xff);
		heart.RC_Net_State[2] = static_cast<int>(Cmd.get()[43] & 0xff);
		heart.RF_Module_State[2] = static_cast<int>(Cmd.get()[45] & 0xff);
		heart.GS_WorkMode[2] = static_cast<int>(Cmd.get()[44] & 0xff);
		heart.GS_Status[2] = static_cast<int>(Cmd.get()[47] & 0xff);

		float x3 = (Cmd.get()[46] & 0x0f) << 8;
		float y3 = (Cmd.get()[49] & 0xff);

		if (HeartVersion == 0)
		{
			heart.RF_Temperature_State[2] = (x3 + y3) / 4;
		}
		else
		{
			heart.RF_Temperature_State[2] = (x3 + y3) / 16;
		}
#pragma endregion

#pragma region 04
		heart.DevID = static_cast<int>(Cmd.get()[53] & 0xff);
		heart.CC_Switch_State[3] = static_cast<int>(Cmd.get()[52] & 0xff);
		heart.CC_Clock_State[3] = static_cast<int>(Cmd.get()[55] & 0xff);

		if (Cmd.get()[64] == 0)
		{
			sign1 = 1;
		}
		else if (Cmd.get()[64] == 1)
		{
			sign1 = -1;
		}
		heart.CC_Temperature_State[3] = sign1 * ((static_cast<float>(Cmd.get()[54] & 0xff) + static_cast<float>(Cmd.get()[57] & 0xff) / 100));
		heart.CC_FPGA_State[3] = static_cast<int>(Cmd.get()[56] & 0xff);
		heart.RC_Net_State[3] = static_cast<int>(Cmd.get()[59] & 0xff);
		heart.RF_Module_State[3] = static_cast<int>(Cmd.get()[61] & 0xff);
		heart.GS_WorkMode[3] = static_cast<int>(Cmd.get()[60] & 0xff);
		heart.GS_Status[3] = static_cast<int>(Cmd.get()[63] & 0xff);

		float x4 = (Cmd.get()[62] & 0x0f) << 8;
		float y4 = (Cmd.get()[65] & 0xff);

		if (HeartVersion == 0)
		{
			heart.RF_Temperature_State[3] = (x4 + y4) / 4;
		}
		else
		{
			heart.RF_Temperature_State[3] = (x4 + y4) / 16;
		}
#pragma endregion

#pragma region 05

		heart.DevID = static_cast<int>(Cmd.get()[69] & 0xff);
		heart.CC_Switch_State[4] = static_cast<int>(Cmd.get()[68] & 0xff);
		heart.CC_Clock_State[4] = static_cast<int>(Cmd.get()[71] & 0xff);

		if (Cmd.get()[80] == 0)
		{
			sign1 = 1;
		}
		else if (Cmd.get()[80] == 1)
		{
			sign1 = -1;
		}
		heart.CC_Temperature_State[4] = sign1 * ((static_cast<float>(Cmd.get()[70] & 0xff) + static_cast<float>(Cmd.get()[73] & 0xff) / 100));
		heart.CC_FPGA_State[4] = static_cast<int>(Cmd.get()[72] & 0xff);
		heart.RC_Net_State[4] = static_cast<int>(Cmd.get()[75] & 0xff);
		heart.RF_Module_State[4] = static_cast<int>(Cmd.get()[77] & 0xff);
		heart.GS_WorkMode[4] = static_cast<int>(Cmd.get()[76] & 0xff);
		heart.GS_Status[4] = static_cast<int>(Cmd.get()[79] & 0xff);

		float x5 = (Cmd.get()[78] & 0x0f) << 8;
		float y5 = (Cmd.get()[81] & 0xff);

		if (HeartVersion == 0)
		{
			heart.RF_Temperature_State[4] = (x1 + y1) / 4;
		}
		else
		{
			heart.RF_Temperature_State[4] = (x1 + y1) / 16;
		}
#pragma endregion

		if (m_HeartBeatCallbackFunc != NULL)
			m_HeartBeatCallbackFunc(heart);
	}
}