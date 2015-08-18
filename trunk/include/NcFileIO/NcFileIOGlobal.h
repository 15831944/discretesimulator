#ifdef NCFILEIO_LIB
#define NCEXPORT __declspec(dllexport)
#else
#define NCEXPORT __declspec(dllimport)
#endif


