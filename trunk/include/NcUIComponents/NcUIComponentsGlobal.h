#ifdef NCUICOMPONENTS_LIB
#define NCUICOMPONENTSEXPORT __declspec(dllexport)
#else
#define NCUICOMPONENTSEXPORT __declspec(dllimport)
#endif
