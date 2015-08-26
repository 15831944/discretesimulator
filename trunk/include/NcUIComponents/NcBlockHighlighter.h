#ifndef NC_BLOCKHIGHLIGHTER
#define NC_BLOCKHIGHLIGHTER

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextCharFormat>
#include <QRegExp>
#include <QVector>
#include "NcUIComponents\NcUIComponentsGlobal.h"


class QTextDocument;

namespace DiscreteSimulator
{

	class NCUICOMPONENTSEXPORT NcBlockHighlighter  : public QSyntaxHighlighter
	{
		
	public:
		NcBlockHighlighter(QTextDocument *parent = 0);
		/*~NcBlockHighlighter();*/

	protected:
		void highlightBlock(const QString &text);

	private:
		class HighlightingRule
		{
		public:
			QRegExp pattern;
			QTextCharFormat format;
		};

		QVector<HighlightingRule> highlightingRules;

		QRegExp commentStartExpression;
		QRegExp commentEndExpression;

		QTextCharFormat keywordFormat;
		QTextCharFormat singleLineFormat;
	};
}


#endif