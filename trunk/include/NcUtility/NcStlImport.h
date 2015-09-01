#ifndef NC_STLIMPORT
#define NC_STLIMPORT

#include "NcUtility\NcExportImport.h"
#include "NcUtility\NcGlobalDefinitions.h"

/* Program for triangulation */
#include	<iostream>
#include	<stdlib.h>
#include	<assert.h>

//#include<GL/gl.h>
#include<QtOpenGl>
using namespace std;


namespace DiscreteSimulator
{	
	class NcParser;
	
	class NCUTILITYEXPORT NcStlImport
	{
		public:
			NcStlImport();
			~NcStlImport();
			STATUS		read(char *filename,triangulation* patch);

		private:
			STATUS		stl_read_triangulation (triangulation *);
			
	
			NcParser*			parser;

	};
	
	
	
}


#endif
