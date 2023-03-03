#include "mygl.h"
#include <la.h>
#include "tiny_obj_loader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QPixmap>
#include <QFileDialog>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QImageWriter>
#include <QDebug>

VertexDisplay::VertexDisplay(OpenGLContext *mp_context) : Drawable(mp_context), representedVertex(nullptr){

}
EdgeDisplay::EdgeDisplay(OpenGLContext *mp_context) : Drawable(mp_context), representedEdge(nullptr) {

}
FaceDisplay::FaceDisplay(OpenGLContext *mp_context) : Drawable(mp_context), representedFace(nullptr) {

}
GLenum VertexDisplay::drawMode() {
    return GL_POINTS;
}
GLenum EdgeDisplay::drawMode() {
    return GL_LINES;
}
GLenum FaceDisplay::drawMode() {
    return GL_LINES;
}
void VertexDisplay::create() {
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> col;
    std::vector<GLuint> idx;

    count = 0;

    if(representedVertex) {
         pos = {glm::vec4(representedVertex -> pos, 1)};

         col = {glm::vec4(1)};

         idx = {0};

         count = 1;
    }


    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}
void EdgeDisplay::create() {
    std::vector<glm::vec4> pos = {
    };
    std::vector<glm::vec4> col = {
    };
    std::vector<GLuint> idx = {};

    count = 0;

    if(representedEdge) {
        HalfEdge* behind = representedEdge;
        while(behind->next->id!=representedEdge->id){
            behind = behind->next;
        }

        pos = {glm::vec4(behind->node->pos, 1),glm::vec4(representedEdge->node->pos, 1)};

        col = {glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1)};

        idx = {0, 1};

        count = 2;
    }


    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}
void FaceDisplay::create() {
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> col;
    std::vector<GLuint> idx;

    count = 0;

    if(representedFace) {
        HalfEdge* edgeAt = representedFace ->edge;
        do{
            pos.push_back(glm::vec4(edgeAt ->node -> pos, 1));
            pos.push_back(glm::vec4(edgeAt -> next->node -> pos, 1));

            edgeAt = edgeAt -> next;

            col.push_back(glm::vec4(glm::vec3(1) - representedFace->color, 1));
            col.push_back(glm::vec4(glm::vec3(1) - representedFace->color, 1));

            idx.push_back(count);
            idx.push_back(count+1);
            count += 2;
        }
        while(edgeAt != representedFace -> edge);

    }

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}



MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_geomSquare(this),
      m_mesh(this),
      m_progLambert(this), m_progFlat(this),
      m_glCamera(),
      m_vertDisplay(this),
      m_faceDisplay(this),
      m_edgeDisplay(this)
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_geomSquare.destroy();
    m_mesh.destroy();
    m_vertDisplay.destroy();
    m_faceDisplay.destroy();
    m_edgeDisplay.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
    //m_geomSquare.create();
    m_mesh.create();

    //setup the displays
    m_vertDisplay.create();
    m_faceDisplay.create();
    m_edgeDisplay.create();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");


    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_glCamera = Camera(w, h);
    glm::mat4 viewproj = m_glCamera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_progFlat.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setCamPos(m_glCamera.eye);
    m_progFlat.setModelMatrix(glm::mat4(1.f));

    //Create a model matrix. This one rotates the square by PI/4 radians then translates it by <-2,0,0>.
    //Note that we have to transpose the model matrix before passing it to the shader
    //This is because OpenGL expects column-major matrices, but you've
    //implemented row-major matrices.
    //Send the geometry's transformation matrix to the shader
    m_progLambert.setModelMatrix(glm::mat4(1.f));

    //draw the mesh

    m_progLambert.draw(m_mesh);
    glDisable(GL_DEPTH_TEST);
    m_progFlat.draw(m_faceDisplay);
    m_progFlat.draw(m_edgeDisplay);
    m_progFlat.draw(m_vertDisplay);
    glEnable(GL_DEPTH_TEST);
}


void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_glCamera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_glCamera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_glCamera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_glCamera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        m_glCamera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        m_glCamera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        m_glCamera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        m_glCamera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        m_glCamera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        m_glCamera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        m_glCamera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        m_glCamera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        m_glCamera = Camera(this->width(), this->height());
    } else if (e->key() == Qt::Key_N) {
        if(m_edgeDisplay.representedEdge) {
            m_edgeDisplay.representedEdge = m_edgeDisplay.representedEdge -> next;
            m_edgeDisplay.create();
            emit sig_edgeclick(m_edgeDisplay.representedEdge);
        }
    } else if (e->key() == Qt::Key_M) {
        if(m_edgeDisplay.representedEdge) {
            m_edgeDisplay.representedEdge = m_edgeDisplay.representedEdge -> mirror;
            m_edgeDisplay.create();
            emit sig_edgeclick(m_edgeDisplay.representedEdge);
        }
    } else if (e->key() == Qt::Key_F) {
        if(m_edgeDisplay.representedEdge) {
            m_faceDisplay.representedFace = m_edgeDisplay.representedEdge -> face;
            m_faceDisplay.create();
            emit sig_faceclick(m_faceDisplay.representedFace);
        }
    } else if (e->key() == Qt::Key_V) {
        if(m_edgeDisplay.representedEdge) {
            m_vertDisplay.representedVertex = m_edgeDisplay.representedEdge -> node;
            m_vertDisplay.create();
            emit sig_vertclick(m_vertDisplay.representedVertex);
        }
    } else if (e->key() == Qt::Key_H) {
        if(e->modifiers() & Qt::ShiftModifier) {
            if(m_faceDisplay.representedFace) {
                m_edgeDisplay.representedEdge = m_faceDisplay.representedFace -> edge;
                m_edgeDisplay.create();
            }
        }
        else{
            if(m_vertDisplay.representedVertex) {
                m_edgeDisplay.representedEdge = m_vertDisplay.representedVertex -> edge;
                m_edgeDisplay.create();
            }
        }
        emit sig_edgeclick(m_edgeDisplay.representedEdge);
    }
    m_glCamera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}

void MyGL::slot_loadobj() {
    QString filename = QFileDialog::getOpenFileName(0, QString("Load Scene File"), QDir::currentPath().append(QString("../..")), QString("*.obj"));
    int i = filename.length() - 1;
    while(QString::compare(filename.at(i), QChar('/')) != 0)
    {
        i--;
    }
    QString local_path = filename.left(i+1);

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)){
        qWarning("Could not open the OBJ file.");
        return;
    }

    QStringList line;
    QTextStream stream(&file);

    m_mesh.clear();

    m_vertDisplay.representedVertex = nullptr;
    m_edgeDisplay.representedEdge = nullptr;
    m_faceDisplay.representedFace = nullptr;
    m_vertDisplay.create();
    m_edgeDisplay.create();
    m_faceDisplay.create();
    emit sig_edgeclick(nullptr);
    emit sig_faceclick(nullptr);
    emit sig_vertclick(nullptr);

    std::vector<glm::vec4> v;
    std::vector<glm::vec2> vt;
    std::vector<glm::vec4> vn;
    std::vector<std::vector<glm::vec3>> f;

    while (!stream.atEnd()){
        line = stream.readLine().split(' ');
        //empty line
        if(line.length() == 0) continue;
        //vertex
        if(line[0] == "v") {
            v.push_back(glm::vec4(line[1].toFloat(), line[2].toFloat(), line[3].toFloat(), 1.0));
        }
        else if(line[0] == "vt") {
            vt.push_back(glm::vec2(line[1].toFloat(), line[2].toFloat()));
        }
        else if(line[0] == "vn") {
            vn.push_back(glm::vec4(line[1].toFloat(), line[2].toFloat(), line[3].toFloat(), 1.0));
        }
        else if(line[0] == "f") {
            std::vector<glm::vec3> someface;
            for(int i = 1; i < line.length(); i++) {
                QStringList faceinfo = line[i].split('/');
                someface.push_back(glm::vec3(faceinfo[0].toInt(), faceinfo[1].toInt(), faceinfo[2].toInt()));
            }
            f.push_back(someface);
        }
    }

    //initalize mesh vertices
    for(glm::vec4 vertex: v) {
        m_mesh.makeVert(glm::vec3(vertex));
    }

    //for sym edge finding
    std::map<long, HalfEdge*> symFinder;
    long symMax = m_mesh.getVert(-1)->id + 1; //last added vertex should have maximal id

    //initialize mesh faces
    for(std::vector<glm::vec3> new_face : f) {
        m_mesh.makeFace();

        std::vector<HalfEdge*> new_edges;

        for(glm::vec3 vertex : new_face) {
            //VBO
            int vi = vertex[0]-1;

            //create a new half-edge, set it as the edge pointer of the vertex and face, and add to halfedges
            new_edges.push_back(m_mesh.makeEdge(m_mesh.getFace(-1), m_mesh.getVert(vi)));
            m_mesh.getVert(vi) -> edge = m_mesh.getEdge(-1);
            m_mesh.getFace(-1)->edge = m_mesh.getEdge(-1);
        }

        int num_edges = new_edges.size();
        for(int i = 0; i < num_edges; i++) {
            new_edges[i]->next = new_edges[(i+1)%num_edges];
            //grab the endpoint vertex ids for the half-edge
            int v1 = new_edges[(i-1+num_edges)%num_edges] -> node -> id;
            int v2 = new_edges[i] -> node -> id;
            //checks if sym edge already exists, and links the two
            if(symFinder.count(v2*symMax + v1)) {
                new_edges[i]->mirror = symFinder[v2*symMax + v1];
                symFinder[v2*symMax + v1] -> mirror = new_edges[i];
                symFinder.erase(v2*symMax + v1);
            }
            else {
                symFinder[v1*symMax + v2] = new_edges[i];
            }
        }
    }


    m_mesh.create();

    for(int i = 0; i < m_mesh.faces.size(); i++) {
        emit sig_sendFaceListNode(m_mesh.getFace(i));
    }

    for(int i = 0; i < m_mesh.vertices.size(); i++) {
        emit sig_sendVertexListNode(m_mesh.getVert(i));
    }

    for(int i = 0; i < m_mesh.halfedges.size(); i++) {
        emit sig_sendEdgeListNode(m_mesh.getEdge(i));
    }
}

void MyGL::slot_catmullclark() {
    long old_size = m_mesh.vertices.size();
    m_mesh.CatmullClark();

    for(int i = 0; i < m_mesh.faces.size(); i++) {
        emit sig_sendFaceListNode(m_mesh.getFace(i));
    }

    for(int i = old_size; i < m_mesh.vertices.size(); i++) {
        emit sig_sendVertexListNode(m_mesh.getVert(i));
    }

    for(int i = 0; i < m_mesh.halfedges.size(); i++) {
        emit sig_sendEdgeListNode(m_mesh.getEdge(i));
    }

    m_vertDisplay.representedVertex = nullptr;
    m_edgeDisplay.representedEdge = nullptr;
    m_faceDisplay.representedFace = nullptr;
    m_vertDisplay.create();
    m_edgeDisplay.create();
    m_faceDisplay.create();
    m_mesh.create();
    update();
}

void MyGL::slot_splitedge() {
    if(!m_edgeDisplay.representedEdge) return;
    HalfEdge* e1 = m_edgeDisplay.representedEdge;
    HalfEdge* e2 = m_edgeDisplay.representedEdge->mirror;
    Vertex* v1 = e1->node;
    Vertex* v2 = e2->node;
    //average endpoints for new position
    Vertex* v3 = m_mesh.makeVert((v1->pos + v2->pos)/2.f);
    HalfEdge* ee1 = m_mesh.makeEdge(e1->face, v1);
    ee1->next = e1->next;
    e1->next = ee1;
    e1->node = v3;
    HalfEdge* ee2 = m_mesh.makeEdge(e2->face, v2);
    ee2->next = e2->next;
    e2->next = ee2;
    e2->node = v3;
    //set sym
    ee1->mirror = e2;
    e2->mirror = ee1;
    ee2->mirror = e1;
    e1->mirror = ee2;
    //set edge vertex pointers
    v3->edge = e1;
    v1->edge = ee1;
    v2->edge = ee2;

    emit sig_sendEdgeListNode(m_mesh.getEdge(-2));
    emit sig_sendEdgeListNode(m_mesh.getEdge(-1));
    emit sig_sendVertexListNode(m_mesh.getVert(-1));
    m_mesh.getEdge(-2)->sharp = e1->sharp;
    m_mesh.getEdge(-1)->sharp = e1->sharp;
    m_mesh.getVert(-1)->sharp = e1->sharp;

    m_edgeDisplay.representedEdge = nullptr;
    m_edgeDisplay.create();
    update();
}

void MyGL::slot_triangulateface() {
    if(!m_faceDisplay.representedFace) return;

    //to triangulate, we will modify the current face and add in n-3 new faces
    HalfEdge* start = m_faceDisplay.representedFace->edge;
    HalfEdge* edgeAt = m_faceDisplay.representedFace->edge;
    int edge_count = m_mesh.halfedges.size();
    int face_count = m_mesh.faces.size();

    //need to store edges to modify ->next at once
    std::vector<HalfEdge*> oldEdges;
    do{
        oldEdges.push_back(edgeAt);
        edgeAt = edgeAt->next;
    } while(edgeAt != start);
    Vertex* pivot = oldEdges[oldEdges.size()-1]->node;

    //iterate through the edges, do not use ->next
    //the edges we make are symmetric, so bind those to each other
    HalfEdge* last_sym = nullptr;
    for(int i = 1; i <= oldEdges.size()-2; i++) {
        Face* this_face;
        if(i == 1) {
            //use this face
            this_face = m_faceDisplay.representedFace;
            Vertex* v1 = oldEdges[0] -> node;

            //add in new half edges
            pivot->edge = m_mesh.makeEdge(this_face, pivot);
            v1->edge = oldEdges[0];
            pivot->edge->next = v1->edge;
            v1->edge->next = oldEdges[1];
            oldEdges[1]->next = pivot->edge;
            //initialize symmetric pointer
            last_sym = pivot->edge;
        }
        else if(i == oldEdges.size()-2) {
            //make new face
            this_face = m_mesh.makeFace();
            this_face->edge = oldEdges[i];
            Vertex* v1 = oldEdges[(i-1+oldEdges.size())%oldEdges.size()] -> node;

            //add in new half edges
            pivot->edge = oldEdges[i+1];
            v1->edge = m_mesh.makeEdge(this_face, v1);
            pivot->edge->next = v1->edge;
            v1->edge->next = oldEdges[i];
            oldEdges[i]->next = pivot->edge;
            //set sym
            v1->edge->mirror = last_sym;
            last_sym->mirror = v1->edge;
        }
        else{
            //make new face
            this_face = m_mesh.makeFace();
            this_face->edge = oldEdges[i];
            Vertex* v1 = oldEdges[(i-1+oldEdges.size())%oldEdges.size()] -> node;

            //add in new half edges
            pivot->edge = m_mesh.makeEdge(this_face, pivot);
            v1->edge = m_mesh.makeEdge(this_face, v1);
            pivot->edge->next = v1->edge;
            v1->edge->next = oldEdges[i];
            oldEdges[i]->next = pivot->edge;
            //set sym
            v1->edge->mirror = last_sym;
            last_sym->mirror = v1->edge;
            last_sym = pivot->edge;
        }
    }

    //create the widgets and do sharpness
    for(int i = face_count; i < m_mesh.faces.size(); i++) {
        emit sig_sendFaceListNode(m_mesh.getFace(i));
        m_mesh.getFace(i) -> makeSharp(m_faceDisplay.representedFace->sharp);
    }

    for(int i = edge_count; i < m_mesh.halfedges.size(); i++) {
        emit sig_sendEdgeListNode(m_mesh.getEdge(i));
    }

    m_faceDisplay.representedFace = nullptr;
    m_faceDisplay.create();
    m_mesh.create();
    update();
}

void MyGL::slot_setRed(double value) {
    if(!m_faceDisplay.representedFace) return;
    m_faceDisplay.representedFace->color[0] = value;
    m_mesh.create();
    m_faceDisplay.create();
    update();
}
void MyGL::slot_setGreen(double value) {
    if(!m_faceDisplay.representedFace) return;
    m_faceDisplay.representedFace->color[1] = value;
    m_mesh.create();
    m_faceDisplay.create();
    update();
}
void MyGL::slot_setBlue(double value) {
    if(!m_faceDisplay.representedFace) return;
    m_faceDisplay.representedFace->color[2] = value;
    m_mesh.create();
    m_faceDisplay.create();
    update();
}

void MyGL::slot_setX(double value) {
    if(!m_vertDisplay.representedVertex) return;
    m_vertDisplay.representedVertex->pos[0] = value;
    m_mesh.create();
    m_vertDisplay.create();
    update();
}

void MyGL::slot_setY(double value) {
    if(!m_vertDisplay.representedVertex) return;
    m_vertDisplay.representedVertex->pos[1] = value;
    m_mesh.create();
    m_vertDisplay.create();
    update();
}

void MyGL::slot_setZ(double value) {
    if(!m_vertDisplay.representedVertex) return;
    m_vertDisplay.representedVertex->pos[2] = value;
    m_mesh.create();
    m_vertDisplay.create();
    update();
}

void MyGL::slot_setSharpness(double value) {
    m_mesh.sharpness = value;
}

