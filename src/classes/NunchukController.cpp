#include "NunchukController.h"
// #include <Wire.h>


#define NUNCHUK_ADDRESS 0x52
#define BAUDRATE 9600

NunchukController::NunchukController() {}

void NunchukController::initialize()
{
  // Wire.begin();
  // Serial.begin(BAUDRATE);
  // Serial.print("Connecting to nunchuk at address 0x");
  // Serial.println(NUNCHUK_ADDRESS, HEX);
  // if (!nunchuk.begin(NUNCHUK_ADDRESS))
  // {
  //   Serial.println("No nunchuk found");
  //   Serial.flush();
  // }
}

/**
 * @brief Updates the player control based on Nunchuk input
 * @return 0 if the joystick is moved left, 1 if the joystick is moved right, 2 if the joystick is not moved
 */
uint8_t NunchukController::updatePlayerControl()
{
  nunchuk.getState(NUNCHUK_ADDRESS); // Update Nunchuk state
  if (nunchuk.state.joy_x_axis < 115)
  {
    // Code to move player left
    return 0;
  }
  else if (nunchuk.state.joy_x_axis > 140)
  {
    // Code to move player right
    return 1;
  }
  return 2;
}

bool NunchukController::isZButtonPressed()
{
  nunchuk.getState(NUNCHUK_ADDRESS); // Update Nunchuk state
  return nunchuk.state.z_button;
}