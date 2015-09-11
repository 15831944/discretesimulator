#include <QFile>
#include <QStringList>
#include "NcStateMachine\NcMachine.h"
#include "NcFileIO\NcFileReader.h"
#include "NcDisplay\NcDisplay.h"

using namespace DiscreteSimulator;

int	NcFileReader::mLineCounter = 0;

NcFileReader::NcFileReader(QString filename):
mCurrentFileName(filename)
{
	
}

QString	NcFileReader::getFullNcCodeText()
{
	QString text = "";
	if(mCurrentFileName.isEmpty())
		return text;
	
	QFile mFile(mCurrentFileName);
	if(!mFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return text;
	text = mFile.readAll();

	return text;
}

STATUS	NcFileReader::checkCodeSyntax()
{
	if(mCurrentFileName.isEmpty())
		return FAIL;

	
	QFile mFile(mCurrentFileName);
	if(!mFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return FAIL;
	ncfilereader.setDevice(&mFile);

	while(!ncfilereader.atEnd())
	{
		//read nc code line by line and put individual lines in the list for further checking
		QString codeblock = ncfilereader.readLine();      
		mCodeBlockList.push_back(codeblock);
	}	
		
	for(; mLineCounter < mCodeBlockList.size();)
	{
		if(mLineCounter == 0)
			NcDisplay::getNcDisplayInstance()->setStockBBInitialValues();

		NcMachine::NcMachineInstance()->initializeCodeBlockInTheMachine(mCodeBlockList.at(mLineCounter), mLineCounter);
		checkEachLineForSyntax();			
	}
		return OK;
}


void NcFileReader::buildSCode(QString code)		//building S code when it is not with a machining command
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

void NcFileReader::buildFCode(QString code)		//building F code when it is not with a machining command
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

void NcFileReader::buildIndTCode(QString code)	//building T code when it is not with a machining command
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

void NcFileReader::buildG20Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(INCHUNITMODE);
	NcMachine::NcMachineInstance()->buildGCodeList();
}

void NcFileReader::buildG21Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(MMUNITMODE);
	NcMachine::NcMachineInstance()->buildGCodeList();
}

STATUS NcFileReader::checkSyntaxOfDwellCode(QStringList codelist)
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
				if(codenumber != 4)
				{
					handleError(code);
				}
				break;
			}
		case 'X': 
		case 'x' :
			{
				mXValueDetected = true;
				mDwellTime = code.right(code.size() - 1).toInt(&ok);
				break;
			}
		case 'N': 
		case 'n':
			{
				/*mNCodeDetected = true;
				NValue = code.right(code.size() - 1).toDouble(&ok);*/
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

void NcFileReader::buildG04Code()
{
	NcMachine::NcMachineInstance()->setMachineOperationalMode(DWELL);

	if(mXValueDetected == true)
		NcMachine::NcMachineInstance()->mDwellTime = mDwellTime;

	NcMachine::NcMachineInstance()->buildGCodeList();
}

STATUS NcFileReader::checkSyntaxOfCCWCirInterpol(QStringList codelist)
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
				if(codenumber != 3)
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
		case 'K':  
		case 'k':
			{
				mKCodeDetected = true;
				KValue = code.right(code.size() - 1).toDouble(&ok); 
				break;
			}
		case 'N': 
		case 'n':
			{
				/*mNCodeDetected = true;
				NValue = code.right(code.size() - 1).toDouble(&ok);*/
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
	/*	mNCodeDetected = false;*/
		mFCodeDetected = false;
		mXValueDetected = false;
		mZValueDetected = false;
		mTCodeDetected = false;
		mICodeDetected = false;
		mKCodeDetected = false;

		return OK;
}

void NcFileReader::buildG03Code()
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

STATUS NcFileReader::checkSyntaxOfCWCirInterpol(QStringList codelist)
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
				if(codenumber != 2)
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
		case 'K': 
		case 'k':
			{
				mKCodeDetected = true;
				KValue = code.right(code.size() - 1).toDouble(&ok); 
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

void NcFileReader::buildG02Code()
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

STATUS NcFileReader::checkSyntaxOfLinearInterpol(QStringList codelist)
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
				if(codenumber != 1)
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
				/*mNCodeDetected = true;
				NValue = code.right(code.size() - 1).toDouble(&ok);*/
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
	//mNCodeDetected = false;
	mSCodeDetected = false;
	mFCodeDetected = false;
	mXValueDetected = false;
	mZValueDetected = false;
	mTCodeDetected = false;

	return OK;
}

void NcFileReader::buildG01Code()
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

STATUS NcFileReader::checkSyntaxOfRapidCode(QStringList codelist)
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
				/***Pranit **/ //to handle multiple g code on same line.
				
				switch (codenumber)
				{
					case 0:
							 break;
					/*case 54: buildG54Code(codelist);
							 break;
					case 90: buildG90Code();
							 break;*/
					default: handleError(code);
							 break;		
				}
					
				
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
				/*mNCodeDetected = true;
				NValue = code.right(code.size() - 1).toDouble(&ok);*/
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
	/*mNCodeDetected = false;*/
	mFCodeDetected = false;
	mXValueDetected = false;
	mZValueDetected = false;
	mTCodeDetected = false;

	return OK;
}

void NcFileReader::buildG00Code()
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

void NcFileReader::buildTCode()
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

void NcFileReader::handleError(QString code)
{
	QString message;
	message += QObject::tr("code ") + code + QObject::tr(" is invalid.") + QObject::tr("\n") +
			QObject::tr("Please correct the NC file and restart the application");

	QMessageBox::StandardButton ret;
	ret = QMessageBox::warning(0,qApp->applicationName(),
								message, QMessageBox::Ok);

	if(ret == QMessageBox::Ok)
	{	
		exit(0);
		//return;
	}
}

void NcFileReader::handleError(QStringList codelist)
{
	QString message;
	message += QObject::tr("Block ");
	foreach(QString code, codelist)
	{
		message += code + QObject::tr(" ");
	}
	message += QObject::tr(" is invalid.") + QObject::tr("\n") +
			QObject::tr("Please correct the NC file and restart the application");

	QMessageBox::StandardButton ret;
	ret = QMessageBox::warning(0, qApp->applicationName(),
								message, QMessageBox::Ok);

	if(ret == QMessageBox::Ok)
	{
		exit(0);
	}
}
