

#include <BLEMidi.h>
#define NUM_BUTTONS  3

const uint8_t button1 = 27;
const uint8_t button2 = 25;
const uint8_t button3 = 32;
const uint8_t buttons[NUM_BUTTONS] = {button1, button2, button3};

const uint8_t cc1 = 16;
const uint8_t cc2 = 17;
const uint8_t cc3 = 18;
const uint8_t cc[NUM_BUTTONS] = {cc1, cc2, cc3};

uint8_t pressedButtons = 0x00;

uint8_t previousButtons = 0x00;

void setup() {
  Serial.begin(115200);
  Serial.println("Setup buttons");
  
  
  for (int i = 0; i < NUM_BUTTONS; i++)
    pinMode(buttons[i], INPUT);

  Serial.println("Initializing bluetooth");
  BLEMidiServer.begin("BLE Pedalboard");
  BLEMidiServer.setOnDisconnectCallback([](){     // To show how to make a callback with a lambda function
    Serial.println("Disconnected");
  });
  BLEMidiServer.setOnConnectCallback([](){     // To show how to make a callback with a lambda function
    Serial.println("Connected");
  });
  Serial.println("Waiting for connections...");

}

void loop() {
  // put your main code here, to run repeatedly:
  readButtons();
  sendCC();

}

void readButtons()
{
  for (int i = 0; i < NUM_BUTTONS; i++)  {
    if (digitalRead(buttons[i]) == LOW)
    {  
      bitWrite(pressedButtons, i, 1);
      delay(50);
    }
    else 
    {
      bitWrite(pressedButtons, i, 0);
    }
  }
}

void sendCC(){
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (bitRead(pressedButtons, i) != bitRead(previousButtons, i))
    {
      if (bitRead(pressedButtons, i))
      {
        bitWrite(previousButtons, i , 1);
        controlChange(0, cc[i], 127);
      }
      else
      {
        bitWrite(previousButtons, i , 0);
        controlChange(0, cc[i], 0);
      }
    }
  }
}

void controlChange(uint8_t channel, uint8_t control, uint8_t value) {

//  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
//
//  MidiUSB.sendMIDI(event);
  if(BLEMidiServer.isConnected()) {
    BLEMidiServer.controlChange(channel, control, value);
  }
  else
  {
    char buff[50];
    sprintf(buff, "should send %d to cc %d", value, control );
    Serial.println(buff);
  }
}
