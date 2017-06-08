#include <Wire.h>
#include "SparkFunLIS3DH.h"
#include "icon.c"
#include <hal_gpt.h>
#include <hal_wdt.h>
#include <LWiFi.h>
#include <WiFiUdp.h>
#include <LFlash.h>
#include <LBLE.h>
#include <LBLECentral.h>
#include <time.h> 
#include "Snake.hpp"
#include "Tetris.hpp"
#include "Control.hpp"


LIS3DH myIMU(I2C_MODE,0x18); //Default constructor is I2C, addr 0x18.

int status = WL_IDLE_STATUS;

char server[] = "download.labs.mediatek.com";   // http://download.labs.mediatek.com/linkit_7697_ascii.txt
WiFiClient client;

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
char *NTP_server = "time-a.nist.gov";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

unsigned long epoch = 0;
bool RTC_flag = 0;

int mode = 0;
#define MODES 4

void setup() {
  for(int i=0; i<9; i++){
    pinMode(buttons[i], INPUT_PULLUP);
  }

  pinMode(6, INPUT);

  delay(1000);

  if(!digitalRead(buttons[5]) && !digitalRead(buttons[6]) && !digitalRead(buttons[7]) && !digitalRead(buttons[8])){
    jumptounittest();
  }
  

  Serial.begin(115200);
  Serial.setTimeout(60000);
  Serial.println("ISSI manual animation test");
  if (! matrix.begin()) {
    Serial.println("IS31 not found");
    while (1);
  }
  Serial.println("IS31 Found!");
  pinMode(3,OUTPUT);
  digitalWrite(3,HIGH);
  matrix.setRotation(1);
  matrix.setTextWrap(false);

  animation(Bike,16,0,0,18,16,100);
   delay(1000);
  drawframe(trophy,0,0,18,16);
  delay(3000);
  printMacAddress();

  
  
  LFlash.begin();
  char ssid[100] = {0};
  char pass[100] = {0};
  uint32_t temp = 100;

  LFlash.read("USER","SSID",  (uint8_t *)ssid,&temp);
  LFlash.read("USER","PASSWD",  (uint8_t *)pass,&temp);
  
  Serial.print("SSID:");Serial.println(ssid);
  Serial.print("PASSWD:");Serial.println(pass);

  if(ssid[0] == 0 || pass[0] == 0){ //Empty SSID and Password => Update Wifi Setting
        Serial.println("Wifi Setting not found, Please reEnter Wifi Setting");
        drawframe(Wifi_not_found,0,0,18,16);
        delay(2000);
  }
  else{
    
    for(int i=0;i<5;i++){
        animation(Wifi_search,4,0,0,18,16,50);  
    }
    
    int retry = 0;

    while (status != WL_CONNECTED) {
      retry++;
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(ssid);
      // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
      status = WiFi.begin(ssid, pass);
      for(int i=0;i<5;i++){
        animation(Wifi_search,4,0,0,18,16,50);  
      }
      
      if(retry>1){
          Serial.println("Unable to Connect to Wifi AP, Please reEnter Wifi Setting");
          drawframe(Wifi_not_found,0,0,18,16);
          delay(2000);
          break;
      }

    }
  }

  if(status == WL_CONNECTED){
    Serial.println("Connected to wifi");

    printWifiStatus();
    epoch = getUTC();
    Serial.println(epoch);
    epoch = epoch + 8*3600; //UTC+8
    
  }

  hal_gpt_running_status_t running_status;
  hal_gpt_status_t         ret_status;

  //Get the running status to check if this port is in use or not.
  if (HAL_GPT_STATUS_OK != hal_gpt_get_running_status(HAL_GPT_1,&running_status)) {//Handle the error, if the timer is running.
     //error handle
  }
  //Set the GPT base environment.
  if(HAL_GPT_STATUS_OK != hal_gpt_init(HAL_GPT_1)) {
     //error handle
  }
  hal_gpt_register_callback(HAL_GPT_1, RTC_Tick, &epoch); //Register a user callback.
  hal_gpt_start_timer_ms(HAL_GPT_1, 1000, HAL_GPT_TIMER_TYPE_REPEAT);

  changeMode();
}


void loop() {
  int button = scanning_button();
  //UP_BUTTON,DOWN_BUTTON,LEFT_BUTTON,RIGHT_BUTTON,CENTER_BUTTON,A_BUTTON,B_BUTTON,X_BUTTON,Y_BUTTON};
  if(digitalRead(6)){
      changeMode();
  }
  switch (mode) {
      case 1:
        Snake();
        changeMode();
        break;
      case 2:
        tetris();
        changeMode();
        // do something
        break;
      case 3:
        UpdateWifiSetting();
        changeMode();
        // do something
        break;
      case 0:
        if(RTC_flag){
          NTPparse(epoch);
          Serial.println(analogRead(A0));
          displayTime(epoch);
          RTC_flag = 0;
        }
  }


}


void changeMode(){
  bool exit = 0;
  uint8_t counter = 0;
  int button = -1;
  while(!exit){
    button = scanning_button_block();
    if(button == 2){
        mode--;
        if(mode<0){
            mode += MODES;
        }
    }
    if(button == 3){
        mode++;
        if(mode>MODES-1){
            mode -= MODES;
        }
    }
    if(button == 4){
        exit = 1;
    }
    int light = 100*analogRead(A0)/512.0;
    counter++;
    matrix.setFrame((counter)%2);
    matrix.clear();
    matrix.drawTriangle(17,7,15,5,15,9,light);
    matrix.drawTriangle(0,7,2,5,2,9,light);
    drawframe(Menu+100*mode,4,3,10,10);
    /*
    if(button == 2 ){//left
        matrix.fillTriangle(0,7,2,5,2,9,100);
    }

    if(button == 3){
        matrix.fillTriangle(17,7,15,5,15,9,100);
    }
    */
    matrix.displayFrame(counter%2);
    delay(50);
  }

}

//#################### ISR ####################
void RTC_Tick(void *epoch_addr){
    unsigned long *epoch  = (unsigned long *)epoch_addr;
    *epoch += 1;
    RTC_flag = 1; 
}

void reset(){
  Serial.println("Restarting");
  hal_wdt_config_t  wdt_config;
  wdt_config.mode = HAL_WDT_MODE_RESET;
  wdt_config.seconds = 0;
  hal_wdt_init(&wdt_config); //set WDT as t0 mode.
  hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
  while(1){
    Serial.println("Restarting");
    hal_wdt_software_reset();
    delay(100);
  }
}

//#################### Screen ####################
void animation(const unsigned char* arr,int frame_count,int x0, int y0, int x_len , int y_len,int frame_speed){
  for(int frame=0;frame<frame_count;frame++){
    matrix.displayFrame(frame%2);
    matrix.setFrame((frame+1)%2);
    drawframe(arr+x_len*y_len*frame,x0,  y0,  x_len ,  y_len);
    delay(frame_speed);
  }
  matrix.displayFrame((frame_count)%2);
}

void drawframe(const unsigned char* arr,int x0, int y0, int x_len , int y_len){
    float light = analogRead(A0)/512.0;
    //matrix.clear();
    //matrix.drawBitmap(0, 0, Icon1, 18, 18, 120);
    for(int x=0;x<x_len;x++){
      for(int y=0;y<y_len;y++){
        matrix.drawPixel(x+x0,y+y0,arr[y*x_len+x]*light);
      }
    }
}

void displayTime(unsigned long epoch){
    // print the hour, minute
    struct tm date = NTPparse(epoch);
    
    Serial.print(date.tm_year);
    Serial.print("/");
    Serial.print(date.tm_mon);
    Serial.print("/");
    Serial.print(date.tm_mday);
    Serial.print(" ");
    Serial.print(date.tm_hour);
    Serial.print(":");
    Serial.print(date.tm_min);
    Serial.print(":");
    Serial.println(date.tm_sec);
    
    matrix.setFrame(epoch%2);
    matrix.clear();

    float light = analogRead(A0)/512.0;

    matrix.drawPixel(8,4,epoch%2*127*light);
    matrix.drawPixel(8,5,epoch%2*127*light);
    drawframe(digits+date.tm_hour/10*15,0,2,3,5);
    drawframe(digits+date.tm_hour%10*15,4,2,3,5);
    drawframe(digits+date.tm_min/10*15,10,2,3,5);
    drawframe(digits+date.tm_min%10*15,14,2,3,5);

    matrix.drawPixel(8,11,127*light);
    matrix.drawPixel(9,11,127*light);

    drawframe(digits+date.tm_mon/10*15,0,9,3,5);
    drawframe(digits+date.tm_mon%10*15,4,9,3,5);
    drawframe(digits+date.tm_mday/10*15,11,9,3,5);
    drawframe(digits+date.tm_mday%10*15,15,9,3,5);

    matrix.displayFrame(epoch%2);
}

//#################### BLE ####################
void printDeviceInfo(int i) {
  Serial.print(i);
  Serial.print("\t");
  Serial.print(LBLECentral.getAddress(i));
  Serial.print("\t");
  Serial.print(LBLECentral.getAdvertisementFlag(i), HEX);
  Serial.print("\t");
  Serial.print(LBLECentral.getRSSI(i));
  Serial.print("\t");
  const String name = LBLECentral.getName(i);
  Serial.print(name);
  if(name.length() == 0)
  {
    Serial.print("(Unknown)");
  }
  Serial.print(" by ");
  const String manu = LBLECentral.getManufacturer(i);
  Serial.print(manu);
  Serial.print(", service: ");
  if (!LBLECentral.getServiceUuid(i).isEmpty()) {
    Serial.print(LBLECentral.getServiceUuid(i));
  } else {
    Serial.print("(no service info)");
  }

  if (LBLECentral.isIBeacon(i)) {
    LBLEUuid uuid;
    uint16_t major = 0, minor = 0;
    uint8_t txPower = 0;
    LBLECentral.getIBeaconInfo(i, uuid, major, minor, txPower);

    Serial.print(" ");
    Serial.print("iBeacon->");
    Serial.print("  UUID: ");
    Serial.print(uuid);
    Serial.print("\tMajor:");
    Serial.print(major);
    Serial.print("\tMinor:");
    Serial.print(minor);
    Serial.print("\ttxPower:");
    Serial.print(txPower);
  }

  Serial.println();
}

//#################### WIFI ####################
void printMacAddress() {
  // the MAC address of your Wifi shield
  byte mac[6];

  // print your MAC address:
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
}

void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }
  
  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
  }
}

void printEncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      Serial.println("WEP");
      break;
    case ENC_TYPE_TKIP:
      Serial.println("WPA");
      break;
    case ENC_TYPE_CCMP:
      Serial.println("WPA2");
      break;
    case ENC_TYPE_NONE:
      Serial.println("None");
      break;
    case ENC_TYPE_AUTO:
      Serial.println("Auto");
      break;
    default :
      Serial.println("WPA-PSK/WPA2-PSK");
      break;
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void UpdateWifiSetting(){

  Serial.println("Please enter Wifi SSID:");
  drawframe(Wifi_input,0,0,18,16);
  bool Reading = 1;
  
  char SSID[100] = {0};
  char PASSWD[100] = {0};


  Serial.readBytesUntil('\n', SSID, 100); 
  
  Serial.println(SSID);

  Serial.println("Please enter Wifi password:");

  Serial.readBytesUntil('\n', PASSWD, 100); 

  for(int i=0; i<100; i++){
      if(SSID[i]=='\r' || SSID[i]=='\n' ){
          SSID[i] = 0;
      }
      if(PASSWD[i]=='\r' || PASSWD[i]=='\n' ){
          PASSWD[i] = 0;
      }
  }
  Serial.println(PASSWD);

  LFlashStatus s1 = LFlash.write("USER","SSID",LFLASH_STRING_DATA,(uint8_t*)SSID,100);
  LFlashStatus s2 = LFlash.write("USER","PASSWD",LFLASH_STRING_DATA,(uint8_t*)PASSWD,100);
  
  if(s1 == 0 && s2 ==0){
      Serial.println("Write Success");
  }
  else{
      Serial.println("Write Error");
  }
  reset();
}
//#################### UTC ####################
unsigned long getUTC(){
  unsigned long epoch_return = 0;
  Serial.println("\nStarting connection to server...");
  Udp.begin(localPort);
  sendNTPpacket(NTP_server); // send an NTP packet to a time server
  while(!Udp.parsePacket()){
      
  }
  if (Udp.parsePacket()) {
    Serial.println("packet received");
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = ");
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900;// - seventyYears;
    // print Unix time:
    Serial.println(epoch);
    epoch_return = epoch;

    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if (((epoch % 3600) / 60) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second
  }
  Serial.println(epoch);
  return epoch_return;
}

#define SECONDS_FROM_1900_TO_2000 3155673600UL;
const uint8_t daysIntheMonth [] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

struct tm  NTPparse(uint32_t t){
    t -= SECONDS_FROM_1900_TO_2000;    // bring to 2000 timestamp from 1970
    struct tm  NTPtime = {0};
    NTPtime.tm_sec = t % 60;
    t /= 60;
    NTPtime.tm_min = t % 60;
    t /= 60;
    NTPtime.tm_hour = t % 24;

    uint32_t days = t / 24;
    uint8_t leap;
    int  yOff = 0;
    int  m = 0;
    int  d = 0;

    for (yOff = 0; ; ++yOff) {
        leap = yOff % 4 == 0;
        if (days < 365 + leap)
            break;
        days -= 365 + leap;
    }
    for (m = 1; ; ++m) {
        uint8_t daysPerMonth = daysIntheMonth[ m - 1];
        if (leap && m == 2)
            ++daysPerMonth;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    d = days + 1;

    NTPtime.tm_year = yOff;
    NTPtime.tm_mday = d;
    NTPtime.tm_mon = m;
    /*
    Serial.print(yOff);
    Serial.print("/");
    Serial.print(m);
    Serial.print("/");
    Serial.print(d);
    Serial.print(" ");
    Serial.print(NTPtime.tm_hour);
    Serial.print(":");
    Serial.print(NTPtime.tm_min);
    Serial.print(":");
    Serial.println(NTPtime.tm_sec);
    */
   return NTPtime;
}

unsigned long sendNTPpacket(char* host) {
  //Serial.println("1");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  //Serial.println("2");
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  //Serial.println("3");

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(host, 123); //NTP requests are to port 123
  //Serial.println("4");
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  //Serial.println("5");
  Udp.endPacket();
  //Serial.println("6");

  return 0;
}

//#################### Unit test ####################
void jumptounittest(){
  Serial.begin(115200);
  Serial.println("Unit test start");
  if (! matrix.begin()) {
    Serial.println("IS31 not found");
    while (1);
  }
  matrix.setRotation(1);
  Serial.println("IS31 Found!");
  for(int x =0;x<18;x++){
    for(int y=0;y<16;y++){
      matrix.drawPixel(x,y,255);
      delay(5);
    }
  }
  delay(3000);
  matrix.clear();
  int status = myIMU.begin();
  if(status == 1){
    Serial.println("LIS3DH not found");
    while (1){
      matrix.setTextWrap(false);
        for (int8_t x=18; x>=-8*12; x--) {
          matrix.clear();
          matrix.setCursor(x,0);
          matrix.print("LIS3DH not found");
          delay(50);
        }
    }
  }
 else{
  Serial.println("LIS3DH found");
 }

    if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi  not present");
    // don't continue:
    while (true);
  }
  printMacAddress();
  Serial.println("Scanning available networks...");
  listNetworks();

  // Initialize BLE subsystem
  Serial.println("BLE begin");
  LBLE.begin();
  while (!LBLE.ready()) {
    delay(10);
  }

  Serial.println("BLE ready, start scan (wait 5 seconds)");
  LBLECentral.scan();
  for(int i = 0; i < 5; ++i)
  {
    delay(1000);
    Serial.print(".");
  }

  // list advertisements found.
  Serial.print("Total ");
  Serial.print(LBLECentral.getPeripheralCount());
  Serial.println(" devices found:");
  Serial.println("idx\taddress\t\t\tflag\tRSSI");
  for (int i = 0; i < LBLECentral.getPeripheralCount(); ++i) {
    printDeviceInfo(i);
  }
  LBLECentral.stopScan();
  Serial.println("------scan stopped-------");


        matrix.setTextWrap(false);
        for (int8_t x=18; x>=-8*12; x--) {
          matrix.clear();
          matrix.setCursor(x,0);
          matrix.print("Button Test");
          delay(50);
        }
  while(1){
    int button_pushed = scanning_button();
    if(button_pushed != -1){
      matrix.clear();                                     
     if(button_pushed<=1){
       matrix.setCursor(4,4);
     }
     else{
       matrix.setCursor(6,4);
     }
      switch (button_pushed) {
        //byte buttons[] = {UP_BUTTON,DOWN_BUTTON,LEFT_BUTTON,RIGHT_BUTTON,CENTER_BUTTON,A_BUTTON,B_BUTTON,X_BUTTON,Y_BUTTON};
          case 0:
            matrix.print("UP");
            break;
          case 1:
            matrix.print("DO");
            break;
          case 2:
            matrix.print("L");
            break;
          case 3:
            matrix.print("R");
            break;
          case 4:
            matrix.print("C");
            break;
          case 5:
            matrix.print("A");
            break;
          case 6:
            matrix.print("B");
            break;
          case 7:
            matrix.print("X");
            break;
          case 8:
            matrix.print("Y");
            break;
      }
    }
  }
}

