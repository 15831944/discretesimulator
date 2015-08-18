#ifdef NCDISPLAY_LIB
#define NCDISPLAYEXPORT __declspec(dllexport)
#else
#define NCDISPLAYEXPORT __declspec(dllimport)
#endif