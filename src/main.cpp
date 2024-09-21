/*************************************************************
  Download latest ERa library here:
    https://github.com/eoh-jsc/era-lib/releases/latest
    https://www.arduino.cc/reference/en/libraries/era
    https://registry.platformio.org/libraries/eoh-ltd/ERa/installation

    ERa website:                https://e-ra.io
    ERa blog:                   https://iotasia.org
    ERa forum:                  https://forum.eoh.io
    Follow us:                  https://www.fb.com/EoHPlatform
 *************************************************************/

// Enable debug console
// Set CORE_DEBUG_LEVEL = 3 first
#define ERA_DEBUG

#define DEFAULT_MQTT_HOST "mqtt1.eoh.io"

// You should get Auth Token in the ERa App or ERa Dashboard
#define ERA_AUTH_TOKEN "1e665cd7-7fd2-4495-885d-bc0b97ae967b"

#include <Arduino.h>
#include <ERa.hpp>
#include <ERa/ERaTimer.hpp>

const char ssid[] = "Wokwi-GUEST";
const char pass[] = "";
// const char ssid[] = "eoh.io";
// const char pass[] = "Eoh@2020";

int buttonPin_door = 14;
int buttonPin_lock = 13;
int led_door = 19;
int led_lock = 18;
int p1;
int p2;

bool currentStateA = true; 
bool currentStateB = true; 

bool lastButtonState_lock = HIGH;
bool lastButtonState_door = HIGH;

ERaTimer timer;

void timerEvent() {
    ERA_LOG("Timer", "Uptime: %d", ERaMillis() / 1000L);
}


void setup() {
    Serial.begin(115200);

    ERa.begin(ssid, pass);
    pinMode(buttonPin_door, INPUT_PULLDOWN);
    pinMode(led_door, OUTPUT);
    pinMode(led_lock, OUTPUT);
    ERa.virtualWrite(V1, 0); 
    ERa.virtualWrite(V0, 0); 
    digitalWrite(led_door, LOW); 
    digitalWrite(led_lock, LOW);   

    timer.setInterval(1000L, timerEvent);
}

ERA_WRITE(V0) {
  p1 = param.getInt();  
  Serial.println(p1);
  if (p1 > 0 && p2 < 1) {
    currentStateB = true;
  } else {
    currentStateB = false;
  } 
}

ERA_WRITE(V1) {
  p2 = param.getInt();  
  if (p2 > 0 ) {
    currentStateA = true;
  } else {
    currentStateA = false;
  }
}

void loop() {
    ERa.run();
    timer.run();

    bool buttonState_lock = digitalRead(buttonPin_lock);
    bool buttonState_door = digitalRead(buttonPin_door);
    
    if (buttonState_lock == LOW && lastButtonState_lock == HIGH) {
        currentStateA = !currentStateA;
        Serial.println(currentStateA ? "A" : "B");
    }
    if (currentStateA) {
        ERa.virtualWrite(V1, 1); 
        digitalWrite(led_lock, HIGH);  
    } else {
        ERa.virtualWrite(V1, 0);
        digitalWrite(led_lock, LOW);   
    }
    lastButtonState_lock = buttonState_lock;

    if (buttonState_door == LOW && lastButtonState_door == HIGH && !currentStateA) {
        currentStateB = !currentStateB;
        Serial.println(currentStateB ? "A1" : "B1");
    }
    if (currentStateB) {
        ERa.virtualWrite(V0, 1); 
        digitalWrite(led_door, HIGH);  
    } else {
        ERa.virtualWrite(V0, 0);
        digitalWrite(led_door, LOW);   
    }
    lastButtonState_door = buttonState_door;

    delay(100);
}

