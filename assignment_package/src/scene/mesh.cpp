#include "mesh.h"
#include<la.h>
#include <iostream>

int Face::index = 0;
Face::Face() : color(glm::vec3((rand() % 100)/100.0, (rand() % 100)/100.0, (rand() % 100)/100.0)), id(index++){
    setText(QString("Face %1").arg(id));
}
int Vertex::index = 0;
Vertex::Vertex(glm::vec3 p) : pos(p), id(index++){
    setText(QString("Vertex %1").arg(id));
}
int HalfEdge::index = 0;
HalfEdge::HalfEdge(Face* f, Vertex* v): face(f), node(v), id(index++) {
    setText(QString("Edge %1").arg(id));
}

Mesh::Mesh(OpenGLContext *mp_context) : Drawable(mp_context)
{

}

Face* Mesh::getFace(int i) {
    return faces[(i+faces.size())%faces.size()].get();
}
Vertex* Mesh::getVert(int i) {
    return vertices[(i+vertices.size())%vertices.size()].get();
}
HalfEdge* Mesh::getEdge(int i) {
    return halfedges[(i+halfedges.size())%halfedges.size()].get();
}

void Mesh::create() {
    std::vector<glm::vec4> pos;

    std::vector<glm::vec4> nor;

    std::vector<glm::vec4> col;

    std::vector<GLuint> idx;


    //index for indices
    int min_idx = 0;

    for(int fi = 0; fi < faces.size(); fi++) {
        Face* face = getFace(fi);
        HalfEdge* begin = face->edge;
        HalfEdge* edgeAt = face->edge;
        do {
            glm::vec3 p1 = edgeAt -> node -> pos;
            glm::vec3 p2 = edgeAt -> next -> node -> pos;
            glm::vec3 p3 = edgeAt -> next -> next -> node -> pos;
            pos.push_back(glm::vec4(p2, 1));
            nor.push_back(glm::vec4(glm::cross((p2-p1), (p3-p2)), 1));
            col.push_back(glm::vec4(face->color, 1));
            edgeAt = edgeAt -> next;
        } while(edgeAt != begin);

        for(unsigned long i = min_idx+1; i < pos.size()-1; i++) {
            idx.push_back(min_idx);
            idx.push_back(i);
            idx.push_back(i+1);
        }
        min_idx = pos.size();
    }

    count = idx.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateNor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec4), nor.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}

//clears the data
void Mesh::clear() {
    faces.clear();
    vertices.clear();
    halfedges.clear();

    Face::index = 0;
    Vertex::index = 0;
    HalfEdge::index = 0;
}

GLenum Mesh::drawMode() {
    return GL_TRIANGLES;
}
