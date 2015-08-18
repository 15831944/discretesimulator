#include <QFile>
#include <QStringList>
#include <QMessageBox>
#include "NcStateMachine\NcMachine.h"
#include "NcStateMachine\NcStartupStockDisplay.h"
#include "NcStateMachine\G74.h"
#include "NcFileIO\NcFanuc10T11TFileReader.h"
#include "NcDisplay\NcDisplay.h"
using namespace DiscreteSimulator;

NcFanuc10T11TFileReader* NcFanuc10T11TFileReader::mNcFileReaderInstance = 0;
int	NcFanuc10T11TFileReader::mLineCounter = 0;

NcFanuc10T11TFileReader*	NcFanuc10T11TFileReader::getReaderInstance()
{
	if(mNcFileReaderInstance == 0)
	{
		mNcFileReaderInstance = new NcFanuc10T11TFileReader();
	}
	return mNcFileReaderInstance;
}

NcFanuc10T11TFileReader::NcFanuc10T11TFileReader() : mFile(0)
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
	mDCodeDetected = false;
	mMachiningCodeDetected = false;
	mRetractCodeDetected = false;

	Rvalue = 0.0;
	PValue = 0.0;
	QValue = 0.0;
	RetractValue = 0.0;
	FValue = NcMachine::NcMachineInstance()->mCurrentFeedRate;
	SValue = NcMachine::NcMachineInstance()->mCurrentSpindleSpeed;
	XValue = -500;
	ZValue = -500;
	IValue = 0.0;
	KValue = 0.0;
	TValue = -1;
	DValue = 0.0;
	mDwellTime = 0.0;

	mLastExecutedCodeType  = UNKNOWN;
}


NcFanuc10T11TFileReader::~NcFanuc10T11TFileReader()
{
	if(mFile != 0)
	{
		delete mFile;
		mFile = 0;
	}
}

QString		NcFanuc10T11TFileReader::getFullNcCodeText()
{
	ncfilereader.seek(0);
	return mFile->readAll();
}


inline QFile* NcFanuc10T11TFileReader::getCurrentNcFile()
{
	return mFile;
}


bool	NcFanuc10T11TFileReader::openNcFileForReadWrite()
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
		
STATUS	NcFanuc10T11TFileReader::checkCodeSyntax()
{
	if(openNcFileForReadWrite() == true)
	{
		ncfilereader.setDevice(mFile);

		while(!ncfilereader.atEnd())
		{
			//read nc code line by line and put individual lines in the list for further checking
			QString codeblock = ncfilereader.readLine();      
			mCodeBlockList.push_back(codeblock);
		}	
		
		
		/*NcDisplay::getNcDisplayInstance()->displayStockProfile();
		NcDisplay::getNcDisplayInstance()->generateStartupDLForStock();*/
		
		for(; mLineCounter < mCodeBlockList.size();)
		{
			if(mLineCounter == 0)
				NcDisplay::getNcDisplayInstance()->setStockBBInitialValues();

			NcMachine::NcMachineInstance()->initializeCodeBlockInTheMachine(mCodeBlockList.at(mLineCounter), mLineCounter);
			checkEachLineForSyntax();			
		}

		NcDisplay::getNcDisplayInstance()->setStockBBFinalValues();
		NcDisplay::getNcDisplayInstance()->generateDisplayLists();
		NcStartupStockDisplay::getStockDisplayInstance()
				->setDLForStartupStock(NcDisplay::getNcDisplayInstance()->getStockDisplayListIndex());
	}
	return OK;
}


void NcFanuc10T11TFileReader::checkEachLineForSyntax()
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
						checkSyntaxOfPeckDrillCycle(codelist);						
						break;
					}
				case 75:
					{
						mMachiningCodeDetected = true;
						checkSyntaxOfPartingGrooving(codelist);					
						break;
					}
				case 76:
					{
						mMachiningCodeDetected = true;
						checkSyntaxOfMultiPassThreading(codelist);
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
						QString message;
						message += tr("code ") + code + tr(" is invalid.") + tr( "\n") +
								tr("Please correct the NC file and restart the application");
 
						QMessageBox::StandardButton ret;
						ret = QMessageBox::warning(0,tr("Discrete Simulator V1.0"),
													message, QMessageBox::Ok);

						if(ret == QMessageBox::Ok)
							exit(0);

					}
				}
				break;
			}

		case 'M':
			{
				buildMCode(code);
				break;
			}
		case 'S':
			{
				buildSCode(code);
				break;
			}
		case 'F':	//need to handle F, S and T here only when they are not with the machining code
			{
				buildFCode(code);
				break;
			}
		case 'T':	//if machining code is detected, then F, S and T are handled in the respective code function
			{
				buildIndTCode(code);
				break;
			}
		case 'X':
			{
				buildIndXCode(code);
				break;
			}
		case 'Z':
			{
				buildIndZCode(code);
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
	NcDisplay::getNcDisplayInstance()->CreateDisplayListsForGCodes();
	mLineCounter++;
	mMachiningCodeDetected = false;
}


void NcFanuc10T11TFileReader::buildIndXCode(QString code)
{
	bool ok = true;
	double newxvalue = code.right(code.size() - 1).toDouble(&ok);
	if(ok == false)
	{
		handleError(code);
	}

	XValue = newxvalue;

	if(mLastExecutedCodeType != UNKNOWN)
	{
		switch(mLastExecutedCodeType)
		{
		case CG90:
			{
				mTCodeDetected = true;
				mFCodeDetected = true;
				mSCodeDetected = true;
				mXValueDetected = true;
				mZValueDetected = true;
				mICodeDetected = true;
				buildG90Code();
				resetBoolValues();
				break;
			}
		case CG92:
			{
				mTCodeDetected = true;
				mFCodeDetected = true;
				mXValueDetected = true;
				mZValueDetected = true;
				buildG92Code();
				resetBoolValues();
				break;
			}
		}
	}
}


void NcFanuc10T11TFileReader::buildIndZCode(QString code)
{
	bool ok = true;
	double newzvalue = code.right(code.size() - 1).toDouble(&ok);
	if(ok == false)
	{
		handleError(code);
	}

	ZValue = newzvalue;

	if(mLastExecutedCodeType != UNKNOWN)
	{
		switch(mLastExecutedCodeType)
		{
		case CG94:
			{
				mTCodeDetected = true;
				mFCodeDetected = true;
				mSCodeDetected = true;
				mXValueDetected = true;
				mZValueDetected = true;
				buildG94Code();
				resetBoolValues();
				break;
			}
		}
	}
}


void NcFanuc10T11TFileReader::resetBoolValues()
{
	mTCodeDetected = false;
	mFCodeDetected = false;
	mSCodeDetected = false;
	mXValueDetected = false;
	mZValueDetected = false;
	mICodeDetected = false;
}


void NcFanuc10T11TFileReader::buildMCode(QString code)		//building M Codes
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


void NcFanuc10T11TFileReader::buildSCode(QString code)		//building S code when it is not with a machining command
{
	bool ok = true;
	double speed = code.right(code.size() - 1).toDouble(&ok);
	if(ok == false)
	{
		handleError(code);
	}
	NcMachine::NcMachineInstance()->mCurrentSpindleSpeed = speed;
	NcMachine::NcMachineInstance()->buildSCodes();
}


void NcFanuc10T11TFileReader::buildFCode(QString code)		//building F code when it is not with a machining command
{
	bool ok = true;

	double feedrate = code.right(code.size() - 1).toDouble(&ok);

	if(ok == false)
	{
		handleError(code);
	}

	NcMachine::NcMachineInstance()->mCurrentFeedRate = feedrate;
	NcMachine::NcMachineInstance()->buildFCodes();
}


void NcFanuc10T11TFileReader::buildIndTCode(QString code)	//building T code when it is not with a machining command
{
	bool ok = true;
	
	mTCodeDetected = true;
	TValue = code.right(code.size() - 1).toInt(&ok);

	if(ok == false)
	{
		handleError(code);
	}

	buildTCode();
	mTCodeDetected = false;
}


void NcFanuc10T11TFileReader::buildG20Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(INCHUNITMODE);
	NcMachine::NcMachineInstance()->buildGCodeList();
}

void NcFanuc10T11TFileReader::buildG21Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(MMUNITMODE);
	NcMachine::NcMachineInstance()->buildGCodeList();
}

void NcFanuc10T11TFileReader::buildG28Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(MACHINEZERORETURN);
	NcMachine::NcMachineInstance()->buildGCodeList();
}



STATUS NcFanuc10T11TFileReader::checkSyntaxOfMultiPassThreading(QStringList codelist)
{
	bool ok = true;

	QStringList codelist1 = mCodeBlockList.at(mLineCounter).split(" ", QString::SkipEmptyParts, Qt::CaseInsensitive);
	
	foreach(QString code, codelist1)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G':
			{
				int codenumber = code.right(code.size() - 1).toInt(&ok);
				if(codenumber != 76)
				{
					handleError(code);
				}
				break;
			}
		case 'X':		//final thread diameter
			{
				mXValueDetected = true;
				XValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Z':				//thread end (total length of thread)
			{
				mZValueDetected = true;
				ZValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'K':
			{
				mKCodeDetected = true;
				KValue = code.right(code.size() - 1).toDouble(&ok); 
				break;
			}
		case 'D':
			{
				mDCodeDetected = true;
				DValue = code.right(code.size() - 1).toDouble(&ok); 
				break;
			}
		case 'F':	//feed rate for thread cutting : pitch = feedrate for single start thread(in DS Pitch = Feedrate/2)
			{
				mFCodeDetected = true;
				FValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'N':
			{
				break;
			}
		case 'A':
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

	mGCodeDetected = false;
	mFCodeDetected = false;
	mXValueDetected = false;
	mZValueDetected = false;
	mDCodeDetected = false;
	mKCodeDetected = false;
	return OK;
}

void NcFanuc10T11TFileReader::buildG76Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(MULTIPASSTHREADING);

	if(mFCodeDetected == true)
		NcMachine::NcMachineInstance()->mCurrentFeedRate = FValue;

	if(mXValueDetected == true)
		NcMachine::NcMachineInstance()->setEndOfMotionXPosition(XValue);

	if(mZValueDetected == true)
		NcMachine::NcMachineInstance()->setEndOfMotionZPosition(ZValue);

	if(mKCodeDetected == true)  
		NcMachine::NcMachineInstance()->K = KValue;  

	if(mDCodeDetected == true)
		NcMachine::NcMachineInstance()->D = DValue;

	NcMachine::NcMachineInstance()->buildGCodeList();
}



STATUS NcFanuc10T11TFileReader::checkSyntaxOfSinglePassThreading(QStringList codelist)
{
	bool ok = true;

	foreach(QString code, codelist)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G':
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
			{
				mXValueDetected = true;
				XValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Z':
			{
				mZValueDetected = true;
				ZValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'F':
			{
				mFCodeDetected = true;
				FValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'T':
			{
				mTCodeDetected = true;
				TValue = code.right(code.size() - 1).toInt(&ok);
				break;
			}
		case 'N':
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
		mLastExecutedCodeType = CG92;
		mGCodeDetected = false;
		mFCodeDetected = false;
		mXValueDetected = false;
		mZValueDetected = false;
		mTCodeDetected = false;
		return OK;
}


void NcFanuc10T11TFileReader::buildG92Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(SINGLEPASSTHREADING);

	if(mTCodeDetected == true)
		buildTCode();

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


STATUS NcFanuc10T11TFileReader::checkSyntaxOfFacingCycle(QStringList codelist)
{
	bool ok = true;

	foreach(QString code, codelist)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G':
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
			{
				mXValueDetected = true;
				XValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Z':
			{
				mZValueDetected = true;
				ZValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'F':
			{
				mFCodeDetected = true;
				FValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'S':
			{
				mSCodeDetected = true;
				SValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'T':
			{
				mTCodeDetected = true;
				TValue = code.right(code.size() - 1).toInt(&ok);
				break;
			}
		case 'N':
			{
				break;
			}
		case 'K': //used for tapered facing : not implemented
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
		mLastExecutedCodeType = CG94;
		mGCodeDetected = false;
		mSCodeDetected = false;
		mFCodeDetected = false;
		mXValueDetected = false;
		mZValueDetected = false;
		mTCodeDetected = false;
		return OK;
}

void NcFanuc10T11TFileReader::buildG94Code()
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

STATUS NcFanuc10T11TFileReader::checkSyntaxOfCannedTurning(QStringList codelist)
{
	bool ok = true;

	foreach(QString code, codelist)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G':
			{
				mGCodeDetected = true;
				int codenumber = code.right(code.size() - 1).toInt(&ok);
				if(codenumber != 90)
				{
					handleError(code);
				}
				break;
			}
		case 'X':
			{
				mXValueDetected = true;
				XValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Z':
			{
				mZValueDetected = true;
				ZValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'F':
			{
				mFCodeDetected = true;
				FValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'S':
			{
				mSCodeDetected = true;
				SValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'T':
			{
				mTCodeDetected = true;
				TValue = code.right(code.size() - 1).toInt(&ok);
				break;
			}
		case 'I':
			{
				mICodeDetected = true;
				IValue = code.right(code.size() - 1).toDouble(&ok); 
				break;
			}
		case 'N':
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
		mLastExecutedCodeType = CG90;
		mGCodeDetected = false;
		mSCodeDetected = false;
		mFCodeDetected = false;
		mXValueDetected = false;
		mZValueDetected = false;
		mTCodeDetected = false;
		return OK;
}


void NcFanuc10T11TFileReader::buildG90Code()
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


STATUS NcFanuc10T11TFileReader::checkSyntaxOfDwellCode(QStringList codelist)
{
	bool ok = true;

	foreach(QString code, codelist)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G':
			{
				mGCodeDetected = true;
				int codenumber = code.right(code.size() - 1).toInt(&ok);
				if(codenumber != 4)
				{
					handleError(code);
				}
				break;
			}
		case 'X':
			{
				mXValueDetected = true;
				mDwellTime = code.right(code.size() - 1).toInt(&ok);
				break;
			}
		case 'N':
			{
				break;
			}
		default:
			{
				//invalid code in dwell block 
				//display error and abort file reading
				handleError(codelist);
			}
		}

		if(ok == false)
		{
			handleError(code);
		}
	}

	buildG04Code();
	mGCodeDetected = false;
	mXValueDetected = false;
	return OK;
}

void NcFanuc10T11TFileReader::buildG04Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(DWELL);

	if(mXValueDetected == true)
		NcMachine::NcMachineInstance()->mDwellTime = mDwellTime;

	NcMachine::NcMachineInstance()->buildGCodeList();
}



STATUS NcFanuc10T11TFileReader::checkSyntaxOfCCWCirInterpol(QStringList codelist)
{
	bool ok = true;

	foreach(QString code, codelist)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G':
			{
				mGCodeDetected = true;
				int codenumber = code.right(code.size() - 1).toInt(&ok);
				if(codenumber != 3)
				{
					handleError(code);
				}
				break;
			}
		case 'X':
			{
				mXValueDetected = true;
				XValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Z':
			{
				mZValueDetected = true;
				ZValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'F':
			{
				mFCodeDetected = true;
				FValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'S':
			{
				mSCodeDetected = true;
				SValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'T':
			{
				mTCodeDetected = true;
				TValue = code.right(code.size() - 1).toInt(&ok);
				break;
			}
		case 'I':
			{
				mICodeDetected = true;
				IValue = code.right(code.size() - 1).toDouble(&ok); 
				break;
			}
		case 'K':
			{
				mKCodeDetected = true;
				KValue = code.right(code.size() - 1).toDouble(&ok); 
				break;
			}
		case 'N':
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
		buildG03Code();
		mGCodeDetected = false;
		mSCodeDetected = false;
		mFCodeDetected = false;
		mXValueDetected = false;
		mZValueDetected = false;
		mTCodeDetected = false;
		mICodeDetected = false;
		mKCodeDetected = false;

		return OK;
}


void NcFanuc10T11TFileReader::buildG03Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(CCWCIRCULARINTERPOL);

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

	if(mKCodeDetected == true)
		NcMachine::NcMachineInstance()->K = KValue;

	NcMachine::NcMachineInstance()->buildGCodeList();
}



STATUS NcFanuc10T11TFileReader::checkSyntaxOfCWCirInterpol(QStringList codelist)
{
	bool ok = true;

	foreach(QString code, codelist)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G':
			{
				mGCodeDetected = true;
				int codenumber = code.right(code.size() - 1).toInt(&ok);
				if(codenumber != 2)
				{
					handleError(code);
				}
				break;
			}
		case 'X':
			{
				mXValueDetected = true;
				XValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Z':
			{
				mZValueDetected = true;
				ZValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'F':
			{
				mFCodeDetected = true;
				FValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'S':
			{
				mSCodeDetected = true;
				SValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'T':
			{
				mTCodeDetected = true;
				TValue = code.right(code.size() - 1).toInt(&ok);
				break;
			}
		case 'I':
			{
				mICodeDetected = true;
				IValue = code.right(code.size() - 1).toDouble(&ok); 
				break;
			}
		case 'K':
			{
				mKCodeDetected = true;
				KValue = code.right(code.size() - 1).toDouble(&ok); 
				break;
			}
		case 'N':
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

		buildG02Code();
		mGCodeDetected = false;
		mSCodeDetected = false;
		mFCodeDetected = false;
		mXValueDetected = false;
		mZValueDetected = false;
		mTCodeDetected = false;
		mICodeDetected = false;
		mKCodeDetected = false;
		return OK;
}

void NcFanuc10T11TFileReader::buildG02Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(CWCIRCULARINTERPOL);

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

	if(mKCodeDetected == true)
		NcMachine::NcMachineInstance()->K = KValue;

	NcMachine::NcMachineInstance()->buildGCodeList();
}

STATUS NcFanuc10T11TFileReader::checkSyntaxOfLinearInterpol(QStringList codelist)
{
	bool ok = true;

	foreach(QString code, codelist)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G':
			{
				mGCodeDetected = true;
				int codenumber = code.right(code.size() - 1).toInt(&ok);
				if(codenumber != 1)
				{
					handleError(code);
				}
				break;
			}
		case 'X':
			{
				mXValueDetected = true;
				XValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Z':
			{
				mZValueDetected = true;
				ZValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'F':
			{
				mFCodeDetected = true;
				FValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'S':
			{
				mSCodeDetected = true;
				SValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'T':
			{
				mTCodeDetected = true;
				TValue = code.right(code.size() - 1).toInt(&ok);
				break;
			}
		case 'N':
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

	buildG01Code();
	mGCodeDetected = false;
	mSCodeDetected = false;
	mFCodeDetected = false;
	mXValueDetected = false;
	mZValueDetected = false;
	mTCodeDetected = false;

	return OK;
}

void NcFanuc10T11TFileReader::buildG01Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(LINEARINTERPOL);

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



STATUS NcFanuc10T11TFileReader::checkSyntaxOfRapidCode(QStringList codelist)
{
	bool ok = true;

	foreach(QString code, codelist)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G':
			{
				mGCodeDetected = true;
				int codenumber = code.right(code.size() - 1).toInt(&ok);
				if(codenumber != 0)
				{
					handleError(code);
				}
				break;
			}
		case 'X':
			{
				mXValueDetected = true;
				XValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Z':
			{
				mZValueDetected = true;
				ZValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'F':
			{
				mFCodeDetected = true;
				FValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'S':
			{
				mSCodeDetected = true;
				SValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'T':
			{
				mTCodeDetected = true;
				TValue = code.right(code.size() - 1).toInt(&ok);
				break;
			}
		case 'N':
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

	buildG00Code();
	mGCodeDetected = false;
	mSCodeDetected = false;
	mFCodeDetected = false;
	mXValueDetected = false;
	mZValueDetected = false;
	mTCodeDetected = false;

	return OK;
}

void NcFanuc10T11TFileReader::buildG00Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(RAPIDMOTION);

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


void NcFanuc10T11TFileReader::buildTCode()
{
	switch(TValue)
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

	NcMachine::NcMachineInstance()->buildTCodes();
}


STATUS NcFanuc10T11TFileReader::checkSyntaxOfPeckDrillCycle(QStringList codelist)
{
	bool ok = true;

	QStringList codelist1 = mCodeBlockList.at(mLineCounter).split(" ", QString::SkipEmptyParts, Qt::CaseInsensitive);
	
	//second line must have G74 code 
	foreach(QString code, codelist1)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G':
			{
				int codenumber = code.right(code.size() - 1).toInt(&ok);
				if(codenumber != 74)
				{
					handleError(code);
				}
				break;
			}
		case 'F':
			{
				mFCodeDetected = true;
				FValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'S':
			{
				mSCodeDetected = true;
				SValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Z':
			{
				mZValueDetected = true;
				ZValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'X':
			{
				mXValueDetected = true;
				XValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'K':
			{
				mKCodeDetected = true;
				KValue = code.right(code.size() - 1).toDouble(&ok); 
				break;
			}
		case 'N':
			{
				break;
			}
		case 'I':	//depth of each cut : not implemented (usually not used)
			{
				break;
			}
		case 'D': //relief amount at the end of cut(must be 0 for face grooving) : not implemented
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
	mRCodeDetected = false;
	mSCodeDetected = false;
	mFCodeDetected = false;
	mXValueDetected = false;
	mZValueDetected = false;
	mKCodeDetected = false;
	return OK;
}




void NcFanuc10T11TFileReader::buildG74Code()
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

	if(mKCodeDetected == true)  //K Value in 2 line format is equal to Q value in single line format
		NcMachine::NcMachineInstance()->K = KValue;  

	NcMachine::NcMachineInstance()->buildGCodeList();
}



STATUS NcFanuc10T11TFileReader::checkSyntaxOfPartingGrooving(QStringList codelist)
{
	bool ok = true;

	QStringList codelist1 = mCodeBlockList.at(mLineCounter).split(" ", QString::SkipEmptyParts, Qt::CaseInsensitive);
	
	foreach(QString code, codelist1)
	{
		char codeCharacter = code.at(0).toAscii();
		switch(codeCharacter)
		{
		case 'G':
			{
				int codenumber = code.right(code.size() - 1).toInt(&ok);
				if(codenumber != 75)
				{
					handleError(code);
				}
				break;
			}
		case 'F':
			{
				mFCodeDetected = true;
				FValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'S':
			{
				mSCodeDetected = true;
				SValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'X':
			{
				mXValueDetected = true;
				XValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'Z':
			{
				mZValueDetected = true;
				ZValue = code.right(code.size() - 1).toDouble(&ok);
				break;
			}
		case 'I':
			{
				mICodeDetected = true;
				IValue = code.right(code.size() - 1).toDouble(&ok); 
				break;
			}
		case 'N':
			{
				break;
			}
		case 'K': //distance between grooves (for multiple grooves only) : not implemented
			{
				break;
			}
		case 'D': //relief amount at the end of cut : not implemented
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

	buildG75Code(); 
	mRCodeDetected = false;
	mSCodeDetected = false;
	mFCodeDetected = false;
	mXValueDetected = false;
	mZValueDetected = false;
	mICodeDetected = false;
	mRetractCodeDetected = false;
	return OK;
}


void NcFanuc10T11TFileReader::buildG75Code()
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

	if(mICodeDetected == true)  
		NcMachine::NcMachineInstance()->I = IValue;  

	NcMachine::NcMachineInstance()->buildGCodeList();
}



void NcFanuc10T11TFileReader::handleError(QString code)
{
	QString message;
	message += tr("code ") + code + tr(" is invalid.") + tr("\n") +
			tr("Please correct the NC file and restart the application");

	QMessageBox::StandardButton ret;
	ret = QMessageBox::warning(0,tr("Discrete Simulator V1.0"),
								message, QMessageBox::Ok);

	if(ret == QMessageBox::Ok)
		exit(0);
}

void NcFanuc10T11TFileReader::handleError(QStringList codelist)
{
	QString message;
	message += tr("Block ");
	foreach(QString code, codelist)
	{
		message += code + tr(" ");
	}
	message += tr(" is invalid.") + tr("\n") +
			tr("Please correct the NC file and restart the application");

	QMessageBox::StandardButton ret;
	ret = QMessageBox::warning(0, tr("Discrete Simulator V1.0"),
								message, QMessageBox::Ok);

	if(ret == QMessageBox::Ok)
		exit(0);
}