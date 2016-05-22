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

#include <QtWidgets>
#include <QtOpenGL>

#include <fstream>
#include <math.h>

#include "glwidget.h"
//#include "text.h"
#include "shimmer3box.h"


GLWidget::GLWidget(CGrCamera* myCamera, QWidget *parent)
  : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
  , fromSide(GLWidget::front)
  , shimmerSensors(NULL)
  , sLabel(tr("Front"))
  , camera(myCamera)
{
  lightPos = QVector4D(0, 4000, 4000, 1.0);
}


GLWidget::~GLWidget() {
}


QSize
GLWidget::minimumSizeHint() const {
  return QSize(50, 50);
}


QSize
GLWidget::sizeHint() const {
  return QSize(800, 800);
}


void
GLWidget::setShimmerBoxes(QVector<Shimmer3Box*>* shimmer3Boxes) {
  shimmerSensors = shimmer3Boxes;
}


void
GLWidget::initializeGL() {
  initializeGLFunctions();
  initShaders();
  initTextures();

  glClearColor(0.1, 0.1, 0.5, 0.0);

  glEnable(GL_DEPTH_TEST);// Enable depth test
  glDepthFunc(GL_LESS);// Accept fragment if it closer to the camera than the former one
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);// Cull triangles whose normal is not towards the camera
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_MULTISAMPLE);

  geometries.init();

}


void
GLWidget::initShaders() {
  // Compile vertex shader
  if(!program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshader.glsl"))
    close();
  // Compile fragment shader
  if(!program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshader.glsl"))
    close();
  // Link shader pipeline
  if(!program.link())
    close();
  // Bind shader pipeline for use
  if(!program.bind())
    close();
}


void
GLWidget::initTextures() {
  // Load the image
   glEnable(GL_TEXTURE_2D);
  texture = new QOpenGLTexture(QImage(":/ROV_1.png").mirrored());
  // Set nearest filtering mode for texture minification
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  texture->setMinificationFilter(QOpenGLTexture::Nearest);
  // Set bilinear filtering mode for texture magnification
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  texture->setMagnificationFilter(QOpenGLTexture::Linear);
  // Wrap texture coordinates by repeating
  // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  texture->setWrapMode(QOpenGLTexture::Repeat);
}


void
GLWidget::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
  // Projection matrix :
  projectionMatrix.setToIdentity();
  projectionMatrix.perspective(camera->FieldOfView(), float(width)/float(height), 0.1f, 100.0f);
}


void
GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(shimmerSensors->isEmpty()) return;

  texture->bind();

  // Use our shader
  glUseProgram(program.programId());

  // Camera matrix
  viewMatrix.setToIdentity();
  viewMatrix.lookAt(
    QVector3D(camera->EyeX(),    camera->EyeY(),    camera->EyeZ()),    // Camera position in World Space
    QVector3D(camera->CenterX(), camera->CenterY(), camera->CenterZ()), // Looking at the origin
    QVector3D(camera->UpX(),     camera->UpY(),     camera->UpZ())      // Head is up (set to 0,-1,0 to look upside-down)
  );

//  // Use texture unit 0 which contains ROV.png
//  program.setUniformValue("qt_Texture0", 0);

  program.setUniformValue("LightPosition_worldspace", lightPos);

  modelMatrix.setToIdentity();

  if(fromSide == GLWidget::top) {
  } else if(fromSide == GLWidget::bottom) {
  } else if(fromSide == GLWidget::left) {
  } else if(fromSide == GLWidget::right) {
  } else if(fromSide == GLWidget::rear) {
  } else if(fromSide == GLWidget::front) {
  }

  Shimmer3Box *pSensor, *pSensor0;

  if(shimmerSensors->count() > 1) {
    pSensor0 = (*shimmerSensors)[0];
    // Sensori dipendenti dal primo
    for(int i=0; i<shimmerSensors->count(); i++) {
      pSensor = (*shimmerSensors)[i];
      // save the unrotated coordinate system.
      matrixStack.prepend(modelMatrix);
      // Translate sensor in his position
      modelMatrix.translate(pSensor->pos[0], pSensor->pos[1], pSensor->pos[2]);
      if(i>0) {
        //Rotate around sensor center
        modelMatrix.rotate(-pSensor0->angle, pSensor0->x, pSensor0->y, pSensor0->z);
        modelMatrix.rotate( pSensor->angle, pSensor->x, pSensor->y, pSensor->z);
      }
      // Draw the sensor with the right dimensions
      float scale = 1.0/(geometries.max-geometries.min);
      modelMatrix.scale(scale, scale, scale);

      // Set modelview-projection matrix
      mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
      program.setUniformValue("mvp_Matrix",   mvpMatrix);
      program.setUniformValue("view_Matrix",  viewMatrix);
      program.setUniformValue("model_Matrix", modelMatrix);
      modelMatrix = modelMatrix.inverted();
      modelMatrix = modelMatrix.transposed();
      program.setUniformValue("model_MatrixIT", modelMatrix);

      geometries.drawROVGeometry(&program);
      // restore the unrotated coordinate system.
      modelMatrix = matrixStack.takeFirst();
    }
  }
  else if(shimmerSensors->count() == 1) {
    pSensor = (*shimmerSensors)[0];
    // save the unrotated coordinate system.
    matrixStack.prepend(modelMatrix);
    // Translate sensor in his position
    modelMatrix.translate(pSensor->pos[0], pSensor->pos[1], pSensor->pos[2]);
    modelMatrix.rotate(pSensor->angle, pSensor->x, pSensor->y, pSensor->z);
    // Draw the sensor with the right dimensions
    float scale = 1.0/(geometries.max-geometries.min);
    modelMatrix.scale(scale, scale, scale);

    mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

    // Set modelview-projection matrix
    program.setUniformValue("mvp_Matrix",   mvpMatrix);
    program.setUniformValue("view_Matrix",  viewMatrix);
    program.setUniformValue("model_Matrix", modelMatrix);
    modelMatrix = modelMatrix.inverted();
    modelMatrix = modelMatrix.transposed();
    program.setUniformValue("model_MatrixIT", modelMatrix);

    // Draw the geometry
    geometries.drawROVGeometry(&program);

    // restore the unrotated coordinate system.
    modelMatrix = matrixStack.takeFirst();

  }// if(shimmerSensors->count() == 1)
}


void
GLWidget::setSide(side from) {
  fromSide = from;
  if(fromSide == top) {
    sLabel = "Top";
  } else if(fromSide == bottom) {
    sLabel = "Bottom";
  } else if(fromSide == left) {
    sLabel = "Left";
  } else if(fromSide == right) {
    sLabel = "Right";
  } else if(fromSide == rear) {
    sLabel = "Rear";
  } else {
    sLabel = "Front";
  }
  updateGL();
}


void
GLWidget::mousePressEvent(QMouseEvent *event) {
  Q_UNUSED(event)
  if (event->buttons() & Qt::RightButton) {
    lastPos = event->pos();
    camera->MouseDown(event->x(), event->y());
    camera->MouseMode(CGrCamera::ROLLMOVE);
    event->accept();
  } else if (event->buttons() & Qt::LeftButton) {
    lastPos = event->pos();
    camera->MouseDown(event->x(), event->y());
    event->accept();
  }
}


void
GLWidget::mouseReleaseEvent(QMouseEvent *event) {
  Q_UNUSED(event)
  if (event->button() & Qt::RightButton) {
    camera->MouseMode(CGrCamera::PITCHYAW);
    event->accept();
  } else if (event->button() & Qt::LeftButton) {
    camera->MouseMode(CGrCamera::PITCHYAW);
    event->accept();
  }
}


void
GLWidget::mouseMoveEvent(QMouseEvent *event) {
  Q_UNUSED(event)
  if (event->buttons() & Qt::LeftButton) {
    camera->MouseMove(event->x(), event->y());
    event->accept();
    emit windowUpdated();
  } else if (event->buttons() & Qt::RightButton) {
    camera->MouseMove(event->x(), event->y());
    event->accept();
    emit windowUpdated();
  }
}


void
GLWidget::wheelEvent(QWheelEvent* event) {
  QPoint numDegrees = event->angleDelta() / 120;
  if (!numDegrees.isNull()) {
    camera->MouseDown(0, 0);
    camera->MouseMode(CGrCamera::ROLLMOVE);
    camera->MouseMove(0, -numDegrees.y());
    camera->MouseMode(CGrCamera::PITCHYAW);
    event->accept();
    emit windowUpdated();
  }
}
