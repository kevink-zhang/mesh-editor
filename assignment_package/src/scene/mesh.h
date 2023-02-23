#pragma once

#include "drawable.h"
#include <la.h>
#include "smartpointerhelp.h"

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QListWidgetItem>

struct HalfEdge;

struct Face : public QListWidgetItem{
public:
    static int index;
    HalfEdge* edge;
    glm::vec3 color;
    int id;
    Face() : color(glm::vec3((rand() % 100)/100.0, (rand() % 100)/100.0, (rand() % 100)/100.0)), id(index++){
        setText(QString("Face %1").arg(id));
    }
};

struct Vertex : public QListWidgetItem{
public:
    static int index;
    HalfEdge* edge;
    glm::vec3 pos;
    int id;

    Vertex(glm::vec3 p) : pos(p), id(index++){
        setText(QString("Vertex %1").arg(id));
    }
};

struct HalfEdge : public QListWidgetItem{
public:
    static int index;
    HalfEdge* next;
    HalfEdge* mirror;
    Face* face;
    Vertex* node;
    int id;
    HalfEdge(Face* f, Vertex* v): face(f), node(v), id(index++) {
        setText(QString("Edge %1").arg(id));
    }

};

class Mesh : public Drawable
{
public:
    std::vector<uPtr<Face>> faces;
    std::vector<uPtr<Vertex>> vertices;
    std::vector<uPtr<HalfEdge>> halfedges;

    Mesh(OpenGLContext* mp_context);
    virtual void create();
    virtual GLenum drawMode();
    void clear();

    //storing obj file info
    std::vector<glm::vec4> v;
    std::vector<glm::vec2> vt;
    std::vector<glm::vec4> vn;
    std::vector<std::vector<glm::vec3>> f;
};
