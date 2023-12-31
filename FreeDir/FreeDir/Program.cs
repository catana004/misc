﻿
using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;



namespace ConsoleApp1
{
    class Program
    {
        static void Main(string[] args)
        {
            // トライブすべてを取得
            string[] driveslist = Directory.GetLogicalDrives();
            var drives = new List<String>();
            foreach (String d in driveslist)
                drives.Add(d);

            drives.Add(@"\\cat-nas1\作業用_松浦");

            //　foreachで取得したドライブ配列にループ
            foreach (string s in drives)
            {
                Hashtable data = new Hashtable();
                DirectoryInfo inf = new DirectoryInfo(s);
                /*** ディレクトリの空き容量を取るには
            https://learn.microsoft.com/ja-jp/dotnet/api/system.management.managementscope?view=dotnet-plat-ext-7.0&redirectedfrom=MSDN                DriveInfo drive = new DriveInfo(s);
                が必要らしい***/
                //ドライブの準備ができているか調べる
                if (drive.IsReady)
                {
                    //TotalSizeを取得して変換してHashに格納。
                    data.Add("TotalSize(" + s + "):", ToGigaByte(Convert.ToDouble(drive.TotalSize)));
                    //TotalFreeSpaceを取得して変換してHashに格納。
                    data.Add("TotalFree(" + s + "):", ToGigaByte(Convert.ToDouble(drive.TotalFreeSpace)));
                }
                foreach (DictionaryEntry h in data)
                {
                    Console.WriteLine(h.Key + " " + h.Value);
                }
            }
            Console.ReadLine();
        }
        // GB換算して戻すメソッド(見やすさ優先)
        static int ToGigaByte(Double calcTarget)
        {
            Double i = calcTarget / 1024; //Kbyte
            i /= 1024; //MB
            i /= 1024; //GB
            i = Math.Ceiling(i);
            return Convert.ToInt16(i);
        }
    }
}
