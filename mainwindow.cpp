#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QToolBar>
#include <QCompleter>
#include <QCursor>
#include <QByteArray>
#include <QStringList>
#include <QFile>
#include <QStringListModel>
#include <QSplitter>
#include <QSizePolicy>
#include <QWidget>
#include <QTreeWidget>
#include <QDockWidget>

#ifdef Q_OS_MAC
const QString resource=":/Resources/Mac/"
#else
const QString resource=":/Resources/Windows/";
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialization Part
    initializeIcons();
    initializeToolbars();
    initializeUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QAbstractItemModel *MainWindow::modelFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return new QStringListModel(completer);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    QStringList words;

    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            words << line.trimmed();
    }

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    return new QStringListModel(words, completer);
}

void MainWindow::initializeIcons()
{
    // File Actions
    ui->actionNew->setIcon(QIcon(resource + "filenew.png"));
    ui->actionOpen->setIcon(QIcon(resource + "fileopen.png"));
    ui->actionSave->setIcon(QIcon(resource + "filesave.png"));
    ui->actionPrint->setIcon(QIcon(resource + "fileprint.png"));
    ui->actionExport->setIcon(QIcon(resource + "exportpdf.png"));

    // Edit Actions
    ui->actionCut->setIcon(QIcon(resource + "editcut.png"));
    ui->actionCopy->setIcon(QIcon(resource + "editcopy.png"));
    ui->actionPaste->setIcon(QIcon(resource + "editpaste.png"));
    ui->actionUndo->setIcon(QIcon(resource + "editundo.png"));
    ui->actionRedo->setIcon(QIcon(resource + "editredo.png"));

    // Run Actions
    ui->actionRun->setIcon(QIcon(resource + "execute.png"));

    // Help Actions
    ui->actionAbout->setIcon(QIcon(resource + "about.png"));
}

void MainWindow::initializeToolbars()
{
    //! Document toolbar
    //!
    auto documentTb = addToolBar(tr("Documents"));
    documentTb->addAction(ui->actionSave);
    documentTb->addAction(ui->actionPrint);
    documentTb->addAction(ui->actionExport);

    //! Edit toolbar
    //!
    auto editTb = addToolBar(tr("Edit"));
    editTb->addAction(ui->actionCut);
    editTb->addAction(ui->actionCopy);
    editTb->addAction(ui->actionPaste);
    editTb->addAction(ui->actionUndo);
    editTb->addAction(ui->actionRedo);

    //! Fonts toolbar
    //!
    auto fontsTb = addToolBar(tr("Fonts"));

    //! Font selector combox box
    //!
    fontsComboBox = new QFontComboBox(this);
    fontsComboBox->setCurrentFont(QFont("Courier"));
    fontsTb->addWidget(fontsComboBox);

    //! Font size selector box
    //!
    fontSizeComboBox = new QComboBox(this);
    fontSizeComboBox->setEditable(true);

    // Adding standard font sizes to the size combo box
    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes) fontSizeComboBox->addItem(QString::number(size));

    // Set the default size 10 which is for Courier
    fontSizeComboBox->setCurrentIndex(standardSizes.indexOf(10));
    fontsTb->addWidget(fontSizeComboBox);

    //! File toolbar
    //!
    addToolBarBreak(Qt::TopToolBarArea);
    auto fileTb = addToolBar(tr("File"));
    fileTb->addAction(ui->actionNew);
    fileTb->addAction(ui->actionOpen);

    //! Selected database indicator tool item
    //!
    auto databaseTb = addToolBar(tr("Database"));
    selectedDatabaseIndicatorComboBox = new QComboBox(this);
    selectedDatabaseIndicatorComboBox->setMinimumWidth(140);
    databaseTb->addWidget(selectedDatabaseIndicatorComboBox);

    //! Run toolbar
    //!
    auto runTb = addToolBar(tr("Run"));
    runTb->addAction(ui->actionRun);
}

void MainWindow::initializeUI()
{
    QSplitter* splitter = new QSplitter(Qt::Vertical, this);

    //! custom text edit (autocompleter)
    editor = new TextEdit(this);
    editor->setPlaceholderText(tr("Sql statement..."));
    completer = new QCompleter(this);
    completer->setModel(modelFromFile(":/autocomplete/completer/wordlist.txt"));
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);
    editor->setCompleter(completer);

    tableView = new QTableView(this);
    activityLog = new QListWidget(this);
    activityLog->setFont(QFont("Calibri"));

    //! custom widget
    resultPanel = new QTabWidget(this);
    resultPanel->setTabPosition(QTabWidget::East);
    resultPanel->addTab(tableView, tr("Result"));
    resultPanel->addTab(activityLog, tr("History"));

    //! adding widgets
    splitter->addWidget(editor);
    splitter->addWidget(resultPanel);

    //! size policy for editor
    QWidget* widget = splitter->widget(0);
    QSizePolicy policy = widget->sizePolicy();
    policy.setHorizontalStretch(QSizePolicy::Expanding);
    policy.setVerticalStretch(QSizePolicy::Expanding);
    widget->setSizePolicy(policy);

    QTreeWidget* solutionTree = new QTreeWidget(this);

    QDockWidget* solutionWidget = new QDockWidget(tr("File Explorar"), this);
    solutionWidget->setObjectName(QStringLiteral("FileExplorar"));
    solutionWidget->setWidget(solutionTree);
    solutionWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, solutionWidget);

    setCentralWidget(splitter);
}




































