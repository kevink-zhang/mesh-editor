#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cameracontrolshelp.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();

    //spinbox stuff
    connect(ui->faceBlueSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setBlue(double)));

    connect(ui->faceRedSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setRed(double)));

    connect(ui->faceGreenSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setGreen(double)));

    connect(ui->vertPosXSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setX(double)));

    connect(ui->vertPosYSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setY(double)));

    connect(ui->vertPosZSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setZ(double)));

    connect(ui->sharpnessSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setSharpness(double)));

    //checkboxes
    connect(ui->edgeSharpCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slot_toggleEdgeSharp(int)));
    connect(ui->vertSharpCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slot_toggleVertSharp(int)));
    connect(ui->faceSharpCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slot_toggleFaceSharp(int)));


    //connect the load obj to button
    connect(ui->loadOBJButton, SIGNAL(clicked()),
            ui->mygl, SLOT(slot_loadobj()));

    //connect catmull clark button
    connect(ui->ccButton, SIGNAL(clicked()),
            ui->mygl, SLOT(slot_catmullclark()));

    //connect edge splitting button
    connect(ui->splitButton, SIGNAL(clicked()),
            ui->mygl, SLOT(slot_splitedge()));

    //connect face triangulation button
    connect(ui->triangulateButton, SIGNAL(clicked()),
            ui->mygl, SLOT(slot_triangulateface()));

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
    connect(ui->mygl, SIGNAL(sig_faceclick(QListWidgetItem*)),
                this, SLOT(slot_onFaceItemClicked(QListWidgetItem*)));
    connect(ui->mygl, SIGNAL(sig_vertclick(QListWidgetItem*)),
                this, SLOT(slot_onVertexItemClicked(QListWidgetItem*)));
    connect(ui->mygl, SIGNAL(sig_edgeclick(QListWidgetItem*)),
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
//use nullptr as a special case for reset
void MainWindow::slot_onFaceItemClicked(QListWidgetItem* item)
{
    if(!item){
        ui->faceRedSpinBox->setValue(0);
        ui->faceGreenSpinBox->setValue(0);
        ui->faceBlueSpinBox->setValue(0);
        ui->faceSharpCheckBox->setChecked(false);
        return;
    }
    ui->mygl->m_faceDisplay = FaceDisplay(ui->mygl);
    ui->mygl->m_faceDisplay.representedFace = (Face*) item;
    ui->faceRedSpinBox->setValue(ui->mygl->m_faceDisplay.representedFace->color[0]);
    ui->faceGreenSpinBox->setValue(ui->mygl->m_faceDisplay.representedFace->color[1]);
    ui->faceBlueSpinBox->setValue(ui->mygl->m_faceDisplay.representedFace->color[2]);
    ui->faceSharpCheckBox->setChecked(ui->mygl->m_faceDisplay.representedFace->sharp);
    ui->mygl->m_faceDisplay.create();
    ui->mygl->update();
}

void MainWindow::slot_onVertexItemClicked(QListWidgetItem* item)
{
    if(!item){
        ui->vertPosXSpinBox->setValue(0);
        ui->vertPosYSpinBox->setValue(0);
        ui->vertPosZSpinBox->setValue(0);
        ui->vertSharpCheckBox->setChecked(false);
        return;
    }
    ui->mygl->m_vertDisplay = VertexDisplay(ui->mygl);
    ui->mygl->m_vertDisplay.representedVertex = (Vertex*) item;
    ui->vertPosXSpinBox->setValue(ui->mygl->m_vertDisplay.representedVertex->pos[0]);
    ui->vertPosYSpinBox->setValue(ui->mygl->m_vertDisplay.representedVertex->pos[1]);
    ui->vertPosZSpinBox->setValue(ui->mygl->m_vertDisplay.representedVertex->pos[2]);
    ui->vertSharpCheckBox->setChecked(ui->mygl->m_vertDisplay.representedVertex->sharp);
    ui->mygl->m_vertDisplay.create();
    ui->mygl->update();
}

void MainWindow::slot_onEdgeItemClicked(QListWidgetItem* item)
{
    if(!item){
        ui->edgeSharpCheckBox->setChecked(false);
        return;
    }
    ui->mygl->m_edgeDisplay = EdgeDisplay(ui->mygl);
    ui->mygl->m_edgeDisplay.representedEdge = (HalfEdge*) item;
    ui->edgeSharpCheckBox->setChecked(ui->mygl->m_edgeDisplay.representedEdge->sharp);
    ui->mygl->m_edgeDisplay.create();
    ui->mygl->update();
}

void MainWindow::slot_toggleEdgeSharp(int value) {
    if(!ui->mygl->m_edgeDisplay.representedEdge) return;
    ui->mygl->m_edgeDisplay.representedEdge->sharp = value;
    ui->mygl->m_edgeDisplay.representedEdge->mirror->sharp = value;
    //if the face was sharp and edge is now not sharp, change the face to not sharp
//    if(ui->mygl->m_edgeDisplay.representedEdge->face->sharp && !value) {
//        ui->mygl->m_edgeDisplay.representedEdge->face->sharp = false;
//        if(ui->mygl->m_faceDisplay.representedFace == ui->mygl->m_edgeDisplay.representedEdge->face)
//            ui->faceSharpCheckBox->setChecked(false);
//    }
//    if(ui->mygl->m_edgeDisplay.representedEdge->mirror->face->sharp && !value) {
//        ui->mygl->m_edgeDisplay.representedEdge->mirror->face->sharp = false;
//        if(ui->mygl->m_faceDisplay.representedFace == ui->mygl->m_edgeDisplay.representedEdge->mirror->face)
//            ui->faceSharpCheckBox->setChecked(false);
//    }
}

void MainWindow::slot_toggleFaceSharp(int value) {
    if(!ui->mygl->m_faceDisplay.representedFace) return;
    ui->mygl->m_faceDisplay.representedFace->makeSharp(value);
    //change the vertex/edge if also selected
    HalfEdge* edgeAt = ui->mygl->m_faceDisplay.representedFace -> edge;
    do{
        if(edgeAt == ui->mygl->m_edgeDisplay.representedEdge){
            ui->edgeSharpCheckBox->setChecked(value);
        }
        if(edgeAt->mirror == ui->mygl->m_edgeDisplay.representedEdge){
            ui->edgeSharpCheckBox->setChecked(value);
        }
        if(edgeAt->node == ui->mygl->m_vertDisplay.representedVertex){
            ui->vertSharpCheckBox->setChecked(value);
        }
        edgeAt = edgeAt->next;
    } while(edgeAt != ui->mygl->m_faceDisplay.representedFace -> edge);
}

void MainWindow::slot_toggleVertSharp(int value) {
    if(!ui->mygl->m_vertDisplay.representedVertex) return;
    ui->mygl->m_vertDisplay.representedVertex->sharp = value;
    //find all faces containing the vertex, and repeat edge sharp logic
//    if(!value) {
//        ui->mygl->m_vertDisplay.representedVertex->edge;
//        HalfEdge* edgeAt = ui->mygl->m_vertDisplay.representedVertex->edge;
//        do{
//            if(edgeAt->face->sharp) {
//                edgeAt->face->sharp = false;
//                //uncheck the face
//                if(edgeAt->face == ui->mygl->m_faceDisplay.representedFace){
//                    ui->faceSharpCheckBox->setChecked(false);
//                }
//            }
//            //get next edge pointing into vertex
//            edgeAt = edgeAt->next->mirror;
//        } while(edgeAt != ui->mygl->m_vertDisplay.representedVertex->edge);
//    }
}


