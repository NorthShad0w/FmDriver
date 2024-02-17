#include"pch.h"
#include"Api.h"
#include"Comms.h"
#include"../FmDriver/Struct.h"
#include"DriverLoad.h"

EXTERN_C BOOLEAN WINAPI DriverLoad()
{
	return LoadDriver();
}

EXTERN_C BOOLEAN WINAPI Fm_QueryMemory(ULONG64 Pid, ULONG64 VirtualAddress, PFMMEMORY_BASIC_INFORMATION InfoMode)
{
	QueryMemoryInfo Info;
	Info.PID = Pid;
	Info.BaseAddress = VirtualAddress;
	Info.RetBase = (ULONG64)InfoMode;
	
	return DriverComm(CMD_QUERYMEMORY, (PVOID)&Info, sizeof(QueryMemoryInfo));

}

EXTERN_C ULONG64 WINAPI Fm_GetModule(ULONG64 Pid, char* ModuleName)
{
	GetModuleInfo Info;

	Info.PID = Pid;
	Info.ModuleName = (ULONG64)ModuleName;

	if (DriverComm(CMD_GETMODULE,(PVOID)&Info,sizeof(GetModuleInfo)))
	{
		return Info.RetBase;
	}
	return 0;
}
EXTERN_C BOOLEAN WINAPI Fm_ReadMemory(ULONG64 Pid, ULONG64 Address, PVOID Buffer, ULONG64 Size)
{
	ReadWriteMemoryInfo Info;
	Info.PID = Pid;
	Info.Base = Address;
	Info.Buffer = (ULONG64)Buffer;
	Info.Size = Size;
	return DriverComm(CMD_READMEMORY, (PVOID)&Info, sizeof(ReadWriteMemoryInfo));
}

EXTERN_C BOOLEAN WINAPI Fm_WriteMemory(ULONG64 Pid, ULONG64 Address, PVOID Buffer, ULONG64 Size)
{
	ReadWriteMemoryInfo Info;
	Info.PID = Pid;
	Info.Base = Address;
	Info.Buffer = (ULONG64)Buffer;
	Info.Size = Size;
	return DriverComm(CMD_WIRTEMEMORY, (PVOID)&Info, sizeof(ReadWriteMemoryInfo));
}


EXTERN_C BOOLEAN WINAPI Fm_Inject(ULONG64 pid, unsigned char* DllData, SIZE_T dwImageSize)
{
	InjectInfo Info;
	Info.PID = pid;
	Info.DllBuffer = DllData;
	Info.size = dwImageSize;
	return DriverComm(CMD_INJECT, (PVOID)&Info, sizeof(InjectInfo));
}

EXTERN_C BOOLEAN WINAPI Fm_FakeProcess(ULONG64 MyPid, ULONG64 fakepid)
{
	FakeProcessInfo Info;
	Info.MyPID = MyPid;
	Info.FakePID = fakepid;
	return DriverComm(CMD_FAKEPROCESS, (PVOID)&Info, sizeof(FakeProcessInfo));
}