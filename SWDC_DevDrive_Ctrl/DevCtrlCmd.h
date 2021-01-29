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


#pragma region ǧ������������

	/*
	*Function	:MakeCmd_SetDevIP
	*Description:ǧ�����豸��IP����;
	*Input		:ip--ip��ַ;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_SetDevIP(char *ip);

	/*
	*Function	:MakeCmd_SetLocalIP
	*Description:ǧ��������IP����;
	*Input		:ip--ip��ַ;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_SetLocalIP(char *ip);

	/*
	*Function	:MakeCmd_SetDevCCPort
	*Description:ǧ�����ɼ���ͨ�Ŷ˿�����;
	*Input		:port--�˿ں�;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_SetDevCCPort(unsigned short port);

	/*
	*Function	:MakeCmd_SetDevFCPort
	*Description:ǧ���������ͨ�Ŷ˿�����;
	*Input		:port--�˿ں�;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_SetDevFCPort(unsigned short port);
#pragma endregion

#pragma region ���������ݴ�������

	/*
	*Function	:MakeCmd_DataSynchronization
	*Description:����������ͬ������λ��;
	*Input		:boardNum --�������忨��   ȡֵ��Χ[0-1]��0Ϊ���壬1Ϊ�Ӱ� 
				:enable -- 1��0���룬��1 -> 0��ɸ�λ������
				:dataSource--����Դ  ȡֵ��Χ[0-1]��1����Ĭ�ϣ�GTX��������  0����������
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��Ҫenable����1�ٵ�0;
	*/
	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_DataReset(int boardNum, int enable, int dataSource = 1 );

	/*
	*Function	:MakeCmd_SetDataDestIP
	*Description:����������Ŀ�Ķ�����;
	*Input		:NetID--��������ں�   ȡֵ��Χ[1-8]��1-4Ϊ���壬5-8Ϊ�Ӱ�  connectionId--����������ڵ��������  ip--���ݴ���Ŀ�Ķ�IP��ַ      disPort--Ŀ�Ķ˿ں�
				 sourcePort--��Ӧ��·Դ�˿ں�
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetDataDestIP(int NetID, int connectionId, char *ip, unsigned short disPort, unsigned short sourcePort);

	/*
	*Function	:MakeCmd_SetDataSourceIP
	*Description:����������Դ������;
	*Input		:NetID--��������ں�   ȡֵ��Χ[1-8]��1-4Ϊ���壬5-8Ϊ�Ӱ�      ip--���ݴ���ԴIP��ַ   mac--���ݴ���Դ��MAC��ַ   
	            
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetDataSourceIP(int NetID, char *ip, char *mac, int PacketLen, int Interval, int SlicesClock);

	/*
	*Function	:MakeCmd_SetDataSendSource
	*Description:������������·ʹ��;
	*Input		:NetID--��������ں�   ȡֵ��Χ[1-8]��1-4Ϊ���壬5-8Ϊ�Ӱ�     connectionId--����������ڵ��������   en--ʹ�ܣ�1���� 0����
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetDataSendSource(int NetID, int connectionId, int en);

	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetDataAllEnable(int NetID,int en);

	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetMultiSelect(int NetID, int connectionId, int multi);

	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetMultiMac(int NetID, int connectionId, char *MultiMac);

#pragma endregion

//0702LX�������AGC���ֹ���
#pragma region �ɼ������

	/*
	*Function	:MakeCmd_ResetDev
	*Description:Զ�̸�λָ��;
	*Input		:DevID--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_ResetDev(int DevID);
	int MakeCmd_ResetDev_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_RFGainMode
	*Description:02��Ƶ����ģʽ����;
	*Input		:Mode-ģʽ[0,1]  0:MGC 1:AGC;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFGainMode(int Mode);
	int MakeCmd_RFGainMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_DevRF_GetGainMode
	*Description:03��Ƶ����ģʽ��ѯ;
	*Input		:Mode-ģʽ[0,1]  0:MGC 1:AGC;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevRF_GetGainMode();
	int MakeCmd_DevRF_GetGainMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_DevRF_GetGainMode_RESULT Reslut);
	/*
	*Function	:MakeCmd_RFGainValue
	*Description:04��Ƶģ������˥��ֵ����;
	*Input		:Value-˥��ֵ[0,31];
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFGainValue(int Value);
	int MakeCmd_RFGainValue_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_RFWokeMode
	*Description:05��Ƶģ�鹤��ģʽ����;
	*Input		:Mode-����ģʽ[0,1] 0:����  1:������;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFWokeMode(int Mode);
	int MakeCmd_RFWokeMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_RFGetDevState_Ret_Analysis
	*Description:06������Ƶģ���豸״̬��ѯ���;
	*Input		:RetData--�豸���յ����Լ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFGetDevState();
	int MakeCmd_RFGetDevState_Ret_Analysis(std::shared_ptr<unsigned char>  &&RetData, PDEV_RF_STATUS_RESULT Reslut);

	/*
	*Function	:MakeCmd_DevRF_GetGainMode
	*Description:07խ��196·����Ƶ�����ã���������;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_NBDDCFreqBand(int Index, int BandWidth, double Freq);
	int MakeCmd_NBDDCFreqBand_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int *Reslut);

	/*
	*Function	:MakeCmd_DevRF_GetGainMode
	*Description:08���60·����Ƶ������;
	*Input		:Mode-ģʽ[0,1]  0:MGC 1:AGC;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_WBDDCFreqBand(int Index, double Freq);
	int MakeCmd_WBDDCFreqBand_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int *Reslut);

	/*
	*Function	:MakeCmd_RFGainParameter
	*Description:09��ƵAGC��������;
	*Input		:UpLimit-AGC����;		DownLimit-AGC����		HoldTime-����ʱ��;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFGainParameter(int UpLimit, int DownLimit, int HoldTime);
	int MakeCmd_RFGainParameter_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);
	
	/*
	*Function	:MakeCmd_DigitalGainParameter
	*Description:10����AGC��������;
	*Input		:UpLimit-AGC����;		DownLimit-AGC����		HoldTime-����ʱ��;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DigitalGainParameter(int UpLimit, int DownLimit, int HoldTime);
	int MakeCmd_DigitalGainParameter_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_SelectTimeDev
	*Description:11ʱ�����Ϳ���ָ��;
	*Input		:Mode-ģʽ[0,1]  0:B�� 1:����;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_SelectTimeDev(int Mode);
	int MakeCmd_SelectTimeDev_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_TIMEDEV_RESULT Reslut);

	/*
	*Function	:MakeCmd_GPSBD
	*Description:12GPS/BD���Ϳ���ָ��;
	*Input		:Mode-ģʽ[0,1]  0:GPS 1:BD;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GPSBD(int Mode);
	int MakeCmd_GPSBD_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GPSBD_RESULT Reslut);

	/*
	*Function	:MakeCmd_DigitalGainSwitch
	*Description:13��������24dB���ؿ���;
	*Input		:WorkSwitch-��������[0,1]		0:�ر�24dB��������		1:����24dB��������;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DigitalGainSwitch(int Switch);
	int MakeCmd_DigitalGainSwitch_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_NBDC_AGC
	*Description:խ��AGC���ؿ���;
	*Input		:WorkSwitch-��������[0,1]		0:�ر�24dB��������		1:����24dB��������;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_NBDC_AGC(int WorkSwitch);
	int MakeCmd_NBDC_AGC_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_CLKStatus
	*Description:14ϵͳʱ��״̬��ѯ;
	*Input		:��;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_CLKStatus();
	/*
	*Function	:MakeCmd_MakeCmd_CLKStatus_Ret_Analysis
	*Description:14����ϵͳʱ��״̬��ѯ���;
	*Input		:RetData--�豸���յ��Ĳ�ѯ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_CLKStatus_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_CLKStatus_RESULT Reslut);

	/*
	*Function	:MakeCmd_AuroraStatus
	*Description:15�ɼ���aurora״̬��ѯ;
	*Input		:��;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_AuroraStatus();
	/*
	*Function	:MakeCmd_MakeCmd_AuroraStatus_Ret_Analysis
	*Description:15�����ɼ���aurora״̬��ѯ���;
	*Input		:RetData--�豸���յ��Ĳ�ѯ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_AuroraStatus_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_CCAURORA_CHECKSELF_RESULT Reslut);

	/*
	*Function	:MakeCmd_FPGAVersion2
	*Description:16FPGA�汾�Ų�ѯ�ɼ���;
	*Input		:
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GetFPGAVersionFC();
	/*
	*Function	:MakeCmd_AuroraStatus_Ret_Analysis
	*Description:16����FPGA�汾�Ų�ѯ��������;
	*Input		:
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_GetFPGAVersionFC_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetFPGAVersionFC_CHECKSELF_RESULT Reslut);

	/*
	*Function	:MakeCmd_GetFPGAVersion
	*Description:16FPGA�汾�Ų�ѯ;
	*Input		:
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GetFPGAVersion();
	/*
	*Function	:MakeCmd_GetFPGAVersion_Ret_Analysis
	*Description:16����FPGA�汾�Ų�ѯ���;
	*Input		:
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_GetFPGAVersion_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetFPGAVersion_CHECKSELF_RESULT Reslut);
	/*
	*Function	:MakeCmd_MakeCmd_AuroraStatus_Ret_Analysis
	*Description:16����FPGA�汾�Ų�ѯ���-8T;
	*Input		:
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_GetFPGAVersion8_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetFPGAVersion_CHECKSELF_RESULT Reslut);

	/*
	*Function	:MakeCmd_DigitalGainMode
	*Description:17��������ģʽ����;
	*Input		:Mode-ģʽ[0,1]  0:MGC 1:AGC;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DigitalGainMode(int Mode);
	int MakeCmd_DigitalGainMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_GetDigitalGainMode
	*Description:18��������ģʽ��ѯ;
	*Input		:��
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GetDigitalGainMode();
	/*
	*Function	:MakeCmd_PowerOnStatus_Ret_Analysis
	*Description:18������������ģʽ��ѯ���;
	*Input		:RetData--�豸���յ��Ĳ�ѯ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_GetDigitalGainMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainMode_RESULT Reslut);

	/*
	*Function	:MakeCmd_DigitalGainValue
	*Description:19����MGC˥��ֵ����;
	*Input		:Value-˥��ֵ[0,20];
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DigitalGainValue(int Value);
	int MakeCmd_DigitalGainValue_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_NBDDCAccuracy
	*Description://21խ��DDCʱ�꾫��;
	*Input		:Value-˥��ֵ[0,20];
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_NBDDCAccuracy(int Accuracy);
	int MakeCmd_NBDDCAccuracy_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_DigitalGainValue
	*Description:19����MGC˥��ֵ����;
	*Input		:Value-˥��ֵ[0,20];
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_FFTAccuracy(int Accuracy);
	int MakeCmd_FFTAccuracy_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_GetDigitalGainValueJGZC
	*Description:JGZCAGC����ֵ��ѯ;
	*Input		:��;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GetDigitalGainValueJGZC(int DevID);
	/*
	*Function	:MakeCmd_GetDigitalGainValue_Ret_Analysis
	*Description:����JGZCAGC����ֵ��ѯ���;
	*Input		:RetData--�豸���յ��Ĳ�ѯ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_GetDigitalGainValueJGZC_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValueJGZC_RESULT Reslut);

#pragma endregion

//0702LX�������AGC���ֹ���;0822LX���FPGA�汾�Ų�ѯ

#pragma region �豸�Լ�

	/*
	*Function	:MakeCmd_DevCC_FPGACheckSelf
	*Description:�ɼ���FPGA�����Լ�;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevCC_FPGACheckSelf();
	/*
	*Function	:MakeCmd_DevCC_FPGACheckSelf_Ret_Analysis
	*Description:�����ɼ���FPGA�����Լ���;
	*Input		:RetData--�豸���յ����Լ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_DevCC_FPGACheckSelf_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_FPGA_CHECKSELF_RESULT Reslut);

	/*
	*Function	:MakeCmd_DevCC_ADCheckSelf
	*Description:�ɼ���AD��·�Լ�;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevCC_ADCheckSelf(int DevID);
	/*
	*Function	:MakeCmd_DevCC_ADCheckSelf_Ret_Analysis
	*Description:�����ɼ���AD��·�Լ���;
	*Input		:RetData--�豸���յ����Լ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_DevCC_ADCheckSelf_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_AD_CHECKSELF_RESULT Reslut);

	/*
	*Function	:MakeCmd_DevRC_CheckSelf
	*Description:������Լ�;
	*Input		:��;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevRC_CheckSelf();
	/*
	*Function	:MakeCmd_DevRC_CheckSelf_Ret_Analysis
	*Description:����������Լ���;
	*Input		:RetData--�豸���յ����Լ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_DevRC_CheckSelf_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_RC_CHECKSELF_RESULT Reslut);
	/*
	*Function	:MakeCmd_DevRC_CheckSelf_Ret_Analysis8
	*Description:����������Լ���(8T);
	*Input		:RetData--�豸���յ����Լ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_DevRC_CheckSelf_Ret_Analysis8(std::shared_ptr<unsigned char> && RetData, PDEV_RC_CHECKSELF_RESULT Reslut);
	
	/*
	*Function	:MakeCmd_RFCommunityStatus
	*Description:��Ƶͨ��״̬��ѯ;
	*Input		:DevID--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;		Status-��Ƶͨ��״̬;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFCommunityStatus(int DevID);
	/*
	*Function	:MakeCmd_MakeCmd_RFCommunityStatus_Ret_Analysis
	*Description:������Ƶͨ��״̬��ѯ���;
	*Input		:RetData--�豸���յ��Ĳ�ѯ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_RFCommunityStatus_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_CHECKSELF_RESULT Reslut);

	/*
	*Function	:MakeCmd_GetDigitalGainValue
	*Description:AGC����ֵ��ѯ;
	*Input		:DevID--�ɼ��忨�� ȡֵ��Χ[1,5];
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GetDigitalGainValue(int DevID);
	/*
	*Function	:MakeCmd_GetDigitalGainValue_Ret_Analysis
	*Description:����AGC����ֵ��ѯ���;
	*Input		:RetData--�豸���յ��Ĳ�ѯ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_GetDigitalGainValue_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut);
	void MakeDigitalGain(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut, int startindex, int endindex);

	/*
	*Function	:MakeCmd_GetDigitalGainValue8
	*Description:AGC����ֵ��ѯ-8ͨ��;
	*Input		:DevID--�ɼ��忨�� ȡֵ��Χ[1,4];
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GetDigitalGainValue8(int DevID);
	/*
	*Function	:MakeCmd_GetDigitalGainValue_Ret_Analysis
	*Description:����AGC����ֵ��ѯ���-8ͨ��;
	*Input		:RetData--�豸���յ��Ĳ�ѯ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_GetDigitalGainValue8_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut);
	void MakeDigitalGain8(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut, int startindex, int endindex);

	/*
	*Function	:MakeCmd_GetDigitalGainValue18
	*Description:AGC����ֵ��ѯ-18ͨ��;
	*Input		:DevID--�ɼ��忨�� ȡֵ��Χ[1,4];
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GetDigitalGainValue18(int DevID);
	/*
	*Function	:MakeCmd_GetDigitalGainValue_Ret_Analysis
	*Description:����AGC����ֵ��ѯ���-18ͨ��;
	*Input		:RetData--�豸���յ��Ĳ�ѯ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_GetDigitalGainValue18_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut);
	void MakeDigitalGain18(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut, int startindex, int endindex);

	std::shared_ptr<unsigned char> MakeCmd_GetWebStatus(int NetDevID, int NetNum, int TableNum);
	int MakeCmd_GetWebStatus_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetWebStatus_CHECKSELF_RESULT Reslut);


	//********************************************************************end Lijf************************************************************************//
#pragma endregion

#pragma region FPGA��������

	/*
	*Function	:MakeCmd_DevRC_FPGAErasure
	*Description:������FPGA�������;
	*Input		:FlashID--�豸�� ȡֵ��Χ[1,4]  1 ��������FPGA1,2��������FPGA2     3����Ӱ�FPGA1��       4����Ӱ�FPGA2;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevRC_FPGAErasure(int FlashID);


	/*
	*Function	:MakeCmd_DevRC_FPGAWriteEnd
	*Description:������FPGA����д�����;
	*Input		:FlashID--�豸�� ȡֵ��Χ[1,4]  1 ��������FPGA1,2��������FPGA2     3����Ӱ�FPGA1��       4����Ӱ�FPGA2;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevRC_FPGAWriteEnd(int FlashID);

	/*
	*Function	:MakeCmd_DevRC_FPGALoad
	*Description:������FPGA�����FLASH����;
	*Input		:FlashID--�豸�� ȡֵ��Χ[1,4]  1 ��������FPGA1,2��������FPGA2     3����Ӱ�FPGA1��       4����Ӱ�FPGA2;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevRC_FPGALoad(int FlashID);

	/*
	*Function	:MakeCmd_DevRC_FPGAWriting
	*Description:������FPGA����д���;
	*Input		:FlashID--�豸�� ȡֵ��Χ[1,4]  1 ��������FPGA1,2��������FPGA2     3����Ӱ�FPGA1��       4����Ӱ�FPGA2;
	             index--FPGA��������        buffer--FPGA��������       filesize--�ļ���С
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevRC_FPGAWriting(int FlashID, int index, char* buffer,int filesize);

	/*
	*Function	:MakeCmd_DevRC_FPGAWriting_Ret_Analysis
	*Description:���������FPGA����ش���Ϣ;
	*Input		:RetData--�豸���յ����Լ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_DevRC_FPGAWriting_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_RCFPGA_LOAD_RESULT Reslut);

	/*
	*Function	:MakeCmd_DevCC_FPGAErasure
	*Description:�ɼ���FPGA�������;
	*Input		:DevID--�豸�� ȡֵ��Χ[1,5]  
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_DevCC_FPGAErasure(int DevID);
	/*
	*Function	:MakeCmd_DevCC_FPGAVersion
	*Description:�ɼ���FPGA����汾��;
	*Input		:DevID--�豸�� ȡֵ��Χ[1,5]   Version--����汾��
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/

	std::shared_ptr<unsigned char> MakeCmd_DevCC_FPGAVersion(int DevID,int Version);

	/*
	*Function	:MakeCmd_DevCC_FPGAWriting
	*Description:�ɼ���FPGA������д;
	*Input		:DevID--�豸�� ȡֵ��Χ[1,5]    index--FPGA��������       buffer--FPGA��������
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/

	std::shared_ptr<unsigned char> MakeCmd_DevCC_FPGAWriting(int DevID, int index, char* buffer);

	/*
	*Function	:MakeCmd_DevCC_FPGAVersion
	*Description:�ɼ���FPGA������д���;
	*Input		:DevID--�豸�� ȡֵ��Χ[1,5]
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/

	std::shared_ptr<unsigned char> MakeCmd_DevCC_FPGAWriteEnd(int DevID);

	/*
	*Function	:MakeCmd_DevRC_FPGA_Ret_Analysis
	*Description:�����ɼ���FPGA�����������ش���Ϣ;
	*Input		:RetData--�豸���յ����Լ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_DevCC_FPGA_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_CCFPGA_LOAD_RESULT Reslut);

#pragma endregion

#pragma region ��Ƶ��������
	std::shared_ptr<unsigned char> MakeCmd_RFFreqBand_X(int DevId, int Channel, int StartFreq, int EndFreq);
	std::shared_ptr<unsigned char> MakeCmd_RFAllParamConfig_X(int DevId, int Channel, std::shared_ptr<RF_PARAM> Param);
	std::shared_ptr<unsigned char> MakeCmd_CSFreqBand_X(int DevId, int StartFreq, int EndFreq);
	std::shared_ptr<unsigned char> MakeCmd_CSAllParamConfig_X(int DevId, std::shared_ptr<CS_PARAM> Param);
	std::shared_ptr<unsigned char> MakeCmd_WSWholeStatus_X(int DevId, std::shared_ptr<DEV_PARAM>  Param);

#pragma endregion

#pragma region ���������ݴ��䱸��
	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_DataReset_B(int boardNum, int enable, int dataSource = 1);
	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetDataDestIP_B(int BoardID, int NetID, int connectionId, char *ip, unsigned short disPort, unsigned short sourcePort);
	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetDataSourceIP_B(int BoardID, int NetID, char *ip, char *mac, int PacketLen, int Interval, int SlicesClock);
	std::pair<std::shared_ptr<unsigned char>, int > MakeCmd_SetDataSendSource_B(int BoardID, int NetID, int connectionId, int en);

#pragma endregion
	
#pragma region ��Ƶ����
	std::shared_ptr<unsigned char> MakeCmd_RFFreqBand(int RF_DevID, int DevId, int Channel, int StartFreq, int EndFreq);
	std::shared_ptr<unsigned char> MakeCmd_RFFreqBand8(int RF_DevID, int DevId, int Channel, int StartFreq, int EndFreq);
	int MakeCmd_RFFreqBand_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	std::shared_ptr<unsigned char> MakeCmd_CSFreqBand(int RF_DevID, int DevId, int StartFreq, int EndFreq);
	int MakeCmd_CSFreqBand_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_RFPowerSwitch
	*Description:��Ƶģ���Դ��������;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;	Channel-���Ƶ�Ŀ��ͨ��[0,19];	 PowerSwitch-��Դ����״̬[0,4];
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFPowerSwitch(int RF_DevID, int DevId, int Channel, int PowerSwitch);
	std::shared_ptr<unsigned char> MakeCmd_RFPowerSwitch8(int RF_DevID, int DevId, int Channel, int PowerSwitch);
	int MakeCmd_RFPowerSwitch_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_RFAllParamConfig
	*Description:��Ƶģ��ȫ����������;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;	Channel-���Ƶ�Ŀ��ͨ��[0,19];	 Param-ȫ״̬�����֣��ṹ�嶨������;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
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
	*Description:У��Դģ������ֵ����;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;	 Value-����ֵ[0,62];
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_CSGainValue(int RF_DevID, int DevId, int Value);
	int MakeCmd_CSGainValue_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);


	/*
	*Function	:MakeCmd_CSWorkMode
	*Description:У��Դģ�鹤��ģʽ����;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;	 Mode-����ֵ[0,1]  0���ض�  1 ��;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_CSWorkMode(int RF_DevID, int DevId, int Mode);
	int MakeCmd_CSWorkMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_CSPowerSwitch
	*Description:У��Դģ���Դ��������;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;	 PowerSwitch-��Դ����״̬[0,4];
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_CSPowerSwitch(int RF_DevID, int DevId, int PowerSwitch);
	int MakeCmd_CSPowerSwitch_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_CSAllParamConfig
	*Description:У��Դģ��ȫ��������;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;	 Param-ȫ�����ṹ��ָ��  ;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_CSAllParamConfig(int RF_DevID, int DevId, std::shared_ptr<CS_PARAM> Param);
	int MakeCmd_CSAllParamConfig_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_GSWorkMode
	*Description:���򿪹�ģ�鹤��ģʽ����;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;	 Mode-����ģʽ[0,1] 0:У��״̬  1������״̬  ;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GSWorkMode(int RF_DevID, int DevId, int Mode);
	int MakeCmd_GSWorkMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_GSPowerSwitch
	*Description:���򿪹ؿ���״̬����;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;	 PowerSwitch-����ģʽ[0,1] 0:�ضϵ�Դ  1���򿪵�Դ  ;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GSPowerSwitch(int RF_DevID, int DevId, int PowerSwitch);
	int MakeCmd_GSPowerSwitch_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_WSWholeStatus
	*Description:����״̬����;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;	 Param-ȫ�����ṹ��ָ��  ;
	*Output		:��;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_WSWholeStatusZC(int RF_DevID, int DevId, std::shared_ptr<DEV_PARAM> Param);
	std::shared_ptr<unsigned char> MakeCmd_WSWholeStatusZC18(int RF_DevID, int DevId, std::shared_ptr<DEV_PARAM> Param);
	std::shared_ptr<unsigned char> MakeCmd_WSWholeStatus(int RF_DevID, int DevId, std::shared_ptr<DEV_PARAM> Param);
	int MakeCmd_WSWholeStatus_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut);

	/*
	*Function	:MakeCmd_RFGetDevIdentity
	*Description:��Ƶģ����ݲ�ѯ;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;   Channel-���Ƶ�Ŀ��ͨ��[0,19];
	*Output		:�� ;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFGetDevIdentity(int RF_DevID, int DevID, int Channel);
	std::shared_ptr<unsigned char> MakeCmd_RFGetDevIdentity8(int RF_DevID, int DevID, int Channel);

	/*
	*Function	:MakeCmd_RFGetDevIdentity_Ret_Analysis
	*Description:������Ƶģ����ݲ�ѯ���;
	*Input		:RetData--�豸���յ����Լ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_RFGetDevIdentity_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_RF_IDENTITY_RESULT Reslut);

	/*
	*Function	:MakeCmd_CSGetDevIdentity
	*Description:У��Դģ����ݲ�ѯ;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;
	*Output		:�� ;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_CSGetDevIdentity_Z(int RF_DevID, int DevID);

	/*
	*Function	:MakeCmd_CSGetDevIdentity_Ret_Analysis
	*Description:����У��Դģ����ݲ�ѯ���;
	*Input		:RetData--�豸���յ����Լ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_CSGetDevIdentity_Ret_Analysis_Z(std::shared_ptr<unsigned char> && RetData, PDEV_CS_IDENTITY_RESULT Reslut);

	/*
	*Function	:MakeCmd_GSGetDevIdentity
	*Description:���򿪹���ݲ�ѯ;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;
	*Output		:�� ;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GSGetDevIdentity_Z(int RF_DevID, int DevID);

	/*
	*Function	:MakeCmd_GSGetDevIdentity_Ret_Analysis
	*Description:�������򿪹���ݲ�ѯ���;
	*Input		:RetData--�豸���յ����Լ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_GSGetDevIdentity_Ret_Analysis_Z(std::shared_ptr<unsigned char> && RetData, PDEV_GS_IDENTITY_RESULT Reslut);

#pragma endregion
	
#pragma region ��ƵЭ��V1.6.1

#pragma region ��Ƶ��ѯ

	int MakeCmd_RFGetDevState_Ret_Analysis_V161(std::shared_ptr<unsigned char>  &&RetData, PDEV_RF_STATUS_RESULT Reslut);
	int MakeCmd_CSGetDevState_Ret_Analysis_V161(std::shared_ptr<unsigned char> && RetData, PDEV_CS_STATUS_RESULT Reslut);
	int MakeCmd_GSGetDevState_Ret_Analysis_V161(std::shared_ptr<unsigned char> && RetData, PDEV_GS_STATUS_RESULT Reslut);
#pragma endregion
#pragma endregion

#pragma region ��Ƶǰ��״̬��ѯV1.3

	/*
	*Function	:MakeCmd_CSGetDevState
	*Description:У��Դģ���豸״̬��ѯ;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;
	*Output		:�� ;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_CSGetDevState(int DevID);

	/*
	*Function	:MakeCmd_CSGetDevState_Ret_Analysis
	*Description:����У��Դģ���豸״̬��ѯ���;
	*Input		:RetData--�豸���յ����Լ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_CSGetDevState_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_CS_STATUS_RESULT Reslut);

	/*
	*Function	:MakeCmd_GSGetDevState
	*Description:���򿪹�ģ���豸״̬��ѯ;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;
	*Output		:�� ;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GSGetDevState(int DevID);

	/*
	*Function	:MakeCmd_GSGetDevState_Ret_Analysis
	*Description:�������򿪹�ģ���豸״̬��ѯ���;
	*Input		:RetData--�豸���յ����Լ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_GSGetDevState_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GS_STATUS_RESULT Reslut);


#pragma endregion

#pragma region ��Ƶ��ѯV1.6.2
	/*
	*Function	:MakeCmd_RFGetDevState
	*Description:��Ƶģ���豸״̬��ѯ;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;	Channel-���Ƶ�Ŀ��ͨ��[0,19];
	*Output		:�� ;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_RFGetDevState_Z(int RF_DevID, int DevID, int Channel);
	std::shared_ptr<unsigned char> MakeCmd_RFGetDevState_Z8(int RF_DevID, int DevID, int Channel);

	/*
	*Function	:MakeCmd_RFGetDevState_Ret_Analysis
	*Description:������Ƶģ���豸״̬��ѯ���;
	*Input		:RetData--�豸���յ����Լ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_RFGetDevState_Ret_Analysis_Z(std::shared_ptr<unsigned char>  &&RetData, PDEV_RF_STATUS_RESULT Reslut);

	/*
	*Function	:MakeCmd_CSGetDevState
	*Description:У��Դģ���豸״̬��ѯ;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;
	*Output		:�� ;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_CSGetDevState_Z(int RF_DevID, int DevID);

	/*
	*Function	:MakeCmd_CSGetDevState_Ret_Analysis
	*Description:����У��Դģ���豸״̬��ѯ���;
	*Input		:RetData--�豸���յ����Լ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_CSGetDevState_Ret_Analysis_Z(std::shared_ptr<unsigned char> && RetData, PDEV_CS_STATUS_RESULT Reslut);

	/*
	*Function	:MakeCmd_GSGetDevState
	*Description:���򿪹�ģ���豸״̬��ѯ;
	*Input		:DevId--�ɼ��忨�� ȡֵ��Χ[0,5]  0 �������а忨;
	*Output		:�� ;
	*Return		:�����ɺ��ڴ�;
	*Other		:��;
	*/
	std::shared_ptr<unsigned char> MakeCmd_GSGetDevState_Z(int RF_DevID, int DevID);

	/*
	*Function	:MakeCmd_GSGetDevState_Ret_Analysis
	*Description:�������򿪹�ģ���豸״̬��ѯ���;
	*Input		:RetData--�豸���յ����Լ���buffer;
	*Output		:Reslut-���������ṹ�� ;
	*Return		:0 ��������ɹ��� -1 �������������Э��;
	*Other		:��;
	*/
	int MakeCmd_GSGetDevState_Ret_Analysis_Z(std::shared_ptr<unsigned char> && RetData, PDEV_GS_STATUS_RESULT Reslut);



#pragma endregion

#pragma region 190814�����������鲥ip����ָ��

	std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_SetTestip();

#pragma endregion

private:
	std::shared_ptr<unsigned char> m_CtrlCmd;
	std::shared_ptr<unsigned char> m_CtrlCmd_CCFPGA;
	std::shared_ptr<unsigned char> m_CtrlCmd_RCFPGA;

	static int const m_ProtocolSize = 128; //������ Э���ܳ���;
	static int const m_CCFPGASize = 1032; //�ɼ���FPGA���ݰ�����;
	static int const m_RCFPGASize = 1056; //������FPGA���ݰ�����;

	std::vector<std::pair<unsigned long, unsigned long>> m_FreqBandInfo; //Ƶ����Ϣ��
	std::vector<std::pair<unsigned long, unsigned long>> m_FreqBandInfoZC; //Ƶ����Ϣ��
	std::vector<std::pair<unsigned long, unsigned long>> m_FreqBandInfoZC18; //Ƶ����Ϣ��

};

