#include "solutiontreewidget.h"

#include <QFileInfo>
#include <QtDebug>
#include <QAction>
#include <QMenu>

SolutionTreeWidget::SolutionTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);

    // Header Item
    auto h = new QTreeWidgetItem;
    h->setIcon(0, QIcon(":/Resources/Tree/foldertree.png"));
    h->setText(0, "(Localdb) | SQLITELocalDB | 160.67.0.1");
    h->setFont(0, QFont("Calibri"));
    setHeaderItem(h);
    setHeaderHidden(false);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &SolutionTreeWidget::itemSelectionChanged, this, &SolutionTreeWidget::OnItemSelectionChanged);
    connect(this, &SolutionTreeWidget::customContextMenuRequested, this, &SolutionTreeWidget::prepareMenu);
}

// Adds a new database to the database expolorar
void SolutionTreeWidget::addItemTotheExplorar(const QString& path)
{
    // Return if the path is null or empty
    if (path.isNull() || path.isEmpty())
        return;

    // Create a new tree item and setting up its properties
    auto qtvi = new QTreeWidgetItem;
    qtvi->setText(0, strippedName(path));
    qtvi->setToolTip(0, path);
    qtvi->setIcon(0, QIcon(":/Resources/Tree/folder.png"));

    // Finally, add the tree item to the explorar
    this->addTopLevelItem(qtvi);
    setCurrentItem(qtvi);
}

// Adds a new table to the selected database
void SolutionTreeWidget::addItemToTheSelectedNode(const QString& path)
{
    // Return if the path is null or empty
    if (path.isNull() || path.isEmpty())
        return;

    // Create a new tree item and setting up its properties
    auto qtvi = new QTreeWidgetItem;
    qtvi->setText(0, strippedName(path));
    qtvi->setToolTip(0, path);
    qtvi->setIcon(0, QIcon(":/Resources/Tree/table.png"));

    // Finally, add the tree item to the currently selected database
    auto selecteddb = this->currentItem();
    if (selecteddb)
        selecteddb->addChild(qtvi);
    else
        delete qtvi;
}

// Returns the type of the item that is currently selected. Could be None, Folder, Database or a Table
SolutionTreeWidget::SelectedItemType SolutionTreeWidget::getSelectedItemType()
{
    // Return if nothing has changed
    auto changedItem = currentItem();
    if (!changedItem)
        return SelectedItemType::None;

    // Make sure the changed item is the selected item
    changedItem->setSelected(true);

    if (changedItem->parent() == nullptr)
    {
        // database is selected
        return SelectedItemType::Database;
    }
    else
    {
        // table is selected
        return SelectedItemType::Table;
    }
}

// Accepts a local URL and returns the absolute file name
QString SolutionTreeWidget::strippedName(const QString &path) const
{
    return QFileInfo(path).fileName();
}

// Fires whenever the selected item is changed
void SolutionTreeWidget::OnItemSelectionChanged()
{
    // Return if nothing has changed
    auto changedItem = currentItem();
    if (!changedItem)
        return;

    // Make the changed item, selected item
    changedItem->setSelected(true);

    if (changedItem->parent() == nullptr)
    {
        // database is selected
        emit selectedItemChanged(changedItem, SelectedItemType::Database);
    }
    else
    {
        // table is selected
        emit selectedItemChanged(changedItem, SelectedItemType::Table);
    }
}

void SolutionTreeWidget::prepareMenu(const QPoint &pos)
{
    // Get the selected item type
    auto st = getSelectedItemType();
    if (st == SelectedItemType::None)
        return;

    // Get a pointer to the selected item
    QTreeWidgetItem* nd = itemAt(pos);
    if (!nd) return;

    // Making actions depending on the type that is selected
    if (st == SelectedItemType::Database)
    {   
        QAction* actionRemoveDatabase = new QAction(tr("Remove File")); actionRemoveDatabase->setFont(QFont("Calibri"));
        QAction* actionNewDatabase = new QAction(tr("Create Table")); actionNewDatabase->setFont(QFont("Calibri"));
        QAction* actionExpandAll = new QAction(tr("Expand All")); actionExpandAll->setFont(QFont("Calibri"));
        QAction* actionCollapseAll = new QAction(tr("Collapse All")); actionCollapseAll->setFont(QFont("Calibri"));

        QMenu menu(this);
        menu.addAction(actionRemoveDatabase);
        menu.addAction(actionNewDatabase);
        menu.addAction(actionExpandAll);
        menu.addAction(actionCollapseAll);

        menu.exec(this->mapToGlobal(pos));
    }
    else // table
    {
        QAction* actionSelectAllCommand = new QAction(tr("Select top 200 records"));
        QAction* actionDropCommand = new QAction(tr("Drop table"));

        QMenu menu(this);
        menu.addAction(actionSelectAllCommand);
        menu.addAction(actionDropCommand);

        menu.exec(this->mapToGlobal(pos));
    }
}













