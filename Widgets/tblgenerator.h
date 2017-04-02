#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QTableWidget;
class QLineEdit;
class QPushButton;
class QCheckBox;
QT_END_NAMESPACE

class TblGenerator : public QDialog
{
    Q_OBJECT

public:
    TblGenerator(QWidget* parent = nullptr);
    QString Generate() const;

private slots:
    void makeCell();
    void createCell();
    void save();

private:
    void initializeUI();
    void createTableHeaders();

    QLineEdit* tableNameEdit;
    QTableWidget* tableEditor;
    QCheckBox* existantIndicator;
};

#endif // CONTROLLER_H
