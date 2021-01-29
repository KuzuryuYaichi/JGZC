#ifndef _DEFINETYPE_H_
#define _DEFINETYPE_H_

#pragma region ������ö��
enum RET_TYPE
{
	CODE_OK = 0,
	CODE_ERROR_ConfigError = -100,
	CODE_ERROR_NetError = -101,
	CODE_ERROR_ProclError = -102,
	CODE_TIMEOUT = -103,
	CODE_ERROR_ErasureError = -104,
	CODE_ERROR_WriteError = -105,
	CODE_ERROR_LoadError = -106,
	CODE_ERROR_Version = -107
};

#pragma endregion

#pragma region �¶Ȼص�����ָ��

typedef int(__stdcall *Func)(float Value);

#pragma endregion

#pragma region ��Ƶ�ṹ��
//RFȫ����
typedef struct _RF_PARAM
{
	int FreqBand;
	int StartFreq;
	int EndFreq;
	int GainVlaue;
	int WorkMode; //0 ����  1 ������
	int PowerSwitch;//0 �رյ�Դ   1 �򿪵�Դ

}*PRF_PARAM, RF_PARAM;
//CSȫ����
typedef struct _CS_PARAM
{
	int FreqBand;
	int StartFreq;
	int EndFreq;
	int GainVlaue;
	int WorkMode; //0 �ض�У��Դ���  1 ��У��Դ���
	int PowerSwitch;//0 �رյ�Դ   1 �򿪵�Դ

}*PCS_PARAM, CS_PARAM;

//********************************************************************Lijf*************************************************************//
//����ȫ����
typedef struct _DEV_PARAM
{
	int FreqBand;
	int StartFreq;
	int EndFreq;
	int GainValue; //----��Ƶģ���У��Դģ��˥��ֵ
	int RF_WorkMode; //0 ����                 1 ������
	int CS_WorkMode; //0 �ض�У��Դ���       1 ����У��Դ��� 
	int GS_WorkMode; //0 У��״̬             1 �����ź�״̬
	int RF_PowerSwitch;//0 �رյ�Դ   1 �򿪵�Դ
	int CS_PowerSwitch;//0 �رյ�Դ   1 �򿪵�Դ
	int GS_PowerSwitch;//0 �رյ�Դ   1 �򿪵�Դ

}*PDEV_PARAM, DEV_PARAM;


//��Ƶģ���豸״̬��ѯ���
typedef struct _DEV_RF_STATUS_RESULT
{
	int RF_GainValue;//��Ƶ˥��ֵ
	int RF_WorkMode; //0���� 1������
	//int	RF_PowerStatus; //0 �ر�  1 ��Դ��
	//int RF_WorkingVoltage;	//mv
	//int RF_WorkingCurrent;  //mA
	//int RF_Channel;
	float RF_Temperature;    //���϶� 45.7 == 4570
}*PDEV_RF_STATUS_RESULT, DEV_RF_STATUS_RESULT;

//У��Դģ���豸״̬��ѯ���
typedef struct _DEV_CS_STATUS_RESULT
{
	int FreqBandStart;//10KHz
	int FreqBandEnd;//10KHz
	int CS_GainValue;
	int CS_WorkMode;//0У׼�źŹ� 1У׼�źſ�
	int	CS_PowerStatus;//0 �ر�  1 ��Դ��
	int CS_WorkingVoltage;//mv
	int CS_WorkingCurrent;//mA
	float CS_Temperature;//���϶�

}*PDEV_CS_STATUS_RESULT, DEV_CS_STATUS_RESULT;

//���򿪹�ģ���豸״̬��ѯ���
typedef struct _DEV_GS_STATUS_RESULT
{
	int GS_WorkMode; // 0 У��״̬  1 ����״̬
	int	GS_PowerStatus;//0 �ر�  1 ��Դ��
	int GS_WorkingVoltage;
	int GS_WorkingCurrent;
	float GS_Temperature;

}*PDEV_GS_STATUS_RESULT, DEV_GS_STATUS_RESULT;


//��ƵԤ����ģ����ݲ�ѯ���
typedef struct _DEV_RF_IDENTITY_RESULT
{
	float RF_BoardVer;   //�ذ�̼��汾��  ��V10.01 (b1010 0001)
	float RF_Ver;        //�̼��汾��  ��V10.01 (b1010 0001)
	int RF_MaxWorkingFreq;  //�����������ʱ��Ƶ����,MHz
	int RF_ComName;    //��˾������  00000000Ϊ��ʮ������00000001Ϊ������裬00000010Ϊ�ɶ����ǣ�00000011Ϊ�ɶ����ң�00000100λ���754
	int RF_ModuleType; //ģ��������
	int RF_BoughtIndex;//ģ���깺�����
	int RF_Addr;       //ģ���ַ��

}*PDEV_RF_IDENTITY_RESULT, DEV_RF_IDENTITY_RESULT;

//У��Դģ����ݲ�ѯ���
typedef struct _DEV_CS_IDENTITY_RESULT
{
	float CS_BoardVer;   //�ذ�̼��汾��  ��V10.01 (b1010 0001)
	float CS_Ver;        //�̼��汾��  ��V10.01 (b1010 0001)
	int CS_MaxWorkingFreq;  //�����������ʱ��Ƶ����,MHz
	int CS_ComName;    //��˾������  00000000Ϊ��ʮ������00000001Ϊ������裬00000010Ϊ�ɶ����ǣ�00000011Ϊ�ɶ����ң�00000100λ���754
	int CS_ModuleType; //ģ��������
	int CS_BoughtIndex;//ģ���깺�����

}*PDEV_CS_IDENTITY_RESULT, DEV_CS_IDENTITY_RESULT;

//���򿪹�ģ����ݲ�ѯ���
typedef struct _DEV_GS_IDENTITY_RESULT
{
	float GS_BoardVer;   //�ذ�̼��汾��  ��V10.01 (b1010 0001)
	float GS_Ver;        //�̼��汾��  ��V10.01 (b1010 0001)
	int GS_MaxWorkingFreq;  //�����������ʱ��Ƶ����,MHz
	int GS_ComName;    //��˾������  00000000Ϊ��ʮ������00000001Ϊ������裬00000010Ϊ�ɶ����ǣ�00000011Ϊ�ɶ����ң�00000100λ���754
	int GS_ModuleType; //ģ��������
	int GS_BoughtIndex;//ģ���깺�����

}*PDEV_GS_IDENTITY_RESULT, DEV_GS_IDENTITY_RESULT;

#pragma endregion

#pragma region ����ģʽ�ṹ��

//����ģʽ���ؽ��
typedef struct _DEV_DevRF_GetGainMode_RESULT
{
	int Mode; //1 AGC״̬ 0 MGC״̬
}*PDEV_DevRF_GetGainMode_RESULT, DEV_DevRF_GetGainMode_RESULT;

#pragma endregion

#pragma region �豸�Լ�ṹ��

typedef struct _DEV_CHECKSELF_RESULT
{
	int DevID;
	int Status; //1 ������ 0 �쳣
}*PDEV_CHECKSELF_RESULT, DEV_CHECKSELF_RESULT;

//�ɼ���FPGA�Լ췵�ؽ��
typedef struct _DEV_FPGA_CHECKSELF_RESULT
{
	int Status1; //1 FPGA���������� 0 FPGA����ʧ��
	float Temperature; //�ɼ����¶�

}*PDEV_FPGA_CHECKSELF_RESULT, DEV_FPGA_CHECKSELF_RESULT;

//�ɼ���AD��·�Լ췵�ؽ��
typedef struct _DEV_AD_CHECKSELF_RESULT
{
	int DevID;
	int Status; //1 AD������ 0 ADʧ��
}*PDEV_AD_CHECKSELF_RESULT, DEV_AD_CHECKSELF_RESULT;

//������������Լ췵�ؽ��
typedef struct _DEV_RC_CHECKSELF_RESULT
{
	//int DevRCMain;//���������״̬     1 �ɹ�       0 ʧ��        
	int DevRCVice;//�����Ӱ�״̬     1 �ɹ�       0 ʧ��                      Status[0]
	int DevRCNet1;//���������1����������״̬     1 �ɹ�       0 ʧ��           Status[1]
	int DevRCNet2;//���������2����������״̬     1 �ɹ�       0 ʧ��           Status[2]
	int DevRCNet3;//���������3����������״̬     1 �ɹ�       0 ʧ��           Status[3]
	int DevRCNet4;//���������4����������״̬     1 �ɹ�       0 ʧ��           Status[4]
	//int DevRCNet5;//�����Ӱ�1����������״̬     1 �ɹ�       0 ʧ��           Status[7]
	//int DevRCNet6;//�����Ӱ�2����������״̬     1 �ɹ�       0 ʧ��           Status[8]
	//int DevRCNet7;//�����Ӱ�3����������״̬     1 �ɹ�       0 ʧ��           Status[9]
	//int DevRCNet8;//�����Ӱ�4����������״̬     1 �ɹ�       0 ʧ��           Status[10]
	int DevRCFPGA1;//���������1��FPGA����������     1 �ɹ�       0 ʧ��      Status[5]
	int DevRCFPGA2;//���������2��FPGA����������     1 �ɹ�       0 ʧ��      Status[6]
	//int DevRCFPGA3;//�����Ӱ�1��FPGA����������     1 �ɹ�       0 ʧ��      Status[11]
	//int DevRCFPGA4;//�����Ӱ�2��FPGA����������     1 �ɹ�       0 ʧ��      Status[12]
	//char Status[13];

}*PDEV_RC_CHECKSELF_RESULT, DEV_RC_CHECKSELF_RESULT;

#pragma endregion

#pragma region ʱ�����ͽṹ��
typedef struct _DEV_TIMEDEV_RESULT
{
	int Mode; //1 FPGA���������� 0 FPGA����ʧ��
}*PDEV_TIMEDEV_RESULT, DEV_TIMEDEV_RESULT;

#pragma endregion

#pragma region GPS/BD�ṹ��
typedef struct _DEV_GPSBD_RESULT
{
	int Mode; //1 FPGA���������� 0 FPGA����ʧ��
}*PDEV_GPSBD_RESULT, DEV_GPSBD_RESULT;

#pragma endregion

#pragma region ʱ��״̬�ṹ��
typedef struct _DEV_CLKStatus_RESULT
{
	int Status; //1 ���ⲿʱ�����룬 0 ���ⲿʱ������
}*PDEV_CLKStatus_RESULT, DEV_CLKStatus_RESULT;

#pragma endregion

#pragma region JGZCAGC����ṹ��
typedef struct _GetDigitalGainValueJGZC_RESULT
{

	int DigitalGain[60];//0428JGZCAGC����

}*PDEV_GetDigitalGainValueJGZC_RESULT, DEV_GetDigitalGainValueJGZC_RESULT;

#pragma endregion

#pragma region �ɼ���aurora״̬��ѯ����ṹ��
typedef struct _DEV_CCAURORA_CHECKSELF_RESULT
{
	int AuroraX1; //1 ������ 0 �쳣
	int AuroraX4_1; //1 ������ 0 �쳣
	int AuroraX4_2; //1 ������ 0 �쳣

}*PDEV_CCAURORA_CHECKSELF_RESULT, DEV_CCAURORA_CHECKSELF_RESULT;
#pragma endregion

#pragma region 18��������ģʽ��ѯ���ؽ��
typedef struct _DEV_GetDigitalGainMode_RESULT
{
	int Mode; //1 ����AGC�� 0 ����MGC
}*PDEV_GetDigitalGainMode_RESULT, DEV_GetDigitalGainMode_RESULT;
#pragma endregion

#pragma region FPGA������ؽṹ��

//�����FPGA���ط��ؽ��
typedef struct _DEV_RCFPGA_LOAD_RESULT
{
	int Status; //1 FPGA�����ɹ��� 3 FPGAд��ɹ���   4 FPGA���سɹ�     0 ʧ��
}*PDEV_RCFPGA_LOAD_RESULT, DEV_RCFPGA_LOAD_RESULT;

//�ɼ���FPGA���ط��ؽ��
typedef struct _DEV_CCFPGA_LOAD_RESULT
{
	int Status; //1 FPGA�����ɹ��� 1 FPGAд��ɹ���   
}*PDEV_CCFPGA_LOAD_RESULT, DEV_CCFPGA_LOAD_RESULT;

#pragma endregion

#pragma region �������ṹ��

typedef struct _DEV_HEARTBEAT_RESULT
{
	int DevID;//   �豸���
	int CC_Switch_State[5];//���ֽ��ջ�����״̬
	int CC_Clock_State[5]; //ϵͳʱ��״̬
	float CC_Temperature_State[5];//���ֽ��ջ��¶�ֵ
	int CC_FPGA_State[5];//FPGAоƬ״̬
	int RC_Net_State[5];//�����״̬
	int RF_Module_State[5];//��Ƶͨ��ģ��״̬
	int GS_WorkMode[5]; // 0 У��״̬  1 ����״̬
	int	GS_Status[5];//0 �ر�  1 ��Դ��
	float RF_Temperature_State[5];//��Ƶ�����¶�

}*PDEV_HEARTBEAT_RESULT, DEV_HEARTBEAT_RESULT;

#pragma endregion

#pragma region 2019������

//��Ƶ�豸״̬��ѯ���
typedef struct _DEV_WS_STATUS_RESULT
{
	int CS_FreqBandStart;//10KHz
	int CS_FreqBandEnd;//10KHz
	int CS_GainValue;
	int CS_WorkMode;//0У׼�źŹ� 1У׼�źſ�
	int	CS_PowerStatus;//0 �ر�  1 ��Դ��
	int GS_WorkMode; // 0 У��״̬  1 ����״̬
	int	GS_PowerStatus;//0 �ر�  1 ��Դ��
	int WorkingVoltage;//mv
	int WorkingCurrent;//mA
	float Temperature;//���϶�

	int RFState1;
	int RFState2;
	int RFState3;
	int RFState4;
	int RFState5;
	int RFState6;
	int RFState7;
	int RFState8;
	int RFState9;
	int RFState0;

}*PDEV_WS_STATUS_RESULT, DEV_WS_STATUS_RESULT;

//AD���ر����ṹ��
typedef struct _DEV_ADOverloadAlarm_RESULT
{
	int DevID;//�ɼ��忨��
	int alarm[20];

}*PDEV_ADOverloadAlarm_RESULT, DEV_ADOverloadAlarm_RESULT;

//��Ƶ�豸״̬��ѯ���
typedef struct _GetDigitalGainValue_RESULT
{
	int DevID;//�忨��

	int RFChannelGain[20];//��Ƶͨ����������

	float PathGain;//��·����

	int DigitalGain[32][30];//����AGC��������//0923�޸Ķ�ά�����С�Լ���8T��18T//0424�޸������С�Լ���JGZC
	
}*PDEV_GetDigitalGainValue_RESULT, DEV_GetDigitalGainValue_RESULT;

//�������Ĵ���״̬��ѯ���
typedef struct _DEV_GetWebStatus_CHECKSELF_RESULT
{
	int DevID;
	int Allsituation[8]; //1 ������ 0 �쳣
	int IPAddr_UDPport[20]; //1 ������ 0 �쳣
	int AuroraX4_2; //1 ������ 0 �쳣

}*PDEV_GetWebStatus_CHECKSELF_RESULT, DEV_GetWebStatus_CHECKSELF_RESULT;

//FPGA�汾��ѯ���
typedef struct _DEV_GetFPGAVersion_CHECKSELF_RESULT
{
	//int DevID;//�忨��
	int manufacturers;//���ҹ̶�Ϊ754
	//int ModuleType; //ģ�����ͣ������ֽ�λ���Զ�ʶ��
	int sampleK7_Type[5];//�ɼ���K7�豸�ͺ�
	int sampleK7_integer[5];//�ɼ���K7�汾������
	int sampleK7_decimal[5];//�ɼ���K7�汾��С��

	int sampleV7_Type[5];//�ɼ���V7�豸�ͺ�
	int sampleV7_integer[5];//�ɼ���V7�汾������
	int sampleV7_decimal[5];//�ɼ���V7�汾��С��


	int netK7_Type[2];//�����K7�豸�ͺ�
	int netK7_integer[2];//�����K7�汾������
	int netK7_decimal[2];//�����V7�汾��С��

	int netF1_Type[2];//�����F1�豸�ͺ�
	int netF1_integer[2];//�����F1�汾������
	int netF1_decimal[2];//�����F1�汾��С��

	int netF2_Type[2];//�����F2�豸�ͺ�
	int netF2_integer[2];//�����F2�汾������
	int netF2_decimal[2];//�����F2�汾��С��

}*PDEV_GetFPGAVersion_CHECKSELF_RESULT, DEV_GetFPGAVersion_CHECKSELF_RESULT;

//FPGA�汾��ѯ��������
typedef struct _DEV_GetFPGAVersionFC_CHECKSELF_RESULT
{
	//int DevID;//�忨��
	int manufacturers;//���ҹ̶�Ϊ754
	//int ModuleType; //ģ�����ͣ������ֽ�λ���Զ�ʶ��
	int sampleK7_Type[5];//�ɼ���K7�豸�ͺ�
	int sampleK7_integer[5];//�ɼ���K7�汾������
	int sampleK7_decimal[5];//�ɼ���K7�汾��С��

	int sampleV7_Type[5];//�ɼ���V7�豸�ͺ�
	int sampleV7_integer[5];//�ɼ���V7�汾������
	int sampleV7_decimal[5];//�ɼ���V7�汾��С��


	int netK7_Type[2];//�����K7�豸�ͺ�
	int netK7_integer[2];//�����K7�汾������
	int netK7_decimal[2];//�����V7�汾��С��

	int netF1_Type[2];//�����F1�豸�ͺ�
	int netF1_integer[2];//�����F1�汾������
	int netF1_decimal[2];//�����F1�汾��С��

	int netF2_Type[2];//�����F2�豸�ͺ�
	int netF2_integer[2];//�����F2�汾������
	int netF2_decimal[2];//�����F2�汾��С��

}*PDEV_GetFPGAVersionFC_CHECKSELF_RESULT, DEV_GetFPGAVersionFC_CHECKSELF_RESULT;

#pragma region AD���ػص�����ָ��

typedef void(_stdcall *Func1)(_DEV_ADOverloadAlarm_RESULT Result);

#pragma endregion

#pragma region �������ص�����ָ��

typedef void(_stdcall *Func2)(_DEV_HEARTBEAT_RESULT heart);

#pragma endregion


//190702�ظ����
		//����״̬��ѯ���
//	typedef struct _PDEV_PowerOnStatus_RESULT
//		{
//			int Status;//1:������0��δ�������忨����������
//
//		}*PDEV_PowerOnStatus_RESULT, DEV_PowerOnStatus_RESULT;
//
//		//ϵͳʱ��״̬��ѯ���
//		typedef struct _PDEV_CLKStatus_RESULT
//		{
//			int Status;//1:������0��δ�������忨����������
//
//		}*PDEV_CLKStatus_RESULT, DEV_CLKStatus_RESULT;
//
//		//��Ƶͨ��״̬��ѯ���
//		typedef struct _PDEV_RFCommunityStatus_RESULT
//		{
//			int Status;//1:������0��δ�������忨����������
//
//		}*PDEV_RFCommunityStatus_RESULT, DEV_RFCommunityStatus_RESULT;
//
//		//�ɼ���aurora״̬��ѯ���
//		typedef struct _PDEV_AuroraStatus_RESULT
//		{
//			int AuroraX1;//1��ͨ��������0��ͨ���쳣
//			int AuroraX4_1;//1��ͨ��������0��ͨ���쳣
//			int AuroraX4_2;//1��ͨ��������0��ͨ���쳣
//
//		}*PDEV_AuroraStatus_RESULT, DEV_AuroraStatus_RESULT;
#pragma endregion

#endif