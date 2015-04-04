#include "Wolf.System.Shared.PCH.h"

//Because all files of this project are type of headers I created it
class SystemFake
{
public:
	__declspec(dllexport) void nop(){};
};

BOOL APIENTRY DllMain(_In_ HMODULE hModule, _In_ DWORD ul_reason_for_call, _In_ LPVOID lpReserved)
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
