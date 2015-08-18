#include "NcUIComponents\NcBlockHighlighter.h"
#include "NcUIComponents\NcCodeEditor.h"
using namespace DiscreteSimulator;

NcCodeEditor* NcCodeEditor::mNcCodeEditorInstance = 0;

NcCodeEditor::NcCodeEditor(QWidget *parent) : QTextEdit(parent)
{
	 mBlockHighlighter = new NcBlockHighlighter(document());
}

NcCodeEditor::~NcCodeEditor()
{
	
}

NcCodeEditor* NcCodeEditor::NcCodeEditorInstance()
{
	if(NcCodeEditor::mNcCodeEditorInstance == 0)
	{
		NcCodeEditor::mNcCodeEditorInstance = new NcCodeEditor();
	}
	return NcCodeEditor::mNcCodeEditorInstance;
}


void	NcCodeEditor::highlightCodeLineInEditor(int lineno)
{
	static int previouslineno = 0;
	QTextCursor hlCursor(textCursor());

	//for resetting the cursor to the start of the file and previouslineno = 0 
	//when simulation is restarted
	if(lineno == 0)  
	{
		hlCursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor, 1);
		previouslineno = 0;
	}

	if(lineno == previouslineno)
	{
		hlCursor.select(QTextCursor::LineUnderCursor);								
	}
	else
	{
		hlCursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, 1);
		hlCursor.select(QTextCursor::LineUnderCursor);
		previouslineno = lineno;
	}
	setTextCursor(hlCursor);

}