/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "geometryengine.h"

#include <QVector2D>
#include <QVector3D>
#include <QFile>


GeometryEngine::GeometryEngine() {
}


GeometryEngine::~GeometryEngine() {
}


bool
GeometryEngine::loadROVobj(QString path,
                           QVector<QVector3D> &out_vertices,
                           QVector<QVector2D> &out_uvs,
                           QVector<QVector3D> &out_normals)
{
  QFile file(path);
  if(!file.open(QIODevice::ReadOnly)) {
    printf("Impossible to open the file !\n");
    return false;
  }
  QVector<unsigned int> vertexIndices, uvIndices, normalIndices;
  QVector<QVector3D> temp_vertices;
  QVector<QVector2D> temp_uvs;
  QVector<QVector3D> temp_normals;
  float x, y, z;

  QByteArray line;
  QString string;
  QStringList stringVals, stringTriples;

  while(!file.atEnd()) {
    line = file.readLine();
    // parse line


    if(line.startsWith("vt")) {
      string = QString(line.mid(3));
      stringVals = string.split(" ");
      if(stringVals.size() != 2) {
        qDebug() << "File can't be read by our simple parser : (Try exporting with other options\n";
        return false;
      }
      x = stringVals.at(0).toFloat();
      y = stringVals.at(1).toFloat();
      temp_uvs.append(QVector2D(x, y));
    }

    else if(line.startsWith("vn")) {
      string = QString(line.mid(3));
      stringVals = string.split(" ");
      if(stringVals.size() != 3) {
        qDebug() << "File can't be read by our simple parser : (Try exporting with other options\n";
        return false;
      }
      x = stringVals.at(0).toFloat();
      y = stringVals.at(1).toFloat();
      z = stringVals.at(2).toFloat();
      temp_normals.append(QVector3D(x, y, z));
    }

    else if(line.startsWith("v")) {
      string = QString(line.mid(2));
      stringVals = string.split(" ");
      if(stringVals.size() != 3) {
        qDebug() << "File can't be read by our simple parser : (Try exporting with other options\n";
        return false;
      }
      x = stringVals.at(0).toFloat();
      y = stringVals.at(1).toFloat();
      z = stringVals.at(2).toFloat();
      temp_vertices.append(QVector3D(x, y, z));
    }

    else if(line.startsWith("f")) {
      string = QString(line.mid(2));
      stringTriples = string.split(" ");
      if(stringTriples.size() != 3) {
        qDebug() << "File can't be read by our simple parser : (Try exporting with other options\n";
        return false;
      }
      for(int i=0; i<3; i++) {
        stringVals = stringTriples.at(i).split("/");
        if(stringVals.size() != 3) {
          qDebug() << "File can't be read by our simple parser : (Try exporting with other options\n";
          return false;
        }
        vertexIndices.append(stringVals.at(0).toFloat());
        uvIndices    .append(stringVals.at(1).toFloat());
        normalIndices.append(stringVals.at(2).toFloat());
      }
    }

    // else Probably a comment, eat up the rest of the line
  }
  file.close();

  // For each vertex of each triangle
  for(int i=0; i<vertexIndices.size(); i++) {

    // Get the indices of its attributes
    unsigned int vertexIndex = vertexIndices[i];
    unsigned int uvIndex     = uvIndices[i];
    unsigned int normalIndex = normalIndices[i];

    // Get the attributes thanks to the index
    QVector3D vertex = temp_vertices[ vertexIndex-1 ];
    QVector2D uv     = temp_uvs[ uvIndex-1 ];
    QVector3D normal = temp_normals[ normalIndex-1 ];

    // Put the attributes in buffers
    out_vertices.append(vertex);
    out_uvs     .append(uv);
    out_normals .append(normal);
  }

  return true;
}


void
GeometryEngine::init() {
  if(!loadROVobj(":/ROV_4.obj", vertices, uvs, normals)) {
    qDebug() << "Impossible to decode obj file";
    exit(-1);
  }
  initializeGLFunctions();
  // Generate 3 VBOs
  vertexbuffer.create();
  uvbuffer.create();
  normalbuffer.create();
  // Initializes cube geometry and transfers it to VBOs
  initROVGeometry();
}


void
GeometryEngine::initROVGeometry() {
  // Transfer vertex data to VBO 0
  vertexbuffer.bind();
  vertexbuffer.allocate((void *)vertices.data(), vertices.size() * sizeof(QVector3D));

  // Transfer uv data to VBO 1
  uvbuffer.bind();
  uvbuffer.allocate((void *)uvs.data(), uvs.size() * sizeof(QVector2D));

  // Transfer normal data to VBO 2
  normalbuffer.bind();
  normalbuffer.allocate((void *)normals.data(), normals.size() * sizeof(QVector3D));
}


void
GeometryEngine::drawROVGeometry(QGLShaderProgram *program) {
  // Tell OpenGL programmable pipeline how to locate vertex position data
  vertexbuffer.bind();
  int vertexLocation = program->attributeLocation("a_position");
  program->enableAttributeArray(vertexLocation);
  program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

  // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
  uvbuffer.bind();
  int texcoordLocation = program->attributeLocation("a_texcoord");
  program->enableAttributeArray(texcoordLocation);
  program->setAttributeBuffer(texcoordLocation, GL_FLOAT, 0, 2, sizeof(QVector2D));

  // Tell OpenGL programmable pipeline how to locate normals data
  normalbuffer.bind();
  int normcoordLocation = program->attributeLocation("vertexNormal_modelspace");
  program->enableAttributeArray(normcoordLocation);
  program->setAttributeBuffer(normcoordLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

  // Draw ROV geometry
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

