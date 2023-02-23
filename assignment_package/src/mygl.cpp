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

VertexDisplay::VertexDisplay(OpenGLContext *mp_context) : Drawable(mp_context) {

}
EdgeDisplay::EdgeDisplay(OpenGLContext *mp_context) : Drawable(mp_context) {

}
FaceDisplay::FaceDisplay(OpenGLContext *mp_context) : Drawable(mp_context) {

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
    std::vector<glm::vec4> pos = {glm::vec4(representedVertex -> pos, 1)};

    std::vector<glm::vec4> col = {glm::vec4(1)};

    std::vector<GLuint> idx = {0};

    count = 1;

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

}
void FaceDisplay::create() {

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
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2,0,0)) * glm::rotate(glm::mat4(), 0.25f * 3.14159f, glm::vec3(0,1,0));
    //Send the geometry's transformation matrix to the shader
    m_progLambert.setModelMatrix(model);
    //Draw the example sphere using our lambert shader
//    m_progLambert.draw(m_geomSquare);

//    //Now do the same to render the cylinder
//    //We've rotated it -45 degrees on the Z axis, then translated it to the point <2,2,0>
//    model = glm::translate(glm::mat4(1.0f), glm::vec3(2,2,0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0,0,1));
//    m_progLambert.setModelMatrix(model);
//    m_progLambert.draw(m_geomSquare);

    //draw the mesh
    m_progFlat.draw(m_mesh);
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

    while (!stream.atEnd()){
        line = stream.readLine().split(' ');
        //empty line
        if(line.length() == 0) continue;
        //vertex
        if(line[0] == "v") {
            m_mesh.v.push_back(glm::vec4(line[1].toFloat(), line[2].toFloat(), line[3].toFloat(), 1.0));
        }
        else if(line[0] == "vt") {
            m_mesh.vt.push_back(glm::vec2(line[1].toFloat(), line[2].toFloat()));
        }
        else if(line[0] == "vn") {
            m_mesh.vn.push_back(glm::vec4(line[1].toFloat(), line[2].toFloat(), line[3].toFloat(), 1.0));
        }
        else if(line[0] == "f") {
            std::vector<glm::vec3> someface;
            for(int i = 1; i < line.length(); i++) {
                QStringList faceinfo = line[i].split('/');
                someface.push_back(glm::vec3(faceinfo[0].toInt(), faceinfo[1].toInt(), faceinfo[2].toInt()));
            }
            m_mesh.f.push_back(someface);
        }
    }

    m_mesh.create();

    for(int i = 0; i < m_mesh.faces.size(); i++) {
        emit sig_sendFaceListNode(m_mesh.faces[i].get());
    }

    for(int i = 0; i < m_mesh.vertices.size(); i++) {
        emit sig_sendVertexListNode(m_mesh.vertices[i].get());
    }

    for(int i = 0; i < m_mesh.halfedges.size(); i++) {
        emit sig_sendEdgeListNode(m_mesh.halfedges[i].get());
    }
}
