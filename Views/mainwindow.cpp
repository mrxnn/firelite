#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFontComboBox>
#include <QFontDatabase>
#include <QComboBox>
#include <QIcon>
#include <QTextEdit>
#include <QTableView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QTabWidget>
#include <QSplitter>
#include <QSizePolicy>
#include <QAction>
#include <QTextDocument>
#include <QClipboard>
#include <QMimeData>
#include <QApplication>
#include <QFont>
#include <QStatusBar>
#include <QTextStream>
#include <QFileDialog>
#include <QFile>
#include <QCloseEvent>
#include <QSettings>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QPrinter>
#include <QPrintDialog>
#include <QMessageBox>
#include <QCompleter>
#include <QDialog>
#include <QPixmap>
#include <QLabel>
#include <QStringListModel>
#include <QDesktopServices>

#include "Libraries/viewmodel.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*
     * Created the 'MainWindow' outline in order to make it simplify the creation of MenuBar/ToolBar icons,
     * actions (or slots that handles the action) and shortcut keys. This can be also done by hand coding..
     * and because of the Toolbars are created dynamically by hand written codes, combining part would be
     * easier a bit. but that needs to create all the QAction pointers to be declared in the MainWindow.h
     * in order to be refered from anywhere in the program. So it makes sense to let the designer to take
     * care of the QAction pointers and all those ugly stuff, at least to my eyes.
     */
    initializeIcons();
    initializeToolbars();
    initializeUI();
    bind();

    /*
     * This section defines database related code, initializing all the necessary objects at startup
     */
    database = QSqlDatabase::addDatabase("QSQLITE");
    tableModel = new QSqlQueryModel(this);
    tableView->setModel(tableModel);
}


MainWindow::~MainWindow()
{
    delete ui;
}

/*
 * @brief Accepts a local URL to a file that contains all the key words that needs to be appeared in the TextEdit for autocompletion, and read it,
 * then returns a pointer to a data model.
 * @return QStringListModel
 */
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


/*
 * Adds the relevant icons to the QActions declared using the designer
 */
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


/*
 * Create all the toolbars and adds the necessary QActions into them.
 */
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


/*
 * Initialize the main UI of the MainWindow
 */
void MainWindow::initializeUI()
{
    QSplitter* splitter = new QSplitter(Qt::Vertical, this);

    //! custom text edit (autocompleter)
    editor = new TextEdit(this);
    editor->setPlaceholderText(tr("Sql statement..."));
    completer = new QCompleter(this);
    completer->setModel(modelFromFile(":/Resources/Completer/wordlist.txt"));
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

    solutionTree = new SolutionTreeWidget(this);

    QDockWidget* solutionWidget = new QDockWidget(tr("File Explorar"), this);
    solutionWidget->setObjectName(QStringLiteral("FileExplorar"));
    solutionWidget->setWidget(solutionTree);
    solutionWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, solutionWidget);

    setCentralWidget(splitter);
}

void MainWindow::bind()
{
    // Default
    ui->actionSave->setEnabled(editor->document()->isModified());
    ui->actionUndo->setEnabled(editor->document()->isUndoAvailable());
    ui->actionRedo->setEnabled(editor->document()->isRedoAvailable());

#ifndef QT_NO_CLIPBOARD
    ui->actionCut->setEnabled(false);
    ui->actionCopy->setEnabled(false);
    ui->actionPaste->setEnabled(false);

    connect(QApplication::clipboard(), &QClipboard::dataChanged, [&](){
        if (const QMimeData *md = QApplication::clipboard()->mimeData())
            ui->actionPaste->setEnabled(md->hasText());
        });
#endif

    connect(editor, &TextEdit::copyAvailable, ui->actionCut, &QAction::setEnabled);
    connect(editor, &TextEdit::copyAvailable, ui->actionCopy, &QAction::setEnabled);
    connect(editor->document(), &QTextDocument::undoAvailable, ui->actionUndo, &QAction::setEnabled);
    connect(editor->document(), &QTextDocument::redoAvailable, ui->actionRedo, &QAction::setEnabled);
    connect(editor->document(), &QTextDocument::modificationChanged, ui->actionSave, &QAction::setEnabled);
}


/*
 * Creates a New Sqlite Database Document and opens it in the database explorar.
 */
void MainWindow::on_actionNew_triggered()
{
    QFileDialog fileDialog(this, tr("Create New Database..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setNameFilter("sqlite database documents (*.db *.sqlite)");
    fileDialog.setDefaultSuffix("db");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    const QString str = fileDialog.selectedFiles().first();

    if (!str.isEmpty() && !str.isNull())
    {
        if (load(str))
        {
            solutionTree->addItemTotheExplorar(str);
        }
    }
}

/*
 * Opens an existing sqlite database document and loads it into the database explorar.
 */
void MainWindow::on_actionOpen_triggered()
{
    QFileDialog fileDialog(this, tr("Open Existing Database..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setNameFilter("sqlite database documents (*.db *.sqlite);; All Files (*)");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    const QString str = fileDialog.selectedFiles().first();

    if (!str.isEmpty() && !str.isNull())
    {
        if(load(str))
        {
            solutionTree->addItemTotheExplorar(str);
        }
    }
}

/* Make the necessary pre requesites when a database document is created or opened. Set the opened
 * database document is the one that is pointed by the global 'database' object in the application.
 * and if the specified database file is not valid for some reason, it creates a new file in the
 * same location silently.
 */
bool MainWindow::load(const QString &str)
{
    if (QFile::exists(str))
    {
        database.setDatabaseName(str);
        checkLastErrorIfAny();
        return database.open();
    }

    QFile db(str);
    if (db.open(QIODevice::WriteOnly))
    {
        database.setDatabaseName(str);
        checkLastErrorIfAny();
        return database.open();
    }

    return false;
}

/* This function is executed manually after doing some database related operations, such as executing
 * a query, changing the global database object to point into another database document ect. and it
 * checks weather there are any error occured by the global database object, and immedietly reports
 * it to the user if any.
 */
void MainWindow::checkLastErrorIfAny()
{
    if (database.lastError().isValid())
    {
        auto msgBox = new QMessageBox(this);
        msgBox->setText(database.lastError().text());
        msgBox->setIcon(QMessageBox::Critical);
        msgBox->exec();
        delete msgBox;
    }
}











































