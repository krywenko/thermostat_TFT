
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "RTClib.h"
#include "PCF8574.h"
PCF8574 pcf8574(0x38);
#include <Adafruit_ILI9341esp.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <XPT2046.h>
#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif
// Modify the following two lines to match your hardware
// Also, update calibration parameters below, as necessary

// For the esp shield, these are the default.
#define TFT_DC 2
#define TFT_CS 15

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
XPT2046 touch(/*cs=*/ 16, /*irq=*/ 5);

Adafruit_GFX_Button button;
Adafruit_GFX_Button button1;
Adafruit_GFX_Button button2;
Adafruit_GFX_Button button3;
Adafruit_GFX_Button button4;
Adafruit_GFX_Button button5;
Adafruit_GFX_Button button6;

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};




String SensorFail = "0";
int val0;
int val1;
int val2;
int val3;
int val4;
int val5;
int val6;
int val7;
int val8;
int val9;
int val10;
int val12;


int r1=16;
int r2=5;
int r3=4;
int r4=0;// GPIO5 or for NodeMCU you can directly write D1
int r5=14;
int r6=12;
int r7=13;
int r8=15;

String SensorT = "/temp/28ff4f3c8c163e9"; // sensor wire for thermostat uses 2-4 digital pins 5-8
String SensorT1 = "";///"/temp/28ff5d370164db"; // simple on off of relay if oy want * realy controls leave  SensorT empty and fill in T1 - T8 with your IDs
String SensorT2 = "/temp/28ffb7da8b1634b";
String SensorT3 = "";
String SensorT4 = "";
String SensorT5 = "";// past this point you might have to remove SensorT data  refference and us "" as these overlap with digital pin use 
String SensorT6 = "";
String SensorT7 = "";
String SensorT8 = ""; //disable Serial   if using digital pin 7-8

String SensorTF ;  // for dectecting sensor failure
String SensorT1F;
String SensorT2F; 
String SensorT3F;
String SensorT4F;
String SensorT5F;  
String SensorT6F; 
String SensorT7F ;
String SensorT8F; 


int heat; 
int fan;
int S_T;
int E_T;
int DiFF;
int E_Diff;

float preset;
float preset1;
float preset2;
float preset3;
float preset4;
float preset5;
float preset6;
float preset7;
float preset8;

float CurrentT; 
float CurrentT1;
float CurrentT2;
float CurrentT3;
float CurrentT4;
float CurrentT5;
float CurrentT6;
float CurrentT7;
float CurrentT8;

long lastMsg2 = 0;
long lastMsg1 = 0;
long lastMsg = 0;
char msg[50];
int value = 0;
int cntID=0;

//Basic Wifi and MQTT-Server Information
const char* ssid = "IOT";
const char* password = "";
char* server = "192.168.168.150";
String clientName;
//DS18B20 on GPIO2
OneWire  ds(0);

WiFiClient wifiClient;
PubSubClient client(server, 1883, wifiClient);

void callback(char* topic, byte* payload, unsigned int length) {
  
String input1 = "/switch/SW1";  /// edit to denote what switch topic it is asscosiated with

if (input1 == topic){
 String inString = "";
  //scan++;
  for (int i=0;i<length;i++) {
     inString +=  ((char)payload[i]);
  }
  Serial.print("Recieved("); Serial.print(inString); Serial.println(")");
  //int presetp = inString.toInt();
  if (inString=="1-0") {   ///relay control thermostat interface
    preset = (preset+1);
    Serial.print("up "); Serial.print( preset); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset;
    Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset", (char*) raw_CMD.c_str()); 
    }
  }
  if (inString=="0-0") {
    preset = (preset-1);
    Serial.print("down "); Serial.print( preset); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset;
     Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset", (char*) raw_CMD.c_str()); 
    }
  }
  if (inString=="1-f") { //fan 
    fan = (fan+1);
    Serial.print("fan-on "); Serial.print(fan); Serial.print( " ");
    String raw_CMD ;
    if (fan >1) fan =0;
    raw_CMD= fan;
    Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tfan", (char*) raw_CMD.c_str()); 
    }
  }
  if (inString=="0-f") {
    fan = (fan-1);
    Serial.print("fan-off "); Serial.print(fan); Serial.print( " ");
    String raw_CMD ;
    if (fan <0) fan =0;
    raw_CMD= fan;
     Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/Switch/SW1/Tfan", (char*) raw_CMD.c_str()); 
    }
  }
  if (inString=="1-h") { //heating
    heat = (heat+1);
    Serial.print("h-up "); Serial.print( heat); Serial.print( " ");
    String raw_CMD ;
    if (heat >2) heat =0;
    raw_CMD= heat;
    Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Theat", (char*) raw_CMD.c_str()); 
    }
  }
  if (inString=="0-h") {
    heat = (heat-1);
    Serial.print("h-down "); Serial.print( heat); Serial.print( " ");
    String raw_CMD ;
    if (heat <0) heat =2;
    raw_CMD= heat;
     Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Theat", (char*) raw_CMD.c_str()); 
    }
  }
if (inString=="1-1") {   //relay 1
    preset1 = (preset1+1);
    Serial.print("up "); Serial.print( preset1); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset1;
    Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset1", (char*) raw_CMD.c_str()); 
    }
  }
  if (inString=="0-1") {
    preset1 = (preset1-1);
    Serial.print("down "); Serial.print( preset1); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset1;
     Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset1", (char*) raw_CMD.c_str()); 
    }
  }
if (inString=="1-2") { //relay2
    preset2 = (preset2+1);
    Serial.print("up "); Serial.print( preset2); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset2;
    Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset2", (char*) raw_CMD.c_str()); 
    }
  }
  if (inString=="0-2") {
    preset2 = (preset2-1);
    Serial.print("down "); Serial.print( preset2); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset2;
     Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset2", (char*) raw_CMD.c_str()); 
    }
  }
if (inString=="1-3") {  //relay3
    preset3 = (preset3+1);
    Serial.print("up "); Serial.print( preset3); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset3;
    Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset3", (char*) raw_CMD.c_str()); 
    }
  }
  if (inString=="0-3") {
    preset3 = (preset3-1);
    Serial.print("down "); Serial.print( preset3); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset3;
     Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset3", (char*) raw_CMD.c_str()); 
    }
  }
  if (inString=="1-4") { //relay4
    preset4 = (preset4+1);
    Serial.print("up "); Serial.print( preset4); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset4;
    Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset4", (char*) raw_CMD.c_str()); 
    }
  }
  if (inString=="0-4") {
    preset4 = (preset4-1);
    Serial.print("down "); Serial.print( preset4); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset4;
     Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset4", (char*) raw_CMD.c_str()); 
    }
  }
if (inString=="1-5") { //relay5
    preset5 = (preset5+1);
    Serial.print("up "); Serial.print( preset5); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset5;
    Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset5", (char*) raw_CMD.c_str()); 
    }
  }
  if (inString=="0-5") {
    preset5 = (preset5-1);
    Serial.print("down "); Serial.print( preset5); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset5;
     Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset5", (char*) raw_CMD.c_str()); 
    }
  }
if (inString=="1-6") {      //relay6
    preset6 = (preset6+1);
    Serial.print("up "); Serial.print( preset6); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset6;
    Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset6", (char*) raw_CMD.c_str()); 
    }
  }
  if (inString=="0-6") {
    preset6 = (preset6-1);
    Serial.print("down "); Serial.print( preset6); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset6;
     Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset6", (char*) raw_CMD.c_str()); 
    }
  }
  if (inString=="1-7") {   //relay7
    preset7 = (preset7+1);
    Serial.print("up "); Serial.print( preset7); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset7;
    Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset7", (char*) raw_CMD.c_str()); 
    }
  }
  if (inString=="0-7") {
    preset7 = (preset7-1);
    Serial.print("down "); Serial.print( preset7); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset7;
     Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset7", (char*) raw_CMD.c_str()); 
    }
  }
  if (inString=="1-8") {     //relay8
    preset8 = (preset8+1);
    Serial.print("up "); Serial.print( preset8); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset8;
    Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset8", (char*) raw_CMD.c_str()); 
    }
  }
  if (inString=="0-8") {
    preset8 = (preset8-1);
    Serial.print("down "); Serial.print( preset8); Serial.print( " ");
    String raw_CMD ;
    raw_CMD= preset8;
     Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/SW1/Tset8", (char*) raw_CMD.c_str()); 
    }
  }
  

  }
  touch_display();
}


String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  EEPROM.begin(512);
  SPI.setFrequency(ESP_SPI_FREQ);
  pcf8574.begin();
  tft.begin();
  delay(10);
  
  touch.begin(tft.width(), tft.height()); // Must be done before setting rotation
touch.setRotation(touch.ROT90);
tft.fillScreen(ILI9341_BLACK);
 touch.setCalibration( 164,1755,1728,209  );
  tft.setRotation(1);
  button.initButton(&tft, 226, 70, 59, 40, ILI9341_DARKCYAN, ILI9341_BLUE, ILI9341_GREENYELLOW, "UP", 2);
  button.drawButton();
   button1.initButton(&tft, 290, 70, 59, 40, ILI9341_DARKCYAN, ILI9341_BLUE, ILI9341_GREENYELLOW, "DOWN", 2);
  button1.drawButton();
  button2.initButton(&tft, 226, 120, 59, 40, ILI9341_DARKCYAN, ILI9341_BLUE, ILI9341_GREENYELLOW, "S-T", 2);
  button2.drawButton();
  button5.initButton(&tft, 290, 120, 59, 40, ILI9341_DARKCYAN, ILI9341_BLUE, ILI9341_GREENYELLOW, "E-T", 2);
  button5.drawButton();
  button3.initButton(&tft, 226, 170, 59, 40, ILI9341_DARKCYAN, ILI9341_BLUE, ILI9341_GREENYELLOW, "FAN", 2);
  button3.drawButton();
  button6.initButton(&tft, 290, 170, 59, 40, ILI9341_DARKCYAN, ILI9341_BLUE, ILI9341_GREENYELLOW, "DIF", 2);
  button6.drawButton();
  button4.initButton(&tft, 260, 220, 120, 40, ILI9341_DARKCYAN, ILI9341_BLUE, ILI9341_GREENYELLOW, "OFF/C/ON", 2);
  button4.drawButton();
  
Serial.print("tftx ="); Serial.print(tft.width()); Serial.print(" tfty ="); Serial.println(tft.height());
  //tft.fillScreen(ILI9341_BLACK);

  
  pcf8574.pinMode(r1, OUTPUT);
  pcf8574.pinMode(r2, OUTPUT);
  pcf8574.pinMode(r3, OUTPUT);
  pcf8574.pinMode(r4, OUTPUT);
  pcf8574.pinMode(r5, OUTPUT);
  pcf8574.pinMode(r6, OUTPUT);
  pcf8574.pinMode(r7, OUTPUT);
  pcf8574.pinMode(r8, OUTPUT);
   pcf8574.digitalWrite(r1, HIGH);
   pcf8574.digitalWrite(r2, HIGH);
   pcf8574.digitalWrite(r3, HIGH);
   pcf8574.digitalWrite(r4, HIGH);
   pcf8574.digitalWrite(r5, HIGH);
   pcf8574.digitalWrite(r6, HIGH);
   pcf8574.digitalWrite(r7, HIGH);
   pcf8574.digitalWrite(r8, HIGH);
  
  int firsttime = EEPROM.read(11);
  if (firsttime==1){ Serial.println(" EEprom Written Already");} else {
    int tmp = 18; 
    EEPROM.write(0,tmp);
    EEPROM.write(1, tmp);
    EEPROM.write(2, tmp);
    EEPROM.write(3, tmp);
    EEPROM.write(4, tmp);
    EEPROM.write(5,tmp);
    EEPROM.write(6,tmp);
    EEPROM.write(7,tmp);
    EEPROM.write(8,tmp);
    EEPROM.write(9,1);
    EEPROM.write(10,1);
    EEPROM.write(11,1);
    EEPROM.write(12,6);
    EEPROM.write(13,22);
    EEPROM.write(14,0);    
    Serial.println(" Setting primary  tempertures to eeprom");
    EEPROM.commit();
    delay(50);
  }
  
 Serial.println("_________________________________");  
int val0 = EEPROM.read(0);Serial.println(val0);
int val1 = EEPROM.read(1);Serial.println(val1);
int val2 = EEPROM.read(2);Serial.println(val2);
int val3 = EEPROM.read(3);Serial.println(val3);
int val4 = EEPROM.read(4);Serial.println(val4);
int val5 = EEPROM.read(5);Serial.println(val5);
int val6 = EEPROM.read(6);Serial.println(val6);
int val7 = EEPROM.read(7);Serial.println(val7);
int val8 = EEPROM.read(8);Serial.println(val8);
int val9 = EEPROM.read(9);Serial.println(val9);
int va10 = EEPROM.read(10);Serial.println(val10);
int val11 = EEPROM.read(11);Serial.println(val11);
int val12 = EEPROM.read(12);Serial.println(S_T);
int val13 = EEPROM.read(13);Serial.println(E_T);
int val14 = EEPROM.read(14);Serial.println(DiFF);
Serial.println("_________________________________");
preset = val0;
preset1 = val1;
preset2 = val2;
preset3 = val3;
preset4 = val4;
preset5 = val5;
preset6 = val6;
preset7 = val7;
preset8 = val8;
heat = val9;
fan = val10; 
S_T= val12;
E_T = val13;
DiFF = val14;

        if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
  setup_wifi();
  client.setCallback(callback);
}

void setup_wifi(){
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Generate client name based on MAC address and last 8 bits of microsecond counter
  
  clientName += "esp8266-";
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);
  clientName += "-";
  clientName += String(micros() & 0xff, 16);

  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);
  
  //MQTT_Connect();
  }
  
  void MQTT_Connect(){
     // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect((char*) clientName.c_str())) {
    Serial.println("Connected to MQTT broker");
   client.subscribe("/switch/SW1");
   delay(500);
   pub_info();
   
   //client.subscribe("/temp/28ff4f3c8c163e9");
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    delay(5000);
  }
 // client.setCallback(callback);
    }
  }
//timer.setInterval(9000L, setup_wifi);


void pub_info(){
  String raw_CMD ;
    raw_CMD= preset;
    Serial.println(raw_CMD);
client.publish("/switch/SW1/Tset", (char*) raw_CMD.c_str());
 raw_CMD= preset1;
    Serial.println(raw_CMD);
client.publish("/switch/SW1/Tset1", (char*) raw_CMD.c_str());
 raw_CMD= preset2;
    Serial.println(raw_CMD);
client.publish("/switch/SW1/Tset2", (char*) raw_CMD.c_str());
 raw_CMD= preset3;
    Serial.println(raw_CMD);
client.publish("/switch/SW1/Tset3", (char*) raw_CMD.c_str());
 raw_CMD= preset4;
    Serial.println(raw_CMD);
client.publish("/switch/SW1/Tset4", (char*) raw_CMD.c_str());
 raw_CMD= preset5;
    Serial.println(raw_CMD);
client.publish("/switch/SW1/Tset5", (char*) raw_CMD.c_str());
 raw_CMD= preset6;
    Serial.println(raw_CMD);
client.publish("/switch/SW1/Tset6", (char*) raw_CMD.c_str());
 raw_CMD= preset7;
    Serial.println(raw_CMD);
client.publish("/switch/SW1/Tset7", (char*) raw_CMD.c_str());
 raw_CMD= preset8;
    Serial.println(raw_CMD);
client.publish("/switch/SW1/Tset8", (char*) raw_CMD.c_str());
 raw_CMD= fan;
    Serial.println(raw_CMD);
client.publish("/switch/SW1/Tfan", (char*) raw_CMD.c_str());
 raw_CMD= heat;
    Serial.println(raw_CMD);
client.publish("/switch/SW1/Theat", (char*) raw_CMD.c_str());
}


static uint16_t prev_x = 0xffff, prev_y = 0xffff;

void touch_display(){

 uint16_t xq, yq;
if (touch.isTouching()) {
    touch.getPosition(xq, yq);
/*//  Serial.print("x ="); Serial.print(x); Serial.print(" y ="); Serial.println(y);
    if (prev_x == 0xffff) {
      tft.drawPixel(xq, yq,ILI9341_BLUE);
    } else {
      tft.drawLine(prev_x, prev_y, xq, yq,ILI9341_BLUE);
    }
    prev_x = xq;
    prev_y = yq;
  } else {
    prev_x = prev_y = 0xffff;*/
  }
  button.press(button.contains(xq, yq)); // tell the button it is pressed
  button1.press(button1.contains(xq, yq)); // tell the button it is pressed
   button2.press(button2.contains(xq, yq)); // tell the button it is pressed
   button5.press(button5.contains(xq, yq)); // tell the button it is pressed
  button3.press(button3.contains(xq, yq)); // tell the button it is pressed
   button6.press(button6.contains(xq, yq)); // tell the button it is pressed
  button4.press(button4.contains(xq, yq)); // tell the button it is pressed
  //button5.press(button5.contains(xq, yq)); // tell the button it is pressed
  
  
  

// now we can ask the buttons if their state has changed
  if (button.justReleased()) {
    preset=preset+1;
    button.drawButton();
    //button1.drawButton();// draw normal
  }

  if (button.justPressed()) {
    button.drawButton(true);
   
    // draw invert!
  } 

  if (button1.justReleased()) {
   preset=preset-1;
    button1.drawButton();
    
  }

  if (button1.justPressed()) {
    button1.drawButton(true);
   
  } 

if (button2.justReleased()) {
    S_T = S_T+1;
    if(S_T >24){S_T=0;}
    button2.drawButton();
    
  }

  if (button2.justPressed()) {
    button2.drawButton(true);
    
  } 

  if (button5.justReleased()) {
    E_T = E_T+1;
    if(E_T >24){E_T=0;}
    button5.drawButton();
    
  }

  if (button5.justPressed()) {
    button5.drawButton(true);
    
  } 
  if (button3.justReleased()) {
    fan=fan+1;
    if (fan >1){ fan=0;}
   
    delay(10);
    button3.drawButton();
    
  }

  if (button3.justPressed()) {
    button3.drawButton(true);
   
  } 

  if (button6.justReleased()) {
    DiFF=DiFF+1;
    if (DiFF >5){ DiFF=0;}
    button6.drawButton();
    
  }

  if (button6.justPressed()) {
    button6.drawButton(true);
    
  } 
  if (button4.justReleased()) {
    heat=heat+1;
    if (heat > 2){ heat=0;}
    delay(10);
    button4.drawButton();
   
  }

  if (button4.justPressed()) {
    button4.drawButton(true);
    
  } 
  
}


void get_time(){
int TIME;
    DateTime now = rtc.now();
 tft.setCursor(10,10);tft.setTextSize(3);tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    tft.print(daysOfTheWeek[now.dayOfTheWeek()]);
   tft.print(" ");
    tft.print(now.hour(),DEC);
    //TIME = (now.hour(), DEC);
    //tft.print(TIME);
   tft.print(':');
   tft.print(now.minute(), DEC);
   tft.print("      ");
  //tft.fillRoundRect(0,35,180,30,0,ILI9341_BLACK);
    TIME=now.hour();
    if ((TIME >= E_T) || (TIME < S_T)) {E_Diff =1;} else {E_Diff =0;}
    //.setCursor(10,200); tft.print(TIME); tft.print(" "); tft.print(E_T); tft.print(" "); tft.print(S_T);
   if (E_Diff ==1){tft.fillCircle(15,220,15,ILI9341_CYAN);} else {tft.fillCircle(15,220,15,ILI9341_BLACK);}
  
}



void loop() {

 tft.fillRoundRect(0,40,100,30,0,ILI9341_BLACK); 
if(fan==0){ tft.fillCircle(80,54,10,ILI9341_RED);}
if(fan==1){ tft.fillCircle(80,54,10,ILI9341_GREEN);}
if(heat==0){ tft.fillCircle(110,54,10,ILI9341_RED);}
if(heat==1){ tft.fillCircle(110,54,10,ILI9341_BLUE);}
if(heat==2){ tft.fillCircle(110,54,10,ILI9341_YELLOW);}

int TEMP;

TEMP=CurrentT;
tft.setCursor(10,75);tft.setTextSize(15);tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK); tft.println(TEMP);
int Set;
Set = preset;
tft.setCursor(65,200);tft.setTextSize(6);tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK); tft.println(Set);
tft.setCursor(45,220);tft.setTextSize(2);tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); tft.println(S_T);
tft.setCursor(145,220);tft.setTextSize(2);tft.setTextColor(ILI9341_RED, ILI9341_BLACK); tft.println(E_T);
tft.setCursor(140,200);tft.setTextSize(2);tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); tft.println(Set-DiFF);

touch_display();

  
  if(WiFi.status() != WL_CONNECTED){setup_wifi();}
  if (!client.connected()) {
    MQTT_Connect();
 }
 client.loop();
  //Sensor();
long now2 = millis();
  if (now2 - lastMsg2 > 5000) {
    lastMsg2 = now2;
pub_info();
  get_time();
  }
  
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    
Sensor();

if ((SensorT !="") && (SensorTF != "" )){  Relay_control(); SensorTF =  "";SensorFail="0";} else{ if (SensorT  !=""){ pcf8574.digitalWrite(r8, HIGH);pcf8574.digitalWrite(r7, HIGH);SensorFail= "0";failure(); Serial.println("Sensor failure 0");}}
if ((SensorT1 !="") && (SensorT1F != "" )){Relay_control1();SensorT1F = "";SensorFail="0";} else{ if (SensorT1 !=""){ pcf8574.digitalWrite(r1, HIGH);SensorFail="1";failure(); Serial.println("Sensor failure 1");}}
if ((SensorT3 !="") && (SensorT3F != "" )){Relay_control3();SensorT3F = "";SensorFail="0";} else{ if (SensorT3 !=""){ pcf8574.digitalWrite(r3, HIGH);SensorFail="3";failure(); Serial.println("Sensor failure 3");}}
if ((SensorT2 !="") && (SensorT2F != "" )){Relay_control2();SensorT2F = "";SensorFail="0";} else{ if (SensorT2 !=""){ pcf8574.digitalWrite(r2, HIGH);SensorFail="2";failure(); Serial.println("Sensor failure 2");}}
if ((SensorT4 !="") && (SensorT4F != "" )){Relay_control4();SensorT4F = "";SensorFail="0";} else{ if (SensorT4 !=""){ pcf8574.digitalWrite(r4, HIGH);SensorFail="4";failure(); Serial.println("Sensor failure 4");}}
if ((SensorT5 !="") && (SensorT5F != "" )){Relay_control5();SensorT5F = "";SensorFail="0";} else{ if (SensorT5 !=""){ pcf8574.digitalWrite(r5, HIGH);SensorFail="5";failure(); Serial.println("Sensor failure 5");}}
if ((SensorT6 !="") && (SensorT6F != "" )){Relay_control6();SensorT6F = "";SensorFail="0";} else{ if (SensorT6 !=""){ pcf8574.digitalWrite(r6, HIGH);SensorFail="6";failure(); Serial.println("Sensor failure 6");}}
if ((SensorT7 !="") && (SensorT7F != "" )){Relay_control7();SensorT7F = "";SensorFail="0";} else{ if (SensorT7 !=""){ pcf8574.digitalWrite(r7, HIGH);SensorFail="7";failure(); Serial.println("Sensor failure 7");}}
if ((SensorT8 !="") && (SensorT8F != "" )){Relay_control8();SensorT8F = "";SensorFail="0";} else{ if (SensorT8 !=""){ pcf8574.digitalWrite(r8, HIGH);SensorFail="8";failure(); Serial.println("Sensor failure 8");}}
  
  
   
  }
long now1 = millis();
  if (now1 - lastMsg1 > 60000) {
    lastMsg1 = now1;
val0=preset; 
val1=preset1; 
val2=preset2; 
val3=preset3; 
val4=preset4; 
val5=preset5; 
val6=preset6; 
val7=preset7; 
val8=preset8; 
val9=heat;
val10=fan;

int val0a;
int val1a;
int val2a;
int val3a;
int val4a;
int val5a;
int val6a;
int val7a;
int val8a;
int heata;
int fana;
int S_Ta;
int E_Ta;
int DiFFa;
val0a = EEPROM.read(0); Serial.println(val0a);
val1a = EEPROM.read(1);Serial.println(val1a);
val2a = EEPROM.read(2) ;Serial.println(val2a);
val3a = EEPROM.read(3);Serial.println(val3a);
val4a = EEPROM.read(4);Serial.println(val4a);
val5a = EEPROM.read(5);Serial.println(val5a);
val6a = EEPROM.read(6);Serial.println(val6a);
val7a = EEPROM.read(7);Serial.println(val7a);
val8a = EEPROM.read(8);Serial.println(val8a);
heata = EEPROM.read(9);Serial.println(heata);
fana = EEPROM.read(10);Serial.println(fana);
S_Ta = EEPROM.read(12);Serial.println(S_Ta);
E_Ta = EEPROM.read(13);Serial.println(E_Ta);
DiFFa = EEPROM.read(14);Serial.println(DiFFa);
 Serial.println("_________________________________"); 
   
 if (val0a == val0){Serial.println("unchanged");}else{EEPROM.write(0, val0);Serial.println("wrote eeprom 0 ");Serial.println(val0);}
 if (val1a == val1){Serial.println("unchanged");}else{EEPROM.write(1, val1);Serial.println("wrote eeprom 1 ");Serial.println(val1); }
 if (val2a == val2){Serial.println("unchanged");}else{ EEPROM.write(2, val2);Serial.println("wrote eeprom 2 ");Serial.println(val2); }
 if (val3a == val3){Serial.println("unchanged");}else{ EEPROM.write(3, val3);Serial.println("wrote eeprom 3 ");Serial.println(val3);} 
 if (val4a == val4){Serial.println("unchanged");}else{ EEPROM.write(4, val4);Serial.println("wrote eeprom 4 ");Serial.println(val4); }
 if (val5a == val5){Serial.println("unchanged");}else{ EEPROM.write(5, val5);Serial.println("wrote eeprom 5 "); Serial.println(val5);}
 if (val6a == val6){Serial.println("unchanged");}else{ EEPROM.write(6, val6);Serial.println("wrote eeprom 6 ");Serial.println(val6); }
 if (val7a == val7){Serial.println("unchanged");}else{ EEPROM.write(7, val7);Serial.println("wrote eeprom 7 ");Serial.println(val7);} 
 if (val8a == val8){Serial.println("unchanged");}else{  EEPROM.write(8, val8); Serial.println("wrote eeprom 8 ");Serial.println(val8); } 
 if (heata == val9){Serial.println("unchanged");}else{ EEPROM.write(9, val9);Serial.println("wrote eeprom 9 ");Serial.println(heat);} 
 if (fana == val10){Serial.println("unchanged");}else{  EEPROM.write(10, val10); Serial.println("wrote eeprom 10 ");Serial.println(fan); } 
 if (S_Ta == S_T){Serial.println("unchanged");}else{  EEPROM.write(12, S_T); Serial.println("wrote eeprom 12 ");Serial.println(S_T); } 
 if (E_Ta == E_T){Serial.println("unchanged");}else{ EEPROM.write(13, E_T); Serial.println("wrote eeprom 13 ");Serial.println(E_T); } 
 if (DiFFa == DiFF){Serial.println("unchanged");}else{ EEPROM.write(14, DiFF); Serial.println("wrote eeprom 14 ");Serial.println(DiFF); } 

 delay(10);
   EEPROM.commit();
 Serial.println("_________________________________");
  
  } 
}

void failure() {
   if (SensorFail !="0") {
    
    String raw_CMD ;
    raw_CMD= SensorFail;
     Serial.println(raw_CMD);
    if (client.connected()){
    client.publish("/switch/failure/thermo", (char*) raw_CMD.c_str()); 
     Serial.println("Failure Published ok");
    }
  }
}

void Relay_control(){
  int Diff_enable;
  Diff_enable = preset;
  if (E_Diff ==1) {Diff_enable= Diff_enable-DiFF;}
  if ((Diff_enable) < (CurrentT-0.75)) {  // 0.75 is the thermostat sensitivity .75 c above and below prest for a variation of 1.5 c
    
    
     if (fan == 1){ pcf8574.digitalWrite(r6, LOW);}else{ pcf8574.digitalWrite(r6, HIGH);}    
     if (heat == 0){ pcf8574.digitalWrite(r8, HIGH);pcf8574.digitalWrite(r7, HIGH);}
     if (heat ==1){ pcf8574.digitalWrite(r8, LOW);pcf8574.digitalWrite(r6, LOW);pcf8574.digitalWrite(r7, HIGH);}  
     if (heat ==2){ pcf8574.digitalWrite(r7, HIGH);}
     Serial.println("relay0 off ");
                    }
  
  if ((Diff_enable) > (CurrentT+0.75)) {
    
      if (heat ==2){ pcf8574.digitalWrite(r7, LOW); pcf8574.digitalWrite(r6, LOW);}
      if (heat ==1){ pcf8574.digitalWrite(r8, HIGH); pcf8574.digitalWrite(r6, HIGH);pcf8574.digitalWrite(r7, HIGH);}
      if (heat ==0){ pcf8574.digitalWrite(r8, HIGH);pcf8574.digitalWrite(r7, HIGH);pcf8574.digitalWrite(r6, HIGH);}
      if (fan == 1){ pcf8574.digitalWrite(r6, LOW);}
   Serial.println("relay0 on");
  }
}
void Relay_control1(){
  if ((preset1) < (CurrentT1-0.75)){ pcf8574.digitalWrite(r1, HIGH);Serial.print("relay1 off ");}
  if ((preset1) > (CurrentT1+0.75)) { pcf8574.digitalWrite(r1, LOW);Serial.print("relay1 on ");}
  Serial.println("finished relay1");
}
void Relay_control2(){
  if ((preset2) < (CurrentT2-0.75)) { pcf8574.digitalWrite(r2, HIGH);Serial.println("relay2 off");}
  if ((preset2) > (CurrentT2+0.75)) { pcf8574.digitalWrite(r2, LOW);Serial.println("relay2 on");}
  Serial.println("finished relay2");
}
void Relay_control3(){
  if ((preset3) < (CurrentT3-0.75)) { pcf8574.digitalWrite(r3, HIGH);Serial.println("relay3 off");}
  if ((preset3) > (CurrentT3+0.75)) { pcf8574.digitalWrite(r3, LOW);Serial.println("relay3 on");}
  Serial.println("finished relay3");
}
void Relay_control4(){
  if ((preset4) < (CurrentT4-0.75)) { pcf8574.digitalWrite(r4, HIGH);Serial.println("relay4 off");}
  if ((preset4) > (CurrentT4+0.75)) { pcf8574.digitalWrite(r4, LOW);Serial.println("relay4 on");}
}
void Relay_control5(){
  if ((preset5) < (CurrentT5-0.75)) { pcf8574.digitalWrite(r5, HIGH);Serial.println("relay5 off");}
  if ((preset5) > (CurrentT5+0.75)) { pcf8574.digitalWrite(r5, LOW);Serial.println("relay5 on");}
}
void Relay_control6(){
  if ((preset6) < (CurrentT6-0.75)) { pcf8574.digitalWrite(r6, HIGH);Serial.println("relay6 off");}
  if ((preset6) > (CurrentT6+0.75)) { pcf8574.digitalWrite(r6, LOW);Serial.println("relay6 on");}
}
void Relay_control7(){
  if ((preset7) < (CurrentT7-0.75)) { pcf8574.digitalWrite(r7, HIGH);Serial.println("relay7 off");}
  if ((preset7) > (CurrentT7+0.75)) { pcf8574.digitalWrite(r7, LOW);Serial.println("relay7 on");}
}
void Relay_control8(){
  if ((preset8) < (CurrentT8-0.75)) { pcf8574.digitalWrite(r8, HIGH);Serial.println("relay8 off");}
  if ((preset8) > (CurrentT8+0.75)) { pcf8574.digitalWrite(r8, LOW);Serial.println("relay8 on");}
}




 void Sensor() {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  

//Loop through all DS1820

while(ds.search(addr))
{ 
  //Serial.print("ROM =");

//Topic is built from a static String plus the ID of the DS18B20
  String romcode = "/temp/";
  String romID = "/ID/";
  for( i = 0; i < 8; i++) {
   // Serial.write(' ');
   // Serial.print(addr[i], HEX);
    
    romcode = romcode + String(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
 // Serial.println();
 
// the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
     // Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
     // Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
     // Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  //Serial.print("  Data = ");
  //Serial.print(present, HEX);
  //Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  //  Serial.print(data[i], HEX);
   // Serial.print(" ");
  }
  //Serial.print(" CRC=");
  //Serial.print(OneWire::crc8(data, 8), HEX);
  //Serial.println();

  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }

//convert RAW Temperature to String
String raw_temp = String(raw, DEC);
float tempc=(raw_temp.toFloat());
tempc=(tempc/16);
raw_temp=tempc;

  if (client.connected()){
    //Serial.print("Sending payload: ");
   // Serial.println(raw_temp);
  }
  if (20 > cntID){
    if (client.publish((char*) romID.c_str(), (char*) romcode.c_str())) {
      //Serial.println((char*) romID.c_str());
      //Serial.println("Publish ok");
      cntID =(cntID +1 );
       //Serial.println(cntID);
    }
    
  }

  
if (SensorT ==  (char*) romcode.c_str() ) {CurrentT  =raw_temp.toFloat(); SensorTF =((char*) romcode.c_str() );}//Serial.println(CurrentT);}
if (SensorT1 == (char*) romcode.c_str() ) {CurrentT1 =raw_temp.toFloat();SensorT1F =((char*) romcode.c_str() );}///Serial.println(CurrentT1);}
if (SensorT2 == (char*) romcode.c_str() ) {CurrentT2 =raw_temp.toFloat();SensorT2F =((char*) romcode.c_str() );}//Serial.print("raM_temp ");Serial.println(((char*) romcode.c_str() ));}
if (SensorT3 == (char*) romcode.c_str() ) {CurrentT3 =raw_temp.toFloat();SensorT3F =((char*) romcode.c_str() );}///Serial.print("raw_temp ");Serial.println(raw_temp);}
if (SensorT4 == (char*) romcode.c_str() ) {CurrentT4 =raw_temp.toFloat();SensorT4F =((char*) romcode.c_str() );}//Serial.println(CurrentT4);}
if (SensorT5 == (char*) romcode.c_str() ) {CurrentT5 =raw_temp.toFloat();SensorT5F =((char*) romcode.c_str() );}//Serial.println(CurrentT5);}
if (SensorT6 == (char*) romcode.c_str() ) {CurrentT6 =raw_temp.toFloat();SensorT6F =((char*) romcode.c_str() );}//Serial.println(CurrentT6);}
if (SensorT7 == (char*) romcode.c_str() ) {CurrentT7 =raw_temp.toFloat();SensorT7F =((char*) romcode.c_str() );}//Serial.println(CurrentT7);}
if (SensorT8 == (char*) romcode.c_str() ) {CurrentT8 =raw_temp.toFloat();SensorT8F =((char*) romcode.c_str() );}//Serial.println(CurrentT8);} 
  
    if (client.publish((char*) romcode.c_str(), (char*) raw_temp.c_str())) {
      Serial.println((char*) romcode.c_str());
      Serial.println("Publish ok");
    }
    else {
      Serial.println("Publish failed");
    }

  //delay(5000);
}
//End of the OneWire-Devices, reset Loop
//Serial.println("End of Onewire Bus");
ds.reset_search();
return;
}


