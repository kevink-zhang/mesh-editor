#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "scene/mesh.h"
#include <QGraphicsScene>
#include <QTreeWidget>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();

    void on_actionCamera_Controls_triggered();

    void slot_addFaceToListWidget(QListWidgetItem *i);

    void slot_addVertexToListWidget(QListWidgetItem *i);

    void slot_addEdgeToListWidget(QListWidgetItem *i);

    void slot_addRootToTreeWidget(QTreeWidgetItem *i);

    void slot_onFaceItemClicked(QListWidgetItem* item);

    void slot_onVertexItemClicked(QListWidgetItem* item);

    void slot_onEdgeItemClicked(QListWidgetItem* item);

    void slot_onJointItemClicked(QTreeWidgetItem* item);

    void slot_toggleEdgeSharp(int value);
    void slot_toggleFaceSharp(int value);
    void slot_toggleVertSharp(int value);
private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
