#include "mesh.h"
#include<la.h>
#include <iostream>

int Face::index = 0;
int Vertex::index = 0;
int HalfEdge::index = 0;

Mesh::Mesh(OpenGLContext *mp_context) : Drawable(mp_context)
{}

void Mesh::create() {
    std::vector<glm::vec4> pos;

    std::vector<glm::vec4> nor;

    std::vector<glm::vec4> uvs;

    std::vector<GLuint> idx;

    count = 0; // TODO: Set "count" to the number of indices in your index VBO

//    for( auto&& f : faces ) {

//    }

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateNor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec4), nor.data(), GL_STATIC_DRAW);

    generateUV();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufUV);
    mp_context->glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);
}

//clears the data
void Mesh::clear() {
    v.clear();
    vt.clear();
    vn.clear();
}

GLenum Mesh::drawMode() {
    return GL_TRIANGLES;
}
