#ifndef FORMATSTREAM_H
#define FORMATSTREAM_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class FormatStream : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    FormatStream(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString&) Q_DECL_OVERRIDE;

private:
    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat singleQuatationFormat;
    QTextCharFormat functionFormat;

    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;
};

#endif // FORMATSTREAM_H
