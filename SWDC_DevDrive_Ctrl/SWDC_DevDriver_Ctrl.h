#pragma once

#ifndef _SWDC_DEVDRIVER_CTRL_H_
#define _SWDC_DEVDRIVER_CTRL_H_
#include "DefineType.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifndef TOEC_DLL_EXPORT
#define TOEC_DLL_EXPORT  _declspec(dllexport) 
#else
#define TOEC_DLL_EXPORT  _declspec(dllimport) 
#endif 

#pragma region 设备初始化与释放设备
	/*
	*Function	:InitialzeDevice
	*Description:初始化设备;
	*Input		:DevIP：设备端IP;  DevCCPort：设备端网络端口0(用于采集板通信);  DevFCPort：设备端网络端口1(用于转发板通信)； LocalIP:本地IP; LocalPort:本地接口;
	*Output		:无 ;
	*Return		:0 初始化成功， -1 初始化失败;
	*Other		:设备初始化，需指定网络信息。设备需要指定两个端口，分别用于不同功能板卡通信。需指定本机地址与端口。本机端口必须:4000;
	*/
	int TOEC_DLL_EXPORT InitialzeDevice(const char* DevIP, const unsigned short DevCCPort, const unsigned DevFCPort,const char * LocalIP);
	/*
	*Function	:FreeDevice
	*Description:释放设备驱动，回收资源;
	*Input		:无 ;
	*Output		:无 ;
	*Return		:0 释放成功， -1 设备未初始化，释放失败;
	*Other		:无;
	*/
	int TOEC_DLL_EXPORT FreeDevice();
#pragma endregion

#pragma region 千兆网网络配置

	/*
	*Function	:LocalNetConfig_IP
	*Description:修改本地IP地址;
	*Input		:IP：修改后本地IP地址;
	*Output		:无 ;
	*Return		:0 成功， 非0 失败;
	*Other		:通知设备，本地端IP地址已改变;
	*/
	int TOEC_DLL_EXPORT LocalNetConfig_IP(const char *IP);
	/*
	*Function	:DevNetConfig_IP
	*Description:修改设备IP地址;
	*Input		:IP：修改后设备IP地址;;
	*Output		:无 ;
	*Return		:0 成功， 非0 失败;
	*Other		:无;
	*/
	int TOEC_DLL_EXPORT DevNetConfig_IP(const char *IP);
	/*
	*Function	:DevNetConfig_CCPort
	*Description:修改设备采集板端口号;
	*Input		:CCPort：修改后端口号;
	*Output		:无 ;
	*Return		:0 成功， 非0 失败;
	*Other		:无;
	*/
	int TOEC_DLL_EXPORT DevNetConfig_CCPort(const unsigned short CCPort);
	/*
	*Function	:DevNetConfig_FCPort
	*Description:修改设备转发板端口号;
	*Input		:CCPort：修改后端口号;
	*Output		:无 ;
	*Return		:0 成功， 非0 失败;
	*Other		:无;
	*/
	int TOEC_DLL_EXPORT DevNetConfig_FCPort(const unsigned short FCPort);

#pragma endregion

#pragma region 采集板控制
	/*
	*Function	:RFGainMode
	*Description:射频增益模式控制;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;		Mode-模式[0,1]  0:MGC 1:AGC;
	*Output		:无;
	*Return		:0 成果， 非0 失败;
	*Other		:无;
	*/

	//时标切换原有
	//int TOEC_DLL_EXPORT SelectTimeDev(int TimeDevType);

	//int TOEC_DLL_EXPORT ResetDev(int DevID = 0);

#pragma endregion

#pragma region 设备自检

	/*
	*Function	:DevRC_CheckSelf
	*Description: Reslut--查询结果
	*Input		:无;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	int TOEC_DLL_EXPORT DevRC_CheckSelf( PDEV_RC_CHECKSELF_RESULT Reslut);
#pragma endregion

#pragma region 万兆网数据传输

#pragma region 万兆网数据传输配置

	/*
	*Function	:DataDestIP
	*Description:万兆网数据目的端配置;
	*Input		:NetID--万兆网光口号   取值范围[1-8]，1-4为主板，5-8为从板  connectionId--万兆网光口内的链接序号  ip--数据传输目的端IP地址      disPort--数据传输目的端口号
	sourcePort--对应链接的源端口号
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	int TOEC_DLL_EXPORT DataDestIP(int NetID, int connectionId, const char *ip, unsigned short disPort, unsigned short sourcePort);

	/*
	*Function	:DataLinkEnable
	*Description:万兆网数据链路使能配置;
	*Input		:NetID--万兆网光口号   取值范围[1-8]，1-4为主板，5-8为从板     linkNum--万兆网光口内的链接序号   en--使能，1：开 0：关
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	int TOEC_DLL_EXPORT DataLinkEnable(int NetID, int linkNum, int en);

	/*
	*Function	:DataSourceIP
	*Description:万兆网数据源端配置;
	*Input		:NetID--万兆网光口号   取值范围[1-8]，1-4为主板，5-8为从板      ip--数据传输源IP地址
	源端的链接序号的IP相同，端口号定义第一个链接的源端口，其他链接依次加1
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	int TOEC_DLL_EXPORT DataSourceIP(int NetID, const char *ip, int PacketLen, int Interval, int SlicesClock);
	int TOEC_DLL_EXPORT DataInformation(int Band, int Standby, char * DisIP, unsigned short  DisPort, unsigned short SourcePort, int Enabled);
	int TOEC_DLL_EXPORT DataAllEnable(int Enabled);
#pragma endregion

#pragma region 万兆网数据传输备份
	int TOEC_DLL_EXPORT DataDestIP_B(int BoardID, int NetID, int connectionId, const char *ip, unsigned short disPort, unsigned short sourcePort);
	int TOEC_DLL_EXPORT DataLinkEnable_B(int BoardID, int NetID, int linkNum, int en);
	int TOEC_DLL_EXPORT DataReset_X_B(int boardNum, int enable, int dataSource);
	int TOEC_DLL_EXPORT DataSourceIP_B(int BoardID, int NetID, const char *ip, int PacketLen, int Interval, int SlicesClock);
#pragma endregion

#pragma endregion

#pragma region FPGA程序下载

	/*
	*Function	:DevCC_FPGALoad
	*Description: DevID--板卡号 取值范围[1,5]    FilePatht--程序地址
	*Input		:无;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	int TOEC_DLL_EXPORT DevCC_FPGALoad(int DevID, char * FilePath);
	/*
	*Function	:DevRC_FPGALoad
	*Description: FlashID--flash编号 取值范围[1,4]    FilePatht--程序地址
	*Input		:无;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	int TOEC_DLL_EXPORT DevRC_FPGALoad(int FlashID, char * FilePath);

#pragma endregion

#pragma region 温度信息
	void TOEC_DLL_EXPORT RegisterTemperatureCallback(Func func);
#pragma endregion

#pragma region 射频协议V1.7

#pragma region 版本控制
	int TOEC_DLL_EXPORT CtrlVersion(int Version = 2);

	int TOEC_DLL_EXPORT CtrlType(int Type = 0);

	int TOEC_DLL_EXPORT DevDriveID(int ID);
#pragma endregion

#pragma region 射频控制

	int TOEC_DLL_EXPORT RFFreqBand(int RF_DevID, int Channel, float StartFreq, float EndFreq);

	int TOEC_DLL_EXPORT CSFreqBand(int RF_DevID, float StartFreq, float EndFreq);

	int TOEC_DLL_EXPORT RFAllGainValue(int RF_DevID,int Value);

	int TOEC_DLL_EXPORT RFAllWokeMode(int RF_DevID, int Mode);

	int TOEC_DLL_EXPORT RFPowerSwitch(int RF_DevID, int Channel, int PowerSwitch);

	int TOEC_DLL_EXPORT RFParamConfig(int RF_DevID, int Channel, int GainValue, int WorkMode);

	int TOEC_DLL_EXPORT CSGainValue(int RF_DevID, int Value);

	int TOEC_DLL_EXPORT CSWorkMode(int RF_DevID, int Mode);

	int TOEC_DLL_EXPORT CSPowerSwitch(int RF_DevID, int PowerSwitch);

	int TOEC_DLL_EXPORT CSAllParamConfig(int RF_DevID, int GainVlaue, int WorkMode);

	int TOEC_DLL_EXPORT GSWorkMode(int RF_DevID, int Mode);

	int TOEC_DLL_EXPORT GSPowerSwitch(int RF_DevID, int PowerSwitch);

	int TOEC_DLL_EXPORT WSWholeStatus(int RF_DevID, int GainValue, int RF_WorkMode, int CS_WorkMode, int GS_WorkMode);

#pragma endregion

#pragma region 射频查询

	int TOEC_DLL_EXPORT RFGetDevIdentity(int RF_DevID, int Channel, PDEV_RF_IDENTITY_RESULT Reslut);

	int TOEC_DLL_EXPORT CSGetDevIdentity(int RF_DevID, PDEV_CS_IDENTITY_RESULT Reslut);

	int TOEC_DLL_EXPORT GSGetDevIdentity(int RF_DevID, PDEV_GS_IDENTITY_RESULT Reslut);

#pragma endregion

#pragma endregion

#pragma region JGZC采集板控制200102LX修改
	
	//01采集板FPGA程序自检
	int TOEC_DLL_EXPORT DevCC_FPGACheckSelf(PDEV_FPGA_CHECKSELF_RESULT Reslut);

	//02射频增益模式控制 200102LX
	int TOEC_DLL_EXPORT RFGainMode(int Mode);

	//03射频增益模式查询 200102LX
	int TOEC_DLL_EXPORT GetRFGainMode(PDEV_DevRF_GetGainMode_RESULT Reslut);

	//04射频增益值控制指令 200102LX
	int TOEC_DLL_EXPORT RFGainValue(int Value);
	
	//05射频工作模式指令 200102LX
	int TOEC_DLL_EXPORT RFWokeMode(int Mode);

	//06射频状态信息回读 200102LX
	int TOEC_DLL_EXPORT RFGetDevState(PDEV_RF_STATUS_RESULT Reslut);

	//07窄带196路中心频点配置，带宽配置 200102LX
	int TOEC_DLL_EXPORT NBDDCFreqBand(int Index, int BandWidth, double Freq);

	//08宽带60路中心频点配置 200102LX
	int TOEC_DLL_EXPORT WBDDCFreqBand(int Index, double Freq);

	//09射频AGC参数控制
	int TOEC_DLL_EXPORT RFGainParameter(int UpLimit, int DownLimit, int HoldTime);
	
	//10数字AGC参数控制 200102LX
	int TOEC_DLL_EXPORT DigitalGainParameter(int UpLimit, int DownLimit, int HoldTime);

	//11时标切换200102LX
	int TOEC_DLL_EXPORT SelectTimeDev(int Mode, PDEV_TIMEDEV_RESULT Reslut);

	//12GPS / BD类型控制指令
	int TOEC_DLL_EXPORT SelectGPSBD(int Mode, PDEV_GPSBD_RESULT Reslut);
	
	//13数字增益24dB开关控制
	int TOEC_DLL_EXPORT DigitalGainSwitch(int WorkSwitch);

	//窄带AGC
	int TOEC_DLL_EXPORT NBDC_AGC(int WorkSwitch);

	//14系统时钟状态查询
	int TOEC_DLL_EXPORT DevCC_SampleClockCheckSelf(PDEV_CLKStatus_RESULT Reslut);

	//15Aurora接口channel_up连接状态
	int TOEC_DLL_EXPORT AuroraStatus(PDEV_CCAURORA_CHECKSELF_RESULT Reslut);

	//16FPGA版本号查询
	int TOEC_DLL_EXPORT GetFPGAVersion(PDEV_GetFPGAVersion_CHECKSELF_RESULT Reslut);

	//16FPGA版本号查询传输板
	int TOEC_DLL_EXPORT GetFPGAVersionFC(PDEV_GetFPGAVersionFC_CHECKSELF_RESULT Reslut);

	//17数字增益模式控制
	int TOEC_DLL_EXPORT DigitalGainMode(int Mode);

	//18数字增益模式查询
	int TOEC_DLL_EXPORT GetCCGainMode(PDEV_GetDigitalGainMode_RESULT Reslut);

	//19数字MGC衰减值控制
	int TOEC_DLL_EXPORT DigitalGainValue(int Value);

	//21窄带DDC时标精度
	int TOEC_DLL_EXPORT NBDDCAccuracy(int Accuracy);

	//22FFT时标精度
	int TOEC_DLL_EXPORT FFTAccuracy(int Accuracy);

	//23JGZCAGC增益值查询
	int TOEC_DLL_EXPORT GetDigitalGainValueJGZC(int DevID, PDEV_GetDigitalGainValueJGZC_RESULT Reslut);
#pragma endregion

#pragma region 原2019新增控制指令

	//03远程复位指令
	int TOEC_DLL_EXPORT ResetDev(int DevID);

	//06开机状态查询	
	int TOEC_DLL_EXPORT DevCC_DevPowerCheckSelf(int DevID, PDEV_CHECKSELF_RESULT Reslut);

	//08射频通信状态查询
	int TOEC_DLL_EXPORT CommunityStatus(int DevID, PDEV_CHECKSELF_RESULT Reslut);

	//12AD过载报警指示
	void TOEC_DLL_EXPORT RegisterADOverloadCallback(Func1 AdCallBack);

	//13AGC增益值查询
	int TOEC_DLL_EXPORT GetDigitalGainValue(int DevID, PDEV_GetDigitalGainValue_RESULT Reslut);

	//14万兆光口查询指令
	int TOEC_DLL_EXPORT GetWebStatus(int NetDevID, int NetNum, int TableNum, PDEV_CHECKSELF_RESULT Reslut);

	//15心跳包
	void TOEC_DLL_EXPORT RegisterHeartBeatCallback(Func2 HeartBeatCallBack);
#pragma region 数字增益模式和射频增益模式
	int TOEC_DLL_EXPORT RFGainState(int DevId, int UpLimit, int DownLimit, int DischargeConst);
	int TOEC_DLL_EXPORT CCGainMode(int DevId, int Mode);
	int TOEC_DLL_EXPORT CCGainState(int DevId, int UpLimit, int DownLimit, int DischargeConst);
#pragma endregion

#pragma region 数字设备状态查询
	int TOEC_DLL_EXPORT DevCC_DevPowerCheckSelf(int DevID, PDEV_CHECKSELF_RESULT Reslut);
	int TOEC_DLL_EXPORT CommunityStatus(int DevID, PDEV_CHECKSELF_RESULT Reslut);
	
#pragma endregion

#pragma region 射频整机状态查询
	int TOEC_DLL_EXPORT DevRF_ConnectCheckSelf(int DevID, PDEV_CHECKSELF_RESULT Reslut);
	int TOEC_DLL_EXPORT WSGetDevState(int RF_DevID, PDEV_WS_STATUS_RESULT Reslut);
	int TOEC_DLL_EXPORT WSGetDevIdentity(int RF_DevID, PDEV_GS_IDENTITY_RESULT Reslut);
#pragma endregion

#pragma endregion

#pragma region 190814新增测试用组播ip设置指令

	//190814新增测试用组播ip设置指令
	int TOEC_DLL_EXPORT Testip();

#pragma endregion

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // !_SWDC_DEVDRIVER_CTRL_H_