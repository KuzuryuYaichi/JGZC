using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SWDirectCollect_Channel8
{
    public class TestProtocol
    {
        private byte m_PossyNumber;
        private Int16 m_UserNumber;
        private Int16 m_DevNumber;

        public byte[] Cmd_RCtrlRegister()
        {
            byte[] Cmd_Buffer = new byte[28];

            Cmd_Buffer[0] = 0x00;
            Cmd_Buffer[1] = 0xa0;
            Cmd_Buffer[2] = m_PossyNumber;
            byte[] temp = BitConverter.GetBytes(m_UserNumber);
            Buffer.BlockCopy(temp, 0, Cmd_Buffer, 3, temp.Length);
            byte[] temp1 = BitConverter.GetBytes(m_DevNumber);
            Buffer.BlockCopy(temp1, 0, Cmd_Buffer, 5, temp1.Length);
            int xor = 0;
            for (int i = 0; i < 7; i++)
            {
                xor = xor ^ Cmd_Buffer[i];
            }
            Cmd_Buffer[7] = Convert.ToByte(xor);
            byte[] temp2 = Encoding.UTF8.GetBytes("0754754754");
            Buffer.BlockCopy(temp2, 0, Cmd_Buffer, 8, temp2.Length);
            byte[] temp3 = Encoding.UTF8.GetBytes("20CHRCV754");
            Buffer.BlockCopy(temp3, 0, Cmd_Buffer, 18, temp3.Length);
            return Cmd_Buffer;
        }

        public byte[] Cmd_RCtrlReport()
        {
            byte[] Cmd_Buffer = new byte[17];

            Cmd_Buffer[0] = 0x02;
            Cmd_Buffer[1] = 0xa0;
            Cmd_Buffer[2] = m_PossyNumber;
            byte[] temp = BitConverter.GetBytes(m_UserNumber);
            Buffer.BlockCopy(temp, 0, Cmd_Buffer, 3, temp.Length);
            byte[] temp1 = BitConverter.GetBytes(m_DevNumber);
            Buffer.BlockCopy(temp1, 0, Cmd_Buffer, 5, temp1.Length);

            int xor = 0;
            for (int i = 0; i < 7; i++)
            {
                xor = xor ^ Cmd_Buffer[i];
            }
            Cmd_Buffer[7] = Convert.ToByte(xor);
            Cmd_Buffer[8] = 0x00;
            Cmd_Buffer[9] = 0x00;
            Cmd_Buffer[10] = 0x00;
            Cmd_Buffer[11] = 131;
            Cmd_Buffer[12] = 82;
            Cmd_Buffer[13] = 0;
            Cmd_Buffer[14] = 128;
            Cmd_Buffer[15] = 6001 & 0xff;
            Cmd_Buffer[15] = (6001 >> 8) & 0xff;

            return Cmd_Buffer;
        }

        public byte[] Cmd_RCtrlHeart()
        {
            byte[] Cmd_Buffer = new byte[10];

            Cmd_Buffer[0] = 0x02;
            Cmd_Buffer[1] = 0xa0;
            Cmd_Buffer[2] = m_PossyNumber;
            byte[] temp = BitConverter.GetBytes(m_UserNumber);
            Buffer.BlockCopy(temp, 0, Cmd_Buffer, 3, temp.Length);
            byte[] temp1 = BitConverter.GetBytes(m_DevNumber);
            Buffer.BlockCopy(temp1, 0, Cmd_Buffer, 5, temp1.Length);

            int xor = 0;
            for (int i = 0; i < 7; i++)
            {
                xor = xor ^ Cmd_Buffer[i];
            }
            Cmd_Buffer[7] = Convert.ToByte(xor);
            Cmd_Buffer[8] = 0x0b;
            Cmd_Buffer[9] = 0x0a;

            return Cmd_Buffer;
        }

        public byte[] Cmd_ManagementHead(Int32 Index,Int16 DataLen)
        {
            byte[] Cmd_Buffer = new byte[21];
            Cmd_Buffer[0] = 0xFE;
            Cmd_Buffer[1] = 0xEF;
            Cmd_Buffer[2] = 0x9F;
            Cmd_Buffer[3] = 0xF9;
            byte[] temp = BitConverter.GetBytes(Index);
            Buffer.BlockCopy(temp, 0, Cmd_Buffer, 4, temp.Length);

            int xor = 0;
            for (int i = 0; i < 6; i++)
            {
                xor = xor ^ Cmd_Buffer[i];
            }
            Cmd_Buffer[8] = Convert.ToByte(xor);
            Cmd_Buffer[9] = 0x10;

            byte[] temp1 = BitConverter.GetBytes(DataLen);
            Buffer.BlockCopy(temp1, 0, Cmd_Buffer, 10, temp1.Length);

            Cmd_Buffer[12] = m_PossyNumber;
            byte[] temp2 = BitConverter.GetBytes(m_UserNumber);
            Buffer.BlockCopy(temp2, 0, Cmd_Buffer, 13, temp2.Length);
            byte[] temp3 = BitConverter.GetBytes(m_DevNumber);
            Buffer.BlockCopy(temp3, 0, Cmd_Buffer, 15, temp3.Length);

            return Cmd_Buffer;
        }

        public int Cmd_RecvRCtrlNotarize(byte[] RecvData)
        {
            if (RecvData[0] != 0x01 && RecvData[1] != 0xaa && RecvData[8] != 0x34 && RecvData[9] != 0x12)
                return -101;
            m_PossyNumber = RecvData[2];
            m_UserNumber = BitConverter.ToInt16(RecvData, 3);
            m_DevNumber = BitConverter.ToInt16(RecvData, 5);
            return 0;
        }

        public int Cmd_RecvCtrlData(byte[] RecvData, ref Int64 Frequency,ref int Type)
        {
            if (RecvData[22]!=0xb0 || RecvData[21]!=0x01 || RecvData[24]!=0x0f || RecvData[23]!=0x16)
                return -101;
            if (RecvData[27] == Convert.ToByte("F") && RecvData[28] != Convert.ToByte("S") && RecvData[28] != Convert.ToByte("?"))
            {
                Frequency = BitConverter.ToInt64(RecvData, 28);
                Type = 1;
            }
            else if (RecvData[27] == Convert.ToByte("N") && RecvData[28] == Convert.ToByte("C") && RecvData[29] == Convert.ToByte("F"))
            {
                Frequency = BitConverter.ToInt64(RecvData, 34);
                Type = 2;
            }

            return 0;
        }
    }
}
