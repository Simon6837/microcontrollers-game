#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <Nunchuk.h>
#include <util/delay.h>

// Define constants for the nunchuk
#define NUNCHUK_ADDRESS 0x52
#define BAUDRATE 9600
#define CHUNKSIZE 32
#define BUFFERLEN 256

// Declare all functions
void updateNunchuckValues(void);

bool setup()
{
  // enable global interrupts
  sei();
  // use Serial for printing nunchuk data
  Serial.begin(BAUDRATE);
  // join I2C bus as master
  Wire.begin();
  // connect with the nunchuk
  Serial.print("Connecting to nunchuk at address 0x");
  Serial.println(NUNCHUK_ADDRESS, HEX);
  if (!Nunchuk.begin(NUNCHUK_ADDRESS))
  {
    Serial.println("No nunchuk found");
    Serial.flush();
    return false;
  }

  return true;
}

int main(void)
{
  setup();
  // super loop
  while (1)
  {
    updateNunchuckValues();
  }
  return (0);
}
/**
 * @brief Updates the nunchuck values to the current state of the nunchuck
 */
void updateNunchuckValues(void)
{
  if (!Nunchuk.getState(NUNCHUK_ADDRESS))
  {
    Serial.println("******** No nunchuk found");
    Serial.flush();
    // todo: try to reconnect
  }
  // Serial.print("Joy X: ");
  // Serial.print(Nunchuk.state.joy_x_axis, DEC);
  // Serial.print("\tJoy Y: ");
  // Serial.print(Nunchuk.state.joy_y_axis, DEC);
  // Serial.print("\t\tButton C: ");
  // Serial.print(Nunchuk.state.c_button, DEC);
  // Serial.print("\t\t\tButton Z: ");
  // Serial.println(Nunchuk.state.z_button, DEC);
}
