#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFontComboBox>
#include <QComboBox>
#include <QCompleter>
#include <QAbstractItemModel>
#include <QTableView>
#include <QListWidget>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDockWidget>

#include "Widgets/textedit.h"
#include "Widgets/solutiontreewidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // type of the query that was executed
    enum ExecuteQueryType
    {
        CreateStatement,
        DropStatement,
        SelectStatement,
        OtherStatement
    };

protected:
    void closeEvent(QCloseEvent* e) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent* e) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent* e) Q_DECL_OVERRIDE;

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionRun_triggered();

    void onSelectedItemChanged(QTreeWidgetItem* item, SolutionTreeWidget::SelectedItemType t);
    void onStatementRequested(QString command);
    void onTableGeneratorRequested();
    void textFamily(const QFont& f);

    //! file
    bool fileSave();
    bool fileSaveAs();
    void filePrint();
    void filePrintPdf();

#ifdef Q_OS_WIN
    void on_actionNativeWindowsUI_triggered(bool checked);
#endif
    void on_actionShowTextOnToolbar_triggered(bool checked);
    void on_actionAbout_triggered();
    void on_actionAbout_Framework_triggered();

private:
    Ui::MainWindow *ui;
    QString fileName;

    //! toolbar
    QFontComboBox* fontsComboBox;
    QComboBox* fontSizeComboBox;
    QComboBox* selectedDatabaseIndicatorComboBox;
    void setSelectedDatabaseIndicatorVisible(const QString& txt);

    //! auto complete
    QAbstractItemModel* modelFromFile(const QString& fileName);
    QCompleter* completer;

    //! Window UI
    SolutionTreeWidget* solutionTree;
    TextEdit* editor = nullptr;
    QTableView* tableView;
    QListWidget* activityLog;
    QTabWidget* resultPanel;

    //! database
    QSqlDatabase database;
    QSqlQueryModel* tableModel;
    bool load(const QString& str);
    QString getQueryResult(const QString& command, int rows);
    ExecuteQueryType getQueryType(const QString &query, QString& message, int rows);
    void loadTablesToTheSelectedDatabase();

    //! database Error Reporting
    void checkLastErrorIfAny(QSqlQuery* query = nullptr);

    //! settings
    void ReadSettings();
    void WriteSettings();
    QStringList recentFileLists;

    void initializeIcons();
    void initializeToolbars();
    void initializeUI();
    void bind();
};

#endif // MAINWINDOW_H
