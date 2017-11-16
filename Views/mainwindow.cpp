#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFontComboBox>
#include <QFontDatabase>
#include <QComboBox>
#include <QListWidget>
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
#include <QtDebug>
#include <QDialog>
#include <QPixmap>
#include <QLabel>
#include <QStringListModel>
#include <QDesktopServices>

#include "Libraries/viewmodel.h"
#include "Formats/formatstream.h"
#include "Widgets/tblgenerator.h"
#include "Widgets/textedit.h"
#include "Widgets/solutiontreewidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*
     * Created the MainWindow outline using the QtDesigner in order to make it simple to cretae MenuBar and Toolbars, including the StatusTips, Shortcuts and so
     * on. This could have done manually by hand coding, but it requires each and every QAction pointers to be created by typing codes, which is a bit overkill.
     * And in fact the designer takes care of all these clumsy stuff just fine for now.
     *
     * Although the QActions are created by using QtDesigner, setting icons is done by @code initializeIcons() function, because the Icons depends on the platform
     * in which the application runs.
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

    ReadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    WriteSettings();
    e->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (QFileInfo(e->mimeData()->urls().first().toLocalFile()).suffix() == "db")
        e->accept();
}

void MainWindow::dropEvent(QDropEvent *e)
{
    const QString str = e->mimeData()->urls().first().toLocalFile();
    bool isLoaded = load(str);
    if (isLoaded)
    {
        solutionTree->addItemTotheExplorar(str);
        loadTablesToTheSelectedDatabase();
    }
    else
        QMessageBox::warning(this, tr(""), database.lastError().text());
}

/*
 * Accepts a local URL to a file in the file system that contains all the keywords that needs to be appeared in the TextEdit for autocompletion. It reads the
 * keywords, creates StringListModel and returns a pointer to the data model.
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
 * MainWindow::initializeIcons
 * Adds the relevant icons to the QActions depending on the Operating System. If the OS has a standard icon theme already defined, those icons are set.
 */
void MainWindow::initializeIcons()
{
    ui->actionNew->setIcon(QIcon::fromTheme("document-new", QIcon(resource + "filenew.png")));
    ui->actionOpen->setIcon(QIcon::fromTheme("document-open", QIcon(resource + "fileopen.png")));
    ui->actionSave->setIcon(QIcon::fromTheme("document-save", QIcon(resource + "filesave.png")));
    ui->actionPrint->setIcon(QIcon::fromTheme("document-print", QIcon(resource + "fileprint.png")));
    ui->actionExport->setIcon(QIcon::fromTheme("document-printpdf", QIcon(resource + "exportpdf.png")));
    ui->actionCut->setIcon(QIcon::fromTheme("edit-cut", QIcon(resource + "editcut.png")));
    ui->actionCopy->setIcon(QIcon::fromTheme("edit-copy", QIcon(resource + "editcopy.png")));
    ui->actionPaste->setIcon(QIcon::fromTheme("edit-paste", QIcon(resource + "editpaste.png")));
    ui->actionUndo->setIcon(QIcon::fromTheme("edit-undo", QIcon(resource + "editundo.png")));
    ui->actionRedo->setIcon(QIcon::fromTheme("edit-redo", QIcon(resource + "editredo.png")));

    //! these two doesn't depend on the platform
    ui->actionRun->setIcon(QIcon(resource + "execute.png"));
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
    documentTb->setObjectName("_document_tb");
    documentTb->addAction(ui->actionSave);
    documentTb->addAction(ui->actionPrint);
    documentTb->addAction(ui->actionExport);

    //! Edit toolbar
    //!
    auto editTb = addToolBar(tr("Edit"));
    editTb->setObjectName("_edit_tb");
    editTb->addAction(ui->actionCut);
    editTb->addAction(ui->actionCopy);
    editTb->addAction(ui->actionPaste);
    editTb->addAction(ui->actionUndo);
    editTb->addAction(ui->actionRedo);

    //! Fonts toolbar
    //!
    auto fontsTb = addToolBar(tr("Fonts"));
    fontsTb->setObjectName("_fonts_tb");

    //! Font selector combox box
    //!
    fontsComboBox = new QFontComboBox(this);
    fontsComboBox->setCurrentFont(QFont("Courier"));
    fontsTb->addWidget(fontsComboBox);
    connect(fontsComboBox, &QFontComboBox::currentFontChanged, this, &MainWindow::textFamily);

    //! Font size selector box
    //!
    fontSizeComboBox = new QComboBox(this);
    fontSizeComboBox->setEditable(true);
    connect(fontSizeComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index)
    {
        Q_UNUSED(index)

        if (editor == nullptr) return;

        QString str = editor->toPlainText();
        editor->clear();
        int size = fontSizeComboBox->currentText().toInt();
        editor->setFontPointSize(size);
        editor->setText(str);
    });

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
    fileTb->setObjectName("_file_tb");
    fileTb->addAction(ui->actionNew);
    fileTb->addAction(ui->actionOpen);

    //! Selected database indicator tool item
    //!
    auto databaseTb = addToolBar(tr("Database"));
    databaseTb->setObjectName("_database_tb");
    selectedDatabaseIndicatorComboBox = new QComboBox(this);
    selectedDatabaseIndicatorComboBox->setMinimumWidth(140);
    databaseTb->addWidget(selectedDatabaseIndicatorComboBox);

    //! Run toolbar
    //!
    auto runTb = addToolBar(tr("Run"));
    runTb->setObjectName("run_tb");
    runTb->addAction(ui->actionRun);
}

/*
 * MainWindow::initializeUI
 * Initialize the main UI of the MainWindow.
 */
void MainWindow::initializeUI()
{
    setAcceptDrops(true);
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

    //! Default Format Stream
    FormatStream* fs = new FormatStream(editor->document());
    Q_UNUSED(fs)

    tableView = new QTableView(this);
    activityLog = new QListWidget(this);
    activityLog->setFont(QFont("Calibri"));

    //! custom widget
    resultPanel = new QTabWidget(this);
    resultPanel->setContextMenuPolicy(Qt::ActionsContextMenu);
    resultPanel->setTabPosition(QTabWidget::East);
    resultPanel->addTab(tableView, tr("Result"));
    resultPanel->addTab(activityLog, tr("History"));

    //! context menu for the result panel
    auto actionResultRemove = new QAction(tr("Remove current records"), this);
    resultPanel->addAction(actionResultRemove);
    connect(actionResultRemove, &QAction::triggered, [&]()
    {
        int index = resultPanel->currentIndex();
        switch (index) {
        case 0:
            tableModel->clear();
            break;
        case 1:
            activityLog->clear();
            break;
        }
    });

    //! adding widgets
    splitter->addWidget(editor);
    splitter->addWidget(resultPanel);

    //! size policy for editor
    QWidget* widget = splitter->widget(0);
    QSizePolicy policy = widget->sizePolicy();
    policy.setHorizontalStretch(QSizePolicy::Expanding);
    policy.setVerticalStretch(QSizePolicy::Expanding);
    widget->setSizePolicy(policy);

    //! Solution Tree (Database Explorar)
    solutionTree = new SolutionTreeWidget(this);

    QDockWidget* solutionWidget = new QDockWidget(tr("File Explorar"), this);
    solutionWidget->setObjectName(QStringLiteral("FileExplorar"));
    solutionWidget->setWidget(solutionTree);
    solutionWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, solutionWidget);

    setCentralWidget(splitter);

#ifndef Q_OS_WIN
    ui->actionNativeWindowsUI->setVisible(false);
#endif
}

/*
 * Establishes all the connections in the differet part of the Main UI. It doesn't contain all the connections, but the ones that are directly related to the Main UI.
 * Note: No connections are defined using the Qt Designer.
 */
void MainWindow::bind()
{
    // Default:
    ui->actionSave->setEnabled(editor->document()->isModified());
    ui->actionUndo->setEnabled(editor->document()->isUndoAvailable());
    ui->actionRedo->setEnabled(editor->document()->isRedoAvailable());

    // File Actions
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::fileSave);
    connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::fileSaveAs);
    connect(ui->actionPrint, &QAction::triggered, this, &MainWindow::filePrint);
    connect(ui->actionExport, &QAction::triggered, this, &MainWindow::filePrintPdf);


#ifndef QT_NO_CLIPBOARD
    ui->actionCut->setEnabled(false);
    ui->actionCopy->setEnabled(false);
    ui->actionPaste->setEnabled(false);

    //! Paste Action
    connect(QApplication::clipboard(), &QClipboard::dataChanged, [&](){
        if (const QMimeData *md = QApplication::clipboard()->mimeData())
            ui->actionPaste->setEnabled(md->hasText());
        });
#endif

    // Edit Actions Default:
    connect(editor, &TextEdit::copyAvailable, ui->actionCut, &QAction::setEnabled);
    connect(editor, &TextEdit::copyAvailable, ui->actionCopy, &QAction::setEnabled);
    connect(editor->document(), &QTextDocument::undoAvailable, ui->actionUndo, &QAction::setEnabled);
    connect(editor->document(), &QTextDocument::redoAvailable, ui->actionRedo, &QAction::setEnabled);
    connect(editor->document(), &QTextDocument::modificationChanged, ui->actionSave, &QAction::setEnabled);

    // Edit Actions
    connect(ui->actionCut, &QAction::triggered, editor, &TextEdit::cut);
    connect(ui->actionCopy, &QAction::triggered, editor, &TextEdit::copy);
    connect(ui->actionPaste, &QAction::triggered, editor, &TextEdit::paste);
    connect(ui->actionUndo, &QAction::triggered, editor, &TextEdit::undo);
    connect(ui->actionRedo, &QAction::triggered, editor, &TextEdit::redo);


    // TreeView connections
    connect(solutionTree, &SolutionTreeWidget::selectedItemChanged, this, &MainWindow::onSelectedItemChanged);
    connect(solutionTree, &SolutionTreeWidget::statementRequested, this, &MainWindow::onStatementRequested);
    connect(solutionTree, &SolutionTreeWidget::itemDoubleClicked, [&](){

        if (solutionTree->getSelectedItemType() == SolutionTreeWidget::Table)
        {
            QString tableName = solutionTree->currentItem()->text(0);
            editor->insertPlainText(tableName);
        }
    });

    connect(solutionTree, &SolutionTreeWidget::tableGeneratorRequested, this, &MainWindow::onTableGeneratorRequested);
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
 * opens an existing sqlite database document and loads it into the database explorar.
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
            loadTablesToTheSelectedDatabase();
        }
    }
}

/*
 * Execute the statement
 */
void MainWindow::on_actionRun_triggered()
{
    // Check if the global database is points to some document
    if (!database.isOpen())
    {
        auto msgBox = new QMessageBox(this);
        msgBox->setIcon(QMessageBox::Information);
        msgBox->setText(tr("Please select a database first before executing statements."));
        msgBox->exec();
        delete msgBox;
        return;
    }

    // Get the text typed in the editor
    const QString command = editor->toPlainText();
    if ((command.isEmpty() || command.isNull()) || command.trimmed().isEmpty())
        return;

    // Executing the command, or query, and report if any error occurred
    QSqlQuery query;
    if (!query.exec(command))
    {
        checkLastErrorIfAny(&query);
        return;
    }

    //! At this point, the query is successfully exectuted
    //!

    // Get the query type that was executed, and generate the appropriate message that needs to be shown in the Activity Log
    QString message;
    auto queryType = getQueryType(command, message, query.numRowsAffected());

    if (queryType == ExecuteQueryType::SelectStatement)
    {
        resultPanel->setCurrentIndex(0);
        tableModel->setQuery(query);
    }
    else
    {
        // Load or remove tables, if new tables are created or removed old ones.
        if (queryType == ExecuteQueryType::CreateStatement || queryType == ExecuteQueryType::DropStatement)
            loadTablesToTheSelectedDatabase();

        QListWidgetItem* indice = new QListWidgetItem(QIcon(resource + "execute.png"), message, activityLog);
        indice->setToolTip(command.trimmed());
        activityLog->setCurrentItem(indice);
        resultPanel->setCurrentIndex(1);
    }
}

/*
 * MainWindow::on_actionNativeWindowsUI_triggered
 * toggle between the Windows Vista Theme and Fusion Theme (only on Windows)
 */
void MainWindow::on_actionNativeWindowsUI_triggered(bool checked)
{
    if (checked)
        qApp->setStyle("windowsvista");
    else
        qApp->setStyle("fusion");
}

/*
 * MainWindow::on_actionShowTextOnToolbar_triggered
 * toggle between tool button text visibility and hidden
 */
void MainWindow::on_actionShowTextOnToolbar_triggered(bool checked)
{
    if (checked)
        setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    else
        setToolButtonStyle(Qt::ToolButtonFollowStyle);
}

/*
 * Displays the about Window
 */
void MainWindow::on_actionAbout_triggered()
{
    QDialog dlg(this);
    dlg.setWindowTitle(tr("About"));
    dlg.setMinimumWidth(220);
    dlg.setMinimumHeight(140);

    Qt::WindowFlags flags = 0;
    flags = Qt::Window;
    flags |= Qt::WindowCloseButtonHint;
    // flags |= Qt::MSWindowsFixedSizeDialogHint;
    dlg.setWindowFlags(flags);

    QVBoxLayout* layout = new QVBoxLayout;
    QLabel* image = new QLabel(&dlg);
    image->setPixmap(QPixmap(":/Resources/Logo/logo-bold-96.png"));
    image->setAlignment(Qt::AlignCenter);
    layout->addWidget(image);

    QLabel* line1 = new QLabel(tr("Firelite 1.1 Sqlite Manager"), &dlg);
    line1->setAlignment(Qt::AlignCenter);
    layout->addWidget(line1);

    QLabel* line2 = new QLabel(tr("Written By Mayura Ramanayaka"), &dlg);
    line2->setAlignment(Qt::AlignCenter);
    layout->addWidget(line2);

    QLabel* line3 = new QLabel(tr("Copyright © 2017 - <a href='http://mayura-ramanayaka.github.io/firelite/'>Homepage</a>"), &dlg);
    line3->setAlignment(Qt::AlignCenter);
    layout->addWidget(line3);

    connect(line3, &QLabel::linkActivated, &QDesktopServices::openUrl);
    dlg.setLayout(layout);
    dlg.exec();
}

/*
 * Displays the About Qt Window
 */
void MainWindow::on_actionAbout_Framework_triggered()
{
    QMessageBox::aboutQt(this, tr("About Framework"));
}

/*
 * Make the necessary pre requesites when a database document is created or opened. Set the opened database document as the one that is pointed by the global
 * 'database' object in the application. and if the specified database file is not valid for any reason, a new document is created at the specified path silently.
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

/*
 * Get the type of query that was executed, and generate a notification based on the query type.
 */
MainWindow::ExecuteQueryType MainWindow::getQueryType(const QString &query, QString& message, int rows)
{
    if (query.trimmed().startsWith("create table if not exists", Qt::CaseInsensitive))
    {
        message = "Succeed: 0 rows affected";
        return ExecuteQueryType::CreateStatement;
    }
    else if (query.trimmed().startsWith("create table", Qt::CaseInsensitive))
    {
        message = QString("Table created: %1 rows affected").arg(rows);
        return ExecuteQueryType::CreateStatement;
    }
    else if (query.trimmed().startsWith("drop table", Qt::CaseInsensitive))
    {
        message = QString("Table deleted: %1 rows affected").arg(rows);
        return ExecuteQueryType::DropStatement;
    }
    else if (query.trimmed().startsWith("select", Qt::CaseInsensitive))
    {
        return ExecuteQueryType::SelectStatement;
    }
    else
    {
        message = QString("Succeed: %1 rows affected").arg(rows);
        return ExecuteQueryType::OtherStatement;
    }
}

/*
 * Loads all the tables that are already stored in a database document and add each table indicators to the treeView underneath the database name Item.
 */
void MainWindow::loadTablesToTheSelectedDatabase()
{   
    // Load all the tables in the current item
    auto ci = solutionTree->currentItem();
    if (ci)
    {
        if (solutionTree->getSelectedItemType() == SolutionTreeWidget::SelectedItemType::Table)
            ci = ci->parent();

        // First we remove all the tables in the item
        foreach (QTreeWidgetItem* i, ci->takeChildren())
        {
            delete i;
        }

        const QString stmt = "Select distinct tbl_name from sqlite_master";
        QSqlQuery query; query.exec(stmt);
        while (query.next())
        {
            // Get the table name
            const QString t = query.value(0).toString();

            auto contains = [&](const QString& tableName)
            {
                for (int i = 0; i < ci->childCount(); ++i)
                    if (tableName == ci->child(i)->text(0) || tableName == "sqlite_sequence")
                        return true;
                return false;
            };

            if (!contains(t))
                solutionTree->addItemToTheSelectedNode(t);
        }
    }
}

/*
 * This function is executed manually after doing any database related task, such as executing a query, changing the global database object to point into
 * another database document etc. and it checks weather there are any error occured by the global database object, and immedietly reports  it to the user if any.
 */
void MainWindow::checkLastErrorIfAny(QSqlQuery *query)
{
    if (query)
    {
        if (query->lastError().isValid())
        {
            QMessageBox::critical(this, tr(""), query->lastError().text());
        }

        return;
    }

    if (database.lastError().isValid())
    {
        auto msgBox = new QMessageBox(this);
        msgBox->setText(database.lastError().text());
        msgBox->setIcon(QMessageBox::Critical);
        msgBox->exec();
        delete msgBox;
    }
}

/*
 * This event is fired whenever an item is selected in the SolutionTreeWidget by the user. It set the selected database (if the selected item is a database)
 * as the active one (or the one that points by the global database object).
 */
void MainWindow::onSelectedItemChanged(QTreeWidgetItem *item, SolutionTreeWidget::SelectedItemType t)
{
    // databases cannot be nullptr, throw an exception if any occurred.
    if (t == SolutionTreeWidget::Database)
    {
        Q_ASSERT(item);
    }

    // check for empty items | when the items are removed from the solution tree and if there are no items left, the item will be a nullptr.
    // so this check is useful for scenario where the topLevelItemCount is zero.
    if (!item)
    {
        database.close();
        setSelectedDatabaseIndicatorVisible("Empty");
        return;
    }

    // normal use when the selected items being changed
    switch (t)
    {
    // if selected item is a database, set the gloabal database object to point to the selected one, and open it...
    case SolutionTreeWidget::SelectedItemType::Database:
        database.setDatabaseName(item->toolTip(0));
        database.open();
        setSelectedDatabaseIndicatorVisible(item->text(0));
        break;

    // if selected item is a table, set it's parent (the database in this case) as the global database object, and open it...
    case SolutionTreeWidget::SelectedItemType::Table:
        if (item->parent())
        {
            database.setDatabaseName(item->parent()->toolTip(0));
            database.open();
            setSelectedDatabaseIndicatorVisible(item->parent()->text(0));
        }

        break;

    default:
        break;
    }
}

/*
 * set text (sql command) to the editor
 */
void MainWindow::onStatementRequested(QString command)
{
    editor->setText(command);
}

/*
 * show the table generator
 */
void MainWindow::onTableGeneratorRequested()
{
    TblGenerator tblgen(this);
    tblgen.resize(455, 350);
    if (tblgen.exec() == QDialog::Accepted)
    {
        editor->setText(tblgen.Generate());
        editor->document()->setModified(true);
    }
}

/*
 * Saves whatever typed in the editor (TextEdit) as a .sql document
 */
bool MainWindow::fileSave()
{
    if (fileName.isEmpty())
        return fileSaveAs();

    QFile document(fileName);
    if (document.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream textStream(&document);
        textStream << editor->toPlainText() << flush;
        document.close();
        return true;
    }
    else
        return false;
}

/*
 * Saves whatever typed in the editor (TextEdit) as a .sql document in a new location
 */
bool MainWindow::fileSaveAs()
{
    QFileDialog fileDialog(this, tr("Save as..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setNameFilter("structured query language file (*.sql)");
    fileDialog.setDefaultSuffix("sql");
    if (fileDialog.exec() != QDialog::Accepted)
        return false;
    const QString fn = fileDialog.selectedFiles().first();
    fileName = fn;
    return fileSave();
}

/*
 * Print whatever typed in the editor (TextEdit) as a PDF, XMS or any custom format
 */
void MainWindow::filePrint()
{
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (editor->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted)
        editor->print(&printer);
    delete dlg;
#endif
}

/*
 * Generate a PDF File containing text typed in the editor (TextEdit)
 */
void MainWindow::filePrintPdf()
{
#ifndef QT_NO_PRINTER
    QFileDialog fileDialog(this, tr("Export PDF"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    editor->document()->print(&printer);
#endif
}

/*
 * Set the currently selected database name visible on the @code SelectedDatbaseIndicator ComboBox.
 */
void MainWindow::setSelectedDatabaseIndicatorVisible(const QString &txt)
{
    if (txt.isEmpty() || txt.isNull()) return;

    // Remove the old item (if any) that is present already
    if (selectedDatabaseIndicatorComboBox->count() > 0)
        selectedDatabaseIndicatorComboBox->removeItem(0);

    // Set the new item to be shown
    selectedDatabaseIndicatorComboBox->addItem(txt);
    selectedDatabaseIndicatorComboBox->setCurrentIndex(0);
}

/*
 * Save Application Settings
 */
void MainWindow::WriteSettings()
{
    recentFileLists.clear();
    for (int i = 0; i < solutionTree->topLevelItemCount(); ++i)
    {
        recentFileLists.removeAll(solutionTree->topLevelItem(i)->toolTip(0));
        recentFileLists.prepend(solutionTree->topLevelItem(i)->toolTip(0));
    }

    QSettings m_settings;
    m_settings.setValue("RecentFiles", recentFileLists);
    m_settings.setValue("IsTextVisibleOnToolButtons", ui->actionShowTextOnToolbar->isChecked());
    m_settings.setValue("WindowState", saveState());
#ifdef Q_OS_WIN
    m_settings.setValue("IsWindowsNativeThemeSet", ui->actionNativeWindowsUI->isChecked());
#endif
}

/*
 * Load all the application settings
 */
void MainWindow::ReadSettings()
{
    QSettings m_settings;

    recentFileLists = m_settings.value("RecentFiles").toStringList();
    ui->actionShowTextOnToolbar->setChecked(m_settings.value("IsTextVisibleOnToolButtons", false).toBool());
    restoreState(m_settings.value("WindowState").toByteArray());

#ifdef Q_OS_WIN
    ui->actionNativeWindowsUI->setChecked(m_settings.value("IsWindowsNativeThemeSet", true).toBool());

    if (ui->actionNativeWindowsUI->isChecked())
        qApp->setStyle("windowsvista");
    else
        qApp->setStyle("fusion");
#endif

    if (ui->actionShowTextOnToolbar->isChecked())
        setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    else
        setToolButtonStyle(Qt::ToolButtonFollowStyle);

    //! Recently opened file list
    //!
    if (recentFileLists.count() == 0)
        return;

    ui->menuFile->addSeparator();
    QMenu* recentFileMenu = new QMenu(tr("Recent Files"), this);
    recentFileMenu->setIcon(QIcon(resource + "fileopen.png"));
    ui->menuFile->addMenu(recentFileMenu);

    foreach (const QString& str, recentFileLists)
    {
        auto strippedName = [](QString str)
        {
            return QFileInfo(str).fileName();
        };

        recentFileMenu->addAction(strippedName(str), this, [&, str]()
        {
            if (!QFile::exists(str))
            {
                QMessageBox msgBox(this);
                msgBox.setWindowTitle(tr("Loading in Solution Explorar Failed"));
                msgBox.setText(tr("Could not find selected file in path to load in File Explorar."));
                msgBox.setDetailedText(QString("The file you selected, %1, cannot be found in the file system. could be deleted or renamed from outside, or moved to another location.").arg(str));
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.exec();
                return;
            }

            bool isLoaded = load(str);
            if (isLoaded)
            {
                solutionTree->addItemTotheExplorar(str);
                loadTablesToTheSelectedDatabase();
            }
        });
    }
}

/*
 * change the current font of the editor
 */
void MainWindow::textFamily(const QFont &f)
{
    editor->setFont(f);
}







