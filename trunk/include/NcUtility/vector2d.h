#ifndef VECTOR2D
#define VECTOR2D
#include "NcUtility\NcExportImport.h"

 class NCUTILITYEXPORT vector2d
{
	public:
		double v[2];
		inline double& operator[](const int index);
        inline const double& operator[](const int index) const;

};

#endif