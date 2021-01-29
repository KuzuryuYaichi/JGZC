using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace SWDirectCollect_Channel8
{
    public class TestDLLImport
    {

        [StructLayout(LayoutKind.Sequential)]
        public struct TestInformation
        {
            public int Freq;
            public int type; //1:窄带   0：宽带
        }
        public delegate int Func(TestInformation t);
        [DllImport("SWDC_DevDrive_Ctrl.dll", EntryPoint = "RegisterTestInformationCallback", CharSet = CharSet.Ansi,
CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public extern static void RegisterTestInformationCallback(Func func);

        [DllImport("SWDC_DevDrive_Ctrl.dll", EntryPoint = "PushWBData", CharSet = CharSet.Ansi,
CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public extern static byte[] PushWBData(byte[] data);

        [DllImport("SWDC_DevDrive_Ctrl.dll", EntryPoint = "PushNBData", CharSet = CharSet.Ansi,
CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public extern static byte[] PushNBData(byte[] data);

        [DllImport("SWDC_DevDrive_Ctrl.dll", EntryPoint = "InitRCtrl", CharSet = CharSet.Ansi,
CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public extern static void CreateRCtrl();

        [DllImport("SWDC_DevDrive_Ctrl.dll", EntryPoint = "InitRCtrl", CharSet = CharSet.Ansi,
CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public extern static void DestroyRCtrl();
    }
}
