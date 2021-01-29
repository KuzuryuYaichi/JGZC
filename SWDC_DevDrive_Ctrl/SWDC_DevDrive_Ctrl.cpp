// SWDC_DevDrive_Ctrl.cpp : ���� DLL Ӧ�ó���ĵ���������
//
#include "stdafx.h"
#define TOEC_DLL_EXPORT
#include "SWDC_DevDriver_Ctrl.h"
#include "DevCtrl.h"
#include <io.h>
#include <fcntl.h>

CDevCtrl* g_DevHandle = nullptr;

#pragma region ��ʼ��
//��ʼ���豸
int InitialzeDevice(const char* DevIP, const unsigned short DevCCPort, const unsigned DevFCPort ,const char * LocalIP)
{
	g_DevHandle = CDevCtrl::CreateDevCtrlInstance(const_cast<char *>(DevIP), DevCCPort, DevFCPort, const_cast<char *>(LocalIP));
	if (g_DevHandle == nullptr)
		return -1;
	return 0;
}
//�ͷ��豸����Դ
int FreeDevice()
{
	if (g_DevHandle == nullptr)
		return -1;
	g_DevHandle->FreeDevCtrlInstance();
	g_DevHandle = nullptr;
	return 0;
}
#pragma endregion

#pragma region ǧ������������
//�޸ı���IP
int LocalNetConfig_IP(const char *IP)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetLocalIP(const_cast<char *>(IP));
	return ret;
}

//�޸��豸IP
int DevNetConfig_IP(const char *IP)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDevIP(const_cast<char *>(IP));
	return ret;
}

//�޸��豸�ɼ���ͨ�Ŷ˿ں�
int DevNetConfig_CCPort(const unsigned short CCPort)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDevCCPort(CCPort);
	return ret;
}

//�޸��豸ת����ͨ�Ŷ˿ں�
int DevNetConfig_FCPort(const unsigned short FCPort)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDevFCPort(FCPort);
	return ret;
}
#pragma endregion

#pragma region ���������ݴ�������
int DataDestIP(int NetID, int connectionId, const char *ip, unsigned short disPort, unsigned short sourcePort)
	//NetID[1, 4]	��������ں�
	//connectionId[0, 3]	����������ͨ·���
	//ip	~������Ŀ�Ķ�IP��ַ
	//port	~������Ŀ�Ķ˶˿ں�
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDataDestIP(NetID, connectionId, const_cast<char*>(ip), disPort, sourcePort);
	return ret;
}

int DataLinkEnable(int NetID, int linkNum, int en)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetLinkEnable(NetID, linkNum, en);
	return ret;
}

int DataReset(int dataSource)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDataReset(dataSource);
	return ret;
}

int DataReset_X(int boardNum, int enable, int dataSource)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDataReset_X(boardNum, enable,dataSource);
	return ret;
}

int EnableInit(int Enable)
{
	int count = 0;
	int res = 0;
	int ret = 0;
	for (int i = 0; i < 8; i++)
	{
		switch (i)
		{
		case 0:
		case 1:
			count = 16;
			break;
		case 2:
		case 3:
			count = 10;
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			count = 20;
			break;
		}

		for (int j = 0; j < count; j++)
		{
			res = DataLinkEnable(i, j, Enable);
			if (res != 0)
			{
				ret = res;
			}
		}
	}
	return ret;
}

int DataSourceIP(int NetID, const char *ip, int PacketLen, int Interval, int SlicesClock)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDataSourceIP(NetID, const_cast<char*>(ip), PacketLen, Interval, SlicesClock);
	g_DevHandle->SetDataReset(1);
	return ret;
}

int DataInformation(int Band, int Standby, char * DisIP, unsigned short  DisPort, unsigned short SourcePort, int Enabled)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDataInformation(Band,  Standby, DisIP, DisPort, SourcePort, Enabled);

	return ret;
}

int DataInformation_Multi(int Band, int multi, int Standby, char * DisIP, unsigned short  DisPort, unsigned short SourcePort, int Enabled)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDataInformation_Multi(Band, multi, Standby, DisIP, DisPort, SourcePort, Enabled);

	return ret;
}

int DataEnable(int Band, int Standby, int Enabled)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDataEnable(Band,Standby, Enabled);

	return ret;
}

//4.	����������ʹ���ܿ���LX
int DataAllEnable(int Enabled)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetAllEnable(Enabled);

	return ret;
}

#pragma endregion

#pragma region FPGA��������

int DevCC_FPGALoad(int DevID, char * FilePath)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->LoadCC_FPGABinFile(DevID, FilePath);
	return ret;
}

int DevRC_FPGALoad(int FlashID, char * FilePath)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->LoadRC_FPGABinFile(FlashID, FilePath);

	return ret;
}
#pragma endregion

#pragma region ���������ݴ��䱸��
//1.	����������Դ�˿�����
int DataSourceIP_B(int BoardID, int NetID, const char *ip, int PacketLen, int Interval, int SlicesClock)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDataSourceIP_B(BoardID, NetID, const_cast<char*>(ip), PacketLen, Interval,  SlicesClock);
	return ret;
}
//5.	���������ݸ�λ
int DataReset_X_B(int boardNum, int enable, int dataSource)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDataReset_X_B(boardNum, enable, dataSource);
	return ret;
}
//2.	����������Ŀ��IP����
int DataDestIP_B(int BoardID, int NetID, int connectionId, const char *ip, unsigned short disPort, unsigned short sourcePort)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDataDestIP_B( BoardID, NetID, connectionId, const_cast<char*>(ip), disPort, sourcePort);
	return ret;
}
//3.	������ʹ�ܵ�������
int DataLinkEnable_B(int BoardID, int NetID, int linkNum, int en)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetLinkEnable_B( BoardID, NetID, linkNum, en);
	return ret;
}
#pragma endregion

#pragma region ��ƵЭ��V1.7


#pragma region �汾����
int Version = 2;
int Type = 0;
int ID = 0;

//Version����ƵЭ��汾 0������վ 2������ 3:713�豸
int CtrlVersion(int m_Ver)
{
	Version = m_Ver;
	g_DevHandle->m_Socket->HeartVersion = Version;
	return 1;
}

//��Ƶ���ӷ�ʽ
int CtrlType(int m_Type)
{
	Type = m_Type;
	return 1;
}

//ID���豸���� 0��20T  2:8T 3:18T
int DevDriveID(int m_ID)
{
	ID = m_ID;
	g_DevHandle->m_Socket->HeartDevTypeID = ID;
	return 1;
}

#pragma endregion

//190807�޸�8T��Ƶ����ָ��İ忨��Ӧ��ʽ
#pragma region ��Ƶ����new
int RFFreqBand(int RF_DevID, int Channel, float StartFreq, float EndFreq)
{
	if (g_DevHandle == nullptr)
		return -1;

	int DevID = 0;
	if (Type == 0)//���ӷ�ʽ��Ĭ��ֱ����
		DevID = RF_DevID;
	else
		DevID = Type;

	int ret = 0;
	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		switch (RF_DevID)
		{
		case 1:
			DevID = 1;
			break;
		case 2:
			DevID = 1;
			break;
		case 3:
			DevID = 2;
			break;
		case 4:
			DevID = 2;
			break;
		default:
			break;
		}
		ret = g_DevHandle->SetRFFreqBand8(RF_DevID, DevID, Channel, StartFreq, EndFreq);
		break;
	default:
		ret = g_DevHandle->SetRFFreqBand(RF_DevID, DevID, Channel, StartFreq, EndFreq);
		break;
	}
	return ret;
}

int CSFreqBand(int RF_DevID, float StartFreq, float EndFreq)
{
	if (g_DevHandle == nullptr)
		return -1;
	int DevID = 0;

	//���ӷ�ʽ��Ĭ��ֱ����
	if (Type == 0)
		DevID = RF_DevID;
	else
		DevID = Type;

	//int ret = g_DevHandle->SetCSFreqBand(RF_DevID, DevID, StartFreq, EndFreq);
	//return ret;

	int ret = 0;
	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		switch (RF_DevID)
		{
		case 1:
			DevID = 1;
			break;
		case 2:
			DevID = 1;
			break;
		case 3:
			DevID = 2;
			break;
		case 4:
			DevID = 2;
			break;
		default:
			break;
		}
		ret = g_DevHandle->SetCSFreqBand(RF_DevID, DevID, StartFreq, EndFreq);
		break;
	default:
		ret = g_DevHandle->SetCSFreqBand(RF_DevID, DevID, StartFreq, EndFreq);

		break;
	}
	return ret;
}

int RFAllGainValue(int RF_DevID, int Value)
{
	if (g_DevHandle == nullptr)
		return -1;
	int DevID = 0;

	//���ӷ�ʽ��Ĭ��ֱ����
	if (Type == 0)
		DevID = RF_DevID;
	else
		DevID = Type;

	//ret = g_DevHandle->SetRFAllGainValue(RF_DevID, DevID, Value);

	int ret = 0;
	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		switch (RF_DevID)
		{
		case 1:
			DevID = 1;
			break;
		case 2:
			DevID = 1;
			break;
		case 3:
			DevID = 2;
			break;
		case 4:
			DevID = 2;
			break;
		default:
			break;
		}
		ret = g_DevHandle->SetRFAllGainValue(RF_DevID, DevID, Value);
		break;
	default:
		ret = g_DevHandle->SetRFAllGainValue(RF_DevID, DevID, Value);

		break;
	}

	return ret;
}

int RFAllWokeMode(int RF_DevID, int Mode)
{
	if (g_DevHandle == nullptr)
		return -1;
	int DevID = 0;

	//���ӷ�ʽ��Ĭ��ֱ����
	if (Type == 0)
		DevID = RF_DevID;
	else
		DevID = Type;

	//ret = g_DevHandle->SetRFAllWorkMode(RF_DevID, DevID, Mode);

	int ret = 0;
	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		switch (RF_DevID)
		{
		case 1:
			DevID = 1;
			break;
		case 2:
			DevID = 1;
			break;
		case 3:
			DevID = 2;
			break;
		case 4:
			DevID = 2;
			break;
		default:
			break;
		}
		ret = g_DevHandle->SetRFAllWorkMode(RF_DevID, DevID, Mode);
		break;
	default:
		ret = g_DevHandle->SetRFAllWorkMode(RF_DevID, DevID, Mode);

		break;
	}

	return ret;
}

int RFPowerSwitch(int RF_DevID, int Channel, int PowerSwitch)
{
	if (g_DevHandle == nullptr)
		return -1;
	int DevID = 0;

	//���ӷ�ʽ��Ĭ��ֱ����
	if (Type == 0)
		DevID = RF_DevID;
	else
		DevID = Type;

	//ret = g_DevHandle->SetRFPowerSwitch(RF_DevID, DevID, Channel, PowerSwitch);

	int ret = 0;
	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		switch (RF_DevID)
		{
		case 1:
			DevID = 1;
			break;
		case 2:
			DevID = 1;
			break;
		case 3:
			DevID = 2;
			break;
		case 4:
			DevID = 2;
			break;
		default:
			break;
		}
		ret = g_DevHandle->SetRFPowerSwitch8(RF_DevID, DevID, Channel, PowerSwitch);
		break;
	default:
		ret = g_DevHandle->SetRFPowerSwitch(RF_DevID, DevID, Channel, PowerSwitch);

		break;
	}
	return ret;
}

int RFParamConfig(int RF_DevID, int Channel, int GainValue, int WorkMode)
{
	if (g_DevHandle == nullptr)
		return -1;
	int DevID = 0;

	//���ӷ�ʽ��Ĭ��ֱ����
	if (Type == 0)
		DevID = RF_DevID;
	else
		DevID = Type;

	//ret = g_DevHandle->SetRFAllParamConfig(RF_DevID, DevID, Channel, GainValue, WorkMode);

	int ret = 0;
	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		switch (RF_DevID)
		{
		case 1:
			DevID = 1;
			break;
		case 2:
			DevID = 1;
			break;
		case 3:
			DevID = 2;
			break;
		case 4:
			DevID = 2;
			break;
		default:
			break;
		}
		ret = g_DevHandle->SetRFAllParamConfig8(RF_DevID, DevID, Channel, GainValue, WorkMode);
		break;
	default:
		ret = g_DevHandle->SetRFAllParamConfig(RF_DevID, DevID, Channel, GainValue, WorkMode);
		break;
	}

	return ret;
}

int CSGainValue(int RF_DevID, int Value)
{
	if (g_DevHandle == nullptr)
		return -1;
	int DevID = 0;

	//���ӷ�ʽ��Ĭ��ֱ����
	if (Type == 0)
		DevID = RF_DevID;
	else
		DevID = Type;

	int ret = 0;
	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		switch (RF_DevID)
		{
		case 1:
			DevID = 1;
			break;
		case 2:
			DevID = 1;
			break;
		case 3:
			DevID = 2;
			break;
		case 4:
			DevID = 2;
			break;
		default:
			break;
		}
		ret = g_DevHandle->SetCSGainValue(RF_DevID, DevID, Value);
		break;
	default:
		ret = g_DevHandle->SetCSGainValue(RF_DevID, DevID, Value);

		break;
	}

	return ret;
}

int CSWorkMode(int RF_DevID, int Mode)
{
	if (g_DevHandle == nullptr)
		return -1;
	int DevID = 0;

	//���ӷ�ʽ��Ĭ��ֱ����
	if (Type == 0)
		DevID = RF_DevID;
	else
		DevID = Type;

	//ret = g_DevHandle->SetCSWorkMode(RF_DevID, DevID, Mode);

	int ret = 0;
	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		switch (RF_DevID)
		{
		case 1:
			DevID = 1;
			break;
		case 2:
			DevID = 1;
			break;
		case 3:
			DevID = 2;
			break;
		case 4:
			DevID = 2;
			break;
		default:
			break;
		}
		ret = g_DevHandle->SetCSWorkMode(RF_DevID, DevID, Mode);
		break;
	default:
		ret = g_DevHandle->SetCSWorkMode(RF_DevID, DevID, Mode);

		break;
	}

	return ret;
}

int CSPowerSwitch(int RF_DevID, int PowerSwitch)
{
	if (g_DevHandle == nullptr)
		return -1;
	int DevID = 0;

	//���ӷ�ʽ��Ĭ��ֱ����
	if (Type == 0)
		DevID = RF_DevID;
	else
		DevID = Type;

	//ret = g_DevHandle->SetCSPowerSwitch(RF_DevID, DevID, PowerSwitch);

	int ret = 0;
	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		switch (RF_DevID)
		{
		case 1:
			DevID = 1;
			break;
		case 2:
			DevID = 1;
			break;
		case 3:
			DevID = 2;
			break;
		case 4:
			DevID = 2;
			break;
		default:
			break;
		}
		ret = g_DevHandle->SetCSPowerSwitch(RF_DevID, DevID, PowerSwitch);
		break;
	default:
		ret = g_DevHandle->SetCSPowerSwitch(RF_DevID, DevID, PowerSwitch);

		break;
	}

	return ret;
}

int CSAllParamConfig(int RF_DevID, int GainVlaue, int WorkMode)
{
	if (g_DevHandle == nullptr)
		return -1;
	int DevID = 0;

	//���ӷ�ʽ��Ĭ��ֱ����
	if (Type == 0)
		DevID = RF_DevID;
	else
		DevID = Type;

	//ret = g_DevHandle->SetCSAllParamConfig(RF_DevID, DevID, GainVlaue, WorkMode);

	int ret = 0;
	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		switch (RF_DevID)
		{
		case 1:
			DevID = 1;
			break;
		case 2:
			DevID = 1;
			break;
		case 3:
			DevID = 2;
			break;
		case 4:
			DevID = 2;
			break;
		default:
			break;
		}
		ret = g_DevHandle->SetCSAllParamConfig(RF_DevID, DevID, GainVlaue, WorkMode);
		break;
	default:
		ret = g_DevHandle->SetCSAllParamConfig(RF_DevID, DevID, GainVlaue, WorkMode);

		break;
	}

	return ret;
}

int GSWorkMode(int RF_DevID, int Mode)
{
	if (g_DevHandle == nullptr)
		return -1;
	int DevID = 0;

	//���ӷ�ʽ��Ĭ��ֱ����
	if (Type == 0)
		DevID = RF_DevID;
	else
		DevID = Type;

	//ret = g_DevHandle->SetGSWorkMode(RF_DevID, DevID, Mode);

	int ret = 0;
	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		switch (RF_DevID)
		{
		case 1:
			DevID = 1;
			break;
		case 2:
			DevID = 1;
			break;
		case 3:
			DevID = 2;
			break;
		case 4:
			DevID = 2;
			break;
		default:
			break;
		}
		ret = g_DevHandle->SetGSWorkMode(RF_DevID, DevID, Mode);
		break;
	default:
		ret = g_DevHandle->SetGSWorkMode(RF_DevID, DevID, Mode);

		break;
	}

	return ret;
}

int GSPowerSwitch(int RF_DevID, int PowerSwitch)
{
	if (g_DevHandle == nullptr)
		return -1;
	int DevID = 0;

	//���ӷ�ʽ��Ĭ��ֱ����
	if (Type == 0)
		DevID = RF_DevID;
	else
		DevID = Type;

	//ret = g_DevHandle->SetGSPowerSwitch(RF_DevID, DevID, PowerSwitch);

	int ret = 0;
	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		switch (RF_DevID)
		{
		case 1:
			DevID = 1;
			break;
		case 2:
			DevID = 1;
			break;
		case 3:
			DevID = 2;
			break;
		case 4:
			DevID = 2;
			break;
		default:
			break;
		}
		ret = g_DevHandle->SetGSPowerSwitch(RF_DevID, DevID, PowerSwitch);
		break;
	default:
		ret = g_DevHandle->SetGSPowerSwitch(RF_DevID, DevID, PowerSwitch);

		break;
	}

	return ret;
}

int WSWholeStatus(int RF_DevID, int GainValue, int RF_WorkMode, int CS_WorkMode, int GS_WorkMode)
{
	if (g_DevHandle == nullptr)
		return -1;
	int DevID = 0;
	int ret = 0;

	if (Type == 0)//��Ƶ���ӷ�ʽ��0Ϊֱ��
		DevID = RF_DevID;
	else
		DevID = Type;

	/*switch (ID)
	{
	case 0:

	//ret = g_DevHandle->SetWSWholeStatus(RF_DevID, DevID, GainValue, RF_WorkMode, CS_WorkMode, GS_WorkMode);

	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
	switch (RF_DevID)
	{
	case 1:
	DevID = 1;
	break;
	case 2:
	DevID = 1;
	break;
	case 3:
	DevID = 2;
	break;
	case 4:
	DevID = 2;
	break;
	default:
	break;
	}
	ret = g_DevHandle->SetWSWholeStatusZC(RF_DevID, DevID, GainValue, RF_WorkMode, CS_WorkMode, GS_WorkMode);
	break;
	default:
	ret = g_DevHandle->SetWSWholeStatus(RF_DevID, DevID, GainValue, RF_WorkMode, CS_WorkMode, GS_WorkMode);

	break;
	}

	break;
	case 2:
	if (Type == 0)
	DevID = RF_DevID;
	else
	DevID = Type;
	//ret = g_DevHandle->SetWSWholeStatusZC(RF_DevID, DevID, GainValue, RF_WorkMode, CS_WorkMode, GS_WorkMode);

	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
	switch (RF_DevID)
	{
	case 1:
	DevID = 1;
	break;
	case 2:
	DevID = 1;
	break;
	case 3:
	DevID = 2;
	break;
	case 4:
	DevID = 2;
	break;
	default:
	break;
	}
	ret = g_DevHandle->SetWSWholeStatus(RF_DevID, DevID, GainValue, RF_WorkMode, CS_WorkMode, GS_WorkMode);
	break;
	default:
	ret = g_DevHandle->SetWSWholeStatus(RF_DevID, DevID, GainValue, RF_WorkMode, CS_WorkMode, GS_WorkMode);

	break;
	}

	break;
	case 3:
	if (Type == 0)
	DevID = RF_DevID;
	else
	DevID = Type;
	//ret = g_DevHandle->SetWSWholeStatusZC(RF_DevID, DevID, GainValue, RF_WorkMode, CS_WorkMode, GS_WorkMode);

	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
	switch (RF_DevID)
	{
	case 1:
	DevID = 1;
	break;
	case 2:
	DevID = 1;
	break;
	case 3:
	DevID = 2;
	break;
	case 4:
	DevID = 2;
	break;
	default:
	break;
	}
	ret = g_DevHandle->SetWSWholeStatus(RF_DevID, DevID, GainValue, RF_WorkMode, CS_WorkMode, GS_WorkMode);
	break;
	default:
	ret = g_DevHandle->SetWSWholeStatus(RF_DevID, DevID, GainValue, RF_WorkMode, CS_WorkMode, GS_WorkMode);

	break;
	}

	break;
	default:
	ret = CODE_ERROR_Version;
	break;
	}*/

	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		switch (RF_DevID)
		{
		case 1:
			DevID = 1;
			break;
		case 2:
			DevID = 1;
			break;
		case 3:
			DevID = 2;
			break;
		case 4:
			DevID = 2;
			break;
		default:
			break;
		}
		ret = g_DevHandle->SetWSWholeStatusZC(RF_DevID, DevID, GainValue, RF_WorkMode, CS_WorkMode, GS_WorkMode);
		break;

	case 3:
		ret = g_DevHandle->SetWSWholeStatusZC18(RF_DevID, DevID, GainValue, RF_WorkMode, CS_WorkMode, GS_WorkMode);
		break;

	default:
		ret = g_DevHandle->SetWSWholeStatus(RF_DevID, DevID, GainValue, RF_WorkMode, CS_WorkMode, GS_WorkMode);
		break;
	}

	return ret;
}

#pragma endregion

//190807�޸�8T��Ƶ��ѯָ��İ忨��Ӧ��ʽ
#pragma region ��Ƶ��ѯnew

int RFGetDevIdentity(int RF_DevID, int Channel, PDEV_RF_IDENTITY_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = 0;
	int DevID = 0;

	if (Type == 0)
		DevID = RF_DevID;
	else
		DevID = Type;

	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		switch (RF_DevID)
		{
		case 1:
			DevID = 1;
			break;
		case 2:
			DevID = 1;
			break;
		case 3:
			DevID = 2;
			break;
		case 4:
			DevID = 2;
			break;
		default:
			break;
		}
		ret = g_DevHandle->InquireRFDevIdentity8(RF_DevID, DevID, Channel, Reslut);
		break;
	default:
		ret = g_DevHandle->InquireRFDevIdentity(RF_DevID, DevID, Channel, Reslut);

		break;
	}

	return ret;
}

int CSGetDevIdentity(int RF_DevID, PDEV_CS_IDENTITY_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = 0;
	int DevID = 0;

	if (Type == 0)
		DevID = RF_DevID;
	else
		DevID = Type;

	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		switch (RF_DevID)
		{
		case 1:
			DevID = 1;
			break;
		case 2:
			DevID = 1;
			break;
		case 3:
			DevID = 2;
			break;
		case 4:
			DevID = 2;
			break;
		default:
			break;
		}
		ret = g_DevHandle->InquireCSDevIdentity(RF_DevID, DevID, Reslut);
		break;
	default:
		ret = g_DevHandle->InquireCSDevIdentity(RF_DevID, DevID, Reslut);

		break;
	}

	return ret;
}

int GSGetDevIdentity(int RF_DevID, PDEV_GS_IDENTITY_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = 0;
	int DevID = 0;

	if (Type == 0)
		DevID = RF_DevID;
	else
		DevID = Type;

	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		switch (RF_DevID)
		{
		case 1:
			DevID = 1;
			break;
		case 2:
			DevID = 1;
			break;
		case 3:
			DevID = 2;
			break;
		case 4:
			DevID = 2;
			break;
		default:
			break;
		}
		ret = g_DevHandle->InquireGSDevIdentity(RF_DevID, DevID, Reslut);
		break;
	default:
		ret = g_DevHandle->InquireGSDevIdentity(RF_DevID, DevID, Reslut);

		break;
	}

	return ret;
}

#pragma endregion

#pragma endregion

#pragma region �豸�Լ�
int DevRC_CheckSelf(PDEV_RC_CHECKSELF_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = 0;
	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		ret = g_DevHandle->InquireDevRC_CheckSelf8(Reslut);
		break;

	default:
		ret = g_DevHandle->InquireDevRC_CheckSelf(Reslut);
		break;
	}
	return ret;
}
#pragma endregion

#pragma region JGZC�ɼ������200102LX�޸�

//01�ɼ����Լ� 200102LX
int DevCC_FPGACheckSelf(PDEV_FPGA_CHECKSELF_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->InquireDevCC_FPGACheckSelf(Reslut);
	return ret;
}

//02��Ƶ����ģʽ���� 200102LX
int RFGainMode(int Mode)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = 0;
	ret = g_DevHandle->SetRFGainMode(Mode);
	return ret;
}

//03��Ƶ����ģʽ��ѯ 200102LX
int GetRFGainMode(PDEV_DevRF_GetGainMode_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = 0;
		ret = g_DevHandle->InquireDevRF_GainMode(Reslut);
	return ret;
}

//04��Ƶ����ֵ����ָ�� 200102LX
int RFGainValue(int Value)
{
	if (g_DevHandle == nullptr)
		return -1;

	int ret = 0;
	ret = g_DevHandle->SetRFGainValue(Value);
	return ret;
}

//05��Ƶ����ģʽָ�� 200102LX
int RFWokeMode(int Mode)
{
	if (g_DevHandle == nullptr)
		return -1;

	int ret = 0;
		ret = g_DevHandle->SetRFWorkMode(Mode);

	return ret;
}

//06��Ƶ״̬��Ϣ�ض� 200102LX
int RFGetDevState(PDEV_RF_STATUS_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;

	int ret = 0;
	ret = g_DevHandle->InquireRFDevState(Reslut);
	return ret;
}

//07խ��196·����Ƶ�����ã��������� 200102LX
int NBDDCFreqBand(int Index, int BandWidth, double Freq)
{
	if (g_DevHandle == nullptr)
		return -1;

	int ret = 0;
	ret = g_DevHandle->SetNBDDCFreqBand(Index, BandWidth, Freq);
	return ret;
}

//08���60·����Ƶ������ 200102LX
int WBDDCFreqBand(int Index, double Freq)
{
	if (g_DevHandle == nullptr)
		return -1;

	int ret = 0;
	ret = g_DevHandle->SetWBDDCFreqBand(Index, Freq);
	return ret;
}

//09��ƵAGC�������� 200102LX
int RFGainParameter(int UpLimit, int DownLimit, int HoldTime)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetRFGainParameter(UpLimit, DownLimit, HoldTime);
	return ret;
}

//10����AGC�������� 200102LX
int DigitalGainParameter(int UpLimit, int DownLimit, int HoldTime)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDigitalGainParameter(UpLimit, DownLimit, HoldTime);
	return ret;
}

//11ʱ���л�200102LX
int SelectTimeDev(int Mode, PDEV_TIMEDEV_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetSelectTimeDev(Mode, Reslut);
	return ret;
}

//12GPS / BD���Ϳ���ָ��200102LX
int SelectGPSBD(int Mode, PDEV_GPSBD_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetGPSBD(Mode, Reslut);
	return ret;
}

//13��������24dB���ؿ���200102LX
int DigitalGainSwitch(int WorkSwitch)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDigitalGainSwitch(WorkSwitch);
	return ret;
}

//խ��AGC
int NBDC_AGC(int WorkSwitch)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->NBDC_AGC(WorkSwitch);
	return ret;
}

//14ϵͳʱ��״̬��ѯ200102LX
int DevCC_SampleClockCheckSelf(PDEV_CLKStatus_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->InquireCLKStatus(Reslut);
	return ret;
}

//15Aurora�ӿ�channel_up����״̬200102LX
int AuroraStatus(PDEV_CCAURORA_CHECKSELF_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->InquireAuroraStatus(Reslut);
	return ret;
}

//16FPGA�汾�Ų�ѯ�ɼ���200102LX
int GetFPGAVersion(PDEV_GetFPGAVersion_CHECKSELF_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;

	int ret = g_DevHandle->InquireFPGAVersion(Reslut);
	return ret;
}

//16FPGA�汾�Ų�ѯ�����200102LX
int GetFPGAVersionFC(PDEV_GetFPGAVersionFC_CHECKSELF_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;

	int ret = g_DevHandle->InquireFPGAVersionFC(Reslut);
	return ret;
}

//17��������ģʽ����200102LX
int DigitalGainMode(int Mode)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDigitalGainMode(Mode);
	return ret;
}

//18��������ģʽ��ѯ200102LX
int GetCCGainMode(PDEV_GetDigitalGainMode_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->InquireDigitalGainMode(Reslut);
	return ret;
}

//19����MGC˥��ֵ����200102LX
int DigitalGainValue(int Value)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetDigitalGainValue(Value);
	return ret;
}

//21խ��DDCʱ�꾫��
int NBDDCAccuracy(int Accuracy)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetNBDDCAccuracy(Accuracy);
	return ret;
}

//22FFTʱ�꾫��
int FFTAccuracy(int Accuracy)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetFFTAccuracy(Accuracy);
	return ret;
}


//123JGZCAGC����ֵ��ѯ
int GetDigitalGainValueJGZC(int DevID, PDEV_GetDigitalGainValueJGZC_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = 0;

		ret = g_DevHandle->InquireDigitalGainValueJGZC(DevID, Reslut);

	return ret;
}

#pragma endregion

//03Զ�̸�λָ��
int ResetDev(int DevID)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->SetResetDev(DevID);
	return ret;
}

//08��Ƶͨ��״̬��ѯ
int CommunityStatus(int DevID, PDEV_CHECKSELF_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->InquireRFCommunityStatus(DevID, Reslut);
	return ret;
}

//12AD���ر���ָʾ�ص�����
void RegisterADOverloadCallback(Func1 AdCallBack)
{
	if (g_DevHandle == nullptr)
		return;
	g_DevHandle->ADOverload(AdCallBack);
}

//13AGC����ֵ��ѯ
int GetDigitalGainValue(int DevID, PDEV_GetDigitalGainValue_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = 0;
	switch (ID)//IDΪ�豸���ͣ�0Ϊ20ͨ����2Ϊ8ͨ����3Ϊ18ͨ��
	{
	case 2:
		ret = g_DevHandle->InquireDigitalGainValue8(DevID, Reslut);
		break;
	case 3:
		ret = g_DevHandle->InquireDigitalGainValue18(DevID, Reslut);
		break;
	default:
		ret = g_DevHandle->InquireDigitalGainValue(DevID, Reslut);
		break;
	}
	return ret;
}

//14���׹�ڲ�ѯָ��
int GetWebStatus(int NetDevID, int NetNum, int TableNum, PDEV_CHECKSELF_RESULT Reslut)
{
	if (g_DevHandle == nullptr)
		return -1;
	int ret = g_DevHandle->InquireWebStatus(NetDevID, NetNum, TableNum, Reslut);
	return ret;
}

//15�������ص�����
void RegisterHeartBeatCallback(Func2 HeartBeatCallBack)
{
	if (g_DevHandle == nullptr)
		return;
	g_DevHandle->HeartBeat(HeartBeatCallBack);
}

//�¶���Ϣ�ص�����
void RegisterTemperatureCallback(Func func)
{
	if (g_DevHandle == nullptr)
		return;
	g_DevHandle->RegisterTemperature(func);
}
