#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFontComboBox>
#include <QComboBox>
#include <QCompleter>
#include <QAbstractItemModel>
#include <QTableView>
#include <QListWidget>

#include "Widgets/textedit.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QFontComboBox* fontsComboBox;
    QComboBox* fontSizeComboBox;
    QComboBox* selectedDatabaseIndicatorComboBox;

    // Auto Complete
    QAbstractItemModel* modelFromFile(const QString& fileName);
    QCompleter* completer;

    // Window UI
    TextEdit* editor;
    QTableView* tableView;
    QListWidget* activityLog;
    QTabWidget* resultPanel;

    void initializeIcons();
    void initializeToolbars();
    void initializeUI();
};

#endif // MAINWINDOW_H
