#ifdef NCSIMULATIONCONTROLLER_LIB
#define NCSIMULATIONCONTROLLEREXPORT __declspec(dllexport)
#else
#define NCSIMULATIONCONTROLLEREXPORT __declspec(dllimport)
#endif
