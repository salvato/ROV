/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector3D>
#include <QVector2D>
#include <QGLShaderProgram>

#include "GrCamera.h"
#include "geometryengine.h"


class Text;
class Shimmer3Box;
class BodyModel;


class GLWidget : public QGLWidget, protected QGLFunctions
{
  Q_OBJECT

public:
  GLWidget(CGrCamera* myCamera, QWidget *parent = 0);
  ~GLWidget();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

  void setShimmerBoxes(QVector<Shimmer3Box*>* shimmer3Boxes);
  enum side {
    front,
    rear,
    top,
    right,
    left,
    bottom
  } fromSide;
  void setSide(side from);
  QVector<Shimmer3Box*>* shimmerSensors;

signals:
  void xRotationChanged(int angle);
  void yRotationChanged(int angle);
  void zRotationChanged(int angle);
  void windowUpdated();

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent* event);

  void initShaders();
  void initTextures();

//  void drawFrame();

private:
  QString sourceDir;
  QString shaderMessage;
  QPoint lastPos;
  QString sLabel;
  CGrCamera* camera;

  QMatrix4x4 projectionMatrix;
  QMatrix4x4 modelMatrix;
  QMatrix4x4 viewMatrix;
  QMatrix4x4 mvpMatrix;
  QList<QMatrix4x4> matrixStack;

  GLuint texture;
  QGLShaderProgram program;
  GeometryEngine geometries;

  GLuint shimmerVertexBuffer;
  GLuint shimmerColorBuffer;
  GLuint programID;
  GLuint textProgramID;
  GLuint MatrixID;
  GLuint Text2DTextureID;
  GLuint Text2DVertexBufferID;
  GLuint Text2DUVBufferID;
  GLuint Text2DShaderID;
  GLuint Text2DUniformID;
};
#endif
