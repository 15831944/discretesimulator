#include "NcUtility\NcVector.h"
#include "NcUtility\NcParser.h"
#include "NcDisplay\NcDisplay.h"
#include "NcUtility\NcStlImport.h"
#include <string>
#include <QDir>
using namespace DiscreteSimulator;



NcStlImport::NcStlImport()
{
	parser = new NcParser();	
}

NcStlImport::~NcStlImport()
{
	if(parser != nullptr)
		delete parser;
}

STATUS NcStlImport::read(char *filename,triangulation* patch)
{
	STATUS retcode = FAIL;
	
	if(parser->init_scan(filename)==0)
	{
		printf("Cannot Open File\n");
		retcode=FAIL;
		return retcode;
	}
	
	retcode = stl_read_triangulation(patch);
	parser->shutdown_scan();
	return retcode;
}

STATUS NcStlImport::stl_read_triangulation (triangulation *patch)
{
	int i=0;
	Symbol token;
	char buffer[80];
	FILE* source = parser->getFile();
	patch->num_triangle=0;	
	token = parser->getsym();
	while(token!=eof){
		switch(token){	
			case solidsym :
				fscanf(source, "%s", buffer);
				printf("Got solid name %s\n", buffer);
				break;
			case facetsym: 
				i=0;
				break;
			case normalsym:
				fscanf(source,"%lf %lf %lf",
					&patch->t[patch->num_triangle].normal.x(),
					&patch->t[patch->num_triangle].normal.y(),
					&patch->t[patch->num_triangle].normal.z());
			case outersym: 
				break;
			case loopsym: 
				break;
			case vertexsym:
				fscanf(source,"%lf %lf %lf",
					&patch->t[patch->num_triangle].v[i].x(),
					&patch->t[patch->num_triangle].v[i].y(),
					&patch->t[patch->num_triangle].v[i].z());
				i++;
			case endloopsym: 
				break;
			case endfacetsym: 
				patch->num_triangle = patch->num_triangle + 1;
				assert((patch->num_triangle-1) < MAX_STL_TRI);
				break;
			case endsolidsym: 
				return OK;
			case eol:
				break;
			default:
				printf("Unknown token %d\n",token);
				break;
		}
		token = parser->getsym();
	}
	return OK;
}



