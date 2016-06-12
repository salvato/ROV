#ifndef JOYSTICKEVENT_H
#define JOYSTICKEVENT_H


#define JS_EVENT_BUTTON 0x01 // button pressed/released
#define JS_EVENT_AXIS   0x02 // joystick moved
#define JS_EVENT_INIT   0x80 // initial state of device


// Encapsulates all data relevant to a sampled joystick event.

class JoystickEvent
{
public:
  JoystickEvent();

  static const short MIN_AXES_VALUE = -32768;/** Minimum value of axes range */
  static const short MAX_AXES_VALUE =  32767;/** Minimum value of axes range */

  unsigned int time;// The timestamp of the event, in milliseconds.

  // The value associated with this joystick event.
  // For buttons this will be either 1 (down) or 0 (up).
  // For axes, this will range between MIN_AXES_VALUE and MAX_AXES_VALUE.
  short value;
  unsigned char type;  //The event type.
  unsigned char number;// The axis/button number.

  bool isButton();
  bool isAxis();
  bool isInitialState() ;
};

#endif // JOYSTICKEVENT_H
