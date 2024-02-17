#include "Function.h"
#include "Tools.h"

RTL_OSVERSIONINFOEXW OsVeSionNinfo = { 0 };


PPEB32 PsGetWow64Peb32(PEPROCESS Process) {
	PPEB32 peb32 = NULL;
	if (OsVeSionNinfo.dwBuildNumber == 7601 || OsVeSionNinfo.dwBuildNumber == 7600)
	{
		peb32 = (PPEB32) * ((PULONG64)((PUCHAR)Process + 0x320));
	}
	else if (OsVeSionNinfo.dwBuildNumber == 22000) {
		ULONG64 tmp = *((((PULONG64)((PUCHAR)Process + 0x580))));
		if (tmp)
		{
			peb32 = (PPEB32) * (PULONG64)(tmp);
		}
	}
	else
	{
		ULONG64 tmp = *((((PULONG64)((PUCHAR)Process + 0x428))));
		if (tmp)
		{
			peb32 = (PPEB32) * (PULONG64)(tmp);
		}
	}
	return peb32;
}

/*从PEB链表中获取指定Wow64进程基址*/
ULONG_PTR Getx86ModuleBase(PPEB32 peb, char* moduleName) {
	if (!peb)
	{
		return NULL;
	}
	PPEB_LDR_DATA32 ldrData = (PPEB_LDR_DATA32)peb->Ldr;
	PLDR_DATA_TABLE_ENTRY32 begin = (PLDR_DATA_TABLE_ENTRY32)ldrData->InLoadOrderModuleList.Flink;
	PLDR_DATA_TABLE_ENTRY32 next = (PLDR_DATA_TABLE_ENTRY32)begin;
	do
	{
		ANSI_STRING name = { 0 };
		UNICODE_STRING nameUni = { 0 };
		RtlInitAnsiString(&name, moduleName);
		RtlAnsiStringToUnicodeString(&nameUni, &name, TRUE);

		if (next->DllBase && wcscmp(nameUni.Buffer, next->BaseDllName.Buffer) == 0)
		{
			RtlFreeAnsiString(&nameUni);
			return next->DllBase;
		}
		next = (PLDR_DATA_TABLE_ENTRY32)next->InLoadOrderLinks.Flink;
		RtlFreeAnsiString(&nameUni);
	} while (begin != next && next);

}

/*从PEB链表中获取指定X64进程基址*/
ULONG_PTR Getx64ModuleBase(PPEB peb64, char* moduleName) {
	if (!peb64)
	{
		return NULL;
	}
	PPEB_LDR_DATA64 ldrData = (PPEB_LDR_DATA64)(*(PULONG64)((ULONG64)peb64 + 0x18));
	PLDR_DATA_TABLE_ENTRY64 begin = (PLDR_DATA_TABLE_ENTRY64)ldrData->InLoadOrderModuleList.Flink;
	PLDR_DATA_TABLE_ENTRY64 next = (PLDR_DATA_TABLE_ENTRY64)begin;
	do
	{
		ANSI_STRING name = { 0 };
		UNICODE_STRING nameUni = { 0 };

		RtlInitAnsiString(&name, moduleName);
		RtlAnsiStringToUnicodeString(&nameUni, &name, TRUE);
		if (next->DllBase && RtlCompareUnicodeString(&nameUni, &(next->BaseDllName), TRUE) == 0)
		{
			RtlFreeAnsiString(&nameUni);
			return next->DllBase;
		}
		next = (PLDR_DATA_TABLE_ENTRY64)next->InLoadOrderLinks.Flink;
		RtlFreeAnsiString(&nameUni);
	} while (begin != next && next);
	return NULL;
}

//查询内存

NTSTATUS Fm_QueryMemory(ULONG64 Pid, ULONG64 VirtualAddress, PFMMEMORY_BASIC_INFORMATION InfoMode)
{
	PEPROCESS Process = NULL;

	FMMEMORY_BASIC_INFORMATION InfoTemp = { 0 };

	NTSTATUS Status = PsLookupProcessByProcessId(Pid, &Process);

	if (!NT_SUCCESS(Status))
	{
		return Status;
	}
	if (PsGetProcessExitStatus(Process) != STATUS_PENDING)
	{
		ObDereferenceObject(Process);

		return STATUS_UNSUCCESSFUL;
	}
	KAPC_STATE KApc = { 0 };

	KeStackAttachProcess(Process, &KApc);

	MODE Mode = SetPreviousMode(KeGetCurrentThread(), KernelMode);


	do 
	{
		MEMORY_BASIC_INFORMATION m_Info = { 0 };

		SIZE_T retSize = 0;

	
		NtQueryVirtualMemoryProc pNtQueryVirtualMemory = (NtQueryVirtualMemoryProc)NtQueryVirtualMemoryAddr();

		//DbgPrintEx(0, 77, "pNtQueryVirtualMemory->%p \r\n", pNtQueryVirtualMemory);

	
		Status = pNtQueryVirtualMemory(NtCurrentProcess(), VirtualAddress, MemoryBasicInformation, &m_Info, sizeof(m_Info),&retSize);

		if (!NT_SUCCESS(Status))
		{
			break;
		}

		InfoTemp.AllocationBase = m_Info.AllocationBase;
		InfoTemp.AllocationProtect = m_Info.AllocationProtect;
		InfoTemp.BaseAddress = m_Info.BaseAddress;
		InfoTemp.PartitionId = m_Info.PartitionId;
		InfoTemp.Protect = m_Info.Protect;
		InfoTemp.RegionSize = m_Info.RegionSize;
		InfoTemp.State = m_Info.State;
		InfoTemp.Type = m_Info.Type;



	} while (0);

	SetPreviousMode(KeGetCurrentThread(), Mode);

	KeUnstackDetachProcess(&KApc);

	DbgBreakPoint();
	if (MmIsAddressValid(InfoMode))
	{
		memcpy(InfoMode, &InfoTemp, sizeof(FMMEMORY_BASIC_INFORMATION));
	}
	
	return InfoTemp.RegionSize != 0 ? STATUS_SUCCESS: STATUS_UNSUCCESSFUL;

}

//获取Exe
ULONG64 Fm_GetExeModule(ULONG64 Pid)
{
	PEPROCESS Process = NULL;

	FMMEMORY_BASIC_INFORMATION InfoTemp = { 0 };

	NTSTATUS Status = PsLookupProcessByProcessId(Pid, &Process);

	if (!NT_SUCCESS(Status))
	{
		return Status;
	}
	ObDereferenceObject(Process);

	return PsGetProcessSectionBaseAddress(Process);
}


//获取模块
ULONG64 Fm_GetModeBase(ULONG64 Pid, char* ModeName)
{
	if (ModeName == NULL)
	{
		return NULL;
	}
	PEPROCESS Process = NULL;

	NTSTATUS Status = PsLookupProcessByProcessId(Pid, &Process);

	KAPC_STATE apc = { 0 };
	
	char* name = (char*)ExAllocatePool(NonPagedPool, 0x200);
	
	strcpy(name, ModeName);

	KeStackAttachProcess(Process, &apc);

	ULONG_PTR base = NULL;
	
	PPEB32 peb32 = PsGetWow64Peb32(Process);

	if (peb32)
	{
		//x86
		base = Getx86ModuleBase(peb32, name);
	}
	else {
		//X64
		PPEB peb64 = (PPEB)PsGetProcessPeb(Process);

		base = Getx64ModuleBase(peb64, name);
	}
	KeUnstackDetachProcess(&apc);
	
	ObDereferenceObject(Process);
	
	return base;
}
PEPROCESS FindProcess(char* Name)
{
	PEPROCESS RetProcess = NULL;

	for (int i = 8; i<100000; i+=4)
	{
		PEPROCESS Process = NULL;

		NTSTATUS Status =  PsLookupProcessByProcessId(i, &Process);

		if (!NT_SUCCESS(Status)) continue;
		
		if (PsGetProcessExitStatus(Process) != STATUS_PENDING)
		{
			ObDereferenceObject(Process);

			continue;
		}

		PUCHAR ProcessName =  PsGetProcessImageFileName(Process);

		if (_stricmp(ProcessName, Name) == 0)
		{
			ObDereferenceObject(Process);
			
			continue;
				
		}

		RetProcess = Process;
		break;
	}

	return RetProcess;
}


NTSTATUS Fm_ReadMemory(ULONG64 Pid, ULONG64 Address, ULONG64 Buffer, ULONG64 Size)
{
	if (Address == 0)return STATUS_UNSUCCESSFUL;

	if (Address >= MM_HIGHEST_USER_ADDRESS)return STATUS_UNSUCCESSFUL;

	if(Address+Size >= MM_HIGHEST_USER_ADDRESS)return STATUS_UNSUCCESSFUL;

	static PEPROCESS WinlogonProcess = NULL;

	PEPROCESS Process = NULL;

	NTSTATUS Status = PsLookupProcessByProcessId(Pid, &Process);

	if (!NT_SUCCESS(Status))
	{
		return Status;
	}
	if (PsGetProcessExitStatus(Process) != STATUS_PENDING)
	{
		ObDereferenceObject(Process);

		return STATUS_UNSUCCESSFUL;
	}

	if (!WinlogonProcess)
	{
		WinlogonProcess = FindProcess("winlogon.exe");
	}

	if (!WinlogonProcess)
	{
		ObDereferenceObject(Process);

		return STATUS_UNSUCCESSFUL;
	}
	if (PsGetProcessExitStatus(WinlogonProcess) != STATUS_PENDING)
	{
		ObDereferenceObject(Process);

		return STATUS_UNSUCCESSFUL;
	}

	PVOID Object = ExAllocatePool(NonPagedPool, PAGE_SIZE);

	memcpy(Object, (PUCHAR)WinlogonProcess - 0x30, PAGE_SIZE);

	PEPROCESS FakeProcess = (PEPROCESS)((PUCHAR)Object + 0x30);

	ULONG64 GameCr3 = *(PLONG64)((ULONG64)Process + 0x28);

	*(PLONG64)((ULONG64)FakeProcess + 0x28) = GameCr3;
	
	SIZE_T RetSize = 0;

	Status = MmCopyVirtualMemory(FakeProcess, Address, IoGetCurrentProcess(), Buffer, Size, UserMode, &RetSize);

	ObDereferenceObject(Process);

	ExFreePool(Object);

	return Status;
}


NTSTATUS Fm_WriteMemory(ULONG64 Pid, ULONG64 Address, ULONG64 Buffer, ULONG64 Size)
{
	if (Address == 0)return STATUS_UNSUCCESSFUL;

	if (Address >= MM_HIGHEST_USER_ADDRESS)return STATUS_UNSUCCESSFUL;

	if (Address + Size >= MM_HIGHEST_USER_ADDRESS)return STATUS_UNSUCCESSFUL;

	static PEPROCESS WinlogonProcess = NULL;

	PEPROCESS Process = NULL;

	NTSTATUS ntPstatus;

	NTSTATUS Status = PsLookupProcessByProcessId(Pid, &Process);

	if (!NT_SUCCESS(Status))
	{
		return Status;
	}
	if (PsGetProcessExitStatus(Process) != STATUS_PENDING)
	{
		ObDereferenceObject(Process);

		return STATUS_UNSUCCESSFUL;
	}

	if (!WinlogonProcess)
	{
		WinlogonProcess = FindProcess("winlogon.exe");
	}

	if (!WinlogonProcess)
	{
		ObDereferenceObject(Process);

		return STATUS_UNSUCCESSFUL;
	}
	if (PsGetProcessExitStatus(WinlogonProcess) != STATUS_PENDING)
	{
		ObDereferenceObject(Process);

		return STATUS_UNSUCCESSFUL;
	}

	PVOID Object = ExAllocatePool(NonPagedPool, PAGE_SIZE);

	memcpy(Object, (PUCHAR)WinlogonProcess - 0x30, PAGE_SIZE);

	PEPROCESS FakeProcess = (PEPROCESS)((PUCHAR)Object + 0x30);

	ULONG64 GameCr3 = *(PLONG64)((ULONG64)Process + 0x28);

	*(PLONG64)((ULONG64)FakeProcess + 0x28) = GameCr3;

	SIZE_T RetSize = 0;

	Status = MmCopyVirtualMemory(IoGetCurrentProcess(), Buffer, FakeProcess, Address, Size, UserMode, &RetSize);

	if (!NT_SUCCESS(Status))
	{
		KAPC_STATE Kapc = { 0 };

		KeStackAttachProcess(FakeProcess, &Kapc);
		//修改内存属性
		NtProtectVirtualMemoryProc pNtProtectVirtualMemory = (NtProtectVirtualMemoryProc)NtProtectVirtualMemoryAddr();

	//	DbgPrintEx(0, 77, "pNtProtectVirtualMemory -> %p \r\n", pNtProtectVirtualMemory);

		MODE Mode = SetPreviousMode(KeGetCurrentThread(), KernelMode);

		PVOID TempBase = Address;

		SIZE_T TempSize = Size;
		
		ULONG OldProtect = 0;

		Status =  pNtProtectVirtualMemory(NtCurrentProcess(), &TempBase, &Size,PAGE_EXECUTE_READWRITE, &OldProtect);

		ntPstatus = Status;

		SetPreviousMode(KeGetCurrentThread(), Mode);

		KeUnstackDetachProcess(&Kapc);

		if (NT_SUCCESS(Status))
		{
			 MmCopyVirtualMemory(IoGetCurrentProcess(), Buffer, FakeProcess, Address, Size, UserMode, &RetSize);
		}
		if (ntPstatus)
		{
			KeStackAttachProcess(FakeProcess, &Kapc);

			pNtProtectVirtualMemory(NtCurrentProcess(), &TempBase, &Size, OldProtect, &OldProtect);

			KeUnstackDetachProcess(&Kapc);
		}

	}

	if (!NT_SUCCESS(Status))
	{
		KAPC_STATE Kapc = { 0 };

		PUCHAR kBuffer = ExAllocatePool(NonPagedPool, Size);

		memcpy(kBuffer, Buffer, Size);

		KeStackAttachProcess(FakeProcess,&Kapc);

		do 
		{
			PMDL Mdl = IoAllocateMdl(Address, Size, NULL, NULL, NULL);

			if (!Mdl)
			{
				break;
			}
			MmBuildMdlForNonPagedPool(Mdl);

			PVOID pBaseAddress = MmMapLockedPagesSpecifyCache(Mdl, KernelMode, MmCached, NULL, NULL, NormalPagePriority);

			if (!pBaseAddress)
			{

				IoFreeMdl(Mdl);
				break;
			}

			memcpy(pBaseAddress, kBuffer, Size);

			MmUnmapLockedPages(pBaseAddress, Mdl);
			IoFreeMdl(Mdl);

			Status = STATUS_SUCCESS;

		} while (0);

		KeUnstackDetachProcess(&Process);

	}

	ObDereferenceObject(Process);

	ExFreePool(Object);

	return Status;
}
