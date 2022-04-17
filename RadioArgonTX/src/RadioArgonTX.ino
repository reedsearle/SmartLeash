/*
 * Project RadioArgonTX
 * Description:
 * Author:
 * Date:
 */
SYSTEM_MODE(SEMI_AUTOMATIC);

//#include <VirtualWire.h>
#include <HX711.h>

const int RADIOTXPIN = D8;

int sendData;
int i;

// void setup()
// {
//     // Initialise the IO and ISR
//     vw_set_tx_pin(RADIOTXPIN);
//     vw_set_ptt_inverted(true); // Required for DR3100
//     vw_setup(500);       // Bits per sec
// }



// void loop()
// {
//   for(i=1; i<=100; i++) {
//     sendData = i;
//     vw_send((uint8_t *)&sendData, sizeof(sendData));
//     vw_wait_tx(); // Wait until the whole message is gone
//     delay(500);
//   }
// }


/*
  Example for different sending methods

  https://github.com/suda/RCSwitch

*/

#include "RCSwitch.h"

RCSwitch mySwitch = RCSwitch();

void setup() {
  // Transmitter is connected to Spark Core Pin D0
  mySwitch.enableTransmit(RADIOTXPIN);

  // Optional set pulse length.
  // mySwitch.setPulseLength(320);

  // Optional set protocol (default is 1, will work for most outlets)
  // mySwitch.setProtocol(2);

  // Optional set number of transmission repetitions.
  // mySwitch.setRepeatTransmit(15);
}

void loop() {

  /* See Example: TypeA_WithDIPSwitches */
  mySwitch.switchOn("11111", "00010");
  delay(1000);
  mySwitch.switchOn("11111", "00010");
  delay(1000);

  /* Same switch as above, but using decimal code */
  mySwitch.send(5393, 24);
  delay(1000);
  mySwitch.send(5396, 24);
  delay(1000);

  /* Same switch as above, but using binary code */
  mySwitch.send("000000000001010100010001");
  delay(1000);
  mySwitch.send("000000000001010100010100");
  delay(1000);

  /* Same switch as above, but tri-state code */
  mySwitch.sendTriState("00000FFF0F0F");
  delay(1000);
  mySwitch.sendTriState("00000FFF0FF0");
  delay(1000);

  delay(20000);
}

