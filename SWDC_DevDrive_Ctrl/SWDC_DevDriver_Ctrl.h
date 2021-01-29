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

#pragma region �豸��ʼ�����ͷ��豸
	/*
	*Function	:InitialzeDevice
	*Description:��ʼ���豸;
	*Input		:DevIP���豸��IP;  DevCCPort���豸������˿�0(���ڲɼ���ͨ��);  DevFCPort���豸������˿�1(����ת����ͨ��)�� LocalIP:����IP; LocalPort:���ؽӿ�;
	*Output		:�� ;
	*Return		:0 ��ʼ���ɹ��� -1 ��ʼ��ʧ��;
	*Other		:�豸��ʼ������ָ��������Ϣ���豸��Ҫָ�������˿ڣ��ֱ����ڲ�ͬ���ܰ忨ͨ�š���ָ��������ַ��˿ڡ������˿ڱ���:4000;
	*/
	int TOEC_DLL_EXPORT InitialzeDevice(const char* DevIP, const unsigned short DevCCPort, const unsigned DevFCPort,const char * LocalIP);
	/*
	*Function	:FreeDevice
	*Description:�ͷ��豸������������Դ;
	*Input		:�� ;
	*Output		:�� ;
	*Return		:0 �ͷųɹ��� -1 �豸δ��ʼ�����ͷ�ʧ��;
	*Other		:��;
	*/
	int TOEC_DLL_EXPORT FreeDevice();
#pragma endregion

#pragma region ǧ������������

	/*
	*Function	:LocalNetConfig_IP
	*Description:�޸ı���IP��ַ;
	*Input		:IP���޸ĺ󱾵�IP��ַ;
	*Output		:�� ;
	*Return		:0 �ɹ��� ��0 ʧ��;
	*Other		:֪ͨ�豸�����ض�IP��ַ�Ѹı�;
	*/
	int TOEC_DLL_EXPORT LocalNetConfig_IP(const char *IP);
	/*
	*Function	:DevNetConfig_IP
	*Description:�޸��豸IP��ַ;
	*Input		:IP���޸ĺ��豸IP��ַ;;
	*Output		:�� ;
	*Return		:0 �ɹ��� ��0 ʧ��;
	*Other		:��;
	*/
	int TOEC_DLL_EXPORT DevNetConfig_IP(const char *IP);
	/*
	*Function	:DevNetConfig_CCPort
	*Description:�޸��豸�ɼ���˿ں�;
	*Input		:CCPort���޸ĺ�˿ں�;
	*Output		:�� ;
	*Return		:0 �ɹ��� ��0 ʧ��;
	*Other		:��;
	*/
	int TOEC_DLL_EXPORT DevNetConfig_CCPort(const unsigned short CCPort);
	/*
	*Function	:DevNetConfig_FCPort
	*Description:�޸��豸ת����˿ں�;
	*Input		:CCPort���޸ĺ�˿ں�;
	*Output		:�� ;
	*Return		:0 �ɹ��� ��0 ʧ��;
	*Other		:��;
	*/
	int TOEC_DLL_EXPORT DevNetConfig_FCPort(const unsigned short FCPort);

#pragma endregion

#pragma region �ɼ������
	/*
	*Function	:RFGainMode
	*Description:��Ƶ����ģʽ����;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;		Mode-ģʽ[0,1]  0:MGC 1:AGC;
	*Output		:��;
	*Return		:0 �ɹ��� ��0 ʧ��;
	*Other		:��;
	*/

	//ʱ���л�ԭ��
	//int TOEC_DLL_EXPORT SelectTimeDev(int TimeDevType);

	//int TOEC_DLL_EXPORT ResetDev(int DevID = 0);

#pragma endregion

#pragma region �豸�Լ�

	/*
	*Function	:DevRC_CheckSelf
	*Description: Reslut--��ѯ���
	*Input		:��;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	int TOEC_DLL_EXPORT DevRC_CheckSelf( PDEV_RC_CHECKSELF_RESULT Reslut);
#pragma endregion

#pragma region ���������ݴ���

#pragma region ���������ݴ�������

	/*
	*Function	:DataDestIP
	*Description:����������Ŀ�Ķ�����;
	*Input		:NetID--��������ں�   ȡֵ��Χ[1-8]��1-4Ϊ���壬5-8Ϊ�Ӱ�  connectionId--����������ڵ��������  ip--���ݴ���Ŀ�Ķ�IP��ַ      disPort--���ݴ���Ŀ�Ķ˿ں�
	sourcePort--��Ӧ���ӵ�Դ�˿ں�
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	int TOEC_DLL_EXPORT DataDestIP(int NetID, int connectionId, const char *ip, unsigned short disPort, unsigned short sourcePort);

	/*
	*Function	:DataLinkEnable
	*Description:������������·ʹ������;
	*Input		:NetID--��������ں�   ȡֵ��Χ[1-8]��1-4Ϊ���壬5-8Ϊ�Ӱ�     linkNum--����������ڵ��������   en--ʹ�ܣ�1���� 0����
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	int TOEC_DLL_EXPORT DataLinkEnable(int NetID, int linkNum, int en);

	/*
	*Function	:DataSourceIP
	*Description:����������Դ������;
	*Input		:NetID--��������ں�   ȡֵ��Χ[1-8]��1-4Ϊ���壬5-8Ϊ�Ӱ�      ip--���ݴ���ԴIP��ַ
	Դ�˵�������ŵ�IP��ͬ���˿ںŶ����һ�����ӵ�Դ�˿ڣ������������μ�1
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	int TOEC_DLL_EXPORT DataSourceIP(int NetID, const char *ip, int PacketLen, int Interval, int SlicesClock);
	int TOEC_DLL_EXPORT DataInformation(int Band, int Standby, char * DisIP, unsigned short  DisPort, unsigned short SourcePort, int Enabled);
	int TOEC_DLL_EXPORT DataAllEnable(int Enabled);
#pragma endregion

#pragma region ���������ݴ��䱸��
	int TOEC_DLL_EXPORT DataDestIP_B(int BoardID, int NetID, int connectionId, const char *ip, unsigned short disPort, unsigned short sourcePort);
	int TOEC_DLL_EXPORT DataLinkEnable_B(int BoardID, int NetID, int linkNum, int en);
	int TOEC_DLL_EXPORT DataReset_X_B(int boardNum, int enable, int dataSource);
	int TOEC_DLL_EXPORT DataSourceIP_B(int BoardID, int NetID, const char *ip, int PacketLen, int Interval, int SlicesClock);
#pragma endregion

#pragma endregion

#pragma region FPGA��������

	/*
	*Function	:DevCC_FPGALoad
	*Description: DevID--�忨�� ȡֵ��Χ[1,5]    FilePatht--�����ַ
	*Input		:��;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	int TOEC_DLL_EXPORT DevCC_FPGALoad(int DevID, char * FilePath);
	/*
	*Function	:DevRC_FPGALoad
	*Description: FlashID--flash��� ȡֵ��Χ[1,4]    FilePatht--�����ַ
	*Input		:��;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	int TOEC_DLL_EXPORT DevRC_FPGALoad(int FlashID, char * FilePath);

#pragma endregion

#pragma region �¶���Ϣ
	void TOEC_DLL_EXPORT RegisterTemperatureCallback(Func func);
#pragma endregion

#pragma region ��ƵЭ��V1.7

#pragma region �汾����
	int TOEC_DLL_EXPORT CtrlVersion(int Version = 2);

	int TOEC_DLL_EXPORT CtrlType(int Type = 0);

	int TOEC_DLL_EXPORT DevDriveID(int ID);
#pragma endregion

#pragma region ��Ƶ����

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

#pragma region ��Ƶ��ѯ

	int TOEC_DLL_EXPORT RFGetDevIdentity(int RF_DevID, int Channel, PDEV_RF_IDENTITY_RESULT Reslut);

	int TOEC_DLL_EXPORT CSGetDevIdentity(int RF_DevID, PDEV_CS_IDENTITY_RESULT Reslut);

	int TOEC_DLL_EXPORT GSGetDevIdentity(int RF_DevID, PDEV_GS_IDENTITY_RESULT Reslut);

#pragma endregion

#pragma endregion

#pragma region JGZC�ɼ������200102LX�޸�
	
	//01�ɼ���FPGA�����Լ�
	int TOEC_DLL_EXPORT DevCC_FPGACheckSelf(PDEV_FPGA_CHECKSELF_RESULT Reslut);

	//02��Ƶ����ģʽ���� 200102LX
	int TOEC_DLL_EXPORT RFGainMode(int Mode);

	//03��Ƶ����ģʽ��ѯ 200102LX
	int TOEC_DLL_EXPORT GetRFGainMode(PDEV_DevRF_GetGainMode_RESULT Reslut);

	//04��Ƶ����ֵ����ָ�� 200102LX
	int TOEC_DLL_EXPORT RFGainValue(int Value);
	
	//05��Ƶ����ģʽָ�� 200102LX
	int TOEC_DLL_EXPORT RFWokeMode(int Mode);

	//06��Ƶ״̬��Ϣ�ض� 200102LX
	int TOEC_DLL_EXPORT RFGetDevState(PDEV_RF_STATUS_RESULT Reslut);

	//07խ��196·����Ƶ�����ã��������� 200102LX
	int TOEC_DLL_EXPORT NBDDCFreqBand(int Index, int BandWidth, double Freq);

	//08���60·����Ƶ������ 200102LX
	int TOEC_DLL_EXPORT WBDDCFreqBand(int Index, double Freq);

	//09��ƵAGC��������
	int TOEC_DLL_EXPORT RFGainParameter(int UpLimit, int DownLimit, int HoldTime);
	
	//10����AGC�������� 200102LX
	int TOEC_DLL_EXPORT DigitalGainParameter(int UpLimit, int DownLimit, int HoldTime);

	//11ʱ���л�200102LX
	int TOEC_DLL_EXPORT SelectTimeDev(int Mode, PDEV_TIMEDEV_RESULT Reslut);

	//12GPS / BD���Ϳ���ָ��
	int TOEC_DLL_EXPORT SelectGPSBD(int Mode, PDEV_GPSBD_RESULT Reslut);
	
	//13��������24dB���ؿ���
	int TOEC_DLL_EXPORT DigitalGainSwitch(int WorkSwitch);

	//խ��AGC
	int TOEC_DLL_EXPORT NBDC_AGC(int WorkSwitch);

	//14ϵͳʱ��״̬��ѯ
	int TOEC_DLL_EXPORT DevCC_SampleClockCheckSelf(PDEV_CLKStatus_RESULT Reslut);

	//15Aurora�ӿ�channel_up����״̬
	int TOEC_DLL_EXPORT AuroraStatus(PDEV_CCAURORA_CHECKSELF_RESULT Reslut);

	//16FPGA�汾�Ų�ѯ
	int TOEC_DLL_EXPORT GetFPGAVersion(PDEV_GetFPGAVersion_CHECKSELF_RESULT Reslut);

	//16FPGA�汾�Ų�ѯ�����
	int TOEC_DLL_EXPORT GetFPGAVersionFC(PDEV_GetFPGAVersionFC_CHECKSELF_RESULT Reslut);

	//17��������ģʽ����
	int TOEC_DLL_EXPORT DigitalGainMode(int Mode);

	//18��������ģʽ��ѯ
	int TOEC_DLL_EXPORT GetCCGainMode(PDEV_GetDigitalGainMode_RESULT Reslut);

	//19����MGC˥��ֵ����
	int TOEC_DLL_EXPORT DigitalGainValue(int Value);

	//21խ��DDCʱ�꾫��
	int TOEC_DLL_EXPORT NBDDCAccuracy(int Accuracy);

	//22FFTʱ�꾫��
	int TOEC_DLL_EXPORT FFTAccuracy(int Accuracy);

	//23JGZCAGC����ֵ��ѯ
	int TOEC_DLL_EXPORT GetDigitalGainValueJGZC(int DevID, PDEV_GetDigitalGainValueJGZC_RESULT Reslut);
#pragma endregion

#pragma region ԭ2019��������ָ��

	//03Զ�̸�λָ��
	int TOEC_DLL_EXPORT ResetDev(int DevID);

	//06����״̬��ѯ	
	int TOEC_DLL_EXPORT DevCC_DevPowerCheckSelf(int DevID, PDEV_CHECKSELF_RESULT Reslut);

	//08��Ƶͨ��״̬��ѯ
	int TOEC_DLL_EXPORT CommunityStatus(int DevID, PDEV_CHECKSELF_RESULT Reslut);

	//12AD���ر���ָʾ
	void TOEC_DLL_EXPORT RegisterADOverloadCallback(Func1 AdCallBack);

	//13AGC����ֵ��ѯ
	int TOEC_DLL_EXPORT GetDigitalGainValue(int DevID, PDEV_GetDigitalGainValue_RESULT Reslut);

	//14���׹�ڲ�ѯָ��
	int TOEC_DLL_EXPORT GetWebStatus(int NetDevID, int NetNum, int TableNum, PDEV_CHECKSELF_RESULT Reslut);

	//15������
	void TOEC_DLL_EXPORT RegisterHeartBeatCallback(Func2 HeartBeatCallBack);
#pragma region ��������ģʽ����Ƶ����ģʽ
	int TOEC_DLL_EXPORT RFGainState(int DevId, int UpLimit, int DownLimit, int DischargeConst);
	int TOEC_DLL_EXPORT CCGainMode(int DevId, int Mode);
	int TOEC_DLL_EXPORT CCGainState(int DevId, int UpLimit, int DownLimit, int DischargeConst);
#pragma endregion

#pragma region �����豸״̬��ѯ
	int TOEC_DLL_EXPORT DevCC_DevPowerCheckSelf(int DevID, PDEV_CHECKSELF_RESULT Reslut);
	int TOEC_DLL_EXPORT CommunityStatus(int DevID, PDEV_CHECKSELF_RESULT Reslut);
	
#pragma endregion

#pragma region ��Ƶ����״̬��ѯ
	int TOEC_DLL_EXPORT DevRF_ConnectCheckSelf(int DevID, PDEV_CHECKSELF_RESULT Reslut);
	int TOEC_DLL_EXPORT WSGetDevState(int RF_DevID, PDEV_WS_STATUS_RESULT Reslut);
	int TOEC_DLL_EXPORT WSGetDevIdentity(int RF_DevID, PDEV_GS_IDENTITY_RESULT Reslut);
#pragma endregion

#pragma endregion

#pragma region 190814�����������鲥ip����ָ��

	//190814�����������鲥ip����ָ��
	int TOEC_DLL_EXPORT Testip();

#pragma endregion

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // !_SWDC_DEVDRIVER_CTRL_H_