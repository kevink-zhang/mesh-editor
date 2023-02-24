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

    //storing obj file info
    std::vector<glm::vec4> v;
    std::vector<glm::vec2> vt;
    std::vector<glm::vec4> vn;
    std::vector<std::vector<glm::vec3>> f;
};
