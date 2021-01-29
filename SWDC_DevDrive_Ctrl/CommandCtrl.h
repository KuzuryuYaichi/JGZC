#include "string.h"
#include "basetsd.h"
#include "winsock2.h"


typedef struct _SrcParam 
{
	unsigned char cmd[8+24+64+8];//8字节主/从区分头,24字节万兆指令头,64字节万兆网源参数

	_SrcParam()
	{
		memset(cmd, 0, 8+24+64+8);
		//ZXLC
		cmd[8] = 90;	cmd[9] = 88;	cmd[10] = 76;	cmd[11] = 67;

		//目的地址
		cmd[12] = 0xFF;	cmd[13] = 0xFF;	cmd[14] = 0x00;	cmd[15] = 0x02;
	}

	// nSerial		命令流水号（包序号）
	// nBoardNo		板卡号（0, 1）
	// nNetCardNo	万兆网口号（0~3）,对应光口1~4
	// nOption		主从选择, 0:从 1:主
	// nSrcIP		万兆网口源IP
	// nSrcMAC		万兆网口源MAC
	// nInterval	MAC帧插入时钟计数:默认为0
	// nPacketLen	UDP分片长度门限:1472
	// nBitControl	位控制寄存器
	// nHeartPort	连接心跳包端口（未使用）
	// nSlicesClock 分片间插入时钟周期数
	void GetCmd(unsigned char* pDstCmd, int* pLen, 
				UINT16 nSerial, UINT8 nBoardNo, UINT8 nNetCardNo, int nOption,
				UINT32 nSrcIP, UINT64 nSrcMAC, 
				UINT32 nInterval, UINT16 nPacketLen,
				UINT32 nBitControl, UINT16 nHeartPort, UINT32 nSlicesClock)
	{
		UINT16 nTmp16 = 0;
		UINT32 nTmp32 = 0;

		//主/从区分头
		if (nOption == 0)//从
		{
			memset(cmd, 0xbc, 8);
		} 
		else//主
		{
			memset(cmd, 0xab, 8);
		}

		//流水号
		nTmp16 = htons(nSerial);
		memcpy(cmd+24, &nTmp16, sizeof(UINT16));

		//万兆网板卡号0、1
		cmd[26] = nBoardNo;
		//万兆网口序号0~3对应0x80、0x81、0x82、0x83(芯片号)
		cmd[27] = 0x80+nNetCardNo;

		//万兆控制命令0x0001
		UINT16 nCommand = htons(0x0001);
		memcpy(cmd+28, &nCommand, sizeof(UINT16));
		//包长
		UINT16 nPackLen = htons(8+24+64);
		memcpy(cmd+30, &nPackLen, sizeof(UINT16));

		//源IP RAM地址
		UINT32 RamAddr = htonl(0x00000000);
		memcpy(cmd+32, &RamAddr, sizeof(UINT32));
		nTmp32 = htonl(nSrcIP);
		memcpy(cmd+36, &nTmp32, sizeof(UINT32));

		//源MAC(6-3) RAM地址
		RamAddr = htonl(0x00000001);
		memcpy(cmd+40, &RamAddr, sizeof(UINT32));
		memcpy(&nTmp32, (char*)(&nSrcMAC) + 2, sizeof(UINT32));
		nTmp32 = htonl(nTmp32);
		memcpy(cmd+44, &nTmp32, sizeof(UINT32));

		//源MAC(2-1) RAM地址
		RamAddr = htonl(0x00000002);
		memcpy(cmd+48, &RamAddr, sizeof(UINT32));
		memcpy(&nTmp16, (char*)(&nSrcMAC), 2);
		nTmp16 = htons(nTmp16);
		memcpy(cmd+52, &nTmp16, 2);

		//包间隔
		RamAddr = htonl(0x00000003);
		memcpy(cmd+56, &RamAddr, sizeof(UINT32));
		nTmp32 = htonl(nInterval);
		memcpy(cmd+60, &nTmp32, sizeof(UINT32));

		//位控制寄存器
		RamAddr = htonl(0x00000004);
		memcpy(cmd+64, &RamAddr, sizeof(UINT32));
		nTmp32 = htonl(nBitControl);
		memcpy(cmd+68, &nTmp32, sizeof(UINT32));

		//心跳端口
		RamAddr = htonl(0x00000005);
		memcpy(cmd+72, &RamAddr, sizeof(UINT32));
		nTmp32 = nHeartPort;
		nTmp32 = htonl(nTmp32);
		memcpy(cmd+76, &nTmp32, sizeof(UINT32));

		//分片长度
		RamAddr = htonl(0x00000006);
		memcpy(cmd+80, &RamAddr, sizeof(UINT32));
		nTmp32 = nPacketLen;
		nTmp32 = htonl(nTmp32);
		memcpy(cmd+84, &nTmp32, sizeof(UINT32));

		//保留
		RamAddr = htonl(0x00000007);
		memcpy(cmd+88, &RamAddr, sizeof(UINT32));
		nTmp32 = 2;
		nTmp32 = htonl(nTmp32);
		memcpy(cmd + 92, &nTmp32, sizeof(UINT32));

		//分片长度
		RamAddr = htonl(0x00000008);
		memcpy(cmd + 96, &RamAddr, sizeof(UINT32));
		nTmp32 = nSlicesClock;
		nTmp32 = htonl(nTmp32);
		memcpy(cmd + 100, &nTmp32, sizeof(UINT32));


		memcpy(pDstCmd, cmd, 8+24+64+8);
		*pLen = 8+24+64+8;
	}


	UINT64 GetMAC(unsigned char *pMac)
	{
		unsigned char mac[8] = {0};
		UINT64 nSrcMAC = 0;
		for (int i = 0; i < 6; i++)
		{
			unsigned char H=0;
			unsigned char L=0;
			if ('a' <= pMac[2*i] && pMac[2*i] <= 'z')
			{
				H = pMac[2*i]-'a' + 10;
			}
			else if ('A' <= pMac[2*i] && pMac[2*i] <= 'Z')
			{
				H = pMac[2*i]-'A' + 10;
			}
			else if ('0' <= pMac[2*i] && pMac[2*i] <= '9')
			{
				H = pMac[2*i]-'0';
			}

			if ('a' <= pMac[2*i+1] && pMac[2*i+1] <= 'z')
			{
				L = pMac[2*i+1]-'a' + 10;
			}
			else if ('A' <= pMac[2*i+1] && pMac[2*i+1] <= 'Z')
			{
				L = pMac[2*i+1]-'A' + 10;
			}
			else if ('0' <= pMac[2*i+1] && pMac[2*i+1] <= '9')
			{
				L = pMac[2*i+1]-'0';
			}

			mac[5-i] = H*16 + L;
		}

		memcpy(&nSrcMAC, mac, sizeof(UINT64));

		return nSrcMAC;
	}
} SrcParam_t;

typedef struct _DstParam 
{
	unsigned char cmd[8+24+16];//8字节主/从区分头,24字节万兆指令头,16字节万兆网目的参数

	_DstParam()
	{
		memset(cmd, 0, 8+24+16);
		//ZXLC
		cmd[8] = 90;	cmd[9] = 88;	cmd[10] = 76;	cmd[11] = 67;

		//目的地址
		cmd[12] = 0xFF;	cmd[13] = 0xFF;	cmd[14] = 0x00;	cmd[15] = 0x02;
	}

	// nSerial		命令流水号（包序号）
	// nBoardNo		板卡号（0, 1）
	// nNetCardNo	万兆网口号（0~3）,对应光口1~4
	// nOption		主从选择, 0:从 1:主
	// IPRamAddr	目的IP地址 RAM地址
	// nIP			目的IP地址
	// PortRamAddr	UDP源端口/目的端口 RAM地址
	// nSrcPort		UDP源端口
	// nDstPort		UDP目的端口
	void GetCmd(unsigned char* pDstCmd, int* pLen, 
		UINT16 nSerial, UINT8 nBoardNo, UINT8 nNetCardNo, int nOption,
		UINT32 IPRamAddr, UINT32 nIP, UINT32 PortRamAddr, UINT16 nSrcPort, UINT16 nDstPort)
	{
		UINT16 nTmp16 = 0;
		UINT32 nTmp32 = 0;

		//主/从区分头
		if (nOption == 0)//从
		{
			memset(cmd, 0xbc, 8);
		} 
		else//主
		{
			memset(cmd, 0xab, 8);
		}

		//流水号
		nTmp16 = htons(nSerial);
		memcpy(cmd+24, &nTmp16, sizeof(UINT16));

		//万兆网板卡号0、1
		cmd[26] = nBoardNo;
		//万兆网口序号0~3对应0x80、0x81、0x82、0x83(芯片号)
		cmd[27] = 0x80+nNetCardNo;

		//万兆控制命令0x0003
		UINT16 nCommand = htons(0x0003);
		memcpy(cmd+28, &nCommand, sizeof(UINT16));
		//包长
		UINT16 nPackLen = htons(8+24+16);
		memcpy(cmd+30, &nPackLen, sizeof(UINT16));

		//IP地址 RAM地址
		nTmp32 = htonl(IPRamAddr);
		memcpy(cmd+32, &nTmp32, sizeof(UINT32));
		//IP地址
		nTmp32 = htonl(nIP);
		memcpy(cmd + 36, &nTmp32, sizeof(UINT32));

		//源与目的端口 RAM地址
		nTmp32 = htonl(PortRamAddr);
		memcpy(cmd+40, &nTmp32, sizeof(UINT32));

		//
		// 源端口和目的端口可能会反
		//

		////目的端口
		//nTmp16 = htons(nDstPort);
		//memcpy(cmd+44, &nTmp16, sizeof(UINT16));

		////源端口
		//nTmp16 = htons(nSrcPort);
		//memcpy(cmd+46, &nTmp16, sizeof(UINT16));

		//目的端口
		nTmp16 = htons(nSrcPort);
		memcpy(cmd + 44, &nTmp16, sizeof(UINT16));

		//源端口
		nTmp16 = htons(nDstPort);
		memcpy(cmd + 46, &nTmp16, sizeof(UINT16));


		memcpy(pDstCmd, cmd, 8+24+16);
		*pLen = 8+24+16;
	}
} DstParam_t;

typedef struct _LinkEnable 
{
	unsigned char cmd[8+24+16];//8字节主/从区分头,24字节万兆指令头,16字节（后8字节为保留-对其16字节）万兆网连接使能参数

	_LinkEnable()
	{
		memset(cmd, 0, 8+24+16);
		//ZXLC
		cmd[8] = 90;	cmd[9] = 88;	cmd[10] = 76;	cmd[11] = 67;

		//目的地址
		cmd[12] = 0xFF;	cmd[13] = 0xFF;	cmd[14] = 0x00;	cmd[15] = 0x02;
	}

	// nSerial		命令流水号（包序号）
	// nBoardNo		板卡号（0, 1）
	// nNetCardNo	万兆网口号（0~3）,对应光口1~4
	// nOption		主从选择, 0:从 1:主
	// EnableRamAddr	连接使能 RAM地址
	// nEnable			连接使能, 0:禁止 1:使能
	void GetCmd(unsigned char* pDstCmd, int* pLen, 
		UINT16 nSerial, UINT8 nBoardNo, UINT8 nNetCardNo, int nOption,
		UINT32 EnableRamAddr, UINT32 nEnable)
	{
		UINT16 nTmp16 = 0;
		UINT32 nTmp32 = 0;

		//主/从区分头
		if (nOption == 0)//从
		{
			memset(cmd, 0xbc, 8);
		} 
		else//主
		{
			memset(cmd, 0xab, 8);
		}

		//流水号
		nTmp16 = htons(nSerial);
		memcpy(cmd+24, &nTmp16, sizeof(UINT16));
		
		//万兆网板卡号0、1
		cmd[26] = nBoardNo;
		//万兆网口序号0~3对应0x80、0x81、0x82、0x83(芯片号)
		cmd[27] = 0x80+nNetCardNo;

		//万兆控制命令0x0004
		UINT16 nCommand = htons(0x0004);
		memcpy(cmd+28, &nCommand, sizeof(UINT16));
		//包长
		UINT16 nPackLen = htons(8+24+16);
		memcpy(cmd+30, &nPackLen, sizeof(UINT16));

		//使能 RAM地址
		nTmp32 = htonl(EnableRamAddr);
		memcpy(cmd+32, &nTmp32, sizeof(UINT32));
		//使能
		nTmp32 = htonl(nEnable);
		memcpy(cmd+36, &nTmp32, sizeof(UINT32));

		//RAM地址,保留（超过有效地址则不判断）
		nTmp32 = htonl(0x0000FFFF);//需要大于有效地址0x000000FF
		memcpy(cmd+40, &nTmp32, sizeof(UINT32));
		//保留
		//nTmp32 = htonl();
		//memcpy(cmd+44, &nTmp32, sizeof(UINT32));

		memcpy(pDstCmd, cmd, 8+24+16);
		*pLen = 8+24+16;
	}
} LinkEnable_t;

typedef struct _AllEnable
{
	unsigned char cmd[24 + 16 + 8 * 40];//8字节主/从区分头,24字节万兆指令头,16字节（后8字节为保留-对其16字节）万兆网连接使能参数

	_AllEnable()
	{
		memset(cmd, 0,  24 + 16 + 8 * 40);
		//ZXLC
		cmd[8] = 90;	cmd[9] = 88;	cmd[10] = 76;	cmd[11] = 67;

		//目的地址
		cmd[12] = 0xFF;	cmd[13] = 0xFF;	cmd[14] = 0x00;	cmd[15] = 0x02;
	}

	// nSerial		命令流水号（包序号）
	// nBoardNo		板卡号（0, 1）
	// nNetCardNo	万兆网口号（0~3）,对应光口1~4
	// nOption		主从选择, 0:从 1:主
	// EnableRamAddr	连接使能 RAM地址
	// nEnable			连接使能, 0:禁止 1:使能
	void GetCmd(unsigned char* pDstCmd, int* pLen,
		UINT16 nSerial, UINT8 nBoardNo, UINT8 nNetCardNo, int nOption,
		UINT32 nEnable)
	{
		UINT16 nTmp16 = 0;
		UINT32 nTmp32 = 0;

		//主/从区分头
		if (nOption == 0)//从
		{
			memset(cmd, 0xbc, 8);
		}
		else//主
		{
			memset(cmd, 0xab, 8);
		}

		//流水号
		nTmp16 = htons(nSerial);
		memcpy(cmd + 24, &nTmp16, sizeof(UINT16));

		//万兆网板卡号0、1
		cmd[26] = nBoardNo;
		//万兆网口序号0~3对应0x80、0x81、0x82、0x83(芯片号)
		cmd[27] = 0x80 + nNetCardNo;

		//万兆控制命令0x0004
		UINT16 nCommand = htons(0x0004);
		memcpy(cmd + 28, &nCommand, sizeof(UINT16));
		//包长
		UINT16 nPackLen = htons(8 + 24 + 16);
		memcpy(cmd + 30, &nPackLen, sizeof(UINT16));

		for (int i = 0; i < 40;i++)
		{
			//使能 RAM地址
			nTmp32 = htonl(i);
			memcpy(cmd + 32 + 8 * i, &nTmp32, sizeof(UINT32));
			//使能
			nTmp32 = htonl(nEnable);
			memcpy(cmd + 36 + 8 * i, &nTmp32, sizeof(UINT32));
		}




		//RAM地址,保留（超过有效地址则不判断）
		nTmp32 = htonl(0x0000FFFF);//需要大于有效地址0x000000FF
		memcpy(cmd + 40 + 8 * 39, &nTmp32, sizeof(UINT32));
		//保留
		//nTmp32 = htonl();
		//memcpy(cmd+44, &nTmp32, sizeof(UINT32));

		memcpy(pDstCmd, cmd, 24 + 16 + 8 * 40);
		*pLen = 24 + 16 + 8 * 40;
	}
} AllEnable_t;

typedef struct _MultiParam
{
	unsigned char cmd[8 + 24 + 8];//8字节主/从区分头,24字节万兆指令头,8字节万兆网单播组播选择

	_MultiParam()
	{
		memset(cmd, 0, 8 + 24 + 8);
		//ZXLC
		cmd[8] = 90;	cmd[9] = 88;	cmd[10] = 76;	cmd[11] = 67;

		//目的地址
		cmd[12] = 0xFF;	cmd[13] = 0xFF;	cmd[14] = 0x00;	cmd[15] = 0x02;
	}

	// nSerial		命令流水号（包序号）
	// nBoardNo		板卡号（0, 1）
	// nNetCardNo	万兆网口号（0~3）,对应光口1~4
	// nOption		主从选择, 0:从 1:主
	// MultiRamAddr 单播组播选择 RAM地址
	// nMulti       单播组播选择， 0：单播   1：组播
	void GetCmd(unsigned char* pDstCmd, int* pLen,
		UINT16 nSerial, UINT8 nBoardNo, UINT8 nNetCardNo, int nOption,
		UINT32 MultiRamAddr, UINT32 nMulti)
	{
		UINT16 nTmp16 = 0;
		UINT32 nTmp32 = 0;

		//主/从区分头
		if (nOption == 0)//从
		{
			memset(cmd, 0xbc, 8);
		}
		else//主
		{
			memset(cmd, 0xab, 8);
		}

		//流水号
		nTmp16 = htons(nSerial);
		memcpy(cmd + 24, &nTmp16, sizeof(UINT16));

		//万兆网板卡号0、1
		cmd[26] = nBoardNo;
		//万兆网口序号0~3对应0x80、0x81、0x82、0x83(芯片号)
		cmd[27] = 0x80 + nNetCardNo;

		//万兆控制命令0x0005
		UINT16 nCommand = htons(0x0005);
		memcpy(cmd + 28, &nCommand, sizeof(UINT16));
		//包长
		UINT16 nPackLen = htons(8 + 24 + 8);
		memcpy(cmd + 30, &nPackLen, sizeof(UINT16));

		//IP地址 RAM地址
		nTmp32 = htonl(MultiRamAddr);
		memcpy(cmd + 32, &nTmp32, sizeof(UINT32));
		//IP地址
		nTmp32 = htonl(nMulti);
		memcpy(cmd + 36, &nTmp32, sizeof(UINT32));

		memcpy(pDstCmd, cmd, 8 + 24 + 8);
		*pLen = 8 + 24 + 8;
	}
} MultiParam_t;

typedef struct _MultiMacParam
{
	unsigned char cmd[8 + 24 + 16];//8字节主/从区分头,24字节万兆指令头,16字节万兆网组播MAC地址

	_MultiMacParam()
	{
		memset(cmd, 0, 8 + 24 + 16);
		//ZXLC
		cmd[8] = 90;	cmd[9] = 88;	cmd[10] = 76;	cmd[11] = 67;

		//目的地址
		cmd[12] = 0xFF;	cmd[13] = 0xFF;	cmd[14] = 0x00;	cmd[15] = 0x02;
	}

	// nSerial		命令流水号（包序号）
	// nBoardNo		板卡号（0, 1）
	// nNetCardNo	万兆网口号（0~3）,对应光口1~4
	// nOption		主从选择, 0:从 1:主
	// MultiMacRamAddr	组播MAC地址 RAM地址
	// nMultiMac	组播MAC地址
	void GetCmd(unsigned char* pDstCmd, int* pLen,
		UINT16 nSerial, UINT8 nBoardNo, UINT8 nNetCardNo, int nOption,
		UINT32 MultiMacRamAddr, UINT64 nMultiMac)
	{
		UINT16 nTmp16 = 0;
		UINT32 nTmp32 = 0;

		//主/从区分头
		if (nOption == 0)//从
		{
			memset(cmd, 0xbc, 8);
		}
		else//主
		{
			memset(cmd, 0xab, 8);
		}

		//流水号
		nTmp16 = htons(nSerial);
		memcpy(cmd + 24, &nTmp16, sizeof(UINT16));

		//万兆网板卡号0、1
		cmd[26] = nBoardNo;
		//万兆网口序号0~3对应0x80、0x81、0x82、0x83(芯片号)
		cmd[27] = 0x80 + nNetCardNo;

		//万兆控制命令0x0006
		UINT16 nCommand = htons(0x0006);
		memcpy(cmd + 28, &nCommand, sizeof(UINT16));
		//包长
		UINT16 nPackLen = htons(8 + 24 + 16);
		memcpy(cmd + 30, &nPackLen, sizeof(UINT16));

		//MAC地址6-3 RAM地址
		nTmp32 = htonl(MultiMacRamAddr);
		memcpy(cmd + 32, &nTmp32, sizeof(UINT32));
		//IPMAC地址6-3
		memcpy(&nTmp32, (char*)(&nMultiMac) + 2, sizeof(UINT32));
		nTmp32 = htonl(nTmp32);
		memcpy(cmd + 36, &nTmp32, sizeof(UINT32));


		//MAC地址2-1 RAM地址
		nTmp32 = htonl(MultiMacRamAddr+1);
		memcpy(cmd + 40, &nTmp32, sizeof(UINT32));
		//MAC地址2-1
		memcpy(&nTmp16, (char*)(&nMultiMac), 2);
		nTmp16 = htons(nTmp16);
		memcpy(cmd + 44, &nTmp16, 2);


		memcpy(pDstCmd, cmd, 8 + 24 + 16);
		*pLen = 8 + 24 + 16;
	}

	UINT64 GetMAC(unsigned char *pMac)
	{
		unsigned char mac[8] = { 0 };
		UINT64 nSrcMAC = 0;
		for (int i = 0; i < 6; i++)
		{
			unsigned char H = 0;
			unsigned char L = 0;
			if ('a' <= pMac[2 * i] && pMac[2 * i] <= 'z')
			{
				H = pMac[2 * i] - 'a' + 10;
			}
			else if ('A' <= pMac[2 * i] && pMac[2 * i] <= 'Z')
			{
				H = pMac[2 * i] - 'A' + 10;
			}
			else if ('0' <= pMac[2 * i] && pMac[2 * i] <= '9')
			{
				H = pMac[2 * i] - '0';
			}

			if ('a' <= pMac[2 * i + 1] && pMac[2 * i + 1] <= 'z')
			{
				L = pMac[2 * i + 1] - 'a' + 10;
			}
			else if ('A' <= pMac[2 * i + 1] && pMac[2 * i + 1] <= 'Z')
			{
				L = pMac[2 * i + 1] - 'A' + 10;
			}
			else if ('0' <= pMac[2 * i + 1] && pMac[2 * i + 1] <= '9')
			{
				L = pMac[2 * i + 1] - '0';
			}

			mac[5 - i] = H * 16 + L;
		}

		memcpy(&nSrcMAC, mac, sizeof(UINT64));

		return nSrcMAC;
	}
} MultiMacParam_t;