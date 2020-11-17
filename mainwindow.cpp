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

#include "mainwindow.h"

#include "glwidget.h"
#include "shimmer3box.h"

#include <unistd.h>       // for usleep()


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , pEditHostName(NULL)
    , pButtonConnect(NULL)
    , pAngleRow(NULL)
    , pButtonRow(NULL)
    , pMainLayout(NULL)

    , widgetSize(QSize(440, 330))
    , stillAliveTime(300)// in ms
    , watchDogTime(30000)
    , getDepthTime(500)
{
    initCamera();
    initWidgets();
    initLayout();// Init Window Layout

    // Widgets events
    connect(pFrontWidget, SIGNAL(windowUpdated()), this, SLOT(updateWidgets()));
    connect(pEditHostName, SIGNAL(returnPressed()), this, SLOT(onConnectToClient()));
    connect(pButtonConnect, SIGNAL(clicked()), this, SLOT(onConnectToClient()));
    connect(pButtonResetOrientation, SIGNAL(clicked(bool)), this, SLOT(onResetOrientation()));

    // Network events
    connect(&tcpClient, SIGNAL(connected()), this, SLOT(onServerConnected()));
    connect(&tcpClient, SIGNAL(disconnected()), this, SLOT(onServerDisconnected()));
    connect(&tcpClient, SIGNAL(readyRead()), this, SLOT(onNewDataAvailable()));
    connect(&tcpClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

    // Watchdog Timer events
    connect(&stillAliveTimer, SIGNAL(timeout()), this, SLOT(onStillAliveTimerTimeout()));
    connect(&watchDogTimer,   SIGNAL(timeout()), this, SLOT(onWatchDogTimerTimeout()));

    connect(&getDepthTimer,   SIGNAL(timeout()), this, SLOT(onGetDepthTimerTimeout()));

    stillAliveTimer.start(stillAliveTime);
}


MainWindow::~MainWindow() {
    stillAliveTimer.stop();
    watchDogTimer.stop();
}


void
MainWindow::onStillAliveTimerTimeout() {
    if(tcpClient.isOpen()) {
        message.clear();
        message.append(char(StillAlive));
        message.append(char(StillAlive));
        tcpClient.write(message);
    }
}


void
MainWindow::onResetOrientation() {
    if(tcpClient.isOpen()) {
        message.clear();
        message.append(char(SetOrientation));
        message.append(char(SetOrientation));
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
MainWindow::onGetDepthTimerTimeout() {
    if(tcpClient.isOpen()) {
        message.clear();
        message.append(char(depthSensor));
        message.append(char(depthSensor));
        tcpClient.write(message);
    }
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
}


void
MainWindow::initLayout() {
    pMainLayout  = new QHBoxLayout;
    pLeftLayout  = new QVBoxLayout;
    pAngleRow    = new QVBoxLayout;
    pButtonRow   = new QHBoxLayout;

    pEditHostName         = new QLineEdit("192.168.1.123", this);
    pButtonConnect        = new QPushButton("Connect", this);

    pButtonRow->addWidget(pEditHostName);
    pButtonRow->addWidget(pButtonConnect);

    pButtonRowLayout = new QHBoxLayout;
    pButtonResetOrientation = new QPushButton("Reset Pos");
    pButtonSwitchOff        = new QPushButton("Switch Off");
    pButtonRowLayout->addWidget(pButtonResetOrientation);
    pButtonRowLayout->addWidget(pButtonSwitchOff);
    pButtonResetOrientation->setEnabled(false);
    pButtonSwitchOff->setEnabled(false);

    pLeftLayout->addLayout(pAngleRow);
    pLeftLayout->addLayout(pButtonRow);
    pLeftLayout->addLayout(pButtonRowLayout);
    console.setReadOnly(true);
    console.document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::darkBlue);
    p.setColor(QPalette::Text, Qt::white);
    console.setPalette(p);
    pLeftLayout->addWidget(&console);

    pGLBoxLayout = new QVBoxLayout();
    pGLBoxLayout->addWidget(pFrontWidget);

    pMainLayout->addLayout(pLeftLayout);
    pMainLayout->addLayout(pGLBoxLayout);
    setLayout(pMainLayout);
}


void
MainWindow::onConnectToClient() {
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
MainWindow::onServerConnected() {
    console.appendPlainText("Connected");
    pButtonConnect->setText("Disconnect");
    pButtonConnect->setEnabled(true);
    pButtonResetOrientation->setEnabled(true);

    watchDogTimer.start(watchDogTime);
    getDepthTimer.start(getDepthTime);
}


void
MainWindow::onServerDisconnected() {
    console.appendPlainText("Disconnected");
    pButtonConnect->setText("Connect");
    pEditHostName->setEnabled(true);
    pButtonResetOrientation->setEnabled(false);
    watchDogTimer.stop();
    getDepthTimer.stop();
}


void
MainWindow::onNewDataAvailable() {
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

    } else if(command.contains(QString("depth"))) {
        QStringList tokens = command.split(' ');
        tokens.removeFirst();
    } else if(command.contains(QString("alive"))) {
        watchDogTimer.start(watchDogTime);
    }
}


int
MainWindow::start() {
    emit operate();
    return 0;
}
