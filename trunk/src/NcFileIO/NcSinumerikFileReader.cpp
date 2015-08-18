#include <QFile>
#include <QStringList>
#include <QMessageBox>
#include "NcStateMachine\NcMachine.h"
#include "NcStateMachine\NcStartupStockDisplay.h"
#include "NcFileIO\NcSinumerikFileReader.h"
#include "NcDisplay\NcDisplay.h"
using namespace DiscreteSimulator;

NcSinumerikFileReader* NcSinumerikFileReader::mReaderInstance = 0;
int	NcSinumerikFileReader::mLineCounter = 0;

NcSinumerikFileReader*	NcSinumerikFileReader::getReaderInstance()
{
	if(mReaderInstance == 0)
	{
		mReaderInstance = new NcSinumerikFileReader();
	}
	return mReaderInstance;
}

NcSinumerikFileReader::NcSinumerikFileReader() : mFile(0)
{
	
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


NcSinumerikFileReader::~NcSinumerikFileReader()
{
	if(mFile != 0)
	{
		delete mFile;
		mFile = 0;
	}
}

QString		NcSinumerikFileReader::getFullNcCodeText()
{
	ncfilereader.seek(0);
	return mFile->readAll();
}


inline QFile* NcSinumerikFileReader::getCurrentNcFile()
{
	return mFile;
}


bool	NcSinumerikFileReader::openNcFileForReadWrite()
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
		
STATUS	NcSinumerikFileReader::checkCodeSyntax()
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


void NcSinumerikFileReader::checkEachLineForSyntax()
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
				case 70:		//set inch unit
					{
						buildG20Code();
						break;
					}
				case 71:		//set mm unit (metric units)
					{
						buildG21Code();
						break;
					}
				default:
					{
						QString message;
						message += tr("code ") + code + tr(" is invalid.") + tr( "\n") +
								tr("Please correct the NC file and restart the application");
 
						QMessageBox::StandardButton ret;
						ret = QMessageBox::warning(0,tr( "Discrete Simulator V1.0"),
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


void NcSinumerikFileReader::buildG20Code() //corresponds to G70 in sinumerik
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(INCHUNITMODE);
	NcMachine::NcMachineInstance()->buildGCodeList();
}

void NcSinumerikFileReader::buildG21Code() //corresponds to G71 in sinumerik
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(MMUNITMODE);
	NcMachine::NcMachineInstance()->buildGCodeList();
}


void NcSinumerikFileReader::buildMCode(QString code)		//building M Codes
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


void NcSinumerikFileReader::buildSCode(QString code)		//building S code when it is not with a machining command
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


void NcSinumerikFileReader::buildFCode(QString code)		//building F code when it is not with a machining command
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


void NcSinumerikFileReader::buildIndTCode(QString code)	//building T code when it is not with a machining command
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





STATUS NcSinumerikFileReader::checkSyntaxOfDwellCode(QStringList codelist)
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
		case 'F': //dwell time in secs : //S code for time in spindle revolutions not implemented
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

void NcSinumerikFileReader::buildG04Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(DWELL);

	if(mXValueDetected == true)
		NcMachine::NcMachineInstance()->mDwellTime = mDwellTime;

	NcMachine::NcMachineInstance()->buildGCodeList();
}



STATUS NcSinumerikFileReader::checkSyntaxOfCCWCirInterpol(QStringList codelist)
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


void NcSinumerikFileReader::buildG03Code()
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



STATUS NcSinumerikFileReader::checkSyntaxOfCWCirInterpol(QStringList codelist)
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

void NcSinumerikFileReader::buildG02Code() //format specifying CR value not implemented
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

STATUS NcSinumerikFileReader::checkSyntaxOfLinearInterpol(QStringList codelist)
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

		buildG01Code();
		mGCodeDetected = false;
		mSCodeDetected = false;
		mFCodeDetected = false;
		mXValueDetected = false;
		mZValueDetected = false;
		mTCodeDetected = false;

		return OK;
}

void NcSinumerikFileReader::buildG01Code()
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



STATUS NcSinumerikFileReader::checkSyntaxOfRapidCode(QStringList codelist)
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

		buildG00Code();
		mGCodeDetected = false;
		mSCodeDetected = false;
		mFCodeDetected = false;
		mXValueDetected = false;
		mZValueDetected = false;
		mTCodeDetected = false;

		return OK;
}

void NcSinumerikFileReader::buildG00Code()
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


void NcSinumerikFileReader::buildTCode()
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







void NcSinumerikFileReader::handleError(QString code)
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

void NcSinumerikFileReader::handleError(QStringList codelist)
{
	QString message;
	message += tr("Block ");
	foreach(QString code, codelist)
	{
		message += code + " ";
	}
	message += tr(" is invalid.") + tr("\n") +
			tr("Please correct the NC file and restart the application");

	QMessageBox::StandardButton ret;
	ret = QMessageBox::warning(0, tr("Discrete Simulator V1.0"),
								message, QMessageBox::Ok);

	if(ret == QMessageBox::Ok)
		exit(0);
}