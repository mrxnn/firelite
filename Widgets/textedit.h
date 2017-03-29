#ifndef TEXTEDIT_H
#define TEXTEDIT_H
#include <QTextEdit>
QT_BEGIN_NAMESPACE
class QCompleter;
QT_END_NAMESPACE

class TextEdit : public QTextEdit
{
    Q_OBJECT

public:
    TextEdit(QWidget *parent = 0);
    ~TextEdit();

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

protected:
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent *e) Q_DECL_OVERRIDE;

private slots:
    void insertCompletion(const QString &completion);

private:
    QString textUnderCursor() const;

private:
    QCompleter *c;
};

#endif // TEXTEDIT_H

