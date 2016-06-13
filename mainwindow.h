#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTcpSocket>
#include <QHostAddress>
#include <QHostInfo>
#include <QPlainTextEdit>
#include <QByteArray>
#include <QTimer>

#include "GrCamera.h"

QT_FORWARD_DECLARE_CLASS(Joystick)
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

#ifdef Q_OS_LINUX
  QT_FORWARD_DECLARE_CLASS(VlcInstance)
  QT_FORWARD_DECLARE_CLASS(VlcMedia)
  QT_FORWARD_DECLARE_CLASS(VlcMediaPlayer)
  QT_FORWARD_DECLARE_CLASS(VlcWidgetVideo)
#endif

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

  static const int LeftRightAxis = 2;
  static const int SpeedAxis     = 3;
  static const int RollAxis      = 4;
  static const int UpDownAxis    = 1;

  static const int InflateButton = 11;
  static const int DeflateButton = 9;

public slots:
  void onJoystickMessage(JoystickEvent* pEvent);
  void connectToClient();
  void handleLookup(QHostInfo hostInfo);
  void displayError(QAbstractSocket::SocketError socketError);
  void serverConnected();
  void serverDisconnected();
  void newDataAvailable();
  void updateWidgets();
  void onStillAliveTimerTimeout();
  void onWatchDogTimerTimeout();

signals:
  void operate();

private:
  QTcpSocket tcpClient;
  QHostAddress serverAddress;

  int bytesWritten;
  int bytesReceived;

  QPlainTextEdit console;

  QSlider* pSpeed;
  QDial*   pDirection;
  QSlider* pPitch;

  QLineEdit*   pEditHostName;
  QPushButton* pButtonConnect;

  QPushButton*  pButtonSetOrientation;
  QPushButton*  pButtonResetOrientation;
  QPushButton*  pButtonSwitchOff;

  QCheckBox*   pCheckInflate;
  QCheckBox*   pCheckDeflate;

  QVBoxLayout* pAngleRow;
  QHBoxLayout* pButtonRow;
  QVBoxLayout* pLeftLayout;

  QHBoxLayout* pButtonRowLayout;
  QHBoxLayout* pBoxesLayout;
  QVBoxLayout* pGLBoxLayout;

  QHBoxLayout* pMainLayout;

  JoystickEvent* pJoystickEvent;
  Joystick* pJoystick;

  QThread joystickThread;

  QByteArray message;
  QString receivedCommand;

  CGrCamera     camera;
  GLWidget*     pFrontWidget;
  QVector<Shimmer3Box*> boxes; // The graphical objects

#ifdef Q_OS_LINUX
  VlcInstance*    pVlcInstance;
  VlcMedia*       pVlcMedia;
  VlcMediaPlayer* pVlcPlayer;
  VlcWidgetVideo* pVlcWidgetVideo;
#endif

  QSize           widgetSize;
  QTimer          stillAliveTimer;
  QTimer          watchDogTimer;
  int             stillAliveTime;
  int             watchDogTime;
};

#endif // MAINWINDOW_H
