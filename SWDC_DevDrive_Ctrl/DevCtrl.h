#pragma once

#include "MyUDPSocket.h"
#include "MyTCPSocket.h"
#include "DevCtrlCmd.h"
#include <memory>
#include <atomic>

#define MAC_1 "aabbccd1e1f1"
#define MAC_2 "aabbccd2e2f2"
#define MAC_3 "aabbccd3e3f3"
#define MAC_4 "aabbccd4e4f4"
#define MAC_5 "aabbccd5e5f5"
#define MAC_6 "aabbccd6e6f6"
#define MAC_7 "aabbccd7e7f7"
#define MAC_8 "aabbccd8e8f8"

class CDevCtrl
{
private:
	static CDevCtrl *m_Instance; //设备句柄
	std::unique_ptr<CDevCtrlCmd> m_DevCtrCmd;
	std::mutex m_mutex;
	std::condition_variable m_Condition;
	std::atomic<bool> m_ConditionFlag;
private:
	explicit CDevCtrl(char *DstIP, unsigned short CCPort, unsigned short FCPort, char *LocalIP); //私有化构造函数
	int InitNetInfo(char *DstIP, unsigned short CCPort, unsigned short FCPort, char *LocalIP); //初始化网络连接信息
public:
	~CDevCtrl();
	//最先析构;
	std::unique_ptr<CMySocket> m_Socket;
	//创建于释放
	static CDevCtrl * CreateDevCtrlInstance(char * DstIP, unsigned short CCPort, unsigned short FCPort, char *LocalIP); // 初始化-构造
	static void FreeDevCtrlInstance();

#pragma region 修改IP

int SetLocalIP(char *IP);

int SetDevIP(char *IP);

int SetDevCCPort(unsigned short Port);

int SetDevFCPort(unsigned short Port);

#pragma endregion

#pragma region JGZC采集板控制200102LX修改

int LoadCC_FPGABinFile(int DevID, char *FilePath);

int LoadRC_FPGABinFile(int FlashID, char *FilePath);

//01采集板自检 200102LX
int InquireDevCC_FPGACheckSelf(PDEV_FPGA_CHECKSELF_RESULT Reslut);

//02射频增益模式控制 200102LX
int SetRFGainMode(int Mode);

//03射频增益模式查询 200102LX
int InquireDevRF_GainMode(PDEV_DevRF_GetGainMode_RESULT Reslut);

//04射频增益值控制指令 200102LX
int SetRFGainValue(int Value);

//05射频工作模式指令 200102LX
int SetRFWorkMode(int Mode);

//06射频状态信息回读 200102LX
int InquireRFDevState(PDEV_RF_STATUS_RESULT Reslut);

//07窄带196路中心频点配置，带宽配置 200102LX
int SetNBDDCFreqBand(int Index, int BandWidth, double Freq);

//08宽带60路中心频点配置 200102LX
int SetWBDDCFreqBand(int Index, double Freq);

//09射频AGC参数控制
int SetRFGainParameter(int UpLimit, int DownLimit, int HoldTime);

//10数字AGC参数控制
int SetDigitalGainParameter(int UpLimit, int DownLimit, int HoldTime);

//11时标切换
int SetSelectTimeDev(int Mode, PDEV_TIMEDEV_RESULT Reslut);

//12GPS / BD类型控制指令
int SetGPSBD(int Mode, PDEV_GPSBD_RESULT Reslut);

//13数字增益24dB开关控制
int SetDigitalGainSwitch(int WorkSwitch);

//窄带AGC开关
int NBDC_AGC(int WorkSwitch);

//14系统时钟状态查询
int InquireCLKStatus(PDEV_CLKStatus_RESULT Reslut);

//15Aurora接口channel_up连接状态
int CDevCtrl::InquireAuroraStatus(PDEV_CCAURORA_CHECKSELF_RESULT Reslut);

//16FPGA版本号查询采集板
int InquireFPGAVersion(PDEV_GetFPGAVersion_CHECKSELF_RESULT Reslut);

//16FPGA版本号查询传输版
int InquireFPGAVersionFC(PDEV_GetFPGAVersionFC_CHECKSELF_RESULT Reslut);

//17数字增益模式控制
int SetDigitalGainMode(int Mode);

//18数字增益模式查询
int InquireDigitalGainMode(PDEV_GetDigitalGainMode_RESULT Reslut);

//19数字MGC衰减值控制
int SetDigitalGainValue(int Value);

//21窄带DDC时标精度
int SetNBDDCAccuracy(int Accuracy);

//22FFT时标精度
int SetFFTAccuracy(int Accuracy);

//23JGZCAGC增益查询
int InquireDigitalGainValueJGZC(int DevID, PDEV_GetDigitalGainValueJGZC_RESULT Reslut);

//06开机状态查询	
int InquirePowerOnStatus(int DevID, PDEV_CHECKSELF_RESULT Reslut);

//08射频通信状态查询
int InquireRFCommunityStatus(int DevID, PDEV_CHECKSELF_RESULT Reslut);

//12AD过载报警指示
int ADOverload(Func1 AdCallBack);

//13AGC增益值查询
int InquireDigitalGainValue(int DevID, PDEV_GetDigitalGainValue_RESULT Reslut);

int InquireDigitalGainValue8(int DevID, PDEV_GetDigitalGainValue_RESULT Reslut);

int InquireDigitalGainValue18(int DevID, PDEV_GetDigitalGainValue_RESULT Reslut);

//14万兆光口查询指令
int InquireWebStatus(int NetDevID, int NetNum, int TableNum, PDEV_CHECKSELF_RESULT Reslut);

//15心跳包
int HeartBeat(Func2 HeartBeatCallBack);

//17 190814新增测试用组播ip设置指令
int SetTestip();

//03远程复位指令

int SetResetDev(int DevID);

int InquireDevCC_ADCheckSelf(int DevID, PDEV_AD_CHECKSELF_RESULT Reslut);

int InquireDevRC_CheckSelf(PDEV_RC_CHECKSELF_RESULT Reslut);

int InquireDevRC_CheckSelf8(PDEV_RC_CHECKSELF_RESULT Reslut);

int SetDataDestIP(int NetID, int connectionId, char *ip, unsigned short  disPort, unsigned short sourcePort);

int SetDataSourceIP(int NetID, char *ip, int PacketLen, int Interval, int SlicesClock);

int SetDataReset_X(int boardNum, int enable, int dataSource);

int SetDataReset(int dataSource = 1);//wdx

int SetLinkEnable(int NetID, int linkNum, int en);//wdx_test

int SetAllEnable(int Enable);

# pragma endregion

# pragma region 外部控制接口

int SetDataDestIP_B(int BoardID, int NetID, int connectionId, char *ip, unsigned short  disPort, unsigned short sourcePort);

int SetDataSourceIP_B(int BoardID, int NetID, char *ip, int PacketLen, int Interval, int SlicesClock);

int SetDataReset_X_B(int boardNum, int enable, int dataSource);

int SetLinkEnable_B(int BoardID, int NetID, int linkNum, int en);//wdx_test

int SetDataInformation(int Band,int Standby, char * DisIP, unsigned short  DisPort, unsigned short SourcePort, int Enabled);

int SetDataInformation_Multi(int Band, int multi, int Standby, char * DisIP, unsigned short  DisPort, unsigned short SourcePort, int Enabled);

int SetDataEnable(int Band, int Standby, int Enabled);

int RegisterTemperature(Func func);//温度回调函数

# pragma endregion

#pragma region 射频控制

	int SetRFFreqBand(int RF_DevID, int DevID, int Channel, float StartFreq, float EndFreq);

	int SetRFFreqBand8(int RF_DevID, int DevID, int Channel, float StartFreq, float EndFreq);

	int SetCSFreqBand(int RF_DevID, int DevID, float StartFreq, float EndFreq);

	int SetRFGainValue8(int RF_DevID, int DevID, int Channel, int Value);

	int SetRFAllGainValue(int RF_DevID, int DevID, int Value);

	int SetRFAllWorkMode(int RF_DevID, int DevID, int Mode);

	int SetCSGainValue(int RF_DevID, int DevID, int Value);

	int SetCSWorkMode(int RF_DevID, int DevID, int Value);

	int SetGSWorkMode(int RF_DevID, int DevID, int Value);

	int SetRFPowerSwitch(int RF_DevID, int DevID, int Channel, int PowerSwitch);

	int SetRFPowerSwitch8(int RF_DevID, int DevID, int Channel, int PowerSwitch);

	int SetCSPowerSwitch(int RF_DevID, int DevID, int PowerSwitch);

	int SetGSPowerSwitch(int RF_DevID, int DevID, int PowerSwitch);

	int SetRFAllParamConfig(int RF_DevID, int DevID, int Channel, int GainValue, int WorkMode);

	int SetRFAllParamConfig8(int RF_DevID, int DevID, int Channel, int GainValue, int WorkMode);

	int SetCSAllParamConfig(int RF_DevID, int DevID, int GainValue, int WorkMode);

	int SetWSWholeStatus(int RF_DevID, int DevID, int GainValue, int RF_WorkMode, int CS_WorkMode, int GS_WorkMode);

	int SetWSWholeStatusZC(int RF_DevID, int DevID, int GainValue, int RF_WorkMode, int CS_WorkMode, int GS_WorkMode);

	int SetWSWholeStatusZC18(int RF_DevID, int DevID, int GainValue, int RF_WorkMode, int CS_WorkMode, int GS_WorkMode);

	int InquireRFDevIdentity(int RF_DevID, int DevID, int Channel, PDEV_RF_IDENTITY_RESULT Reslut);

	int InquireRFDevIdentity8(int RF_DevID, int DevID, int Channel, PDEV_RF_IDENTITY_RESULT Reslut);

	int InquireCSDevIdentity(int RF_DevID, int DevID, PDEV_CS_IDENTITY_RESULT Reslut);

	int InquireGSDevIdentity(int RF_DevID, int DevID, PDEV_GS_IDENTITY_RESULT Reslut);


#pragma endregion

#pragma region 射频查询V1.6.2

	int InquireRFDevState_Z(int RF_DevID, int DevID, int Channel, PDEV_RF_STATUS_RESULT Reslut);

	int InquireCSDevState_Z(int RF_DevID, int DevID, PDEV_CS_STATUS_RESULT Reslut);

	int InquireGSDevState_Z(int RF_DevID, int DevID, PDEV_GS_STATUS_RESULT Reslut);

#pragma endregion

#pragma region 射频协议V1.6.1

	int InquireRFDevState_V161(int RF_DevID, int DevID, int Channel, PDEV_RF_STATUS_RESULT Reslut);

	int InquireRFDevState_V161_8(int RF_DevID, int DevID, int Channel, PDEV_RF_STATUS_RESULT Reslut);

	int InquireCSDevState_V161(int RF_DevID, int DevID, PDEV_CS_STATUS_RESULT Reslut);

	int InquireGSDevState_V161(int RF_DevID, int DevID, PDEV_GS_STATUS_RESULT Reslut);

#pragma endregion

#pragma region V1.3

	int InquireCSDevState(int DevID, PDEV_CS_STATUS_RESULT Reslut);

	int InquireGSDevState(int DevID, PDEV_GS_STATUS_RESULT Reslut);

#pragma endregion
	//********************************************************************END  Lijf*************************************************************//

	void TestSend();
	void TestRecv();
};