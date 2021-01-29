#pragma once
#include <memory>
#include <vector>
#include "DefineType.h"


class CDevCtrlCmd
{
private:
	void ClearBuffer();

public:
	CDevCtrlCmd();
	~CDevCtrlCmd();


#pragma region 千兆网网络配置

	/*
	*Function	:MakeCmd_SetDevIP
	*Description:千兆网设备端IP配置;
	*Input		:ip--ip地址;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_SetDevIP(char *ip);

	/*
	*Function	:MakeCmd_SetLocalIP
	*Description:千兆网本地IP配置;
	*Input		:ip--ip地址;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_SetLocalIP(char *ip);

	/*
	*Function	:MakeCmd_SetDevCCPort
	*Description:千兆网采集板通信端口配置;
	*Input		:port--端口号;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_SetDevCCPort(unsigned short port);

	/*
	*Function	:MakeCmd_SetDevFCPort
	*Description:千兆网传输板通信端口配置;
	*Input		:port--端口号;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_SetDevFCPort(unsigned short port);
#pragma endregion

#pragma region 万兆网数据传输配置

	/*
	*Function	:MakeCmd_DataSynchronization
	*Description:万兆网数据同步（复位）;
	*Input		:boardNum --万兆网板卡号   取值范围[0-1]，0为主板，1为从板 
				:enable -- 1或0输入，由1 -> 0完成复位工作。
				:dataSource--数据源  取值范围[0-1]，1：（默认）GTX传输数据  0：递增数据
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:需要enable先由1再到0;
	*/
	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_DataReset(int boardNum, int enable, int dataSource = 1 );

	/*
	*Function	:MakeCmd_SetDataDestIP
	*Description:万兆网数据目的端配置;
	*Input		:NetID--万兆网光口号   取值范围[1-8]，1-4为主板，5-8为从板  connectionId--万兆网光口内的链接序号  ip--数据传输目的端IP地址      disPort--目的端口号
				 sourcePort--对应链路源端口号
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetDataDestIP(int NetID, int connectionId, char *ip, unsigned short disPort, unsigned short sourcePort);

	/*
	*Function	:MakeCmd_SetDataSourceIP
	*Description:万兆网数据源端配置;
	*Input		:NetID--万兆网光口号   取值范围[1-8]，1-4为主板，5-8为从板      ip--数据传输源IP地址   mac--数据传输源端MAC地址   
	            
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetDataSourceIP(int NetID, char *ip, char *mac, int PacketLen, int Interval, int SlicesClock);

	/*
	*Function	:MakeCmd_SetDataSendSource
	*Description:万兆网传输链路使能;
	*Input		:NetID--万兆网光口号   取值范围[1-8]，1-4为主板，5-8为从板     connectionId--万兆网光口内的链接序号   en--使能，1：开 0：关
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetDataSendSource(int NetID, int connectionId, int en);

	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetDataAllEnable(int NetID,int en);

	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetMultiSelect(int NetID, int connectionId, int multi);

	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetMultiMac(int NetID, int connectionId, char *MultiMac);

#pragma endregion

//0702LX添加数字AGC部分功能
#pragma region 采集板控制

	/*
	*Function	:MakeCmd_ResetDev
	*Description:远程复位指令;
	*Input		:DevID--采集板卡号 取值范围[0,5]  0 代表所有板卡;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_ResetDev(int DevID);
	int MakeCmd_ResetDev_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_RFGainMode
	*Description:02射频增益模式控制;
	*Input		:Mode-模式[0,1]  0:MGC 1:AGC;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFGainMode(int Mode);
	int MakeCmd_RFGainMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_DevRF_GetGainMode
	*Description:03射频增益模式查询;
	*Input		:Mode-模式[0,1]  0:MGC 1:AGC;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevRF_GetGainMode();
	int MakeCmd_DevRF_GetGainMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_DevRF_GetGainMode_RESULT Reslut);
	/*
	*Function	:MakeCmd_RFGainValue
	*Description:04射频模块增益衰减值控制;
	*Input		:Value-衰减值[0,31];
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFGainValue(int Value);
	int MakeCmd_RFGainValue_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_RFWokeMode
	*Description:05射频模块工作模式控制;
	*Input		:Mode-工作模式[0,1] 0:常规  1:低噪声;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFWokeMode(int Mode);
	int MakeCmd_RFWokeMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_RFGetDevState_Ret_Analysis
	*Description:06解析射频模块设备状态查询结果;
	*Input		:RetData--设备接收到的自检结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成果， -1 解析结果不符合协议;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFGetDevState();
	int MakeCmd_RFGetDevState_Ret_Analysis(std::shared_ptr<unsigned char>  &&RetData, PDEV_RF_STATUS_RESULT Reslut);

	/*
	*Function	:MakeCmd_DevRF_GetGainMode
	*Description:07窄带196路中心频点配置，带宽配置;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_NBDDCFreqBand(int Index, int BandWidth, double Freq);
	int MakeCmd_NBDDCFreqBand_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int *Reslut);

	/*
	*Function	:MakeCmd_DevRF_GetGainMode
	*Description:08宽带60路中心频点配置;
	*Input		:Mode-模式[0,1]  0:MGC 1:AGC;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_WBDDCFreqBand(int Index, double Freq);
	int MakeCmd_WBDDCFreqBand_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int *Reslut);

	/*
	*Function	:MakeCmd_RFGainParameter
	*Description:09射频AGC参数控制;
	*Input		:UpLimit-AGC上限;		DownLimit-AGC下限		HoldTime-保持时间;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFGainParameter(int UpLimit, int DownLimit, int HoldTime);
	int MakeCmd_RFGainParameter_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);
	
	/*
	*Function	:MakeCmd_DigitalGainParameter
	*Description:10数字AGC参数控制;
	*Input		:UpLimit-AGC上限;		DownLimit-AGC下限		HoldTime-保持时间;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DigitalGainParameter(int UpLimit, int DownLimit, int HoldTime);
	int MakeCmd_DigitalGainParameter_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_SelectTimeDev
	*Description:11时标类型控制指令;
	*Input		:Mode-模式[0,1]  0:B码 1:串码;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_SelectTimeDev(int Mode);
	int MakeCmd_SelectTimeDev_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_TIMEDEV_RESULT Reslut);

	/*
	*Function	:MakeCmd_GPSBD
	*Description:12GPS/BD类型控制指令;
	*Input		:Mode-模式[0,1]  0:GPS 1:BD;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GPSBD(int Mode);
	int MakeCmd_GPSBD_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GPSBD_RESULT Reslut);

	/*
	*Function	:MakeCmd_DigitalGainSwitch
	*Description:13数字增益24dB开关控制;
	*Input		:WorkSwitch-工作开关[0,1]		0:关闭24dB数字增益		1:开启24dB数字增益;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DigitalGainSwitch(int Switch);
	int MakeCmd_DigitalGainSwitch_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_NBDC_AGC
	*Description:窄带AGC开关控制;
	*Input		:WorkSwitch-工作开关[0,1]		0:关闭24dB数字增益		1:开启24dB数字增益;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_NBDC_AGC(int WorkSwitch);
	int MakeCmd_NBDC_AGC_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_CLKStatus
	*Description:14系统时钟状态查询;
	*Input		:无;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_CLKStatus();
	/*
	*Function	:MakeCmd_MakeCmd_CLKStatus_Ret_Analysis
	*Description:14解析系统时钟状态查询结果;
	*Input		:RetData--设备接收到的查询结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成功， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_CLKStatus_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_CLKStatus_RESULT Reslut);

	/*
	*Function	:MakeCmd_AuroraStatus
	*Description:15采集板aurora状态查询;
	*Input		:无;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_AuroraStatus();
	/*
	*Function	:MakeCmd_MakeCmd_AuroraStatus_Ret_Analysis
	*Description:15解析采集板aurora状态查询结果;
	*Input		:RetData--设备接收到的查询结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成功， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_AuroraStatus_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_CCAURORA_CHECKSELF_RESULT Reslut);

	/*
	*Function	:MakeCmd_FPGAVersion2
	*Description:16FPGA版本号查询采集板;
	*Input		:
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GetFPGAVersionFC();
	/*
	*Function	:MakeCmd_AuroraStatus_Ret_Analysis
	*Description:16解析FPGA版本号查询结果传输版;
	*Input		:
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成功， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_GetFPGAVersionFC_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetFPGAVersionFC_CHECKSELF_RESULT Reslut);

	/*
	*Function	:MakeCmd_GetFPGAVersion
	*Description:16FPGA版本号查询;
	*Input		:
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GetFPGAVersion();
	/*
	*Function	:MakeCmd_GetFPGAVersion_Ret_Analysis
	*Description:16解析FPGA版本号查询结果;
	*Input		:
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成功， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_GetFPGAVersion_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetFPGAVersion_CHECKSELF_RESULT Reslut);
	/*
	*Function	:MakeCmd_MakeCmd_AuroraStatus_Ret_Analysis
	*Description:16解析FPGA版本号查询结果-8T;
	*Input		:
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成功， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_GetFPGAVersion8_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetFPGAVersion_CHECKSELF_RESULT Reslut);

	/*
	*Function	:MakeCmd_DigitalGainMode
	*Description:17数字增益模式控制;
	*Input		:Mode-模式[0,1]  0:MGC 1:AGC;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DigitalGainMode(int Mode);
	int MakeCmd_DigitalGainMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_GetDigitalGainMode
	*Description:18数字增益模式查询;
	*Input		:无
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GetDigitalGainMode();
	/*
	*Function	:MakeCmd_PowerOnStatus_Ret_Analysis
	*Description:18解析数字增益模式查询结果;
	*Input		:RetData--设备接收到的查询结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成功， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_GetDigitalGainMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainMode_RESULT Reslut);

	/*
	*Function	:MakeCmd_DigitalGainValue
	*Description:19数字MGC衰减值控制;
	*Input		:Value-衰减值[0,20];
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DigitalGainValue(int Value);
	int MakeCmd_DigitalGainValue_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_NBDDCAccuracy
	*Description://21窄带DDC时标精度;
	*Input		:Value-衰减值[0,20];
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_NBDDCAccuracy(int Accuracy);
	int MakeCmd_NBDDCAccuracy_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_DigitalGainValue
	*Description:19数字MGC衰减值控制;
	*Input		:Value-衰减值[0,20];
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_FFTAccuracy(int Accuracy);
	int MakeCmd_FFTAccuracy_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_GetDigitalGainValueJGZC
	*Description:JGZCAGC增益值查询;
	*Input		:无;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GetDigitalGainValueJGZC(int DevID);
	/*
	*Function	:MakeCmd_GetDigitalGainValue_Ret_Analysis
	*Description:解析JGZCAGC增益值查询结果;
	*Input		:RetData--设备接收到的查询结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成功， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_GetDigitalGainValueJGZC_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValueJGZC_RESULT Reslut);

#pragma endregion

//0702LX添加数字AGC部分功能;0822LX添加FPGA版本号查询

#pragma region 设备自检

	/*
	*Function	:MakeCmd_DevCC_FPGACheckSelf
	*Description:采集板FPGA程序自检;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevCC_FPGACheckSelf();
	/*
	*Function	:MakeCmd_DevCC_FPGACheckSelf_Ret_Analysis
	*Description:解析采集板FPGA程序自检结果;
	*Input		:RetData--设备接收到的自检结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成果， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_DevCC_FPGACheckSelf_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_FPGA_CHECKSELF_RESULT Reslut);

	/*
	*Function	:MakeCmd_DevCC_ADCheckSelf
	*Description:采集板AD链路自检;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevCC_ADCheckSelf(int DevID);
	/*
	*Function	:MakeCmd_DevCC_ADCheckSelf_Ret_Analysis
	*Description:解析采集板AD链路自检结果;
	*Input		:RetData--设备接收到的自检结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成果， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_DevCC_ADCheckSelf_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_AD_CHECKSELF_RESULT Reslut);

	/*
	*Function	:MakeCmd_DevRC_CheckSelf
	*Description:传输板自检;
	*Input		:无;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevRC_CheckSelf();
	/*
	*Function	:MakeCmd_DevRC_CheckSelf_Ret_Analysis
	*Description:解析传输板自检结果;
	*Input		:RetData--设备接收到的自检结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成果， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_DevRC_CheckSelf_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_RC_CHECKSELF_RESULT Reslut);
	/*
	*Function	:MakeCmd_DevRC_CheckSelf_Ret_Analysis8
	*Description:解析传输板自检结果(8T);
	*Input		:RetData--设备接收到的自检结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成果， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_DevRC_CheckSelf_Ret_Analysis8(std::shared_ptr<unsigned char> && RetData, PDEV_RC_CHECKSELF_RESULT Reslut);
	
	/*
	*Function	:MakeCmd_RFCommunityStatus
	*Description:射频通信状态查询;
	*Input		:DevID--采集板卡号 取值范围[0,5]  0 代表所有板卡;		Status-射频通信状态;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFCommunityStatus(int DevID);
	/*
	*Function	:MakeCmd_MakeCmd_RFCommunityStatus_Ret_Analysis
	*Description:解析射频通信状态查询结果;
	*Input		:RetData--设备接收到的查询结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成功， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_RFCommunityStatus_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_CHECKSELF_RESULT Reslut);

	/*
	*Function	:MakeCmd_GetDigitalGainValue
	*Description:AGC增益值查询;
	*Input		:DevID--采集板卡号 取值范围[1,5];
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GetDigitalGainValue(int DevID);
	/*
	*Function	:MakeCmd_GetDigitalGainValue_Ret_Analysis
	*Description:解析AGC增益值查询结果;
	*Input		:RetData--设备接收到的查询结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成功， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_GetDigitalGainValue_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut);
	void MakeDigitalGain(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut, int startindex, int endindex);

	/*
	*Function	:MakeCmd_GetDigitalGainValue8
	*Description:AGC增益值查询-8通道;
	*Input		:DevID--采集板卡号 取值范围[1,4];
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GetDigitalGainValue8(int DevID);
	/*
	*Function	:MakeCmd_GetDigitalGainValue_Ret_Analysis
	*Description:解析AGC增益值查询结果-8通道;
	*Input		:RetData--设备接收到的查询结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成功， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_GetDigitalGainValue8_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut);
	void MakeDigitalGain8(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut, int startindex, int endindex);

	/*
	*Function	:MakeCmd_GetDigitalGainValue18
	*Description:AGC增益值查询-18通道;
	*Input		:DevID--采集板卡号 取值范围[1,4];
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GetDigitalGainValue18(int DevID);
	/*
	*Function	:MakeCmd_GetDigitalGainValue_Ret_Analysis
	*Description:解析AGC增益值查询结果-18通道;
	*Input		:RetData--设备接收到的查询结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成功， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_GetDigitalGainValue18_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut);
	void MakeDigitalGain18(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut, int startindex, int endindex);

	std::shared_ptr<unsigned char> MakeCmd_GetWebStatus(int NetDevID, int NetNum, int TableNum);
	int MakeCmd_GetWebStatus_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetWebStatus_CHECKSELF_RESULT Reslut);


	//********************************************************************end Lijf************************************************************************//
#pragma endregion

#pragma region FPGA程序下载

	/*
	*Function	:MakeCmd_DevRC_FPGAErasure
	*Description:传输板板FPGA程序擦除;
	*Input		:FlashID--设备号 取值范围[1,4]  1 代表主板FPGA1,2代表主板FPGA2     3代表从板FPGA1，       4代表从板FPGA2;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevRC_FPGAErasure(int FlashID);


	/*
	*Function	:MakeCmd_DevRC_FPGAWriteEnd
	*Description:传输板板FPGA程序写入完毕;
	*Input		:FlashID--设备号 取值范围[1,4]  1 代表主板FPGA1,2代表主板FPGA2     3代表从板FPGA1，       4代表从板FPGA2;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevRC_FPGAWriteEnd(int FlashID);

	/*
	*Function	:MakeCmd_DevRC_FPGALoad
	*Description:传输板板FPGA程序从FLASH加载;
	*Input		:FlashID--设备号 取值范围[1,4]  1 代表主板FPGA1,2代表主板FPGA2     3代表从板FPGA1，       4代表从板FPGA2;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevRC_FPGALoad(int FlashID);

	/*
	*Function	:MakeCmd_DevRC_FPGAWriting
	*Description:传输板板FPGA程序写入包;
	*Input		:FlashID--设备号 取值范围[1,4]  1 代表主板FPGA1,2代表主板FPGA2     3代表从板FPGA1，       4代表从板FPGA2;
	             index--FPGA程序包序号        buffer--FPGA程序数据       filesize--文件大小
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevRC_FPGAWriting(int FlashID, int index, char* buffer,int filesize);

	/*
	*Function	:MakeCmd_DevRC_FPGAWriting_Ret_Analysis
	*Description:解析传输板FPGA程序回传信息;
	*Input		:RetData--设备接收到的自检结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成果， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_DevRC_FPGAWriting_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_RCFPGA_LOAD_RESULT Reslut);

	/*
	*Function	:MakeCmd_DevCC_FPGAErasure
	*Description:采集板FPGA程序擦除;
	*Input		:DevID--设备号 取值范围[1,5]  
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevCC_FPGAErasure(int DevID);
	/*
	*Function	:MakeCmd_DevCC_FPGAVersion
	*Description:采集板FPGA程序版本号;
	*Input		:DevID--设备号 取值范围[1,5]   Version--程序版本号
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/

	std::shared_ptr<unsigned char> MakeCmd_DevCC_FPGAVersion(int DevID,int Version);

	/*
	*Function	:MakeCmd_DevCC_FPGAWriting
	*Description:采集板FPGA程序烧写;
	*Input		:DevID--设备号 取值范围[1,5]    index--FPGA程序包序号       buffer--FPGA程序数据
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/

	std::shared_ptr<unsigned char> MakeCmd_DevCC_FPGAWriting(int DevID, int index, char* buffer);

	/*
	*Function	:MakeCmd_DevCC_FPGAVersion
	*Description:采集板FPGA程序烧写完毕;
	*Input		:DevID--设备号 取值范围[1,5]
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/

	std::shared_ptr<unsigned char> MakeCmd_DevCC_FPGAWriteEnd(int DevID);

	/*
	*Function	:MakeCmd_DevRC_FPGA_Ret_Analysis
	*Description:解析采集板FPGA程序擦除结果回传信息;
	*Input		:RetData--设备接收到的自检结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成果， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_DevCC_FPGA_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_CCFPGA_LOAD_RESULT Reslut);

#pragma endregion

#pragma region 射频控制增加
	std::shared_ptr<unsigned char> MakeCmd_RFFreqBand_X(int DevId, int Channel, int StartFreq, int EndFreq);
	std::shared_ptr<unsigned char> MakeCmd_RFAllParamConfig_X(int DevId, int Channel, std::shared_ptr<RF_PARAM> Param);
	std::shared_ptr<unsigned char> MakeCmd_CSFreqBand_X(int DevId, int StartFreq, int EndFreq);
	std::shared_ptr<unsigned char> MakeCmd_CSAllParamConfig_X(int DevId, std::shared_ptr<CS_PARAM> Param);
	std::shared_ptr<unsigned char> MakeCmd_WSWholeStatus_X(int DevId, std::shared_ptr<DEV_PARAM>  Param);

#pragma endregion

#pragma region 万兆网数据传输备份
	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_DataReset_B(int boardNum, int enable, int dataSource = 1);
	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetDataDestIP_B(int BoardID, int NetID, int connectionId, char *ip, unsigned short disPort, unsigned short sourcePort);
	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetDataSourceIP_B(int BoardID, int NetID, char *ip, char *mac, int PacketLen, int Interval, int SlicesClock);
	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetDataSendSource_B(int BoardID, int NetID, int connectionId, int en);

#pragma endregion
	
#pragma region 射频控制
	std::shared_ptr<unsigned char> MakeCmd_RFFreqBand(int RF_DevID, int DevId, int Channel, int StartFreq, int EndFreq);
	std::shared_ptr<unsigned char> MakeCmd_RFFreqBand8(int RF_DevID, int DevId, int Channel, int StartFreq, int EndFreq);
	int MakeCmd_RFFreqBand_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	std::shared_ptr<unsigned char> MakeCmd_CSFreqBand(int RF_DevID, int DevId, int StartFreq, int EndFreq);
	int MakeCmd_CSFreqBand_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_RFPowerSwitch
	*Description:射频模块电源开关设置;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;	Channel-控制的目标通道[0,19];	 PowerSwitch-电源开关状态[0,4];
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFPowerSwitch(int RF_DevID, int DevId, int Channel, int PowerSwitch);
	std::shared_ptr<unsigned char> MakeCmd_RFPowerSwitch8(int RF_DevID, int DevId, int Channel, int PowerSwitch);
	int MakeCmd_RFPowerSwitch_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_RFAllParamConfig
	*Description:射频模块全控制字设置;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;	Channel-控制的目标通道[0,19];	 Param-全状态控制字，结构体定义如上;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFAllParamConfig(int RF_DevID, int DevId, int Channel, std::shared_ptr<RF_PARAM> Param);
	std::shared_ptr<unsigned char> MakeCmd_RFAllParamConfig8(int RF_DevID, int DevId, int Channel, std::shared_ptr<RF_PARAM> Param);
	int MakeCmd_RFAllParamConfig_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);


	std::shared_ptr<unsigned char> MakeCmd_RFAllGainValue(int RF_DevID, int DevId, int Value);
	int MakeCmd_RFAllGainValue_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	std::shared_ptr<unsigned char> MakeCmd_RFAllWokeMode(int RF_DevID, int DevId, int Mode);
	int MakeCmd_RFAllWokeMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);


	/*
	*Function	:MakeCmd_CSGainValue
	*Description:校正源模块增益值设置;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;	 Value-增益值[0,62];
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_CSGainValue(int RF_DevID, int DevId, int Value);
	int MakeCmd_CSGainValue_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);


	/*
	*Function	:MakeCmd_CSWorkMode
	*Description:校正源模块工作模式设置;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;	 Mode-增益值[0,1]  0：关断  1 打开;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_CSWorkMode(int RF_DevID, int DevId, int Mode);
	int MakeCmd_CSWorkMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_CSPowerSwitch
	*Description:校正源模块电源开关设置;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;	 PowerSwitch-电源开关状态[0,4];
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_CSPowerSwitch(int RF_DevID, int DevId, int PowerSwitch);
	int MakeCmd_CSPowerSwitch_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_CSAllParamConfig
	*Description:校正源模块全参数设置;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;	 Param-全参数结构体指针  ;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_CSAllParamConfig(int RF_DevID, int DevId, std::shared_ptr<CS_PARAM> Param);
	int MakeCmd_CSAllParamConfig_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_GSWorkMode
	*Description:测向开关模块工作模式设置;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;	 Mode-工作模式[0,1] 0:校正状态  1：天线状态  ;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GSWorkMode(int RF_DevID, int DevId, int Mode);
	int MakeCmd_GSWorkMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_GSPowerSwitch
	*Description:测向开关开关状态设置;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;	 PowerSwitch-工作模式[0,1] 0:关断电源  1：打开电源  ;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GSPowerSwitch(int RF_DevID, int DevId, int PowerSwitch);
	int MakeCmd_GSPowerSwitch_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_WSWholeStatus
	*Description:整机状态设置;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;	 Param-全参数结构体指针  ;
	*Output		:无;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_WSWholeStatusZC(int RF_DevID, int DevId, std::shared_ptr<DEV_PARAM> Param);
	std::shared_ptr<unsigned char> MakeCmd_WSWholeStatusZC18(int RF_DevID, int DevId, std::shared_ptr<DEV_PARAM> Param);
	std::shared_ptr<unsigned char> MakeCmd_WSWholeStatus(int RF_DevID, int DevId, std::shared_ptr<DEV_PARAM> Param);
	int MakeCmd_WSWholeStatus_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_RFGetDevIdentity
	*Description:射频模块身份查询;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;   Channel-控制的目标通道[0,19];
	*Output		:无 ;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFGetDevIdentity(int RF_DevID, int DevID, int Channel);
	std::shared_ptr<unsigned char> MakeCmd_RFGetDevIdentity8(int RF_DevID, int DevID, int Channel);

	/*
	*Function	:MakeCmd_RFGetDevIdentity_Ret_Analysis
	*Description:解析射频模块身份查询结果;
	*Input		:RetData--设备接收到的自检结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成果， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_RFGetDevIdentity_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_RF_IDENTITY_RESULT Reslut);

	/*
	*Function	:MakeCmd_CSGetDevIdentity
	*Description:校正源模块身份查询;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;
	*Output		:无 ;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_CSGetDevIdentity_Z(int RF_DevID, int DevID);

	/*
	*Function	:MakeCmd_CSGetDevIdentity_Ret_Analysis
	*Description:解析校正源模块身份查询结果;
	*Input		:RetData--设备接收到的自检结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成果， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_CSGetDevIdentity_Ret_Analysis_Z(std::shared_ptr<unsigned char> && RetData, PDEV_CS_IDENTITY_RESULT Reslut);

	/*
	*Function	:MakeCmd_GSGetDevIdentity
	*Description:测向开关身份查询;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;
	*Output		:无 ;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GSGetDevIdentity_Z(int RF_DevID, int DevID);

	/*
	*Function	:MakeCmd_GSGetDevIdentity_Ret_Analysis
	*Description:解析测向开关身份查询结果;
	*Input		:RetData--设备接收到的自检结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成果， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_GSGetDevIdentity_Ret_Analysis_Z(std::shared_ptr<unsigned char> && RetData, PDEV_GS_IDENTITY_RESULT Reslut);

#pragma endregion
	
#pragma region 射频协议V1.6.1

#pragma region 射频查询

	int MakeCmd_RFGetDevState_Ret_Analysis_V161(std::shared_ptr<unsigned char>  &&RetData, PDEV_RF_STATUS_RESULT Reslut);
	int MakeCmd_CSGetDevState_Ret_Analysis_V161(std::shared_ptr<unsigned char> && RetData, PDEV_CS_STATUS_RESULT Reslut);
	int MakeCmd_GSGetDevState_Ret_Analysis_V161(std::shared_ptr<unsigned char> && RetData, PDEV_GS_STATUS_RESULT Reslut);
#pragma endregion
#pragma endregion

#pragma region 射频前端状态查询V1.3

	/*
	*Function	:MakeCmd_CSGetDevState
	*Description:校正源模块设备状态查询;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;
	*Output		:无 ;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_CSGetDevState(int DevID);

	/*
	*Function	:MakeCmd_CSGetDevState_Ret_Analysis
	*Description:解析校正源模块设备状态查询结果;
	*Input		:RetData--设备接收到的自检结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成果， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_CSGetDevState_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_CS_STATUS_RESULT Reslut);

	/*
	*Function	:MakeCmd_GSGetDevState
	*Description:测向开关模块设备状态查询;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;
	*Output		:无 ;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GSGetDevState(int DevID);

	/*
	*Function	:MakeCmd_GSGetDevState_Ret_Analysis
	*Description:解析测向开关模块设备状态查询结果;
	*Input		:RetData--设备接收到的自检结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成果， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_GSGetDevState_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GS_STATUS_RESULT Reslut);


#pragma endregion

#pragma region 射频查询V1.6.2
	/*
	*Function	:MakeCmd_RFGetDevState
	*Description:射频模块设备状态查询;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;	Channel-控制的目标通道[0,19];
	*Output		:无 ;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFGetDevState_Z(int RF_DevID, int DevID, int Channel);
	std::shared_ptr<unsigned char> MakeCmd_RFGetDevState_Z8(int RF_DevID, int DevID, int Channel);

	/*
	*Function	:MakeCmd_RFGetDevState_Ret_Analysis
	*Description:解析射频模块设备状态查询结果;
	*Input		:RetData--设备接收到的自检结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成果， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_RFGetDevState_Ret_Analysis_Z(std::shared_ptr<unsigned char>  &&RetData, PDEV_RF_STATUS_RESULT Reslut);

	/*
	*Function	:MakeCmd_CSGetDevState
	*Description:校正源模块设备状态查询;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;
	*Output		:无 ;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_CSGetDevState_Z(int RF_DevID, int DevID);

	/*
	*Function	:MakeCmd_CSGetDevState_Ret_Analysis
	*Description:解析校正源模块设备状态查询结果;
	*Input		:RetData--设备接收到的自检结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成果， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_CSGetDevState_Ret_Analysis_Z(std::shared_ptr<unsigned char> && RetData, PDEV_CS_STATUS_RESULT Reslut);

	/*
	*Function	:MakeCmd_GSGetDevState
	*Description:测向开关模块设备状态查询;
	*Input		:DevId--采集板卡号 取值范围[0,5]  0 代表所有板卡;
	*Output		:无 ;
	*Return		:组包完成后内存;
	*Other		:无;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GSGetDevState_Z(int RF_DevID, int DevID);

	/*
	*Function	:MakeCmd_GSGetDevState_Ret_Analysis
	*Description:解析测向开关模块设备状态查询结果;
	*Input		:RetData--设备接收到的自检结果buffer;
	*Output		:Reslut-解析后结果结构体 ;
	*Return		:0 结果解析成果， -1 解析结果不符合协议;
	*Other		:无;
	*/
	int MakeCmd_GSGetDevState_Ret_Analysis_Z(std::shared_ptr<unsigned char> && RetData, PDEV_GS_STATUS_RESULT Reslut);



#pragma endregion

#pragma region 190814新增测试用组播ip设置指令

	std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_SetTestip();

#pragma endregion

private:
	std::shared_ptr<unsigned char> m_CtrlCmd;
	std::shared_ptr<unsigned char> m_CtrlCmd_CCFPGA;
	std::shared_ptr<unsigned char> m_CtrlCmd_RCFPGA;

	static int const m_ProtocolSize = 128; //控制字 协议总长度;
	static int const m_CCFPGASize = 1032; //采集板FPGA数据包长度;
	static int const m_RCFPGASize = 1056; //传输板板FPGA数据包长度;

	std::vector<std::pair<unsigned long, unsigned long>> m_FreqBandInfo; //频段信息；
	std::vector<std::pair<unsigned long, unsigned long>> m_FreqBandInfoZC; //频段信息；
	std::vector<std::pair<unsigned long, unsigned long>> m_FreqBandInfoZC18; //频段信息；

};

