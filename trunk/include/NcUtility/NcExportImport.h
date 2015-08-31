#ifdef NCUTILITY_LIB
#define NCUTILITYEXPORT __declspec(dllexport)
#else
#define NCUTILITYEXPORT __declspec(dllimport)
#endif