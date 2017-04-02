#include "tblgenerator.h"

#include <QtDebug>
#include <QtWidgets>

TblGenerator::TblGenerator(QWidget *parent) : QDialog(parent)
{
    initializeUI();

    auto createCellAction = new QAction(tr("Increment Cell"), this);
    auto removeCellAction = new QAction(tr("Decrement Cell"), this);
    tableEditor->addAction(createCellAction);
    tableEditor->addAction(removeCellAction);
    connect(createCellAction, &QAction::triggered, this, &TblGenerator::createCell);
    connect(removeCellAction, &QAction::triggered, [&](){ tableEditor->removeRow(tableEditor->rowCount() - 1); });
}

void TblGenerator::makeCell()
{
    createCell();
}

void TblGenerator::createCell()
{
    auto ComboBox = [&](QWidget* parent)
    {
        auto cb = new QComboBox(parent);
        cb->addItems({"Text", "Integer", "Real", "Blob", "Varchar", "SmallInt", "Float", "Double", "Boolean", "Currency", "Date", "Time", "Timestamp", "Binary"});
        return cb;
    };

    int counter = tableEditor->rowCount();
    tableEditor->insertRow(counter);
    tableEditor->setCellWidget(counter, 1, ComboBox(this));
    tableEditor->setCellWidget(counter, 2, new QRadioButton(this));
    tableEditor->setCellWidget(counter, 3, new QCheckBox(this));
    tableEditor->setCellWidget(counter, 4, new QCheckBox(this));
}

QString TblGenerator::Generate() const
{
    QString stmt = QString("create table %1%2\n( \n")
            .arg( existantIndicator->isChecked() ? "if not exists " : "" )
            .arg( tableNameEdit->text() );

    for (int i = 0; i < tableEditor->rowCount(); ++i)
    {
        auto name = tableEditor->item(i, 0)->text();
        auto constraint = static_cast<QComboBox*>(tableEditor->cellWidget(i, 1))->currentText();
        bool primarykey = static_cast<QRadioButton*>(tableEditor->cellWidget(i, 2))->isChecked();
        bool autoincrement = static_cast<QCheckBox*>(tableEditor->cellWidget(i, 3))->isChecked();
        bool notnull = static_cast<QCheckBox*>(tableEditor->cellWidget(i, 4))->isChecked();

        //! Generating Statement
        stmt    += "\t" + name + " "
                + constraint.toLower()
                + (primarykey       ? " primary key" : "")
                + (autoincrement    ? " autoincrement" : "")
                + (notnull          ? " not null" : "")
                + ",\n";
    }

    stmt.remove(stmt.length() - 2, 2);
    stmt.append("\n)");
    return stmt;
}

void TblGenerator::save()
{
    //! check if tbl name doesn't exist
    if (tableNameEdit->text().isEmpty())
    {
        auto msgBox = new QMessageBox(this);
        msgBox->setText(tr("Could not create a table for the given name Please provide a valid name in order to create a table."));
        msgBox->setIcon(QMessageBox::Critical);
        msgBox->exec();
        delete msgBox;
        return;
    }

    //! checks unnamed column names
    for (int i = 0; i < tableEditor->rowCount(); ++i)
    {
        if (!tableEditor->item(i, 0))
        {
            auto msgBox = new QMessageBox(this);
            msgBox->setText(tr("unnamed columns cannot be exists please name all the\ncolumns identically."));
            msgBox->setIcon(QMessageBox::Critical);
            msgBox->exec();
            delete msgBox;
            return;
        }
    }

    accept();
}

void TblGenerator::initializeUI()
{
    //! top layout
    QHBoxLayout* topLayout = new QHBoxLayout;
    tableNameEdit = new QLineEdit(this);
    QPushButton* makeCellButton = new QPushButton(tr("+"), this);
    makeCellButton->setMaximumWidth(25);
    QLabel* lbl = new QLabel(tr("Enter table name: "), this);
    existantIndicator = new QCheckBox(tr("Create Table\nIf Not Exists"), this);

    topLayout->addWidget(lbl);
    topLayout->addWidget(tableNameEdit, 1);
    topLayout->addWidget(existantIndicator);
    topLayout->addWidget(makeCellButton, 0);
    connect(makeCellButton, &QPushButton::clicked, this, &TblGenerator::makeCell);

    //! table editor ie query generator
    tableEditor = new QTableWidget(0, 5, this);
    tableEditor->setAccessibleName("Editor");
    tableEditor->setContextMenuPolicy(Qt::ActionsContextMenu);
    createTableHeaders();

    //! bottom buttons panel
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Reset, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &TblGenerator::save);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &TblGenerator::close);

    //! main layout
    QVBoxLayout* rootLayout = new QVBoxLayout;
    rootLayout->addLayout(topLayout);
    rootLayout->addWidget(tableEditor);
    rootLayout->addWidget(buttonBox);

    setLayout(rootLayout);
    layout()->setMargin(4);
}

void TblGenerator::createTableHeaders()
{
    //! sizes
    tableEditor->setColumnWidth(0, 100);
    tableEditor->setColumnWidth(1, 80);
    tableEditor->setColumnWidth(2, 80);
    tableEditor->setColumnWidth(3, 100);
    tableEditor->setColumnWidth(4, 70);

    //! font
    const QString fontString = "Calibri";
    tableEditor->setFont(QFont(fontString));

    //! headers
    auto nameHdr = new QTableWidgetItem(tr("name")); nameHdr->setFont(QFont(fontString));
    auto constraintHdr = new QTableWidgetItem(tr("type")); constraintHdr->setFont(QFont(fontString));
    auto primary_keyHdr = new QTableWidgetItem(tr("primarykey")); primary_keyHdr->setFont(QFont(fontString));
    auto autoincrementHdr = new QTableWidgetItem(tr("autoincrement")); autoincrementHdr->setFont(QFont(fontString));
    auto notnullHdr = new QTableWidgetItem(tr("notnull")); notnullHdr->setFont(QFont(fontString));

    tableEditor->setHorizontalHeaderItem(0, nameHdr);
    tableEditor->setHorizontalHeaderItem(1, constraintHdr);
    tableEditor->setHorizontalHeaderItem(2, primary_keyHdr);
    tableEditor->setHorizontalHeaderItem(3, autoincrementHdr);
    tableEditor->setHorizontalHeaderItem(4, notnullHdr);
}
