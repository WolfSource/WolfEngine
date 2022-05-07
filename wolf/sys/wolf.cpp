#include "wolf.h"

// // making incompatible API changes
// constexpr auto WOLF_MAJOR_VERSION = 3;
// // adding functionality in a backwards - compatible manner
// constexpr auto WOLF_MINOR_VERSION = 1;
// // bug fixes
// constexpr auto WOLF_PATCH_VERSION = 2;
// // for debugging
// constexpr auto WOLF_DEBUG_VERSION = 0;

#ifdef _WIN32

// BOOL APIENTRY DllMain(_In_ HMODULE pHModule, _In_ DWORD pULReasonForCall, _In_ LPVOID pLPReserved)
// {
//     switch (pULReasonForCall)
//     {
//     case DLL_PROCESS_ATTACH:
//     case DLL_THREAD_ATTACH:
//     case DLL_THREAD_DETACH:
//     case DLL_PROCESS_DETACH:
//         break;
//     }
//     return TRUE;
// }

#endif
