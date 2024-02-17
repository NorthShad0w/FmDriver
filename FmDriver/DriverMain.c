#include<ntifs.h>
#include"Comm.h"
#include"Function.h"
#include "FakeInject.h"
#include "FakeProcess.h"

NTSTATUS NTAPI  DispatchCallBack(PVOID CommInfo)
{
	PCommInfo Info = (PCommInfo)CommInfo;

	//DbgBreakPoint();
	PVOID Date = Info->InData;

	NTSTATUS status = STATUS_UNSUCCESSFUL;

	switch (Info->Cmd)
	{

	case CMD_INIT: {//测试通讯

		status = STATUS_SUCCESS;
		break;
	}
		
	case CMD_QUERYMEMORY: {
		PQueryMemoryInfo Info = (PQueryMemoryInfo)Date;

		status=  Fm_QueryMemory(Info->PID, Info->BaseAddress, (PFMMEMORY_BASIC_INFORMATION)Info->RetBase);
	}
		break;

	case CMD_GETMODULE: {
		PGetModuleInfo Info = (PGetModuleInfo)Date;
		
		Info->RetBase = Fm_GetModeBase(Info->PID, Info->ModuleName);
		
		if (Info->RetBase)
		{
			status = STATUS_SUCCESS;
		}

	}
		break;

	case CMD_READMEMORY: {

		PReadWriteMemoryInfo Info = (PReadWriteMemoryInfo)Date;

		status = Fm_ReadMemory(Info->PID, Info->Base, Info->Buffer, Info->Size);

	}
		break;

	case CMD_WIRTEMEMORY: {

		PReadWriteMemoryInfo Info = (PReadWriteMemoryInfo)Date;

		status = Fm_WriteMemory(Info->PID, Info->Base, Info->Buffer, Info->Size);
	}
		break;
	case CMD_INJECT: {
		PInjectInfo Info = (PInjectInfo)Date;

		status = Fm_Inject(Info->PID, Info->DllBuffer, Info->size);

	}
		break;

	case CMD_FAKEPROCESS: {
		PFakeProcessInfo Info = (PFakeProcessInfo)Date;
		
		status = Fm_FakeProcess(Info->MyPID, Info->FakePID);

	}
		break;

	default:
		break;
	}

	return status;
}

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UnRegisterComm;

	//DbgPrint("驱动卸载成功！\r\n");
}


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg_Path)
{

	if (RegisterComm(DispatchCallBack))
	{
		//DbgPrintEx(77, 0, "通讯成功 \r\n");
	}

	pDriver->DriverUnload = DriverUnload;

	return STATUS_SUCCESS;
}



