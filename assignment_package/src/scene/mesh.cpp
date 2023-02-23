#include "mesh.h"
#include<la.h>
#include <iostream>

int Face::index = 0;
int Vertex::index = 0;
int HalfEdge::index = 0;

Mesh::Mesh(OpenGLContext *mp_context) : Drawable(mp_context)
{

}

void Mesh::create() {
    std::vector<glm::vec4> pos;

    std::vector<glm::vec4> nor;

    std::vector<glm::vec2> uvs;

    std::vector<glm::vec4> col;

    std::vector<GLuint> idx;

    //initalize mesh vertices
    for(glm::vec4 vertex: v) {
        Vertex new_vertex = Vertex(glm::vec3(vertex));
        vertices.push_back(mkU<Vertex>(new_vertex));
    }

    //index for indices
    int min_idx = 0;

    //map for symmetric half-edge locating
    std::map<long, HalfEdge*> symFinder;
    long symMax = v.size()+1;

    for(std::vector<glm::vec3> face : f) {
        Face new_face;
        faces.push_back(mkU<Face>(new_face));
        std::vector<HalfEdge> new_edges;

        for(glm::vec3 vertex : face) {
            //VBO
            int vi = vertex[0]-1;
            int vti = vertex[1]-1;
            int vni = vertex[2]-1;
            pos.push_back(v[vi]);
            nor.push_back(vn[vni]);
            uvs.push_back(vt[vti]);
            col.push_back(glm::vec4(new_face.color, 1));

            //create a new half-edge, set it as the edge pointer of the vertex and face, and add to halfedges
            new_edges.push_back(HalfEdge(&new_face, vertices[vi].get()));
            vertices[vi].get() -> edge = &new_edges[new_edges.size()-1];
            new_face.edge = &new_edges[new_edges.size()-1];
            halfedges.push_back(mkU<HalfEdge>(new_edges[new_edges.size()-1]));
        }

        //cycle through the new half-edges, linking neighbors and finding symmetrical half-edges
        int num_edges = new_edges.size();
        for(int i = 0; i < num_edges; i++) {
            new_edges[i].next = &new_edges[(i+1)%num_edges];
            //grab the endpoint vertex ids for the half-edge
            int v1 = new_edges[(i-1+num_edges)%num_edges].node -> id;
            int v2 = new_edges[i].node -> id;
            //checks if sym edge already exists, and links the two
            if(symFinder.count(v2*symMax + v1)) {
                new_edges[i].mirror = symFinder[v2*symMax + v1];
                symFinder[v2*symMax + v1] -> mirror = &new_edges[i];
                symFinder.erase(v2*symMax + v1);
            }
            else {
                symFinder[v1*symMax + v2] = &new_edges[i];
            }
        }

        for(int i = min_idx+1; i < pos.size()-1; i++) {
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

    generateUV();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufUV);
    mp_context->glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}

//clears the data
void Mesh::clear() {
    v.clear();
    vt.clear();
    vn.clear();
    f.clear();

    faces.clear();
    vertices.clear();
    halfedges.clear();
}

GLenum Mesh::drawMode() {
    return GL_TRIANGLES;
}
