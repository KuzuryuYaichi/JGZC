using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.Net;
using System.Threading;

namespace SWDirectCollect_Channel8
{
    public class TCPSocket
    {
        private Socket m_MySocket;
        private Thread RecvThread;

        public delegate void PassInformationr(byte[] t);
        public event PassInformationr passParameter;

        public int TCPInit(string LocalIP, int LocalPort, string DestIP, int DestPort)
        {
            m_MySocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            m_MySocket.ReceiveTimeout = 1000;
            m_MySocket.ReceiveBufferSize = 1024 * 1024 * 1024;
            IPEndPoint LocalIPEndPoint = new IPEndPoint(IPAddress.Parse(LocalIP), LocalPort);
            IPEndPoint DestIPEndPoint = new IPEndPoint(IPAddress.Parse(DestIP), DestPort);

            m_MySocket.Bind(LocalIPEndPoint);
            m_MySocket.Connect(DestIPEndPoint);
            if (!m_MySocket.Connected)
            {
                return -1;
            }

            RecvThread = new Thread(ReceiveData);
            RecvThread.IsBackground = false;
            RecvThread.Start();

            return 0;
        }

        public int Send(byte[] data)
        {
            int res = m_MySocket.Send(data, data.Length, 0);
            return res;
        }

        void ReceiveData()
        {
            byte[] data = new byte[1024];
            while (true)
            {
                int res = m_MySocket.Receive(data, data.Length, 0);
                if (res < 0)
                    continue;
                else
                {
                    if (passParameter != null)
                    {
                        passParameter(data);
                    }
                }
            }

        }
    }
}
