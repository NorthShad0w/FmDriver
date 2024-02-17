#include<Windows.h>

EXTERN_C BOOLEAN WINAPI DriverLoad(); //可以加验证

EXTERN_C ULONG64 WINAPI Fm_GetModule(ULONG64 Pid, char* ModuleName);

EXTERN_C BOOLEAN WINAPI Fm_ReadMemory(ULONG64 Pid, ULONG64 Address, PVOID Buffer, ULONG64 Size);

EXTERN_C BOOLEAN WINAPI Fm_WriteMemory(ULONG64 Pid, ULONG64 Address, PVOID Buffer, ULONG64 Size);

EXTERN_C BOOLEAN WINAPI Fm_Inject(ULONG64 pid, unsigned char* DllData, SIZE_T dwImageSize);

EXTERN_C BOOLEAN WINAPI Fm_FakeProcess(ULONG64 MyPid, ULONG64 fakepid);