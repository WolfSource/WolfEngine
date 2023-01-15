#define W_SYSTEM_EXPORT

#if defined(WIN32) || defined(WIN64)
#define W_SYSTEM_EXPORT __declspec(dllexport)
#endif
