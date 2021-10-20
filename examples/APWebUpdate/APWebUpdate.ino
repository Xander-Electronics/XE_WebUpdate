#include <XanderElectronics_WebUpdate.h>

const int csSD = 10;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  if(!WebUpdate.begin(csSD)) {
    Serial.println("Failed to initialize library");
    while(1);
  }

  String APName = WebUpdate.getAPName();
  Serial.println("Connect to the wifi network: " + APName);
  Serial.println("Navigate to: 192.168.4.1/update");
}

void loop() {
  WebUpdate.pollServer();
}
