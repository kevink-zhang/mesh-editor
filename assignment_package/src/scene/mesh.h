#pragma once

#include "drawable.h"
#include <la.h>
#include "smartpointerhelp.h"

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QListWidgetItem>

class HalfEdge;

class Face : public QListWidgetItem{
public:
    static int index;
    HalfEdge* edge;
    glm::vec3 color;
    int id;
    Face();
};

class Vertex : public QListWidgetItem{
public:
    static int index;
    HalfEdge* edge;
    glm::vec3 pos;
    int id;

    Vertex(glm::vec3 p);
};

class HalfEdge : public QListWidgetItem{
public:
    static int index;
    HalfEdge* next;
    HalfEdge* mirror;
    Face* face;
    Vertex* node;
    int id;
    HalfEdge(Face* f, Vertex* v);

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

    Face* getFace(int i);
    Vertex* getVert(int i);
    HalfEdge* getEdge(int i);

    //storing obj file info
};
