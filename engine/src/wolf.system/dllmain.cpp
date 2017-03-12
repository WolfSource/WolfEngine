#include "w_system_pch.h"

#if defined(__WIN32) || defined (__UWP)

BOOL APIENTRY DllMain(_In_ HMODULE pHModule, _In_ DWORD pULReasonForCall, _In_ LPVOID pLPReserved)
{
	switch (pULReasonForCall)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif
