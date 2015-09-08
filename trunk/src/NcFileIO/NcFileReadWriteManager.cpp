#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <iostream>
#include "NcFileIO\NcFileReadWriteManager.h"
#include "NcStateMachine\NcMachine.h"
#include "NcStateMachine\NcStartupStockDisplay.h"
#include "NcSimulationController\NcSimulationController.h"
#include "NcDisplay\NcDisplay.h"
#include "NcGeomKernel\NcProfile.h"


using namespace std;
using namespace DiscreteSimulator;

NcFileReadWriteManager	*NcFileReadWriteManager::mNcFileReadWriteInstance = 0;

static bool		GCodeDetected = false;
static bool		TCodeDetected = false;
static bool		OnlyAxisCoordinatesDetected = false;	//for conditions where only x or z co-ordinates are specified and
														//g code is the last g code detected
static bool		firstGCodeDetected = false;
static bool		MCodeDetected = false;
static bool		FCodeDetected = false;
static bool		SCodeDetected = false;
static bool		mDwellCodeDetected = false;
static int		count = 0;

NcFileReadWriteManager::NcFileReadWriteManager() : mFile(0)
 {}

 NcFileReadWriteManager::~NcFileReadWriteManager()
 {
	if(mFile != 0)
	{
		delete mFile;
		mFile = 0;
	}
 }

 NcFileReadWriteManager* NcFileReadWriteManager::getFileReadWriteInstance()
 {
	 if(NcFileReadWriteManager::mNcFileReadWriteInstance == 0)
	{
		NcFileReadWriteManager::mNcFileReadWriteInstance = new NcFileReadWriteManager();
	}
	return NcFileReadWriteManager::mNcFileReadWriteInstance;
 }


inline QFile* NcFileReadWriteManager::getCurrentNcFile()
{
	return mFile;
}

 bool	NcFileReadWriteManager::openNcFileForReadWrite()
 {
	if(mFile == 0)
	{
		if(!mCurrentFileName.isEmpty())
		{
			mFile = new QFile(mCurrentFileName);
			if(mFile->open(QFile::ReadWrite | QFile::Text))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
 }


QString		NcFileReadWriteManager::getFullNcCodeText()
{
	return mFile->readAll();
}


static int linecount = 0;

bool	NcFileReadWriteManager::interpreteNcFile()
{
	mFile->close();
	if(mFile->open(QFile::ReadWrite | QFile::Text))
	{
		QTextStream ncfilereader(mFile);

		while(!ncfilereader.atEnd())
		{
			QString codeblock = ncfilereader.readLine();      //read nc code line by line
			interpreteCodeBlock(codeblock);					  //interpret each block read
		}	
		/*if(*/NcDisplay::getNcDisplayInstance()->setStockBBFinalValues();/*)*/
			NcDisplay::getNcDisplayInstance()->generateDisplayLists();  //generating display list after file reading is over
			NcStartupStockDisplay::getStockDisplayInstance()
				->setDLForStartupStock(NcDisplay::getNcDisplayInstance()->getStockDisplayListIndex());
			/*else
			return false;*/
		linecount = 0;
	}
	return true;
}


void	NcFileReadWriteManager::interpreteCodeBlock(QString codeblock)
{
	//write code for checking each code block for errors here
	//check for errors in the block before initializing code block in the machine

	QStringList codelist = codeblock.split(" ", QString::SkipEmptyParts, Qt::CaseInsensitive);	    //split code blocks at spaces 
	
	NcMachine::NcMachineInstance()->initializeCodeBlockInTheMachine(codeblock, linecount++);

	foreach(QString code, codelist)
	{
		intrepreteIndividualCodes(code);
	}

	if(GCodeDetected == true)
	{
		if(firstGCodeDetected == true && ::count == 1)
		{
			NcDisplay::getNcDisplayInstance()->setStockBBInitialValues();
			::count++;
		}

		NcMachine::NcMachineInstance()->buildGCodeList();			//build QList of GCodes

		//create display list for the g code detected here
		NcDisplay::getNcDisplayInstance()->updateStockBoundingBox();
		
	}

	GCodeDetected = false;		//set g code detection bool to false for the next g code detection

	if(MCodeDetected == true)
	{
		NcMachine::NcMachineInstance()->buildMCodes();
		MCodeDetected = false;
	}

	if(TCodeDetected == true)
	{
		NcMachine::NcMachineInstance()->buildTCodes();
		TCodeDetected = false;
	}

	if(FCodeDetected == true)
	{
		NcMachine::NcMachineInstance()->buildFCodes();
		FCodeDetected = false;
	}

	if(SCodeDetected == true)
	{
		NcMachine::NcMachineInstance()->buildSCodes();
		SCodeDetected = false;
	}

	mDwellCodeDetected = false;
}


void	NcFileReadWriteManager::intrepreteIndividualCodes(QString code)
{
	
	char codeCharacter = code.at(0).toAscii();
	switch(codeCharacter)
	{
	case 'G': 
	case 'g':
		{
			interpreteGCode(code);
			GCodeDetected = true;
			if(firstGCodeDetected == false)
			{
				firstGCodeDetected = true;
				::count++;
			}
			break;
		}
	case 'M': 
	case 'm':
		{
			MCodeDetected = true;
			interpreteMCode(code);
			break;
		}
	case 'S': 
	case 's':
		{
			SCodeDetected = true;
			interpreteSCode(code);
			break;
		}
	case 'F':  
	case 'f':
		{
			FCodeDetected = true;
			interpreteFCode(code);
			break;
		}
	case 'T':
	case 't':
		{
			TCodeDetected = true;
			interpreteTCode(code);			
			break;
		}
	case 'X':  
	case 'x' : 
		{
			interpreteXCode(code);
			break;
		}
	case 'Z': 
	case 'z' :
		{
			interpreteZCode(code);
			break;
		}
	case 'U':
	case 'u':
		{
			interpreteUCode(code);
			break;
		}
	case 'W': 
	case 'w':
		{
			interpreteWCode(code);
			break;
		}
	case 'I': 
	case 'i':
		{
			interpreteICode(code);
			break;
		}
	case 'K': 
	case 'k':
		{
			interpreteKCode(code);
			break;
		}
	case 'D': 
	case 'd':
		{
			interpreteDCode(code);
			break;
		}
	default:
		{
			
		}

	}
	
}

void	NcFileReadWriteManager::interpreteGCode(QString code)
{
	int codenumber = code.right(code.size() - 1).toInt();

	//setting the machine operational mode in this way eliminates the need to check for conflicting 
	//G codes in the same block since the last detected G code will be set as the current operational
	//mode and the last detected code will be built in NcMachine
	switch(codenumber)
	{
	case 0:								//G00 - Rapid Motion mode   for non-productive moves between the cycles
		{								//moves the tool to the start of the cycle
			NcMachine::NcMachineInstance()->setMachineOperationalMode(RAPIDMOTION);
			break;
		}
	case 1:			//linear interpolation
		{
			NcMachine::NcMachineInstance()->setMachineOperationalMode(LINEARINTERPOL);
			break;
		}
	case 2:			//circular interpolation cw
		{
			NcMachine::NcMachineInstance()->setMachineOperationalMode(CWCIRCULARINTERPOL);
			break;
		}
	case 3:			//circular interpolation ccw
		{
			NcMachine::NcMachineInstance()->setMachineOperationalMode(CCWCIRCULARINTERPOL);
			break;
		}
	case 4:			//Dwell command  
		{
			mDwellCodeDetected = true;
			NcMachine::NcMachineInstance()->setMachineOperationalMode(DWELL);
			break;
		}
	case 20:		//set inch unit
		{
			NcMachine::NcMachineInstance()->setMachineOperationalMode(INCHUNITMODE);
			break;
		}
	case 21:		//set mm unit
		{
			NcMachine::NcMachineInstance()->setMachineOperationalMode(MMUNITMODE);
			break;
		}
	case 28:		//machine zero return --- X and Z co-ordinates in the code denotes the intermediate point
		{
			NcMachine::NcMachineInstance()->setMachineOperationalMode(MACHINEZERORETURN);
			break;
		}
	case 74:		//peck drilling cycle
		{
			NcMachine::NcMachineInstance()->setMachineOperationalMode(PECKDRILLING);
			break;
		}
	case 75:		//parting and grooving cycle
		{
			NcMachine::NcMachineInstance()->setMachineOperationalMode(PARTINGGROOVING);
			break;
		}
	case 76:		//multi pass threading cycle
		{
			NcMachine::NcMachineInstance()->setMachineOperationalMode(MULTIPASSTHREADING);
			break;
		}
	case 90:		//turning and boring (same G90 used for both turning and boring)
		{
			NcMachine::NcMachineInstance()->setMachineOperationalMode(CANNEDTURNING);
			break;
		}
	case 92: //single pass threading cycle
		{
			NcMachine::NcMachineInstance()->setMachineOperationalMode(SINGLEPASSTHREADING);
			break;
		}
	case 94:		//facing cycle
		{
			NcMachine::NcMachineInstance()->setMachineOperationalMode(FACING);
			break;
		}
	
	}
}


void	NcFileReadWriteManager::interpreteMCode(QString code)
{
	int mcode = code.right(code.size() - 1).toInt();
	switch(mcode)
	{
	case 3:
		{
			NcMachine::NcMachineInstance()->mCurrentMCode = SPINDLECW;
			break;
		}
	case 5:
		{
			NcMachine::NcMachineInstance()->mCurrentMCode = SPINDLESTOP;
			break;
		}
	case 30:
		{
			NcMachine::NcMachineInstance()->mCurrentMCode = PROGRAMSTOP;
			break;
		}
		
	}
}


void	NcFileReadWriteManager::interpreteSCode(QString code)
{
	double speed = code.right(code.size() - 1).toDouble();
	NcMachine::NcMachineInstance()->mCurrentSpindleSpeed = speed;
}


void	NcFileReadWriteManager::interpreteFCode(QString code)
{
	double feedrate = code.right(code.size() - 1).toDouble();
	NcMachine::NcMachineInstance()->mCurrentFeedRate = feedrate;
}


void	NcFileReadWriteManager::interpreteTCode(QString code)
{
	int toolnumber = code.right(code.size() - 1).toInt();
	switch(toolnumber)
	{
	case 1:
		{
			NcMachine::NcMachineInstance()->mCurrentToolType = TURNINGTOOL;
			break;
		}
	case 2:
		{
			NcMachine::NcMachineInstance()->mCurrentToolType = DRILLINGTOOL;
			break;
		}
	case 3:
		{
			NcMachine::NcMachineInstance()->mCurrentToolType = PARTINGTOOL;
			break;
		}
	case 4:
		{
			NcMachine::NcMachineInstance()->mCurrentToolType = BORINGTOOL;
			break;
		}
	case 5:
		{
			NcMachine::NcMachineInstance()->mCurrentToolType = FACINGTOOL;
			break;
		}
	case 6:
		{
			break;
		}
	case 7:
		{
			NcMachine::NcMachineInstance()->mCurrentToolType = THREADINGTOOL;
			break;
		}
	case 8:
		{
			break;
		}
	}
}


void	NcFileReadWriteManager::interpreteXCode(QString code)
{
	if(mDwellCodeDetected == false)
	{
		double xpos = code.right(code.size() - 1).toDouble();
		NcMachine::NcMachineInstance()->setEndOfMotionXPosition(xpos);
	}
	else if(mDwellCodeDetected == true)
	{
		int dwelltime = code.right(code.size() - 1).toInt();  //dwell time is either in sec or milliseconds
		NcMachine::NcMachineInstance()->mDwellTime = dwelltime;
	}
}


void	NcFileReadWriteManager::interpreteZCode(QString code)
{
	double zpos = code.right(code.size() - 1).toDouble();
	NcMachine::NcMachineInstance()->setEndOfMotionZPosition(zpos);
}


void	NcFileReadWriteManager::interpreteUCode(QString code)
{
	
}

void	NcFileReadWriteManager::interpreteWCode(QString code)
{
	
}

void	NcFileReadWriteManager::interpreteICode(QString code)
{
	double ival = code.right(code.size() - 1).toDouble();
	NcMachine::NcMachineInstance()->I = ival;
}

void	NcFileReadWriteManager::interpreteKCode(QString code)
{
	double kval = code.right(code.size() - 1).toDouble();
	NcMachine::NcMachineInstance()->K = kval;
}

void	NcFileReadWriteManager::interpreteDCode(QString code)
{
	double dval = code.right(code.size() - 1).toDouble();
	NcMachine::NcMachineInstance()->D = dval;
}


STATUS	NcFileReadWriteManager::writeSTLFile(const char *file)
{
	Profile *stock = NcDisplay::getNcDisplayInstance()->getStockProfile();
	
	int j, k;
	FILE *output_stl_handle;
	
	output_stl_handle = fopen(file, "w+");	

	fprintf(output_stl_handle, "solid CCTech_DiscreteSimulator_Stock\n");

	for(j = 0; j < MAX; j++)
		for(k = 0; k < stock->no_pts - 1; k++)
		{
			fprintf(output_stl_handle, "\tfacet normal %18.8e %18.8e %18.8e\n",
					stock->unitnormal[j][k][0], stock->unitnormal[j][k][1], stock->unitnormal[j][k][2]);
			
			fprintf(output_stl_handle,"\t\touter loop\n");
			
			fprintf(output_stl_handle,"\t\t\tvertex %18.8e %18.8e %18.8e\n",
					stock->S[j][k][0], stock->S[j][k][1], stock->S[j][k][2]);
			
			fprintf(output_stl_handle,"\t\t\tvertex %18.8e %18.8e %18.8e\n",
                	stock->S[j+1][k][0], stock->S[j+1][k][1], stock->S[j+1][k][2]);
			
			fprintf(output_stl_handle,"\t\t\tvertex %18.8e %18.8e %18.8e\n",
                	stock->S[j+1][k+1][0], stock->S[j+1][k+1][1], stock->S[j+1][k+1][2]);
			
			fprintf(output_stl_handle, "\t\tendloop\n");
			
			fprintf(output_stl_handle, "\tendfacet\n");
			
			fprintf(output_stl_handle,"\tfacet normal %18.8e %18.8e %18.8e\n",
				    stock->unitnormal[j][k][0], stock->unitnormal[j][k][1], stock->unitnormal[j][k][2]);
			
			fprintf(output_stl_handle, "\t\touter loop\n");
			
			fprintf(output_stl_handle,"\t\t\tvertex %18.8e %18.8e %18.8e\n",
					stock->S[j][k][0], stock->S[j][k][1], stock->S[j][k][2]);
			
			fprintf(output_stl_handle, "\t\t\tvertex %18.8e %18.8e %18.8e\n",
                			stock->S[j+1][k+1][0], stock->S[j+1][k+1][1], stock->S[j+1][k+1][2]);
			
			fprintf(output_stl_handle, "\t\t\tvertex %18.8e %18.8e %18.8e\n",
					stock->S[j][k+1][0], stock->S[j][k+1][1], stock->S[j][k+1][2]);
			
			fprintf(output_stl_handle, "\t\tendloop\n");
			
			fprintf(output_stl_handle, "\tendfacet\n");
	}
	fprintf(output_stl_handle, "endsolid\n");

	fclose(output_stl_handle);
	
	return OK;
}