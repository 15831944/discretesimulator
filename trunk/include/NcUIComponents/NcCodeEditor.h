#ifndef NC_CODEEDITOR
#define NC_CODEEDITOR

#include "NcUIComponents\NcUIComponentsGlobal.h"
#include <QTextEdit>

namespace DiscreteSimulator
{
	class NcBlockHighlighter;

	class NCUICOMPONENTSEXPORT NcCodeEditor : public QTextEdit
	{
	public:
		static NcCodeEditor* NcCodeEditorInstance();
		virtual ~NcCodeEditor();

		void	highlightCodeLineInEditor(int lineno);

	private:
		NcCodeEditor(QWidget *parent = 0);
		NcCodeEditor(const NcCodeEditor&);
		NcCodeEditor& operator=(const NcCodeEditor&);

	private:
		static NcCodeEditor* mNcCodeEditorInstance;
		NcBlockHighlighter		*mBlockHighlighter;
		
	
	};
}


#endif