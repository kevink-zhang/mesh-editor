#include "mesh.h"
#include<la.h>
#include <iostream>

//2d to 1d mapping indices
long dictEdge(HalfEdge* prev, long maxID) {
    long b1 = prev->node->id;
    long b2 = prev->next->node->id;
    return std::min(b1, b2)*maxID + std::max(b1, b2);
}

int Face::index = 0;
Face::Face() : color(glm::vec3((rand() % 100)/100.0, (rand() % 100)/100.0, (rand() % 100)/100.0)), sharp(false), id(index++){
    setText(QString("Face %1").arg(id+1));
}
int Vertex::index = 0;
Vertex::Vertex(glm::vec3 p) : pos(p), sharp(false), id(index++){
    setText(QString("Vertex %1").arg(id+1));
}
int HalfEdge::index = 0;
HalfEdge::HalfEdge(Face* f, Vertex* v): face(f), node(v), sharp(false), id(index++) {
    setText(QString("Edge %1").arg(id+1));
}

void Face::makeSharp(bool value) {
    sharp = value;
    HalfEdge* edgeAt = edge;
    do{
        edgeAt->node->sharp = value;
        edgeAt->sharp = value;
        edgeAt->mirror->sharp = value;
        edgeAt = edgeAt->next;
    } while(edgeAt != edge);
}

Mesh::Mesh(OpenGLContext *mp_context) : Drawable(mp_context), sharpness(1.f)
{

}

Face* Mesh::getFace(int i)  {
    return faces[(i+faces.size())%faces.size()].get();
}
Vertex* Mesh::getVert(int i)  {
    return vertices[(i+vertices.size())%vertices.size()].get();
}
HalfEdge* Mesh::getEdge(int i)  {
    return halfedges[(i+halfedges.size())%halfedges.size()].get();
}

Face* Mesh::makeFace(){
    faces.push_back(mkU<Face>());
    return getFace(-1);
}
Vertex* Mesh::makeVert(glm::vec3 pos){
    vertices.push_back(mkU<Vertex>(pos));
    return getVert(-1);
}
HalfEdge* Mesh::makeEdge(Face* f, Vertex* v){
    halfedges.push_back(mkU<HalfEdge>(f, v));
    return getEdge(-1);
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

//catmull-clark
void Mesh::CatmullClark() {
    if(vertices.empty()) return;
    //calculate centroids
    std::map<Face*, glm::vec3> centroids;
    std::map<Vertex*, glm::vec3> new_pos;

    for(int i = 0; i < faces.size(); i++) {
        Face* this_face = getFace(i);
        HalfEdge* start = this_face -> edge;
        HalfEdge* edgeAt = start;
        glm::vec3 center = glm::vec3(0);
        int num = 0;
        do {
            //adds to centroid average
            center += edgeAt -> node -> pos;
            num++;
            edgeAt = edgeAt -> next;
        } while(edgeAt != start);

        centroids[this_face] = center/(float) num;

        do {
            //adds centroid to new pos of vertices on face
            if(!new_pos.count(edgeAt->node)) new_pos[edgeAt->node] = glm::vec3(0);
            new_pos[edgeAt->node] += centroids[this_face];
            edgeAt = edgeAt -> next;
        } while(edgeAt != start);
    }
    //calculate smoothed midpoints
    std::map<long, glm::vec3> midpts;
    std::map<long, glm::vec3> midptave;
    std::map<long, bool> midptsharps;

    long maxID = getVert(-1)->id+1;
    std::map<Vertex*, int> n;
    std::map<Vertex*, std::vector<HalfEdge*>> n_sharp;

    for(int i = 0; i < halfedges.size(); i++) {
        HalfEdge* prev_edge = getEdge(i);
        HalfEdge* this_edge = prev_edge -> next;
        if(!midpts.count(dictEdge(prev_edge, maxID))){
            midpts[dictEdge(prev_edge, maxID)] = prev_edge->node->pos +
                                       this_edge->node->pos+
                                       centroids[this_edge->face];
            if(this_edge->mirror) {
                midpts[dictEdge(prev_edge, maxID)] += centroids[this_edge->mirror->face];
                midpts[dictEdge(prev_edge, maxID)] /= 4.f;
            }
            else{
                midpts[dictEdge(prev_edge, maxID)] /= 3.f;
            }
            //sharpness things
            midptave[dictEdge(prev_edge, maxID)] = (this_edge->node->pos+prev_edge->node->pos)/2.f;
            midptsharps[dictEdge(prev_edge, maxID)] = this_edge->sharp;
        }
        //count
        if(!n.count(this_edge->node)) n[this_edge->node] = 0;
        n[this_edge->node]++;
        if(!n_sharp.count(this_edge->node))
            n_sharp[this_edge->node] = std::vector<HalfEdge*>();
        if(this_edge->sharp && n_sharp[this_edge->node].size() <= 3)
            n_sharp[this_edge->node].push_back(this_edge);
        new_pos[this_edge -> node] += midpts[dictEdge(prev_edge, maxID)];
    }
    //smooth the og vertices
    for(int i = 0; i < vertices.size(); i++) {
        new_pos[getVert(i)]/=(float)(n[getVert(i)]*n[getVert(i)]);
        new_pos[getVert(i)]+=(n[getVert(i)]-2.f)/(float)n[getVert(i)]*getVert(i)->pos;
        //vertex is sharp, or has 3+ adj sharp edges
        if(getVert(i)->sharp || n_sharp[getVert(i)].size() >= 3){
            //use og position
            getVert(i)->pos = getVert(i)->pos * sharpness + new_pos[getVert(i)] * (1-sharpness);
        }
        //vertex is not sharp, but has 2 adj sharp edges
        else if (n_sharp[getVert(i)].size() == 2){
            glm::vec3 endpt1 = n_sharp[getVert(i)][0]->mirror->node->pos;
            glm::vec3 endpt2 = n_sharp[getVert(i)][1]->mirror->node->pos;
            getVert(i)->pos = (0.75f*(getVert(i)->pos) + 0.125f*(endpt1 + endpt2)) * sharpness + new_pos[getVert(i)] * (1-sharpness);
        }
        //vertex is not sharp
        else {
            getVert(i)->pos = new_pos[getVert(i)];
        }
    }
    //subdivision new vectors
    std::vector<uPtr<Face>> new_faces;
    std::vector<uPtr<HalfEdge>> new_halfedges;

    //reset all face and halfedge indexes back to 0, as we will be completely overriding them
    Face::index = 0;
    HalfEdge::index = 0;

    //insert in new vertices, use maps to keep track of vertex pointers
    std::map<Face*, Vertex*> centroid_map;
    std::map<long, Vertex*> midpt_map;

    for(std::map<Face*,glm::vec3>::iterator it = centroids.begin(); it != centroids.end(); ++it) {
        centroid_map[it->first] = makeVert(it->second);
    }
    //sharpness for edges implemented here for midpts
    for(std::map<long, glm::vec3>::iterator it = midpts.begin(); it != midpts.end(); ++it) {
        if(midptsharps[it->first])
            midpt_map[it->first] = makeVert((1-sharpness)* (it->second) + sharpness * midptave[it->first]);
        else
             midpt_map[it->first] = makeVert(it->second);
    }

    //go through each old face and quadrangalize on it
    for(int i = 0; i < faces.size(); i++) {
        HalfEdge* start = getFace(i)->edge;
        HalfEdge* prev = getFace(i)->edge;
        do{
            //initialize a new face
            new_faces.push_back(mkU<Face>());
            Face* new_face = new_faces[new_faces.size()-1].get();
            //find the quadrangle vertices
            Vertex* new_verts[4] = {centroid_map[getFace(i)], midpt_map[dictEdge(prev, maxID)], prev->next->node, midpt_map[dictEdge(prev->next, maxID)]};
            HalfEdge* new_edge[4];
            bool new_sharps[4] = {
                false, false, prev->sharp, prev->next->sharp
            };
            //add in new edges, link to vertex and face
            for(int i = 0; i < 4; i++) {
                new_halfedges.push_back(mkU<HalfEdge>(new_face, new_verts[i]));
                new_edge[i] = new_halfedges[new_halfedges.size()-1].get();
                new_edge[i]->sharp = new_sharps[i];
                new_verts[i]->edge = new_edge[i];
                new_face->edge = new_edge[i];
            }
            //link the edges up
            for(int i = 0; i < 4; i++) {
                new_edge[i]->next = new_edge[(i+1)%4];
            }
            //iterate
            prev = prev -> next;
        } while(prev != start);
    }
    //overrid our old mesh with the new mesh
    faces.clear();
    for(int i = 0; i < new_faces.size(); i++) {
        faces.push_back(std::move(new_faces[i]));
    }
    halfedges.clear();
    for(int i = 0; i < new_halfedges.size(); i++) {
        halfedges.push_back(std::move(new_halfedges[i]));
    }
    //find symmetrical halfedges
    std::map<long, HalfEdge*> symFinder;
    maxID= getVert(-1)->id + 1;
    for(int i = 0; i < halfedges.size(); i++) {
        //grab the endpoint vertex ids for the half-edge
        int v1 = getEdge(i)->node->id;
        int v2 = getEdge(i)->next->node->id;
        //checks if sym edge already exists, and links the two
        if(symFinder.count(v2*maxID + v1)) {
            getEdge(i)->next->mirror = symFinder[v2*maxID + v1];
            symFinder[v2*maxID + v1] -> mirror = getEdge(i)->next;
            symFinder.erase(v2*maxID + v1);
        }
        else {
            symFinder[v1*maxID + v2] = getEdge(i)->next;
        }
    }
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
