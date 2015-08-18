#ifdef NCGEOMKERNEL_LIB
#define NCGEOMKERNELEXPORT __declspec(dllexport)
#else
#define NCGEOMKERNELEXPORT __declspec(dllimport)
#endif


