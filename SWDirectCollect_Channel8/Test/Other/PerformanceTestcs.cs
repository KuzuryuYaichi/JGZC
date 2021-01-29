using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace SWDirectCollect_Channel8
{
    public class PerformanceTestcs
    {
        private TCPSocket RCtrlSocket = new TCPSocket();
        private TCPSocket CtrlSocket = new TCPSocket();
        private TestProtocol TestProtocol = new TestProtocol();

        private string RCtrlIP = "128.0.82.131";
        private int RCtrlPort = 6000;
        private string CtrlIP = "128.0.82.131";
        private int CtrlPort = 6001;
        private string LocalIP = "128.0.82.130";
        private int LocalRCtrlPort = 5527;
        private int LocalCtrlPort = 5528;

        public PerformanceTestcs()
        {
            RCtrlSocket.TCPInit(LocalIP, LocalRCtrlPort, RCtrlIP, RCtrlPort);
            CtrlSocket.TCPInit(LocalIP, LocalCtrlPort, CtrlIP, CtrlPort);

            RCtrlSocket.passParameter += new TCPSocket.PassInformationr(RCtrlSocket_passParameter);
            CtrlSocket.passParameter += new TCPSocket.PassInformationr(CtrlSocket_passParameter);
        }

        void CtrlSocket_passParameter(byte[] t)
        {
            Int64 Frequency = 0;
            int Type = 0;
            int res = TestProtocol.Cmd_RecvCtrlData(t, ref Frequency, ref Type);
        }

        void RCtrlSocket_passParameter(byte[] t)
        {
            int res = TestProtocol.Cmd_RecvRCtrlNotarize(t);
            if (res == 0)
            {
                
            }
        }
    }
}
