
#include "NcStateMachine\NcMachine.h"
#include "NcGeomKernel\NcProfile.h"
#include "NcGeomKernel\NcRevolve.h"
#include "NcGeomKernel\NcPolygonBoolean.h"
#include "NcUtility\NcVector.h"
#include "NcUtility\NcStlImport.h"
#include "NcStateMachine\NcCode.h"
#include "NcStateMachine\G00.h"
#include "NcStateMachine\G01.h"
#include "NcStateMachine\G02.h"
#include "NcStateMachine\G03.h"
#include "NcStateMachine\G90.h"
#include "NcStateMachine\G74.h"
#include "NcStateMachine\G75.h"
#include "NcStateMachine\NcEndOfCutFeedRateMove.h"
#include "NcStateMachine\NcAuxRapidMove.h"
#include "NcStateMachine\NcCodeBlock.h"
#include <NcDisplay\NcDisplay.h>
#include <iostream>
#include <QMessageBox>

using namespace std;
using namespace DiscreteSimulator;

NcDisplay	*NcDisplay::mNcDisplayInstance = 0;
int NcDisplay::dllistcount = -1;

NcDisplay::NcDisplay() : stock(0)
{
	mSimulationState = PAUSED;

	mLastMachiningOpIndex = 0;

	mTotalPartDisplayLists = 0;  //total number of part display lists to be generated

	if(stock == 0)
	{ 
		stock = new Profile;
	}
	else 
		stock->free_allocate();

	mStockBoundingBox.zmin	= -5.0;
	mStockBoundingBox.xmin	= 0.0001;
	mStockBoundingBox.zmax	= 80.0;
	mStockBoundingBox.xmax	= 60.0;
}

NcDisplay::~NcDisplay()
{
	if(stock != 0)
	{
		delete stock;
		stock = 0;
	}
}

Profile*	NcDisplay::getStockProfile()
{
	return stock;
}

NcDisplay* NcDisplay::getNcDisplayInstance()
{
	if(0 == mNcDisplayInstance)
	{
		mNcDisplayInstance = new NcDisplay();
	}
	return mNcDisplayInstance;
}


void	NcDisplay::CreateDisplayListsForGCodes()
{
	updateStockBoundingBox();
}


bool	NcDisplay::setStockBBFinalValues()
{
	mStockBoundingBox.xmin = mStockBoundingBox.xmin + 0.0001;							// This is to avoid zero area triangle in revolve
	mStockBoundingBox.xmax = (mStockBoundingBox.xmax / 2) + 1;		// This is to avoid final cut of zero length
	mStockBoundingBox.zmax = mStockBoundingBox.zmax - 0.5;		//This is to avoid intersection on edge by tool
	mStockBoundingBox.zmin = mStockBoundingBox.zmin - 10.0;	//This is to avoid intersection on edge by tool

	/*double l = NcStockDimensionDialog::StockDimDialogInstance()->mStockLength;
	double d = NcStockDimensionDialog::StockDimDialogInstance()->mStockDiameter;
	if((fabs(mStockBoundingBox.xmax) > NcStockDimensionDialog::StockDimDialogInstance()->mStockDiameter/2) ||
		(fabs(mStockBoundingBox.zmax) > NcStockDimensionDialog::StockDimDialogInstance()->mStockLength))
		{
			 QMessageBox::StandardButton ret;
				ret = QMessageBox::warning(0, QString("Application"),
												QString("Stock values in the program are larger than values "
														"specified in Workpiece setup. Stock Dimension values from"
														" the part program will be used. Do you wish to continue?"));

				if(ret==QMessageBox::Ok)
				{
					return true;
				}
				else
				{
					return false;
				}
		}*/

	//mStockBoundingBox.xmin = 0.0001;							// This is to avoid zero area triangle in revolve
	//mStockBoundingBox.xmax = 60;		// This is to avoid final cut of zero length
	//mStockBoundingBox.zmax = 0.0;		//This is to avoid intersection on edge by tool
	//mStockBoundingBox.zmin = -80;	//This is to avoid intersection on edge by tool

	return true;
}

static double minXval;
static double maxXval;
static double minZval;
static double maxZval;

static bool firstcode = false;

void	NcDisplay::setStockBBInitialValues()
{
	mStockBoundingBox.xmin = mStockBoundingBox.xmax = NcMachine::NcMachineInstance()->mLastMachineXPos;
	mStockBoundingBox.zmin = mStockBoundingBox.zmax = NcMachine::NcMachineInstance()->mLastMachineZPos;

	minXval = maxXval = 0;
	minZval = maxZval = 0;

	firstcode = true;
	/*mStockBoundingBox.xmin = mStockBoundingBox.xmax = 10;
	mStockBoundingBox.zmin = mStockBoundingBox.zmax = 10;*/
}


void	NcDisplay::updateStockBoundingBox()
{
	minXval = minimum(minXval, NcMachine::NcMachineInstance()->mLastMachineXPos);

	mStockBoundingBox.xmin = minimum(minXval, NcMachine::NcMachineInstance()->mEndOfMotionX);

	if(firstcode == true)
	{
		maxXval = minimum(maxXval, NcMachine::NcMachineInstance()->mLastMachineXPos);
	}
	else
	{
		maxXval = maximum(maxXval, NcMachine::NcMachineInstance()->mLastMachineXPos);
	}
	mStockBoundingBox.xmax = maximum(maxXval, NcMachine::NcMachineInstance()->mEndOfMotionX);
		
	minZval = minimum(minZval, NcMachine::NcMachineInstance()->mLastMachineZPos);
	mStockBoundingBox.zmin = minimum(minZval, NcMachine::NcMachineInstance()->mEndOfMotionZ);

	if(firstcode == true)
	{
		maxZval = minimum(maxZval, NcMachine::NcMachineInstance()->mLastMachineZPos);
	}
	else
	{
		maxZval = maximum(maxZval, NcMachine::NcMachineInstance()->mLastMachineZPos);
	}
	mStockBoundingBox.zmax = maximum(maxZval, NcMachine::NcMachineInstance()->mEndOfMotionZ);

#ifdef debug
	std::cout << mStockBoundingBox.xmin << " " << mStockBoundingBox.xmax <<  " "
			  << mStockBoundingBox.zmin << " " << mStockBoundingBox.zmax << std::endl;
#endif
	firstcode = false;
}



STATUS	NcDisplay::displayStockProfile()
{
	stock->no_pts = 5;
	stock->allocate();
	stock->P[0][0] = mStockBoundingBox.zmin;		
	stock->P[0][1] = mStockBoundingBox.xmin;	
	stock->P[0][2] = 0;
	stock->P[1][0] = mStockBoundingBox.zmax;
	stock->P[1][1] = mStockBoundingBox.xmin;
	stock->P[1][2] = 0;
	stock->P[2][0] = mStockBoundingBox.zmax;
	stock->P[2][1] = mStockBoundingBox.xmax;
	stock->P[2][2] = 0;
	stock->P[3][0] = mStockBoundingBox.zmin;
	stock->P[3][1] = mStockBoundingBox.xmax;
	stock->P[3][2] = 0;
	stock->P[4][0] = mStockBoundingBox.zmin;
	stock->P[4][1] = mStockBoundingBox.xmin;
	stock->P[4][2] = 0;

	createPart();
	
	surfaceDisplay();
	return OK;
}	


STATUS	NcDisplay::createPart()
{
	createSurfaceOfRotation(0);
	normalvector(0);
	return OK;
}

STATUS	NcDisplay::createSurfaceOfRotation(int t)
{
	int i, j, k;
	int no = 0; 
	double dtheta =(double)(2.0 * M_PI / MAX);
	
	for(double rotation_theta = 0; rotation_theta <= (2.0 * M_PI); rotation_theta = rotation_theta + dtheta)
	{
		double R[][3]=
		{
			{1,					0,						0},
			{0, cos(rotation_theta),  sin(rotation_theta)},
			{0, -sin(rotation_theta), cos(rotation_theta)}
		};

		for(j = 0; j < stock->no_pts; j++)
			for(k = 0; k < 3; k++)
				stock->S[no][j][k] = 0;

		for(i = 0; i < stock->no_pts; i++)
			for(j = 0; j < 3; j++)
				for(k = 0; k < 3; k++)
					stock->S[no][i][j] += stock->P[i][k] * R[k][j];
		no++;
	}
	return OK;
}


void	NcDisplay::surfaceDisplay()
{
	//Apply Material
	material_stock();

	int j,k;	
	for(j = 0; j < MAX; j++)
	{
		for(k = 0; k < stock->no_pts - 1; k++)
		{
			glBegin(GL_QUADS);
				glNormal3f(stock->unitnormal[j][k][0], stock->unitnormal[j][k][1], stock->unitnormal[j][k][2]);
				glVertex3f(stock->S[j][k][0], stock->S[j][k][1], stock->S[j][k][2]);
				glVertex3f(stock->S[j+1][k][0], stock->S[j+1][k][1], stock->S[j+1][k][2]);
				glVertex3f(stock->S[j+1][k+1][0], stock->S[j+1][k+1][1], stock->S[j+1][k+1][2]);
				glVertex3f(stock->S[j][k+1][0], stock->S[j][k+1][1], stock->S[j][k+1][2]);
			glEnd();
		}
	}
}

void	NcDisplay::displayProfile(Profile *target)
{

	static double c = 0.1;
	glBegin(GL_LINE_STRIP);
		glColor3d(1.0, 0.0, c);
		for(int i = 0; i < target->no_pts; i++)
			glVertex3d(target->P[i][0], target->P[i][1], 0.0);
	glEnd();
	c+= 0.01;

}


STATUS	NcDisplay::compressArray(double target[][2], int &n)
{
	NcVector v1, v2, v3;
	for(int i = 0; i < (n-2); i++)
	{
		v1 = NcVector::get_v(target[i]);
		v2 = NcVector::get_v(target[i+1]);
		v3 = NcVector::get_v(target[i+2]);
		
		double area = NcVector::get_area(v1, v2, v3);

		if(fabs(area) <= TOL)
		{
			for(int j = i+1; j < (n-1); j++)
			{
				target[j][0] = target[j+1][0];
				target[j][1] = target[j+1][1];
			}
			n--;
			if(i)
				i--;
		}
	}
	return OK;
}


STATUS	NcDisplay::createDeformedBody(Profile* target, double **tool, CUT cut)
{	
	double modi_target[1000][2];
	int iTargetPoints;
	if(cut == NO_VERTICAL_CUT)
	{
		if(fabs(tool[0][0] - tool[1][0]) < TOL)
		{
			surfaceDisplay();
#ifdef debug
			fprintf(stderr,"Facing operation skiping\n");
#endif
			return OK;
		}
	}
	if(tool[0][0] > tool[1][0])
	{
		surfaceDisplay();
#ifdef debug
		fprintf(stderr,"return stroke in left hand tool does not cut\n");
#endif
		return OK;
	}

	NcPolygonBoolean polyboolean;
	polyboolean.boolean_main(target->P, target->no_pts, tool, 5, modi_target, iTargetPoints);

	compressArray(modi_target, iTargetPoints);		

	if(target->no_pts != iTargetPoints)
	{
		target->free_allocate();
		target->no_pts = iTargetPoints;
		target->allocate();
	}
	for(int i=0; i < target->no_pts; i++)
	{
		target->P[i][0] = modi_target[i][0];
		target->P[i][1] = modi_target[i][1];
		target->P[i][2] = 0.0;
	}
	displayProfile(target);			
	createPart();
	surfaceDisplay();
	return OK;
}

STATUS	NcDisplay::createDeformedBody(Profile* target)
{
	displayProfile(target);				
	createPart();
	surfaceDisplay();
	return OK;
}


void	NcDisplay::addDLIndexesToList(NcCode *code)
{
	QList<Profile *>::iterator profileItr = code->getCodePartProfileList()->begin();
	for(; profileItr != code->getCodePartProfileList()->end(); profileItr++)
	{
		QList<GLuint>::iterator gllistItr = (*profileItr)->mAssociated2DDLIndexes->begin();
		for(; gllistItr != (*profileItr)->mAssociated2DDLIndexes->end(); gllistItr++)
		{
			mDisplayListIndexes.push_back(*gllistItr);
		}
	}
}

void	NcDisplay::addProfilesToList(NcCode *code)
{
	QList<Profile *>::iterator proflistItr = code->getCodePartProfileList()->begin();
	for(; proflistItr != code->getCodePartProfileList()->end(); proflistItr++)
	{
		mPartProfileList.push_back(*proflistItr);
	}	
}


void	NcDisplay::generateDisplayLists()
{
	generateStartupDLForStock();

	QList<NcCode *>::iterator codeItr = NcMachine::NcMachineInstance()->mGCodeList->begin();

	for(; codeItr != NcMachine::NcMachineInstance()->mGCodeList->end(); codeItr++)	//iterate through code list created when reading file
	{
		(*codeItr)->generateDisplayList();
		addDLIndexesToList(*codeItr);
		addProfilesToList(*codeItr);	
	}

	double P[2] = {0};
	P[0] = mStockBoundingBox.zmax;
	P[1] = mStockBoundingBox.xmax;
	
	double **tool;
	tool = new double* [5];
	for(int i = 0; i < 5; i++)
	{
		tool[i] = new double[2];
	}
	
	for(int i = 1; i < mPartProfileList.size(); i++)  //i = 0 for display of initial stock profile
	{
		Profile *ppp = mPartProfileList.at(i);

		if(mPartProfileList.at(i)->type == CGAUX)		//do not generate DL for auxillary moves 
			continue;
		if(mPartProfileList.at(i)->typeTool == CT02)
		{
			for(int j = 0; j < mPartProfileList.at(i)->no_pts - 1; j += 2)
			{
				load_Drilling_Tool(tool, mPartProfileList.at(i)->P[j], mPartProfileList.at(i)->P[j+1], P);
				
				GLuint dlId = glGenLists(1);

				mPartProfileList.at(i)->mAssocitedDBDLIndexes->push_back(dlId);

				mPartProfileList.at(i)->mNoOfDBDL++;

				mDeformedBodyDLIndexes.push_back(dlId);

				glNewList(dlId, GL_COMPILE);
					createDeformedBody(stock, tool, ALLOW_VERTICAL_CUT);		
				glEndList();
			}	
		}
		else if(mPartProfileList.at(i)->typeTool == CT00)
		{	
			int kk = mPartProfileList.at(i)->no_pts - 1;
			Profile *pp = mPartProfileList.at(i);

			for(int j = 0; j < mPartProfileList.at(i)->no_pts - 1; j++)
			{
				/*load_CG00_Tool(tool, P, i, j);*/
				
				GLuint dlId = glGenLists(1);
				
				mPartProfileList.at(i)->mAssocitedDBDLIndexes->push_back(dlId); //keeping related DL indexes with the profile

				mPartProfileList.at(i)->mNoOfDBDL++;							//keeping related DB DL count with the profile

				int sz = mPartProfileList.at(i)->mAssocitedDBDLIndexes->size();

				mDeformedBodyDLIndexes.push_back(dlId);
				
				glNewList(dlId, GL_COMPILE);
						createDeformedBody(stock);						// Display list for deformed body
				glEndList();
			}	
		}
		else if(mPartProfileList.at(i)->typeTool == CT01)
		{	
			int kk = mPartProfileList.at(i)->no_pts - 1;
			Profile *pp = mPartProfileList.at(i);

			for(int j = 0; j < mPartProfileList.at(i)->no_pts - 1; j++)
			{
				load_Cutting_Tool(tool, P, i, j);
				
				GLuint dlId = glGenLists(1);
				
				mPartProfileList.at(i)->mAssocitedDBDLIndexes->push_back(dlId); //keeping related DL indexes with the profile

				mPartProfileList.at(i)->mNoOfDBDL++;							//keeping related DB DL count with the profile

				int sz = mPartProfileList.at(i)->mAssocitedDBDLIndexes->size();

				mDeformedBodyDLIndexes.push_back(dlId);
				
				glNewList(dlId, GL_COMPILE);
					createDeformedBody(stock, tool);							// Display list for deformed body
				glEndList();
				#ifdef debug
				cout << stock->P[0][0] << " " << stock->P[0][1] << " " << stock->P[0][2] << " " 
					 << stock->P[1][0] << " " << stock->P[1][1] << " " << stock->P[1][2] << " "
					 << stock->P[2][0] << " " << stock->P[2][1] << " " << stock->P[2][2] << " "
					 << stock->P[3][0] << " " << stock->P[3][1] << " " << stock->P[3][2] << " "
					 << stock->P[4][0] << " " << stock->P[4][1] << " " << stock->P[4][2] << endl;
				#endif
			}	
		}
		else if(mPartProfileList.at(i)->typeTool == CT03)     //for parting/grooving tool
		{
			int u = mPartProfileList.at(i)->no_pts;
			Profile *ppp = mPartProfileList.at(i);

			for(int j = 0; j < mPartProfileList.at(i)->no_pts - 2; j+=2)
			{
				load_Parting_Tool(tool, P, i, j);
				
				GLuint dlId = glGenLists(1);
				
				mPartProfileList.at(i)->mAssocitedDBDLIndexes->push_back(dlId); //keeping related DL indexes with the profile

				mPartProfileList.at(i)->mNoOfDBDL++;							//keeping related DB DL count with the profile

				int sz = mPartProfileList.at(i)->mAssocitedDBDLIndexes->size();

				mDeformedBodyDLIndexes.push_back(dlId);
				
				glNewList(dlId, GL_COMPILE);
					createDeformedBody(stock, tool);							// Display list for deformed body
				glEndList();

				#ifdef debug
				cout << stock->P[0][0] << " " << stock->P[0][1] << " " << stock->P[0][2] << " " 
					 << stock->P[1][0] << " " << stock->P[1][1] << " " << stock->P[1][2] << " "
					 << stock->P[2][0] << " " << stock->P[2][1] << " " << stock->P[2][2] << " "
					 << stock->P[3][0] << " " << stock->P[3][1] << " " << stock->P[3][2] << " "
					 << stock->P[4][0] << " " << stock->P[4][1] << " " << stock->P[4][2] << endl;
				#endif

			}	
		}
		else if(mPartProfileList.at(i)->typeTool == CT05)
		{	
			int kk = mPartProfileList.at(i)->no_pts - 1;
			Profile *pp = mPartProfileList.at(i);

			for(int j = 0; j < mPartProfileList.at(i)->no_pts - 1; j++)
			{
				load_Facing_Tool(tool, P, i, j);
				
				GLuint dlId = glGenLists(1);
				
				mPartProfileList.at(i)->mAssocitedDBDLIndexes->push_back(dlId); //keeping related DL indexes with the profile

				mPartProfileList.at(i)->mNoOfDBDL++;							//keeping related DB DL count with the profile

				int sz = mPartProfileList.at(i)->mAssocitedDBDLIndexes->size();

				mDeformedBodyDLIndexes.push_back(dlId);
				
				glNewList(dlId, GL_COMPILE);
					createDeformedBody(stock, tool, ALLOW_VERTICAL_CUT);							// Display list for deformed body
				glEndList();

				/*cout << stock->P[0][0] << " " << stock->P[0][1] << " " << stock->P[0][2] << " " 
					 << stock->P[1][0] << " " << stock->P[1][1] << " " << stock->P[1][2] << " "
					 << stock->P[2][0] << " " << stock->P[2][1] << " " << stock->P[2][2] << " "
					 << stock->P[3][0] << " " << stock->P[3][1] << " " << stock->P[3][2] << " "
					 << stock->P[4][0] << " " << stock->P[4][1] << " " << stock->P[4][2] << endl;*/
			}	
		}
		else if(mPartProfileList.at(i)->typeTool == CT07)
		{	
			int kk = mPartProfileList.at(i)->no_pts - 1;
			Profile *pp = mPartProfileList.at(i);

			for(int j = 0; j < mPartProfileList.at(i)->no_pts - 1; j++)
			{
				load_Cutting_Tool(tool, P, i, j);
				
				GLuint dlId = glGenLists(1);
				
				mPartProfileList.at(i)->mAssocitedDBDLIndexes->push_back(dlId); //keeping related DL indexes with the profile

				mPartProfileList.at(i)->mNoOfDBDL++;							//keeping related DB DL count with the profile

				int sz = mPartProfileList.at(i)->mAssocitedDBDLIndexes->size();

				mDeformedBodyDLIndexes.push_back(dlId);
				
				glNewList(dlId, GL_COMPILE);
					createDeformedBody(stock, tool);							// Display list for deformed body
				glEndList();

				#ifdef debug
				cout << stock->P[0][0] << " " << stock->P[0][1] << " " << stock->P[0][2] << " " 
					 << stock->P[1][0] << " " << stock->P[1][1] << " " << stock->P[1][2] << " "
					 << stock->P[2][0] << " " << stock->P[2][1] << " " << stock->P[2][2] << " "
					 << stock->P[3][0] << " " << stock->P[3][1] << " " << stock->P[3][2] << " "
					 << stock->P[4][0] << " " << stock->P[4][1] << " " << stock->P[4][2] << endl;
				#endif
			}	
		}
	}
}

STATUS	NcDisplay::load_Facing_Tool(double **tool, double *P, int i, int j)
{

	//cout << "profile co-ords" << mPartProfileList.at(i)->P[j+1][0] << " " << mPartProfileList.at(i)->P[j+1][1]
	//	 << " " << mPartProfileList.at(i)->P[j][0] << " " << mPartProfileList.at(i)->P[j][1] << endl;


	tool[0][0] = mPartProfileList.at(i)->P[j][0];
	tool[0][1] = mPartProfileList.at(i)->P[j][1];
	tool[1][0] = mPartProfileList.at(i)->P[j+1][0];
	tool[1][1] = mPartProfileList.at(i)->P[j+1][1];
	tool[2][0] = tool[1][0] + 50;
	tool[2][1] = tool[1][1];
	tool[3][0] = tool[2][0];
	tool[3][1] = tool[0][1];
	tool[4][0] = tool[0][0];
	tool[4][1] = tool[0][1];
	
	#ifdef debug
	std::cout << "tool coords: "   << tool[0][0] << " " << tool[0][1] << " " << tool[1][0] << " " 
			  << tool[1][1] << " " << tool[2][0] << " " << tool[2][1] << " " << tool[3][0] << " "
			  << tool[3][1] << " " << tool[4][0] << " " << tool[4][1] << endl;
	#endif

	return OK;
}


STATUS	NcDisplay::load_CG00_Tool(double **tool, double *P, int i, int j)
{
	tool[0][0] = 0.0;
	tool[0][1] = 0.0;
	tool[1][0] = 0.0;
	tool[1][1] = 0.0;
	tool[2][0] = 0.0;
	tool[2][1] = 0.0;
	tool[3][0] = 0.0;
	tool[3][1] = 0.0;
	tool[4][0] = 0.0;
	tool[4][1] = 0.0;
	return OK;
}

void	NcDisplay::generateStartupDLForStock()
{
	GLuint dlId1 = glGenLists(1);
	
	mPartProfileList.push_back(stock);

	stock->mAssocitedDBDLIndexes->push_back(dlId1);

	glNewList(dlId1, GL_COMPILE);
		displayStockProfile();
	glEndList();
}


GLuint	NcDisplay::getStockDisplayListIndex()
{
	return stock->mAssocitedDBDLIndexes->at(0);
}


int		NcDisplay::spinDisplay(bool gstepmode, int goNextOperation, int& deformed_ds_count, int& DISPLAY_count)
{	
	//static int jc=0;
	////if(u>=DISPLAY_count)
	////	return ;
	//ToolX = tool_st_x;
	//ToolZ = tool_st_y;
	//
	//if(jc < part[u].no_pts - 1)
	//{
	//	if(part[u].type == CG91 && jc != 0)
	//	{
	//		if(part[u].P[jc][0] == part[u].P[jc-1][0])
	//			;	//don't do anything
	//		else 
	//		{
	//			translate_dx = (part[u].P[jc][0] - tool_st_x - 6);		
	//			translate_dy = (part[u].P[jc][1] - tool_st_y);		
	//		}
	//	}
	//	else
	//	{
	//		//if was added by Nem to stop the tool while implementing next functionalities
	//		if(u != 0)
	//		{
	//			translate_dx = (part[u].P[jc][0] - tool_st_x-6);		
	//			translate_dy = (part[u].P[jc][1] - tool_st_y);
	//		}		
	//	}
	//	codetype = part[u].type;
	//	ttype = part[u].typeTool;
	//	cool1 = part[u].coolant1;
	//	cool2 = part[u].coolant2;
	//	sstatus = part[u].spindle;
	//	ffeed = part[u].feed;

	//	jc++;	

	//	if(part[u].type != CG00)
	//	{
	//		current_deformed_ds_index++;
	//		
	//		if(current_deformed_ds_index == deformed_ds_count)
	//		{
	//			current_deformed_ds_index = 0;
	//		}
	//	}
	//}
	//else
	//{
	//	if(gstepmode == true)
	//		u = goNextOperation;
	//	else
	//	{
	//		goNextOperation=u++;
	//	}
	//	if(u >= DISPLAY_count)
	//	{			
	//		u = 0;
	//		goNextOperation = 0;
	//		current_deformed_ds_index = 0;
	//	}
	//	jc=0;
	//}

	//if(ang<=360)
	//	ang=ang+10;
	//else 
	//	ang=0.0;

	//return u;

	return 0;
}



STATUS	NcDisplay::load_Cutting_Tool(double **tool, double *P, int i, int j)
{	
	tool[0][0] = mPartProfileList.at(i)->P[j+1][0];
	tool[0][1] = mPartProfileList.at(i)->P[j+1][1];
	tool[1][0] = mPartProfileList.at(i)->P[j][0];
	tool[1][1] = mPartProfileList.at(i)->P[j][1];
	tool[2][0] = tool[1][0];
	tool[2][1] = P[1]+50;
	tool[3][0] = tool[0][0];
	tool[3][1] = P[1]+50;
	tool[4][0] = tool[0][0];
	tool[4][1] = tool[0][1];
	return OK;
}


STATUS	NcDisplay::load_Parting_Tool(double **tool, double *P, int i, int j)
{	
	Profile *prof = mPartProfileList.at(i);

	#ifdef debug
	cout << "profile co-ords" << mPartProfileList.at(i)->P[j+1][0] << " " << mPartProfileList.at(i)->P[j+1][1]
		 << " " << mPartProfileList.at(i)->P[j][0] << " " << mPartProfileList.at(i)->P[j][1] << endl;
	#endif

	tool[0][0] = mPartProfileList.at(i)->P[j][0];  //intersection works with anticlockwise order of tool vertices
	tool[0][1] = mPartProfileList.at(i)->P[j][1];
	tool[1][0] = mPartProfileList.at(i)->P[j+1][0];
	tool[1][1] = mPartProfileList.at(i)->P[j+1][1];
	tool[2][0] = tool[1][0];
	tool[2][1] = tool[1][1] + 120 ;
	tool[3][0] = tool[0][0];
	tool[3][1] = tool[1][1] + 120;
	tool[4][0] = tool[0][0];
	tool[4][1] = tool[0][1];

	#ifdef debug
	std::cout << "tool coords: "   << tool[0][0] << " " << tool[0][1] << " " << tool[1][0] << " " 
			  << tool[1][1] << " " << tool[2][0] << " " << tool[2][1] << " " << tool[3][0] << " "
			  << tool[3][1] << " " << tool[4][0] << " " << tool[4][1] << endl;	
	#endif

	return OK;
}


STATUS	NcDisplay::load_Drilling_Tool(double **tool, double *P1, double *P2, double *P)
{
	tool[0][0] = P2[0];
	tool[0][1] = P2[1];
	tool[1][0] = P1[0];
	tool[1][1] = P1[1];
	tool[2][0] = P1[0] + 50;
	tool[2][1] = tool[1][1];
	tool[3][0] = P2[0] + 50;
	tool[3][1] = tool[0][1];
	tool[4][0] = tool[0][0];
	tool[4][1] = tool[0][1];

	/*std::cout << tool[0][0] << " " << tool[0][1] << endl;
	std::cout << tool[1][0] << " " << tool[1][1] << endl;
	std::cout << tool[2][0] << " " << tool[2][1] << endl;
	std::cout << tool[3][0] << " " << tool[3][1] << endl;
	std::cout << tool[4][0] << " " << tool[4][1] << endl;*/
	return OK;
}

void	NcDisplay::setIndex(int index)
{
	dllistcount = index;
}



void	NcDisplay::callToolPathDL()
{
	for(int i = 0; i < dllistcount; i++)
	{
		glCallList(mDisplayListIndexes.at(i));
	}
}


//used in PaintGL of NcSimulationWindow to display the initial stock
void	NcDisplay::callDeformedBodyDL(GLuint id) 
{
	glCallList(id);
}


void	NcDisplay::material_stock()
{
	GLfloat mat_shininess[] = { 100.0 };
	GLfloat mat_solid[] = { 0.75, 0.75, 0.0, 1.0 };
	GLfloat mat_zero[] = { 0.0, 0.0, 0.0, 1.0 };

	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_solid);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_zero);

	glMaterialfv(GL_BACK, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_BACK, GL_DIFFUSE, mat_solid);
	glMaterialfv(GL_BACK, GL_EMISSION, mat_zero);
}


STATUS	NcDisplay::normalvector(int t)
{
	double v1[3], v2[3];
	double length;
	int i, k;
	for(int j = 0; j < MAX; j++)
	{
		for(k = 0; k < stock->no_pts - 1; k++)
		{
			for(i = 0; i < 3; i++)
			{
				v1[i] = stock->S[j][k+1][i] - stock->S[j][k][i];

				if(j+1 == MAX)
				{
					v2[i] = stock->S[0][k][i] - stock->S[j][k][i];
				}
				else
				{
					v2[i] = stock->S[j+1][k][i] - stock->S[j][k][i];
				}
			}

			stock->unitnormal[j][k][0] = v1[1] * v2[2] - v1[2] * v2[1];
			stock->unitnormal[j][k][1] = v1[2] * v2[0] - v1[0] * v2[2];
			stock->unitnormal[j][k][2] = v1[0] * v2[1] - v1[1] * v2[0];

			length = sqrt(stock->unitnormal[j][k][0] * stock->unitnormal[j][k][0]
							+ stock->unitnormal[j][k][1] * stock->unitnormal[j][k][1]
							+ stock->unitnormal[j][k][2] * stock->unitnormal[j][k][2]);
			if(length==0.0)
				length=1.0;

			stock->unitnormal[j][k][0] = stock->unitnormal[j][k][0] / length;
			stock->unitnormal[j][k][1] = stock->unitnormal[j][k][1] / length;
			stock->unitnormal[j][k][2] = stock->unitnormal[j][k][2] / length;
		}
	}
	return OK;
}


