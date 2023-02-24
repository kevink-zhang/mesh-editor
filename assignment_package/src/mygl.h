#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include <scene/mesh.h>
#include "camera.h"

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

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_glCamera;
public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    VertexDisplay m_vertDisplay;
    EdgeDisplay m_edgeDisplay;
    FaceDisplay m_faceDisplay;

protected:
    void keyPressEvent(QKeyEvent *e);
signals:
    void sig_sendFaceListNode(QListWidgetItem*);
    void sig_sendVertexListNode(QListWidgetItem*);
    void sig_sendEdgeListNode(QListWidgetItem*);
public slots:
    void slot_loadobj();
    void slot_catmullclark();
};


#endif // MYGL_H
