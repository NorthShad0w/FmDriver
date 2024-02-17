// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <tlhelp32.h>
#include <string>
#include <locale.h>
#include"Api.h"
#include"dll.h"
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
DWORD GetProcessID(LPCTSTR lpProcessName)//根据进程名查找进程PID 
{
    DWORD dwRet = 0;
    HANDLE hSnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapShot == INVALID_HANDLE_VALUE)
    {
        printf("\n获得进程快照失败,返回的GetLastError():%d", ::GetLastError());
        return dwRet;
    }

    PROCESSENTRY32 pe32;//声明进程入口对象 
    pe32.dwSize = sizeof(PROCESSENTRY32);//填充进程入口对象大小 
    ::Process32First(hSnapShot, &pe32);//遍历进程列表 
    do
    {
        if (!lstrcmp(pe32.szExeFile, lpProcessName))//查找指定进程名的PID 
        {
            dwRet = pe32.th32ProcessID;
            break;
        }
    } while (::Process32Next(hSnapShot, &pe32));
    ::CloseHandle(hSnapShot);
    return dwRet;//返回 
}
int main(int argc,char * argv[]) 
{
   // ULONG PID = GetProcessID("MfcRwProcss.exe"); //59480;

   // ULONG64 Mudlue =  Fm_GetModule(PID, "MfcRwProcss.exe");

   // printf("Mudlue -> %llx \r\n", Mudlue);

   // unsigned char buffer[10] = { 0 };
   // printf("{ ");
   //// ULONG64 buffer = 0;
   // 
   // Fm_ReadMemory(PID, Mudlue, buffer, sizeof(buffer));

   // for (int i = 0; i < sizeof(buffer); i++)
   // {
   //     printf("0x%02X", buffer[i]);
   // }
   // printf("} \r\n");
   // char bufw[10] = { 0 };
   // memset(bufw, 0x80, 10);
   // Fm_WriteMemory(PID, Mudlue, bufw, sizeof(bufw));

  //  ULONG64 FakePid =  GetProcessID("winlogon.exe");

     // Fm_FakeProcess(PID, FakePid);
    BOOLEAN ret;

    ret =   DriverLoad();

    if (ret == TRUE)
    {
        printf("驱动加载成功！\r\n");
    }

    HWND hwnd = NULL;
    DWORD PID;

    do
    {
        hwnd = FindWindowA("UnrealWindow", NULL);//PUBG：绝地求生 

    } while (!hwnd);

    GetWindowThreadProcessId(hwnd, &PID);

    ret =  Fm_Inject(PID, sysData, sizeof(sysData));  //注入测试

    if (ret == TRUE)
    {
        printf("游戏注入成功！\r\n");
    }

    system("pause");
}
