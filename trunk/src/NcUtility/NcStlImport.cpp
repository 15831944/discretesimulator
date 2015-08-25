#include "NcUtility\NcVector.h"
#include "NcUtility\NcParser.h"
#include "NcUtility\NcStlImport.h"
#include <string>
#include <QDir>
using namespace DiscreteSimulator;



NcStlImport::NcStlImport()
{
	parser = new NcParser();
}

//NcStlImport::~NcStlImport()
//{
//}

STATUS NcStlImport::stl_main(char *filename,triangulation* patch)
{
	STATUS retcode = FAIL;
	
	if(parser->init_scan(filename)==0)
	{
		printf("Cannot Open File\n");
		retcode=FAIL;
		return retcode;
	}
	source = parser->getFile();
	retcode = stl_read_triangulation(patch);
	parser->shutdown_scan();
	return retcode;
}

STATUS NcStlImport::stl_read_triangulation (triangulation *patch)
{
	int i=0;
	Symbol token;
	char buffer[80];
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
					&patch->t[patch->num_triangle].normal.vx,
					&patch->t[patch->num_triangle].normal.vy,
					&patch->t[patch->num_triangle].normal.vz);
			case outersym: 
				break;
			case loopsym: 
				break;
			case vertexsym:
				fscanf(source,"%lf %lf %lf",
					&patch->t[patch->num_triangle].v[i].vx,
					&patch->t[patch->num_triangle].v[i].vy,
					&patch->t[patch->num_triangle].v[i].vz);
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

STATUS NcStlImport::stl_display_triangulation (triangulation *patch)
{
	int i,j;
	glBegin(GL_TRIANGLES);
		for(i = 0; i < patch->num_triangle; i++)
		{	
			glNormal3f(patch->t[i].normal.x(), patch->t[i].normal.y(), patch->t[i].normal.z());
			for(j = 0; j < 3; j++)
			{
				glVertex3f(patch->t[i].v[j].x(), patch->t[i].v[j].y(), patch->t[i].v[j].z());
			}
		}
	glEnd();
	return OK;
}

int NcStlImport::importLatheTools()
{
	
	STATUS retcode;
	n_list_count	= 4;
	n_list = glGenLists(n_list_count);
	tool_list	= n_list;
	drill_tool_list	= n_list + 1;
	parting_tool_list = n_list + 2;
	threading_tool_list	= n_list + 3;
	
	//----------------
	QDir dir("..\\resources\\tools");
	QString abspath = dir.absolutePath();
	QString path = abspath + QString("/") + QString("turning_holder_lh.stl");
	
	triangulation* patch = new triangulation();
	retcode = stl_main(const_cast<char *>(path.toStdString().c_str()),patch);

	if(retcode == FAIL)
		qDebug()<<QObject::tr("turning_holder_lh.stl failed to read\n")<<endl;

	glNewList(tool_list, GL_COMPILE);
		stl_display_triangulation(patch);	
	glEndList();

	delete patch;patch=0;
	//-------------------------
	patch = new triangulation();
	path.clear();
	path = abspath + QString("/") + QString("parting_tool.stl");

	retcode = stl_main(const_cast<char *>(path.toStdString().c_str()),patch);
	
	if(retcode == FAIL)
		qDebug()<<QObject::tr("parting_tool.stl failed to read\n")<<endl;

	glNewList(parting_tool_list, GL_COMPILE);
		glPushMatrix();
		glRotatef(0,-90,1,0);
		glPopMatrix();
		stl_display_triangulation(patch);	
	glEndList();
	delete patch;patch=0;
	//---------------------------------------
	// New Drill tool
	patch = new triangulation();
	path.clear();
	path = abspath + QString("/") + QString("new_drill.stl");

	retcode = stl_main(const_cast<char *>(path.toStdString().c_str()),patch);
	
	if(retcode==FAIL)
		qDebug()<<QObject::tr("new_drill.stl failed to read\n")<<endl;

	glNewList(drill_tool_list,GL_COMPILE);
		glPushMatrix();
		glTranslatef(68,0,2);
		glScalef(11.0,11.0,11.0);
		stl_display_triangulation(patch);
		glPopMatrix();
	glEndList();
	delete patch;patch=0;
	//----------------------------------
	// New Threading Tool
	patch = new triangulation();
	path.clear();
	path = abspath + QString("/") + QString("new_threading.stl");

	retcode = stl_main(const_cast<char *>(path.toStdString().c_str()),patch);

	if(retcode==FAIL)
		qDebug()<<QObject::tr("new_threading.stl failed to read\n")<<endl;

	glNewList(threading_tool_list, GL_COMPILE);
		glPushMatrix();
		glTranslatef(60,2,0);
		glRotatef(-90,0,0,1);
		glScalef(11.0,11.0,11.0);
		stl_display_triangulation(patch);
		glPopMatrix();
	glEndList();
	delete patch;patch=0;
	return 1;
}
