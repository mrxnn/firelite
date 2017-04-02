#include "formatstream.h"

FormatStream::FormatStream(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(QColor::fromRgb(0,0,255));
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList keywordPatterns;
    keywordPatterns << "\\babort\\b"
                    << "\\baction\\b"
                    << "\\badd\\b"
                    << "\\bafter\\b"
                    << "\\ball\\b"
                    << "\\balter\\b"
                    << "\\banalyze\\b"
                    << "\\band\\b"
                    << "\\bas\\b"
                    << "\\basc\\b"
                    << "\\battach\\b"
                    << "\\bautoincrement\\b"

                       //! Letter B
                    << "\\bbefore\\b"
                    << "\\bbegin\\b"
                    << "\\bbetween\\b"
                    << "\\bbinary\\b"
                    << "\\bblob\\b"
                    << "\\bby\\b"

                       //! Letter C
                    << "\\bcascade\\b"
                    << "\\bcase\\b"
                    << "\\bcast\\b"
                    << "\\bcheck\\b"
                    << "\\bcollate\\b"
                    << "\\bcolumn\\b"
                    << "\\bcommit\\b"
                    << "\\bconflict\\b"
                    << "\\bconstraint\\b"
                    << "\\bcreate\\b"
                    << "\\bcross\\b"
                    << "\\bcurrency\\b"
                    << "\\bcurrent_date\\b"
                    << "\\bcurrent_time\\b"
                    << "\\bcurrent_timestamp\\b"

                       //! Letter D
                    << "\\bdatabase\\b"
                    << "\\bdefault\\b"
                    << "\\bdeferrable\\b"
                    << "\\bdeferred\\b"
                    << "\\bdelete\\b"
                    << "\\bdesc\\b"
                    << "\\bdetach\\b"
                    << "\\bdistinct\\b"
                    << "\\bdrop\\b"

                       //! Letter E
                    << "\\beach\\b"
                    << "\\belse\\b"
                    << "\\bend\\b"
                    << "\\bescape\\b"
                    << "\\bexcept\\b"
                    << "\\bexclusive\\b"
                    << "\\bexists\\b"
                    << "\\bexplain\\b"

                       //! Letter F
                    << "\\bfail\\b"
                    << "\\bfor\\b"
                    << "\\bforeign\\b"
                    << "\\bfrom\\b"
                    << "\\bfloat\\b"
                    << "\\bfull\\b"

                       //! Letter G
                    << "\\bglob\\b"
                    << "\\bgroup\\b"

                       //! Letter H
                    << "\\bhaving\\b"

                       //! Letter I
                    << "\\bif\\b"
                    << "\\bignore\\b"
                    << "\\bimmediate\\b"
                    << "\\bin\\b"
                    << "\\bindex\\b"
                    << "\\bindexed\\b"
                    << "\\binitially\\b"
                    << "\\binner\\b"
                    << "\\binsert\\b"
                    << "\\binstead\\b"
                    << "\\bintersect\\b"
                    << "\\binto\\b"
                    << "\\bis\\b"
                    << "\\bisnull\\b"

                       //! Letter J
                    << "\\bjoin\\b"

                       //! Letter K
                    << "\\bkey\\b"

                       //! Letter L
                    << "\\bleft\\b"
                    << "\\blike\\b"
                    << "\\blimit\\b"

                       //! Letter M
                    << "\\bmatch\\b"

                       //! Letter N
                    << "\\bnatural\\b"
                    << "\\bno\\b"
                    << "\\bnot\\b"
                    << "\\bnotnull\\b"
                    << "\\bnull\\b"

                       //! Letter O
                    << "\\bof\\b"
                    << "\\boffset\\b"
                    << "\\bon\\b"
                    << "\\bor\\b"
                    << "\\border\\b"
                    << "\\bouter\\b"

                       //! Letter P
                    << "\\bplan\\b"
                    << "\\bpragma\\b"
                    << "\\bprimary\\b"

                       //! Letter Q
                    << "\\bquery\\b"

                       //! Letter R
                    << "\\braise\\b"
                    << "\\breal\\b"
                    << "\\breferences\\b"
                    << "\\bregexp\\b"
                    << "\\breindex\\b"
                    << "\\brelease\\b"
                    << "\\brename\\b"
                    << "\\breplace\\b"
                    << "\\brestrict\\b"
                    << "\\bright\\b"
                    << "\\brollback\\b"
                    << "\\brow\\b"

                       //! Letter S
                    << "\\bsavepoint\\b"
                    << "\\bselect\\b"
                    << "\\bset\\b"
                    << "\\bsmallint\\b"

                       //! Letter T
                    << "\\btable\\b"
                    << "\\btemp\\b"
                    << "\\btemporary\\b"
                    << "\\bthen\\b"
                    << "\\btime\\b"
                    << "\\btimestamp\\b"
                    << "\\bto\\b"
                    << "\\btransaction\\b"
                    << "\\btrigger\\b"

                       //! Letter U
                    << "\\bunion\\b"
                    << "\\bunique\\b"
                    << "\\bupdate\\b"
                    << "\\busing\\b"

                       //! Letter V
                    << "\\bvacuum\\b"
                    << "\\bvalues\\b"
                    << "\\bview\\b"
                    << "\\bvirtual\\b"

                       //! Letter W
                    << "\\bwhen\\b"
                    << "\\bwhere\\b"

                       //! Data Types
                    << "\\bint\\b"
                    << "\\binteger\\b"
                    << "\\bchar\\b"
                    << "\\bvarchar\\b"
                    << "\\btext\\b"
                    << "\\bbool\\b"
                    << "\\bboolean\\b"
                    << "\\bdouble\\b"
                    << "\\bdate\\b"
                    << "\\bMayura\\b";

    foreach (const QString& pattern, keywordPatterns)
    {
        rule.pattern=QRegExp(pattern,Qt::CaseInsensitive);
        rule.format=keywordFormat;
        highlightingRules.append(rule);
    }

    multiLineCommentFormat.setForeground(QColor::fromRgb(0,128,0));

    numberFormat.setForeground(QColor::fromRgb(255,130,0));
    rule.pattern = QRegExp("[0-9]");
    rule.format = numberFormat;
    highlightingRules.append(rule);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    singleQuatationFormat.setForeground(Qt::darkGray);
    rule.pattern = QRegExp("\'.*\'");
    rule.format = singleQuatationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void FormatStream::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule& rule, highlightingRules)
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

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0)
    {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
            commentLength = endIndex - startIndex + commentEndExpression.matchedLength();

        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
