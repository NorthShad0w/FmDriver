// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
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

int main(int argc,char * argv[]) 
{
    ULONG PID = 59480;

    ULONG64 Mudlue =  Fm_GetModule(PID, "MfcRwProcss.exe");

    printf("Mudlue -> %llx \r\n", Mudlue);

    unsigned char buffer[10] = { 0 };
    printf("{ ");
   // ULONG64 buffer = 0;
    
    Fm_ReadMemory(PID, Mudlue, buffer, sizeof(buffer));

    for (int i = 0; i < sizeof(buffer); i++)
    {
        printf("0x%02X", buffer[i]);
    }
    printf("} \r\n");
    char bufw[10] = { 0 };
    memset(bufw, 0x80, 10);
    Fm_WriteMemory(PID, Mudlue, bufw, sizeof(bufw));


    Fm_Inject(PID, sysData, sizeof(sysData));

    system("pause");
}
