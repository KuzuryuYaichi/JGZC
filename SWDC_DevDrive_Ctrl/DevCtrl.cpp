#include "stdafx.h"
#include "DevCtrl.h"
#include <mutex>
#include <condition_variable>
#include <memory>
#include <string>
#include <iostream>
#include <MMSystem.h>

#pragma comment(lib, "winmm.lib")

CDevCtrl *CDevCtrl::m_Instance = nullptr;

//将16进制数转换为字符串
void hex_to_str(char *ptr, char *buf, int len)
{
	for (int i = 0; i < len; i++)
	{
		unsigned char cTmp = (unsigned char)buf[i];
		cTmp = (cTmp & 0xff);
		sprintf(ptr, "%02x", cTmp);
		ptr += 2;
	}
}

//将IP点分十进制(字符串)转换为字节
void atob11(char *src, int *dest)
{
	std::string ip_end = ".1";
	std::string src_transit[4];//裁剪的后端字符串
	std::string src_tr[4];  //裁剪的前端字符串
	int pos[4];        //点的位置

	std::string src_s = static_cast<std::string>(src);
	src_transit[0] = src_s + ip_end;

	pos[0] = src_transit[0].find('.');
	src_tr[0] = src_transit[0].substr(0, pos[0]);
	dest[0] = atoi(src_tr[0].c_str());

	for (int i = 1; i < 4; i++)
	{
		src_transit[i] = src_transit[i - 1].substr(pos[i - 1] + 1, src_transit[i - 1].length() - 1);
		pos[i] = src_transit[i].find('.');
		src_tr[i] = src_transit[i].substr(0, pos[i]);
		dest[i] = atoi(src_tr[i].c_str());
	}
}

CDevCtrl::CDevCtrl(char *DstIP, unsigned short CCPort, unsigned short FCPort, char *LocalIP) :m_Socket(nullptr)
{
	if (InitNetInfo(DstIP, CCPort, FCPort, LocalIP))
		throw std::exception("驱动网络初始化异常\r\n");

	std::unique_ptr<CDevCtrlCmd> sp(new CDevCtrlCmd());
	m_DevCtrCmd = std::move(sp);
}

CDevCtrl::~CDevCtrl()
{

}

int CDevCtrl::InitNetInfo(char *DstIP, unsigned short CCPort, unsigned short FCPort, char *LocalIP)
{
	m_Socket = std::unique_ptr<CMySocket>(new CMyUDPSocket(DstIP, CCPort, FCPort, LocalIP));

	if (m_Socket == nullptr)
		return -1;
	else
	{
		m_Socket->SetSharedCondition(&m_Condition, m_ConditionFlag);//200103LX条件变量之间的传递
		m_Socket->StartRevThread();
		return 0;
	}
}

#pragma region 初始化
//初始化函数，Singleton Model
CDevCtrl * CDevCtrl::CreateDevCtrlInstance(char *DstIP, unsigned short CCPort, unsigned short FCPort, char *LocalIP)
{
	if (nullptr == m_Instance)
	{
		try
		{
			m_Instance = new CDevCtrl(DstIP, CCPort, FCPort, LocalIP);
		}
		catch (std::exception e)
		{
			std::cerr << e.what() << std::endl;
			delete m_Instance;
			return (m_Instance = nullptr);
		}
	}
	return m_Instance;
}
//释放资源
void CDevCtrl::FreeDevCtrlInstance()
{
	if (nullptr != m_Instance)
	{
		delete m_Instance;
		m_Instance = nullptr;
	}
}
#pragma endregion


#pragma region 千兆网网络配置
int CDevCtrl::SetLocalIP(char *IP)
{
	if (m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_SetLocalIP(IP)) < 0)
		return CODE_ERROR_NetError;
	else
		return CODE_OK;
}

int CDevCtrl::SetDevIP(char *IP)
{
	if (m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_SetDevIP(IP)) < 0)
		return CODE_ERROR_NetError;
	else
		return CODE_OK;
}

int CDevCtrl::SetDevCCPort(unsigned short Port)
{
	if (m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_SetDevCCPort(Port)) < 0)
		return CODE_ERROR_NetError;
	else
		return CODE_OK;
}

int CDevCtrl::SetDevFCPort(unsigned short Port)
{
	if (m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_SetDevFCPort(Port)) < 0)
		return CODE_ERROR_NetError;
	else
		return CODE_OK;
}
#pragma endregion

#pragma region FPGA程序下载
int CDevCtrl::LoadCC_FPGABinFile(int DevID, char * FilePath)
{
	if (DevID < 1 || DevID > 5)
		return CODE_ERROR_ConfigError;

	//擦除
	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_DevCC_FPGAErasure(DevID)) < 0)
		return CODE_ERROR_NetError;
	{
		std::unique_lock<std::mutex> lock1(m_mutex);
		m_Condition.wait(lock1);
	}
	DEV_CCFPGA_LOAD_RESULT Reslut_e;
	while (true)
	{
		int ret1 = m_DevCtrCmd->MakeCmd_DevCC_FPGA_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut_e);
		if (ret1 == 0)
			break;
		else
		{
			if (ret1 == 1)
				continue;
		}
	}

	if (Reslut_e.Status != 1)
		return CODE_ERROR_ErasureError;

	//版本号
	WIN32_FIND_DATA ffd;
	WCHAR FilePath_1[256];
	MultiByteToWideChar(CP_ACP, 0, FilePath, strlen(FilePath) + 1, FilePath_1, sizeof(FilePath_1) / sizeof(FilePath_1[0]));
	HANDLE hFind = FindFirstFile(FilePath_1, &ffd);
	FILETIME ftime = ffd.ftLastWriteTime;
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&ftime, &sysTime);
	FindClose(hFind);
	int f_year = sysTime.wYear;
	int f_month = sysTime.wMonth;
	int f_day = sysTime.wDay;
	int Version = 1;
	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_DevCC_FPGAVersion(DevID, Version)) < 0)
		return CODE_ERROR_NetError;

	//数据
	FILE *file;
	std::cout << "FILE PATH:" << FilePath << std::endl;
	fopen_s(&file, FilePath, "rb");
	int fileLen;
	int index = 0;
	char buffer[512];
	while (true)
	{
		memset(buffer, 0, 512);
		fileLen = fread(buffer, 1, 512, file);

		if ((m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_DevCC_FPGAWriting(DevID, index, buffer), 1032)) < 0)
			return CODE_ERROR_NetError;

		std::unique_lock<std::mutex> lock2(m_mutex);
		m_Condition.wait(lock2);

		DEV_CCFPGA_LOAD_RESULT Reslut_w;
		while (true)
		{
			int ret2 = m_DevCtrCmd->MakeCmd_DevCC_FPGA_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut_w);
			if (ret2 == 0)
				break;
			else
			{
				if (ret2 == 1)
					continue;
			}
		}
		if (Reslut_w.Status != 1)
			return CODE_ERROR_WriteError;

		index++;
		if (feof(file))
		{
			break;
		}
	}

	fclose(file);
	//写入完毕
	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_DevCC_FPGAWriteEnd(DevID)) < 0)
		return CODE_ERROR_NetError;

	return CODE_OK;
}

int CDevCtrl::LoadRC_FPGABinFile(int FlashID, char * FilePath)
{
	if (FlashID < 1 || FlashID > 4)
		return CODE_ERROR_ConfigError;
	//擦除
	if (m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_DevRC_FPGAErasure(FlashID)) < 0)
		return CODE_ERROR_NetError;

	DEV_RCFPGA_LOAD_RESULT Reslut_e;
	while (true)
	{
		int ret1 = m_DevCtrCmd->MakeCmd_DevRC_FPGAWriting_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut_e);
		if (ret1 == 0)
			break;
		else
		{
			if (ret1 == 1)
				continue;
		}
	}
	if (Reslut_e.Status != 1)
		return CODE_ERROR_ErasureError;

	//数据
	FILE *file;
	fopen_s(&file, FilePath, "rb");
	int fileLen;
	int index = 0;
	while (!feof(file))
	{
		index++;
		std::cout << "Read File INDEX = " << index << std::endl;
		if (index == 1)
		{
			fseek(file, 0L, SEEK_END);
			int fileLeng = ftell(file);
			fseek(file, 0L, SEEK_SET);
			char buffer[1020];
			memset(buffer, 0, 1020);
			fileLen = fread(buffer, 1, 1020, file);
			if (m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_DevRC_FPGAWriting(FlashID, index, buffer, fileLeng), 1056) < 0)
				return CODE_ERROR_NetError;
			{
				std::unique_lock<std::mutex> lock21(m_mutex);
				m_Condition.wait(lock21);
			}
			DEV_RCFPGA_LOAD_RESULT Reslut_w;
			while (true)
			{
				int ret2 = m_DevCtrCmd->MakeCmd_DevRC_FPGAWriting_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut_w);
				std::cout << "写入是否成功返回值：" << ret2 << std::endl;
				if (ret2 == 0)
					break;
				else
				{
					if (ret2 == 1)
						continue;
				}
			}
			if (Reslut_w.Status != 3)
				return CODE_ERROR_WriteError;
		}
		else
		{
			char buffer0[1024];
			memset(buffer0, 0, 1024);
			fileLen = fread(buffer0, 1, 1024, file);
			std::cout << "Read File Len = " << fileLen << std::endl;

			if (m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_DevRC_FPGAWriting(FlashID, index, buffer0, 1), 1056) < 0)
				return CODE_ERROR_NetError;
			{
				std::unique_lock<std::mutex> lock2(m_mutex);
				m_Condition.wait(lock2);
			}
			DEV_RCFPGA_LOAD_RESULT Reslut_w1;
			while (true)
			{
				int ret2 = m_DevCtrCmd->MakeCmd_DevRC_FPGAWriting_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut_w1);
				std::cout << "写入是否成功返回值：" << ret2 << std::endl;
				if (ret2 == 0)
					break;
				else
				{
					if (ret2 == 1)
						continue;
				}
			}
			if (Reslut_w1.Status != 3)
				return CODE_ERROR_WriteError;
		}
	}
	fclose(file);

	//写完毕
	if (m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_DevRC_FPGAWriteEnd(FlashID)) < 0)
		return CODE_ERROR_NetError;

	//加载
	if (m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_DevRC_FPGALoad(FlashID)) < 0)
		return CODE_ERROR_NetError;

	return CODE_OK;
}
#pragma endregion

#pragma region 设备自检查询
int CDevCtrl::InquireDevRC_CheckSelf(PDEV_RC_CHECKSELF_RESULT Reslut)
{
	m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_DevRC_CheckSelf());

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_DevRC_CheckSelf_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}

int CDevCtrl::InquireDevRC_CheckSelf8(PDEV_RC_CHECKSELF_RESULT Reslut)
{
	m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_DevRC_CheckSelf());

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_DevRC_CheckSelf_Ret_Analysis8(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}

#pragma endregion

#pragma region 万兆网数据传输配置
int CDevCtrl::SetDataDestIP(int NetID, int connectionId, char *ip, unsigned short  disPort, unsigned short sourcePort)
{
	if (NetID < 1 || NetID > 8)
		return CODE_ERROR_ConfigError;
	if (connectionId < 0 || connectionId > 255)
		return CODE_ERROR_ConfigError;
	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_SetDataDestIP(NetID, connectionId, ip, disPort, sourcePort)) < 0)
		return CODE_ERROR_NetError;

	return CODE_OK;
}
int CDevCtrl::SetDataSourceIP(int NetID, char *ip, int PacketLen, int Interval, int SlicesClock)
{
	if (NetID < 1 || NetID > 8)
		return CODE_ERROR_ConfigError;

	char *mac = NULL;
	switch (NetID)
	{
	case 1:
		mac = MAC_1;
		break;
	case 2:
		mac = MAC_2;
		break;
	case 3:
		mac = MAC_3;
		break;
	case 4:
		mac = MAC_4;
		break;
	case 5:
		mac = MAC_5;
		break;
	case 6:
		mac = MAC_6;
		break;
	case 7:
		mac = MAC_7;
		break;
	case 8:
		mac = MAC_8;
		break;
	default:
		break;
	}
	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_SetDataSourceIP(NetID, ip, mac, PacketLen, Interval, SlicesClock)) < 0)
		return CODE_ERROR_NetError;

	return CODE_OK;
}

//wdx添加
int CDevCtrl::SetLinkEnable(int NetID, int linkNum, int en)
{
	if (NetID < 1 || NetID > 8)
		return CODE_ERROR_ConfigError;
	if (linkNum < 0 || linkNum > 255)
		return CODE_ERROR_ConfigError;
	if ((en < 0) || (en > 1))
		return CODE_ERROR_ConfigError;
	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_SetDataSendSource(NetID, linkNum, en)) < 0)//打开
		return CODE_ERROR_NetError;

	return CODE_OK;
}
//数据复位
int CDevCtrl::SetDataReset(int dataSource)
{
	if ((dataSource < 0) || (dataSource > 1))
		return CODE_ERROR_ConfigError;
	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_DataReset(0, 1, dataSource)) < 0)//打开
		return CODE_ERROR_NetError;
	Sleep(1);
	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_DataReset(0, 0, dataSource)) < 0)//打开
		return CODE_ERROR_NetError;
	Sleep(1);
	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_DataReset(1, 1, dataSource)) < 0)//打开
		return CODE_ERROR_NetError;
	Sleep(1);
	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_DataReset(1, 0, dataSource)) < 0)//打开
		return CODE_ERROR_NetError;

	return CODE_OK;
}

int CDevCtrl::SetDataReset_X(int boardNum, int enable, int dataSource)
{
	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_DataReset(boardNum, enable, dataSource)) < 0)//打开
		return CODE_ERROR_NetError;
	return CODE_OK;
}

int CDevCtrl::SetDataInformation(int Band, int Standby, char * DisIP, unsigned short  DisPort, unsigned short SourcePort, int Enabled)
{
	if (Band < 0 || Band > 65)
		return CODE_ERROR_ConfigError;

	int NetID = 0;
	int connectionId = 0;
	if (Band >= 0 && Band <= 15)
	{
		if (Standby == 0)
		{
			NetID = 1;
			connectionId = Band;
		}
		else
		{
			NetID = 2;
			connectionId = Band;
		}
	}
	else if (Band >= 16 && Band <= 25)
	{
		if (Standby == 0)
		{
			NetID = 3;
			connectionId = Band - 16;
		}
		else
		{
			NetID = 4;
			connectionId = Band - 16;
		}
	}
	else if (Band >= 26 && Band <= 45)
	{
		if (Standby == 0)
		{
			NetID = 5;
			connectionId = Band - 26;
		}
		else
		{
			NetID = 6;
			connectionId = Band - 26;
		}
	}
	else if (Band >= 46 && Band <= 65)
	{
		if (Standby == 0)
		{
			NetID = 7;
			connectionId = Band - 46;
		}
		else
		{
			NetID = 8;
			connectionId = Band - 46;
		}
	}

	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_SetDataDestIP(NetID, connectionId, DisIP, DisPort, SourcePort)) < 0)
		return CODE_ERROR_NetError;

	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_SetDataSendSource(NetID, connectionId, Enabled)) < 0)//打开
		return CODE_ERROR_NetError;
	return CODE_OK;
}

int CDevCtrl::SetDataInformation_Multi(int Band, int multi, int Standby, char * DisIP, unsigned short  DisPort, unsigned short SourcePort, int Enabled)
{
	if (Band < 0 || Band > 65)
		return CODE_ERROR_ConfigError;

	int NetID = 0;
	int connectionId = 0;
	if (Band >= 0 && Band <= 15)
	{
		if (Standby == 0)
		{
			NetID = 1;
			connectionId = Band;
		}
		else
		{
			NetID = 2;
			connectionId = Band;
		}
	}
	else if (Band >= 16 && Band <= 25)
	{
		if (Standby == 0)
		{
			NetID = 3;
			connectionId = Band - 16;
		}
		else
		{
			NetID = 4;
			connectionId = Band - 16;
		}
	}
	else if (Band >= 26 && Band <= 45)
	{
		if (Standby == 0)
		{
			NetID = 5;
			connectionId = Band - 26;
		}
		else
		{
			NetID = 6;
			connectionId = Band - 26;
		}
	}
	else if (Band >= 46 && Band <= 65)
	{
		if (Standby == 0)
		{
			NetID = 7;
			connectionId = Band - 46;
		}
		else
		{
			NetID = 8;
			connectionId = Band - 46;
		}
	}

	//****************************************将组播IP转化为组播MAC地址**********************************************//
	int ipCtr[4];
	atob11(DisIP, ipCtr);
	char MultiMac[6];
	MultiMac[0] = 0x01;
	MultiMac[1] = 0x00;
	MultiMac[2] = 0x5E;
	MultiMac[3] = ipCtr[1] & 0x7f;
	MultiMac[4] = ipCtr[2];
	MultiMac[5] = ipCtr[3];

	char MultiMac_ptr[32];
	hex_to_str(MultiMac_ptr, MultiMac, 6);

	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_SetMultiSelect(NetID, connectionId, multi)) < 0)
		return CODE_ERROR_NetError;

	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_SetDataDestIP(NetID, connectionId, DisIP, DisPort, SourcePort)) < 0)
		return CODE_ERROR_NetError;

	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_SetMultiMac(NetID, connectionId, MultiMac_ptr)) < 0)
		return CODE_ERROR_NetError;

	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_SetDataSendSource(NetID, connectionId, Enabled)) < 0)//打开
		return CODE_ERROR_NetError;
	return CODE_OK;
}

int CDevCtrl::SetDataEnable(int Band, int Standby, int Enabled)
{
	if (Band < 0 || Band > 65)
		return CODE_ERROR_ConfigError;

	int NetID = 0;
	int connectionId = 0;
	if (Band >= 0 && Band <= 15)
	{
		if (Standby == 0)
		{
			NetID = 1;
			connectionId = Band;
		}
		else
		{
			NetID = 2;
			connectionId = Band;
		}
	}
	else if (Band >= 16 && Band <= 25)
	{
		if (Standby == 0)
		{
			NetID = 3;
			connectionId = Band - 16;
		}
		else
		{
			NetID = 4;
			connectionId = Band - 16;
		}
	}
	else if (Band >= 26 && Band <= 45)
	{
		if (Standby == 0)
		{
			NetID = 5;
			connectionId = Band - 26;
		}
		else
		{
			NetID = 6;
			connectionId = Band - 26;
		}
	}
	else if (Band >= 46 && Band <= 65)
	{
		if (Standby == 0)
		{
			NetID = 7;
			connectionId = Band - 46;
		}
		else
		{
			NetID = 8;
			connectionId = Band - 46;
		}
	}

	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_SetDataSendSource(NetID, connectionId, Enabled)) < 0)//打开
		return CODE_ERROR_NetError;

	return CODE_OK;
}

int  CDevCtrl::SetAllEnable(int Enable)
{
	for (int i = 8; i > 0; i--)
	{
		if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_SetDataAllEnable(i, Enable)) < 0)//打开
			return CODE_ERROR_NetError;
		Sleep(0);
	}

	return CODE_OK;
}

#pragma endregion

#pragma region 温度信息
int CDevCtrl::RegisterTemperature(Func func)
{
	m_Socket->RegisterTemperatureCallbackFunc(func);
	return CODE_OK;
}
#pragma endregion

#pragma region 15 心跳包
int CDevCtrl::HeartBeat(Func2 HeartBeatCallBack)
{
	m_Socket->RegisterHeartBeatCallbackFunc(HeartBeatCallBack);
	return CODE_OK;
}
#pragma endregion

#pragma region 12 AD过载报警
int CDevCtrl::ADOverload(Func1 AdCallBack)
{
	m_Socket->RegisterADOverloadAlarmCallbackFunc(AdCallBack);
	return CODE_OK;
}
#pragma endregion

#pragma region 万兆网数据传输备份
int CDevCtrl::SetDataDestIP_B(int BoardID, int NetID, int connectionId, char *ip, unsigned short  disPort, unsigned short sourcePort)
{
	if (NetID < 1 || NetID > 8)
		return CODE_ERROR_ConfigError;
	if (connectionId < 0 || connectionId > 255)
		return CODE_ERROR_ConfigError;
	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_SetDataDestIP_B(BoardID, NetID, connectionId, ip, disPort, sourcePort)) < 0)
		return CODE_ERROR_NetError;

	return CODE_OK;
}
int CDevCtrl::SetDataSourceIP_B(int BoardID, int NetID, char *ip, int PacketLen, int Interval, int SlicesClock)
{
	if (NetID < 1 || NetID > 8)
		return CODE_ERROR_ConfigError;

	char *mac = NULL;
	switch (NetID)
	{
	case 1:
		mac = MAC_1;
		break;
	case 2:
		mac = MAC_2;
		break;
	case 3:
		mac = MAC_3;
		break;
	case 4:
		mac = MAC_4;
		break;
	case 5:
		mac = MAC_5;
		break;
	case 6:
		mac = MAC_6;
		break;
	case 7:
		mac = MAC_7;
		break;
	case 8:
		mac = MAC_8;
		break;
	default:
		break;
	}

	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_SetDataSourceIP_B(BoardID, NetID, ip, mac, PacketLen, Interval, SlicesClock)) < 0)
		return CODE_ERROR_NetError;

	return CODE_OK;
}

//wdx添加
int CDevCtrl::SetLinkEnable_B(int BoardID, int NetID, int linkNum, int en)
{
	if (NetID < 1 || NetID > 8)
		return CODE_ERROR_ConfigError;
	if (linkNum < 0 || linkNum > 255)
		return CODE_ERROR_ConfigError;
	if ((en < 0) || (en > 1))
		return CODE_ERROR_ConfigError;
	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_SetDataSendSource_B(BoardID, NetID, linkNum, en)) < 0)//打开
		return CODE_ERROR_NetError;

	return CODE_OK;
}
int CDevCtrl::SetDataReset_X_B(int boardNum, int enable, int dataSource)
{
	if (m_Socket->Send2DevCmd_FC_X(m_DevCtrCmd->MakeCmd_DataReset_B(boardNum, enable, dataSource)) < 0)//打开
		return CODE_ERROR_NetError;
	return CODE_OK;
}
#pragma endregion

#pragma region 射频控制


int CDevCtrl::SetRFFreqBand(int RF_DevID, int DevID, int Channel, float StartFreq, float EndFreq)
{
	if (DevID < 0 || DevID > 5)
		return CODE_ERROR_ConfigError;
	if (Channel < 0 || Channel > 19)
		return CODE_ERROR_ConfigError;
	int StartFreq_INT = static_cast<int>(StartFreq * 100);
	int EndFreq_INT = static_cast<int>(EndFreq * 100);
	int Reslut = -10;
	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_RFFreqBand(RF_DevID, DevID, Channel, StartFreq_INT, EndFreq_INT)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_RFFreqBand_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);
	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}
int CDevCtrl::SetRFFreqBand8(int RF_DevID, int DevID, int Channel, float StartFreq, float EndFreq)
{
	if (DevID < 0 || DevID > 5)
		return CODE_ERROR_ConfigError;
	if (Channel < 0 || Channel > 19)
		return CODE_ERROR_ConfigError;
	int StartFreq_INT = static_cast<int>(StartFreq * 100);
	int EndFreq_INT = static_cast<int>(EndFreq * 100);
	int Reslut = -10;
	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_RFFreqBand8(RF_DevID, DevID, Channel, StartFreq_INT, EndFreq_INT)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_RFFreqBand_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);
	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;


	return Reslut;
}

int CDevCtrl::SetCSFreqBand(int RF_DevID, int DevID, float StartFreq, float EndFreq)
{
	if (DevID < 0 || DevID > 5)
		return CODE_ERROR_ConfigError;
	int StartFreq_INT = static_cast<int>(StartFreq * 100);
	int EndFreq_INT = static_cast<int>(EndFreq * 100);
	int Reslut = -10;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_CSFreqBand(RF_DevID, DevID, StartFreq_INT, EndFreq_INT)) < 0)
		return CODE_ERROR_NetError;
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_CSFreqBand_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

int CDevCtrl::SetRFAllGainValue(int RF_DevID, int DevID, int Value)
{
	if (Value < 0 || Value > 31)
		return CODE_ERROR_ConfigError;
	int Reslut = -10;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_RFAllGainValue(RF_DevID, DevID, Value)) < 0)
		return CODE_ERROR_NetError;
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_RFAllGainValue_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

int CDevCtrl::SetRFAllWorkMode(int RF_DevID, int DevID, int Mode)
{
	if (Mode < 0 || Mode > 1)
		return CODE_ERROR_ConfigError;

	int Reslut = -10;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_RFAllWokeMode(RF_DevID, DevID, Mode)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_RFAllWokeMode_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

int CDevCtrl::SetRFAllParamConfig(int RF_DevID, int DevID, int Channel, int GainValue, int WorkMode)
{
	if (DevID < 1 || DevID > 5)
		return CODE_ERROR_ConfigError;
	if (Channel < 0 || Channel > 19)
		return CODE_ERROR_ConfigError;
	if (GainValue < 0 || GainValue > 31)
		return CODE_ERROR_ConfigError;
	if (WorkMode < 0 || WorkMode > 1)
		return CODE_ERROR_ConfigError;

	std::shared_ptr<RF_PARAM> Param(new RF_PARAM());

	Param->GainVlaue = GainValue;
	Param->WorkMode = WorkMode;
	Param->FreqBand = DevID - 1;
	Param->PowerSwitch = 1;

	int Reslut = -10;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_RFAllParamConfig(RF_DevID, DevID, Channel, Param)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_RFAllParamConfig_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}
int CDevCtrl::SetRFAllParamConfig8(int RF_DevID, int DevID, int Channel, int GainValue, int WorkMode)
{
	if (DevID < 1 || DevID > 5)
		return CODE_ERROR_ConfigError;
	if (Channel < 0 || Channel > 19)
		return CODE_ERROR_ConfigError;
	if (GainValue < 0 || GainValue > 31)
		return CODE_ERROR_ConfigError;
	if (WorkMode < 0 || WorkMode > 1)
		return CODE_ERROR_ConfigError;

	std::shared_ptr<RF_PARAM> Param(new RF_PARAM());


	Param->GainVlaue = GainValue;
	Param->WorkMode = WorkMode;
	Param->FreqBand = DevID - 1;
	Param->PowerSwitch = 1;

	int Reslut = -10;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_RFAllParamConfig8(RF_DevID, DevID, Channel, Param)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_RFAllParamConfig_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

int CDevCtrl::SetCSWorkMode(int RF_DevID, int DevID, int Value)
{
	if (Value < 0 || Value > 1)
		return CODE_ERROR_ConfigError;
	int Reslut = -10;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_CSWorkMode(RF_DevID, DevID, Value)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_CSWorkMode_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

int CDevCtrl::SetCSGainValue(int RF_DevID, int DevID, int Value)
{
	if (Value < 0 || Value > 62)
		return CODE_ERROR_ConfigError;

	int Reslut = -10;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_CSGainValue(RF_DevID, DevID, Value)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_CSGainValue_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

int CDevCtrl::SetCSAllParamConfig(int RF_DevID, int DevID, int GainValue, int WorkMode)
{
	if (DevID < 1 || DevID > 5)
		return CODE_ERROR_ConfigError;
	if (GainValue < 0 || GainValue > 62)
		return CODE_ERROR_ConfigError;
	if (WorkMode < 0 || WorkMode > 1)
		return CODE_ERROR_ConfigError;

	std::shared_ptr<CS_PARAM> Param(new CS_PARAM());

	Param->GainVlaue = GainValue;
	Param->WorkMode = WorkMode;
	Param->FreqBand = DevID - 1;
	Param->PowerSwitch = 1;

	int Reslut = -10;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_CSAllParamConfig(RF_DevID, DevID, Param)) < 0)
		return CODE_ERROR_NetError;
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_CSAllParamConfig_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);


	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

int CDevCtrl::SetGSWorkMode(int RF_DevID, int DevID, int Value)
{
	if (Value < 0 || Value > 1)
		return CODE_ERROR_ConfigError;
	int Reslut = -10;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_GSWorkMode(RF_DevID, DevID, Value)) < 0)
		return CODE_ERROR_NetError;
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_GSWorkMode_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

int CDevCtrl::SetRFPowerSwitch(int RF_DevID, int DevID, int Channel, int PowerSwitch)
{
	if (PowerSwitch < 0 || PowerSwitch > 1)
		return CODE_ERROR_ConfigError;
	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_RFPowerSwitch(RF_DevID, DevID, Channel, PowerSwitch)) < 0)
		return CODE_ERROR_NetError;

	int Reslut = -10;

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_RFPowerSwitch_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}
int CDevCtrl::SetRFPowerSwitch8(int RF_DevID, int DevID, int Channel, int PowerSwitch)
{
	if (PowerSwitch < 0 || PowerSwitch > 1)
		return CODE_ERROR_ConfigError;
	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_RFPowerSwitch8(RF_DevID, DevID, Channel, PowerSwitch)) < 0)
		return CODE_ERROR_NetError;

	int Reslut = -10;

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_RFPowerSwitch_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

int CDevCtrl::SetCSPowerSwitch(int RF_DevID, int DevID, int PowerSwitch)
{
	if (PowerSwitch < 0 || PowerSwitch > 1)
		return CODE_ERROR_ConfigError;
	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_CSPowerSwitch(RF_DevID, DevID, PowerSwitch)) < 0)
		return CODE_ERROR_NetError;

	int Reslut = -10;

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_CSPowerSwitch_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

int CDevCtrl::SetGSPowerSwitch(int RF_DevID, int DevID, int PowerSwitch)
{
	if (PowerSwitch < 0 || PowerSwitch > 1)
		return CODE_ERROR_ConfigError;
	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_GSPowerSwitch(RF_DevID, DevID, PowerSwitch)) < 0)
		return CODE_ERROR_NetError;
	int Reslut = -10;

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_GSPowerSwitch_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

int CDevCtrl::SetWSWholeStatus(int RF_DevID, int DevID, int GainValue, int RF_WorkMode, int CS_WorkMode, int GS_WorkMode)
{
	if (DevID < 1 || DevID > 5)
		return CODE_ERROR_ConfigError;
	if (GainValue < 0 || GainValue > 31)
		return CODE_ERROR_ConfigError;
	if (RF_WorkMode < 0 || RF_WorkMode > 1)
		return CODE_ERROR_ConfigError;
	if (CS_WorkMode < 0 || CS_WorkMode > 1)
		return CODE_ERROR_ConfigError;
	if (GS_WorkMode < 0 || GS_WorkMode > 1)
		return CODE_ERROR_ConfigError;

	std::shared_ptr<DEV_PARAM> Param(new DEV_PARAM());

	Param->GainValue = GainValue;
	Param->FreqBand = RF_DevID - 1;
	Param->RF_WorkMode = RF_WorkMode;
	Param->CS_WorkMode = CS_WorkMode;
	Param->GS_WorkMode = GS_WorkMode;
	Param->RF_PowerSwitch = 1;
	Param->CS_PowerSwitch = 1;
	Param->GS_PowerSwitch = 1;

	int Reslut = -10;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_WSWholeStatus(RF_DevID, DevID, Param)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_WSWholeStatus_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

int CDevCtrl::SetWSWholeStatusZC(int RF_DevID, int DevID, int GainValue, int RF_WorkMode, int CS_WorkMode, int GS_WorkMode)
{
	if (DevID < 1 || DevID > 5)
		return CODE_ERROR_ConfigError;
	if (GainValue < 0 || GainValue > 31)
		return CODE_ERROR_ConfigError;
	if (RF_WorkMode < 0 || RF_WorkMode > 1)
		return CODE_ERROR_ConfigError;
	if (CS_WorkMode < 0 || CS_WorkMode > 1)
		return CODE_ERROR_ConfigError;
	if (GS_WorkMode < 0 || GS_WorkMode > 1)
		return CODE_ERROR_ConfigError;

	std::shared_ptr<DEV_PARAM> Param(new DEV_PARAM());

	Param->GainValue = GainValue;
	Param->FreqBand = RF_DevID - 1;
	Param->RF_WorkMode = RF_WorkMode;
	Param->CS_WorkMode = CS_WorkMode;
	Param->GS_WorkMode = GS_WorkMode;
	Param->RF_PowerSwitch = 1;
	Param->CS_PowerSwitch = 1;
	Param->GS_PowerSwitch = 1;

	int Reslut = -10;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_WSWholeStatusZC(RF_DevID, DevID, Param)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_WSWholeStatus_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

int CDevCtrl::SetWSWholeStatusZC18(int RF_DevID, int DevID, int GainValue, int RF_WorkMode, int CS_WorkMode, int GS_WorkMode)
{
	if (DevID < 1 || DevID > 5)
		return CODE_ERROR_ConfigError;
	if (GainValue < 0 || GainValue > 31)
		return CODE_ERROR_ConfigError;
	if (RF_WorkMode < 0 || RF_WorkMode > 1)
		return CODE_ERROR_ConfigError;
	if (CS_WorkMode < 0 || CS_WorkMode > 1)
		return CODE_ERROR_ConfigError;
	if (GS_WorkMode < 0 || GS_WorkMode > 1)
		return CODE_ERROR_ConfigError;

	std::shared_ptr<DEV_PARAM> Param(new DEV_PARAM());

	Param->GainValue = GainValue;
	Param->FreqBand = RF_DevID - 1;
	Param->RF_WorkMode = RF_WorkMode;
	Param->CS_WorkMode = CS_WorkMode;
	Param->GS_WorkMode = GS_WorkMode;
	Param->RF_PowerSwitch = 1;
	Param->CS_PowerSwitch = 1;
	Param->GS_PowerSwitch = 1;

	int Reslut = -10;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_WSWholeStatusZC18(RF_DevID, DevID, Param)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_WSWholeStatus_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

int CDevCtrl::InquireRFDevIdentity(int RF_DevID, int DevID, int Channel, PDEV_RF_IDENTITY_RESULT Reslut)
{
	if (DevID < 1 || DevID > 5)
		return CODE_ERROR_ConfigError;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_RFGetDevIdentity(RF_DevID, DevID, Channel)) < 0)
		return CODE_ERROR_NetError;
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(2000));
	}

	int ret = m_DevCtrCmd->MakeCmd_RFGetDevIdentity_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}
int CDevCtrl::InquireRFDevIdentity8(int RF_DevID, int DevID, int Channel, PDEV_RF_IDENTITY_RESULT Reslut)
{
	if (DevID < 1 || DevID > 5)
		return CODE_ERROR_ConfigError;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_RFGetDevIdentity8(RF_DevID, DevID, Channel)) < 0)
		return CODE_ERROR_NetError;
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(2000));
	}

	int ret = m_DevCtrCmd->MakeCmd_RFGetDevIdentity_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}

int CDevCtrl::InquireCSDevIdentity(int RF_DevID, int DevID, PDEV_CS_IDENTITY_RESULT Reslut)
{
	if (DevID < 1 || DevID > 5)
		return CODE_ERROR_ConfigError;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_CSGetDevIdentity_Z(RF_DevID, DevID)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(2000));
	}

	int ret = m_DevCtrCmd->MakeCmd_CSGetDevIdentity_Ret_Analysis_Z(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}

int CDevCtrl::InquireGSDevIdentity(int RF_DevID, int DevID, PDEV_GS_IDENTITY_RESULT Reslut)
{
	if (DevID < 0 || DevID > 5)
		return CODE_ERROR_ConfigError;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_GSGetDevIdentity_Z(RF_DevID, DevID)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(2000));
	}

	int ret = m_DevCtrCmd->MakeCmd_GSGetDevIdentity_Ret_Analysis_Z(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}
#pragma endregion

#pragma region 射频协议V1.6.2

int CDevCtrl::InquireCSDevState_Z(int RF_DevID, int DevID, PDEV_CS_STATUS_RESULT Reslut)
{
	m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_CSGetDevState_Z(RF_DevID, DevID));

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}
	int ret = m_DevCtrCmd->MakeCmd_CSGetDevState_Ret_Analysis_Z(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}
int CDevCtrl::InquireGSDevState_Z(int RF_DevID, int DevID, PDEV_GS_STATUS_RESULT Reslut)
{
	m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_GSGetDevState_Z(RF_DevID, DevID));

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}
	int ret = m_DevCtrCmd->MakeCmd_GSGetDevState_Ret_Analysis_Z(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}
#pragma endregion

#pragma region JGZC采集板控制200102LX修改

//01采集板自检 200102LX
int CDevCtrl::InquireDevCC_FPGACheckSelf(PDEV_FPGA_CHECKSELF_RESULT Reslut)
{
	m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_DevCC_FPGACheckSelf());
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_DevCC_FPGACheckSelf_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}

//02射频增益模式控制 200102LX
int CDevCtrl::SetRFGainMode(int Mode)
{
	if (Mode < 0 || Mode > 1)
		return CODE_ERROR_ConfigError;
	
	int Reslut = -10;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_RFGainMode(Mode)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_RFGainMode_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

//03射频增益模式查询 200102LX
int CDevCtrl::InquireDevRF_GainMode(PDEV_DevRF_GetGainMode_RESULT Reslut)
{

	m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_DevRF_GetGainMode());
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_DevRF_GetGainMode_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}

//04射频增益值控制指令 200102LX
int CDevCtrl::SetRFGainValue(int Value)
{
	if (Value < 0 || Value > 31)
		return CODE_ERROR_ConfigError;
	int Reslut = -10;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_RFGainValue(Value)) < 0)
		return CODE_ERROR_NetError;
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_RFGainValue_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

//05射频工作模式指令 200102LX
int CDevCtrl::SetRFWorkMode(int Mode)
{
	if (Mode < 0 || Mode > 1)
		return CODE_ERROR_ConfigError;

	int Reslut = -10;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_RFWokeMode(Mode)) < 0)
		return CODE_ERROR_NetError;
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_RFWokeMode_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

//06射频状态信息回读 200102LX
int CDevCtrl::InquireRFDevState(PDEV_RF_STATUS_RESULT Reslut)
{
	m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_RFGetDevState());
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_RFGetDevState_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}

//07窄带196路中心频点配置，带宽配置 200102LX
int CDevCtrl::SetNBDDCFreqBand(int Index, int BandWidth, double Freq)
{
	if (Index < 0 || Index > 196)
		return CODE_ERROR_ConfigError;
	if (BandWidth < 0 || BandWidth > 13)
		return CODE_ERROR_ConfigError;
	if (Freq < 15 || Freq > 300)
		return CODE_ERROR_ConfigError;
	int Reslut = -10;

	m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_NBDDCFreqBand(Index, BandWidth, Freq));
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_NBDDCFreqBand_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	return CODE_OK;
}

//08宽带60路中心频点配置 200102LX
int CDevCtrl::SetWBDDCFreqBand(int Index, double Freq)
{
	if (Index < 0 || Index > 60)
		return CODE_ERROR_ConfigError;
	if (Freq < 0 || Freq > 300)
		return CODE_ERROR_ConfigError;
	int Reslut = -10;

	m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_WBDDCFreqBand(Index, Freq));
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_WBDDCFreqBand_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	return CODE_OK;
}

//09射频AGC参数控制 200102LX
int CDevCtrl::SetRFGainParameter(int UpLimit, int DownLimit, int HoldTime)
{
	if (UpLimit < 0 || UpLimit > 9)
		return CODE_ERROR_ConfigError;
	if (DownLimit < 0 || DownLimit > 12)
		return CODE_ERROR_ConfigError;
	if (HoldTime < 0 || HoldTime > 10)
		return CODE_ERROR_ConfigError;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_RFGainParameter(UpLimit, DownLimit, HoldTime)) < 0)
		return CODE_ERROR_NetError;

	int Reslut = -10;

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_RFGainParameter_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

//10数字AGC参数控制 200102LX
int CDevCtrl::SetDigitalGainParameter(int UpLimit, int DownLimit, int HoldTime)
{
	if (UpLimit < 0 || UpLimit > 9)
		return CODE_ERROR_ConfigError;
	if (DownLimit < 0 || DownLimit > 12)
		return CODE_ERROR_ConfigError;
	if (HoldTime < 0 || HoldTime > 10)
		return CODE_ERROR_ConfigError;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_DigitalGainParameter(UpLimit, DownLimit, HoldTime)) < 0)
		return CODE_ERROR_NetError;

	int Reslut = -10;

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_DigitalGainParameter_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

//11时标切换200102LX
int CDevCtrl::SetSelectTimeDev(int Mode, PDEV_TIMEDEV_RESULT Reslut)
{
	if (Mode < 0 || Mode > 1)
		return CODE_ERROR_ConfigError;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_SelectTimeDev(Mode)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_SelectTimeDev_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}

//12GPS / BD类型控制指令
int CDevCtrl::SetGPSBD(int Mode, PDEV_GPSBD_RESULT Reslut)
{
	if (Mode < 0 || Mode > 1)
		return CODE_ERROR_ConfigError;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_GPSBD(Mode)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_GPSBD_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}

//13数字增益24dB开关控制
int CDevCtrl::SetDigitalGainSwitch(int WorkSwitch)
{
	if (WorkSwitch < 0 || WorkSwitch > 1)
		return CODE_ERROR_ConfigError;

	int Reslut = -10;
	
	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_DigitalGainSwitch(WorkSwitch)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_DigitalGainSwitch_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

//窄带AGC开关
int CDevCtrl::NBDC_AGC(int WorkSwitch)
{
	if (WorkSwitch < 0 || WorkSwitch > 1)
		return CODE_ERROR_ConfigError;

	int Reslut = -10;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_NBDC_AGC(WorkSwitch)) < 0)
		return CODE_ERROR_NetError;

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_NBDC_AGC_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;

	return Reslut;
}

//14系统时钟状态查询
int CDevCtrl::InquireCLKStatus(PDEV_CLKStatus_RESULT Reslut)
{
	m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_CLKStatus());

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_CLKStatus_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}

//15Aurora接口channel_up连接状态
int CDevCtrl::InquireAuroraStatus(PDEV_CCAURORA_CHECKSELF_RESULT Reslut)
{
	m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_AuroraStatus());

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_AuroraStatus_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}

//16FPGA版本号查询采集板
int CDevCtrl::InquireFPGAVersion(PDEV_GetFPGAVersion_CHECKSELF_RESULT Reslut)
{
	m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_GetFPGAVersion());

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_GetFPGAVersion_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}

//16FPGA版本号查询传输版
int CDevCtrl::InquireFPGAVersionFC(PDEV_GetFPGAVersionFC_CHECKSELF_RESULT Reslut)
{
	m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_GetFPGAVersionFC());

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_GetFPGAVersionFC_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}

//17数字增益模式控制
int CDevCtrl::SetDigitalGainMode(int Mode)
{
	if (Mode < 0 || Mode > 1)
		return CODE_ERROR_ConfigError;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_DigitalGainMode(Mode)) < 0)
		return CODE_ERROR_NetError;

	int Reslut = -10;

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_DigitalGainMode_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;
	return Reslut;
}

//18数字增益模式查询
int CDevCtrl::InquireDigitalGainMode(PDEV_GetDigitalGainMode_RESULT Reslut)
{
	m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_GetDigitalGainMode());

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_GetDigitalGainMode_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;
}

//19数字MGC衰减值控制
int CDevCtrl::SetDigitalGainValue(int Value)
{
	if (Value < 0 || ((Value > 20) && (Value != 31)))
		return CODE_ERROR_ConfigError;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_DigitalGainValue(Value)) < 0)
		return CODE_ERROR_NetError;

	int Reslut = -10;

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_DigitalGainValue_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;
	return Reslut;
}

//21窄带DDC时标精度
int CDevCtrl::SetNBDDCAccuracy(int Accuracy)
{
	if (Accuracy < 0 || ((Accuracy > 1024)))
		return CODE_ERROR_ConfigError;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_NBDDCAccuracy(Accuracy)) < 0)
		return CODE_ERROR_NetError;

	int Reslut = -10;

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_NBDDCAccuracy_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;
	return Reslut;
}

//22FFT时标精度
int CDevCtrl::SetFFTAccuracy(int Accuracy)
{
	if (Accuracy < 0 || ((Accuracy > 1024)))
		return CODE_ERROR_ConfigError;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_FFTAccuracy(Accuracy)) < 0)
		return CODE_ERROR_NetError;

	int Reslut = -10;

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_FFTAccuracy_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;
	return Reslut;
}

//23JGZCAGC增益值查询-8T
int CDevCtrl::InquireDigitalGainValueJGZC(int DevID, PDEV_GetDigitalGainValueJGZC_RESULT Reslut)
{
	if (DevID < 0 || DevID > 5)
		return CODE_ERROR_ConfigError;

	m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_GetDigitalGainValueJGZC(DevID));

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_GetDigitalGainValueJGZC_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);
	
	return CODE_OK;
}


//03远程复位指令
int CDevCtrl::SetResetDev(int DevID)
{
	if (DevID < 0 || DevID > 5)
		return CODE_ERROR_ConfigError;

	if (m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_ResetDev(DevID)) < 0)
		return CODE_ERROR_NetError;

	int Reslut = -10;

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_ResetDev_Ret_Analysis(std::move(m_Socket->GetDataPtr()), &Reslut);

	if (Reslut == 1)
		Reslut = 0;
	else if (Reslut == 0)
		Reslut = -2;
	return Reslut;

}

//08射频通信状态查询
int CDevCtrl::InquireRFCommunityStatus(int DevID, PDEV_CHECKSELF_RESULT Reslut)
{
	if (DevID < 0 || DevID > 5)
		return CODE_ERROR_ConfigError;

	m_Socket->Send2DevCmd_CC(m_DevCtrCmd->MakeCmd_RFCommunityStatus(DevID));

	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = m_DevCtrCmd->MakeCmd_RFCommunityStatus_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

	return CODE_OK;

}

//13AGC增益值查询
int CDevCtrl::InquireDigitalGainValue(int DevID, PDEV_GetDigitalGainValue_RESULT Reslut)
{
	if (DevID < 0 || DevID > 5)
		return CODE_ERROR_ConfigError;

	m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_GetDigitalGainValue(DevID));

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = 0;
	if (DevID > 3)
	{
			 ret = m_DevCtrCmd->MakeCmd_GetDigitalGainValue_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);
			 std::unique_lock<std::mutex> lock(m_mutex);
			 m_Condition.wait_for(lock, std::chrono::microseconds(5000));
			 ret = m_DevCtrCmd->MakeCmd_GetDigitalGainValue_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);
	}
	else
	{
		     ret = m_DevCtrCmd->MakeCmd_GetDigitalGainValue_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);
	}

	return CODE_OK;
}

//13AGC增益值查询-8T
int CDevCtrl::InquireDigitalGainValue8(int DevID, PDEV_GetDigitalGainValue_RESULT Reslut)
{
	if (DevID < 0 || DevID > 5)
		return CODE_ERROR_ConfigError;

	m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_GetDigitalGainValue8(DevID));

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}
	int ret = 0;
	if (DevID > 3)
	{
		ret = m_DevCtrCmd->MakeCmd_GetDigitalGainValue8_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(10000));
		ret = m_DevCtrCmd->MakeCmd_GetDigitalGainValue8_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

		m_Condition.wait_for(lock, std::chrono::microseconds(10000));
		ret = m_DevCtrCmd->MakeCmd_GetDigitalGainValue8_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);
	}
	else
	{
		ret = m_DevCtrCmd->MakeCmd_GetDigitalGainValue8_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);
	}

	return CODE_OK;
}

//13AGC增益值查询-18T
int CDevCtrl::InquireDigitalGainValue18(int DevID, PDEV_GetDigitalGainValue_RESULT Reslut)
{
	if (DevID < 0 || DevID > 5)
		return CODE_ERROR_ConfigError;

	m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_GetDigitalGainValue(DevID));

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}

	int ret = 0;
	if (DevID > 3)
	{
		ret = m_DevCtrCmd->MakeCmd_GetDigitalGainValue18_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
		ret = m_DevCtrCmd->MakeCmd_GetDigitalGainValue18_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
		ret = m_DevCtrCmd->MakeCmd_GetDigitalGainValue18_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);

	}
	else
	{
		ret = m_DevCtrCmd->MakeCmd_GetDigitalGainValue18_Ret_Analysis(std::move(m_Socket->GetDataPtr()), Reslut);
	}

	return CODE_OK;
}

//14万兆光口查询指令
int CDevCtrl::InquireWebStatus(int NetDevID, int NetNum, int TableNum, PDEV_CHECKSELF_RESULT Reslut)
{
	m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_GetWebStatus(NetDevID, NetNum, TableNum));
	return CODE_OK;
}

#pragma endregion

#pragma region 190814新增测试用组播ip设置指令
int CDevCtrl::SetTestip()
{
	m_Socket->Send2DevCmd_FC(m_DevCtrCmd->MakeCmd_SetTestip());
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_Condition.wait_for(lock, std::chrono::microseconds(5000));
	}
	return CODE_OK;
}

#pragma endregion
