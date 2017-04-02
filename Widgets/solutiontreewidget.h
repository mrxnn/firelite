#ifndef SOLUTIONTREEWIDGET_H
#define SOLUTIONTREEWIDGET_H

#include <QWidget>
#include <QTreeWidget>

class SolutionTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    SolutionTreeWidget(QWidget* parent = nullptr);
    enum SelectedItemType
    {
        // Indicates non has selected
        None,

        // folder is selected
        Folder,

        // database is selected
        Database,

        // table
        Table
    };

    void addItemTotheExplorar(const QString&);
    void addItemToTheSelectedNode(const QString&);
    SelectedItemType getSelectedItemType();

signals:
    void selectedItemChanged(QTreeWidgetItem*, SelectedItemType t);

    // Context Menu Related Signals
    void tableGeneratorRequested();
    void statementRequested(QString command);
    void statementAppendRequested(QString command);

private slots:
    void OnItemSelectionChanged();
    void prepareMenu(const QPoint& pos);

private:
    QTreeWidgetItem* currentlySelectedDatabase;
    QTreeWidgetItem* currentlySelectedTable;

    QString strippedName(const QString& path) const;
};

#endif // SOLUTIONTREEWIDGET_H
