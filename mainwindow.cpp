// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>

#include <QDebug>
#include <QDial>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>

#ifdef Q_OS_LINUX
  #include <VLCQtCore/Common.h>
  #include <VLCQtCore/Instance.h>
  #include <VLCQtCore/Media.h>
  #include <VLCQtCore/MediaPlayer.h>
  #include <VLCQtWidgets/WidgetVideo.h>
#endif

#include "mainwindow.h"
#include "joystickevent.h"
#include "joystick.h"

#include "glwidget.h"
#include "shimmer3box.h"

#include <unistd.h>       // for usleep()


MainWindow::MainWindow(QWidget *parent)
  : QWidget(parent)
  , pSpeed(NULL)
  , pDirection(NULL)
  , pPitch(NULL)
  , pEditHostName(NULL)
  , pButtonConnect(NULL)
  , pAngleRow(NULL)
  , pButtonRow(NULL)
  , pMainLayout(NULL)
  , pJoystickEvent(NULL)
  , pJoystick(NULL)
#ifdef Q_OS_LINUX
  , pVlcInstance(NULL)
  , pVlcMedia(NULL)
  , pVlcPlayer(NULL)
  , pVlcWidgetVideo(NULL)
#endif
  , widgetSize(QSize(400, 300))
  , stillAliveTime(300)// in ms
  , watchDogTime(1000)
{
  // Create an instance of Joystick
  pJoystick = new Joystick("/dev/input/js0");

#ifdef Q_OS_LINUX
  // The following is mandatory for using VLC-Qt and all its other classes.
  pVlcInstance = new VlcInstance(VlcCommon::args(), this);

  //pVlcInstance->setLogLevel(Vlc::DebugLevel);
  //pVlcInstance->setLogLevel(Vlc::ErrorLevel);
  pVlcInstance->setLogLevel(Vlc::DisabledLevel);

  // A basic MediaPlayer manager for VLC-Qt library.
  // It provides main playback controls.
  // This is mandatory to use libvlc playback functions.
  pVlcPlayer = new VlcMediaPlayer(pVlcInstance);

  pVlcWidgetVideo = new VlcWidgetVideo(pVlcPlayer, this);
  pVlcWidgetVideo->setFixedSize(widgetSize);
  //pVlcWidgetVideo->setAspectRatio(Vlc::R_4_3);

  pVlcPlayer->setVideoWidget(pVlcWidgetVideo);
#endif

  initCamera();
  initWidgets();
  initLayout();// Init Window Layout

  // Widgets events
  connect(pFrontWidget, SIGNAL(windowUpdated()), this, SLOT(updateWidgets()));
  connect(pEditHostName, SIGNAL(returnPressed()), this, SLOT(connectToClient()));
  connect(pButtonConnect, SIGNAL(clicked()), this, SLOT(connectToClient()));

  // Network events
  connect(&tcpClient, SIGNAL(connected()), this, SLOT(serverConnected()));
  connect(&tcpClient, SIGNAL(disconnected()), this, SLOT(serverDisconnected()));
  connect(&tcpClient, SIGNAL(readyRead()), this, SLOT(newDataAvailable()));
  connect(&tcpClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

  // Watchdog Timer event s
  connect(&stillAliveTimer, SIGNAL(timeout()), this, SLOT(onStillAliveTimerTimeout()));
  connect(&watchDogTimer,   SIGNAL(timeout()), this, SLOT(onWatchDogTimerTimeout()));
  stillAliveTimer.start(stillAliveTime);
}


MainWindow::~MainWindow() {
  stillAliveTimer.stop();
  watchDogTimer.stop();
  pJoystick->bStopSampling = true;
  joystickThread.quit();
  joystickThread.wait(3000);
#ifdef Q_OS_LINUX
  delete pVlcWidgetVideo;
  delete pVlcPlayer;
  delete pVlcMedia;
  delete pVlcInstance;
#endif
}


void
MainWindow::onStillAliveTimerTimeout() {
  if(tcpClient.isOpen()) {
    message.clear();
    message.append(char(126));
    message.append(char(126));
    tcpClient.write(message);
  }
}


void
MainWindow::onWatchDogTimerTimeout() {
  if(tcpClient.isOpen()) {
    tcpClient.close();
    console.appendPlainText("Timeout in getting data from ROV");
  }
}


void
MainWindow::updateWidgets() {
  pFrontWidget->updateGL();
}


void
MainWindow::initCamera() {
  //     Set(eyePosX, eyePosY, eyePosZ, centerX, centerY, centerZ, upX, upY, upZ)
  camera.Set(0.0,     0.0,     2.0,     0.0,     0.0,     0.0,     0.0, 1.0, 0.0);
  camera.FieldOfView(45.0);
  camera.MouseMode(CGrCamera::PITCHYAW);
  camera.Gravity(false);
}


void
MainWindow::initWidgets() {
  boxes.clear();
  boxes.append(new Shimmer3Box());
  pFrontWidget = new GLWidget(&camera, this);
  camera.Set(-2.0,     0.0,     0.0,     0.0,     0.0,     0.0,     0.0, 0.0, 1.0);
  pFrontWidget->lightPos = QVector4D(-2800, -2800, 2800, 1.0);

  pFrontWidget->setShimmerBoxes(&boxes);
  pFrontWidget->setFixedSize(widgetSize);

//  updateWidgets();
}


void
MainWindow::initLayout() {
  pMainLayout  = new QHBoxLayout;
  pLeftLayout  = new QVBoxLayout;
  pAngleRow    = new QVBoxLayout;
  pButtonRow   = new QHBoxLayout;

  pSpeed       = new QSlider();
  pSpeed->setRange(-10, 10);
  pSpeed->setInvertedAppearance(true);

  pDirection   = new QDial();
  pDirection->setRange(-15, 15);

  pPitch       = new QSlider();

  //pPitch->setRange(JoystickEvent::MIN_AXES_VALUE, JoystickEvent::MAX_AXES_VALUE);
  pPitch->setRange(-10, 10);

  pEditHostName         = new QLineEdit("192.168.1.123", this);
  pButtonConnect        = new QPushButton("Connect", this);
  pCheckInflate         = new QCheckBox("Inflate");
  pCheckDeflate         = new QCheckBox("Deflate");

  pAngleRow->addWidget(pSpeed,     0, Qt::AlignCenter);
  pAngleRow->addWidget(pDirection, 0, Qt::AlignCenter);
  pAngleRow->addSpacing(10);
  pAngleRow->addWidget(pPitch,     0, Qt::AlignCenter);
  pAngleRow->addSpacing(10);
  pAngleRow->addWidget(pCheckInflate, 0, Qt::AlignCenter);
  pAngleRow->addWidget(pCheckDeflate, 0, Qt::AlignCenter);

  pButtonRow->addWidget(pEditHostName);
  pButtonRow->addWidget(pButtonConnect);

  pLeftLayout->addLayout(pAngleRow);
  pLeftLayout->addLayout(pButtonRow);
  console.setReadOnly(true);
  console.document()->setMaximumBlockCount(100);
  QPalette p = palette();
  p.setColor(QPalette::Base, Qt::darkBlue);
  p.setColor(QPalette::Text, Qt::white);
  console.setPalette(p);
  pLeftLayout->addWidget(&console);

  pButtonRowLayout = new QHBoxLayout;
  pButtonSetOrientation   = new QPushButton("Set Pos");
  pButtonResetOrientation = new QPushButton("Reset Pos");
  pButtonSwitchOff        = new QPushButton("Switch Off");
  pButtonRowLayout->addWidget(pButtonSetOrientation);
  pButtonRowLayout->addWidget(pButtonResetOrientation);
  pButtonRowLayout->addWidget(pButtonSwitchOff);
  pButtonSetOrientation->setEnabled(false);
  pButtonResetOrientation->setEnabled(false);
  pButtonSwitchOff->setEnabled(false);

  pGLBoxLayout = new QVBoxLayout();
  pGLBoxLayout->addWidget(pFrontWidget);
#ifdef Q_OS_LINUX
  pGLBoxLayout->addWidget(pVlcWidgetVideo);
#endif
  pGLBoxLayout->addLayout(pButtonRowLayout);

  pMainLayout->addLayout(pLeftLayout);
  pMainLayout->addLayout(pGLBoxLayout);
  setLayout(pMainLayout);

}


void
MainWindow::connectToClient() {
  if(pButtonConnect->text() == tr("Connect")) {
    pButtonConnect->setEnabled(false);
    pEditHostName->setEnabled(false);
    QHostInfo::lookupHost(pEditHostName->text(), this, SLOT(handleLookup(QHostInfo)));
  } else {//pButtonConnect->text() == tr("Disconnect")
    tcpClient.close();
  }
}


void
MainWindow::handleLookup(QHostInfo hostInfo) {
  // Handle the results.
  if(hostInfo.error() == QHostInfo::NoError) {
    serverAddress = hostInfo.addresses().first();
    console.appendPlainText("Connecting to: " + hostInfo.hostName());
    bytesWritten = 0;
    bytesReceived = 0;
    tcpClient.connectToHost(serverAddress, 43210);
#ifdef Q_OS_LINUX
      if(pVlcMedia) {
        delete pVlcMedia;
        pVlcMedia = NULL;
      }
      QString url = QString("http://") + hostInfo.hostName() + QString(":8080/?action=stream");
      pVlcMedia = new VlcMedia(url, pVlcInstance);
      pVlcPlayer->open(pVlcMedia);
#endif
  } else {
    console.appendPlainText(hostInfo.errorString());
    pButtonConnect->setEnabled(true);
    pEditHostName->setEnabled(true);
  }
}


void
MainWindow::displayError(QAbstractSocket::SocketError socketError) {
  if(socketError == QTcpSocket::RemoteHostClosedError) {
    console.appendPlainText("The remote host has closed the connection");
    tcpClient.close();
    return;
  }
  console.appendPlainText(tcpClient.errorString());
  tcpClient.close();
  pButtonConnect->setEnabled(true);
  pEditHostName->setEnabled(true);
}


void
MainWindow::serverConnected() {
  console.appendPlainText("Connected");
  pButtonConnect->setText("Disconnect");
  pButtonConnect->setEnabled(true);
  watchDogTimer.start(watchDogTime);
}


void
MainWindow::serverDisconnected() {
  console.appendPlainText("Disconnected");
  pButtonConnect->setText("Connect");
  pEditHostName->setEnabled(true);
  watchDogTimer.stop();
}


void
MainWindow::newDataAvailable() {
  receivedCommand += tcpClient.readAll();
  QString sNewCommand;
  int iPos;
  iPos = receivedCommand.indexOf("#");
  while(iPos != -1) {
    sNewCommand = receivedCommand.left(iPos);
    //console.appendPlainText(sNewCommand + " Received");
    executeCommand(sNewCommand);
    receivedCommand = receivedCommand.mid(iPos+1);
    iPos = receivedCommand.indexOf("#");
  }
}


void
MainWindow::executeCommand(QString command) {
  if(command.contains(QString("box_pos"))) {
    QStringList tokens = command.split(' ');
    tokens.removeFirst();
    if(tokens.count() == 8) {
      int iSensorNumber = tokens.at(0).toInt();
      if(iSensorNumber >= boxes.count()) {
        for(int i=boxes.count(); i<=iSensorNumber; i++) {
          boxes.append(new Shimmer3Box());
        }
      }
      if(iSensorNumber < boxes.count()) {
        Shimmer3Box* pBox = boxes[iSensorNumber];
        pBox->x      = tokens.at(1).toDouble();
        pBox->y      = tokens.at(2).toDouble();
        pBox->z      = tokens.at(3).toDouble();
        pBox->pos[0] = tokens.at(4).toDouble();
        pBox->pos[1] = tokens.at(5).toDouble();
        pBox->pos[2] = tokens.at(6).toDouble();
        pBox->angle  = tokens.at(7).toDouble();
        updateWidgets();
      }
    }
  } else if(command.contains(QString("alive"))) {
        watchDogTimer.start(watchDogTime);
  }
}


int
MainWindow::start() {
  // Ensure that it was found and that we can use it
  if (!pJoystick->isFound()) {
    console.appendPlainText("Joystick open failed.");
    return joystickNotFoundError;
  }
  pJoystick->moveToThread(&joystickThread);

  connect(&joystickThread, SIGNAL(finished()), pJoystick, SLOT(deleteLater()));
  connect(this, SIGNAL(operate()), pJoystick, SLOT(startSampling()));
  connect(pJoystick, SIGNAL(newValue(JoystickEvent*)), this, SLOT(onJoystickMessage(JoystickEvent*)));
  joystickThread.start();

  emit operate();

  return 0;
}


void
MainWindow::onJoystickMessage(JoystickEvent* pEvent) {
  message.clear();
  if (pEvent->isButton()) {
    if(pEvent->number == InflateButton) {//Inflate Button
      pCheckInflate->setChecked(pEvent->value ? true : false);
      message.append(char(pEvent->number+100));
      message.append(char(pEvent->value));
      if(tcpClient.isOpen()) tcpClient.write(message);
    }
    else if(pEvent->number == DeflateButton) {//Deflate Button
      pCheckDeflate->setChecked(pEvent->value ? true : false);
      message.append(char(pEvent->number+100));
      message.append(char(pEvent->value));
      if(tcpClient.isOpen()) tcpClient.write(message);
    }
  }
  else if (pEvent->isAxis()) {
    if(pEvent->number == UpDownAxis) {//Pitch Accelerometer
      pPitch->setValue(pEvent->value*10/JoystickEvent::MAX_AXES_VALUE);
      message.append(char(pEvent->number));
      message.append(char(pEvent->value*10/JoystickEvent::MAX_AXES_VALUE));
      if(tcpClient.isOpen()) tcpClient.write(message);
    }
    else if(pEvent->number == SpeedAxis) {//Right stick Up/Down (Motor Speed)
      pSpeed->setValue(pEvent->value*10/JoystickEvent::MAX_AXES_VALUE);
      message.append(char(pEvent->number));
      message.append(char(pEvent->value*10/JoystickEvent::MAX_AXES_VALUE));
      if(tcpClient.isOpen()) tcpClient.write(message);
    }
    else if(pEvent->number == LeftRightAxis) {//Right stick Left/Right (Motor Speed)
      pDirection->setValue(pEvent->value*10/JoystickEvent::MAX_AXES_VALUE);
      message.append(char(pEvent->number));
      message.append(char(pEvent->value*10/JoystickEvent::MAX_AXES_VALUE));
      if(tcpClient.isOpen()) tcpClient.write(message);
    }
  }
}

