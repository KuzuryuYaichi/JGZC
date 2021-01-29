#include "stdafx.h"
#include <string>
#include <iostream>
#include "DevCtrlCmd.h"
#include "cmath"
#include "CommandCtrl.h"

CDevCtrlCmd::CDevCtrlCmd()
{
	m_CtrlCmd.reset(new unsigned char[m_ProtocolSize], [](unsigned char *p){ delete[]p; });
	m_CtrlCmd_CCFPGA.reset(new unsigned char[m_CCFPGASize], [](unsigned char *p1){ delete[]p1; });
	m_CtrlCmd_RCFPGA.reset(new unsigned char[m_RCFPGASize], [](unsigned char *p2){ delete[]p2; });

	m_FreqBandInfo.resize(5);
	std::pair<unsigned long, unsigned long> FreqBand0(150, 550);
	std::pair<unsigned long, unsigned long> FreqBand1(550, 950);
	std::pair<unsigned long, unsigned long> FreqBand2(950, 1450);
	std::pair<unsigned long, unsigned long> FreqBand3(1050, 2050);
	std::pair<unsigned long, unsigned long> FreqBand4(2050, 3050);

	m_FreqBandInfoZC.resize(5);
	std::pair<unsigned long, unsigned long> FreqBandZC0(150, 550);
	std::pair<unsigned long, unsigned long> FreqBandZC1(950, 1550);//190828LX交换8通道2、3频段频率
	std::pair<unsigned long, unsigned long> FreqBandZC2(550, 950);//190828LX交换8通道2、3频段频率
	std::pair<unsigned long, unsigned long> FreqBandZC3(1550, 3000);

	m_FreqBandInfoZC18.resize(5);
	std::pair<unsigned long, unsigned long> FreqBandZC180(150, 550);
	std::pair<unsigned long, unsigned long> FreqBandZC181(550, 950);
	std::pair<unsigned long, unsigned long> FreqBandZC182(950, 1550);
	std::pair<unsigned long, unsigned long> FreqBandZC183(1550, 3050);

	m_FreqBandInfo[0] = FreqBand0;
	m_FreqBandInfo[1] = FreqBand1;
	m_FreqBandInfo[2] = FreqBand2;
	m_FreqBandInfo[3] = FreqBand3;
	m_FreqBandInfo[4] = FreqBand4;

	m_FreqBandInfoZC[0] = FreqBandZC0;
	m_FreqBandInfoZC[1] = FreqBandZC1;
	m_FreqBandInfoZC[2] = FreqBandZC2;
	m_FreqBandInfoZC[3] = FreqBandZC3;
	
	m_FreqBandInfoZC18[0] = FreqBandZC180;
	m_FreqBandInfoZC18[1] = FreqBandZC181;
	m_FreqBandInfoZC18[2] = FreqBandZC182;
	m_FreqBandInfoZC18[3] = FreqBandZC183;
	//std::cout << "CDevCtrlCmd" << std::endl;

}


CDevCtrlCmd::~CDevCtrlCmd()
{
	//std::cout << "~CDevCtrlCmd" << std::endl;

}

//调转单字节，小堆顶变大堆顶
char turn_around(char little)
{
	return ((little & 0x01) << 7)
		+ ((little & 0x02) << 5)
		+ ((little & 0x04) << 3)
		+ ((little & 0x08) << 1)
		+ ((little & 0x10) >> 1)
		+ ((little & 0x20) >> 3)
		+ ((little & 0x40) >> 5)
		+ ((little & 0x80) >> 7);
}

//将IP点分十进制（字符串）转换为字节
void atob1(char *src, int *dest)
{
	std::string ip_end = ".1";
	std::string src_transit[4];//裁剪的后端字符串
	std::string src_tr[4];  //裁剪的前端字符串
	int pos[4];        //点的位置

	std::string src_s = static_cast<std::string>(src);
	src_transit[0] = src_s + ip_end;
	//src_transit[0] = src_s.data();

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

//将MAC地址字符串转化为字节
void atob2(char *src, int *dest)
{
	std::string src_tr[6];
	const char* src_transit[6];
	std::string src_s = static_cast<std::string>(src);
	for (int i = 0; i < 6; i++)
	{
		src_tr[i] = (src_s.substr(2 * i, 2));
		src_transit[i] = src_tr[i].c_str();
		dest[i] = (int)strtol(src_transit[i], NULL, 16);
	}
}

void CDevCtrlCmd::ClearBuffer()
{
	memset(m_CtrlCmd.get(), 0x00, m_ProtocolSize);
	memset(m_CtrlCmd_CCFPGA.get(), 0x00, m_CCFPGASize);
	memset(m_CtrlCmd_RCFPGA.get(), 0x00, m_RCFPGASize);
}

#pragma region 千兆网网络配置
//********************************************************************Lijf*************************************************************//
//LJF_TODO:
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_SetLocalIP(char *ip)
{
	ClearBuffer();
	int ipCtr[4];
	atob1(ip, ipCtr);
	m_CtrlCmd.get()[0] = 0x07;
	m_CtrlCmd.get()[1] = 0x54;
	m_CtrlCmd.get()[2] = 0xAB;
	m_CtrlCmd.get()[3] = 0xC0;
	m_CtrlCmd.get()[4] = 0xAB;
	m_CtrlCmd.get()[5] = 0xAB;
	m_CtrlCmd.get()[6] = 0xA0;
	m_CtrlCmd.get()[7] = 0xA0;
	m_CtrlCmd.get()[8] = static_cast<char>(ipCtr[0]) & 0xFF;
	m_CtrlCmd.get()[9] = static_cast<char>(ipCtr[1]) & 0xFF;
	m_CtrlCmd.get()[10] = static_cast<char>(ipCtr[2]) & 0xFF;
	m_CtrlCmd.get()[11] = static_cast<char>(ipCtr[3]) & 0xFF;
	m_CtrlCmd.get()[16] = 0xAA;
	m_CtrlCmd.get()[17] = 0xAA;
	m_CtrlCmd.get()[18] = 0xAA;
	m_CtrlCmd.get()[19] = 0xAA;
	return m_CtrlCmd;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_SetDevIP(char *ip)
{
	ClearBuffer();
	int ipCtr[6];
	atob1(ip, ipCtr);
	m_CtrlCmd.get()[0] = 0x07;
	m_CtrlCmd.get()[1] = 0x54;
	m_CtrlCmd.get()[2] = 0xAB;
	m_CtrlCmd.get()[3] = 0xC0;
	m_CtrlCmd.get()[4] = 0xAB;
	m_CtrlCmd.get()[5] = 0xAB;
	m_CtrlCmd.get()[6] = 0xA2;
	m_CtrlCmd.get()[7] = 0xA2;
	m_CtrlCmd.get()[8] = static_cast<char>(ipCtr[0]) & 0xFF;
	m_CtrlCmd.get()[9] = static_cast<char>(ipCtr[1]) & 0xFF;
	m_CtrlCmd.get()[10] = static_cast<char>(ipCtr[2]) & 0xFF;
	m_CtrlCmd.get()[11] = static_cast<char>(ipCtr[3]) & 0xFF;
	m_CtrlCmd.get()[16] = 0xAA;
	m_CtrlCmd.get()[17] = 0xAA;
	m_CtrlCmd.get()[18] = 0xAA;
	m_CtrlCmd.get()[19] = 0xAA;
	return m_CtrlCmd;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_SetDevCCPort(unsigned short port)
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0x07;
	m_CtrlCmd.get()[1] = 0x54;
	m_CtrlCmd.get()[2] = 0xAB;
	m_CtrlCmd.get()[3] = 0xC0;
	m_CtrlCmd.get()[4] = 0xAB;
	m_CtrlCmd.get()[5] = 0xAB;
	m_CtrlCmd.get()[6] = 0xA1;
	m_CtrlCmd.get()[7] = 0xA1;
	m_CtrlCmd.get()[8] = (port >> 8) & 0xFF;
	m_CtrlCmd.get()[9] = port & 0xFF;
	m_CtrlCmd.get()[10] = (port >> 8) & 0xFF;
	m_CtrlCmd.get()[11] = port & 0xFF;
	m_CtrlCmd.get()[16] = 0xAA;
	m_CtrlCmd.get()[17] = 0xAA;
	m_CtrlCmd.get()[18] = 0xAA;
	m_CtrlCmd.get()[19] = 0xAA;
	return m_CtrlCmd;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_SetDevFCPort(unsigned short port)
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0x07;
	m_CtrlCmd.get()[1] = 0x54;
	m_CtrlCmd.get()[2] = 0xAB;
	m_CtrlCmd.get()[3] = 0xC0;
	m_CtrlCmd.get()[4] = 0xAB;
	m_CtrlCmd.get()[5] = 0xAB;
	m_CtrlCmd.get()[6] = 0xA7;
	m_CtrlCmd.get()[7] = 0xA7;
	m_CtrlCmd.get()[8] = (port >> 8) & 0xFF;
	m_CtrlCmd.get()[9] = port & 0xFF;
	m_CtrlCmd.get()[10] = (port >> 8) & 0xFF;
	m_CtrlCmd.get()[11] = port & 0xFF;
	m_CtrlCmd.get()[16] = 0xAA;
	m_CtrlCmd.get()[17] = 0xAA;
	m_CtrlCmd.get()[18] = 0xAA;
	m_CtrlCmd.get()[19] = 0xAA;
	return m_CtrlCmd;
}
#pragma endregion

#pragma region 万兆网数据传输配置

//数据源同步(复位功能)
std::pair<std::shared_ptr<unsigned char>, int > CDevCtrlCmd::MakeCmd_DataReset(int boardNum, int enable, int dataSource)
{
	std::pair<std::shared_ptr<unsigned char>, int > p;
	std::shared_ptr<unsigned char> t(new unsigned char[256], [](unsigned char *p){ delete[]p; });
	p.first = t;

	/*****************以下为了得到帧头*******************************************/
	UINT16 nSerial = 0;//命令流水号（包序号）
	UINT8 nBoardNo = boardNum; //板卡号（0, 1）
	UINT8 nNetCardNo = 0;//万兆网口号（0~3）,对应光口1~4
	int nOption = boardNum; //主从选择, 0:从 1:主
	UINT32 nSrcIP = 0; //万兆网口源IP
	UINT64 nSrcMAC = 0;//万兆网口源MAC
	UINT32 nInterval = 0;  //MAC帧插入时钟计数:默认为0
	UINT16 nPacketLen = 8192;//UDP分片长度门限:1472
	UINT32 nBitControl = 0;//位控制寄存器
	UINT16 nHeartPort = 0;//连接心跳包端口（未使用）
	UINT32 nSlicesClock = 0; //分片间插入时钟周期数
	unsigned char theDstCmd[256];//输出
	int theCmdLen = 0;//输出
	SrcParam_t SrcParam;
	SrcParam.GetCmd(theDstCmd, &theCmdLen, nSerial, nBoardNo, nNetCardNo, nOption,
		nSrcIP, nSrcMAC, nInterval, nPacketLen, nBitControl, nHeartPort, nSlicesClock);
	/**********************************************************************/
	theCmdLen = 8 + 24 + 8;//规定死

	theDstCmd[32] = 0;
	theDstCmd[33] = 0;
	theDstCmd[34] = 0;
	theDstCmd[35] = 7;//地址为0x07

	theDstCmd[36] = 0;
	theDstCmd[37] = 0;
	theDstCmd[38] = 0;
	theDstCmd[39] = 0;
	theDstCmd[39] = dataSource * 2 + enable;//D[1]:数据源选择 ； D[0]控制信号

	memcpy(t.get(), theDstCmd, theCmdLen);
	p.second = theCmdLen;
	return p;
}

//目的IP设置
std::pair<std::shared_ptr<unsigned char>, int > CDevCtrlCmd::MakeCmd_SetDataDestIP(int NetID, int connectionId, char *ip, unsigned short disPort, unsigned short sourcePort)
{
	std::pair<std::shared_ptr<unsigned char>, int > p;
	std::shared_ptr<unsigned char> t(new unsigned char[256], [](unsigned char *p){ delete[]p; });
	p.first = t;
	/*以下为wdx建立*/
	ClearBuffer();

	UINT16 nSerial = 0;//命令流水号（包序号）
	UINT8 nBoardNo = 0; //板卡号（0, 1）
	UINT8 nNetCardNo = 0;//万兆网口号（0~3）,对应光口1~4
	int nOption = 0; //主从选择, 0:从 1:主
	UINT32 IPRamAddr = 0;//目的IP地址 RAM地址
	UINT32 nIP = 0; //目的IP地址
	UINT32 PortRamAddr = 0;//UDP源端口/目的端口 RAM地址
	UINT16 nSrcPort = 0;//UDP源端口
	UINT16 nDstPort = 0;//UDP目的端口

	unsigned char theDstCmd[256];//输出
	int theCmdLen = 0;//输出
	DstParam_t DstParam;

	if (NetID <= 4 && NetID >= 1)//主板
	{
		nBoardNo = 1;
		nOption = 1;
		nNetCardNo = NetID - 1;
	}
	else if (NetID <= 8 && NetID >= 5)//从板
	{
		nOption = 0;
		nBoardNo = 0;
		nNetCardNo = NetID - 4 - 1;
	}

	IPRamAddr = connectionId * 2;
	PortRamAddr = connectionId * 2 + 1;

	int ipCtr[4];
	atob1(ip, ipCtr);
	nIP = ipCtr[0] * 256 * 256 * 256 + ipCtr[1] * 256 * 256 + ipCtr[2] * 256 + ipCtr[3];

	nSrcPort = sourcePort;//源端口
	nDstPort = disPort;//目的端口


	DstParam.GetCmd(theDstCmd, &theCmdLen, nSerial, nBoardNo, nNetCardNo, nOption,
		IPRamAddr, nIP, PortRamAddr, nSrcPort, nDstPort);

	/*得到命令帧*/
	memcpy(t.get(), theDstCmd, theCmdLen);
	p.second = theCmdLen;
	return p;
}

/*wdx修改*/
std::pair<std::shared_ptr<unsigned char>, int > CDevCtrlCmd::MakeCmd_SetDataSourceIP(int NetID, char *ip, char *mac, int PacketLen, int Interval, int SlicesClock)
{
	std::pair<std::shared_ptr<unsigned char>, int > p;
	std::shared_ptr<unsigned char> t(new unsigned char[256], [](unsigned char *p){ delete[]p; });
	p.first = t;


	/*以下为wdx建立*/
	UINT16 nSerial = 0;//命令流水号（包序号）
	UINT8 nBoardNo = 0; //板卡号（0, 1）
	UINT8 nNetCardNo = 2;//万兆网口号（0~3）,对应光口1~4
	int nOption = 0; //主从选择, 0:从 1:主
	UINT32 nSrcIP = 0; //万兆网口源IP
	UINT64 nSrcMAC = 0;//万兆网口源MAC
	UINT32 nInterval = 16;  //MAC帧插入时钟计数:默认为0
	UINT16 nPacketLen = 8000;//UDP分片长度门限:1472
	UINT32 nBitControl = 0;//位控制寄存器
	UINT16 nHeartPort = 0;//连接心跳包端口（未使用）
	UINT32 nSlicesClock = 0;//分片间插入时钟周期数

	unsigned char theDstCmd[256];//输出
	int theCmdLen = 0;//输出
	SrcParam_t SrcParam;

	/*以下为参数转换*/
	nSrcMAC = SrcParam.GetMAC((unsigned char *)mac);

	if (NetID <= 4 && NetID >= 1)//主板
	{
		nBoardNo = 1;
		nOption = 1;
		nNetCardNo = NetID - 1;
	}
	else if (NetID <= 8 && NetID >= 5)//从板
	{
		nOption = 0;
		nBoardNo = 0;
		nNetCardNo = NetID - 4 - 1;
	}


	int ipCtr[4];
	atob1(ip, ipCtr);
	nSrcIP = ipCtr[0] * 256 * 256 * 256 + ipCtr[1] * 256 * 256 + ipCtr[2] * 256 + ipCtr[3];
	nHeartPort = 0;
	nPacketLen = PacketLen;
	nInterval = Interval;
	nSlicesClock = SlicesClock;
	SrcParam.GetCmd(theDstCmd, &theCmdLen, nSerial, nBoardNo, nNetCardNo, nOption,
		nSrcIP, nSrcMAC, nInterval, nPacketLen, nBitControl, nHeartPort, nSlicesClock);

	/*得到命令帧*/
	memcpy(t.get(), theDstCmd, theCmdLen);
	p.second = theCmdLen;
	return p;
}

/*使能*/
std::pair<std::shared_ptr<unsigned char>, int >CDevCtrlCmd::MakeCmd_SetDataSendSource(int NetID, int connectionId, int en)
{
	std::pair<std::shared_ptr<unsigned char>, int > p;
	std::shared_ptr<unsigned char> t(new unsigned char[256], [](unsigned char *p){ delete[]p; });
	p.first = t;

	/*以下为wdx建立*/
	UINT16 nSerial = 0;//命令流水号（包序号）
	UINT8 nBoardNo = 0; //板卡号（0, 1）
	UINT8 nNetCardNo = 0;//万兆网口号（0~3）,对应光口1~4
	int nOption = 0; //主从选择, 0:从 1:主
	UINT32 EnableRamAddr = 0;//连接使能 RAM地址
	UINT32 nEnable = 0;//连接使能, 0:禁止 1:使能
	unsigned char theDstCmd[256] = { 0 };//输出
	int theCmdLen = 0;//输出
	LinkEnable_t LinkEnable;

	if (NetID <= 4 && NetID >= 1)//主板
	{
		nBoardNo = 1;
		nOption = 1;
		nNetCardNo = NetID - 1;
	}
	else if (NetID <= 8 && NetID >= 5)//从板
	{
		nOption = 0;
		nBoardNo = 0;
		nNetCardNo = NetID - 4 - 1;
	}

	EnableRamAddr = connectionId;
	nEnable = en;

	LinkEnable.GetCmd(theDstCmd, &theCmdLen, nSerial, nBoardNo, nNetCardNo, nOption,
		EnableRamAddr, nEnable);

	/*得到命令帧*/
	memcpy(t.get(), theDstCmd, theCmdLen);
	p.second = theCmdLen;
	return p;
}

//*********************************************************************使能总控制*****************************************************************************//
std::pair<std::shared_ptr<unsigned char>, int >CDevCtrlCmd::MakeCmd_SetDataAllEnable(int NetID, int en)
{
	std::pair<std::shared_ptr<unsigned char>, int > p;
	std::shared_ptr<unsigned char> t(new unsigned char[360], [](unsigned char *p){ delete[]p; });
	p.first = t;

	/*以下为wdx建立*/

	UINT16 nSerial = 0;//命令流水号（包序号）
	UINT8 nBoardNo = 0; //板卡号（0, 1）
	UINT8 nNetCardNo = 0;//万兆网口号（0~3）,对应光口1~4
	int nOption = 0; //主从选择, 0:从 1:主
	UINT32 nEnable = 0;//连接使能, 0:禁止 1:使能

	unsigned char theDstCmd[360] = { 0 };//输出
	int theCmdLen = 0;//输出
	AllEnable_t AllEnable;

	if (NetID <= 4 && NetID >= 1)//主板
	{
		nBoardNo = 1;
		nOption = 1;
		nNetCardNo = NetID - 1;
	}
	else if (NetID <= 8 && NetID >= 5)//从板
	{
		nOption = 0;
		nBoardNo = 0;
		nNetCardNo = NetID - 4 - 1;
	}

	nEnable = en;

	AllEnable.GetCmd(theDstCmd, &theCmdLen, nSerial, nBoardNo, nNetCardNo, nOption,
		nEnable);

	/*得到命令帧*/
	memcpy(t.get(), theDstCmd, theCmdLen);
	p.second = theCmdLen;
	return p;
}

//**********************************************************************区分组播单播**********************************************************************//
std::pair<std::shared_ptr<unsigned char>, int > CDevCtrlCmd::MakeCmd_SetMultiSelect(int NetID, int connectionId, int multi)
{
	std::pair<std::shared_ptr<unsigned char>, int > p;
	std::shared_ptr<unsigned char> t(new unsigned char[256], [](unsigned char *p){ delete[]p; });
	p.first = t;
	/*以下为wdx建立*/
	ClearBuffer();

	UINT16 nSerial = 0;//命令流水号（包序号）
	UINT8 nBoardNo = 0; //板卡号（0, 1）
	UINT8 nNetCardNo = 0;//万兆网口号（0~3）,对应光口1~4
	int nOption = 0; //主从选择, 0:从 1:主
	UINT32 MultiRamAddr = 0;//单播组播选择 RAM地址
	UINT32 nMulti = 0; //单播组播选择

	unsigned char theDstCmd[256];//输出
	int theCmdLen = 0;//输出

	MultiParam_t MultiParam;

	if (NetID <= 4 && NetID >= 1)//主板
	{
		nBoardNo = 1;
		nOption = 1;
		nNetCardNo = NetID - 1;
	}
	else if (NetID <= 8 && NetID >= 5)//从板
	{
		nOption = 0;
		nBoardNo = 0;
		nNetCardNo = NetID - 4 - 1;
	}

	MultiRamAddr = connectionId;

	MultiParam.GetCmd(theDstCmd, &theCmdLen, nSerial, nBoardNo, nNetCardNo, nOption,
		MultiRamAddr, multi);

	/*得到命令帧*/
	memcpy(t.get(), theDstCmd, theCmdLen);
	p.second = theCmdLen;
	return p;
}

std::pair<std::shared_ptr<unsigned char>, int > CDevCtrlCmd::MakeCmd_SetMultiMac(int NetID, int connectionId, char *MultiMac)
{
	std::pair<std::shared_ptr<unsigned char>, int > p;
	std::shared_ptr<unsigned char> t(new unsigned char[256], [](unsigned char *p){ delete[]p; });
	p.first = t;
	/*以下为wdx建立*/
	ClearBuffer();

	UINT16 nSerial = 0;//命令流水号（包序号）
	UINT8 nBoardNo = 0; //板卡号（0, 1）
	UINT8 nNetCardNo = 0;//万兆网口号（0~3）,对应光口1~4
	int nOption = 0; //主从选择, 0:从 1:主
	UINT32 MultiMacRamAddr = 0;//组播MAC地址 RAM地址
	UINT64 nMultiMac = 0; //组播MAC地址

	unsigned char theDstCmd[256];//输出
	int theCmdLen = 0;//输出

	MultiMacParam_t MultiMacParam;

	if (NetID <= 4 && NetID >= 1)//主板
	{
		nBoardNo = 1;
		nOption = 1;
		nNetCardNo = NetID - 1;
	}
	else if (NetID <= 8 && NetID >= 5)//从板
	{
		nOption = 0;
		nBoardNo = 0;
		nNetCardNo = NetID - 4 - 1;
	}

	MultiMacRamAddr = connectionId * 2;
	nMultiMac = MultiMacParam.GetMAC((unsigned char *)MultiMac);

	MultiMacParam.GetCmd(theDstCmd, &theCmdLen, nSerial, nBoardNo, nNetCardNo, nOption,
		MultiMacRamAddr, nMultiMac);

	/*得到命令帧*/
	memcpy(t.get(), theDstCmd, theCmdLen);
	p.second = theCmdLen;
	return p;
}

//********************************************************************end  Lijf*************************************************************//
#pragma endregion

#pragma region 原DBZC采集板控制

//03远程复位指令
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_ResetDev(int DevID)
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = static_cast<char>(DevID);
	m_CtrlCmd.get()[4] = 0x03;

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_ResetDev_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xCC || (RetData.get()[4]) != 0x04)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = 1;
	}
	return 0;
}

//08射频通信状态查询
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFCommunityStatus(int DevID)
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = static_cast<char>(DevID);
	m_CtrlCmd.get()[4] = 0x08;
	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_RFCommunityStatus_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_CHECKSELF_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xCC || RetData.get()[4] != 0x0B)
	{
		memset(Reslut, 0x00, sizeof(PDEV_CHECKSELF_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		int DevID = static_cast<int>(RetData.get()[2] & 0xff);
		int Status = static_cast<int>(RetData.get()[6] & 0xff);
		Reslut->DevID = DevID;
		Reslut->Status = Status;
	}
	return 0;
}

//13AGC增益值查询
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_GetDigitalGainValue(int DevID)
{
	ClearBuffer();
	int offset = 8;

	m_CtrlCmd.get()[0] = 0xab;
	m_CtrlCmd.get()[1] = 0xab;
	m_CtrlCmd.get()[2] = 0xab;
	m_CtrlCmd.get()[3] = 0xab;
	m_CtrlCmd.get()[4] = 0xab;
	m_CtrlCmd.get()[5] = 0xab;
	m_CtrlCmd.get()[6] = 0xab;
	m_CtrlCmd.get()[7] = 0xab;

	m_CtrlCmd.get()[offset + 0] = 0x5A;
	m_CtrlCmd.get()[offset + 1] = 0x58;
	m_CtrlCmd.get()[offset + 2] = 0x4C;
	m_CtrlCmd.get()[offset + 3] = 0x43;

	m_CtrlCmd.get()[offset + 4] = 0xFF;
	m_CtrlCmd.get()[offset + 5] = 0xFF;
	m_CtrlCmd.get()[offset + 6] = 0x00;
	m_CtrlCmd.get()[offset + 7] = 0x02;

	m_CtrlCmd.get()[offset + 16] = 0x00;
	m_CtrlCmd.get()[offset + 17] = 0x00;

	m_CtrlCmd.get()[offset + 23] = 0x80;

	m_CtrlCmd.get()[offset + 31] = static_cast<char>(DevID);
	m_CtrlCmd.get()[offset + 19] = 0x40;
	m_CtrlCmd.get()[offset + 20] = 0x01;

	if (DevID == 1)
	{
		m_CtrlCmd.get()[offset + 18] = 0x01;

	}
	else if (DevID == 2)
	{
		m_CtrlCmd.get()[offset + 18] = 0x01;

	}
	else if (DevID == 3)
	{
		m_CtrlCmd.get()[offset + 18] = 0x01;

	}
	else if (DevID == 4)
	{
		m_CtrlCmd.get()[offset + 18] = 0x00;

	}
	else if (DevID == 5)
	{
		m_CtrlCmd.get()[offset + 18] = 0x00;

	}

	for (size_t i = 0; i < 96; i++)
	{
		m_CtrlCmd.get()[offset + 32 + i] = 0xFF;
	}

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_GetDigitalGainValue_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[0] != 0x5A || RetData.get()[1] != 0x58 || RetData.get()[2] != 0x4C || RetData.get()[3] != 0x43 || RetData.get()[5] != 0x7E)
	{
		memset(Reslut, 0x00, sizeof(PDEV_GetDigitalGainValue_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		Reslut->DevID = static_cast<int>(RetData.get()[4] & 0xff) - 1;

		for (int i = 0; i < 20; i++)
		{
			Reslut->RFChannelGain[i] = static_cast<int>(RetData.get()[i + 12]);
		}
		/*Reslut->RFChannelGain[0] = static_cast<int>(RetData.get()[10]);
		Reslut->RFChannelGain[1] = static_cast<int>(RetData.get()[11]);
		Reslut->RFChannelGain[2] = static_cast<int>(RetData.get()[12]);
		Reslut->RFChannelGain[3] = static_cast<int>(RetData.get()[13]);
		Reslut->RFChannelGain[4] = static_cast<int>(RetData.get()[14]);
		Reslut->RFChannelGain[5] = static_cast<int>(RetData.get()[15]);
		Reslut->RFChannelGain[6] = static_cast<int>(RetData.get()[16]);
		Reslut->RFChannelGain[7] = static_cast<int>(RetData.get()[17]);
		Reslut->RFChannelGain[8] = static_cast<int>(RetData.get()[18]);
		Reslut->RFChannelGain[9] = static_cast<int>(RetData.get()[19]);
		Reslut->RFChannelGain[10] = static_cast<int>(RetData.get()[20]);
		Reslut->RFChannelGain[11] = static_cast<int>(RetData.get()[21]);
		Reslut->RFChannelGain[12] = static_cast<int>(RetData.get()[22]);
		Reslut->RFChannelGain[13] = static_cast<int>(RetData.get()[23]);
		Reslut->RFChannelGain[14] = static_cast<int>(RetData.get()[24]);
		Reslut->RFChannelGain[15] = static_cast<int>(RetData.get()[25]);
		Reslut->RFChannelGain[16] = static_cast<int>(RetData.get()[26]);
		Reslut->RFChannelGain[17] = static_cast<int>(RetData.get()[27]);
		Reslut->RFChannelGain[18] = static_cast<int>(RetData.get()[28]);
		Reslut->RFChannelGain[19] = static_cast<int>(RetData.get()[29]);*/

		//定义临时数组用于解析链路增益小数部分
		float temp1 = static_cast<float>(((RetData.get()[37]) & 0x01) >> 0) / 128.0;
		float temp2 = static_cast<float>(((RetData.get()[37]) & 0x02) >> 1) / 64.0;
		float temp3 = static_cast<float>(((RetData.get()[37]) & 0x04) >> 2) / 32.0;
		float temp4 = static_cast<float>(((RetData.get()[37]) & 0x08) >> 3) / 16.0;
		float temp5 = static_cast<float>(((RetData.get()[37]) & 0x10) >> 4) / 8.0;
		float temp6 = static_cast<float>(((RetData.get()[37]) & 0x20) >> 5) / 4.0;
		float temp7 = static_cast<float>(((RetData.get()[37]) & 0x40) >> 6) / 2.0;
		float decimals = temp1 + temp2 + temp3 + temp4 + temp5 + temp6 + temp7;
		Reslut->PathGain = static_cast<float>(RetData.get()[36]) + decimals;

		int startbandindex = 0;
		int endbandindex = 0;
		switch (Reslut->DevID)
		{
			// 板卡号为1
		case 1:
			startbandindex = 0;
			endbandindex = 8;
			MakeDigitalGain(std::move(RetData), Reslut, startbandindex, endbandindex);
			break;
			// 板卡号为2
		case 2:
			startbandindex = 0;
			endbandindex = 8;
			MakeDigitalGain(std::move(RetData), Reslut, startbandindex, endbandindex);
			break;
			// 板卡号为3
		case 3:
			startbandindex = 0;
			endbandindex = 10;
			MakeDigitalGain(std::move(RetData), Reslut, startbandindex, endbandindex);
			break;
			// 板卡号为4,zidai:26-45
		case 4:
			if ((RetData.get()[9])<36)
			{
				startbandindex = 0;
				endbandindex = 10;
				MakeDigitalGain(std::move(RetData), Reslut, startbandindex, endbandindex);
			}
			else
			{
				startbandindex = 10;
				endbandindex = 20;
				MakeDigitalGain(std::move(RetData), Reslut, startbandindex, endbandindex);
			}

			break;
			// 板卡号为5,zidai:46-65
		case 5:
			if ((RetData.get()[9])<56)
			{
				startbandindex = 0;
				endbandindex = 10;
				MakeDigitalGain(std::move(RetData), Reslut, startbandindex, endbandindex);
			}
			else
			{
				startbandindex = 10;
				endbandindex = 20;
				MakeDigitalGain(std::move(RetData), Reslut, startbandindex, endbandindex);
			}

			break;
		default:
			break;
		}
	}
	return 0;
}
void CDevCtrlCmd::MakeDigitalGain(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut, int startindex, int endindex)
{
	int index = 0;
	int byteindex = 0;
	for (int i = startindex; i < endindex; i++)
	{

		for (int j = 0; j < 20; j++)
		{
			index = byteindex * 84 + 8 + 30 + j * 2;//8为包头，84为单个子带信息字节数，30为子带数据中数字增益的偏移字节
			Reslut->DigitalGain[i][j] = static_cast<int>(RetData.get()[index]);
		}
		byteindex++;
	}
}

//13AGC增益值查询-8T
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_GetDigitalGainValue8(int DevID)
{
	ClearBuffer();
	int offset = 8;

	m_CtrlCmd.get()[0] = 0xab;
	m_CtrlCmd.get()[1] = 0xab;
	m_CtrlCmd.get()[2] = 0xab;
	m_CtrlCmd.get()[3] = 0xab;
	m_CtrlCmd.get()[4] = 0xab;
	m_CtrlCmd.get()[5] = 0xab;
	m_CtrlCmd.get()[6] = 0xab;
	m_CtrlCmd.get()[7] = 0xab;

	m_CtrlCmd.get()[offset + 0] = 0x5A;
	m_CtrlCmd.get()[offset + 1] = 0x58;
	m_CtrlCmd.get()[offset + 2] = 0x4C;
	m_CtrlCmd.get()[offset + 3] = 0x43;

	m_CtrlCmd.get()[offset + 4] = 0xFF;
	m_CtrlCmd.get()[offset + 5] = 0xFF;
	m_CtrlCmd.get()[offset + 6] = 0x00;
	m_CtrlCmd.get()[offset + 7] = 0x02;

	m_CtrlCmd.get()[offset + 16] = 0x00;
	m_CtrlCmd.get()[offset + 17] = 0x00;

	m_CtrlCmd.get()[offset + 23] = 0x80;

	m_CtrlCmd.get()[offset + 31] = static_cast<char>(DevID);
	m_CtrlCmd.get()[offset + 19] = 0x40;
	m_CtrlCmd.get()[offset + 20] = 0x01;

	if (DevID == 1)
	{
		m_CtrlCmd.get()[offset + 18] = 0x00;//0427JGZC查询网板0

	}
	else if (DevID == 2)
	{
		m_CtrlCmd.get()[offset + 18] = 0x00;

	}
	else if (DevID == 3)
	{
		m_CtrlCmd.get()[offset + 18] = 0x00;

	}
	else if (DevID == 4)
	{
		m_CtrlCmd.get()[offset + 18] = 0x00;

	}

	for (size_t i = 0; i < 96; i++)
	{
		m_CtrlCmd.get()[offset + 32 + i] = 0xFF;
	}

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_GetDigitalGainValue8_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[0] != 0x5A || RetData.get()[1] != 0x58 || RetData.get()[2] != 0x4C || RetData.get()[3] != 0x43 || RetData.get()[5] != 0x7E)
	{
		memset(Reslut, 0x00, sizeof(PDEV_GetDigitalGainValue_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		Reslut->DevID = static_cast<int>(RetData.get()[4] & 0xff) - 1;

		for (int i = 0; i < 20; i++)
		{
			Reslut->RFChannelGain[i] = static_cast<int>(RetData.get()[i + 12]);
		}

		//定义临时数组用于解析链路增益小数部分
		float temp1 = static_cast<float>(((RetData.get()[37]) & 0x01) >> 0) / 128.0;
		float temp2 = static_cast<float>(((RetData.get()[37]) & 0x02) >> 1) / 64.0;
		float temp3 = static_cast<float>(((RetData.get()[37]) & 0x04) >> 2) / 32.0;
		float temp4 = static_cast<float>(((RetData.get()[37]) & 0x08) >> 3) / 16.0;
		float temp5 = static_cast<float>(((RetData.get()[37]) & 0x10) >> 4) / 8.0;
		float temp6 = static_cast<float>(((RetData.get()[37]) & 0x20) >> 5) / 4.0;
		float temp7 = static_cast<float>(((RetData.get()[37]) & 0x40) >> 6) / 2.0;
		float decimals = temp1 + temp2 + temp3 + temp4 + temp5 + temp6 + temp7;
		Reslut->PathGain = static_cast<float>(RetData.get()[36]) + decimals;

		int startbandindex = 0;
		int endbandindex = 0;
		switch (Reslut->DevID)
		{
			// 板卡号为1
		case 1:
			startbandindex = 0;
			endbandindex = 8;
			MakeDigitalGain8(std::move(RetData), Reslut, startbandindex, endbandindex);
			break;
			// 板卡号为2
		case 2:
			startbandindex = 0;
			endbandindex = 12;
			MakeDigitalGain8(std::move(RetData), Reslut, startbandindex, endbandindex);
			break;
			// 板卡号为3
		case 3:
			startbandindex = 0;
			endbandindex = 8;
			MakeDigitalGain8(std::move(RetData), Reslut, startbandindex, endbandindex);
			break;
			// 板卡号为4,zidai:28~57+3（0427JGZC）
		case 4:
			if ((RetData.get()[9])<38)
			{
				startbandindex = 0;
				endbandindex = 10;
				MakeDigitalGain8(std::move(RetData), Reslut, startbandindex, endbandindex);
			}
			if ((RetData.get()[9]) < 48 && (RetData.get()[9]) >= 38)
			{
				startbandindex = 10;
				endbandindex = 20;
				MakeDigitalGain8(std::move(RetData), Reslut, startbandindex, endbandindex);
			}
			if ((RetData.get()[9]) < 60 && (RetData.get()[9]) >= 48)
			{
				startbandindex = 20;
				endbandindex = 32;
				MakeDigitalGain8(std::move(RetData), Reslut, startbandindex, endbandindex);
			}
		default:
			break;
		}
	}
	return 0;
}
void CDevCtrlCmd::MakeDigitalGain8(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut, int startindex, int endindex)
{
	int index = 0;
	int byteindex = 0;
	for (int i = startindex; i < endindex; i++)
	{

		for (int j = 0; j < 8; j++)
		{
			index = byteindex * 84 + 8 + 30 + j * 2;//8为包头，84为单个子带信息字节数，30为子带数据中数字增益的偏移字节
			Reslut->DigitalGain[i][j] = static_cast<int>(RetData.get()[index]);
		}
		byteindex++;
	}
}

//13AGC增益值查询-18T
int CDevCtrlCmd::MakeCmd_GetDigitalGainValue18_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[0] != 0x5A || RetData.get()[1] != 0x58 || RetData.get()[2] != 0x4C || RetData.get()[3] != 0x43 || RetData.get()[5] != 0x7E)
	{
		memset(Reslut, 0x00, sizeof(PDEV_GetDigitalGainValue_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		Reslut->DevID = static_cast<int>(RetData.get()[4] & 0xff) - 1;

		for (int i = 0; i < 20; i++)
		{
			Reslut->RFChannelGain[i] = static_cast<int>(RetData.get()[i + 12]);
		}

		//定义临时数组用于解析链路增益小数部分
		float temp1 = static_cast<float>(((RetData.get()[37]) & 0x01) >> 0) / 128.0;
		float temp2 = static_cast<float>(((RetData.get()[37]) & 0x02) >> 1) / 64.0;
		float temp3 = static_cast<float>(((RetData.get()[37]) & 0x04) >> 2) / 32.0;
		float temp4 = static_cast<float>(((RetData.get()[37]) & 0x08) >> 3) / 16.0;
		float temp5 = static_cast<float>(((RetData.get()[37]) & 0x10) >> 4) / 8.0;
		float temp6 = static_cast<float>(((RetData.get()[37]) & 0x20) >> 5) / 4.0;
		float temp7 = static_cast<float>(((RetData.get()[37]) & 0x40) >> 6) / 2.0;
		float decimals = temp1 + temp2 + temp3 + temp4 + temp5 + temp6 + temp7;
		Reslut->PathGain = static_cast<float>(RetData.get()[36]) + decimals;

		int startbandindex = 0;
		int endbandindex = 0;
		switch (Reslut->DevID)
		{
			// 板卡号为1
		case 1:
			startbandindex = 0;
			endbandindex = 8;
			MakeDigitalGain18(std::move(RetData), Reslut, startbandindex, endbandindex);
			break;
			// 板卡号为2
		case 2:
			startbandindex = 0;
			endbandindex = 8;
			MakeDigitalGain18(std::move(RetData), Reslut, startbandindex, endbandindex);
			break;
			// 板卡号为3
		case 3:
			startbandindex = 0;
			endbandindex = 12;
			MakeDigitalGain18(std::move(RetData), Reslut, startbandindex, endbandindex);
			break;
			// 板卡号为4,zidai:28~57
		case 4:
			if ((RetData.get()[9]) < 38)
			{
				startbandindex = 0;
				endbandindex = 10;
				MakeDigitalGain18(std::move(RetData), Reslut, startbandindex, endbandindex);
			}
			if ((RetData.get()[9]) < 48 && (RetData.get()[9]) >= 38)
			{
				startbandindex = 10;
				endbandindex = 20;
				MakeDigitalGain18(std::move(RetData), Reslut, startbandindex, endbandindex);
			}
			if ((RetData.get()[9]) < 58 && (RetData.get()[9]) >= 48)
			{
				startbandindex = 20;
				endbandindex = 30;
				MakeDigitalGain18(std::move(RetData), Reslut, startbandindex, endbandindex);
			}
			break;
		default:
			break;
		}
	}
	return 0;
}
void CDevCtrlCmd::MakeDigitalGain18(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValue_RESULT Reslut, int startindex, int endindex)
{
	int index = 0;
	int byteindex = 0;
	for (int i = startindex; i < endindex; i++)
	{

		for (int j = 0; j < 18; j++)
		{
			index = byteindex * 84 + 8 + 30 + j * 2;//8为包头，84为单个子带信息字节数，30为子带数据中数字增益的偏移字节
			Reslut->DigitalGain[i][j] = static_cast<int>(RetData.get()[index]);
		}
		byteindex++;
	}
}

//14万兆光口查询指令
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_GetWebStatus(int NetDevID, int NetNum, int TableNum)
{
	ClearBuffer();
	int offset = 8;

	m_CtrlCmd.get()[0] = 0xab;
	m_CtrlCmd.get()[1] = 0xab;
	m_CtrlCmd.get()[2] = 0xab;
	m_CtrlCmd.get()[3] = 0xab;
	m_CtrlCmd.get()[4] = 0xab;
	m_CtrlCmd.get()[5] = 0xab;
	m_CtrlCmd.get()[6] = 0xab;
	m_CtrlCmd.get()[7] = 0xab;

	m_CtrlCmd.get()[offset + 0] = 0x5A;
	m_CtrlCmd.get()[offset + 1] = 0x58;
	m_CtrlCmd.get()[offset + 2] = 0x4C;
	m_CtrlCmd.get()[offset + 3] = 0x43;

	m_CtrlCmd.get()[offset + 4] = 0xFF;
	m_CtrlCmd.get()[offset + 5] = 0xFF;
	m_CtrlCmd.get()[offset + 6] = 0x00;
	m_CtrlCmd.get()[offset + 7] = 0x02;

	m_CtrlCmd.get()[offset + 16] = 0x00;
	m_CtrlCmd.get()[offset + 17] = 0x00;



	m_CtrlCmd.get()[offset + 18] = static_cast<char>(NetDevID);
	m_CtrlCmd.get()[offset + 19] = static_cast<char>(NetNum);
	m_CtrlCmd.get()[offset + 20] = 0x01;
	m_CtrlCmd.get()[offset + 23] = 0x80;
	m_CtrlCmd.get()[offset + 31] = static_cast<char>(TableNum);

	for (size_t i = 0; i < 96; i++)
	{
		m_CtrlCmd.get()[offset + 32 + i] = 0xFF;
	}

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_GetWebStatus_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetWebStatus_CHECKSELF_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[0] != 0x5A || RetData.get()[1] != 0x58 || (RetData.get()[2]) != 0x4C || (RetData.get()[3]) != 0x43)
	{
		return CODE_ERROR_ProclError;
	}
	//else
	//{
	//	*Reslut = 1;
	//}
	return 0;
}
#pragma endregion

#pragma region JGZC采集板控制200102LX修改

//01采集板自检 200102LX
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_DevCC_FPGACheckSelf()
{
	ClearBuffer();

	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x01;
	m_CtrlCmd.get()[5] = 0x00;

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_DevCC_FPGACheckSelf_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_FPGA_CHECKSELF_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x01)
	{
		memset(Reslut, 0x00, sizeof(DEV_FPGA_CHECKSELF_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		int Status1 = static_cast<int>(RetData.get()[6] & 0x0f);
		int a = ((RetData.get()[7] & 0xff) << 4);
		int b = ((RetData.get()[6] & 0xf0) >> 4);

		int Temperature0 = static_cast<int>((RetData.get()[7] & 0xff) << 4) + ((RetData.get()[6] & 0xf0)>>4);
		float Temperature = Temperature0 *503.975 / 4096 - 273.15;
		Reslut->Status1 = Status1;
		Reslut->Temperature = Temperature;

	}
	return 0;
}

//02射频增益模式控制指令 200102LX
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFGainMode(int Mode)
{

	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x02;
	m_CtrlCmd.get()[7] = static_cast<char>(Mode & 0xff);

	return m_CtrlCmd;
}//190422LX修改8通道AGC增益模式控制
int CDevCtrlCmd::MakeCmd_RFGainMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x02)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = 1;
	}
	return 0;
}

//03射频增益模式查询 200102LX
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_DevRF_GetGainMode()
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x03;

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_DevRF_GetGainMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_DevRF_GetGainMode_RESULT Reslut)//回传解析
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x03)
	{
		memset(Reslut, 0x00, sizeof(DEV_DevRF_GetGainMode_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		int Mode = static_cast<int>(RetData.get()[6] & 0xff);
		Reslut->Mode = Mode;
	}
	return 0;
}

//04射频增益值控制指令 200102LX
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFGainValue(int Value)
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x04;
	m_CtrlCmd.get()[7] = static_cast<int>(Value);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_RFGainValue_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x04)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = static_cast<int>(RetData.get()[6] & 0xff);
	}
	return 0;
}

//05射频工作模式指令 200102LX
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFWokeMode(int Mode)
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x05;
	m_CtrlCmd.get()[7] = static_cast<int>(Mode);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_RFWokeMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{

	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x05)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = static_cast<int>(RetData.get()[6] & 0xff);
	}
	return 0;
}

//06射频状态信息回读 200102LX
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFGetDevState()
{
	ClearBuffer();

	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x06;

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_RFGetDevState_Ret_Analysis(std::shared_ptr<unsigned char>&& RetData, PDEV_RF_STATUS_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;

	if ((RetData.get()[1] != 0xCA) || (RetData.get()[5] != 0x06))
	{
		memset(Reslut, 0x00, sizeof(DEV_RF_STATUS_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		Reslut->RF_WorkMode = static_cast<int>(((RetData.get()[4] & 0x40) >> 6) ^ 1);
		Reslut->RF_GainValue = static_cast<int>(RetData.get()[4] & 0x3f);
		unsigned int temperatureABS = ((RetData.get()[7] & 0x1F) << 8) | RetData.get()[6];
		Reslut->RF_Temperature = static_cast<float>((RetData.get()[7] & (1 << 5)) ? (-temperatureABS) : temperatureABS) * 0.03125;
	}
	return 0;
}

//07窄带196路中心频点配置，带宽配置 200102LX
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_NBDDCFreqBand(int Index, int BandWidth, double Freq)
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x07;
	m_CtrlCmd.get()[6] = static_cast<char>(BandWidth & 0xff);
	m_CtrlCmd.get()[7] = static_cast<char>(Index);
	double a = Freq * 279620.266;
	//double x = 2796202.44 + 0.5;
	//int y = round(x);
	//std::cout << y << std::endl;
	int FreqNB = round(a);
	m_CtrlCmd.get()[8] = static_cast<char>((FreqNB &0xff000000) >> 24);
	m_CtrlCmd.get()[9] = static_cast<char>((FreqNB & 0x00ff0000) >> 16);
	m_CtrlCmd.get()[10] = static_cast<char>((FreqNB & 0x0000ff00) >> 8);
	m_CtrlCmd.get()[11] = static_cast<char>(FreqNB & 0x000000ff);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_NBDDCFreqBand_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int *Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x07)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = 1;
	}
	return 0;
}

//08宽带60路中心频点配置 200102LX
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_WBDDCFreqBand(int Index, double Freq)
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x08;
	m_CtrlCmd.get()[7] = static_cast<char>(Index);
	double a = Freq * 279620.266;
	//float x = 2796202.66;
	//int y = round(x);
	//std::cout << y << endl;
	int FreqWB = round(a);
	m_CtrlCmd.get()[8] = static_cast<char>((FreqWB & 0xff000000) >> 24);
	m_CtrlCmd.get()[9] = static_cast<char>((FreqWB & 0x00ff0000) >> 16);
	m_CtrlCmd.get()[10] = static_cast<char>((FreqWB & 0x0000ff00) >> 8);
	m_CtrlCmd.get()[11] = static_cast<char>(FreqWB & 0x000000ff);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_WBDDCFreqBand_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int *Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x08)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = 1;
	}
	return 0;
}

//09射频AGC参数控制
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFGainParameter(int UpLimit, int DownLimit, int HoldTime)
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x09;
	m_CtrlCmd.get()[6] = static_cast<char>(HoldTime & 0xff);
	m_CtrlCmd.get()[7] = static_cast<char>(((UpLimit & 0xff) << 4) + (DownLimit & 0xff));
	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_RFGainParameter_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || (RetData.get()[5]) != 0x09)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = 1;
	}
	return 0;
}

//10数字AGC参数控制
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_DigitalGainParameter(int UpLimit, int DownLimit, int HoldTime)
{

	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x0A;
	m_CtrlCmd.get()[6] = static_cast<char>(HoldTime & 0xff);
	m_CtrlCmd.get()[7] = static_cast<char>(((UpLimit & 0xff) << 4) + (DownLimit & 0xff));
	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_DigitalGainParameter_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x0A)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = 1;
	}
	return 0;
}

//11时标类型控制指令
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_SelectTimeDev(int Mode)
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x0B;
	m_CtrlCmd.get()[7] = static_cast<char>(Mode);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_SelectTimeDev_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_TIMEDEV_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x0B)
	{
		memset(Reslut, 0x00, sizeof(PDEV_TIMEDEV_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		int Mode = static_cast<int>(RetData.get()[6] & 0xff);
		Reslut->Mode = Mode;
	}
	return 0;
}

//12GPS / BD类型控制指令
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_GPSBD(int Mode)
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x0C;
	m_CtrlCmd.get()[7] = static_cast<char>(Mode);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_GPSBD_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GPSBD_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x0C)
	{
		memset(Reslut, 0x00, sizeof(PDEV_GPSBD_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		int Mode = static_cast<int>(RetData.get()[6] & 0xff);
		Reslut->Mode = Mode;
	}
	return 0;
}

//13数字增益24dB开关控制
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_DigitalGainSwitch(int WorkSwitch)
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x0D;
	m_CtrlCmd.get()[7] = static_cast<int>(WorkSwitch & 0xff);

	return m_CtrlCmd;
}

int CDevCtrlCmd::MakeCmd_DigitalGainSwitch_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || (RetData.get()[5]) != 0x0D)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = 1;
	}
	return 0;
}

//NB_AGC
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_NBDC_AGC(int WorkSwitch)
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x16;
	m_CtrlCmd.get()[7] = static_cast<int>(WorkSwitch & 0xff);

	return m_CtrlCmd;
}

int CDevCtrlCmd::MakeCmd_NBDC_AGC_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || (RetData.get()[5]) != 0x16)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = 1;
	}
	return 0;
}

//14系统时钟状态查询
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_CLKStatus()
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x0E;
	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_CLKStatus_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_CLKStatus_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x0E)
	{
		memset(Reslut, 0x00, sizeof(PDEV_CLKStatus_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		int Status = static_cast<int>(RetData.get()[6] & 0xff);
		Reslut->Status = Status;
	}
	return 0;
}

//15Aurora接口channel_up连接状态
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_AuroraStatus()
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x0F;
	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_AuroraStatus_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_CCAURORA_CHECKSELF_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x0F)
	{
		memset(Reslut, 0x00, sizeof(PDEV_CCAURORA_CHECKSELF_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		int AuroraX1 = static_cast<int>(RetData.get()[7] & 0x0f);//1：通信正常，0：通信异常
		Reslut->AuroraX1 = AuroraX1;
		int AuroraX4_1 = static_cast<int>(RetData.get()[6] & 0xf0 >> 4);//1：通信正常，0：通信异常
		Reslut->AuroraX4_1 = AuroraX4_1;
		int AuroraX4_2 = static_cast<int>(RetData.get()[6] & 0x0f);//1：通信正常，0：通信异常
		Reslut->AuroraX4_2 = AuroraX4_2;

	}
	return 0;
}

//16FPGA版本号查询(采集板)
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_GetFPGAVersion()
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x10;

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_GetFPGAVersion_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetFPGAVersion_CHECKSELF_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x10)
	{
		memset(Reslut, 0x00, sizeof(PDEV_GetFPGAVersion_CHECKSELF_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		Reslut->manufacturers = 754;
		Reslut->netF1_Type[0] = static_cast<int>((RetData.get()[9] >> 4) & 0xff);
		Reslut->netF1_integer[0] = static_cast<int>(RetData.get()[9] & 0x0f);

		Reslut->netF1_decimal[0] = static_cast<int>((RetData.get()[8] >> 4) & 0xff);
		Reslut->netF2_Type[0] = static_cast<int>(RetData.get()[8] & 0x0f);

		Reslut->netF2_integer[0] = static_cast<int>((RetData.get()[11] >> 4) & 0xff);
		Reslut->netF2_decimal[0] = static_cast<int>(RetData.get()[11] & 0x0f);

		Reslut->netK7_Type[0] = static_cast<int>((RetData.get()[10] >> 4) & 0xff);
		Reslut->netK7_integer[0] = static_cast<int>(RetData.get()[10] & 0x0f);

		Reslut->netK7_decimal[0] = static_cast<int>(RetData.get()[14] & 0x0f);


		Reslut->sampleK7_Type[0] = static_cast<int>((RetData.get()[4] >> 4) & 0xff);
		Reslut->sampleK7_integer[0] = static_cast<int>(RetData.get()[4] & 0x0f);

		Reslut->sampleK7_decimal[0] = static_cast<int>(RetData.get()[5] & 0x0f);
		Reslut->sampleV7_Type[0] = static_cast<int>(RetData.get()[7] & 0x0f);

		Reslut->sampleV7_integer[0] = static_cast<int>((RetData.get()[6] >> 4) & 0xff);
		Reslut->sampleV7_decimal[0] = static_cast<int>(RetData.get()[6] & 0x0f);

	}
	return 0;
}
//16FPGA版本号查询(传输板)
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_GetFPGAVersionFC()
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x0A;
	m_CtrlCmd.get()[4] = 0x0C;
	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_GetFPGAVersionFC_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetFPGAVersionFC_CHECKSELF_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xAA)
	{
		memset(Reslut, 0x00, sizeof(PDEV_GetFPGAVersionFC_CHECKSELF_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		Reslut->manufacturers = 754;
		Reslut->netF1_Type[0] = static_cast<int>((RetData.get()[9] >> 4) & 0xff);
		Reslut->netF1_integer[0] = static_cast<int>(RetData.get()[9] & 0x0f);

		Reslut->netF1_decimal[0] = static_cast<int>((RetData.get()[8] >> 4) & 0xff);
		Reslut->netF2_Type[0] = static_cast<int>(RetData.get()[8] & 0x0f);

		Reslut->netF2_integer[0] = static_cast<int>((RetData.get()[11] >> 4) & 0xff);
		Reslut->netF2_decimal[0] = static_cast<int>(RetData.get()[11] & 0x0f);

		Reslut->netK7_Type[0] = static_cast<int>((RetData.get()[10] >> 4) & 0xff);
		Reslut->netK7_integer[0] = static_cast<int>(RetData.get()[10] & 0x0f);

		Reslut->netK7_decimal[0] = static_cast<int>(RetData.get()[14] & 0x0f);

		Reslut->netF1_Type[1] = static_cast<int>((RetData.get()[17] >> 4) & 0xff);
		Reslut->netF1_integer[1] = static_cast<int>(RetData.get()[17] & 0x0f);

		Reslut->netF1_decimal[1] = static_cast<int>((RetData.get()[16] >> 4) & 0xff);
		Reslut->netF2_Type[1] = static_cast<int>(RetData.get()[16] & 0x0f);

		Reslut->netF2_integer[1] = static_cast<int>((RetData.get()[19] >> 4) & 0xff);
		Reslut->netF2_decimal[1] = static_cast<int>(RetData.get()[19] & 0x0f);

		Reslut->netK7_Type[1] = static_cast<int>((RetData.get()[18] >> 4) & 0xff);
		Reslut->netK7_integer[1] = static_cast<int>(RetData.get()[18] & 0x0f);

		Reslut->netK7_decimal[1] = static_cast<int>(RetData.get()[22] & 0x0f);

		Reslut->sampleK7_Type[2] = static_cast<int>((RetData.get()[25] >> 4) & 0xff);
		Reslut->sampleK7_integer[2] = static_cast<int>(RetData.get()[25] & 0x0f);

		Reslut->sampleK7_decimal[2] = static_cast<int>((RetData.get()[24] >> 4) & 0xff);
		Reslut->sampleV7_Type[2] = static_cast<int>(RetData.get()[24] & 0x0f);

		Reslut->sampleV7_integer[2] = static_cast<int>((RetData.get()[27] >> 4) & 0xff);
		Reslut->sampleV7_decimal[2] = static_cast<int>(RetData.get()[27] & 0x0f);

		Reslut->sampleK7_Type[1] = static_cast<int>((RetData.get()[29] >> 4) & 0xff);
		Reslut->sampleK7_integer[1] = static_cast<int>(RetData.get()[29] & 0x0f);

		Reslut->sampleK7_decimal[1] = static_cast<int>((RetData.get()[28] >> 4) & 0xff);
		Reslut->sampleV7_Type[1] = static_cast<int>(RetData.get()[28] & 0x0f);

		Reslut->sampleV7_integer[1] = static_cast<int>((RetData.get()[31] >> 4) & 0xff);
		Reslut->sampleV7_decimal[1] = static_cast<int>(RetData.get()[31] & 0x0f);

		Reslut->sampleK7_Type[0] = static_cast<int>((RetData.get()[33] >> 4) & 0xff);
		Reslut->sampleK7_integer[0] = static_cast<int>(RetData.get()[33] & 0x0f);

		Reslut->sampleK7_decimal[0] = static_cast<int>((RetData.get()[32] >> 4) & 0xff);
		Reslut->sampleV7_Type[0] = static_cast<int>(RetData.get()[32] & 0x0f);

		//std::cout << Reslut->sampleK7_integer[0] << endl;
		//std::cout << Reslut->sampleK7_decimal[0] << endl;
		//std::cout << Reslut->sampleK7_integer[1] << endl;
		//std::cout << Reslut->sampleK7_decimal[1] << endl;
		//std::cout << Reslut->sampleK7_integer[2] << endl;
		//std::cout << Reslut->sampleK7_decimal[2] << endl;
		//std::cout << Reslut->sampleK7_integer[3] << endl;
		//std::cout << Reslut->sampleK7_decimal[3] << endl;
		//std::cout << Reslut->sampleK7_integer[4] << endl;
		//std::cout << Reslut->sampleK7_decimal[4] << endl;

		Reslut->sampleV7_integer[0] = static_cast<int>((RetData.get()[35] >> 4) & 0xff);
		Reslut->sampleV7_decimal[0] = static_cast<int>(RetData.get()[35] & 0x0f);

		Reslut->sampleK7_Type[3] = static_cast<int>((RetData.get()[37] >> 4) & 0xff);
		Reslut->sampleK7_integer[3] = static_cast<int>(RetData.get()[37] & 0x0f);

		Reslut->sampleK7_decimal[3] = static_cast<int>((RetData.get()[36] >> 4) & 0xff);
		Reslut->sampleV7_Type[3] = static_cast<int>(RetData.get()[36] & 0x0f);

		Reslut->sampleV7_integer[3] = static_cast<int>((RetData.get()[39] >> 4) & 0xff);
		Reslut->sampleV7_decimal[3] = static_cast<int>(RetData.get()[39] & 0x0f);

		Reslut->sampleK7_Type[4] = static_cast<int>((RetData.get()[41] >> 4) & 0xff);
		Reslut->sampleK7_integer[4] = static_cast<int>(RetData.get()[41] & 0x0f);

		Reslut->sampleK7_decimal[4] = static_cast<int>((RetData.get()[40] >> 4) & 0xff);
		Reslut->sampleV7_Type[4] = static_cast<int>(RetData.get()[40] & 0x0f);

		Reslut->sampleV7_integer[4] = static_cast<int>((RetData.get()[43] >> 4) & 0xff);
		Reslut->sampleV7_decimal[4] = static_cast<int>(RetData.get()[43] & 0x0f);
	}
	return 0;
}

//17数字增益模式控制
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_DigitalGainMode(int Mode)
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x11;
	m_CtrlCmd.get()[7] = static_cast<char>(Mode & 0xff);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_DigitalGainMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x11)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = 1;
	}
	return 0;
}

//18数字增益模式查询
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_GetDigitalGainMode()
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x12;
	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_GetDigitalGainMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainMode_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x12)
	{
		memset(Reslut, 0x00, sizeof(PDEV_GetDigitalGainMode_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		int Mode = static_cast<int>(RetData.get()[6] & 0xff);
		Reslut->Mode = Mode;
	}
	return 0;
}

//19数字MGC衰减值控制
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_DigitalGainValue(int Value)
{
	ClearBuffer();
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x13;
	m_CtrlCmd.get()[7] = static_cast<char>(Value & 0xff);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_DigitalGainValue_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x13)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = 1;
	}
	return 0;
}

//21窄带DDC时标精度
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_NBDDCAccuracy(int Accuracy)
{
	ClearBuffer();
	for (int i = 0; i < 1100; i++)
	{
		m_CtrlCmd.get()[i] = 0xCA;
	}
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x14;
	m_CtrlCmd.get()[6] = static_cast<char>((Accuracy & 0x0f00) >> 8);
	m_CtrlCmd.get()[7] = static_cast<char>(Accuracy & 0xff);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_NBDDCAccuracy_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x14)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = 1;
	}
	return 0;
}

//22FFT时标精度
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_FFTAccuracy(int Accuracy)
{
	ClearBuffer();
	for (int i = 0; i < 1100; i++)
	{
		m_CtrlCmd.get()[i] = 0xCA;
	}
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = 0x01;
	m_CtrlCmd.get()[4] = 0x15;
	m_CtrlCmd.get()[6] = static_cast<char>((Accuracy & 0x0f00) >> 8);
	m_CtrlCmd.get()[7] = static_cast<char>(Accuracy & 0xff);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_FFTAccuracy_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0x15)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = 1;
	}
	return 0;
}

//23JGZC查询AGC增益
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_GetDigitalGainValueJGZC(int DevID)
{
	ClearBuffer();
	int offset = 8;

	m_CtrlCmd.get()[0] = 0xab;
	m_CtrlCmd.get()[1] = 0xab;
	m_CtrlCmd.get()[2] = 0xab;
	m_CtrlCmd.get()[3] = 0xab;
	m_CtrlCmd.get()[4] = 0xab;
	m_CtrlCmd.get()[5] = 0xab;
	m_CtrlCmd.get()[6] = 0xab;
	m_CtrlCmd.get()[7] = 0xab;

	m_CtrlCmd.get()[offset + 0] = 0x5A;
	m_CtrlCmd.get()[offset + 1] = 0x58;
	m_CtrlCmd.get()[offset + 2] = 0x4C;
	m_CtrlCmd.get()[offset + 3] = 0x43;

	m_CtrlCmd.get()[offset + 4] = 0xFF;
	m_CtrlCmd.get()[offset + 5] = 0xFF;
	m_CtrlCmd.get()[offset + 6] = 0x00;
	m_CtrlCmd.get()[offset + 7] = 0x02;

	m_CtrlCmd.get()[offset + 16] = 0x00;
	m_CtrlCmd.get()[offset + 17] = 0x00;

	m_CtrlCmd.get()[offset + 23] = 0x80;

	m_CtrlCmd.get()[offset + 31] = static_cast<char>(DevID);
	m_CtrlCmd.get()[offset + 19] = 0x40;
	m_CtrlCmd.get()[offset + 20] = 0x01;

	if (DevID == 1)
	{
		m_CtrlCmd.get()[offset + 18] = 0x00;//0427JGZC查询网板0

	}
	else if (DevID == 2)
	{
		m_CtrlCmd.get()[offset + 18] = 0x00;

	}
	else if (DevID == 3)
	{
		m_CtrlCmd.get()[offset + 18] = 0x00;

	}
	else if (DevID == 4)
	{
		m_CtrlCmd.get()[offset + 18] = 0x00;

	}
	else if (DevID == 5)
	{
		m_CtrlCmd.get()[offset + 18] = 0x00;

	}
	for (size_t i = 0; i < 96; i++)
	{
		m_CtrlCmd.get()[offset + 32 + i] = 0xFF;
	}

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_GetDigitalGainValueJGZC_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GetDigitalGainValueJGZC_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[0] != 0x5A || RetData.get()[1] != 0x58 || RetData.get()[2] != 0x4C || RetData.get()[3] != 0x43 || RetData.get()[5] != 0x7E)
	{
		memset(Reslut, 0x00, sizeof(PDEV_GetDigitalGainValueJGZC_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{

		for (int i = 0; i < 60; i++)
		{
			Reslut->DigitalGain[i] = static_cast<int>(RetData.get()[i + 16]);
		}

	}
	return 0;
}

#pragma endregion

#pragma region 设备自检

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_DevRC_CheckSelf()
{
	ClearBuffer();

	m_CtrlCmd.get()[0] = 0x07;
	m_CtrlCmd.get()[1] = 0x54;
	m_CtrlCmd.get()[2] = 0xAB;
	m_CtrlCmd.get()[3] = 0xC0;
	m_CtrlCmd.get()[4] = 0x0A;
	m_CtrlCmd.get()[5] = 0xBC;
	m_CtrlCmd.get()[6] = 0x75;
	m_CtrlCmd.get()[7] = 0x40;

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_DevRC_CheckSelf_Ret_Analysis8(std::shared_ptr<unsigned char> && RetData, PDEV_RC_CHECKSELF_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;

	if (RetData.get()[0] != 0x54 || RetData.get()[1] != 0x07)
	{
		memset(Reslut, 0x00, sizeof(DEV_RC_CHECKSELF_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{

		/*for (int i = 0; i < 13; i++)
		{
			if (RetData.get()[4 * i + 28] == 0xBB)
			{
				Reslut->Status[i] = 0x01;
			}
			else
			{
				Reslut->Status[i] = 0x00;
			}
		}*/

		/*Reslut->DevRCVice = 0x01;
			Reslut->DevRCNet1 = 0x01;
			Reslut->DevRCNet2 = 0x01;
			Reslut->DevRCNet3 = 0x01;
			Reslut->DevRCNet4 = 0x01;
			Reslut->DevRCFPGA1 = 0x01;
			Reslut->DevRCFPGA2 = 0x01;
			Reslut->DevRCNet5 = 0x00;
			Reslut->DevRCNet6 = 0x00;
			Reslut->DevRCNet7 = 0x00;
			Reslut->DevRCNet8 = 0x00;
			Reslut->DevRCFPGA3 = 0x00;
			Reslut->DevRCFPGA4 = 0x00;*/

		if (RetData.get()[28] == 0xBB)
		{
			Reslut->DevRCVice = 0x01;
		}
		else
		{
			Reslut->DevRCVice = 0x00;
		}

		/*if (RetData.get()[32] == 0xBB)
		{
			Reslut->DevRCNet1 = 0x01;
		}
		else
		{
			Reslut->DevRCNet1 = 0x00;
		}

		if (RetData.get()[36] == 0xBB)
		{
			Reslut->DevRCNet2 = 0x01;
		}
		else
		{
			Reslut->DevRCNet2 = 0x00;
		}

		if (RetData.get()[40] == 0xBB)
		{
			Reslut->DevRCNet3 = 0x01;
		}
		else
		{
			Reslut->DevRCNet3 = 0x00;
		}

		if (RetData.get()[44] == 0xBB)
		{
			Reslut->DevRCNet4 = 0x01;
		}
		else
		{
			Reslut->DevRCNet4 = 0x00;
		}*/

		if (RetData.get()[48] == 0xBB)
		{
			Reslut->DevRCFPGA1 = 0x01;
		}
		else
		{
			Reslut->DevRCFPGA1 = 0x00;
		}

		if (RetData.get()[52] == 0xBB)
		{
			Reslut->DevRCFPGA2 = 0x01;
		}
		else
		{
			Reslut->DevRCFPGA2 = 0x00;
		}

		/*if (RetData.get()[56] == 0xBB)
		{
			Reslut->DevRCNet5 = 0x01;
		}
		else
		{
			Reslut->DevRCNet5 = 0x00;
		}

		if (RetData.get()[60] == 0xBB)
		{
			Reslut->DevRCNet6 = 0x01;
		}
		else
		{
			Reslut->DevRCNet6 = 0x00;
		}

		if (RetData.get()[64] == 0xBB)
		{
			Reslut->DevRCNet7 = 0x01;
		}
		else
		{
			Reslut->DevRCNet7 = 0x00;
		}

		if (RetData.get()[68] == 0xBB)
		{
			Reslut->DevRCNet8 = 0x01;
		}
		else
		{
			Reslut->DevRCNet8 = 0x00;
		}

		if (RetData.get()[72] == 0xBB)
		{
			Reslut->DevRCFPGA3 = 0x01;
		}
		else
		{
			Reslut->DevRCFPGA3 = 0x00;
		}

		if (RetData.get()[76] == 0xBB)
		{
			Reslut->DevRCFPGA4 = 0x01;
		}
		else
		{
			Reslut->DevRCFPGA4 = 0x00;
		}*/
	}
	return 0;

}
int CDevCtrlCmd::MakeCmd_DevRC_CheckSelf_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_RC_CHECKSELF_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;

	if (RetData.get()[0] != 0x54 || RetData.get()[1] != 0x07)
	{
		memset(Reslut, 0x00, sizeof(DEV_RC_CHECKSELF_RESULT));
		return CODE_ERROR_ProclError;
	}
	//else
	//{
		//for (int i = 0; i < 13; i++)
		//{
		//	if (RetData.get()[4 * i + 28] == 0xBB)
		//	{
		//		Reslut->Status[i] = 0x01;
		//	}
		//	else
		//	{
		//		Reslut->Status[i] = 0x00;
		//	}
		//}

		/*Reslut->DevRCVice = 0x01;
		Reslut->DevRCNet1 = 0x01;
		Reslut->DevRCNet2 = 0x01;
		Reslut->DevRCNet3 = 0x01;
		Reslut->DevRCNet4 = 0x01;
		Reslut->DevRCFPGA1 = 0x01;
		Reslut->DevRCFPGA2 = 0x01;

		Reslut->DevRCNet5 = 0x01;
		Reslut->DevRCNet6 = 0x01;
		Reslut->DevRCNet7 = 0x01;
		Reslut->DevRCNet8 = 0x01;
		Reslut->DevRCFPGA3 = 0x01;
		Reslut->DevRCFPGA4 = 0x01;*/
		std::cout << RetData.get()[28] << std::endl;
		std::cout << RetData.get()[48] << std::endl;
		std::cout << RetData.get()[52] << std::endl;


		if (RetData.get()[28] == 0xBB)
		{
			Reslut->DevRCVice = 0x01;
		}
		else
		{
			Reslut->DevRCVice = 0x00;
		}

		if (RetData.get()[32] == 0xBB)
		{
			Reslut->DevRCNet1 = 0x01;
		}
		else
		{
			Reslut->DevRCNet1 = 0x00;
		}

		if (RetData.get()[36] == 0xBB)
		{
			Reslut->DevRCNet2 = 0x01;
		}
		else
		{
			Reslut->DevRCNet2 = 0x00;
		}

		if (RetData.get()[40] == 0xBB)
		{
			Reslut->DevRCNet3 = 0x01;
		}
		else
		{
			Reslut->DevRCNet3 = 0x00;
		}

		if (RetData.get()[44] == 0xBB)
		{
			Reslut->DevRCNet4 = 0x01;
		}
		else
		{
			Reslut->DevRCNet4 = 0x00;
		}

		if (RetData.get()[48] == 0xBB)
		{
			Reslut->DevRCFPGA1 = 0x01;
		}
		else
		{
			Reslut->DevRCFPGA1 = 0x00;
		}

		if (RetData.get()[52] == 0xBB)
		{
			Reslut->DevRCFPGA2 = 0x01;
		}
		else
		{
			Reslut->DevRCFPGA2 = 0x00;
		}

		/*if (RetData.get()[56] == 0xBB)
		{
			Reslut->DevRCNet5 = 0x01;
		}
		else
		{
			Reslut->DevRCNet5 = 0x00;
		}

		if (RetData.get()[60] == 0xBB)
		{
			Reslut->DevRCNet6 = 0x01;
		}
		else
		{
			Reslut->DevRCNet6 = 0x00;
		}

		if (RetData.get()[64] == 0xBB)
		{
			Reslut->DevRCNet7 = 0x01;
		}
		else
		{
			Reslut->DevRCNet7 = 0x00;
		}

		if (RetData.get()[68] == 0xBB)
		{
			Reslut->DevRCNet8 = 0x01;
		}
		else
		{
			Reslut->DevRCNet8 = 0x00;
		}

		if (RetData.get()[72] == 0xBB)
		{
			Reslut->DevRCFPGA3 = 0x01;
		}
		else
		{
			Reslut->DevRCFPGA3 = 0x00;
		}

		if (RetData.get()[76] == 0xBB)
		{
			Reslut->DevRCFPGA4 = 0x01;
		}
		else
		{
			Reslut->DevRCFPGA4 = 0x00;
		}*/
	return 0;
}

#pragma endregion

#pragma region FPGA程序下载
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_DevRC_FPGAErasure(int FlashID)
{
	ClearBuffer();

	m_CtrlCmd.get()[0] = 0x11;
	m_CtrlCmd.get()[1] = 0x22;
	m_CtrlCmd.get()[2] = 0x33;
	m_CtrlCmd.get()[3] = 0x44;
	m_CtrlCmd.get()[6] = static_cast<char>(FlashID);
	m_CtrlCmd.get()[7] = 0x01;
	m_CtrlCmd.get()[23] = 0x01;

	int checknum = 0;
	for (int i = 0; i < 24; i++)
	{
		checknum += m_CtrlCmd.get()[i];
	}

	m_CtrlCmd.get()[24] = static_cast<char>((checknum >> 24) & 0xff);
	m_CtrlCmd.get()[25] = static_cast<char>((checknum >> 16) & 0xff);
	m_CtrlCmd.get()[26] = static_cast<char>((checknum >> 8) & 0xff);
	m_CtrlCmd.get()[27] = static_cast<char>(checknum & 0xff);

	return m_CtrlCmd;
}
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_DevRC_FPGAWriteEnd(int FlashID)
{
	ClearBuffer();

	m_CtrlCmd.get()[0] = 0x11;
	m_CtrlCmd.get()[1] = 0x22;
	m_CtrlCmd.get()[2] = 0x33;
	m_CtrlCmd.get()[3] = 0x44;
	m_CtrlCmd.get()[6] = static_cast<char>(FlashID);
	m_CtrlCmd.get()[7] = 0x01;
	m_CtrlCmd.get()[23] = 0x03;

	int checknum = 0;
	for (int i = 0; i < 24; i++)
	{
		checknum += m_CtrlCmd.get()[i];
	}

	m_CtrlCmd.get()[24] = static_cast<char>((checknum >> 24) & 0xff);
	m_CtrlCmd.get()[25] = static_cast<char>((checknum >> 16) & 0xff);
	m_CtrlCmd.get()[26] = static_cast<char>((checknum >> 8) & 0xff);
	m_CtrlCmd.get()[27] = static_cast<char>(checknum & 0xff);

	return m_CtrlCmd;
}
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_DevRC_FPGALoad(int FlashID)
{
	ClearBuffer();

	m_CtrlCmd.get()[0] = 0x11;
	m_CtrlCmd.get()[1] = 0x22;
	m_CtrlCmd.get()[2] = 0x33;
	m_CtrlCmd.get()[3] = 0x44;
	m_CtrlCmd.get()[6] = static_cast<char>(FlashID);
	m_CtrlCmd.get()[7] = 0x01;
	m_CtrlCmd.get()[23] = 0x04;

	int checknum = 0;
	for (int i = 0; i < 24; i++)
	{
		checknum += m_CtrlCmd.get()[i];
	}

	m_CtrlCmd.get()[24] = static_cast<char>((checknum >> 24) & 0xff);
	m_CtrlCmd.get()[25] = static_cast<char>((checknum >> 16) & 0xff);
	m_CtrlCmd.get()[26] = static_cast<char>((checknum >> 8) & 0xff);
	m_CtrlCmd.get()[27] = static_cast<char>(checknum & 0xff);

	return m_CtrlCmd;
}
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_DevRC_FPGAWriting(int FlashID, int index, char* buffer, int filesize)
{
	ClearBuffer();


	m_CtrlCmd_RCFPGA.get()[0] = 0x11;
	m_CtrlCmd_RCFPGA.get()[1] = 0x22;
	m_CtrlCmd_RCFPGA.get()[2] = 0x33;
	m_CtrlCmd_RCFPGA.get()[3] = 0x44;
	m_CtrlCmd_RCFPGA.get()[6] = static_cast<char>(FlashID);
	m_CtrlCmd_RCFPGA.get()[7] = 0x01;
	m_CtrlCmd_RCFPGA.get()[23] = 0x02;

	if (index == 1)
	{
		m_CtrlCmd_RCFPGA.get()[27] = 0x01;
		m_CtrlCmd_RCFPGA.get()[28] = static_cast<char>((filesize >> 24) & 0xff);
		m_CtrlCmd_RCFPGA.get()[29] = static_cast<char>((filesize >> 16) & 0xff);
		m_CtrlCmd_RCFPGA.get()[30] = static_cast<char>((filesize >> 8) & 0xff);
		m_CtrlCmd_RCFPGA.get()[31] = static_cast<char>(filesize & 0xff);

		memcpy(m_CtrlCmd_RCFPGA.get() + 32, buffer, 1020);
	}
	else
	{

		m_CtrlCmd_RCFPGA.get()[24] = static_cast<char>((index >> 24) & 0xff);
		m_CtrlCmd_RCFPGA.get()[25] = static_cast<char>((index >> 16) & 0xff);
		m_CtrlCmd_RCFPGA.get()[26] = static_cast<char>((index >> 8) & 0xff);
		m_CtrlCmd_RCFPGA.get()[27] = static_cast<char>(index & 0xff);

		memcpy(m_CtrlCmd_RCFPGA.get() + 28, buffer, 1024);
	}

	return m_CtrlCmd_RCFPGA;
}
int CDevCtrlCmd::MakeCmd_DevRC_FPGAWriting_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_RCFPGA_LOAD_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	std::cout << "返回值是否为0x77:" <<std::to_string(RetData.get()[0]) << std::endl;
	if (RetData.get()[0] != 0x77)
	{
		memset(Reslut, 0x00, sizeof(PDEV_RCFPGA_LOAD_RESULT));
		return CODE_ERROR_ProclError;
	}
	if (RetData.get()[0] = 0x77)
	{
		int Status = static_cast<int>(RetData.get()[34] & 0xff);
		Reslut->Status = Status;
		return 0;
	}
	else
		return 1;

}



std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_DevCC_FPGAErasure(int DevID)
{
	ClearBuffer();

	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[1] = 0x00;
	m_CtrlCmd.get()[2] = 0x00;
	m_CtrlCmd.get()[3] = static_cast<char>(DevID);
	m_CtrlCmd.get()[4] = 0xDE;
	m_CtrlCmd.get()[5] = 0x00;
	m_CtrlCmd.get()[6] = 0x00;
	m_CtrlCmd.get()[7] = 0x01;

	return m_CtrlCmd;
}


std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_DevCC_FPGAVersion(int DevID, int Version)
{
	ClearBuffer();


	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[1] = 0x00;
	m_CtrlCmd.get()[2] = 0x00;
	m_CtrlCmd.get()[3] = static_cast<char>(DevID);
	m_CtrlCmd.get()[4] = 0xFE;
	m_CtrlCmd.get()[5] = 0x00;
	m_CtrlCmd.get()[6] = static_cast<char>((Version >> 8) & 0xff);
	m_CtrlCmd.get()[7] = static_cast<char>(Version & 0xff);

	return m_CtrlCmd;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_DevCC_FPGAWriting(int DevID, int index, char* buffer)
{
	ClearBuffer();

	m_CtrlCmd_CCFPGA.get()[0] = 0xCA;
	m_CtrlCmd_CCFPGA.get()[1] = 0x00;
	m_CtrlCmd_CCFPGA.get()[2] = 0x00;
	m_CtrlCmd_CCFPGA.get()[3] = static_cast<char>(DevID);
	m_CtrlCmd_CCFPGA.get()[4] = 0xFA;
	m_CtrlCmd_CCFPGA.get()[5] = static_cast<char>((index >> 8) & 0xff);
	m_CtrlCmd_CCFPGA.get()[6] = static_cast<char>(index & 0xff);
	//m_CtrlCmd_CCFPGA.get()[7] = static_cast<char>(index & 0xff);

	for (int i = 0; i < 256; i++)
	{
		m_CtrlCmd_CCFPGA.get()[4 * i + 8] = 0xFD;
		m_CtrlCmd_CCFPGA.get()[4 * i + 9] = 0x00;
		m_CtrlCmd_CCFPGA.get()[4 * i + 10] = turn_around(buffer[i * 2]);
		m_CtrlCmd_CCFPGA.get()[4 * i + 11] = turn_around(buffer[i * 2 + 1]);
	}



	return m_CtrlCmd_CCFPGA;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_DevCC_FPGAWriteEnd(int DevID)
{
	ClearBuffer();

	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[1] = 0x00;
	m_CtrlCmd.get()[2] = 0x00;
	m_CtrlCmd.get()[3] = static_cast<char>(DevID);
	m_CtrlCmd.get()[4] = 0xFC;
	m_CtrlCmd.get()[5] = 0x80;
	m_CtrlCmd.get()[6] = 0x80;
	m_CtrlCmd.get()[7] = 0xCF;

	return m_CtrlCmd;
}

int CDevCtrlCmd::MakeCmd_DevCC_FPGA_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_CCFPGA_LOAD_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;

	if (RetData.get()[1] != 0xCA)
	{
		memset(Reslut, 0x00, sizeof(PDEV_CCFPGA_LOAD_RESULT));
		return CODE_ERROR_ProclError;
	}
	//std::string ss = std::to_string(RetData.get()[5]);

	else if (RetData.get()[5] == 0xDE || RetData.get()[5] == 0xFD)
	{

		int Status = static_cast<int>(RetData.get()[6] & 0xff);
		Reslut->Status = Status;
		std::cout << "写入是否成功：" << std::to_string(Status) << std::endl;
		return 0;
	}


	//	else
	//	{
	//		Reslut->Status = 1;
	//	}
	/////*	else if (RetData.get()[5] == 0xFD)
	////	{
	////		int Status = static_cast<int>(RetData.get()[6] & 0xff);
	////		*/Reslut->Status = Status;
	////	}

	//return 0;
	else
		return 1;
}




#pragma endregion

#pragma region 万兆网数据传输备份
//数据源同步(复位功能)
std::pair<std::shared_ptr<unsigned char>, int > CDevCtrlCmd::MakeCmd_DataReset_B(int boardNum, int enable, int dataSource)
{
	std::pair<std::shared_ptr<unsigned char>, int > p;
	std::shared_ptr<unsigned char> t(new unsigned char[256], [](unsigned char *p){ delete[]p; });
	p.first = t;

	/*****************以下为了得到帧头*******************************************/
	UINT16 nSerial = 0;//命令流水号（包序号）
	UINT8 nBoardNo = boardNum; //板卡号（0, 1）
	UINT8 nNetCardNo = 0;//万兆网口号（0~3）,对应光口1~4
	int nOption = boardNum; //主从选择, 0:从 1:主
	UINT32 nSrcIP = 0; //万兆网口源IP
	UINT64 nSrcMAC = 0;//万兆网口源MAC
	UINT32 nInterval = 0;  //MAC帧插入时钟计数:默认为0
	UINT16 nPacketLen = 8192;//UDP分片长度门限:1472
	UINT32 nBitControl = 0;//位控制寄存器
	UINT16 nHeartPort = 0;//连接心跳包端口（未使用）
	UINT32 nSlicesClock = 0;//分片间插入时钟周期数

	nOption = 1;
	nBoardNo = boardNum;

	unsigned char theDstCmd[256];//输出
	int theCmdLen = 0;//输出
	SrcParam_t SrcParam;
	SrcParam.GetCmd(theDstCmd, &theCmdLen, nSerial, nBoardNo, nNetCardNo, nOption,
		nSrcIP, nSrcMAC, nInterval, nPacketLen, nBitControl, nHeartPort, nSlicesClock);
	/**********************************************************************/
	theCmdLen = 8 + 24 + 8;//规定死

	theDstCmd[32] = 0;
	theDstCmd[33] = 0;
	theDstCmd[34] = 0;
	theDstCmd[35] = 7;//地址为0x07

	theDstCmd[36] = 0;
	theDstCmd[37] = 0;
	theDstCmd[38] = 0;
	theDstCmd[39] = 0;
	theDstCmd[39] = dataSource * 2 + enable;//D[1]:数据源选择 ； D[0]控制信号

	memcpy(t.get(), theDstCmd, theCmdLen);
	p.second = theCmdLen;
	return p;
}

//目的IP设置
std::pair<std::shared_ptr<unsigned char>, int > CDevCtrlCmd::MakeCmd_SetDataDestIP_B(int BoardID, int NetID, int connectionId, char *ip, unsigned short disPort, unsigned short sourcePort)
{
	std::pair<std::shared_ptr<unsigned char>, int > p;
	std::shared_ptr<unsigned char> t(new unsigned char[256], [](unsigned char *p){ delete[]p; });
	p.first = t;
	/*以下为wdx建立*/
	ClearBuffer();

	UINT16 nSerial = 0;//命令流水号（包序号）
	UINT8 nBoardNo = 0; //板卡号（0, 1）
	UINT8 nNetCardNo = 0;//万兆网口号（0~3）,对应光口1~4
	int nOption = 0; //主从选择, 0:从 1:主
	UINT32 IPRamAddr = 0;//目的IP地址 RAM地址
	UINT32 nIP = 0; //目的IP地址
	UINT32 PortRamAddr = 0;//UDP源端口/目的端口 RAM地址
	UINT16 nSrcPort = 0;//UDP源端口
	UINT16 nDstPort = 0;//UDP目的端口

	unsigned char theDstCmd[256];//输出
	int theCmdLen = 0;//输出

	DstParam_t DstParam;


	if (NetID <= 4 && NetID >= 1)//主板
	{
		nBoardNo = 1;
		nOption = 1;
		nNetCardNo = NetID - 1;
	}
	else if (NetID <= 8 && NetID >= 5)//从板
	{
		nOption = 0;
		nBoardNo = 0;
		nNetCardNo = NetID - 4 - 1;
	}

	nOption = 1;
	nBoardNo = BoardID;


	IPRamAddr = connectionId * 2;
	PortRamAddr = connectionId * 2 + 1;

	int ipCtr[4];
	atob1(ip, ipCtr);
	nIP = ipCtr[0] * 256 * 256 * 256 + ipCtr[1] * 256 * 256 + ipCtr[2] * 256 + ipCtr[3];

	nSrcPort = sourcePort;//源端口
	nDstPort = disPort;//目的端口


	DstParam.GetCmd(theDstCmd, &theCmdLen, nSerial, nBoardNo, nNetCardNo, nOption,
		IPRamAddr, nIP, PortRamAddr, nSrcPort, nDstPort);

	/*得到命令帧*/
	memcpy(t.get(), theDstCmd, theCmdLen);
	p.second = theCmdLen;
	return p;
}

/*wdx修改*/
std::pair<std::shared_ptr<unsigned char>, int > CDevCtrlCmd::MakeCmd_SetDataSourceIP_B(int BoardID, int NetID, char *ip, char *mac, int PacketLen, int Interval, int SlicesClock)
{

	std::pair<std::shared_ptr<unsigned char>, int > p;
	std::shared_ptr<unsigned char> t(new unsigned char[256], [](unsigned char *p){ delete[]p; });
	p.first = t;


	/*以下为wdx建立*/
	UINT16 nSerial = 0;//命令流水号（包序号）
	UINT8 nBoardNo = 0; //板卡号（0, 1）
	UINT8 nNetCardNo = 2;//万兆网口号（0~3）,对应光口1~4
	int nOption = 0; //主从选择, 0:从 1:主
	UINT32 nSrcIP = 0; //万兆网口源IP
	UINT64 nSrcMAC = 0;//万兆网口源MAC
	UINT32 nInterval = 16;  //MAC帧插入时钟计数:默认为0
	UINT16 nPacketLen = 8000;//UDP分片长度门限:1472
	UINT32 nBitControl = 0;//位控制寄存器
	UINT16 nHeartPort = 0;//连接心跳包端口（未使用）
	UINT32 nSlicesClock = 0;//分片间插入时钟周期数

	unsigned char theDstCmd[256];//输出
	int theCmdLen = 0;//输出
	SrcParam_t SrcParam;

	/*以下为参数转换*/
	nSrcMAC = SrcParam.GetMAC((unsigned char *)mac);

	if (NetID <= 4 && NetID >= 1)//主板
	{
		nBoardNo = 1;
		nOption = 1;
		nNetCardNo = NetID - 1;
	}
	else if (NetID <= 8 && NetID >= 5)//从板
	{
		nOption = 0;
		nBoardNo = 0;
		nNetCardNo = NetID - 4 - 1;
	}

	nOption = 1;
	nBoardNo = BoardID;

	int ipCtr[4];
	atob1(ip, ipCtr);
	nSrcIP = ipCtr[0] * 256 * 256 * 256 + ipCtr[1] * 256 * 256 + ipCtr[2] * 256 + ipCtr[3];
	nHeartPort = 0;
	nPacketLen = PacketLen;
	nInterval = Interval;
	nSlicesClock = SlicesClock;
	SrcParam.GetCmd(theDstCmd, &theCmdLen, nSerial, nBoardNo, nNetCardNo, nOption,
		nSrcIP, nSrcMAC, nInterval, nPacketLen, nBitControl, nHeartPort, nSlicesClock);

	/*得到命令帧*/
	memcpy(t.get(), theDstCmd, theCmdLen);
	p.second = theCmdLen;
	return p;

}
/*使能*/
std::pair<std::shared_ptr<unsigned char>, int >CDevCtrlCmd::MakeCmd_SetDataSendSource_B(int BoardID, int NetID, int connectionId, int en)
{
	std::pair<std::shared_ptr<unsigned char>, int > p;
	std::shared_ptr<unsigned char> t(new unsigned char[256], [](unsigned char *p){ delete[]p; });
	p.first = t;

	/*以下为wdx建立*/

	UINT16 nSerial = 0;//命令流水号（包序号）
	UINT8 nBoardNo = 0; //板卡号（0, 1）
	UINT8 nNetCardNo = 0;//万兆网口号（0~3）,对应光口1~4
	int nOption = 0; //主从选择, 0:从 1:主
	UINT32 EnableRamAddr = 0;//连接使能 RAM地址
	UINT32 nEnable = 0;//连接使能, 0:禁止 1:使能

	unsigned char theDstCmd[256] = { 0 };//输出
	int theCmdLen = 0;//输出
	LinkEnable_t LinkEnable;

	if (NetID <= 4 && NetID >= 1)//主板
	{
		nBoardNo = 1;
		nOption = 1;
		nNetCardNo = NetID - 1;
	}
	else if (NetID <= 8 && NetID >= 5)//从板
	{
		nOption = 0;
		nBoardNo = 0;
		nNetCardNo = NetID - 4 - 1;
	}

	nOption = 1;
	nBoardNo = BoardID;

	EnableRamAddr = connectionId;
	nEnable = en;

	LinkEnable.GetCmd(theDstCmd, &theCmdLen, nSerial, nBoardNo, nNetCardNo, nOption,
		EnableRamAddr, nEnable);

	/*得到命令帧*/
	memcpy(t.get(), theDstCmd, theCmdLen);
	p.second = theCmdLen;
	return p;


}
#pragma endregion

#pragma region 射频控制

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFFreqBand(int RF_DevID, int DevId, int Channel, int StartFreq, int EndFreq)
{
	ClearBuffer();
	char ChannelCtr = 0x00;
	if (Channel % 2 == 0)
	{
		ChannelCtr = (Channel / 2) << 4;
	}
	else
	{
		ChannelCtr = ((Channel / 2) << 4) | (Channel % 2);
	}

	unsigned long Param1 = (EndFreq >> 12) & 0xFF; //总计20bit有效，向右移动12bit，取高8bit;
	unsigned long Param2 = ((EndFreq & 0xFFF) << 20) + (StartFreq & 0xFFFFF);//EndFreq 取 后12位作为Param前12bit， StartFreq 的低20bit，做Param后20bit。

	m_CtrlCmd.get()[0] = 0xCA;

	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));
	m_CtrlCmd.get()[4] = 0xEC;
	m_CtrlCmd.get()[6] = static_cast<char>(ChannelCtr);
	m_CtrlCmd.get()[7] = static_cast<char>(Param1 & 0xFF);

	m_CtrlCmd.get()[8] = 0xEC;
	m_CtrlCmd.get()[9] = 0x01;
	m_CtrlCmd.get()[10] = static_cast<char>((Param2 >> 24) & 0xFF);
	m_CtrlCmd.get()[11] = static_cast<char>((Param2 >> 16) & 0xFF);

	m_CtrlCmd.get()[12] = 0xEC;
	m_CtrlCmd.get()[13] = 0x02;
	m_CtrlCmd.get()[14] = static_cast<char>((Param2 >> 8) & 0xFF);
	m_CtrlCmd.get()[15] = static_cast<char>(Param2 & 0xFF);

	return m_CtrlCmd;

}
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFFreqBand8(int RF_DevID, int DevId, int Channel, int StartFreq, int EndFreq)
{
	ClearBuffer();
	char ChannelCtr = 0x00;

	ChannelCtr = (Channel << 4) & 0xff;

	unsigned long Param1 = (EndFreq >> 12) & 0xFF; //总计20bit有效，向右移动12bit，取高8bit;
	unsigned long Param2 = ((EndFreq & 0xFFF) << 20) + (StartFreq & 0xFFFFF);//EndFreq 取 后12位作为Param前12bit， StartFreq 的低20bit，做Param后20bit。

	m_CtrlCmd.get()[0] = 0xCA;

	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));
	m_CtrlCmd.get()[4] = 0xEC;
	m_CtrlCmd.get()[6] = static_cast<char>(ChannelCtr);
	m_CtrlCmd.get()[7] = static_cast<char>(Param1 & 0xFF);

	m_CtrlCmd.get()[8] = 0xEC;
	m_CtrlCmd.get()[9] = 0x01;
	m_CtrlCmd.get()[10] = static_cast<char>((Param2 >> 24) & 0xFF);
	m_CtrlCmd.get()[11] = static_cast<char>((Param2 >> 16) & 0xFF);

	m_CtrlCmd.get()[12] = 0xEC;
	m_CtrlCmd.get()[13] = 0x02;
	m_CtrlCmd.get()[14] = static_cast<char>((Param2 >> 8) & 0xFF);
	m_CtrlCmd.get()[15] = static_cast<char>(Param2 & 0xFF);

	return m_CtrlCmd;

}
int CDevCtrlCmd::MakeCmd_RFFreqBand_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDD || ((RetData.get()[6] >> 4) & 0xff) != 0x00)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = static_cast<int>(RetData.get()[6] & 0xf);
	}
	return 0;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_CSFreqBand(int RF_DevID, int DevId, int StartFreq, int EndFreq)
{
	ClearBuffer();
	char ChannelCtr = 0x00;


	unsigned long Param1 = (EndFreq >> 12) & 0xFF; //总计20bit有效，向右移动12bit，取高8bit;
	unsigned long Param2 = ((EndFreq & 0xFFF) << 20) + (StartFreq & 0xFFFFF);//EndFreq 取 后12位作为Param前12bit， StartFreq 的低20bit，做Param后20bit。

	m_CtrlCmd.get()[0] = 0xCA;

	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));
	m_CtrlCmd.get()[4] = 0xEC;

	m_CtrlCmd.get()[5] = 0x10;
	m_CtrlCmd.get()[7] = static_cast<char>(Param1 & 0xFF);

	m_CtrlCmd.get()[8] = 0xEC;
	m_CtrlCmd.get()[9] = 0x11;
	m_CtrlCmd.get()[10] = static_cast<char>((Param2 >> 24) & 0xFF);
	m_CtrlCmd.get()[11] = static_cast<char>((Param2 >> 16) & 0xFF);

	m_CtrlCmd.get()[12] = 0xEC;
	m_CtrlCmd.get()[13] = 0x12;
	m_CtrlCmd.get()[14] = static_cast<char>((Param2 >> 8) & 0xFF);
	m_CtrlCmd.get()[15] = static_cast<char>(Param2 & 0xFF);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_CSFreqBand_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDD || ((RetData.get()[6] >> 4) & 0xff) != 0x05)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = static_cast<int>(RetData.get()[6] & 0xf);
	}
	return 0;
}


std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFPowerSwitch(int RF_DevID, int DevId, int Channel, int PowerSwitch)
{
	ClearBuffer();
	char ChannelCtr = 0x00;
	if (Channel % 2 == 0)
	{
		ChannelCtr = (Channel / 2) << 4;
	}
	else
	{
		ChannelCtr = ((Channel / 2) << 4) | (Channel % 2);
	}
	m_CtrlCmd.get()[0] = 0xCA;


	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));


	m_CtrlCmd.get()[4] = 0xEE;
	m_CtrlCmd.get()[6] = static_cast<char>(ChannelCtr);
	m_CtrlCmd.get()[7] = static_cast<char>(PowerSwitch);

	return m_CtrlCmd;
}
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFPowerSwitch8(int RF_DevID, int DevId, int Channel, int PowerSwitch)
{
	ClearBuffer();
	char ChannelCtr = 0x00;

	ChannelCtr = (Channel << 4) & 0xff;

	m_CtrlCmd.get()[0] = 0xCA;


	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));


	m_CtrlCmd.get()[4] = 0xEE;
	m_CtrlCmd.get()[6] = static_cast<char>(ChannelCtr);
	m_CtrlCmd.get()[7] = static_cast<char>(PowerSwitch);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_RFPowerSwitch_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{

	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDD || ((RetData.get()[6] >> 4) & 0xff) != 0x03)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = static_cast<int>(RetData.get()[6] & 0xf);
	}
	return 0;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFAllParamConfig(int RF_DevID, int DevId, int Channel, std::shared_ptr<RF_PARAM> Param)
{
	ClearBuffer();
	char ChannelCtr = 0x00;
	if (Channel % 2 == 0)
	{
		ChannelCtr = (Channel / 2) << 4;
	}
	else
	{
		ChannelCtr = ((Channel / 2) << 4) | (Channel % 2);
	}
	std::pair<unsigned long, unsigned long> FreqBand = m_FreqBandInfo.at(Param->FreqBand);

	unsigned long Param1 = (FreqBand.second >> 12) & 0xFF; //总计20bit有效，向右移动12bit，取高8bit;
	unsigned long Param2 = ((FreqBand.second & 0xFFF) << 20) + (FreqBand.first & 0xFFFFF);//EndFreq 取 后12位作为Param前12bit， StartFreq 的低20bit，做Param后20bit。


	m_CtrlCmd.get()[0] = 0xCA;


	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));



	m_CtrlCmd.get()[4] = 0xEF;
	m_CtrlCmd.get()[6] = static_cast<char>(ChannelCtr);
	m_CtrlCmd.get()[7] = static_cast<char>(Param1 & 0xFF);

	m_CtrlCmd.get()[8] = 0xEF;
	m_CtrlCmd.get()[9] = 0x01;
	m_CtrlCmd.get()[10] = static_cast<char>((Param2 >> 24) & 0xFF);
	m_CtrlCmd.get()[11] = static_cast<char>((Param2 >> 16) & 0xFF);

	m_CtrlCmd.get()[12] = 0xEF;
	m_CtrlCmd.get()[13] = 0x02;
	m_CtrlCmd.get()[14] = static_cast<char>((Param2 >> 8) & 0xFF);
	m_CtrlCmd.get()[15] = static_cast<char>(Param2 & 0xFF);


	m_CtrlCmd.get()[16] = 0xEF;
	m_CtrlCmd.get()[17] = 0x03;
	m_CtrlCmd.get()[18] = static_cast<char>(Param->GainVlaue);
	m_CtrlCmd.get()[19] = static_cast<char>(Param->WorkMode);

	m_CtrlCmd.get()[20] = 0xEF;
	m_CtrlCmd.get()[21] = 0x04;
	m_CtrlCmd.get()[23] = static_cast<char>(Param->PowerSwitch);

	return m_CtrlCmd;
}
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFAllParamConfig8(int RF_DevID, int DevId, int Channel, std::shared_ptr<RF_PARAM> Param)
{
	ClearBuffer();
	char ChannelCtr = 0x00;

	ChannelCtr = (Channel << 4) & 0xff;

	std::pair<unsigned long, unsigned long> FreqBand = m_FreqBandInfo.at(Param->FreqBand);

	unsigned long Param1 = (FreqBand.second >> 12) & 0xFF; //总计20bit有效，向右移动12bit，取高8bit;
	unsigned long Param2 = ((FreqBand.second & 0xFFF) << 20) + (FreqBand.first & 0xFFFFF);//EndFreq 取 后12位作为Param前12bit， StartFreq 的低20bit，做Param后20bit。


	m_CtrlCmd.get()[0] = 0xCA;


	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));



	m_CtrlCmd.get()[4] = 0xEF;
	m_CtrlCmd.get()[6] = static_cast<char>(ChannelCtr);
	m_CtrlCmd.get()[7] = static_cast<char>(Param1 & 0xFF);

	m_CtrlCmd.get()[8] = 0xEF;
	m_CtrlCmd.get()[9] = 0x01;
	m_CtrlCmd.get()[10] = static_cast<char>((Param2 >> 24) & 0xFF);
	m_CtrlCmd.get()[11] = static_cast<char>((Param2 >> 16) & 0xFF);

	m_CtrlCmd.get()[12] = 0xEF;
	m_CtrlCmd.get()[13] = 0x02;
	m_CtrlCmd.get()[14] = static_cast<char>((Param2 >> 8) & 0xFF);
	m_CtrlCmd.get()[15] = static_cast<char>(Param2 & 0xFF);


	m_CtrlCmd.get()[16] = 0xEF;
	m_CtrlCmd.get()[17] = 0x03;
	m_CtrlCmd.get()[18] = static_cast<char>(Param->GainVlaue);
	m_CtrlCmd.get()[19] = static_cast<char>(Param->WorkMode);

	m_CtrlCmd.get()[20] = 0xEF;
	m_CtrlCmd.get()[21] = 0x04;
	m_CtrlCmd.get()[23] = static_cast<char>(Param->PowerSwitch);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_RFAllParamConfig_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{

	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDD || ((RetData.get()[6] >> 4) & 0xff) != 0x04)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = static_cast<int>(RetData.get()[6] & 0xf);
	}
	return 0;
}


/////等待协议
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFAllGainValue(int RF_DevID, int DevId, int Value)
{
	ClearBuffer();

	m_CtrlCmd.get()[0] = 0xCA;

	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));
	//m_CtrlCmd.get()[4] = 0xEB;
	m_CtrlCmd.get()[7] = static_cast<char>(Value);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_RFAllGainValue_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDD || ((RetData.get()[6] >> 4) & 0xff) != 0x01)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = static_cast<int>(RetData.get()[6] & 0xf);
	}
	return 0;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFAllWokeMode(int RF_DevID, int DevId, int Mode)
{
	ClearBuffer();

	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));


	//m_CtrlCmd.get()[4] = 0xED;

	m_CtrlCmd.get()[7] = static_cast<char>(Mode);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_RFAllWokeMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDD || ((RetData.get()[6] >> 4) & 0xff) != 0x02)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = static_cast<int>(RetData.get()[6] & 0xf);
	}
	return 0;
}


std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_CSGainValue(int RF_DevID, int DevId, int Value)
{
	ClearBuffer();


	m_CtrlCmd.get()[0] = 0xCA;


	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));


	m_CtrlCmd.get()[4] = 0xEB;
	m_CtrlCmd.get()[5] = 0x10;
	m_CtrlCmd.get()[7] = static_cast<char>(Value);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_CSGainValue_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{

	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDD || ((RetData.get()[6] >> 4) & 0xff) != 0x06)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = static_cast<int>(RetData.get()[6] & 0xf);
	}
	return 0;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_CSWorkMode(int RF_DevID, int DevId, int Mode)
{
	ClearBuffer();


	m_CtrlCmd.get()[0] = 0xCA;


	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));


	m_CtrlCmd.get()[4] = 0xED;
	m_CtrlCmd.get()[5] = 0x10;
	m_CtrlCmd.get()[7] = static_cast<char>(Mode);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_CSWorkMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{

	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDD || ((RetData.get()[6] >> 4) & 0xff) != 0x08)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = static_cast<int>(RetData.get()[6] & 0xf);
	}
	return 0;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_CSPowerSwitch(int RF_DevID, int DevId, int PowerSwitch)
{
	ClearBuffer();

	m_CtrlCmd.get()[0] = 0xCA;


	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));


	m_CtrlCmd.get()[4] = 0xEE;
	m_CtrlCmd.get()[5] = 0x10;
	m_CtrlCmd.get()[7] = static_cast<char>(PowerSwitch);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_CSPowerSwitch_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{

	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDD || ((RetData.get()[6] >> 4) & 0xff) != 0x07)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = static_cast<int>(RetData.get()[6] & 0xf);
	}
	return 0;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_CSAllParamConfig(int RF_DevID, int DevId, std::shared_ptr<CS_PARAM> Param)
{
	ClearBuffer();

	std::pair<unsigned long, unsigned long> FreqBand = m_FreqBandInfo.at(Param->FreqBand);

	unsigned long Param1 = (FreqBand.second >> 12) & 0xFF; //总计20bit有效，向右移动12bit，取高8bit;
	unsigned long Param2 = ((FreqBand.second & 0xFFF) << 20) + (FreqBand.first & 0xFFFFF);//EndFreq 取 后12位作为Param前12bit， StartFreq 的低20bit，做Param后20bit。


	m_CtrlCmd.get()[0] = 0xCA;


	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));



	m_CtrlCmd.get()[4] = 0xEF;
	m_CtrlCmd.get()[5] = 0x10;
	m_CtrlCmd.get()[7] = static_cast<char>(Param1 & 0xFF);

	m_CtrlCmd.get()[8] = 0xEF;
	m_CtrlCmd.get()[9] = 0x11;
	m_CtrlCmd.get()[10] = static_cast<char>((Param2 >> 24) & 0xFF);
	m_CtrlCmd.get()[11] = static_cast<char>((Param2 >> 16) & 0xFF);

	m_CtrlCmd.get()[12] = 0xEF;
	m_CtrlCmd.get()[13] = 0x12;
	m_CtrlCmd.get()[14] = static_cast<char>((Param2 >> 8) & 0xFF);
	m_CtrlCmd.get()[15] = static_cast<char>(Param2 & 0xFF);


	m_CtrlCmd.get()[16] = 0xEF;
	m_CtrlCmd.get()[17] = 0x13;
	m_CtrlCmd.get()[18] = static_cast<char>(Param->GainVlaue);
	m_CtrlCmd.get()[19] = static_cast<char>(Param->WorkMode);

	m_CtrlCmd.get()[20] = 0xEF;
	m_CtrlCmd.get()[21] = 0x14;
	m_CtrlCmd.get()[23] = static_cast<char>(Param->PowerSwitch);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_CSAllParamConfig_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{

	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDD || ((RetData.get()[6] >> 4) & 0xff) != 0x09)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = static_cast<int>(RetData.get()[6] & 0xf);
	}
	return 0;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_GSPowerSwitch(int RF_DevID, int DevId, int PowerSwitch)
{
	ClearBuffer();


	m_CtrlCmd.get()[0] = 0xCA;


	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));


	m_CtrlCmd.get()[4] = 0xEE;
	m_CtrlCmd.get()[5] = 0x20;
	m_CtrlCmd.get()[7] = static_cast<char>(PowerSwitch);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_GSPowerSwitch_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{

	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDD || ((RetData.get()[6] >> 4) & 0xff) != 0x0A)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = static_cast<int>(RetData.get()[6] & 0xf);
	}
	return 0;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_GSWorkMode(int RF_DevID, int DevId, int Mode)
{
	ClearBuffer();


	m_CtrlCmd.get()[0] = 0xCA;


	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));


	m_CtrlCmd.get()[4] = 0xED;
	m_CtrlCmd.get()[5] = 0x20;
	m_CtrlCmd.get()[7] = static_cast<char>(Mode);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_GSWorkMode_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{

	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDD || ((RetData.get()[6] >> 4) & 0xff) != 0x0B)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = static_cast<int>(RetData.get()[6] & 0xf);
	}
	return 0;
}

//********************************************************************Lijf*************************************************************//
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_WSWholeStatusZC(int RF_DevID, int DevId, std::shared_ptr<DEV_PARAM>  Param)
{
	ClearBuffer();

	std::pair<unsigned long, unsigned long> FreqBand = m_FreqBandInfoZC.at(Param->FreqBand);

	unsigned long Param1 = (FreqBand.second >> 12) & 0xFF; //总计20bit有效，向右移动12bit，取高8bit;
	unsigned long Param2 = ((FreqBand.second & 0xFFF) << 20) + (FreqBand.first & 0xFFFFF);//EndFreq 取 后12位作为Param前12bit， StartFreq 的低20bit，做Param后20bit。
	unsigned long WorkMode = ((Param->GS_WorkMode & 0xff) << 4) + ((Param->RF_WorkMode & 0xff) << 2) + ((Param->CS_WorkMode) & 0xff);
	unsigned long PowerSwich = ((Param->GS_PowerSwitch & 0xff) << 4) + ((Param->RF_PowerSwitch & 0xff) << 2) + ((Param->CS_PowerSwitch) & 0xff);

	m_CtrlCmd.get()[0] = 0xCA;


	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));



	m_CtrlCmd.get()[4] = 0xE0;
	m_CtrlCmd.get()[5] = 0x00;
	m_CtrlCmd.get()[7] = static_cast<char>(Param1 & 0xFF);

	m_CtrlCmd.get()[8] = 0xE0;
	m_CtrlCmd.get()[9] = 0x01;
	m_CtrlCmd.get()[10] = static_cast<char>((Param2 >> 24) & 0xFF);
	m_CtrlCmd.get()[11] = static_cast<char>((Param2 >> 16) & 0xFF);

	m_CtrlCmd.get()[12] = 0xE0;
	m_CtrlCmd.get()[13] = 0x02;
	m_CtrlCmd.get()[14] = static_cast<char>((Param2 >> 8) & 0xFF);
	m_CtrlCmd.get()[15] = static_cast<char>(Param2 & 0xFF);


	m_CtrlCmd.get()[16] = 0xE0;
	m_CtrlCmd.get()[17] = 0x03;
	m_CtrlCmd.get()[18] = static_cast<char>(Param->GainValue);
	m_CtrlCmd.get()[19] = static_cast<char>(WorkMode & 0xFF);

	m_CtrlCmd.get()[20] = 0xE0;
	m_CtrlCmd.get()[21] = 0x04;
	m_CtrlCmd.get()[23] = static_cast<char>(PowerSwich & 0xFF);

	return m_CtrlCmd;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_WSWholeStatusZC18(int RF_DevID, int DevId, std::shared_ptr<DEV_PARAM>  Param)
{
	ClearBuffer();

	std::pair<unsigned long, unsigned long> FreqBand = m_FreqBandInfoZC18.at(Param->FreqBand);

	unsigned long Param1 = (FreqBand.second >> 12) & 0xFF; //总计20bit有效，向右移动12bit，取高8bit;
	unsigned long Param2 = ((FreqBand.second & 0xFFF) << 20) + (FreqBand.first & 0xFFFFF);//EndFreq 取 后12位作为Param前12bit， StartFreq 的低20bit，做Param后20bit。
	unsigned long WorkMode = ((Param->GS_WorkMode & 0xff) << 4) + ((Param->RF_WorkMode & 0xff) << 2) + ((Param->CS_WorkMode) & 0xff);
	unsigned long PowerSwich = ((Param->GS_PowerSwitch & 0xff) << 4) + ((Param->RF_PowerSwitch & 0xff) << 2) + ((Param->CS_PowerSwitch) & 0xff);

	m_CtrlCmd.get()[0] = 0xCA;


	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));



	m_CtrlCmd.get()[4] = 0xE0;
	m_CtrlCmd.get()[5] = 0x00;
	m_CtrlCmd.get()[7] = static_cast<char>(Param1 & 0xFF);

	m_CtrlCmd.get()[8] = 0xE0;
	m_CtrlCmd.get()[9] = 0x01;
	m_CtrlCmd.get()[10] = static_cast<char>((Param2 >> 24) & 0xFF);
	m_CtrlCmd.get()[11] = static_cast<char>((Param2 >> 16) & 0xFF);

	m_CtrlCmd.get()[12] = 0xE0;
	m_CtrlCmd.get()[13] = 0x02;
	m_CtrlCmd.get()[14] = static_cast<char>((Param2 >> 8) & 0xFF);
	m_CtrlCmd.get()[15] = static_cast<char>(Param2 & 0xFF);


	m_CtrlCmd.get()[16] = 0xE0;
	m_CtrlCmd.get()[17] = 0x03;
	m_CtrlCmd.get()[18] = static_cast<char>(Param->GainValue);
	m_CtrlCmd.get()[19] = static_cast<char>(WorkMode & 0xFF);

	m_CtrlCmd.get()[20] = 0xE0;
	m_CtrlCmd.get()[21] = 0x04;
	m_CtrlCmd.get()[23] = static_cast<char>(PowerSwich & 0xFF);

	return m_CtrlCmd;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_WSWholeStatus(int RF_DevID, int DevId, std::shared_ptr<DEV_PARAM>  Param)
{
	ClearBuffer();

	std::pair<unsigned long, unsigned long> FreqBand = m_FreqBandInfo.at(Param->FreqBand);

	unsigned long Param1 = (FreqBand.second >> 12) & 0xFF; //总计20bit有效，向右移动12bit，取高8bit;
	unsigned long Param2 = ((FreqBand.second & 0xFFF) << 20) + (FreqBand.first & 0xFFFFF);//EndFreq 取 后12位作为Param前12bit， StartFreq 的低20bit，做Param后20bit。
	unsigned long WorkMode = ((Param->GS_WorkMode & 0xff) << 4) + ((Param->RF_WorkMode & 0xff) << 2) + ((Param->CS_WorkMode) & 0xff);
	unsigned long PowerSwich = ((Param->GS_PowerSwitch & 0xff) << 4) + ((Param->RF_PowerSwitch & 0xff) << 2) + ((Param->CS_PowerSwitch) & 0xff);

	m_CtrlCmd.get()[0] = 0xCA;


	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevId & 0xff));



	m_CtrlCmd.get()[4] = 0xE0;
	m_CtrlCmd.get()[5] = 0x00;
	m_CtrlCmd.get()[7] = static_cast<char>(Param1 & 0xFF);

	m_CtrlCmd.get()[8] = 0xE0;
	m_CtrlCmd.get()[9] = 0x01;
	m_CtrlCmd.get()[10] = static_cast<char>((Param2 >> 24) & 0xFF);
	m_CtrlCmd.get()[11] = static_cast<char>((Param2 >> 16) & 0xFF);

	m_CtrlCmd.get()[12] = 0xE0;
	m_CtrlCmd.get()[13] = 0x02;
	m_CtrlCmd.get()[14] = static_cast<char>((Param2 >> 8) & 0xFF);
	m_CtrlCmd.get()[15] = static_cast<char>(Param2 & 0xFF);


	m_CtrlCmd.get()[16] = 0xE0;
	m_CtrlCmd.get()[17] = 0x03;
	m_CtrlCmd.get()[18] = static_cast<char>(Param->GainValue);
	m_CtrlCmd.get()[19] = static_cast<char>(WorkMode & 0xFF);

	m_CtrlCmd.get()[20] = 0xE0;
	m_CtrlCmd.get()[21] = 0x04;
	m_CtrlCmd.get()[23] = static_cast<char>(PowerSwich & 0xFF);

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_WSWholeStatus_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, int* Reslut)
{

	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDD || ((RetData.get()[6] >> 4) & 0xff) != 0x0C)
	{
		*Reslut = -10;
		return CODE_ERROR_ProclError;
	}
	else
	{
		*Reslut = static_cast<int>(RetData.get()[6] & 0xf);
	}
	return 0;
}



std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFGetDevIdentity(int RF_DevID, int DevID, int Channel)
{
	ClearBuffer();
	char ChannelCtr = 0x00;
	if (Channel % 2 == 0)
	{
		ChannelCtr = (Channel / 2) << 4;
	}
	else
	{
		ChannelCtr = ((Channel / 2) << 4) | (Channel % 2);
	}
	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevID & 0xff));

	m_CtrlCmd.get()[4] = 0xCE;
	m_CtrlCmd.get()[6] = 0x01;
	m_CtrlCmd.get()[7] = ChannelCtr;

	return m_CtrlCmd;
}
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFGetDevIdentity8(int RF_DevID, int DevID, int Channel)
{
	ClearBuffer();
	char ChannelCtr = 0x00;

	ChannelCtr = (Channel << 4) & 0xff;

	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevID & 0xff));

	m_CtrlCmd.get()[4] = 0xCE;
	m_CtrlCmd.get()[6] = 0x01;
	m_CtrlCmd.get()[7] = ChannelCtr;

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_RFGetDevIdentity_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_RF_IDENTITY_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDA)
	{
		memset(Reslut, 0x00, sizeof(DEV_RF_IDENTITY_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		int RF_Channel_module = static_cast<int>((RetData.get()[7] >> 4) & 0xf);
		int RF_Channel_channel = static_cast<int>(RetData.get()[7] & 0xf);
		int RF_Addr = 0x00;
		RF_Addr = RF_Channel_module * 2 + RF_Channel_channel;

		Reslut->RF_Addr = RF_Addr;

		int RF_BoardVer_h = static_cast<int>(RetData.get()[6] >> 4 & 0xf);
		int RF_BoardVer_l = static_cast<int>(RetData.get()[6] & 0xf);
		Reslut->RF_BoardVer = static_cast<float>(RF_BoardVer_h)+static_cast<float>(RF_BoardVer_l) / 100;
		int RF_Ver_h = static_cast<int>(RetData.get()[11] >> 4 & 0xf);
		int RF_Ver_l = static_cast<int>(RetData.get()[11] & 0xf);
		Reslut->RF_Ver = static_cast<float>(RF_Ver_h)+static_cast<float>(RF_Ver_l) / 100;
		int RF_MaxWorkingFreq = static_cast<int>(RetData.get()[10] & 0xff);
		Reslut->RF_MaxWorkingFreq = RF_MaxWorkingFreq;
		int RF_ComName = static_cast<int>(RetData.get()[14] & 0xff);
		Reslut->RF_ComName = RF_ComName;
		int RF_BoughtIndex = static_cast<int>(((RetData.get()[19] & 0xff) << 24) + ((RetData.get()[18] & 0xff) << 16) + ((RetData.get()[23] & 0xff) << 8) + (RetData.get()[22] & 0xff));
		Reslut->RF_BoughtIndex = RF_BoughtIndex;
		int RF_ModuleType = static_cast<int>(RetData.get()[27] & 0xff);
		Reslut->RF_ModuleType = RF_ModuleType;
	}
	return 0;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_CSGetDevIdentity_Z(int RF_DevID, int DevID)
{
	ClearBuffer();

	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevID & 0xff));

	m_CtrlCmd.get()[4] = 0xCE;
	m_CtrlCmd.get()[6] = 0x02;
	m_CtrlCmd.get()[7] = 0x00;

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_CSGetDevIdentity_Ret_Analysis_Z(std::shared_ptr<unsigned char> && RetData, PDEV_CS_IDENTITY_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDB)
	{
		memset(Reslut, 0x00, sizeof(DEV_CS_IDENTITY_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{

		int CS_BoardVer_h = static_cast<int>(RetData.get()[6] >> 4 & 0xf);
		int CS_BoardVer_l = static_cast<int>(RetData.get()[6] & 0xf);
		Reslut->CS_BoardVer = static_cast<float>(CS_BoardVer_h)+static_cast<float>(CS_BoardVer_l) / 100;
		int CS_Ver_h = static_cast<int>(RetData.get()[11] >> 4 & 0xf);
		int CS_Ver_l = static_cast<int>(RetData.get()[11] & 0xf);
		Reslut->CS_Ver = static_cast<float>(CS_Ver_h)+static_cast<float>(CS_Ver_l) / 100;
		int CS_MaxWorkingFreq = static_cast<int>(RetData.get()[10] & 0xff);
		Reslut->CS_MaxWorkingFreq = CS_MaxWorkingFreq;
		int CS_ComName = static_cast<int>(RetData.get()[14] & 0xff);
		Reslut->CS_ComName = CS_ComName;
		int CS_BoughtIndex = static_cast<int>(((RetData.get()[19] & 0xff) << 24) + ((RetData.get()[18] & 0xff) << 16) + ((RetData.get()[23] & 0xff) << 8) + (RetData.get()[22] & 0xff));
		Reslut->CS_BoughtIndex = CS_BoughtIndex;
		int CS_ModuleType = static_cast<int>(RetData.get()[27] & 0xff);
		Reslut->CS_ModuleType = CS_ModuleType;
	}
	return 0;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_GSGetDevIdentity_Z(int RF_DevID, int DevID)
{
	ClearBuffer();

	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevID & 0xff));

	m_CtrlCmd.get()[4] = 0xCE;
	m_CtrlCmd.get()[6] = 0x03;
	m_CtrlCmd.get()[7] = 0x00;

	return m_CtrlCmd;
}
int CDevCtrlCmd::MakeCmd_GSGetDevIdentity_Ret_Analysis_Z(std::shared_ptr<unsigned char> && RetData, PDEV_GS_IDENTITY_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDC)
	{
		memset(Reslut, 0x00, sizeof(DEV_CS_IDENTITY_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{

		int GS_BoardVer_h = static_cast<int>(RetData.get()[6] >> 4 & 0xf);
		int GS_BoardVer_l = static_cast<int>(RetData.get()[6] & 0xf);
		Reslut->GS_BoardVer = static_cast<float>(GS_BoardVer_h)+static_cast<float>(GS_BoardVer_l) / 100;
		int GS_Ver_h = static_cast<int>(RetData.get()[11] >> 4 & 0xf);
		int GS_Ver_l = static_cast<int>(RetData.get()[11] & 0xf);
		Reslut->GS_Ver = static_cast<float>(GS_Ver_h)+static_cast<float>(GS_Ver_l) / 100;
		int GS_MaxWorkingFreq = static_cast<int>(RetData.get()[10] & 0xff);
		Reslut->GS_MaxWorkingFreq = GS_MaxWorkingFreq;
		int GS_ComName = static_cast<int>(RetData.get()[14] & 0xff);
		Reslut->GS_ComName = GS_ComName;
		int GS_BoughtIndex = static_cast<int>(((RetData.get()[19] & 0xff) << 24) + ((RetData.get()[18] & 0xff) << 16) + ((RetData.get()[23] & 0xff) << 8) + (RetData.get()[22] & 0xff));
		Reslut->GS_BoughtIndex = GS_BoughtIndex;
		int GS_ModuleType = static_cast<int>(RetData.get()[27] & 0xff);
		Reslut->GS_ModuleType = GS_ModuleType;
	}
	return 0;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFGetDevState_Z(int RF_DevID, int DevID, int Channel)
{
	ClearBuffer();
	char ChannelCtr = 0x00;
	if (Channel % 2 == 0)
	{
		ChannelCtr = (Channel / 2) << 4;
	}
	else
	{
		ChannelCtr = ((Channel / 2) << 4) | (Channel % 2);
	}

	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevID & 0xff));

	m_CtrlCmd.get()[4] = 0xCD;
	m_CtrlCmd.get()[5] = 0x00;
	m_CtrlCmd.get()[6] = 0x01;
	m_CtrlCmd.get()[7] = static_cast<char>(ChannelCtr);

	return m_CtrlCmd;
}
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFGetDevState_Z8(int RF_DevID, int DevID, int Channel)
{
	ClearBuffer();
	char ChannelCtr = 0x00;

	ChannelCtr = (Channel << 4) & 0xff;

	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevID & 0xff));

	m_CtrlCmd.get()[4] = 0xCD;
	m_CtrlCmd.get()[5] = 0x00;
	m_CtrlCmd.get()[6] = 0x01;
	m_CtrlCmd.get()[7] = static_cast<char>(ChannelCtr);

	return m_CtrlCmd;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_CSGetDevState_Z(int RF_DevID, int DevID)
{
	ClearBuffer();

	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevID & 0xff));

	m_CtrlCmd.get()[4] = 0xCD;
	m_CtrlCmd.get()[5] = 0x00;
	m_CtrlCmd.get()[6] = 0x02;

	return m_CtrlCmd;
}

std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_GSGetDevState_Z(int RF_DevID, int DevID)
{
	ClearBuffer();

	m_CtrlCmd.get()[0] = 0xCA;
	m_CtrlCmd.get()[3] = static_cast<char>((((RF_DevID - 1) << 4) & 0xff) + (DevID & 0xff));

	m_CtrlCmd.get()[4] = 0xCD;
	m_CtrlCmd.get()[5] = 0x00;
	m_CtrlCmd.get()[6] = 0x03;

	return m_CtrlCmd;
}
#pragma endregion

#pragma region 射频查询V1.6.2

//int CDevCtrlCmd::MakeCmd_RFGetDevState_Ret_Analysis_Z(std::shared_ptr<unsigned char>&& RetData, PDEV_RF_STATUS_RESULT Reslut)
//{
//	if (RetData == nullptr)
//		return CODE_ERROR_ProclError;
//	if (RetData == nullptr)
//		return CODE_ERROR_ProclError;
//
//	if ((RetData.get()[1] != 0xCA) || (RetData.get()[5] != 0xDA))
//	{
//		memset(Reslut, 0x00, sizeof(DEV_RF_STATUS_RESULT));
//		return CODE_ERROR_ProclError;
//	}
//	else
//	{
//		int RF_DevID = static_cast<int>((RetData.get()[2] >> 4) & 0xf);
//		switch (RF_DevID)
//		{
//		case 0:
//			Reslut->FreqBandEnd = 550;
//			Reslut->FreqBandStart = 150;
//			break;
//		case 1:
//			Reslut->FreqBandEnd = 950;
//			Reslut->FreqBandStart = 550;
//			break;
//		case 2:
//			Reslut->FreqBandEnd = 1450;
//			Reslut->FreqBandStart = 950;
//			break;
//		case 3:
//			Reslut->FreqBandEnd = 2050;
//			Reslut->FreqBandStart = 1050;
//			break;
//		case 4:
//			Reslut->FreqBandEnd = 3050;
//			Reslut->FreqBandStart = 2050;
//			break;
//		default:
//			break;
//		}
//		int RF_Channel_module = static_cast<int>((RetData.get()[7] >> 4) & 0xf);
//		int RF_Channel_channel = static_cast<int>(RetData.get()[7] & 0xf);
//		int RF_Channel = 0x00;
//		RF_Channel = RF_Channel_module * 2 + RF_Channel_channel;
//		Reslut->RF_Channel = RF_Channel;
//
//		int RF_PowerStatus = static_cast<int>((RetData.get()[6] >> 7) & 0x01);
//		Reslut->RF_PowerStatus = RF_PowerStatus;
//
//		int RF_WorkMode = static_cast<int>((RetData.get()[6] >> 6) & 0x01);
//		Reslut->RF_WorkMode = RF_WorkMode;
//
//		int RF_GainValue = static_cast<int>(RetData.get()[6] & 0x3f);
//		Reslut->RF_GainValue = RF_GainValue;
//
//
//		int RF_WorkingVoltage = static_cast<int>(((RetData.get()[11] & 0xff) << 8) + ((RetData.get()[10]) & 0xff));
//		Reslut->RF_WorkingVoltage = RF_WorkingVoltage * 25;
//		int RF_WorkingCurrent = static_cast<int>(((RetData.get()[15] & 0xff) << 8) + ((RetData.get()[14]) & 0xff));
//		Reslut->RF_WorkingCurrent = RF_WorkingCurrent;
//		int RF_Temperature_State = static_cast<int>((RetData.get()[19] >> 4) & 0x01);
//		float RF_Temperature = static_cast<float>(((RetData.get()[19] & 0xf) << 8) + ((RetData.get()[18]) & 0xff));
//		if (RF_Temperature_State == 0)
//		{
//			Reslut->RF_Temperature = RF_Temperature / 16;
//		}
//		else
//		{
//			Reslut->RF_Temperature = RF_Temperature / 16;
//		}
//	}
//	return 0;
//}
int CDevCtrlCmd::MakeCmd_CSGetDevState_Ret_Analysis_Z(std::shared_ptr<unsigned char> && RetData, PDEV_CS_STATUS_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDB)
	{
		memset(Reslut, 0x00, sizeof(DEV_CS_STATUS_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		int RF_DevID = static_cast<int>((RetData.get()[2] >> 4) & 0xf);
		switch (RF_DevID)
		{
		case 0:
			Reslut->FreqBandEnd = 550;
			Reslut->FreqBandStart = 150;
			break;
		case 1:
			Reslut->FreqBandEnd = 950;
			Reslut->FreqBandStart = 550;
			break;
		case 2:
			Reslut->FreqBandEnd = 1450;
			Reslut->FreqBandStart = 950;
			break;
		case 3:
			Reslut->FreqBandEnd = 2050;
			Reslut->FreqBandStart = 1050;
			break;
		case 4:
			Reslut->FreqBandEnd = 3050;
			Reslut->FreqBandStart = 2050;
			break;
		default:
			break;
		}

		int CS_GainValue = static_cast<int>(RetData.get()[6] & 0xff);
		Reslut->CS_GainValue = CS_GainValue;

		int CS_WorkMode = static_cast<int>((RetData.get()[10] >> 2) & 0x03);
		Reslut->CS_WorkMode = CS_WorkMode;
		int CS_PowerStatus = static_cast<int>((RetData.get()[10]) & 0x03);
		Reslut->CS_PowerStatus = CS_PowerStatus;

		int CS_WorkingVoltage = static_cast<int>(((RetData.get()[15] & 0xff) << 8) + ((RetData.get()[14]) & 0xff));
		Reslut->CS_WorkingVoltage = CS_WorkingVoltage * 305.18 / 1000;
		int CS_WorkingCurrent = static_cast<int>(((RetData.get()[19] & 0xff) << 8) + ((RetData.get()[18]) & 0xff));
		Reslut->CS_WorkingCurrent = CS_WorkingCurrent;
		int CS_Temperature_State = static_cast<int>((RetData.get()[23] >> 4) & 0x01);
		float CS_Temperature = static_cast<float>(((RetData.get()[23] & 0xf) << 8) + ((RetData.get()[22]) & 0xff));
		if (CS_Temperature_State == 0)
		{
			Reslut->CS_Temperature = CS_Temperature / 16;
		}
		else
		{
			Reslut->CS_Temperature = CS_Temperature / 16;
		}
	}
	return 0;
}
int CDevCtrlCmd::MakeCmd_GSGetDevState_Ret_Analysis_Z(std::shared_ptr<unsigned char> && RetData, PDEV_GS_STATUS_RESULT Reslut)
{
	if (RetData == nullptr)
		return CODE_ERROR_ProclError;
	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDC)
	{
		memset(Reslut, 0x00, sizeof(DEV_GS_STATUS_RESULT));
		return CODE_ERROR_ProclError;
	}
	else
	{
		int GS_WorkMode = static_cast<int>((RetData.get()[10] >> 2) & 0x03);
		Reslut->GS_WorkMode = GS_WorkMode;
		int GS_PowerStatus = static_cast<int>((RetData.get()[10]) & 0x03);
		Reslut->GS_PowerStatus = GS_PowerStatus;

		int GS_WorkingVoltage = static_cast<int>(((RetData.get()[15] & 0xff) << 8) + ((RetData.get()[14]) & 0xff));
		Reslut->GS_WorkingVoltage = GS_WorkingVoltage * 305.18 / 1000;
		int GS_WorkingCurrent = static_cast<int>(((RetData.get()[19] & 0xff) << 8) + ((RetData.get()[18]) & 0xff));
		Reslut->GS_WorkingCurrent = GS_WorkingCurrent;
		int GS_Temperature_State = static_cast<int>((RetData.get()[23] >> 4) & 0x01);
		float GS_Temperature = static_cast<float>(((RetData.get()[23] & 0xf) << 8) + ((RetData.get()[22]) & 0xff));
		if (GS_Temperature_State == 0)
		{
			Reslut->GS_Temperature = GS_Temperature / 16;
		}
		else
		{
			Reslut->GS_Temperature = GS_Temperature / 16;
		}
	}
	return 0;
}

#pragma endregion

#pragma region 射频前端状态查询V1.3

//std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_RFGetDevState(int DevID, int Channel)
//{
//	ClearBuffer();
//	char ChannelCtr = 0x00;
//	if (Channel % 2 == 0)
//	{
//		ChannelCtr = (Channel / 2) << 4;
//	}
//	else
//	{
//		ChannelCtr = ((Channel / 2) << 4) | (Channel % 2);
//	}
//
//	m_CtrlCmd.get()[0] = 0xCA;
//	m_CtrlCmd.get()[3] = static_cast<char>(DevID);
//	m_CtrlCmd.get()[4] = 0xCD;
//	m_CtrlCmd.get()[5] = 0x00;
//	m_CtrlCmd.get()[6] = 0x01;
//	m_CtrlCmd.get()[7] = static_cast<char>(ChannelCtr);
//
//	return m_CtrlCmd;
//}
//int CDevCtrlCmd::MakeCmd_RFGetDevState_Ret_Analysis(std::shared_ptr<unsigned char>&& RetData, PDEV_RF_STATUS_RESULT Reslut)
//{
//	if (RetData == nullptr)
//		return CODE_ERROR_ProclError;
//	if (RetData == nullptr)
//		return CODE_ERROR_ProclError;
//
//	if ((RetData.get()[1] != 0xCA) || (RetData.get()[5] != 0xDA))
//	{
//		memset(Reslut, 0x00, sizeof(DEV_RF_STATUS_RESULT));
//		return CODE_ERROR_ProclError;
//	}
//	else
//	{
//
//		int RF_Channel_module = static_cast<int>((RetData.get()[7] >> 4) & 0xf);
//		int RF_Channel_channel = static_cast<int>(RetData.get()[7] & 0xf);
//		int RF_Channel = 0x00;
//		RF_Channel = RF_Channel_module * 2 + RF_Channel_channel;
//
//		Reslut->RF_Channel = RF_Channel;
//
//		int FreqBandEnd = static_cast<int>(((RetData.get()[6] & 0xff) << 12) + ((RetData.get()[11] & 0xff) << 4) + ((RetData.get()[10] >> 4) & 0xf));
//		Reslut->FreqBandEnd = FreqBandEnd;
//		int FreqBandStart = static_cast<int>(((RetData.get()[10] & 0xf) << 16) + ((RetData.get()[15] & 0xff) << 8) + ((RetData.get()[14]) & 0xff));
//		Reslut->FreqBandStart = FreqBandStart;
//		int RF_GainValue = static_cast<int>(((RetData.get()[19] & 0xf) << 4) + ((RetData.get()[18] >> 4) & 0xf));
//		Reslut->RF_GainValue = RF_GainValue;
//		int RF_WorkMode = static_cast<int>((RetData.get()[18] >> 2) & 0x03);
//		Reslut->RF_WorkMode = RF_WorkMode;
//		int RF_PowerStatus = static_cast<int>((RetData.get()[18]) & 0x03);
//		Reslut->RF_PowerStatus = RF_PowerStatus;
//		int RF_WorkingVoltage = static_cast<int>(((RetData.get()[23] & 0xf) << 8) + ((RetData.get()[22]) & 0xff));
//		Reslut->RF_WorkingVoltage = RF_WorkingVoltage * 12;
//		int RF_WorkingCurrent = static_cast<int>(((RetData.get()[27] & 0xff) << 6) + ((RetData.get()[26] >> 2) & 0xff));
//		Reslut->RF_WorkingCurrent = RF_WorkingCurrent;
//		int RF_Temperature_State = static_cast<int>((RetData.get()[26] >> 1) & 0x01);
//		float RF_Temperature = static_cast<float>(((RetData.get()[26] & 0x03) << 8) + ((RetData.get()[31]) & 0xff));
//		if (RF_Temperature_State == 0)
//		{
//			Reslut->RF_Temperature = RF_Temperature / 4;
//		}
//		else
//		{
//			Reslut->RF_Temperature = RF_Temperature / 4;
//		}
//	}
//	return 0;
//}
//
//std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_CSGetDevState(int DevID)
//{
//	ClearBuffer();
//
//	m_CtrlCmd.get()[0] = 0xCA;
//	m_CtrlCmd.get()[3] = static_cast<char>(DevID);
//	m_CtrlCmd.get()[4] = 0xCD;
//	m_CtrlCmd.get()[5] = 0x00;
//	m_CtrlCmd.get()[6] = 0x02;
//
//	return m_CtrlCmd;
//}
//int CDevCtrlCmd::MakeCmd_CSGetDevState_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_CS_STATUS_RESULT Reslut)
//{
//	if (RetData == nullptr)
//		return CODE_ERROR_ProclError;
//	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDB)
//	{
//		memset(Reslut, 0x00, sizeof(DEV_CS_STATUS_RESULT));
//		return CODE_ERROR_ProclError;
//	}
//	else
//	{
//		int FreqBandEnd = static_cast<int>(((RetData.get()[6] & 0xff) << 12) + ((RetData.get()[11] & 0xff) << 4) + ((RetData.get()[10] >> 4) & 0xf));
//		Reslut->FreqBandEnd = FreqBandEnd;
//		int FreqBandStart = static_cast<int>(((RetData.get()[10] & 0xf) << 16) + ((RetData.get()[15] & 0xff) << 8) + ((RetData.get()[14]) & 0xff));
//		Reslut->FreqBandStart = FreqBandStart;
//		int CS_GainValue = static_cast<int>(((RetData.get()[19] & 0xf) << 4) + ((RetData.get()[18] >> 4) & 0xf));
//		Reslut->CS_GainValue = CS_GainValue;
//		int CS_WorkMode = static_cast<int>((RetData.get()[18] >> 2) & 0x03);
//		Reslut->CS_WorkMode = CS_WorkMode;
//		int CS_PowerStatus = static_cast<int>((RetData.get()[18]) & 0x03);
//		Reslut->CS_PowerStatus = CS_PowerStatus;
//		int CS_WorkingVoltage = static_cast<int>(((RetData.get()[23] & 0xf) << 8) + ((RetData.get()[22]) & 0xff));
//		Reslut->CS_WorkingVoltage = CS_WorkingVoltage * 12;
//		int CS_WorkingCurrent = static_cast<int>(((RetData.get()[27] & 0xff) << 6) + ((RetData.get()[26] >> 2) & 0xff));
//		Reslut->CS_WorkingCurrent = CS_WorkingCurrent;
//		int CS_Temperature_State = static_cast<int>((RetData.get()[26] >> 1) & 0x01);
//		float CS_Temperature = static_cast<float>(((RetData.get()[26] & 0x03) << 8) + ((RetData.get()[31]) & 0xff));
//		if (CS_Temperature_State == 0)
//		{
//			Reslut->CS_Temperature = CS_Temperature / 4;
//		}
//		else
//		{
//			Reslut->CS_Temperature = CS_Temperature / 4;
//		}
//	}
//	return 0;
//}
//
//std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_GSGetDevState(int DevID)
//{
//	ClearBuffer();
//
//	m_CtrlCmd.get()[0] = 0xCA;
//	m_CtrlCmd.get()[3] = static_cast<char>(DevID);
//	m_CtrlCmd.get()[4] = 0xCD;
//	m_CtrlCmd.get()[5] = 0x00;
//	m_CtrlCmd.get()[6] = 0x03;
//
//	return m_CtrlCmd;
//}
//int CDevCtrlCmd::MakeCmd_GSGetDevState_Ret_Analysis(std::shared_ptr<unsigned char> && RetData, PDEV_GS_STATUS_RESULT Reslut)
//{
//	if (RetData == nullptr)
//		return CODE_ERROR_ProclError;
//	if (RetData.get()[1] != 0xCA || RetData.get()[5] != 0xDC)
//	{
//		memset(Reslut, 0x00, sizeof(DEV_GS_STATUS_RESULT));
//		return CODE_ERROR_ProclError;
//	}
//	else
//	{
//		int GS_WorkMode = static_cast<int>((RetData.get()[6] >> 2) & 0x03);
//		Reslut->GS_WorkMode = GS_WorkMode;
//		int GS_PowerStatus = static_cast<int>((RetData.get()[6]) & 0x03);
//		Reslut->GS_PowerStatus = GS_PowerStatus;
//		int GS_WorkingVoltage = static_cast<int>(((RetData.get()[11] & 0xf) << 8) + ((RetData.get()[10]) & 0xff));
//		Reslut->GS_WorkingVoltage = GS_WorkingVoltage * 12;
//		int GS_WorkingCurrent = static_cast<int>(((RetData.get()[15] & 0xff) << 6) + ((RetData.get()[14] >> 2) & 0xff));
//		Reslut->GS_WorkingCurrent = GS_WorkingCurrent;
//		int GS_Temperature_State = static_cast<int>((RetData.get()[14] >> 1) & 0x01);
//		float GS_Temperature = static_cast<float>(((RetData.get()[14] & 0x03) << 8) + ((RetData.get()[19]) & 0xff));
//		if (GS_Temperature_State == 0)
//		{
//			Reslut->GS_Temperature = GS_Temperature / 4;
//		}
//		else
//		{
//			Reslut->GS_Temperature = GS_Temperature / 4;
//		}
//	}
//	return 0;
//}


#pragma endregion

#pragma region 190814新增测试用组播ip设置指令
std::shared_ptr<unsigned char> CDevCtrlCmd::MakeCmd_SetTestip()
{
	ClearBuffer();
	int offset = 8;

	m_CtrlCmd.get()[0] = 0xab;
	m_CtrlCmd.get()[1] = 0xab;
	m_CtrlCmd.get()[2] = 0xab;
	m_CtrlCmd.get()[3] = 0xab;
	m_CtrlCmd.get()[4] = 0xab;
	m_CtrlCmd.get()[5] = 0xab;
	m_CtrlCmd.get()[6] = 0xab;
	m_CtrlCmd.get()[7] = 0xab;

	m_CtrlCmd.get()[offset + 0] = 0x5A;
	m_CtrlCmd.get()[offset + 1] = 0x58;
	m_CtrlCmd.get()[offset + 2] = 0x4C;
	m_CtrlCmd.get()[offset + 3] = 0x43;

	m_CtrlCmd.get()[offset + 4] = 0xFF;
	m_CtrlCmd.get()[offset + 5] = 0xFF;
	m_CtrlCmd.get()[offset + 6] = 0x00;
	m_CtrlCmd.get()[offset + 7] = 0x02;

	m_CtrlCmd.get()[offset + 16] = 0x00;
	m_CtrlCmd.get()[offset + 17] = 0x00;

	m_CtrlCmd.get()[offset + 18] = 0x01;//传输板1（8T只有一块传输板）
	m_CtrlCmd.get()[offset + 19] = 0x80;//光口1
	m_CtrlCmd.get()[offset + 21] = 0x01;
	m_CtrlCmd.get()[offset + 23] = 0x78;

	m_CtrlCmd.get()[offset + 27] = 0x0B;//表6
	m_CtrlCmd.get()[offset + 28] = 0xEF;//表6
	m_CtrlCmd.get()[offset + 29] = 0x01;//表6
	m_CtrlCmd.get()[offset + 30] = 0x01;//表6
	m_CtrlCmd.get()[offset + 31] = 0xEF;//表6



	for (size_t i = 0; i < 88; i++)
	{
		m_CtrlCmd.get()[offset + 32 + i] = 0xFF;
	}

	return m_CtrlCmd;
}

#pragma endregion
