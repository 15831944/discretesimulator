#include "NcUIComponents\NcBlockHighlighter.h"
#include <QTextDocument>
using namespace DiscreteSimulator;



NcBlockHighlighter::NcBlockHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{

	HighlightingRule rule;

	QTextCharFormat GFormat;
	QTextCharFormat NFormat;
	QTextCharFormat MFormat;
	QTextCharFormat XFormat;
	QTextCharFormat ZFormat;
	QTextCharFormat UFormat;
	QTextCharFormat WFormat;
	QTextCharFormat IFormat;
	QTextCharFormat KFormat;
	QTextCharFormat FFormat;
	QTextCharFormat SFormat;
	QTextCharFormat TFormat;

	GFormat.setFontWeight(QFont::Bold);
	GFormat.setForeground(Qt::darkMagenta);
	QString Gpattern ="\\b[G][0-9]{1,2}\\b";

	rule.pattern = QRegExp(Gpattern);
	rule.format = GFormat;
	highlightingRules.append(rule);


     	
 	NFormat.setFontWeight(QFont::Bold);
 	NFormat.setForeground(Qt::black);
 	QString Npattern = "\\b[N][0-9]{1,4}\\b";
	
 	rule.pattern = QRegExp(Npattern);
	rule.format = NFormat;
	highlightingRules.append(rule);


	MFormat.setFontWeight(QFont::Bold);
    MFormat.setForeground(Qt::darkYellow);
    QString Mpattern = "\\b[M][0-9]{1,3}\\b";

	rule.pattern = QRegExp(Mpattern);
	rule.format = MFormat;
	highlightingRules.append(rule);

	FFormat.setFontWeight(QFont::Bold);
    FFormat.setForeground(Qt::darkBlue);
    QString Fpattern = "\\b[F][0-9]{1,5}\\b";

    rule.pattern = QRegExp(Fpattern);
	rule.format = FFormat;
	highlightingRules.append(rule);


	XFormat.setFontWeight(QFont::Bold);
    XFormat.setForeground(Qt::darkGreen);
	QString Xpattern = "\\b[X][+-]?\\d*\\.?\\d*\\b";

    rule.pattern = QRegExp(Xpattern);
	rule.format = XFormat;
	highlightingRules.append(rule);

	ZFormat.setFontWeight(QFont::Bold);
    ZFormat.setForeground(Qt::darkGreen);
    QString Zpattern = "\\b[Z][+-]?\\d*\\.?\\d*\\b";

    rule.pattern = QRegExp(Zpattern);
	rule.format = ZFormat;
	highlightingRules.append(rule);

	UFormat.setFontWeight(QFont::Bold);
    UFormat.setForeground(Qt::darkCyan);
	QString Upattern = "\\b[U][+-]?\\d*\\.?\\d*\\b";

    rule.pattern = QRegExp(Upattern);
	rule.format = UFormat;
	highlightingRules.append(rule);


	WFormat.setFontWeight(QFont::Bold);
    WFormat.setForeground(Qt::darkCyan);
    QString Wpattern = "\\b[W][+-]?\\d*\\.?\\d*\\b";

	rule.pattern = QRegExp(Wpattern);
	rule.format = WFormat;
	highlightingRules.append(rule);

	SFormat.setFontWeight(QFont::Bold);
 	SFormat.setForeground(Qt::darkBlue);
 	QString Spattern = "\\b[S][0-9]{1,5}\\b";

	rule.pattern = QRegExp(Spattern);
	rule.format = SFormat;
	highlightingRules.append(rule);

	TFormat.setFontWeight(QFont::Bold);
 	TFormat.setForeground(Qt::magenta);
 	QString Tpattern = "\\b[T][0-9]{1,4}\\b";

	rule.pattern = QRegExp(Tpattern);
	rule.format = TFormat;
	highlightingRules.append(rule);


	IFormat.setFontWeight(QFont::Bold);
    IFormat.setForeground(Qt::red);
    QString Ipattern = "\\b[I][+-]?\\d*\\.?\\d*\\b";

	rule.pattern = QRegExp(Ipattern);
	rule.format = IFormat;
	highlightingRules.append(rule);
     	
	KFormat.setFontWeight(QFont::Bold);
    KFormat.setForeground(Qt::red);
    QString Kpattern = "\\b[K][+-]?\\d*\\.?\\d*\\b";

	rule.pattern = QRegExp(Kpattern);
	rule.format = KFormat;
	highlightingRules.append(rule);
}


NcBlockHighlighter::~NcBlockHighlighter()
{

}


void NcBlockHighlighter::highlightBlock(const QString &text)
{
	foreach (const HighlightingRule &rule, highlightingRules)
	{
         QRegExp expression(rule.pattern);
         int index = expression.indexIn(text);
         while (index >= 0)
		 {
             int length = expression.matchedLength();
             setFormat(index, length, rule.format);
             index = expression.indexIn(text, index + length);
         }
     }
     setCurrentBlockState(0);
}

