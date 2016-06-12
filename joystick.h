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

#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include <QObject>
#include <string>
#include "joystickevent.h"

// Represents a joystick device. Allows data to be sampled from it.
class Joystick : public QObject
{
  Q_OBJECT
public:
  // Initialises an instance for the first joystick: /dev/input/js0
  Joystick();
  // Initialises an instance for the joystick with the specified zero-indexed number.
  Joystick(int joystickNumber);
  // Initialises an instance for the joystick device specified.
  Joystick(std::string devicePath);

  ~Joystick();

  // Returns true if the joystick was found and may be used, otherwise false.
  bool isFound();
  
  // Attempts to populate the provided JoystickEvent instance with data
  // from the joystick. Returns true if data is available, otherwise false.
  bool sample(JoystickEvent* event);

public:
  bool bStopSampling;


private:
  void openPath(std::string devicePath);

  int _fd;

signals:
  void newValue(JoystickEvent* event);

public slots:
  void startSampling();

};

#endif
