#include "joint.h"
#include <glm/gtc/type_ptr.hpp>

int Joint::index = 0;
int Joint::selected = -1;

Joint::Joint(OpenGLContext* mp_context, QString n, glm::vec4 a, glm::vec3 p): Drawable(mp_context), id(index++), name(n), pos(p), parent(nullptr){
    setText(0, name);
    float t = a[0];
    angle = glm::quat(cos(t/2), sin(t/2)*a[1], sin(t/2)*a[2], sin(t/2)*a[3]);
}

glm::mat4 Joint::getLocalTransformation() {
    glm::mat4 tt = glm::mat4(1);
    tt = glm::translate(tt, pos);
    return tt*glm::toMat4(angle);
}

glm::mat4 Joint::getOverallTransformation() {
    if(!parent) return getLocalTransformation();
    return parent->getOverallTransformation()*getLocalTransformation();
}

void Joint::create() {
    glm::mat4 r = getOverallTransformation();
    glm::vec4 p = r*glm::vec4(0,0,0,1);
    r[3] = glm::vec4(0,0,0,1);
    glm::vec4 x = r*glm::vec4(0.5, 0, 0, 0);
    glm::vec4 y = r*glm::vec4(0, 0.5, 0, 0);
    glm::vec4 z = r*glm::vec4(0, 0, 0.5, 0);

    std::vector<glm::vec4> vbopos  = {};
    std::vector<glm::vec4> vbocol  = {};
    std::vector<int> vboidx = {};

    //joint
    float da = glm::radians(3.f);
    glm::vec4 p1 = x;
    glm::vec4 p2 = x;
    glm::mat4 rm = glm::rotate(glm::mat4(1), da, glm::vec3(y));
    for(int i = 0; i < 120; i++) {
        p1 = p2;
        p2 = rm*p2;
        vbopos.push_back(p1+p);
        vbopos.push_back(p2+p);
        if(selected == id){
            vbocol.push_back(glm::vec4(1, 0.5, 0.5, 1));
            vbocol.push_back(glm::vec4(1, 0.5, 0.5, 1));
        }
        else{
            vbocol.push_back(glm::vec4(1, 0, 0, 1));
            vbocol.push_back(glm::vec4(1, 0, 0, 1));
        }
        vboidx.push_back(2*i);
        vboidx.push_back(2*i+1);
    }

    p1 = z;
    p2 = z;
    rm = glm::rotate(glm::mat4(1), da, glm::vec3(x));
    for(int i = 120; i < 240; i++) {
        p1 = p2;
        p2 = rm*p2;
        vbopos.push_back(p1+p);
        vbopos.push_back(p2+p);
        if(selected == id){
            vbocol.push_back(glm::vec4(0.5, 1, 0.5, 1));
            vbocol.push_back(glm::vec4(0.5, 1, 0.5, 1));
        }
        else{
            vbocol.push_back(glm::vec4(0, 1, 0, 1));
            vbocol.push_back(glm::vec4(0, 1, 0, 1));
        }
        vboidx.push_back(2*i);
        vboidx.push_back(2*i+1);
    }

    p1 = y;
    p2 = y;
    rm = glm::rotate(glm::mat4(1), da, glm::vec3(z));
    for(int i = 240; i < 360; i++) {
        p1 = p2;
        p2 = rm*p2;
        vbopos.push_back(p1+p);
        vbopos.push_back(p2+p);
        if(selected == id){
            vbocol.push_back(glm::vec4(0.5, 0.5, 1, 1));
            vbocol.push_back(glm::vec4(0.5, 0.5, 1, 1));
        }
        else{
            vbocol.push_back(glm::vec4(0, 0, 1, 1));
            vbocol.push_back(glm::vec4(0, 0, 1, 1));
        }
        vboidx.push_back(2*i);
        vboidx.push_back(2*i+1);
    }

    //bone
    if(parent) {
        vbopos.push_back(p);
        vbopos.push_back(parent->getOverallTransformation()*glm::vec4(0,0,0,1));
        vbocol.push_back(glm::vec4(1, 1, 0, 1));
        vbocol.push_back(glm::vec4(1, 0, 1, 1));
        vboidx.push_back(vboidx.size());
        vboidx.push_back(vboidx.size());
    }

    count = vboidx.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, vboidx.size() * sizeof(GLuint), vboidx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, vbopos.size() * sizeof(glm::vec4), vbopos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, vbocol.size() * sizeof(glm::vec4), vbocol.data(), GL_STATIC_DRAW);

}

GLenum Joint::drawMode() {
    return GL_LINES;
}

void Joint::pushUpdate() {
    create();
    for(Joint* j: children) {
        j->pushUpdate();
    }
}


