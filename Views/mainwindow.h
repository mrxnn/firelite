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

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();

private:
    Ui::MainWindow *ui;

    // Toolbar
    QFontComboBox* fontsComboBox;
    QComboBox* fontSizeComboBox;
    QComboBox* selectedDatabaseIndicatorComboBox;

    // Auto Complete
    QAbstractItemModel* modelFromFile(const QString& fileName);
    QCompleter* completer;

    // Window UI
    SolutionTreeWidget* solutionTree;
    TextEdit* editor;
    QTableView* tableView;
    QListWidget* activityLog;
    QTabWidget* resultPanel;

    // Database
    QSqlDatabase database;
    QSqlQueryModel* tableModel;
    bool load(const QString& str);

    // Database Error Reporting
    void checkLastErrorIfAny();

    void initializeIcons();
    void initializeToolbars();
    void initializeUI();
    void bind();
};

#endif // MAINWINDOW_H
