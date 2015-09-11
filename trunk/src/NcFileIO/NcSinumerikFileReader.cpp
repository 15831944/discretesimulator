#include <QFile>
#include <QStringList>
#include <QMessageBox>
#include "NcStateMachine\NcMachine.h"
#include "NcStateMachine\NcStartupStockDisplay.h"
#include "NcFileIO\NcSinumerikFileReader.h"
#include "NcDisplay\NcDisplay.h"
using namespace DiscreteSimulator;


NcSinumerikFileReader::NcSinumerikFileReader(QString filename)
	:NcFileReader(filename)
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
						message += QObject::tr("code ") + code + QObject::tr(" is invalid.") + QObject::tr( "\n") +
								QObject::tr("Please correct the NC file and restart the application");
 
						QMessageBox::StandardButton ret;
						ret = QMessageBox::warning(0,qApp->applicationName(),
													message, QMessageBox::Ok);

						if(ret == QMessageBox::Ok)
							exit(0);

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
