#ifndef _DEFINETYPE_H_
#define _DEFINETYPE_H_

#pragma region 错误码枚举
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

#pragma region 温度回调函数指针

typedef int(__stdcall *Func)(float Value);

#pragma endregion

#pragma region 射频结构体
//RF全参数
typedef struct _RF_PARAM
{
	int FreqBand;
	int StartFreq;
	int EndFreq;
	int GainVlaue;
	int WorkMode; //0 常规  1 低噪声
	int PowerSwitch;//0 关闭电源   1 打开电源

}*PRF_PARAM, RF_PARAM;
//CS全参数
typedef struct _CS_PARAM
{
	int FreqBand;
	int StartFreq;
	int EndFreq;
	int GainVlaue;
	int WorkMode; //0 关断校正源输出  1 打开校正源输出
	int PowerSwitch;//0 关闭电源   1 打开电源

}*PCS_PARAM, CS_PARAM;

//********************************************************************Lijf*************************************************************//
//整机全参数
typedef struct _DEV_PARAM
{
	int FreqBand;
	int StartFreq;
	int EndFreq;
	int GainValue; //----射频模块和校正源模块衰减值
	int RF_WorkMode; //0 常规                 1 低噪声
	int CS_WorkMode; //0 关断校正源输出       1 开启校正源输出 
	int GS_WorkMode; //0 校正状态             1 天线信号状态
	int RF_PowerSwitch;//0 关闭电源   1 打开电源
	int CS_PowerSwitch;//0 关闭电源   1 打开电源
	int GS_PowerSwitch;//0 关闭电源   1 打开电源

}*PDEV_PARAM, DEV_PARAM;


//射频模块设备状态查询结果
typedef struct _DEV_RF_STATUS_RESULT
{
	int RF_GainValue;//射频衰减值
	int RF_WorkMode; //0常规 1低噪声
	//int	RF_PowerStatus; //0 关闭  1 电源打开
	//int RF_WorkingVoltage;	//mv
	//int RF_WorkingCurrent;  //mA
	//int RF_Channel;
	float RF_Temperature;    //摄氏度 45.7 == 4570
}*PDEV_RF_STATUS_RESULT, DEV_RF_STATUS_RESULT;

//校正源模块设备状态查询结果
typedef struct _DEV_CS_STATUS_RESULT
{
	int FreqBandStart;//10KHz
	int FreqBandEnd;//10KHz
	int CS_GainValue;
	int CS_WorkMode;//0校准信号关 1校准信号开
	int	CS_PowerStatus;//0 关闭  1 电源打开
	int CS_WorkingVoltage;//mv
	int CS_WorkingCurrent;//mA
	float CS_Temperature;//摄氏度

}*PDEV_CS_STATUS_RESULT, DEV_CS_STATUS_RESULT;

//测向开关模块设备状态查询结果
typedef struct _DEV_GS_STATUS_RESULT
{
	int GS_WorkMode; // 0 校正状态  1 天线状态
	int	GS_PowerStatus;//0 关闭  1 电源打开
	int GS_WorkingVoltage;
	int GS_WorkingCurrent;
	float GS_Temperature;

}*PDEV_GS_STATUS_RESULT, DEV_GS_STATUS_RESULT;


//射频预处理模块身份查询结果
typedef struct _DEV_RF_IDENTITY_RESULT
{
	float RF_BoardVer;   //载板固件版本码  如V10.01 (b1010 0001)
	float RF_Ver;        //固件版本号  如V10.01 (b1010 0001)
	int RF_MaxWorkingFreq;  //最大正常工作时钟频率码,MHz
	int RF_ComName;    //公司名称码  00000000为五十七所，00000001为重庆会凌，00000010为成都中亚，00000011为成都赛狄，00000100位天津754
	int RF_ModuleType; //模块类型码
	int RF_BoughtIndex;//模块申购编号码
	int RF_Addr;       //模块地址码

}*PDEV_RF_IDENTITY_RESULT, DEV_RF_IDENTITY_RESULT;

//校正源模块身份查询结果
typedef struct _DEV_CS_IDENTITY_RESULT
{
	float CS_BoardVer;   //载板固件版本码  如V10.01 (b1010 0001)
	float CS_Ver;        //固件版本号  如V10.01 (b1010 0001)
	int CS_MaxWorkingFreq;  //最大正常工作时钟频率码,MHz
	int CS_ComName;    //公司名称码  00000000为五十七所，00000001为重庆会凌，00000010为成都中亚，00000011为成都赛狄，00000100位天津754
	int CS_ModuleType; //模块类型码
	int CS_BoughtIndex;//模块申购编号码

}*PDEV_CS_IDENTITY_RESULT, DEV_CS_IDENTITY_RESULT;

//测向开关模块身份查询结果
typedef struct _DEV_GS_IDENTITY_RESULT
{
	float GS_BoardVer;   //载板固件版本码  如V10.01 (b1010 0001)
	float GS_Ver;        //固件版本号  如V10.01 (b1010 0001)
	int GS_MaxWorkingFreq;  //最大正常工作时钟频率码,MHz
	int GS_ComName;    //公司名称码  00000000为五十七所，00000001为重庆会凌，00000010为成都中亚，00000011为成都赛狄，00000100位天津754
	int GS_ModuleType; //模块类型码
	int GS_BoughtIndex;//模块申购编号码

}*PDEV_GS_IDENTITY_RESULT, DEV_GS_IDENTITY_RESULT;

#pragma endregion

#pragma region 增益模式结构体

//增益模式返回结果
typedef struct _DEV_DevRF_GetGainMode_RESULT
{
	int Mode; //1 AGC状态 0 MGC状态
}*PDEV_DevRF_GetGainMode_RESULT, DEV_DevRF_GetGainMode_RESULT;

#pragma endregion

#pragma region 设备自检结构体

typedef struct _DEV_CHECKSELF_RESULT
{
	int DevID;
	int Status; //1 正常， 0 异常
}*PDEV_CHECKSELF_RESULT, DEV_CHECKSELF_RESULT;

//采集板FPGA自检返回结果
typedef struct _DEV_FPGA_CHECKSELF_RESULT
{
	int Status1; //1 FPGA加载正常， 0 FPGA加载失败
	float Temperature; //采集板温度

}*PDEV_FPGA_CHECKSELF_RESULT, DEV_FPGA_CHECKSELF_RESULT;

//采集板AD链路自检返回结果
typedef struct _DEV_AD_CHECKSELF_RESULT
{
	int DevID;
	int Status; //1 AD正常， 0 AD失败
}*PDEV_AD_CHECKSELF_RESULT, DEV_AD_CHECKSELF_RESULT;

//万兆网传输板自检返回结果
typedef struct _DEV_RC_CHECKSELF_RESULT
{
	//int DevRCMain;//传输板主板状态     1 成功       0 失败        
	int DevRCVice;//传输板从板状态     1 成功       0 失败                      Status[0]
	int DevRCNet1;//传输板主板1号万兆网口状态     1 成功       0 失败           Status[1]
	int DevRCNet2;//传输板主板2号万兆网口状态     1 成功       0 失败           Status[2]
	int DevRCNet3;//传输板主板3号万兆网口状态     1 成功       0 失败           Status[3]
	int DevRCNet4;//传输板主板4号万兆网口状态     1 成功       0 失败           Status[4]
	//int DevRCNet5;//传输板从板1号万兆网口状态     1 成功       0 失败           Status[7]
	//int DevRCNet6;//传输板从板2号万兆网口状态     1 成功       0 失败           Status[8]
	//int DevRCNet7;//传输板从板3号万兆网口状态     1 成功       0 失败           Status[9]
	//int DevRCNet8;//传输板从板4号万兆网口状态     1 成功       0 失败           Status[10]
	int DevRCFPGA1;//传输板主板1号FPGA程序加载情况     1 成功       0 失败      Status[5]
	int DevRCFPGA2;//传输板主板2号FPGA程序加载情况     1 成功       0 失败      Status[6]
	//int DevRCFPGA3;//传输板从板1号FPGA程序加载情况     1 成功       0 失败      Status[11]
	//int DevRCFPGA4;//传输板从板2号FPGA程序加载情况     1 成功       0 失败      Status[12]
	//char Status[13];

}*PDEV_RC_CHECKSELF_RESULT, DEV_RC_CHECKSELF_RESULT;

#pragma endregion

#pragma region 时标类型结构体
typedef struct _DEV_TIMEDEV_RESULT
{
	int Mode; //1 FPGA加载正常， 0 FPGA加载失败
}*PDEV_TIMEDEV_RESULT, DEV_TIMEDEV_RESULT;

#pragma endregion

#pragma region GPS/BD结构体
typedef struct _DEV_GPSBD_RESULT
{
	int Mode; //1 FPGA加载正常， 0 FPGA加载失败
}*PDEV_GPSBD_RESULT, DEV_GPSBD_RESULT;

#pragma endregion

#pragma region 时钟状态结构体
typedef struct _DEV_CLKStatus_RESULT
{
	int Status; //1 有外部时钟输入， 0 无外部时钟输入
}*PDEV_CLKStatus_RESULT, DEV_CLKStatus_RESULT;

#pragma endregion

#pragma region JGZCAGC增益结构体
typedef struct _GetDigitalGainValueJGZC_RESULT
{

	int DigitalGain[60];//0428JGZCAGC增益

}*PDEV_GetDigitalGainValueJGZC_RESULT, DEV_GetDigitalGainValueJGZC_RESULT;

#pragma endregion

#pragma region 采集板aurora状态查询结果结构体
typedef struct _DEV_CCAURORA_CHECKSELF_RESULT
{
	int AuroraX1; //1 正常， 0 异常
	int AuroraX4_1; //1 正常， 0 异常
	int AuroraX4_2; //1 正常， 0 异常

}*PDEV_CCAURORA_CHECKSELF_RESULT, DEV_CCAURORA_CHECKSELF_RESULT;
#pragma endregion

#pragma region 18数字增益模式查询返回结果
typedef struct _DEV_GetDigitalGainMode_RESULT
{
	int Mode; //1 数字AGC， 0 数字MGC
}*PDEV_GetDigitalGainMode_RESULT, DEV_GetDigitalGainMode_RESULT;
#pragma endregion

#pragma region FPGA程序加载结构体

//传输板FPGA加载返回结果
typedef struct _DEV_RCFPGA_LOAD_RESULT
{
	int Status; //1 FPGA擦除成功， 3 FPGA写入成功，   4 FPGA加载成功     0 失败
}*PDEV_RCFPGA_LOAD_RESULT, DEV_RCFPGA_LOAD_RESULT;

//采集板FPGA加载返回结果
typedef struct _DEV_CCFPGA_LOAD_RESULT
{
	int Status; //1 FPGA擦除成功， 1 FPGA写入成功，   
}*PDEV_CCFPGA_LOAD_RESULT, DEV_CCFPGA_LOAD_RESULT;

#pragma endregion

#pragma region 心跳包结构体

typedef struct _DEV_HEARTBEAT_RESULT
{
	int DevID;//   设备编号
	int CC_Switch_State[5];//数字接收机开机状态
	int CC_Clock_State[5]; //系统时钟状态
	float CC_Temperature_State[5];//数字接收机温度值
	int CC_FPGA_State[5];//FPGA芯片状态
	int RC_Net_State[5];//传输板状态
	int RF_Module_State[5];//射频通道模块状态
	int GS_WorkMode[5]; // 0 校正状态  1 天线状态
	int	GS_Status[5];//0 关闭  1 电源打开
	float RF_Temperature_State[5];//射频机箱温度

}*PDEV_HEARTBEAT_RESULT, DEV_HEARTBEAT_RESULT;

#pragma endregion

#pragma region 2019年新添

//射频设备状态查询结果
typedef struct _DEV_WS_STATUS_RESULT
{
	int CS_FreqBandStart;//10KHz
	int CS_FreqBandEnd;//10KHz
	int CS_GainValue;
	int CS_WorkMode;//0校准信号关 1校准信号开
	int	CS_PowerStatus;//0 关闭  1 电源打开
	int GS_WorkMode; // 0 校正状态  1 天线状态
	int	GS_PowerStatus;//0 关闭  1 电源打开
	int WorkingVoltage;//mv
	int WorkingCurrent;//mA
	float Temperature;//摄氏度

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

//AD过载报警结构体
typedef struct _DEV_ADOverloadAlarm_RESULT
{
	int DevID;//采集板卡号
	int alarm[20];

}*PDEV_ADOverloadAlarm_RESULT, DEV_ADOverloadAlarm_RESULT;

//射频设备状态查询结果
typedef struct _GetDigitalGainValue_RESULT
{
	int DevID;//板卡号

	int RFChannelGain[20];//射频通道增益数组

	float PathGain;//链路增益

	int DigitalGain[32][30];//数字AGC增益数组//0923修改二维数组大小以兼容8T、18T//0424修改数组大小以兼容JGZC
	
}*PDEV_GetDigitalGainValue_RESULT, DEV_GetDigitalGainValue_RESULT;

//万兆网寄存器状态查询结果
typedef struct _DEV_GetWebStatus_CHECKSELF_RESULT
{
	int DevID;
	int Allsituation[8]; //1 正常， 0 异常
	int IPAddr_UDPport[20]; //1 正常， 0 异常
	int AuroraX4_2; //1 正常， 0 异常

}*PDEV_GetWebStatus_CHECKSELF_RESULT, DEV_GetWebStatus_CHECKSELF_RESULT;

//FPGA版本查询结果
typedef struct _DEV_GetFPGAVersion_CHECKSELF_RESULT
{
	//int DevID;//板卡号
	int manufacturers;//厂家固定为754
	//int ModuleType; //模块类型（根据字节位置自动识别）
	int sampleK7_Type[5];//采集板K7设备型号
	int sampleK7_integer[5];//采集板K7版本号整数
	int sampleK7_decimal[5];//采集板K7版本号小数

	int sampleV7_Type[5];//采集板V7设备型号
	int sampleV7_integer[5];//采集板V7版本号整数
	int sampleV7_decimal[5];//采集板V7版本号小数


	int netK7_Type[2];//传输板K7设备型号
	int netK7_integer[2];//传输板K7版本号整数
	int netK7_decimal[2];//传输板V7版本号小数

	int netF1_Type[2];//传输板F1设备型号
	int netF1_integer[2];//传输板F1版本号整数
	int netF1_decimal[2];//传输板F1版本号小数

	int netF2_Type[2];//传输板F2设备型号
	int netF2_integer[2];//传输板F2版本号整数
	int netF2_decimal[2];//传输板F2版本号小数

}*PDEV_GetFPGAVersion_CHECKSELF_RESULT, DEV_GetFPGAVersion_CHECKSELF_RESULT;

//FPGA版本查询结果传输板
typedef struct _DEV_GetFPGAVersionFC_CHECKSELF_RESULT
{
	//int DevID;//板卡号
	int manufacturers;//厂家固定为754
	//int ModuleType; //模块类型（根据字节位置自动识别）
	int sampleK7_Type[5];//采集板K7设备型号
	int sampleK7_integer[5];//采集板K7版本号整数
	int sampleK7_decimal[5];//采集板K7版本号小数

	int sampleV7_Type[5];//采集板V7设备型号
	int sampleV7_integer[5];//采集板V7版本号整数
	int sampleV7_decimal[5];//采集板V7版本号小数


	int netK7_Type[2];//传输板K7设备型号
	int netK7_integer[2];//传输板K7版本号整数
	int netK7_decimal[2];//传输板V7版本号小数

	int netF1_Type[2];//传输板F1设备型号
	int netF1_integer[2];//传输板F1版本号整数
	int netF1_decimal[2];//传输板F1版本号小数

	int netF2_Type[2];//传输板F2设备型号
	int netF2_integer[2];//传输板F2版本号整数
	int netF2_decimal[2];//传输板F2版本号小数

}*PDEV_GetFPGAVersionFC_CHECKSELF_RESULT, DEV_GetFPGAVersionFC_CHECKSELF_RESULT;

#pragma region AD过载回调函数指针

typedef void(_stdcall *Func1)(_DEV_ADOverloadAlarm_RESULT Result);

#pragma endregion

#pragma region 心跳包回调函数指针

typedef void(_stdcall *Func2)(_DEV_HEARTBEAT_RESULT heart);

#pragma endregion


//190702重复添加
		//开机状态查询结果
//	typedef struct _PDEV_PowerOnStatus_RESULT
//		{
//			int Status;//1:开机，0：未开机，板卡供电有问题
//
//		}*PDEV_PowerOnStatus_RESULT, DEV_PowerOnStatus_RESULT;
//
//		//系统时钟状态查询结果
//		typedef struct _PDEV_CLKStatus_RESULT
//		{
//			int Status;//1:开机，0：未开机，板卡供电有问题
//
//		}*PDEV_CLKStatus_RESULT, DEV_CLKStatus_RESULT;
//
//		//射频通信状态查询结果
//		typedef struct _PDEV_RFCommunityStatus_RESULT
//		{
//			int Status;//1:开机，0：未开机，板卡供电有问题
//
//		}*PDEV_RFCommunityStatus_RESULT, DEV_RFCommunityStatus_RESULT;
//
//		//采集板aurora状态查询结果
//		typedef struct _PDEV_AuroraStatus_RESULT
//		{
//			int AuroraX1;//1：通信正常，0：通信异常
//			int AuroraX4_1;//1：通信正常，0：通信异常
//			int AuroraX4_2;//1：通信正常，0：通信异常
//
//		}*PDEV_AuroraStatus_RESULT, DEV_AuroraStatus_RESULT;
#pragma endregion

#endif