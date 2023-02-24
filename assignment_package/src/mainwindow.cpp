#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cameracontrolshelp.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();

    //connect the load obj to button
    connect(ui->loadOBJButton, SIGNAL(clicked()),
            ui->mygl, SLOT(slot_loadobj()));

    connect(ui->mygl,
            // Signal name
            SIGNAL(sig_sendFaceListNode(QListWidgetItem*)),
            // Widget with the slot that receives the signal
            this,
            // Slot name
            SLOT(slot_addFaceToListWidget(QListWidgetItem*)));

    connect(ui->mygl,
            // Signal name
            SIGNAL(sig_sendVertexListNode(QListWidgetItem*)),
            // Widget with the slot that receives the signal
            this,
            // Slot name
            SLOT(slot_addVertexToListWidget(QListWidgetItem*)));

    connect(ui->mygl,
            // Signal name
            SIGNAL(sig_sendEdgeListNode(QListWidgetItem*)),
            // Widget with the slot that receives the signal
            this,
            // Slot name
            SLOT(slot_addEdgeToListWidget(QListWidgetItem*)));

    connect(ui->facesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                this, SLOT(slot_onFaceItemClicked(QListWidgetItem*)));
    connect(ui->vertsListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                this, SLOT(slot_onVertexItemClicked(QListWidgetItem*)));
    connect(ui->halfEdgesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                this, SLOT(slot_onEdgeItemClicked(QListWidgetItem*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}

void MainWindow::slot_addFaceToListWidget(QListWidgetItem *i) {
    ui->facesListWidget->addItem(i);
}

void MainWindow::slot_addVertexToListWidget(QListWidgetItem *i) {
    ui->vertsListWidget->addItem(i);
}

void MainWindow::slot_addEdgeToListWidget(QListWidgetItem *i) {
    ui->halfEdgesListWidget->addItem(i);
}

void MainWindow::slot_onFaceItemClicked(QListWidgetItem* item)
{
    ui->mygl->m_faceDisplay.destroy();
    ui->mygl->m_faceDisplay = FaceDisplay(ui->mygl);
    ui->mygl->m_faceDisplay.representedFace = (Face*) item;
    ui->mygl->m_faceDisplay.create();
}

void MainWindow::slot_onVertexItemClicked(QListWidgetItem* item)
{
    ui->mygl->m_vertDisplay.destroy();
    ui->mygl->m_vertDisplay = VertexDisplay(ui->mygl);
    ui->mygl->m_vertDisplay.representedVertex = (Vertex*) item;
    ui->mygl->m_vertDisplay.create();
}

void MainWindow::slot_onEdgeItemClicked(QListWidgetItem* item)
{
    ui->mygl->m_edgeDisplay.destroy();
    ui->mygl->m_edgeDisplay = EdgeDisplay(ui->mygl);
    ui->mygl->m_edgeDisplay.representedEdge = (HalfEdge*) item;
    ui->mygl->m_edgeDisplay.create();
}
