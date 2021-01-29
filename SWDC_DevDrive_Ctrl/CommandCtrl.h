#include "string.h"
#include "basetsd.h"
#include "winsock2.h"


typedef struct _SrcParam 
{
	unsigned char cmd[8+24+64+8];//8�ֽ���/������ͷ,24�ֽ�����ָ��ͷ,64�ֽ�������Դ����

	_SrcParam()
	{
		memset(cmd, 0, 8+24+64+8);
		//ZXLC
		cmd[8] = 90;	cmd[9] = 88;	cmd[10] = 76;	cmd[11] = 67;

		//Ŀ�ĵ�ַ
		cmd[12] = 0xFF;	cmd[13] = 0xFF;	cmd[14] = 0x00;	cmd[15] = 0x02;
	}

	// nSerial		������ˮ�ţ�����ţ�
	// nBoardNo		�忨�ţ�0, 1��
	// nNetCardNo	�������ںţ�0~3��,��Ӧ���1~4
	// nOption		����ѡ��, 0:�� 1:��
	// nSrcIP		��������ԴIP
	// nSrcMAC		��������ԴMAC
	// nInterval	MAC֡����ʱ�Ӽ���:Ĭ��Ϊ0
	// nPacketLen	UDP��Ƭ��������:1472
	// nBitControl	λ���ƼĴ���
	// nHeartPort	�����������˿ڣ�δʹ�ã�
	// nSlicesClock ��Ƭ�����ʱ��������
	void GetCmd(unsigned char* pDstCmd, int* pLen, 
				UINT16 nSerial, UINT8 nBoardNo, UINT8 nNetCardNo, int nOption,
				UINT32 nSrcIP, UINT64 nSrcMAC, 
				UINT32 nInterval, UINT16 nPacketLen,
				UINT32 nBitControl, UINT16 nHeartPort, UINT32 nSlicesClock)
	{
		UINT16 nTmp16 = 0;
		UINT32 nTmp32 = 0;

		//��/������ͷ
		if (nOption == 0)//��
		{
			memset(cmd, 0xbc, 8);
		} 
		else//��
		{
			memset(cmd, 0xab, 8);
		}

		//��ˮ��
		nTmp16 = htons(nSerial);
		memcpy(cmd+24, &nTmp16, sizeof(UINT16));

		//�������忨��0��1
		cmd[26] = nBoardNo;
		//�����������0~3��Ӧ0x80��0x81��0x82��0x83(оƬ��)
		cmd[27] = 0x80+nNetCardNo;

		//���׿�������0x0001
		UINT16 nCommand = htons(0x0001);
		memcpy(cmd+28, &nCommand, sizeof(UINT16));
		//����
		UINT16 nPackLen = htons(8+24+64);
		memcpy(cmd+30, &nPackLen, sizeof(UINT16));

		//ԴIP RAM��ַ
		UINT32 RamAddr = htonl(0x00000000);
		memcpy(cmd+32, &RamAddr, sizeof(UINT32));
		nTmp32 = htonl(nSrcIP);
		memcpy(cmd+36, &nTmp32, sizeof(UINT32));

		//ԴMAC(6-3) RAM��ַ
		RamAddr = htonl(0x00000001);
		memcpy(cmd+40, &RamAddr, sizeof(UINT32));
		memcpy(&nTmp32, (char*)(&nSrcMAC) + 2, sizeof(UINT32));
		nTmp32 = htonl(nTmp32);
		memcpy(cmd+44, &nTmp32, sizeof(UINT32));

		//ԴMAC(2-1) RAM��ַ
		RamAddr = htonl(0x00000002);
		memcpy(cmd+48, &RamAddr, sizeof(UINT32));
		memcpy(&nTmp16, (char*)(&nSrcMAC), 2);
		nTmp16 = htons(nTmp16);
		memcpy(cmd+52, &nTmp16, 2);

		//�����
		RamAddr = htonl(0x00000003);
		memcpy(cmd+56, &RamAddr, sizeof(UINT32));
		nTmp32 = htonl(nInterval);
		memcpy(cmd+60, &nTmp32, sizeof(UINT32));

		//λ���ƼĴ���
		RamAddr = htonl(0x00000004);
		memcpy(cmd+64, &RamAddr, sizeof(UINT32));
		nTmp32 = htonl(nBitControl);
		memcpy(cmd+68, &nTmp32, sizeof(UINT32));

		//�����˿�
		RamAddr = htonl(0x00000005);
		memcpy(cmd+72, &RamAddr, sizeof(UINT32));
		nTmp32 = nHeartPort;
		nTmp32 = htonl(nTmp32);
		memcpy(cmd+76, &nTmp32, sizeof(UINT32));

		//��Ƭ����
		RamAddr = htonl(0x00000006);
		memcpy(cmd+80, &RamAddr, sizeof(UINT32));
		nTmp32 = nPacketLen;
		nTmp32 = htonl(nTmp32);
		memcpy(cmd+84, &nTmp32, sizeof(UINT32));

		//����
		RamAddr = htonl(0x00000007);
		memcpy(cmd+88, &RamAddr, sizeof(UINT32));
		nTmp32 = 2;
		nTmp32 = htonl(nTmp32);
		memcpy(cmd + 92, &nTmp32, sizeof(UINT32));

		//��Ƭ����
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
	unsigned char cmd[8+24+16];//8�ֽ���/������ͷ,24�ֽ�����ָ��ͷ,16�ֽ�������Ŀ�Ĳ���

	_DstParam()
	{
		memset(cmd, 0, 8+24+16);
		//ZXLC
		cmd[8] = 90;	cmd[9] = 88;	cmd[10] = 76;	cmd[11] = 67;

		//Ŀ�ĵ�ַ
		cmd[12] = 0xFF;	cmd[13] = 0xFF;	cmd[14] = 0x00;	cmd[15] = 0x02;
	}

	// nSerial		������ˮ�ţ�����ţ�
	// nBoardNo		�忨�ţ�0, 1��
	// nNetCardNo	�������ںţ�0~3��,��Ӧ���1~4
	// nOption		����ѡ��, 0:�� 1:��
	// IPRamAddr	Ŀ��IP��ַ RAM��ַ
	// nIP			Ŀ��IP��ַ
	// PortRamAddr	UDPԴ�˿�/Ŀ�Ķ˿� RAM��ַ
	// nSrcPort		UDPԴ�˿�
	// nDstPort		UDPĿ�Ķ˿�
	void GetCmd(unsigned char* pDstCmd, int* pLen, 
		UINT16 nSerial, UINT8 nBoardNo, UINT8 nNetCardNo, int nOption,
		UINT32 IPRamAddr, UINT32 nIP, UINT32 PortRamAddr, UINT16 nSrcPort, UINT16 nDstPort)
	{
		UINT16 nTmp16 = 0;
		UINT32 nTmp32 = 0;

		//��/������ͷ
		if (nOption == 0)//��
		{
			memset(cmd, 0xbc, 8);
		} 
		else//��
		{
			memset(cmd, 0xab, 8);
		}

		//��ˮ��
		nTmp16 = htons(nSerial);
		memcpy(cmd+24, &nTmp16, sizeof(UINT16));

		//�������忨��0��1
		cmd[26] = nBoardNo;
		//�����������0~3��Ӧ0x80��0x81��0x82��0x83(оƬ��)
		cmd[27] = 0x80+nNetCardNo;

		//���׿�������0x0003
		UINT16 nCommand = htons(0x0003);
		memcpy(cmd+28, &nCommand, sizeof(UINT16));
		//����
		UINT16 nPackLen = htons(8+24+16);
		memcpy(cmd+30, &nPackLen, sizeof(UINT16));

		//IP��ַ RAM��ַ
		nTmp32 = htonl(IPRamAddr);
		memcpy(cmd+32, &nTmp32, sizeof(UINT32));
		//IP��ַ
		nTmp32 = htonl(nIP);
		memcpy(cmd + 36, &nTmp32, sizeof(UINT32));

		//Դ��Ŀ�Ķ˿� RAM��ַ
		nTmp32 = htonl(PortRamAddr);
		memcpy(cmd+40, &nTmp32, sizeof(UINT32));

		//
		// Դ�˿ں�Ŀ�Ķ˿ڿ��ܻᷴ
		//

		////Ŀ�Ķ˿�
		//nTmp16 = htons(nDstPort);
		//memcpy(cmd+44, &nTmp16, sizeof(UINT16));

		////Դ�˿�
		//nTmp16 = htons(nSrcPort);
		//memcpy(cmd+46, &nTmp16, sizeof(UINT16));

		//Ŀ�Ķ˿�
		nTmp16 = htons(nSrcPort);
		memcpy(cmd + 44, &nTmp16, sizeof(UINT16));

		//Դ�˿�
		nTmp16 = htons(nDstPort);
		memcpy(cmd + 46, &nTmp16, sizeof(UINT16));


		memcpy(pDstCmd, cmd, 8+24+16);
		*pLen = 8+24+16;
	}
} DstParam_t;

typedef struct _LinkEnable 
{
	unsigned char cmd[8+24+16];//8�ֽ���/������ͷ,24�ֽ�����ָ��ͷ,16�ֽڣ���8�ֽ�Ϊ����-����16�ֽڣ�����������ʹ�ܲ���

	_LinkEnable()
	{
		memset(cmd, 0, 8+24+16);
		//ZXLC
		cmd[8] = 90;	cmd[9] = 88;	cmd[10] = 76;	cmd[11] = 67;

		//Ŀ�ĵ�ַ
		cmd[12] = 0xFF;	cmd[13] = 0xFF;	cmd[14] = 0x00;	cmd[15] = 0x02;
	}

	// nSerial		������ˮ�ţ�����ţ�
	// nBoardNo		�忨�ţ�0, 1��
	// nNetCardNo	�������ںţ�0~3��,��Ӧ���1~4
	// nOption		����ѡ��, 0:�� 1:��
	// EnableRamAddr	����ʹ�� RAM��ַ
	// nEnable			����ʹ��, 0:��ֹ 1:ʹ��
	void GetCmd(unsigned char* pDstCmd, int* pLen, 
		UINT16 nSerial, UINT8 nBoardNo, UINT8 nNetCardNo, int nOption,
		UINT32 EnableRamAddr, UINT32 nEnable)
	{
		UINT16 nTmp16 = 0;
		UINT32 nTmp32 = 0;

		//��/������ͷ
		if (nOption == 0)//��
		{
			memset(cmd, 0xbc, 8);
		} 
		else//��
		{
			memset(cmd, 0xab, 8);
		}

		//��ˮ��
		nTmp16 = htons(nSerial);
		memcpy(cmd+24, &nTmp16, sizeof(UINT16));
		
		//�������忨��0��1
		cmd[26] = nBoardNo;
		//�����������0~3��Ӧ0x80��0x81��0x82��0x83(оƬ��)
		cmd[27] = 0x80+nNetCardNo;

		//���׿�������0x0004
		UINT16 nCommand = htons(0x0004);
		memcpy(cmd+28, &nCommand, sizeof(UINT16));
		//����
		UINT16 nPackLen = htons(8+24+16);
		memcpy(cmd+30, &nPackLen, sizeof(UINT16));

		//ʹ�� RAM��ַ
		nTmp32 = htonl(EnableRamAddr);
		memcpy(cmd+32, &nTmp32, sizeof(UINT32));
		//ʹ��
		nTmp32 = htonl(nEnable);
		memcpy(cmd+36, &nTmp32, sizeof(UINT32));

		//RAM��ַ,������������Ч��ַ���жϣ�
		nTmp32 = htonl(0x0000FFFF);//��Ҫ������Ч��ַ0x000000FF
		memcpy(cmd+40, &nTmp32, sizeof(UINT32));
		//����
		//nTmp32 = htonl();
		//memcpy(cmd+44, &nTmp32, sizeof(UINT32));

		memcpy(pDstCmd, cmd, 8+24+16);
		*pLen = 8+24+16;
	}
} LinkEnable_t;

typedef struct _AllEnable
{
	unsigned char cmd[24 + 16 + 8 * 40];//8�ֽ���/������ͷ,24�ֽ�����ָ��ͷ,16�ֽڣ���8�ֽ�Ϊ����-����16�ֽڣ�����������ʹ�ܲ���

	_AllEnable()
	{
		memset(cmd, 0,  24 + 16 + 8 * 40);
		//ZXLC
		cmd[8] = 90;	cmd[9] = 88;	cmd[10] = 76;	cmd[11] = 67;

		//Ŀ�ĵ�ַ
		cmd[12] = 0xFF;	cmd[13] = 0xFF;	cmd[14] = 0x00;	cmd[15] = 0x02;
	}

	// nSerial		������ˮ�ţ�����ţ�
	// nBoardNo		�忨�ţ�0, 1��
	// nNetCardNo	�������ںţ�0~3��,��Ӧ���1~4
	// nOption		����ѡ��, 0:�� 1:��
	// EnableRamAddr	����ʹ�� RAM��ַ
	// nEnable			����ʹ��, 0:��ֹ 1:ʹ��
	void GetCmd(unsigned char* pDstCmd, int* pLen,
		UINT16 nSerial, UINT8 nBoardNo, UINT8 nNetCardNo, int nOption,
		UINT32 nEnable)
	{
		UINT16 nTmp16 = 0;
		UINT32 nTmp32 = 0;

		//��/������ͷ
		if (nOption == 0)//��
		{
			memset(cmd, 0xbc, 8);
		}
		else//��
		{
			memset(cmd, 0xab, 8);
		}

		//��ˮ��
		nTmp16 = htons(nSerial);
		memcpy(cmd + 24, &nTmp16, sizeof(UINT16));

		//�������忨��0��1
		cmd[26] = nBoardNo;
		//�����������0~3��Ӧ0x80��0x81��0x82��0x83(оƬ��)
		cmd[27] = 0x80 + nNetCardNo;

		//���׿�������0x0004
		UINT16 nCommand = htons(0x0004);
		memcpy(cmd + 28, &nCommand, sizeof(UINT16));
		//����
		UINT16 nPackLen = htons(8 + 24 + 16);
		memcpy(cmd + 30, &nPackLen, sizeof(UINT16));

		for (int i = 0; i < 40;i++)
		{
			//ʹ�� RAM��ַ
			nTmp32 = htonl(i);
			memcpy(cmd + 32 + 8 * i, &nTmp32, sizeof(UINT32));
			//ʹ��
			nTmp32 = htonl(nEnable);
			memcpy(cmd + 36 + 8 * i, &nTmp32, sizeof(UINT32));
		}




		//RAM��ַ,������������Ч��ַ���жϣ�
		nTmp32 = htonl(0x0000FFFF);//��Ҫ������Ч��ַ0x000000FF
		memcpy(cmd + 40 + 8 * 39, &nTmp32, sizeof(UINT32));
		//����
		//nTmp32 = htonl();
		//memcpy(cmd+44, &nTmp32, sizeof(UINT32));

		memcpy(pDstCmd, cmd, 24 + 16 + 8 * 40);
		*pLen = 24 + 16 + 8 * 40;
	}
} AllEnable_t;

typedef struct _MultiParam
{
	unsigned char cmd[8 + 24 + 8];//8�ֽ���/������ͷ,24�ֽ�����ָ��ͷ,8�ֽ������������鲥ѡ��

	_MultiParam()
	{
		memset(cmd, 0, 8 + 24 + 8);
		//ZXLC
		cmd[8] = 90;	cmd[9] = 88;	cmd[10] = 76;	cmd[11] = 67;

		//Ŀ�ĵ�ַ
		cmd[12] = 0xFF;	cmd[13] = 0xFF;	cmd[14] = 0x00;	cmd[15] = 0x02;
	}

	// nSerial		������ˮ�ţ�����ţ�
	// nBoardNo		�忨�ţ�0, 1��
	// nNetCardNo	�������ںţ�0~3��,��Ӧ���1~4
	// nOption		����ѡ��, 0:�� 1:��
	// MultiRamAddr �����鲥ѡ�� RAM��ַ
	// nMulti       �����鲥ѡ�� 0������   1���鲥
	void GetCmd(unsigned char* pDstCmd, int* pLen,
		UINT16 nSerial, UINT8 nBoardNo, UINT8 nNetCardNo, int nOption,
		UINT32 MultiRamAddr, UINT32 nMulti)
	{
		UINT16 nTmp16 = 0;
		UINT32 nTmp32 = 0;

		//��/������ͷ
		if (nOption == 0)//��
		{
			memset(cmd, 0xbc, 8);
		}
		else//��
		{
			memset(cmd, 0xab, 8);
		}

		//��ˮ��
		nTmp16 = htons(nSerial);
		memcpy(cmd + 24, &nTmp16, sizeof(UINT16));

		//�������忨��0��1
		cmd[26] = nBoardNo;
		//�����������0~3��Ӧ0x80��0x81��0x82��0x83(оƬ��)
		cmd[27] = 0x80 + nNetCardNo;

		//���׿�������0x0005
		UINT16 nCommand = htons(0x0005);
		memcpy(cmd + 28, &nCommand, sizeof(UINT16));
		//����
		UINT16 nPackLen = htons(8 + 24 + 8);
		memcpy(cmd + 30, &nPackLen, sizeof(UINT16));

		//IP��ַ RAM��ַ
		nTmp32 = htonl(MultiRamAddr);
		memcpy(cmd + 32, &nTmp32, sizeof(UINT32));
		//IP��ַ
		nTmp32 = htonl(nMulti);
		memcpy(cmd + 36, &nTmp32, sizeof(UINT32));

		memcpy(pDstCmd, cmd, 8 + 24 + 8);
		*pLen = 8 + 24 + 8;
	}
} MultiParam_t;

typedef struct _MultiMacParam
{
	unsigned char cmd[8 + 24 + 16];//8�ֽ���/������ͷ,24�ֽ�����ָ��ͷ,16�ֽ��������鲥MAC��ַ

	_MultiMacParam()
	{
		memset(cmd, 0, 8 + 24 + 16);
		//ZXLC
		cmd[8] = 90;	cmd[9] = 88;	cmd[10] = 76;	cmd[11] = 67;

		//Ŀ�ĵ�ַ
		cmd[12] = 0xFF;	cmd[13] = 0xFF;	cmd[14] = 0x00;	cmd[15] = 0x02;
	}

	// nSerial		������ˮ�ţ�����ţ�
	// nBoardNo		�忨�ţ�0, 1��
	// nNetCardNo	�������ںţ�0~3��,��Ӧ���1~4
	// nOption		����ѡ��, 0:�� 1:��
	// MultiMacRamAddr	�鲥MAC��ַ RAM��ַ
	// nMultiMac	�鲥MAC��ַ
	void GetCmd(unsigned char* pDstCmd, int* pLen,
		UINT16 nSerial, UINT8 nBoardNo, UINT8 nNetCardNo, int nOption,
		UINT32 MultiMacRamAddr, UINT64 nMultiMac)
	{
		UINT16 nTmp16 = 0;
		UINT32 nTmp32 = 0;

		//��/������ͷ
		if (nOption == 0)//��
		{
			memset(cmd, 0xbc, 8);
		}
		else//��
		{
			memset(cmd, 0xab, 8);
		}

		//��ˮ��
		nTmp16 = htons(nSerial);
		memcpy(cmd + 24, &nTmp16, sizeof(UINT16));

		//�������忨��0��1
		cmd[26] = nBoardNo;
		//�����������0~3��Ӧ0x80��0x81��0x82��0x83(оƬ��)
		cmd[27] = 0x80 + nNetCardNo;

		//���׿�������0x0006
		UINT16 nCommand = htons(0x0006);
		memcpy(cmd + 28, &nCommand, sizeof(UINT16));
		//����
		UINT16 nPackLen = htons(8 + 24 + 16);
		memcpy(cmd + 30, &nPackLen, sizeof(UINT16));

		//MAC��ַ6-3 RAM��ַ
		nTmp32 = htonl(MultiMacRamAddr);
		memcpy(cmd + 32, &nTmp32, sizeof(UINT32));
		//IPMAC��ַ6-3
		memcpy(&nTmp32, (char*)(&nMultiMac) + 2, sizeof(UINT32));
		nTmp32 = htonl(nTmp32);
		memcpy(cmd + 36, &nTmp32, sizeof(UINT32));


		//MAC��ַ2-1 RAM��ַ
		nTmp32 = htonl(MultiMacRamAddr+1);
		memcpy(cmd + 40, &nTmp32, sizeof(UINT32));
		//MAC��ַ2-1
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