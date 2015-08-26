#include <QString>
#include "NcStateMachine\NcMachine.h"
#include "NcStateMachine\NcCode.h"
#include "NcStateMachine\NcCodeBlock.h"
#include "NcStateMachine\G00.h"
#include "NcStateMachine\G01.h"
#include "NcStateMachine\G02.h"
#include "NcStateMachine\G03.h"
#include "NcStateMachine\G04.h"
#include "NcStateMachine\G2021.h"
#include "NcStateMachine\G28.h"
#include "NcStateMachine\NcEndOfCutFeedRateMove.h"
#include "NcStateMachine\NcAuxRapidMove.h"
#include "NcStateMachine\G53.h"
#include "NcStateMachine\G54.h"
#include "NcStateMachine\G90.h"
#include "NcStateMachine\G74.h"
#include "NcStateMachine\G75.h"
#include "NcStateMachine\G76.h"
#include "NcStateMachine\G91.h"
#include "NcStateMachine\G92.h"
#include "NcStateMachine\G94.h"
#include "NcStateMachine\T01.h"
#include "NcStateMachine\T02.h"
#include "NcStateMachine\T03.h"
#include "NcStateMachine\T04.h"
#include "NcStateMachine\T05.h"
#include "NcStateMachine\T06.h"
#include "NcStateMachine\T07.h"
#include "NcStateMachine\T08.h"
#include "NcStateMachine\F.h"
#include "NcStateMachine\S.h"
#include "NcStateMachine\MCode.h"
#include <math.h>
#include <typeinfo>
#include <iostream>

using namespace std;

using namespace DiscreteSimulator;
NcMachine		*NcMachine::mNcMachineInstance = 0;

NcMachine::NcMachine()
{
	//machine tool change position, to be set at the startup of the application
	mToolChangeXPos = 100.0;	//+ve limit on x axis		
	mToolChangeZPos = 250.0;	//+ve limit on Z axis

	//preserves end of cut tool position for the last G code, helpful in setting start point for G02 and G03
	mLastMachineXPos = mToolChangeXPos;
	mLastMachineZPos = mToolChangeZPos;

	//preserves end of cut tool position for the current G code
	mEndOfMotionX = mToolChangeXPos;
	mEndOfMotionZ = mToolChangeZPos;

	//cycle start position for every cycle, tool comes to this point before taking the next depth of cut
	mCycleStartXPos = mToolChangeXPos;	
	mCycleStartZPos = mToolChangeZPos;

	mGCodeList = new QList<NcCode *>;
	mCompleteCodeList = new QList<NcCode *>;
	mNcCodeBlockList = new QList<NcCodeBlock *>;

	mDimensionUnit = MM;		//default unit = metric (G21)

	mCurrentSpindleSpeed = 550;		//default spindle speed (arbitrarily chosen)
	mCurrentFeedRate = 0.5;			//defualt feed rate (arbitrarily chosen)

}
		
NcMachine::~NcMachine()
{
	if(mNcCodeBlockList != 0)
	{
		delete mNcCodeBlockList;
		mNcCodeBlockList = 0;
	}
	if(mGCodeList != 0)
	{
		delete mGCodeList;
		mGCodeList = 0;
	}
	if(mCompleteCodeList != 0)
	{
		delete mCompleteCodeList;
		mCompleteCodeList = 0;
	}
}

void NcMachine::setConversionFactor(DimensionUnit unit)
{
	if(unit == MM)
	{
		mConversionFactor = 1.0;
		mDimensionUnit = MM;
	}
	else if(unit == INCH)
	{
		mConversionFactor = 25.4;
		mDimensionUnit = INCH;
	}
}

NcMachine* NcMachine::NcMachineInstance()
{
	if(NcMachine::mNcMachineInstance == 0)
	{
		NcMachine::mNcMachineInstance = new NcMachine();
	}
	return NcMachine::mNcMachineInstance;
}

//setting end of cut tool position for the current G code and preserving 
//end of cut tool position for the previous G code
void	NcMachine::setEndOfMotionXPosition(double xpos) 
{
	mLastMachineXPos = mEndOfMotionX;		//save tool position at the end of previous machining cut(G code)
	mEndOfMotionX = xpos;					//assign new tool end position to mEndOfMotionX
}

//setting end of cut tool position for the current G code and preserving 
//end of cut tool position for the previous G code
void	NcMachine::setEndOfMotionZPosition(double zpos)
{
	mLastMachineZPos = mEndOfMotionZ;		//save the tool postion at the end of previous machining cut(G code)
	mEndOfMotionZ = zpos;					//assign new tool end position to mEndOfMotionZ
}


void	NcMachine::setMachineOperationalMode(OperationalMode operationalmode)  //set current machine operational mode RAPID, LINEARINTERPOL etc
{
	mOperationalMode = operationalmode;
}


void	NcMachine::buildGCodeList()
{
	NcCode *newcode;

	if(mOperationalMode == RAPIDMOTION)		//for moving tool to the next cycle start, to go rapidly to tool change pos
	{
		newcode = new G00(1000);
		newcode->ctype = CG00;
		pushbackGCodeInLists(newcode);

		newcode->setstartX(mLastMachineXPos);  //initialized to tool change pos for first rapid move
		newcode->setstartZ(mLastMachineZPos);  //subsequent codes should update the cycle start x and z positions

		newcode->setX(mEndOfMotionX);			
		newcode->setZ(mEndOfMotionZ);

		mCycleStartXPos = mEndOfMotionX;			//cycle start X pos for the subsequent codes	
		mCycleStartZPos = mEndOfMotionZ;

		newcode->setF(mCurrentFeedRate);
		newcode->setS(mCurrentSpindleSpeed);

		newcode->toolType = CT00;
		mCurrentCodeBlock->mContainsMachiningGCode = true;
		newcode->setAsMachiningCode(true);

		mCurrentCodeBlock->mBlockCodeList.push_back(newcode);//push back G code in the current NcCodeBlock
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;
	}

	if(mOperationalMode == CANNEDTURNING)			//turning cycle G90	
	{
		newcode = new G90();
		newcode->ctype = CG90;

		newcode->setCycleStartX(mCycleStartXPos);
		newcode->setCycleStartZ(mCycleStartZPos);

		newcode->setstartX(mEndOfMotionX);				//move to the start of the next cut for turning
		newcode->setstartZ(mCycleStartZPos);		//turning always in -ve x. set start x to the new x obtained from the code

		newcode->setX(mEndOfMotionX);
		newcode->setZ(mEndOfMotionZ);

		newcode->setF(mCurrentFeedRate);
		newcode->setS(mCurrentSpindleSpeed);
		
		newcode->toolType = CT01;
		newcode->setAsMachiningCode(true);
		mCurrentCodeBlock->mContainsMachiningGCode = true;
		generateInitialRapidMoveToCutDepth(newcode);  //generate G00 for first rapid move to cut depth
		
		pushbackGCodeInLists(newcode);
		
		mCurrentCodeBlock->mBlockCodeList.push_back(newcode);//push back G code in the current NcCodeBlock
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;
		
		generateEndOfCutFeedrateMove(newcode);
		generateRapidReturnToCycleStart(newcode);
	}
	/******Pranit**/
	if(mOperationalMode == INCREMENTALMOVE)			//turning cycle G91	
	{
		newcode = new G91();
		newcode->ctype = CG91;

		newcode->setCycleStartX(mCycleStartXPos);
		newcode->setCycleStartZ(mCycleStartZPos);

		newcode->setstartX(mLastMachineXPos);				//move to the start of the next cut for turning
		newcode->setstartZ(mLastMachineZPos);	 	//turning always in -ve z. set start z to the new z obtained from the code

		newcode->setX(mEndOfMotionX); 
		newcode->setZ(mEndOfMotionZ);  

		newcode->setF(mCurrentFeedRate);
		newcode->setS(mCurrentSpindleSpeed);

		newcode->toolType = CT01;
		newcode->setAsMachiningCode(true);
		mCurrentCodeBlock->mContainsMachiningGCode = true;
		generateInitialRapidMoveToCutDepth(newcode);  //generate G00 for first rapid move to cut depth

		pushbackGCodeInLists(newcode);

		mCurrentCodeBlock->mBlockCodeList.push_back(newcode);//push back G code in the current NcCodeBlock
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;

		generateEndOfCutFeedrateMove(newcode);
		generateRapidReturnToCycleStart(newcode);
	}

	/******Pranit**/

	if(mOperationalMode == MACHINECOORDINATE)			//Move to machine co ordinate  G53	
	{
		newcode = new G53();
		newcode->ctype = CG53;	

		newcode->setstartX(mLastMachineXPos);				
		newcode->setstartZ(mLastMachineZPos);		

		newcode->setX(mEndOfMotionX);
		newcode->setZ(mEndOfMotionZ);

		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;

		//create two G00 codes in the G53 code one for motion from current tool position
		//to the machine zero point and add them to the G code list so that display list 
		//creation is taken care of

		generateG00CodesForG53Code(newcode);
	}

	/******Pranit**/
	if(mOperationalMode == WORKCOORDINATESYSTEM)			//Move to machine co ordinate  G53	
	{
		newcode = new G54();
		newcode->ctype = CG54;	

		newcode->setstartX(mLastMachineXPos);				
		newcode->setstartZ(mLastMachineZPos);		

		newcode->setX(mEndOfMotionX);
		newcode->setZ(mEndOfMotionZ);

		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;

		//create two G00 codes in the G53 code one for motion from current tool position
		//to the machine zero point and add them to the G code list so that display list 
		//creation is taken care of

		generateG00CodesForG53Code(newcode);
	}

	if(mOperationalMode == LINEARINTERPOL)
	{
		newcode = new G01();
		newcode->ctype = CG01;
		pushbackGCodeInLists(newcode);

		mCurrentCodeBlock->mBlockCodeList.push_back(newcode);//push back G code in the current NcCodeBlock
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;

		newcode->setstartX(mLastMachineXPos);				//for linear interpolation, start point is the current tool position
		newcode->setstartZ(mLastMachineZPos);				// (end of last G code)

		newcode->setX(mEndOfMotionX);						//end point is the target X and Z Coordinates mentioned in the G01 code block 
		newcode->setZ(mEndOfMotionZ);	

		newcode->setF(mCurrentFeedRate);
		newcode->setS(mCurrentSpindleSpeed);

		newcode->toolType = CT01;
		mCurrentCodeBlock->mContainsMachiningGCode = true;
		newcode->setAsMachiningCode(true);

		mCycleStartXPos = mEndOfMotionX;			//cycle start X pos for the subsequent codes	
		mCycleStartZPos = mEndOfMotionZ;			//tool taken to cycle start either with G00 or with G01

		
	}

	if(mOperationalMode == CWCIRCULARINTERPOL)
	{
		newcode = new G02();
		newcode->ctype = CG02;
		pushbackGCodeInLists(newcode);

		mCurrentCodeBlock->mBlockCodeList.push_back(newcode);//push back G code in the current NcCodeBlock
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;

		//for circular interpolation end of cut of the last G code becomes the start point for the 
		//circular interpolation and X and Z mentioned in the circular interpolation block itself 
		//becomes the end point
		newcode->setstartX(mLastMachineXPos);	
		newcode->setstartZ(mLastMachineZPos);	

		newcode->setX(mEndOfMotionX);
		newcode->setZ(mEndOfMotionZ);

		newcode->setI(I);
		newcode->setK(K);

		newcode->setF(mCurrentFeedRate);
		newcode->setS(mCurrentSpindleSpeed);

		newcode->toolType = CT01;
		mCurrentCodeBlock->mContainsMachiningGCode = true;
		newcode->setAsMachiningCode(true);

		
	}

	if(mOperationalMode == CCWCIRCULARINTERPOL)
	{
		newcode = new G03();
		newcode->ctype = CG03;
		pushbackGCodeInLists(newcode);

		mCurrentCodeBlock->mBlockCodeList.push_back(newcode);//push back G code in the current NcCodeBlock
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;

		//for circular interpolation end of cut of the last G code becomes the start point for the 
		//circular interpolation and X and Z mentioned in the circular interpolation block itself 
		//becomes the end point
		newcode->setstartX(mLastMachineXPos);	
		newcode->setstartZ(mLastMachineZPos);	

		newcode->setX(mEndOfMotionX);
		newcode->setZ(mEndOfMotionZ);

		newcode->setI(I);
		newcode->setK(K);

		newcode->setF(mCurrentFeedRate);
		newcode->setS(mCurrentSpindleSpeed);

		newcode->toolType = CT01;
		mCurrentCodeBlock->mContainsMachiningGCode = true;
		newcode->setAsMachiningCode(true);
	}

	if(mOperationalMode == DWELL)			//dwell code G04	
	{
		G04 *g04code = new G04();
		g04code->setDwellTime(mDwellTime);
		newcode = g04code;
		newcode->ctype = CGAUX;		

		newcode->setX(mEndOfMotionX);
		newcode->setZ(mEndOfMotionZ);
		
		//push back auxillary codes (CGAUX) in the current code block only and not in the
		//list of G codes 
		mCurrentCodeBlock->mBlockCodeList.push_back(newcode);	//push back G code in the current NcCodeBlock
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;
	}

	if(mOperationalMode == PECKDRILLING)
	{
		newcode = new G74();
		newcode->ctype = CG74;
		pushbackGCodeInLists(newcode);  //push back the first G74 code

		mCurrentCodeBlock->mBlockCodeList.push_back(newcode);//push back G code in the current NcCodeBlock
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;
						 
		newcode->setCycleStartX(mEndOfMotionX);
		newcode->setCycleStartZ(mLastMachineZPos);

		newcode->setstartX(mEndOfMotionX);		//end of motion set by the last Gcode( probably a positioning command)
		newcode->setstartZ(mLastMachineZPos);	

		newcode->setK(K);						//k represents depth of each cut in drilling in G74 cycle

		newcode->setX(mEndOfMotionX);
		newcode->setZ(mEndOfMotionZ);

		newcode->setF(mCurrentFeedRate);
		newcode->setS(mCurrentSpindleSpeed);

		newcode->toolType = CT02;
		mCurrentCodeBlock->mContainsMachiningGCode = true;
		newcode->setAsMachiningCode(true);
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;

		newcode->generateToolPathsForThecode();
	}

	if(mOperationalMode == PARTINGGROOVING)
	{
		newcode = new G75();
		newcode->ctype = CG75;
		pushbackGCodeInLists(newcode);

		mCurrentCodeBlock->mBlockCodeList.push_back(newcode);//push back G code in the current NcCodeBlock
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;

		newcode->setstartX(mLastMachineXPos);	//end of motion set by the last Gcode( probably a positioning command)
		newcode->setstartZ(mLastMachineZPos);	
												
		newcode->setX(mEndOfMotionX);
		newcode->setZ(mEndOfMotionZ);

		newcode->setI(I);						//depth of cut for parting / grooving

		newcode->setF(mCurrentFeedRate);
		newcode->setS(mCurrentSpindleSpeed);

		newcode->toolType = CT03;
		mCurrentCodeBlock->mContainsMachiningGCode = true;
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;
		newcode->setAsMachiningCode(true);
		newcode->generateToolPathsForThecode();
	}

	if(mOperationalMode == FACING)			//facing cycle G94	
	{
		newcode = new G94();
		newcode->ctype = CG94;

		newcode->setCycleStartX(mCycleStartXPos);
		newcode->setCycleStartZ(mCycleStartZPos);

		newcode->setstartX(mCycleStartXPos);				
		newcode->setstartZ(mEndOfMotionZ);		

		newcode->setX(mEndOfMotionX);
		newcode->setZ(mEndOfMotionZ);

		newcode->setF(mCurrentFeedRate);
		newcode->setS(mCurrentSpindleSpeed);
		
		newcode->toolType = CT05;
		newcode->setAsMachiningCode(true);
		mCurrentCodeBlock->mContainsMachiningGCode = true;
		generateInitialRapidMoveToCutDepth(newcode);	//generate G00 for first rapid move to cut depth
														//along Z for facing cut
		pushbackGCodeInLists(newcode);
		
		mCurrentCodeBlock->mBlockCodeList.push_back(newcode);	//push back G code in the current NcCodeBlock
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;
		
		generateEndOfCutFeedrateMove(newcode);
		generateRapidReturnToCycleStart(newcode);
	}


	if(mOperationalMode == MULTIPASSTHREADING)  //G76 - multi pass threading cycle
	{
		newcode = new G76();
		newcode->ctype = CG76;
		pushbackGCodeInLists(newcode); 

		mCurrentCodeBlock->mBlockCodeList.push_back(newcode);
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;
						 
		newcode->setCycleStartX(mLastMachineXPos);
		newcode->setCycleStartZ(mLastMachineZPos);

		newcode->setstartX(mLastMachineXPos);		//end of motion set by the last Gcode( probably a positioning command)
		newcode->setstartZ(mLastMachineZPos);	

		newcode->setK(K);		//single depth of thread (on one side of the cut)
		newcode->setD(D);		//depth of first threading pass

		//I code for taper threading not handled

		newcode->setX(mEndOfMotionX);
		newcode->setZ(mEndOfMotionZ);

		newcode->setF(mCurrentFeedRate);
		newcode->setS(mCurrentSpindleSpeed);

		newcode->toolType = CT07;
		mCurrentCodeBlock->mContainsMachiningGCode = true;
		newcode->setAsMachiningCode(true);
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;

		newcode->generateToolPathsForThecode();
	}


	if(mOperationalMode == MACHINEZERORETURN)     //G28 code
	{
		newcode = new G28();
		newcode->ctype = CG28;		

		newcode->setstartX(mLastMachineXPos);				
		newcode->setstartZ(mLastMachineZPos);		

		newcode->setX(mEndOfMotionX);
		newcode->setZ(mEndOfMotionZ);

		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;

		//create two G00 codes in the G28 code (one for motion from current tool position
		//to the intermediate point and the second from the intermediate point to the 
		//machine zero point) and add them to the G code list so that display list 
		//creation is taken care of

		generateG00CodesForG28Code(newcode);
	}

	if(mOperationalMode == SINGLEPASSTHREADING)			//single pass threading G92	
	{
		newcode = new G92();
		newcode->ctype = CG92;

		newcode->setCycleStartX(mCycleStartXPos);
		newcode->setCycleStartZ(mCycleStartZPos);

		newcode->setstartX(mLastMachineXPos);			//move to the start of the next cut for turning
		newcode->setstartZ(mCycleStartZPos);		//turning always in -ve x. set start x to the new x obtained from the code

		newcode->setX(mEndOfMotionX);
		newcode->setZ(mEndOfMotionZ);

		newcode->setF(mCurrentFeedRate);
		newcode->setS(mCurrentSpindleSpeed);
		
		newcode->toolType = CT07;
		newcode->setAsMachiningCode(true);
		mCurrentCodeBlock->mContainsMachiningGCode = true;
		generateInitialRapidMoveToCutDepth(newcode);  //generate G00 for first rapid move to cut depth
		
		pushbackGCodeInLists(newcode); //for executing the actual threading cut after the initial rapid move
		
		mCurrentCodeBlock->mBlockCodeList.push_back(newcode); //push back G code in the current NcCodeBlock
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;
		
		generateEndOfCutFeedrateMove(newcode);
		generateRapidReturnToCycleStart(newcode);
	}

	if(mOperationalMode == MMUNITMODE)
	{
		setConversionFactor(MM);
		newcode = new G21();
		newcode->ctype = CGAUX;
		pushbackGCodeInLists(newcode);
		mCurrentCodeBlock->mBlockCodeList.push_back(newcode);	//push back G code in the current NcCodeBlock
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;
	}

	if(mOperationalMode == INCHUNITMODE)
	{
		setConversionFactor(INCH);
		newcode = new G20();
		newcode->ctype = CGAUX;
		pushbackGCodeInLists(newcode);
		mCurrentCodeBlock->mBlockCodeList.push_back(newcode);//push back G code in the current NcCodeBlock
		newcode->mAssociatedCodeBlock = mCurrentCodeBlock;
	}

	int size = mGCodeList->size();
}	


void	NcMachine::buildMCodes()
{
	NcCode *mcode;
	if(mCurrentMCode == SPINDLECW)
	{
		mcode = new M03();
		mcode->ctype = CGAUX;
	}

	if(mCurrentMCode == SPINDLESTOP)
	{
		mcode = new M05();
		mcode->ctype = CGAUX;
	}

	if(mCurrentMCode == PROGRAMSTOP)
	{
		mcode = new M30();
		mcode->ctype = PRGSTOP;
	}
/*****************************Pranit*********************/
	
	if(mCurrentMCode == TOOLCHANGE)
	{
		mcode = new M06();
	}

	if(mCurrentMCode == COOLENTON)
	{
		mcode = new M08();
		mcode->ccoolant2 = true;
	}
	if(mCurrentMCode == COOLENTOFF)
	{
		mcode = new M09();
		mcode->ccoolant2 =false;
	}

/*************************************************************/	

	//Push the M code in the current code block to be iterated
	//over and executed by NcSimulationController
	mCurrentCodeBlock->mBlockCodeList.push_front(mcode);
	mcode->mAssociatedCodeBlock = mCurrentCodeBlock;
}

void	NcMachine::buildFCodes()
{
	NcCode *feedcode = new F();
	feedcode->ctype = CGAUX;
	feedcode->setF(mCurrentFeedRate);

	//push the F code in the current code block to be iterated in SimulationController
	mCurrentCodeBlock->mBlockCodeList.push_front(feedcode);
	feedcode->mAssociatedCodeBlock = mCurrentCodeBlock;
}


void	NcMachine::buildSCodes()
{
	NcCode *speedcode = new S();
	speedcode->ctype = CGAUX;
	speedcode->setS(mCurrentSpindleSpeed);

	//push the F code in the current code block to be iterated in SimulationController
	mCurrentCodeBlock->mBlockCodeList.push_front(speedcode);
	speedcode->mAssociatedCodeBlock = mCurrentCodeBlock;
}

void	NcMachine::buildTCodes()
{
	NcCode *tcode;

	if(mCurrentToolType == TURNINGTOOL)
	{
		tcode = new T01();
		tcode->toolType = CT01;
		tcode->ctype = CGAUX;
	}
	
	if(mCurrentToolType == DRILLINGTOOL)
	{
		tcode = new T02();
		tcode->toolType = CT02;
		tcode->ctype = CGAUX;
	}

	if(mCurrentToolType == PARTINGTOOL)
	{
		tcode = new T03();
		tcode->toolType = CT03;
		tcode->ctype = CGAUX;
	}

	if(mCurrentToolType == BORINGTOOL)
	{
		tcode = new T04();
		tcode->toolType = CT04;
		tcode->ctype = CGAUX;
	}

	if(mCurrentToolType == THREADINGTOOL)
	{
		tcode = new T07();
		tcode->toolType = CT07;
		tcode->ctype = CGAUX;
	}

	//Push the T code in the current code block to be iterated
	//over and executed by NcSimulationController
	mCurrentCodeBlock->mBlockCodeList.push_front(tcode);
	tcode->mAssociatedCodeBlock = mCurrentCodeBlock;

}


void	NcMachine::initializeCodeBlockInTheMachine(QString codeblock, int linecount /*used in line highlighting*/)
{
	//for every line (block) in Nc file, create a new code block and push it in the
	//block list kept with the machine. NcSimulationController iterated over through
	//this list of blocks and codes in each block
	mCurrentCodeBlock = new NcCodeBlock();
	mCurrentCodeBlock->mCodeBlockString = codeblock;
	mNcCodeBlockList->push_back(mCurrentCodeBlock);
	mCurrentCodeBlock->mLineIndex = linecount;
	
}


void	NcMachine::pushbackGCodeInLists(NcCode *newcode)
{
	mGCodeList->push_back(newcode);
	mCompleteCodeList->push_back(newcode);
}


void	NcMachine::generateG00CodesForG28Code(NcCode *code)
{
	//movement from tool current position to the intermediate point
		G00 *firstrapidmove = new G00(1000);
		firstrapidmove->ctype = CG00;
		
		firstrapidmove->setstartX(code->getstartX() * 2);  
		firstrapidmove->setstartZ(code->getstartZ());  

		firstrapidmove->setX(code->getX() * 2);
		firstrapidmove->setZ(code->getZ());

		firstrapidmove->setF(1000);
		firstrapidmove->setS(mCurrentSpindleSpeed);

		firstrapidmove->setAsAuxillaryPath(true);
		
		pushbackGCodeInLists(firstrapidmove);
		
		mCurrentCodeBlock->mBlockCodeList.push_back(firstrapidmove);   //push back G code in the current NcCodeBlock
		firstrapidmove->mAssociatedCodeBlock = mCurrentCodeBlock;



		//movement from intermediate point to the machine zero point
		G00 *secondrapidmove = new G00(1000);
		secondrapidmove->ctype = CG00;
		
		secondrapidmove->setstartX(code->getX() * 2);  
		secondrapidmove->setstartZ(code->getZ());  

		secondrapidmove->setX(mToolChangeXPos);
		secondrapidmove->setZ(mToolChangeZPos);

		secondrapidmove->setF(1000);
		secondrapidmove->setS(mCurrentSpindleSpeed);

		secondrapidmove->setAsAuxillaryPath(true);
		
		pushbackGCodeInLists(secondrapidmove);
		
		mCurrentCodeBlock->mBlockCodeList.push_back(secondrapidmove);   //push back G code in the current NcCodeBlock
		secondrapidmove->mAssociatedCodeBlock = mCurrentCodeBlock;

}



void	NcMachine::generateInitialRapidMoveToCutDepth(NcCode *code)
{
	if(code->ctype == CG90 || code->ctype == CG92)
	{
		G00 *firstrapidmove = new G00(1000);
		firstrapidmove->ctype = CG00;
		
		firstrapidmove->setstartX(code->getCycleStartX() * 2);  
		firstrapidmove->setstartZ(code->getCycleStartZ());  

		firstrapidmove->setX(code->getX() * 2);
		firstrapidmove->setZ(code->getCycleStartZ());

		setEndOfMotionXPosition(firstrapidmove->getX() * 2);
		setEndOfMotionZPosition(firstrapidmove->getZ());

		firstrapidmove->setF(1000);
		firstrapidmove->setS(mCurrentSpindleSpeed);

		firstrapidmove->setAsAuxillaryPath(true);
		
		pushbackGCodeInLists(firstrapidmove);
		
		mCurrentCodeBlock->mBlockCodeList.push_back(firstrapidmove);//push back G code in the current NcCodeBlock
		firstrapidmove->mAssociatedCodeBlock = mCurrentCodeBlock;
	}
	/****Pranit******/
	if(code->ctype == CG91)
	{
		G00 *firstrapidmove = new G00(1000);
		firstrapidmove->ctype = CG00;

		firstrapidmove->setstartX(code->getCycleStartX() * 2);  
		firstrapidmove->setstartZ(code->getCycleStartZ());  

		firstrapidmove->setX(code->getX() * 2);
		firstrapidmove->setZ(code->getCycleStartZ());

		setEndOfMotionXPosition(firstrapidmove->getX() * 2);
		setEndOfMotionZPosition(firstrapidmove->getZ());

		firstrapidmove->setF(1000);
		firstrapidmove->setS(mCurrentSpindleSpeed);

		firstrapidmove->setAsAuxillaryPath(true);

		pushbackGCodeInLists(firstrapidmove);

		mCurrentCodeBlock->mBlockCodeList.push_back(firstrapidmove);//push back G code in the current NcCodeBlock
		firstrapidmove->mAssociatedCodeBlock = mCurrentCodeBlock;
	}

	if(code->ctype == CG94)
	{
		G00 *firstrapidmove = new G00(1000);
		firstrapidmove->ctype = CG00;
		
		firstrapidmove->setstartX(code->getCycleStartX() * 2);  
		firstrapidmove->setstartZ(code->getCycleStartZ());  

		firstrapidmove->setX(code->getCycleStartX() * 2);
		firstrapidmove->setZ(code->getZ());

		setEndOfMotionXPosition(firstrapidmove->getX() * 2);
		setEndOfMotionZPosition(firstrapidmove->getZ());

		firstrapidmove->setF(1000);
		firstrapidmove->setS(mCurrentSpindleSpeed);

		firstrapidmove->setAsAuxillaryPath(true);
		
		pushbackGCodeInLists(firstrapidmove);
		
		mCurrentCodeBlock->mBlockCodeList.push_back(firstrapidmove);//push back G code in the current NcCodeBlock
		firstrapidmove->mAssociatedCodeBlock = mCurrentCodeBlock;
	}
}

void	NcMachine::generateEndOfCutFeedrateMove(NcCode *code)
{
	if(code->ctype == CG90 || code->ctype == CG92)
	{
		NcEndOfCutFeedRateMove *eocFeedRateMove = new NcEndOfCutFeedRateMove();
		eocFeedRateMove->ctype = CGAUX;

		eocFeedRateMove->setstartX(code->getX() * 2);  
		eocFeedRateMove->setstartZ(code->getZ());  

		eocFeedRateMove->setX(code->getCycleStartX() * 2);
		eocFeedRateMove->setZ(code->getZ());

		eocFeedRateMove->setF(mCurrentFeedRate);
		eocFeedRateMove->setS(mCurrentSpindleSpeed);

		setEndOfMotionXPosition(eocFeedRateMove->getX() * 2);
		setEndOfMotionZPosition(eocFeedRateMove->getZ());

		eocFeedRateMove->setAsAuxillaryPath(true);
		
		pushbackGCodeInLists(eocFeedRateMove);

		mCurrentCodeBlock->mBlockCodeList.push_back(eocFeedRateMove);//push back G code in the current NcCodeBlock
		eocFeedRateMove->mAssociatedCodeBlock = mCurrentCodeBlock;
	}
	/*****Pranit******/
	if(code->ctype == CG91)
	{
		NcEndOfCutFeedRateMove *eocFeedRateMove = new NcEndOfCutFeedRateMove();
		eocFeedRateMove->ctype = CGAUX;

		eocFeedRateMove->setstartX(code->getX() * 2);  
		eocFeedRateMove->setstartZ(code->getZ());  

		eocFeedRateMove->setX(code->getCycleStartX() * 2);
		eocFeedRateMove->setZ(code->getZ());

		eocFeedRateMove->setF(mCurrentFeedRate);
		eocFeedRateMove->setS(mCurrentSpindleSpeed);

		setEndOfMotionXPosition(eocFeedRateMove->getX() * 2);
		setEndOfMotionZPosition(eocFeedRateMove->getZ());

		eocFeedRateMove->setAsAuxillaryPath(true);

		pushbackGCodeInLists(eocFeedRateMove);

		mCurrentCodeBlock->mBlockCodeList.push_back(eocFeedRateMove);//push back G code in the current NcCodeBlock
		eocFeedRateMove->mAssociatedCodeBlock = mCurrentCodeBlock;
	}

	if(code->ctype == CG94)
	{
		NcEndOfCutFeedRateMove *eocFeedRateMove = new NcEndOfCutFeedRateMove();
		eocFeedRateMove->ctype = CGAUX;

		eocFeedRateMove->setstartX(code->getX() * 2);  
		eocFeedRateMove->setstartZ(code->getZ());  

		eocFeedRateMove->setX(code->getX() * 2);
		eocFeedRateMove->setZ(code->getCycleStartZ());

		eocFeedRateMove->setF(mCurrentFeedRate);
		eocFeedRateMove->setS(mCurrentSpindleSpeed);

		setEndOfMotionXPosition(eocFeedRateMove->getX() * 2);
		setEndOfMotionZPosition(eocFeedRateMove->getZ());

		eocFeedRateMove->setAsAuxillaryPath(true);
		
		pushbackGCodeInLists(eocFeedRateMove);

		mCurrentCodeBlock->mBlockCodeList.push_back(eocFeedRateMove);//push back G code in the current NcCodeBlock
		eocFeedRateMove->mAssociatedCodeBlock = mCurrentCodeBlock;
	}
}


void	NcMachine::generateRapidReturnToCycleStart(NcCode *code)
{
	if(code->ctype == CG90 || code->ctype == CG92)
	{
		NcAuxRapidMove *returnrapidmove = new NcAuxRapidMove(1000);
		returnrapidmove->ctype = CGAUX;
		
		returnrapidmove->setstartX(code->getCycleStartX() * 2);  
		returnrapidmove->setstartZ(code->getZ());  

		returnrapidmove->setX(code->getCycleStartX() * 2);
		returnrapidmove->setZ(code->getCycleStartZ());

		returnrapidmove->setF(1000);
		returnrapidmove->setS(mCurrentSpindleSpeed);

		returnrapidmove->setAsAuxillaryPath(true);

		setEndOfMotionXPosition(returnrapidmove->getX() * 2);
		setEndOfMotionZPosition(returnrapidmove->getZ());
		
		pushbackGCodeInLists(returnrapidmove);

		mCurrentCodeBlock->mBlockCodeList.push_back(returnrapidmove);//push back G code in the current NcCodeBlock
		returnrapidmove->mAssociatedCodeBlock = mCurrentCodeBlock;
	}

/******Pranit*******/
	if(code->ctype == CG91)
	{
		NcAuxRapidMove *returnrapidmove = new NcAuxRapidMove(1000);
		returnrapidmove->ctype = CGAUX;

		returnrapidmove->setstartX(code->getCycleStartX() * 2);  
		returnrapidmove->setstartZ(code->getZ());  

		returnrapidmove->setX(code->getCycleStartX() * 2);
		returnrapidmove->setZ(code->getCycleStartZ());

		returnrapidmove->setF(1000);
		returnrapidmove->setS(mCurrentSpindleSpeed);

		returnrapidmove->setAsAuxillaryPath(true);

		setEndOfMotionXPosition(returnrapidmove->getX() * 2);
		setEndOfMotionZPosition(returnrapidmove->getZ());

		pushbackGCodeInLists(returnrapidmove);

		mCurrentCodeBlock->mBlockCodeList.push_back(returnrapidmove);//push back G code in the current NcCodeBlock
		returnrapidmove->mAssociatedCodeBlock = mCurrentCodeBlock;
	}
	if(code->ctype == CG94)
	{
		NcAuxRapidMove *returnrapidmove = new NcAuxRapidMove(1000);
		returnrapidmove->ctype = CGAUX;
		
		returnrapidmove->setstartX(code->getX() * 2);  
		returnrapidmove->setstartZ(code->getCycleStartZ());  

		returnrapidmove->setX(code->getCycleStartX() * 2);
		returnrapidmove->setZ(code->getCycleStartZ());

		returnrapidmove->setF(1000);
		returnrapidmove->setS(mCurrentSpindleSpeed);

		returnrapidmove->setAsAuxillaryPath(true);

		setEndOfMotionXPosition(returnrapidmove->getX() * 2);
		setEndOfMotionZPosition(returnrapidmove->getZ());
		
		pushbackGCodeInLists(returnrapidmove);

		mCurrentCodeBlock->mBlockCodeList.push_back(returnrapidmove);//push back G code in the current NcCodeBlock
		returnrapidmove->mAssociatedCodeBlock = mCurrentCodeBlock;
	}
}

NcCode* NcMachine:: getGCodeInBlock(int indexOfLastGCodeLine)
{
	if(mNcCodeBlockList != NULL)
		return NULL;
	return(mNcCodeBlockList->size()>=(indexOfLastGCodeLine-1))? mNcCodeBlockList->at(indexOfLastGCodeLine)->getGCodeInBlock(): NULL;
}


// This function is used to send the machine tool to the absolute co-ordinate 
// This function sends the machine tool
void NcMachine::generateG00CodesForG53Code(NcCode *newcode)
{

	//movement to the machine zero point
	G00 *secondrapidmove = new G00(1000);
	secondrapidmove->ctype = CG00;

	secondrapidmove->setstartX(mEndOfMotionX);  
	secondrapidmove->setstartZ(mEndOfMotionZ);  

	secondrapidmove->setX(mEndOfMotionX);
	secondrapidmove->setZ(mEndOfMotionZ);

	/*secondrapidmove->setF(1000);
	secondrapidmove->setS(mCurrentSpindleSpeed);
	*/
	secondrapidmove->setAsAuxillaryPath(true);

	pushbackGCodeInLists(secondrapidmove);

	mCurrentCodeBlock->mBlockCodeList.push_back(secondrapidmove);   //push back G code in the current NcCodeBlock
	secondrapidmove->mAssociatedCodeBlock = mCurrentCodeBlock;
}