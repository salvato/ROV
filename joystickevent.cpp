#include "joystickevent.h"

JoystickEvent::JoystickEvent() {
}

// Returns true if this event is the result of a button press.
bool
JoystickEvent::isButton() {
  return (type & JS_EVENT_BUTTON) != 0;
}


// Returns true if this event is the result of an axis movement.
bool
JoystickEvent::isAxis() {
  return (type & JS_EVENT_AXIS) != 0;
}


// Returns true if this event is part of the initial state obtained when the joystick is first connected to.
bool
JoystickEvent::isInitialState() {
  return (type & JS_EVENT_INIT) != 0;
}
