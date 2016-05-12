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

#include "joystick.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sstream>
#include "unistd.h"


Joystick::Joystick()
  : QObject()
{
  openPath("/dev/input/js0");
}


Joystick::Joystick(int joystickNumber)
  : QObject()
{
  std::stringstream sstm;
#ifdef WIN32
  sstm << "/dev/input/js" << joystickNumber;
#else
  sstm << "/dev/input/js" << joystickNumber;
#endif
  openPath(sstm.str());
}


Joystick::Joystick(std::string devicePath)
  : QObject()
{
  openPath(devicePath);
}


Joystick::~Joystick() {
  close(_fd);
}


void
Joystick::openPath(std::string devicePath) {
#ifdef WIN32
  _fd = open(devicePath.c_str(), O_RDONLY);
#else
  _fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);
#endif
}


bool
Joystick::sample(JoystickEvent* event) {
  int bytes = read(_fd, event, sizeof(*event)); 

  if (bytes == -1)
    return false;

  // NOTE if this condition is not met, we're probably out of sync and this
  // Joystick instance is likely unusable
  return bytes == sizeof(*event);
}


bool
Joystick::isFound() {
  return _fd >= 0;
}


void Joystick::startSampling() {
  bStopSampling = false;
  while (!bStopSampling) {
    // Attempt to sample an event from the joystick
    JoystickEvent event;
    if (sample(&event)) {
      emit newValue(&event);
    }
    usleep(1000);
  }
}
