using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Common.ComponentCs.Log;
using Common.ComponentCs;

namespace SWDirectCollect_Channel8
{
    public  class Test_DataProcessing
    {
        private AsyncDataQueue<byte[]> m_queue;

        private TCPSocket DataSocket;
        public Test_DataProcessing()
        {
            m_queue = new AsyncDataQueue<byte[]>();
            m_queue.TEvent += new THandler<byte[]>(m_queue_TEvent);
        }

        private int m_DataType = 0;
        public void SelectDataType(int DataType)
        {
            m_DataType = DataType;
        }

        private byte[] m_Data = new byte[60 * 1024];
        private int DataLen = 0;
        private int DataPackets = 0;
        private int DataIndex = 0;
        public void PushData(DataAndFreq t)
        {
            if (DataLen != t.Data.Length)
            {
                DataLen = t.Data.Length;
                DataPackets = (60 * 1024) / DataLen;
            }

            byte[] TestIQData = new byte[DataLen];
            for (int i = 0; i < DataLen / 4; i++)
            {
                byte temp1 = TestIQData[4 * i + 3];
                byte temp2 = TestIQData[4 * i + 2];
                byte temp3 = TestIQData[4 * i + 1];
                byte temp4 = TestIQData[4 * i];
                TestIQData[4 * i + 3] = temp1;
                TestIQData[4 * i + 2] = temp2;
                TestIQData[4 * i + 1] = temp3;
                TestIQData[4 * i] = temp1;
            }

            Buffer.BlockCopy(TestIQData, 0, m_Data, DataIndex * DataLen, DataLen);
            DataIndex++;
            if (DataIndex < DataPackets)
                return;
            DataIndex = 0;
            m_queue.Enqueue(m_Data);
        }

        void m_queue_TEvent(byte[] t)
        {
            byte[] SendData = new byte[60 * 1024 + 123];
            if (m_DataType == 0)
            {
                ///TODO:进入dll的宽带数据，添加管理头
            }
            else if(m_DataType == 1)
            {
                ///TODO:进入dll的窄带滤波器，添加管理头
            }
            DataSocket.Send(SendData);
        }
    }
}
