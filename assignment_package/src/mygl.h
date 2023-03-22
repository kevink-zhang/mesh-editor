#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include <scene/mesh.h>
#include "camera.h"
#include "scene/joint.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

class VertexDisplay : public Drawable {
public:
    Vertex *representedVertex;
    // Creates VBO data to make a visual
    // representation of the currently selected Vertex
    VertexDisplay(OpenGLContext *mp_context);
    virtual void create();
    virtual GLenum drawMode();
    // Change which Vertex representedVertex points to

};

class EdgeDisplay : public Drawable {
public:
    HalfEdge *representedEdge;
    // Creates VBO data to make a visual
    // representation of the currently selected Vertex
    EdgeDisplay(OpenGLContext *mp_context);
    virtual void create();
    virtual GLenum drawMode();
    // Change which Vertex representedVertex points to

};

class FaceDisplay : public Drawable {
public:
    Face *representedFace;
    // Creates VBO data to make a visual
    // representation of the currently selected Vertex
    FaceDisplay(OpenGLContext *mp_context);
    virtual void create();
    virtual GLenum drawMode();
    // Change which Vertex representedVertex points to

};

class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    SquarePlane m_geomSquare;// The instance of a unit cylinder we can use to render any cylinder
    Mesh m_mesh;
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)
    ShaderProgram m_progSkeleton;

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_glCamera;
    std::vector<uPtr<Joint>> joints;
    Joint* jsonRecursion(QJsonObject j, Joint*);

public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void selectJoint(int);

    VertexDisplay m_vertDisplay;
    EdgeDisplay m_edgeDisplay;
    FaceDisplay m_faceDisplay;
    Joint* m_jointDisplay;

protected:
    void keyPressEvent(QKeyEvent *e);
signals:
    void sig_sendFaceListNode(QListWidgetItem*);
    void sig_sendVertexListNode(QListWidgetItem*);
    void sig_sendEdgeListNode(QListWidgetItem*);
    void sig_sendRootNode(QTreeWidgetItem*);
    void sig_faceclick(QListWidgetItem*);
    void sig_edgeclick(QListWidgetItem*);
    void sig_vertclick(QListWidgetItem*);
    void sig_jointclick(QTreeWidgetItem*);

public slots:
    void slot_loadobj();
    void slot_loadjson();

    void slot_skinmesh();
    void slot_catmullclark();
    void slot_splitedge();
    void slot_triangulateface();

    void slot_setRed(double value);
    void slot_setGreen(double value);
    void slot_setBlue(double value);

    void slot_setX(double value);
    void slot_setY(double value);
    void slot_setZ(double value);

    void slot_setJointX(double value);
    void slot_setJointY(double value);
    void slot_setJointZ(double value);

    void slot_rotateJointX();
    void slot_rotateJointY();
    void slot_rotateJointZ();
    void slot_rotateJointNX();
    void slot_rotateJointNY();
    void slot_rotateJointNZ();

    void slot_setSharpness(double value);
};


#endif // MYGL_H
