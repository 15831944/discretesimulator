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
	/*virtual				~NcStlImport();*/
	STATUS				stl_read_triangulation (triangulation *);
	
	
	
	GLuint				getDLIdForTurningTool() {return tool_list;}
	GLuint				getDLIdForDrillingTool() {return drill_tool_list;}
	GLuint				getDLIdForPartingTool() {return parting_tool_list;}
	GLuint				getDLIdForThreadingTool() {return threading_tool_list;}
	
	int					importLatheTools();

private:
	STATUS				stl_main(char *filename,triangulation* patch);
	
	NcParser*			parser;
	
	FILE*				source;

	GLuint			n_list;
	GLuint			n_list_count;
	GLuint			tool_list;
	GLuint			drill_tool_list;
	GLuint			parting_tool_list;
	GLuint			threading_tool_list;
	GLuint			lathe_list;
	GLuint			chuck_list;
	
	};
	
	
	
}


#endif