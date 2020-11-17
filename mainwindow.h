#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QThread>
#include <QTcpSocket>
#include <QHostAddress>
#include <QHostInfo>
#include <QPlainTextEdit>
#include <QByteArray>
#include <QTimer>

#include "GrCamera.h"

QT_FORWARD_DECLARE_CLASS(QDial)
QT_FORWARD_DECLARE_CLASS(QSlider)
QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QHBoxLayout)
QT_FORWARD_DECLARE_CLASS(QVBoxLayout)
QT_FORWARD_DECLARE_CLASS(JoystickEvent)
QT_FORWARD_DECLARE_CLASS(Joystick)
QT_FORWARD_DECLARE_CLASS(QCheckBox)
QT_FORWARD_DECLARE_CLASS(Shimmer3Box)
QT_FORWARD_DECLARE_CLASS(GLWidget)


class MainWindow : public QWidget
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();

  int start();

private:
  void initCamera();
  void initWidgets();
  void initLayout();
  void executeCommand(QString command);

public:
  static const int noError = -1;
  static const int joystickNotFoundError = -1;

  static const int upDownAxis     =   0;
  static const int pitchAxis      =   1;
  static const int LeftRightAxis  =   2;
  static const int SpeedAxis      =   3;
  static const int RollAxis       =   4;

  static const int DeflateButton  =   9;
  static const int InflateButton  =  11;

  static const int depthSensor    =  81;

  static const int SetOrientation = 125;
  static const int StillAlive     = 126;

public slots:
  void onConnectToClient();
  void onResetOrientation();
  void handleLookup(QHostInfo hostInfo);
  void displayError(QAbstractSocket::SocketError socketError);
  void onServerConnected();
  void onServerDisconnected();
  void onNewDataAvailable();
  void updateWidgets();
  void onStillAliveTimerTimeout();
  void onWatchDogTimerTimeout();
  void onGetDepthTimerTimeout();

signals:
  void operate();

private:
  QTcpSocket tcpClient;
  QHostAddress serverAddress;
  QDateTime     dateTime;

  int bytesWritten;
  int bytesReceived;

  QPlainTextEdit console;
  QLineEdit*   pEditHostName;
  QPushButton* pButtonConnect;

  QPushButton*  pButtonResetOrientation;
  QPushButton*  pButtonSwitchOff;

  QVBoxLayout* pAngleRow;
  QHBoxLayout* pButtonRow;
  QVBoxLayout* pLeftLayout;

  QHBoxLayout* pButtonRowLayout;
  QHBoxLayout* pBoxesLayout;
  QVBoxLayout* pGLBoxLayout;

  QHBoxLayout* pMainLayout;

  QByteArray message;
  QString receivedCommand;

  CGrCamera     camera;
  GLWidget*     pFrontWidget;
  QVector<Shimmer3Box*> boxes; // The graphical objects

  QSize           widgetSize;
  QTimer          stillAliveTimer;
  QTimer          watchDogTimer;
  QTimer          getDepthTimer;
  int             stillAliveTime;
  int             watchDogTime;
  int             getDepthTime;
};

#endif // MAINWINDOW_H
