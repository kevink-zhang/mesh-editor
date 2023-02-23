#include "mygl.h"
#include <la.h>
#include "tiny_obj_loader.h"

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QPixmap>
#include <QFileDialog>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QImageWriter>
#include <QDebug>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_geomSquare(this),
      m_mesh(this),
      m_progLambert(this), m_progFlat(this),
      m_glCamera()
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_geomSquare.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
    //m_geomSquare.create();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");


    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_glCamera = Camera(w, h);
    glm::mat4 viewproj = m_glCamera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_progFlat.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setCamPos(m_glCamera.eye);
    m_progFlat.setModelMatrix(glm::mat4(1.f));

    //Create a model matrix. This one rotates the square by PI/4 radians then translates it by <-2,0,0>.
    //Note that we have to transpose the model matrix before passing it to the shader
    //This is because OpenGL expects column-major matrices, but you've
    //implemented row-major matrices.
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2,0,0)) * glm::rotate(glm::mat4(), 0.25f * 3.14159f, glm::vec3(0,1,0));
    //Send the geometry's transformation matrix to the shader
    m_progLambert.setModelMatrix(model);
    //Draw the example sphere using our lambert shader
//    m_progLambert.draw(m_geomSquare);

//    //Now do the same to render the cylinder
//    //We've rotated it -45 degrees on the Z axis, then translated it to the point <2,2,0>
//    model = glm::translate(glm::mat4(1.0f), glm::vec3(2,2,0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0,0,1));
//    m_progLambert.setModelMatrix(model);
//    m_progLambert.draw(m_geomSquare);

    //draw the mesh
    //m_progLambert.draw(m_mesh);
}


void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_glCamera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_glCamera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_glCamera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_glCamera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        m_glCamera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        m_glCamera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        m_glCamera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        m_glCamera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        m_glCamera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        m_glCamera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        m_glCamera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        m_glCamera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        m_glCamera = Camera(this->width(), this->height());
    }
    m_glCamera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}

void MyGL::slot_loadobj() {
    QString filename = QFileDialog::getOpenFileName(0, QString("Load Scene File"), QDir::currentPath().append(QString("../..")), QString("*.obj"));
    int i = filename.length() - 1;
    while(QString::compare(filename.at(i), QChar('/')) != 0)
    {
        i--;
    }
    QString local_path = filename.left(i+1);

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)){
        qWarning("Could not open the OBJ file.");
        return;
    }

    std::vector<tinyobj::shape_t> shapes; std::vector<tinyobj::material_t> materials;
    std::string errors = tinyobj::LoadObj(shapes, materials, filename.toStdString().c_str());
    std::cout << errors << std::endl;
    if(errors.size() == 0)
    {
        //make sure the obj data stored in mesh is cleared
        m_mesh.clear();

        //Read the information from the vector of shape_ts
        for(unsigned int i = 0; i < shapes.size(); i++)
        {
            std::vector<float> &positions = shapes[i].mesh.positions;
            std::vector<float> &normals = shapes[i].mesh.normals;
            std::vector<float> &uvs = shapes[i].mesh.texcoords;
            for(unsigned int j = 0; j < positions.size()/3; j++)
            {
                m_mesh.v.push_back(glm::vec4(positions[j*3], positions[j*3+1], positions[j*3+2],1));
            }
            for(unsigned int j = 0; j < normals.size()/3; j++)
            {
                m_mesh.vn.push_back(glm::vec4(normals[j*3], normals[j*3+1], normals[j*3+2],0));
            }
            for(unsigned int j = 0; j < uvs.size()/2; j++)
            {
                m_mesh.vt.push_back(glm::vec2(uvs[j*2], uvs[j*2+1]));
            }
        }
        qDebug() << m_mesh.v.size() << m_mesh.vn.size() << m_mesh.vt.size();
        //create the mesh
        m_mesh.create();
        m_mesh.clear();
    }
    else
    {
        //An error loading the OBJ occurred!
        std::cout << errors << std::endl;
    }

    //Read the mesh data in the file
//    for(int i = 0; i < objects.size(); i++)
//    {
//        std::vector<glm::vec4> vert_pos;
//        std::vector<glm::vec3> vert_col;
//        QJsonObject obj = objects[i].toObject();
//        QString type = obj["type"].toString();
//        qDebug() << type;
//        //Custom Polygon case
//        if(QString::compare(type, QString("custom")) == 0)
//        {
//            QString name = obj["name"].toString();
//            QJsonArray pos = obj["vertexPos"].toArray();
//            for(int j = 0; j < pos.size(); j++)
//            {
//                QJsonArray arr = pos[j].toArray();
//                glm::vec4 p(arr[0].toDouble(), arr[1].toDouble(), arr[2].toDouble(), 1);
//                vert_pos.push_back(p);
//            }
//            QJsonArray col = obj["vertexCol"].toArray();
//            for(int j = 0; j < col.size(); j++)
//            {
//                QJsonArray arr = col[j].toArray();
//                glm::vec3 c(arr[0].toDouble(), arr[1].toDouble(), arr[2].toDouble());
//                vert_col.push_back(c);
//            }
//            Polygon p(name, vert_pos, vert_col);
//            polygons.push_back(p);
//        }
//        //Regular Polygon case
//        else if(QString::compare(type, QString("regular")) == 0)
//        {
//            QString name = obj["name"].toString();
//            int sides = obj["sides"].toInt();
//            QJsonArray colorA = obj["color"].toArray();
//            glm::vec3 color(colorA[0].toDouble(), colorA[1].toDouble(), colorA[2].toDouble());
//            QJsonArray posA = obj["pos"].toArray();
//            glm::vec4 pos(posA[0].toDouble(), posA[1].toDouble(), posA[2].toDouble(),1);
//            float rot = obj["rot"].toDouble();
//            QJsonArray scaleA = obj["scale"].toArray();
//            glm::vec4 scale(scaleA[0].toDouble(), scaleA[1].toDouble(), scaleA[2].toDouble(),1);
//            Polygon p(name, sides, color, pos, rot, scale);
//            polygons.push_back(p);
//        }
//        //OBJ file case
//        else if(QString::compare(type, QString("obj")) == 0)
//        {
//            QString name = obj["name"].toString();
//            QString filename = local_path;
//            filename.append(obj["filename"].toString());
//            Polygon p = LoadOBJ(filename, name);
//            QString texPath = local_path;
//            texPath.append(obj["texture"].toString());
//            p.SetTexture(new QImage(texPath));
//            if(obj.contains(QString("normalMap")))
//            {
//                p.SetNormalMap(new QImage(local_path.append(obj["normalMap"].toString())));
//            }
//            polygons.push_back(p);
//        }
//    }
}

//obj loading
//Mesh MainWindow::LoadOBJ(const QString &file, const QString &polyName)
//{
//    Polygon p(polyName);
//    QString filepath = file;

//    return p;
    //return Mesh(mp_context);
//}

