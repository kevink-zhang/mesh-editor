#pragma once

#include "drawable.h"
#include <la.h>
#include "smartpointerhelp.h"

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QTreeWidgetItem>


class Joint : public QTreeWidgetItem, public Drawable{
public:
    static int index;
    static int selected;
    QString name;
    Joint* parent;
    std::vector<Joint*> children;
    glm::vec3 pos;
    glm::quat angle;
    glm::mat4 bind;

    int id;

    Joint(OpenGLContext* mp_context, QString n, glm::vec4 a, glm::vec3 p);
    glm::mat4 getLocalTransformation();
    glm::mat4 getOverallTransformation();
    void pushUpdate();
    virtual void create();
    virtual GLenum drawMode();
};

