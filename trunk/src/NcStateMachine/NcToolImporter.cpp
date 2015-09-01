#include <QString>
#include "NcStateMachine\NcToolImporter.h"
#include "NcUtility\NcStlImport.h"


using namespace DiscreteSimulator;

STATUS NcToolImporter::stl_display_triangulation (triangulation *patch)
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


int NcToolImporter::importLatheTools()
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
	NcStlImport stlImport;

	retcode = stlImport.read(const_cast<char *>(path.toStdString().c_str()),patch);
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

	retcode = stlImport.read(const_cast<char *>(path.toStdString().c_str()),patch);
	
	if(retcode == FAIL)
		qDebug()<<QObject::tr("parting_tool.stl failed to read\n")<<endl;

	glNewList(parting_tool_list, GL_COMPILE);
		glPushMatrix();
		glRotated(0,-90,1,0);
		//glRotated(90,0,1,0);
		//glRotated(90,0,0,1);
		//glScaled(0.5,0.5,0.5);
		stl_display_triangulation(patch);	
		glPopMatrix();
	glEndList();
	delete patch;patch=0;
	//---------------------------------------
	// New Drill tool
	patch = new triangulation();
	path.clear();
	path = abspath + QString("/") + QString("new_drill.stl");

	retcode = stlImport.read(const_cast<char *>(path.toStdString().c_str()),patch);
	
	if(retcode==FAIL)
		qDebug()<<QObject::tr("new_drill.stl failed to read\n")<<endl;

	glNewList(drill_tool_list,GL_COMPILE);
		//glPushMatrix();
			//glTranslatef(68,0,2);
			//glScalef(11.0,11.0,11.0);
			stl_display_triangulation(patch);
		//glPopMatrix();
	glEndList();
	delete patch;patch=0;
	//----------------------------------
	// New Threading Tool
	patch = new triangulation();
	path.clear();
	path = abspath + QString("/") + QString("new_threading.stl");

	retcode = stlImport.read(const_cast<char *>(path.toStdString().c_str()),patch);

	if(retcode==FAIL)
		qDebug()<<QObject::tr("new_threading.stl failed to read\n")<<endl;

	glNewList(threading_tool_list, GL_COMPILE);
		stl_display_triangulation(patch);
	glEndList();
	delete patch;patch=0;
	return 1;
}