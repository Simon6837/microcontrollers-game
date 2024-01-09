#ifndef NUNCHUK_CONTROLLER_H
#define NUNCHUK_CONTROLLER_H

#include <Nunchuk.h>

class NunchukController
{
public:
  NunchukController();
  void initialize();
  uint8_t updatePlayerControl();
  uint8_t updateMenu();
  bool isZButtonPressed();

private:
  NunChuk nunchuk; // Declare the Nunchuk object
};

#endif
