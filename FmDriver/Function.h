#include <ntifs.h>
#include"Struct.h"

typedef struct _PEB32
{
	UCHAR InheritedAddressSpace;                                            //0x0
	UCHAR ReadImageFileExecOptions;                                         //0x1
	UCHAR BeingDebugged;                                                    //0x2
	union
	{
		UCHAR BitField;                                                     //0x3
		struct
		{
			UCHAR ImageUsesLargePages : 1;                                    //0x3
			UCHAR IsProtectedProcess : 1;                                     //0x3
			UCHAR IsLegacyProcess : 1;                                        //0x3
			UCHAR IsImageDynamicallyRelocated : 1;                            //0x3
			UCHAR SkipPatchingUser32Forwarders : 1;                           //0x3
			UCHAR SpareBits : 3;                                              //0x3
		};
	};
	ULONG Mutant;                                                           //0x4
	ULONG ImageBaseAddress;                                                 //0x8
	ULONG Ldr;                                                              //0xc

}PEB32, * PPEB32;

typedef struct _PEB_LDR_DATA32
{
	ULONG Length;                                                           //0x0
	UCHAR Initialized;                                                      //0x4
	ULONG SsHandle;                                                         //0x8
	LIST_ENTRY32 InLoadOrderModuleList;                               //0xc
	LIST_ENTRY32 InMemoryOrderModuleList;                             //0x14
	LIST_ENTRY32 InInitializationOrderModuleList;                     //0x1c
	ULONG Entr32yInProgress;                                                  //0x24
	UCHAR ShutdownInProgress;                                               //0x28
	ULONG ShutdownThreadId;                                                 //0x2c
}PEB_LDR_DATA32, * PPEB_LDR_DATA32;

typedef struct _LDR_DATA_TABLE_ENTRY32
{
	LIST_ENTRY32 InLoadOrderLinks;                                    //0x0
	LIST_ENTRY32 InMemoryOrderLinks;                                  //0x8
	LIST_ENTRY32 InInitializationOrderLinks;                          //0x10
	ULONG DllBase;                                                          //0x18
	ULONG EntryPoint;                                                       //0x1c
	ULONG SizeOfImage;                                                      //0x20
	UNICODE_STRING32 FullDllName;                                     //0x24
	UNICODE_STRING32 BaseDllName;                                     //0x2c
}LDR_DATA_TABLE_ENTRY32, * PLDR_DATA_TABLE_ENTRY32;

typedef struct _PEB_LDR_DATA64
{
	ULONG Length;                                                           //0x0
	UCHAR Initialized;                                                      //0x4
	VOID* SsHandle;                                                         //0x8
	struct _LIST_ENTRY InLoadOrderModuleList;                               //0x10
	struct _LIST_ENTRY InMemoryOrderModuleList;                             //0x20
	struct _LIST_ENTRY InInitializationOrderModuleList;                     //0x30
	VOID* EntryInProgress;                                                  //0x40
	UCHAR ShutdownInProgress;                                               //0x48
	VOID* ShutdownThreadId;                                                 //0x50
}PEB_LDR_DATA64, * PPEB_LDR_DATA64;

typedef struct _LDR_DATA_TABLE_ENTRY64
{
	struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0
	struct _LIST_ENTRY InMemoryOrderLinks;                                  //0x10
	struct _LIST_ENTRY InInitializationOrderLinks;                          //0x20
	VOID* DllBase;                                                          //0x30
	VOID* EntryPoint;                                                       //0x38
	ULONG SizeOfImage;                                                      //0x40
	struct _UNICODE_STRING FullDllName;                                     //0x48
	struct _UNICODE_STRING BaseDllName;                                     //0x58
	ULONG Flags;                                                            //0x68
	USHORT LoadCount;                                                       //0x6c
	USHORT TlsIndex;                                                        //0x6e
	union
	{
		struct _LIST_ENTRY HashLinks;                                       //0x70
		struct
		{
			VOID* SectionPointer;                                           //0x70
			ULONG CheckSum;                                                 //0x78
		};
	};
	union
	{
		ULONG TimeDateStamp;                                                //0x80
		VOID* LoadedImports;                                                //0x80
	};
	struct _ACTIVATION_CONTEXT* EntryPointActivationContext;                //0x88
	VOID* PatchInformation;                                                 //0x90
	struct _LIST_ENTRY ForwarderLinks;                                      //0x98
	struct _LIST_ENTRY ServiceTagLinks;                                     //0xa8
	struct _LIST_ENTRY StaticLinks;                                         //0xb8
	VOID* ContextInformation;                                               //0xc8
	ULONGLONG OriginalBase;                                                 //0xd0
	union _LARGE_INTEGER LoadTime;                                          //0xd8
}LDR_DATA_TABLE_ENTRY64, * PLDR_DATA_TABLE_ENTRY64;


EXTERN_C PVOID PsGetProcessSectionBaseAddress(PEPROCESS Process);

EXTERN_C ULONG_PTR NTAPI PsGetProcessPeb(PEPROCESS proc);

EXTERN_C CHAR* PsGetProcessImageFileName(__in PEPROCESS Process);

EXTERN_C NTSTATUS  MmCopyVirtualMemory(PEPROCESS FromProcess, PVOID FromAddress,
	PEPROCESS ToProcess, PVOID ToAddress, SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode,
	PSIZE_T NumberOfBytesCopied);

typedef NTSTATUS(NTAPI* _ZwCreateThreadEx)(
	OUT PHANDLE ThreadHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN HANDLE ProcessHandle,
	IN PVOID StartRoutine,
	IN PVOID StartContext,
	IN ULONG CreateThreadFlags,
	IN SIZE_T ZeroBits OPTIONAL,
	IN SIZE_T StackSize OPTIONAL,
	IN SIZE_T MaximumStackSize OPTIONAL,
	IN PVOID AttributeList
	);



NTSTATUS Fm_QueryMemory(ULONG64 Pid, ULONG64 VirtualAddress, PFMMEMORY_BASIC_INFORMATION InfoMode);

ULONG64 Fm_GetExeModule(ULONG64 Pid);

ULONG64 Fm_GetModeBase(ULONG64 Pid, char* ModeName);

NTSTATUS Fm_ReadMemory(ULONG64 Pid, ULONG64 Address, ULONG64 Buffer, ULONG64 Size);

NTSTATUS Fm_WriteMemory(ULONG64 Pid, ULONG64 Address, ULONG64 Buffer, ULONG64 Size);
