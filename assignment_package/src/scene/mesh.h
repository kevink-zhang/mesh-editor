#pragma once

#include "drawable.h"
#include <la.h>
#include "smartpointerhelp.h"

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

struct HalfEdge;

struct Face {
    static int index;
    HalfEdge* edge;
    glm::vec3 color;
    int id;
    Face(HalfEdge* e, glm::vec3 c) {
        edge = e;
        color = c;
        id = index++;
    }
};

struct Vertex {
    static int index;
    HalfEdge* edge;
    glm::vec3 pos;
    int id;
    Vertex(HalfEdge* e, glm::vec3 p) {
        edge = e;
        pos = p;
        id = index++;
    }
};

struct HalfEdge {
    static int index;
    HalfEdge* next;
    HalfEdge* mirror;
    Face* face;
    Vertex* node;
    int id;
    HalfEdge(HalfEdge* n, HalfEdge* m, Face* f, Vertex* v) {
        next = n;
        mirror = m;
        face = f;
        node = v;
        id = index++;
    }
};

class Mesh : public Drawable
{
    std::vector<uPtr<Face>> faces;
    std::vector<uPtr<Vertex>> vertices;
    std::vector<uPtr<HalfEdge>> halfedges;
public:
    Mesh(OpenGLContext* mp_context);
    virtual void create();
    virtual GLenum drawMode();
    void clear();

    //storing obj file info
    std::vector<glm::vec4> v;
    std::vector<glm::vec2> vt;
    std::vector<glm::vec4> vn;
};
