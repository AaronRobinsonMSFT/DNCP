// Copyright 2022 Aaron R Robinson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

using System;
using System.Runtime.InteropServices;

using Xunit;

namespace ComClient
{
    [Guid("4A23EA71-363E-4609-AD85-CFBF5716A9FA")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IComServer
    {
        [return: MarshalAs(UnmanagedType.BStr)]
        string GuidToString(in Guid guid);

        Guid StringToGuid([MarshalAs(UnmanagedType.LPWStr)] string? guidAsString);

        void DoubleIntegers(
            int length,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)] int[] integers,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)] out int[] result);
    }

    public static unsafe class ComServer
    {
        [DllImport("comserver", PreserveSig = false)]
        public static extern void CreateComServer(Guid* iid, IntPtr* server);

        public static void TestGuidToString(IComServer server)
        {
            var guid = Guid.NewGuid();
            string asStr = server.GuidToString(in guid);

            // The COM APIs being used convert GUIDs to uppercase.
            Assert.Equal(guid.ToString("B").ToUpper(), asStr);
        }

        public static void TestStringToGuid(IComServer server)
        {
            Assert.Equal(Guid.Empty, server.StringToGuid(null));

            var valid = new[] {
                "{12345678-9abc-def0-1234-56789ABCDEF0}",
                "{98cdc830-f1fa-4b61-ad73-88786fd6aeea}",
                "{dbccf98e-c950-452d-9524-d3da45d6a704}",
                "{ff15c80b-d53c-45ab-8ac6-fcca2125e240}",
                "{9baf5b68-3750-46f0-8829-686666518c01}",
                "{af8aaf7d-2cc4-4d53-9e02-cd960136a5e4}",
                "{93A121DD-17A0-4652-A94C-9A3D88C1B7CA}",
                "{F6497567-CC6F-46CF-8D1B-FB782474C534}",
                "{B5A005E3-96BB-46DE-946D-9713A4D15398}",
                "{08B6B51F-8F58-4CB7-90EA-7C37B8F0A4D7}",
                "{D5F940C9-30A1-487C-9AF2-77961BCD11F3}"
            };
            foreach (var v in valid)
            {
                Assert.Equal(new Guid(v), server.StringToGuid(v));
            }

            var invalid = new[] {
                "{123456789abc-def0-1234-56789ABCDEF0}",
                "{12345678-9abcdef0-1234-56789ABCDEF0}",
                "{12345678-9abc-def01234-56789ABCDEF0}",
                "{12345678-9abc-def0-123456789ABCDEF0}",
                "{12345678-9abc-def0-1234-56789ABCDEF0",
                "12345678-9abc-def0-1234-56789ABCDEF0}",
                "{12345678-9abc-def0-1234-56789ABCDEF0} "
            };
            foreach (var i in invalid)
            {
                Assert.Throws<ArgumentException>(() => server.StringToGuid(i));
            }
        }

        public static void TestDoubleIntegers(IComServer server)
        {
            Assert.Throws<NullReferenceException>(() => server.DoubleIntegers(1, null!, out int[] _));

            int[] ints = new[] { 1, 2, 3, 4, 5 };
            server.DoubleIntegers(ints.Length, ints, out int[] result);

            for (int i = 0; i < ints.Length; ++i)
            {
                Assert.Equal(ints[i] * 2, result[i]);
            }
        }
    }
}