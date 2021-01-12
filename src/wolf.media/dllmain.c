#include <wolf.h>

BOOL APIENTRY DllMain(_In_ HMODULE phModule, _In_ DWORD phl_reason_for_call, _In_ LPVOID plpReserved)
{
    switch (phl_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}