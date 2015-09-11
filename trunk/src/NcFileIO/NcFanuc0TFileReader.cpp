#include <QFile>
#include <QStringList>
#include "NcStateMachine\NcMachine.h"
#include "NcStateMachine\NcStartupStockDisplay.h"
#include "NcStateMachine\G74.h"
#include "NcFileIO\NcFanuc0TFileReader.h"
#include "NcDisplay\NcDisplay.h"
using namespace DiscreteSimulator;



NcFanuc0TFileReader::NcFanuc0TFileReader(QString filename) 
	:NcFileReader(filename)
{
	mRCodeDetected = false;
	mSCodeDetected = false;
	mFCodeDetected = false;
	mXValueDetected = false;
	mZValueDetected = false;
	mQCodeDetected = false;
	mTCodeDetected = false;
	mICodeDetected = false;
	mKCodeDetected = false;
	mPCodeDetected = false;
	mMachiningCodeDetected = false;
	mRetractCodeDetected = false;

}


	
static int linecount = 0;


void NcFanuc0TFileReader::checkEachLineForSyntax()
{
	QStringList codelist = mCodeBlockList.at(mLineCounter).split(" ", QString::SkipEmptyParts, Qt::CaseInsensitive);
	
	//check individual codes in the list 
	//for conflicting G codes
	foreach(QString code, codelist)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G':  
		case 'g':
			{
				int codenumber = code.right(code.size() - 1).toInt();
				switch(codenumber)
				{
				case 0:
					{
						mMachiningCodeDetected = true;
						checkSyntaxOfRapidCode(codelist);
						break;
					}
				case 1:
					{
						mMachiningCodeDetected = true;
						checkSyntaxOfLinearInterpol(codelist);
						break;
					}
				case 2:
					{
						mMachiningCodeDetected = true;
						checkSyntaxOfCWCirInterpol(codelist);
						break;
					}
				case 3:
					{
						mMachiningCodeDetected = true;
						checkSyntaxOfCCWCirInterpol(codelist);
						break;
					}
				case 4:
					{
						checkSyntaxOfDwellCode(codelist);
						break;
					}
				case 20:		//set inch unit
					{
						buildG20Code();
						break;
					}
				case 21:		//set mm unit
					{
						buildG21Code();
						break;
					}
				case 28:		//machine zero return --- X and Z co-ordinates in the code denotes the intermediate point
					{
						buildG28Code();
						break;
					}
				case 74:
					{
						mMachiningCodeDetected = true;
						mFirstG74CodeDetected = true;
						checkSyntaxOfPeckDrillCycle(codelist);
						mFirstG74CodeDetected = false;
						break;
					}
				case 75:
					{
						mMachiningCodeDetected = true;
						mFirstG75CodeDetected = true;
						checkSyntaxOfPartingGrooving(codelist);
						mFirstG75CodeDetected = false;
						break;
					}
				case 76:
					{
						mMachiningCodeDetected = true;
						mFirstG76CodeDetected = true;
						checkSyntaxOfMultiPassThreading(codelist);
						mFirstG76CodeDetected = false;
						break;
					}
				case 90:
					{
						mMachiningCodeDetected = true;
						checkSyntaxOfCannedTurning(codelist);
						break;
					}
				case 92:
					{
						mMachiningCodeDetected = true;
						checkSyntaxOfSinglePassThreading(codelist);
						break;
					}
				case 94:
					{
						mMachiningCodeDetected = true;
						checkSyntaxOfFacingCycle(codelist);
						break;
					}
				default:
					{
						handleError(code);
					}
				}
				break;
			}

		case 'M': 
		case 'm':
			{
				buildMCode(code);
				break;
			}
		case 'S': 
		case 's':
			{
				buildSCode(code);
				break;
			}
		case 'F': 
		case 'f':	//need to handle F, S and T here only when they are not with the machining code
			{
				buildFCode(code);
				break;
			}
		case 'T': 
		case 't':	//if machining code is detected, then F, S and T are handled in the respective code function
			{
				buildIndTCode(code);
				break;
			}
			//break when a machining code is detected from this switch as well as the for loop
			//since blocks including machining codes are checked in their respective functions
			//continue only when there is no machining code detected and there are only auxillary
			//codes in the block
			
			if(mMachiningCodeDetected == true)
				break;
		}

		if(mMachiningCodeDetected == true) //breaking from the for loop when machining code is detected
			break;
	}
	NcDisplay::getNcDisplayInstance()->updateStockBoundingBox();
	mLineCounter++;
	mMachiningCodeDetected = false;
}

void NcFanuc0TFileReader::buildMCode(QString code)		//building M Codes
{
	bool ok = true;

	int mcode = code.right(code.size() - 1).toInt(&ok);

	if(ok == false)
	{
		handleError(code);
	}

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
	default:
		{
			handleError(code);
		}
	}
	NcMachine::NcMachineInstance()->buildMCodes();
}

void NcFanuc0TFileReader::buildG28Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(MACHINEZERORETURN);
	NcMachine::NcMachineInstance()->buildGCodeList();
}

STATUS NcFanuc0TFileReader::checkSyntaxOfMultiPassThreading(QStringList codelist)
{
	bool ok = true;

	if(mFirstG76CodeDetected == true)
	{
		if(codelist.size() > 5 /*more than N, G and R code present*/ ||
		   codelist.size() < 2 /* only G code present*/)
		{
			//G74 first line can only have R code, so at max only three codes N, G and R are allowed
		}

		foreach(QString code, codelist)
		{
			char codeCharacter = code.at(0).toAscii();
			switch(codeCharacter)
			{
			case 'G': 
			case 'g':
				{
					break;
				}
			case 'P': 
			case 'p':
				{
					break;
				}
			case 'Q':  
			case 'q':
				{
					break;
				}
			case 'R':
			case 'r':
				{
					/*mRCodeDetected = true;
					Rvalue = code.right(code.size() - 1).toDouble();*/
					break;
				}
			case 'N': 
			case 'n':
				{
					break;
				}
			default:
				{
					//invalid code in the first line of G74
					handleError(codelist);
				}
			}
			if(ok == false)
			{
				handleError(code);
			}			
		}
		
		checkSecondLineForG76();
	}
	return OK;
}

STATUS NcFanuc0TFileReader::checkSecondLineForG76()
{
	bool ok = true;

	mLineCounter++;

	QStringList codelist = mCodeBlockList.at(mLineCounter).split(" ", QString::SkipEmptyParts, Qt::CaseInsensitive);
	
	//second line must have G76 code 
	foreach(QString code, codelist)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G': 
		case 'g':
			{
				int codenumber = code.right(code.size() - 1).toInt(&ok);
				if(codenumber != 76)
				{
					handleError(code);
				}
				break;
			}
		case 'P': 
		case 'p': //single depth of thread = k of single line format
			{
				mPCodeDetected = true;
				PValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Q': 
		case 'q': //depth of the first threading pass = D of single line format
			{
				mQCodeDetected = true;
				QValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'R':
		case 'r':
			{
				mRetractCodeDetected = true;
				RetractValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'F': 
		case 'f':	//feed rate for thread cutting : pitch = feedrate for single start thread(in DS Pitch = Feedrate/2)
			{
				mFCodeDetected = true;
				FValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'X':
		case 'x' :		//final thread diameter
			{
				mXValueDetected = true;
				XValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Z':  
		case 'z' :				//thread end (total length of thread)
			{
				mZValueDetected = true;
				ZValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'N': 
		case 'n':
			{
				break;
			}
		default:
			{
				//invalid G76 code: line contains non-permissible code
				//display error message and stop reading the file
				handleError(codelist);
			}
		}

		if(ok == false)
		{
			handleError(code);
		}		
	}

	if(mXValueDetected == false)
	{
		XValue = NcMachine::NcMachineInstance()->mEndOfMotionX;
		mXValueDetected = true;
	}
	if(mZValueDetected == false)
	{
		ZValue = NcMachine::NcMachineInstance()->mEndOfMotionZ;
		mZValueDetected = true;
	}

	buildG76Code();  //G76 code is correct: build the code here

	return OK;
}

void NcFanuc0TFileReader::buildG76Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(MULTIPASSTHREADING);

	if(mFCodeDetected == true)
		NcMachine::NcMachineInstance()->mCurrentFeedRate = FValue;

	if(mXValueDetected == true)
		NcMachine::NcMachineInstance()->setEndOfMotionXPosition(XValue);

	if(mZValueDetected == true)
		NcMachine::NcMachineInstance()->setEndOfMotionZPosition(ZValue);

	if(mQCodeDetected == true)  
		NcMachine::NcMachineInstance()->D = QValue;  

	if(mPCodeDetected == true)
		NcMachine::NcMachineInstance()->K = PValue;

	NcMachine::NcMachineInstance()->buildGCodeList();

	/*mRCodeDetected = false;*/
	mFCodeDetected = false;
	mXValueDetected = false;
	mZValueDetected = false;
	mQCodeDetected = false;
	mPCodeDetected = false;
	/*mRetractCodeDetected = false;*/
}

STATUS NcFanuc0TFileReader::checkSyntaxOfSinglePassThreading(QStringList codelist)
{
	bool ok = true;

	foreach(QString code, codelist)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G': 
		case 'g':
			{
				mGCodeDetected = true;
				int codenumber = code.right(code.size() - 1).toInt(&ok);
				if(codenumber != 92)
				{
					handleError(code);
				}
				break;
			}
		case 'X':
		case 'x' :
			{
				mXValueDetected = true;
				XValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Z':
		case 'z' :
			{
				mZValueDetected = true;
				ZValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'F': 
		case 'f':
			{
				mFCodeDetected = true;
				FValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'N': 
		case 'n':
			{
				break;
			}
		default:
			{
				//invalid code on the line
				//display error and abort file reading
				handleError(codelist);
			}
		}

		if(ok == false)
		{
			handleError(code);
		}
	}

		buildG92Code();
		mGCodeDetected = false;
		mFCodeDetected = false;
		mXValueDetected = false;
		mZValueDetected = false;
		return OK;
}

void NcFanuc0TFileReader::buildG92Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(SINGLEPASSTHREADING);

	if(mFCodeDetected == true)
	{
		NcMachine::NcMachineInstance()->mCurrentFeedRate = FValue;
		NcMachine::NcMachineInstance()->buildFCodes();
	}

	if(mXValueDetected == true)
		NcMachine::NcMachineInstance()->setEndOfMotionXPosition(XValue);

	if(mZValueDetected == true)
		NcMachine::NcMachineInstance()->setEndOfMotionZPosition(ZValue);

	NcMachine::NcMachineInstance()->buildGCodeList();
}

STATUS NcFanuc0TFileReader::checkSyntaxOfFacingCycle(QStringList codelist)
{
	bool ok = true;

	foreach(QString code, codelist)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G':  
		case 'g':
			{
				mGCodeDetected = true;
				int codenumber = code.right(code.size() - 1).toInt(&ok);
				if(codenumber != 94)
				{
					handleError(code);
				}
				break;
			}
		case 'X': 
		case 'x' :
			{
				mXValueDetected = true;
				XValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Z':  
		case 'z' :
			{
				mZValueDetected = true;
				ZValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'F': 
		case 'f':
			{
				mFCodeDetected = true;
				FValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'S':  
		case 's':
			{
				mSCodeDetected = true;
				SValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'T':
		case 't':
			{
				mTCodeDetected = true;
				TValue = code.right(code.size() - 1).toInt(&ok);
				break;
			}
		case 'N':
		case 'n':
			{
				break;
			}
		case 'K': 
		case 'k': //used for tapered facing : not implemented
			{
				break;
			}
		default:
			{
				//invalid code on the line
				//display error and abort file reading
				handleError(codelist);
			}
		}

		if(ok == false)
		{
			handleError(code);
		}
	}

		buildG94Code();
		mGCodeDetected = false;
		mSCodeDetected = false;
		mFCodeDetected = false;
		mXValueDetected = false;
		mZValueDetected = false;
		mTCodeDetected = false;
		return OK;
}

void NcFanuc0TFileReader::buildG94Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(FACING);

	if(mTCodeDetected == true)
		buildTCode();

	if(mFCodeDetected == true)
	{
		NcMachine::NcMachineInstance()->mCurrentFeedRate = FValue;
		NcMachine::NcMachineInstance()->buildFCodes();
	}

	if(mSCodeDetected == true)
	{
		NcMachine::NcMachineInstance()->mCurrentSpindleSpeed = SValue;
		NcMachine::NcMachineInstance()->buildSCodes();
	}

	if(mXValueDetected == true)
		NcMachine::NcMachineInstance()->setEndOfMotionXPosition(XValue);

	if(mZValueDetected == true)
		NcMachine::NcMachineInstance()->setEndOfMotionZPosition(ZValue);

	NcMachine::NcMachineInstance()->buildGCodeList();
}

STATUS NcFanuc0TFileReader::checkSyntaxOfCannedTurning(QStringList codelist)
{
	bool ok = true;

	foreach(QString code, codelist)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G': 
		case 'g':
			{
				//if any other G code than G90 is present on the same line, then display the error and 
				//stop reading the file. Only one machining / valid Auxillary G code could be present
				//in the single block of NC file
				mGCodeDetected = true;
				int codenumber = code.right(code.size() - 1).toInt(&ok);
				if(codenumber != 90)
				{
					handleError(code);
				}
				break;
			}
		case 'X': 
		case 'x' :
			{
				mXValueDetected = true;
				XValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Z':
		case 'z' :
			{
				mZValueDetected = true;
				ZValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'F': 
		case 'f':
			{
				mFCodeDetected = true;
				FValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'S': 
		case 's':
			{
				mSCodeDetected = true;
				SValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'T':
		case 't':
			{
				mTCodeDetected = true;
				TValue = code.right(code.size() - 1).toInt(&ok);
				break;
			}
		case 'I':  
		case 'i':
			{
				mICodeDetected = true;
				IValue = code.right(code.size() - 1).toDouble(&ok); 
				break;
			}
		case 'N': 
		case 'n':
			{
				break;
			}
		default:
			{
				//invalid code on the line
				//display error and abort file reading
				handleError(codelist);
			}
		}

		if(ok == false)
		{
			handleError(code);
		}
	}

		buildG90Code();
		mGCodeDetected = false;
		mSCodeDetected = false;
		mFCodeDetected = false;
		mXValueDetected = false;
		mZValueDetected = false;
		mTCodeDetected = false;
		return OK;
}

void NcFanuc0TFileReader::buildG90Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(CANNEDTURNING);

	if(mTCodeDetected == true)
		buildTCode();

	if(mFCodeDetected == true)
	{
		NcMachine::NcMachineInstance()->mCurrentFeedRate = FValue;
		NcMachine::NcMachineInstance()->buildFCodes();
	}

	if(mSCodeDetected == true)
	{
		NcMachine::NcMachineInstance()->mCurrentSpindleSpeed = SValue;
		NcMachine::NcMachineInstance()->buildSCodes();
	}

	if(mXValueDetected == true)
		NcMachine::NcMachineInstance()->setEndOfMotionXPosition(XValue);

	if(mZValueDetected == true)
		NcMachine::NcMachineInstance()->setEndOfMotionZPosition(ZValue);

	if(mICodeDetected == true)
		NcMachine::NcMachineInstance()->I = IValue;

	NcMachine::NcMachineInstance()->buildGCodeList();
}

STATUS NcFanuc0TFileReader::checkSyntaxOfPeckDrillCycle(QStringList codelist)
{
	bool ok = true;

	if(mFirstG74CodeDetected == true)
	{
		if(codelist.size() > 3 /*more than N, G and R code present*/ ||
		   codelist.size() < 2 /* only G code present*/)
		{
			//G74 first line can only have R code, so at max only three codes N, G and R are allowed
		}

		foreach(QString code, codelist)
		{
			char codeCharacter = code.at(0).toAscii();
			switch(codeCharacter)
			{
			case 'G':  
			case 'g':
				{
					break;
				}
			case 'R':
				{
					mRCodeDetected = true;
					Rvalue = code.right(code.size() - 1).toDouble(&ok);
					checkSecondLineForG74();
					break;
				}
			case 'N':
			case 'n':
				{
					break;
				}
			default:
				{
					//invalid code in the first line of G74
					handleError(codelist);
				}
			}

			if(ok == false)
			{
				handleError(code);
			}
		}
	}
	return OK;
}

STATUS NcFanuc0TFileReader::checkSecondLineForG74()
{
	bool ok = true;

	mLineCounter++;

	QStringList codelist = mCodeBlockList.at(mLineCounter).split(" ", QString::SkipEmptyParts, Qt::CaseInsensitive);
	
	//second line must have G74 code 
	foreach(QString code, codelist)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G':  
		case 'g':
			{
				int codenumber = code.right(code.size() - 1).toInt();
				if(codenumber != 74)
				{
					handleError(code);
				}
				break;
			}
		case 'P': 
		case 'p':
			{
				PValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Q': 
		case 'q':
			{
				//equal to K code in single line format
				mQCodeDetected = true;
				QValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'R':
		case 'r':
			{
				mRetractCodeDetected = true;
				RetractValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'F':  
		case 'f':
			{
				mFCodeDetected = true;
				FValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'S': 
		case 's':
			{
				mSCodeDetected = true;
				SValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'X':
		case 'x' :
			{
				mXValueDetected = true;
				XValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Z':
		case 'z' :
			{
				mZValueDetected = true;
				ZValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'N':
		case 'n':
			{
				break;
			}
		default:
			{
				//invalid G74 code: line contains non-permissible code
				//display error message and stop reading the file
				handleError(codelist);
			}
		}
		
		if(ok == false)
		{
			handleError(code);
		}
	}

	if(mXValueDetected == false)
	{
		XValue = NcMachine::NcMachineInstance()->mEndOfMotionX;
		mXValueDetected = true;
	}
	if(mZValueDetected == false)
	{
		ZValue = NcMachine::NcMachineInstance()->mEndOfMotionZ;
		mZValueDetected = true;
	}

	buildG74Code(); //G74 code is correct: build the code here

	return OK;
}

void NcFanuc0TFileReader::buildG74Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(PECKDRILLING);

	if(mSCodeDetected == true)
		NcMachine::NcMachineInstance()->mCurrentSpindleSpeed = SValue;

	if(mFCodeDetected == true)
		NcMachine::NcMachineInstance()->mCurrentFeedRate = FValue;

	if(mXValueDetected == true)
		NcMachine::NcMachineInstance()->setEndOfMotionXPosition(XValue);

	if(mZValueDetected == true)
		NcMachine::NcMachineInstance()->setEndOfMotionZPosition(ZValue);

	if(mQCodeDetected == true)  //Q Value in 2 line format is equal to K value in single line format
		NcMachine::NcMachineInstance()->K = QValue;  

	NcMachine::NcMachineInstance()->buildGCodeList();

	mRCodeDetected = false;
	mSCodeDetected = false;
	mFCodeDetected = false;
	mXValueDetected = false;
	mZValueDetected = false;
	mQCodeDetected = false;
	mRetractCodeDetected = false;
}

STATUS NcFanuc0TFileReader::checkSyntaxOfPartingGrooving(QStringList codelist)
{
	bool ok = true;

	if(mFirstG75CodeDetected == true)
	{
		if(codelist.size() > 3 /*more than N, G and R code present*/ ||
		   codelist.size() < 2 /* only G code present*/)
		{
			//G75 first line can only have R code, so at max only three codes N, G and R are allowed
		}

		foreach(QString code, codelist)
		{
			char codeCharacter = code.at(0).toAscii();
			switch(codeCharacter)
			{
			case 'G': 
			case 'g':
				{
					break;
				}
			case 'R':
			case 'r':
				{
					mRCodeDetected = true;
					Rvalue = code.right(code.size() - 1).toDouble(&ok);
					checkSecondLineForG75();
					break;
				}
			case 'N':
			case 'n':
				{
					break;
				}
			default:
				{
					//invalid code in the first line of G74
					handleError(codelist);
				}
			}

			if(ok == false)
			{
				handleError(code);
			}
		}

	}
	return OK;
}

STATUS NcFanuc0TFileReader::checkSecondLineForG75()
{
	bool ok = true;

	mLineCounter++;

	QStringList codelist = mCodeBlockList.at(mLineCounter).split(" ", QString::SkipEmptyParts, Qt::CaseInsensitive);
	
	//second line must have G75 code 
	foreach(QString code, codelist)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G':
		case 'g':
			{
				int codenumber = code.right(code.size() - 1).toInt(&ok);
				if(codenumber != 75)
				{
					handleError(code);
				}
				break;
			}
		case 'P': 
		case 'p':
			{
				PValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Q':  
		case 'q':
			{
				//equal to K code in single line format
				mQCodeDetected = true;
				QValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'R':
		case 'r':
			{
				mRetractCodeDetected = true;
				RetractValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'F':  
		case 'f':
			{
				mFCodeDetected = true;
				FValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'S':  
		case 's':
			{
				mSCodeDetected = true;
				SValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'X':
		case 'x' :
			{
				mXValueDetected = true;
				XValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Z': 
		case 'z' :
			{
				mZValueDetected = true;
				ZValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'N': 
		case 'n':
			{
				break;
			}
		default:
			{
				//invalid G75 code: line contains non-permissible code
				//display error message and stop reading the file
				handleError(codelist);
			}
		}

		if(ok == false)
		{
			handleError(code);
		}
	}

	if(mXValueDetected == false)
	{
		XValue = NcMachine::NcMachineInstance()->mEndOfMotionX;
		mXValueDetected = true;
	}
	if(mZValueDetected == false)
	{
		ZValue = NcMachine::NcMachineInstance()->mEndOfMotionZ;
		mZValueDetected = true;
	}

	buildG75Code(); //G75 code is correct: build the code here

	return OK;
}

void NcFanuc0TFileReader::buildG75Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(PARTINGGROOVING);

	if(mSCodeDetected == true)
		NcMachine::NcMachineInstance()->mCurrentSpindleSpeed = SValue;

	if(mFCodeDetected == true)
		NcMachine::NcMachineInstance()->mCurrentFeedRate = FValue;

	if(mXValueDetected == true)
		NcMachine::NcMachineInstance()->setEndOfMotionXPosition(XValue);

	if(mZValueDetected == true)
		NcMachine::NcMachineInstance()->setEndOfMotionZPosition(ZValue);

	if(mQCodeDetected == true)  //Q Value in 2 line format is equal to K value in single line format
		NcMachine::NcMachineInstance()->I = QValue;  

	NcMachine::NcMachineInstance()->buildGCodeList();

	mRCodeDetected = false;
	mSCodeDetected = false;
	mFCodeDetected = false;
	mXValueDetected = false;
	mZValueDetected = false;
	mQCodeDetected = false;
	mRetractCodeDetected = false;
}
