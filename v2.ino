/**********************************************************************************
 *  TITLE: ESP RainMaker + IR + Manual Button control 8 Relays using ESP32 DHT11 LDR (Real time feedback + no WiFi control)
 *  Click on the following links to learn more. 
 *  YouTube Video: https://youtu.be/7knQaSuEgsU
 *  Related Blog : https://iotcircuithub.com/esp32-projects/
 *  by Tech StudyCell
 *  Preferences--> Aditional boards Manager URLs : 
 *  http://arduino.esp8266.com/stable/package_esp8266com_index.json,https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 *  
 *  Download Board ESP32 (2.0.4): https://github.com/espressif/arduino-esp32
 *
 *  Download the libraries 
 *  AceButton Library (1.9.2): https://github.com/bxparks/AceButton
 *  IRremote Library (3.6.1): https://github.com/Arduino-IRremote/Arduino-IRremote
 *  DHT Library (1.4.4): https://github.com/adafruit/DHT-sensor-library
 **********************************************************************************/

#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include <IRremote.h>
#include <Preferences.h> //<------------------
#include <DHT.h> 
#include <SimpleTimer.h>
#include <AceButton.h>
using namespace ace_button;

Preferences pref; //<======================


const char *service_name = "PROV_1234RB8";
const char *pop = "12345RB8";

// define the Chip Id
uint32_t espChipId = 0;

// define the Node Name
char nodeName[] = "ESP32_Relay_8B";

// define the Device Names
char deviceName_1[] = "Switch1";
char deviceName_2[] = "Switch2";
char deviceName_3[] = "Switch3";
char deviceName_4[] = "Switch4";
char deviceName_5[] = "Switch5";
char deviceName_6[] = "Switch6";
char deviceName_7[] = "Switch7";
char deviceName_8[] = "Switch8";

char deviceName_9[] = "Switch9";

//Update the HEX code of IR Remote buttons 0x<HEX CODE>
#define IR_Button_1   0x80BF49B6
#define IR_Button_2   0x80BFC936
#define IR_Button_3   0x80BF33CC
#define IR_Button_4   0x80BF718E
#define IR_Button_5   0x80BFF10E
#define IR_Button_6   0x80BF13EC
#define IR_Button_7   0x80BF51AE
#define IR_Button_8   0x80BFD12E

#define IR_Button_9   0x80BFD10E
#define IR_All_Off    0x80BF3BC4

// define the GPIO connected with Relays and switches
static uint8_t RelayPin1 = 23;  //D23
static uint8_t RelayPin2 = 22;  //D22
static uint8_t RelayPin3 = 21;  //D21
static uint8_t RelayPin4 = 19;  //D19
static uint8_t RelayPin5 = 18;  //D18
static uint8_t RelayPin6 = 5;   //D5
static uint8_t RelayPin7 = 25;  //D25
static uint8_t RelayPin8 = 26;  //D26

static uint8_t RelayPin9 = 17;  //D17. <--------------------------------------

static uint8_t SwitchPin1 = 13;  //D13
static uint8_t SwitchPin2 = 12;  //D12
static uint8_t SwitchPin3 = 14;  //D14
static uint8_t SwitchPin4 = 27;  //D27
static uint8_t SwitchPin5 = 33;  //D33
static uint8_t SwitchPin6 = 32;  //D32
static uint8_t SwitchPin7 = 15;  //D15
static uint8_t SwitchPin8 = 4;   //D4

static uint8_t SwitchPin9 = 39;  //D16 <--------------------------------------


static uint8_t wifiLed      = 2;  // D2
static uint8_t gpio_reset   = 0;  // Press BOOT for reset WiFi
static uint8_t IR_RECV_PIN  = 35; // D35 (IR receiver pin)
static uint8_t DHTPIN       = 16; // RX2  pin connected with DHT
static uint8_t LDR_PIN      = 34; // D34  pin connected with LDR

/* Variable for reading pin status*/
bool toggleState_1 = LOW; //Define integer to remember the toggle state for relay 1
bool toggleState_2 = LOW; //Define integer to remember the toggle state for relay 2
bool toggleState_3 = LOW; //Define integer to remember the toggle state for relay 3
bool toggleState_4 = LOW; //Define integer to remember the toggle state for relay 4
bool toggleState_5 = LOW; //Define integer to remember the toggle state for relay 5
bool toggleState_6 = LOW; //Define integer to remember the toggle state for relay 6
bool toggleState_7 = LOW; //Define integer to remember the toggle state for relay 7
bool toggleState_8 = LOW; //Define integer to remember the toggle state for relay 8

bool toggleState_9 = LOW; //Define integer to remember the toggle state for relay 8. <-----------------------


float temperature1 = 0;
float humidity1   = 0;
float ldrVal  = 0;

DHT dht(DHTPIN, DHT11);

IRrecv irrecv(IR_RECV_PIN);
decode_results results;

SimpleTimer Timer;

ButtonConfig config1;
AceButton button1(&config1);
ButtonConfig config2;
AceButton button2(&config2);
ButtonConfig config3;
AceButton button3(&config3);
ButtonConfig config4;
AceButton button4(&config4);
ButtonConfig config5;
AceButton button5(&config5);
ButtonConfig config6;
AceButton button6(&config6);
ButtonConfig config7;
AceButton button7(&config7);
ButtonConfig config8;
AceButton button8(&config8);

ButtonConfig config9;   // <-------------------
AceButton button9(&config9);  //<-------------------

void handleEvent1(AceButton*, uint8_t, uint8_t);
void handleEvent2(AceButton*, uint8_t, uint8_t);
void handleEvent3(AceButton*, uint8_t, uint8_t);
void handleEvent4(AceButton*, uint8_t, uint8_t);
void handleEvent5(AceButton*, uint8_t, uint8_t);
void handleEvent6(AceButton*, uint8_t, uint8_t);
void handleEvent7(AceButton*, uint8_t, uint8_t);
void handleEvent8(AceButton*, uint8_t, uint8_t);

void handleEvent9(AceButton*, uint8_t, uint8_t);       //  <------------------


//The framework provides some standard device types like switch, lightbulb, fan, temperature sensor.
static Switch my_switch1(deviceName_1, &RelayPin1);
static Switch my_switch2(deviceName_2, &RelayPin2);
static Switch my_switch3(deviceName_3, &RelayPin3);
static Switch my_switch4(deviceName_4, &RelayPin4);
static Switch my_switch5(deviceName_5, &RelayPin5);
static Switch my_switch6(deviceName_6, &RelayPin6);
static Switch my_switch7(deviceName_7, &RelayPin7);
static Switch my_switch8(deviceName_8, &RelayPin8);

static Switch my_switch9(deviceName_9, &RelayPin9);  //<----------------

static TemperatureSensor temperature("Temperature");
static TemperatureSensor humidity("Humidity");
static TemperatureSensor ldr("LDR");

void sysProvEvent(arduino_event_t *sys_event)
{
    switch (sys_event->event_id) {      
        case ARDUINO_EVENT_PROV_START:
#if CONFIG_IDF_TARGET_ESP32
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
        printQR(service_name, pop, "ble");
#else
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
        printQR(service_name, pop, "softap");
#endif        
        break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.printf("\nConnected to Wi-Fi!\n");
        digitalWrite(wifiLed, true);
        break;
    }
}

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
{
    const char *device_name = device->getDeviceName();
    const char *param_name = param->getParamName();

    if(strcmp(device_name, deviceName_1) == 0) {
      
      Serial.printf("Lightbulb = %s\n", val.val.b? "true" : "false");
      
      if(strcmp(param_name, "Power") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        toggleState_1 = val.val.b;
        (toggleState_1 == false) ? digitalWrite(RelayPin1, HIGH) : digitalWrite(RelayPin1, LOW);
        param->updateAndReport(val);
        pref.putBool("Relay1", toggleState_1);

      }
      
    } else if(strcmp(device_name, deviceName_2) == 0) {
      
      Serial.printf("Switch value = %s\n", val.val.b? "true" : "false");

      if(strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        toggleState_2 = val.val.b;
        (toggleState_2 == false) ? digitalWrite(RelayPin2, HIGH) : digitalWrite(RelayPin2, LOW);
        param->updateAndReport(val);
        pref.putBool("Relay2", toggleState_2);

      }
  
    } else if(strcmp(device_name, deviceName_3) == 0) {
      
      Serial.printf("Switch value = %s\n", val.val.b? "true" : "false");

      if(strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        toggleState_3 = val.val.b;
        (toggleState_3 == false) ? digitalWrite(RelayPin3, HIGH) : digitalWrite(RelayPin3, LOW);
        param->updateAndReport(val);
        pref.putBool("Relay3", toggleState_3);

      }
  
    } else if(strcmp(device_name, deviceName_4) == 0) {
      
      Serial.printf("Switch value = %s\n", val.val.b? "true" : "false");

      if(strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        toggleState_4 = val.val.b;
        (toggleState_4 == false) ? digitalWrite(RelayPin4, HIGH) : digitalWrite(RelayPin4, LOW);
        param->updateAndReport(val);
        pref.putBool("Relay4", toggleState_4);

      } 
       
    } else if(strcmp(device_name, deviceName_5) == 0) {
      
      Serial.printf("Lightbulb = %s\n", val.val.b? "true" : "false");
      
      if(strcmp(param_name, "Power") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        toggleState_5 = val.val.b;
        (toggleState_5 == false) ? digitalWrite(RelayPin5, HIGH) : digitalWrite(RelayPin5, LOW);
        param->updateAndReport(val);
        pref.putBool("Relay5", toggleState_5);

      }
      
    } else if(strcmp(device_name, deviceName_6) == 0) {
      
      Serial.printf("Switch value = %s\n", val.val.b? "true" : "false");

      if(strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        toggleState_6 = val.val.b;
        (toggleState_6 == false) ? digitalWrite(RelayPin6, HIGH) : digitalWrite(RelayPin6, LOW);
        param->updateAndReport(val);
        pref.putBool("Relay6", toggleState_6);
       
      }
  
    } else if(strcmp(device_name, deviceName_7) == 0) {
      
      Serial.printf("Switch value = %s\n", val.val.b? "true" : "false");

      if(strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        toggleState_7 = val.val.b;
        (toggleState_7 == false) ? digitalWrite(RelayPin7, HIGH) : digitalWrite(RelayPin7, LOW);
        param->updateAndReport(val);
        pref.putBool("Relay7", toggleState_7);
       
      }
  
    } else if(strcmp(device_name, deviceName_8) == 0) {
      
      Serial.printf("Switch value = %s\n", val.val.b? "true" : "false");

      if(strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        toggleState_8 = val.val.b;
        (toggleState_8 == false) ? digitalWrite(RelayPin8, HIGH) : digitalWrite(RelayPin8, LOW);
        param->updateAndReport(val);
        pref.putBool("Relay8", toggleState_8);      
      }  
    } else if(strcmp(device_name, deviceName_9) == 0) {
      
      Serial.printf("Switch value = %s\n", val.val.b? "true" : "false");

      if(strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        toggleState_9 = val.val.b;
        (toggleState_9 == false) ? digitalWrite(RelayPin9, HIGH) : digitalWrite(RelayPin9, LOW);
        param->updateAndReport(val);
        pref.putBool("Relay9", toggleState_9);
      }  
    } 
}

void readSensor(){

  ldrVal = map(analogRead(LDR_PIN), 0, 4095, 10, 0);
  Serial.print("LDR - "); Serial.println(ldrVal);
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else {
    humidity1 = h;
    temperature1 = t;
    Serial.print("Temperature - "); Serial.println(t);
    Serial.print("Humidity - "); Serial.println(h);
  }  
}

void sendSensor()
{
  readSensor();
  temperature.updateAndReportParam("Temperature", temperature1);
  humidity.updateAndReportParam("Temperature", humidity1);
  ldr.updateAndReportParam("Temperature", ldrVal);
}

void ir_remote(){
  if (irrecv.decode(&results)) {
      switch(results.value){
          case IR_Button_1:  
            digitalWrite(RelayPin1, toggleState_1);
            toggleState_1 = !toggleState_1;
            pref.putBool("Relay1", toggleState_1);

            my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);
            delay(100);            
            break;
          case IR_Button_2:  
            digitalWrite(RelayPin2, toggleState_2);
            toggleState_2 = !toggleState_2;
            pref.putBool("Relay2", toggleState_2);

            my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);
            delay(100);            
            break;
          case IR_Button_3:  
            digitalWrite(RelayPin3, toggleState_3);
            toggleState_3 = !toggleState_3;
            pref.putBool("Relay3", toggleState_3);

            my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3);
            delay(100);            
            break;
          case IR_Button_4:  
            digitalWrite(RelayPin4, toggleState_4);
            toggleState_4 = !toggleState_4;
            pref.putBool("Relay4", toggleState_4);

            my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4);
            delay(100);            
            break;
          case IR_Button_5:  
            digitalWrite(RelayPin5, toggleState_5);
            toggleState_5 = !toggleState_5;
            pref.putBool("Relay5", toggleState_5);

            my_switch5.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_5);
            delay(100);            
            break;
          case IR_Button_6:  
            digitalWrite(RelayPin6, toggleState_6);
            toggleState_6 = !toggleState_6;
            pref.putBool("Relay6", toggleState_6);

            my_switch6.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_6);
            delay(100);            
            break;
          case IR_Button_7:  
            digitalWrite(RelayPin7, toggleState_7);
            toggleState_7 = !toggleState_7;
            pref.putBool("Relay7", toggleState_7);

            my_switch7.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_7);
            delay(100);            
            break;
          case IR_Button_8:  
            digitalWrite(RelayPin8, toggleState_8);
            toggleState_8 = !toggleState_8;
            pref.putBool("Relay8", toggleState_8);

            my_switch8.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_8);
            delay(100);            
            break;


          case IR_Button_9:    //<---------------------
            digitalWrite(RelayPin9, toggleState_9);
            toggleState_8 = !toggleState_8;
            pref.putBool("Relay9", toggleState_9);

            my_switch9.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_9);
            delay(100);            
            break;


          case IR_All_Off:
            all_SwitchOff();  
            break;
          default : break;         
        }   
        //Serial.println(results.value, HEX);    
        irrecv.resume();   
  } 
}

void all_SwitchOff(){
  toggleState_1 = 0; digitalWrite(RelayPin1, HIGH); my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1); delay(100);
  toggleState_2 = 0; digitalWrite(RelayPin2, HIGH); my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2); delay(100);
  toggleState_3 = 0; digitalWrite(RelayPin3, HIGH); my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3); delay(100);
  toggleState_4 = 0; digitalWrite(RelayPin4, HIGH); my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4); delay(100);
  toggleState_5 = 0; digitalWrite(RelayPin5, HIGH); my_switch5.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_5); delay(100);
  toggleState_6 = 0; digitalWrite(RelayPin6, HIGH); my_switch6.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_6); delay(100);
  toggleState_7 = 0; digitalWrite(RelayPin7, HIGH); my_switch7.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_7); delay(100);
  toggleState_8 = 0; digitalWrite(RelayPin8, HIGH); my_switch7.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_8); delay(100);

  
  
  
  toggleState_9 = 0; digitalWrite(RelayPin9, HIGH); my_switch9.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_9); delay(100);  //<-----------------
}

void getRelayState()
{
  Serial.println("reading data ");
  toggleState_1 = pref.getBool("Relay1", 0);
  digitalWrite(RelayPin1, !toggleState_1); 
  my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);
  delay(200);
  toggleState_2 = pref.getBool("Relay2", 0);
  digitalWrite(RelayPin2, !toggleState_2); 
  my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);
  delay(200);
  toggleState_3 = pref.getBool("Relay3", 0);
  digitalWrite(RelayPin3, !toggleState_3); 
  my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3);
  delay(200);
  toggleState_4 = pref.getBool("Relay4", 0);
  digitalWrite(RelayPin4, !toggleState_4); 
  my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4);
  delay(200);
  toggleState_5 = pref.getBool("Relay5", 0);
  digitalWrite(RelayPin5, !toggleState_5); 
  my_switch5.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_5);
  delay(200);
  toggleState_6 = pref.getBool("Relay6", 0);
  digitalWrite(RelayPin6, !toggleState_6); 
  my_switch6.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_6);
  delay(200);
  toggleState_7 = pref.getBool("Relay7", 0);
  digitalWrite(RelayPin7, !toggleState_7); 
  my_switch7.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_7);
  delay(200);
  toggleState_8 = pref.getBool("Relay8", 0);
  digitalWrite(RelayPin8, !toggleState_8); 
  my_switch8.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_8);
  delay(200);
  

  toggleState_9 = pref.getBool("Relay9", 0);
  digitalWrite(RelayPin9, !toggleState_9); 
  my_switch9.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_9);
  delay(200);
}  

void setup()
{
    //uint32_t espChipId = espespChipId;
    
    Serial.begin(115200);

    pref.begin("Relay_State", false); //<======================

    
    // Set the Relays GPIOs as output mode
    pinMode(RelayPin1, OUTPUT);
    pinMode(RelayPin2, OUTPUT);
    pinMode(RelayPin3, OUTPUT);
    pinMode(RelayPin4, OUTPUT);
    pinMode(RelayPin5, OUTPUT);
    pinMode(RelayPin6, OUTPUT);
    pinMode(RelayPin7, OUTPUT);
    pinMode(RelayPin8, OUTPUT);  

    pinMode(RelayPin9, OUTPUT);  //<--------------------


    pinMode(wifiLed, OUTPUT);
    
    // Configure the input GPIOs
    pinMode(SwitchPin1, INPUT_PULLUP);
    pinMode(SwitchPin2, INPUT_PULLUP);
    pinMode(SwitchPin3, INPUT_PULLUP);
    pinMode(SwitchPin4, INPUT_PULLUP);
    pinMode(SwitchPin5, INPUT_PULLUP);
    pinMode(SwitchPin6, INPUT_PULLUP);
    pinMode(SwitchPin7, INPUT_PULLUP);
    pinMode(SwitchPin8, INPUT_PULLUP);

    pinMode(SwitchPin9, INPUT_PULLUP);//<-----------


    pinMode(gpio_reset, INPUT);
    
    // Write to the GPIOs the default state on booting
    digitalWrite(RelayPin1, !toggleState_1);
    digitalWrite(RelayPin2, !toggleState_2);
    digitalWrite(RelayPin3, !toggleState_3);
    digitalWrite(RelayPin4, !toggleState_4);
    digitalWrite(RelayPin5, !toggleState_5);
    digitalWrite(RelayPin6, !toggleState_6);
    digitalWrite(RelayPin7, !toggleState_7);
    digitalWrite(RelayPin8, !toggleState_8);

    digitalWrite(RelayPin9, !toggleState_9);  //<----------------
    digitalWrite(wifiLed, LOW);

    irrecv.enableIRIn(); // Enabling IR sensor
    dht.begin();    // Enabling DHT sensor

    config1.setEventHandler(button1Handler);
    config2.setEventHandler(button2Handler);
    config3.setEventHandler(button3Handler);
    config4.setEventHandler(button4Handler);
    config5.setEventHandler(button5Handler);
    config6.setEventHandler(button6Handler);
    config7.setEventHandler(button7Handler);
    config8.setEventHandler(button8Handler);
    config9.setEventHandler(button9Handler); //<==================
    
    
    button1.init(SwitchPin1);
    button2.init(SwitchPin2);
    button3.init(SwitchPin3);
    button4.init(SwitchPin4);
    button5.init(SwitchPin5);
    button6.init(SwitchPin6);
    button7.init(SwitchPin7);
    button8.init(SwitchPin8);

    button9.init(SwitchPin9);  //<===============

    Node my_node;    
    my_node = RMaker.initNode(nodeName);

    //Standard switch device
    my_switch1.addCb(write_callback);
    my_switch2.addCb(write_callback);
    my_switch3.addCb(write_callback);
    my_switch4.addCb(write_callback);
    my_switch5.addCb(write_callback);
    my_switch6.addCb(write_callback);
    my_switch7.addCb(write_callback);
    my_switch8.addCb(write_callback);

    my_switch9.addCb(write_callback); //<=============


    //Add switch device to the node   
    my_node.addDevice(my_switch1);
    my_node.addDevice(my_switch2);
    my_node.addDevice(my_switch3);
    my_node.addDevice(my_switch4);
    my_node.addDevice(my_switch5);
    my_node.addDevice(my_switch6);
    my_node.addDevice(my_switch7);
    my_node.addDevice(my_switch8);

    my_node.addDevice(my_switch9);  //<====================


    my_node.addDevice(temperature);
    my_node.addDevice(humidity);
    my_node.addDevice(ldr);

    Timer.setInterval(2000); 

    //This is optional 
    RMaker.enableOTA(OTA_USING_PARAMS);
    //If you want to enable scheduling, set time zone for your region using setTimeZone(). 
    //The list of available values are provided here https://rainmaker.espressif.com/docs/time-service.html
    // RMaker.setTimeZone("Asia/Shanghai");
    // Alternatively, enable the Timezone service and let the phone apps set the appropriate timezone
    RMaker.enableTZService();
    RMaker.enableSchedule();

    //Service Name
    for(int i=0; i<17; i=i+8) {
      espChipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }

    Serial.printf("\nChip ID:  %d Service Name: %s\n", espChipId, service_name);

    Serial.printf("\nStarting ESP-RainMaker\n");
    RMaker.start();

    WiFi.onEvent(sysProvEvent);
#if CONFIG_IDF_TARGET_ESP32
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
#else
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
#endif
    getRelayState(); // Get the last state of Relays <=================

    my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch5.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch6.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch7.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch8.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);

    my_switch9.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false); //<========================
}

void loop()
{
    // Read GPIO0 (external button to reset device
    if(digitalRead(gpio_reset) == LOW) { //Push button pressed
        Serial.printf("Reset Button Pressed!\n");
        // Key debounce handling
        delay(100);
        int startTime = millis();
        while(digitalRead(gpio_reset) == LOW) delay(50);
        int endTime = millis();

        if ((endTime - startTime) > 10000) {
          // If key pressed for more than 10secs, reset all
          Serial.printf("Reset to factory.\n");
          RMakerFactoryReset(2);
        } else if ((endTime - startTime) > 3000) {
          Serial.printf("Reset Wi-Fi.\n");
          // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
          RMakerWiFiReset(2);
        }
    }
    delay(100);

    if (WiFi.status() != WL_CONNECTED)
    {
      //Serial.println("WiFi Not Connected");
      digitalWrite(wifiLed, false);
    }
    else
    {
      //Serial.println("WiFi Connected");
      digitalWrite(wifiLed, true);
      if (Timer.isReady()) { 
        //Serial.println("Sending Sensor Data");
        sendSensor();
        Timer.reset();      // Reset a second timer
      }
    }

    ir_remote(); //IR remote Control
    
    button1.check();
    button2.check();
    button3.check();
    button4.check();
    button5.check();
    button6.check();
    button7.check();
    button8.check();

    button9.check(); //<===============
}

void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT1");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin1, toggleState_1);
      toggleState_1 = !toggleState_1;
       pref.putBool("Relay1", toggleState_1);
     
      my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);
      break;
  }
}
void button2Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT2");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin2, toggleState_2);
      toggleState_2 = !toggleState_2;
      pref.putBool("Relay2", toggleState_2);

      my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);
      break;
  }
}
void button3Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT3");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin3, toggleState_3);
      toggleState_3 = !toggleState_3;
      pref.putBool("Relay3", toggleState_3);

      my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3);
      break;
  }
}
void button4Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT4");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin4, toggleState_4);
      toggleState_4 = !toggleState_4;
      pref.putBool("Relay4", toggleState_4);

      my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4);
      break;
  }
}
void button5Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT5");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin5, toggleState_5);
      toggleState_5 = !toggleState_5;
      pref.putBool("Relay5", toggleState_5);

      my_switch5.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_5);
      break;
  }
}
void button6Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT6");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin6, toggleState_6);
      toggleState_6 = !toggleState_6;
      pref.putBool("Relay6", toggleState_6);

      my_switch6.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_6);
      break;
  }
}
void button7Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT7");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin7, toggleState_7);
      toggleState_7 = !toggleState_7;
      pref.putBool("Relay7", toggleState_7);

      my_switch7.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_7);
      break;
  }
}
void button8Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT8");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin8, toggleState_8);
      toggleState_8 = !toggleState_8;
      pref.putBool("Relay8", toggleState_8);

      my_switch8.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_8);
      break;
  }
}

void button9Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT9");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin9, toggleState_9);
      toggleState_9 = !toggleState_9;
      pref.putBool("Relay9", toggleState_9);

      my_switch9.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_9);
      break;
  }
}

