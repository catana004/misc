using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Management;
using System.Runtime.InteropServices;

//[System.ComponentModel.TypeConverter(typeof(System.Management.ManagementScopeConverter))]
//public class ManagementScope : ICloneable


namespace WinDirFree
{
    public partial class Form1 : Form
    {
        [System.Runtime.InteropServices.DllImport("kernel32.dll", CharSet = System.Runtime.InteropServices.CharSet.Auto)]
        [return: System.Runtime.InteropServices.MarshalAs(System.Runtime.InteropServices.UnmanagedType.Bool)]
        public static extern bool GetDiskFreeSpaceEx(
        string lpDirectoryName,
        out ulong lpFreeBytesAvailable,
        out ulong lpTotalNumberOfBytes,
        out ulong lpTotalNumberOfFreeBytes);

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            {
                String netpath = "\\\\cat-nas1\\作業用_松浦";
                ulong freeBytesAvailable;
                ulong totalNumberOfBytes;
                ulong totalNumberOfFreeBytes;

                GetDiskFreeSpaceEx(netpath, out freeBytesAvailable, out totalNumberOfBytes, out totalNumberOfFreeBytes);

                Console.WriteLine("FreeBytesAvailable {0} totalBytes {1} totalFreeBytes {2}",
                    freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes);
            }
            {
                /*// Build an options object for the remote connection
                //   if you plan to connect to the remote
                //   computer with a different user name
                //   and password than the one you are currently using

                     ConnectionOptions options =
                         new ConnectionOptions();

                     // and then set the options.Username and
                     // options.Password properties to the correct values
                     // and also set
                     // options.Authority = "ntlmdomain:DOMAIN";
                     // and replace DOMAIN with the remote computer's
                     // domain.  You can also use Kerberos instead
                     // of ntlmdomain.
                */

                // Make a connection to a remote computer.
                // Replace the "FullComputerName" section of the
                // string "\\\\FullComputerName\\root\\cimv2" with
                // the full computer name or IP address of the
                // remote computer.
                ManagementScope scope =
                    new ManagementScope(
                        @"\\cat-nas1");
//                @"\\cat-nas1\作業用_松浦");
                scope.Connect();

                // Use this code if you are connecting with a
                // different user name and password:
                //
                // ManagementScope scope =
                //    new ManagementScope(
                //        "\\\\FullComputerName\\root\\cimv2", options);
                // scope.Connect();

                //Query system for Operating System information
                ObjectQuery query = new ObjectQuery(
                "SELECT * FROM Win32_LogicalDisk"
                //"SELECT * FROM Win32_Directory"
                );
                ManagementObjectSearcher searcher =
                    new ManagementObjectSearcher(scope, query);

                ManagementObjectCollection queryCollection = searcher.Get();
                foreach (ManagementObject m in queryCollection)
                {
                    var v1 = m.Properties;
                    Console.WriteLine("=============");
                    foreach( var v2 in v1)
                    {
                        Console.WriteLine("Key {0} Value {1}" , v2.Name , v2.Value);

                    }
                    //foreach( var n in m)
                    //{
                    //    Console.WriteLine("Key {0} Value {1}", n.Key , n.Value );
                    //}

                    //// Display the remote computer information
                    //Console.WriteLine("Computer Name : {0}",
                    //    m["csname"]);
                    //Console.WriteLine("Windows Directory : {0}",
                    //    m["WindowsDirectory"]);
                    //Console.WriteLine("Operating System: {0}",
                    //    m["Caption"]);
                    //Console.WriteLine("Version: {0}", m["Version"]);
                    //Console.WriteLine("Manufacturer : {0}",
                    //    m["Manufacturer"]);
                }
            }
        }
    }
}
