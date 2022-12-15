#include <ArduinoBLE.h>
#define SOLENOID_1   21
//uint8_t solPins[] = {2, 14, 2, 2, 21, 16, 2, 20, 2};
//uint8_t solPins[] = {2, 14, 15, 16, 18, 20, 19, 21, 18}; //19 not in use
uint8_t solPins[] = {2, 14, 15, 17, 16, 18, 19, 20, 21};
bool solPinStatus[] = {false, false, false, false, false, false, false, false, false};
//uint8_t solPins[] = {2, 14, 21, 16, 20};
//uint8_t solPins[] = {2, 14, 16};
//uint8_t solPins[] = {2, 2, 2, 2, 2, 2, 2, 2, 2};
//uint8_t solPins[] = {2, 14, 2, 16, 2, 2, 2, 2, 2};
//uint8_t solPins[] = {2, 14, 2, 2, 2, 16, 2, 20, 19};;
//uint8_t solPins[] = {2, 2};
uint8_t motorPins[] = {9, 10, 11, 12};
uint8_t leftVibPin = 11;
uint8_t rightVibPin = 10;
uint8_t upVibPin = 12;
uint8_t downVibPin = 9;
long previousMillis = 0;
int interval = 0;
int ledState = LOW;
const char* uuidOfRxChar = "00002A3D-0000-1000-8000-00805f9b34fb";
// Setup the incoming data characteristic (RX).
const int RX_BUFFER_SIZE = 256;
bool RX_BUFFER_FIXED_LENGTH = false;
//BLEService ledService("180A"); // BLE LED Service
const char* uuidOfService = "0000181a-0000-1000-8000-00805f9b34fb";
int pressDelay = 80;
BLEService ledService(uuidOfService);


// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("2A57", BLERead | BLEWrite);
BLECharacteristic rxChar(uuidOfRxChar, BLEWriteWithoutResponse | BLEWrite, RX_BUFFER_SIZE, RX_BUFFER_FIXED_LENGTH);



void setup() {
  for(uint8_t i = 0; i < sizeof(solPins); ++i){
    pinMode(solPins[i], OUTPUT);
  }
  for(uint8_t i = 0; i < sizeof(motorPins); ++i){
    pinMode(motorPins[i], OUTPUT);
  }
  Serial.begin(9600);
  // while (!Serial);
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("BrushLens");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);
  ledService.addCharacteristic(rxChar);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characteristic:
  switchCharacteristic.writeValue(0);
  rxChar.setEventHandler(BLEWritten, onRxCharValueUpdate);


  // start advertising
  BLE.advertise();
  Serial.println("BrushLens BLE Started");
  Serial.println("Performing Hardware Test: ");
  // testAllHardware(800);
  // singASong();
  beep();
  Serial.println("Test Finished");
}


void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    // testAllHardware(500);
    beep();
    // while the central is still connected to peripheral:
    while (central.connected()) {

    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    //todo: turn all pins off on disconnect
  }
}
