//#################### Unit test ####################
#include <Arduino.h>
#include <LBLE.h>
#include <LWiFi.h>
#include <LBLECentral.h>
#include "Factory.hpp"
#include "Control.hpp"

//#################### BLE ####################
void printDeviceInfo(int i)
{
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
  if (name.length() == 0)
  {
    Serial.print("(Unknown)");
  }
  Serial.print(" by ");
  const String manu = LBLECentral.getManufacturer(i);
  Serial.print(manu);
  Serial.print(", service: ");
  if (!LBLECentral.getServiceUuid(i).isEmpty())
  {
    Serial.print(LBLECentral.getServiceUuid(i));
  }
  else
  {
    Serial.print("(no service info)");
  }

  if (LBLECentral.isIBeacon(i))
  {
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

//#################### Wi-Fi ####################
void printMacAddress()
{
  // the MAC address of your Wifi shield
  byte mac[6] = {0};

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

void printEncryptionType(int thisType)
{
  // read the encryption type and print out the name:
  switch (thisType)
  {
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
  default:
    Serial.println("WPA-PSK/WPA2-PSK");
    break;
  }
}

void listNetworks()
{
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1)
  {
    Serial.println("Couldn't get a wifi connection");
    while (true)
      ;
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++)
  {
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

//#################### Unit Test for Factory ####################
void jumpToUnitTest()
{
  Serial.begin(115200);
  Serial.println("Unit test start");
  if (!matrix.begin())
  {
    Serial.println("IS31 not found");
    while (1)
      ;
  }
  matrix.setRotation(1);
  Serial.println("IS31 Found!");
  for (int x = 0; x < 18; x++)
  {
    for (int y = 0; y < 16; y++)
    {
      matrix.drawPixel(x, y, 255);
      delay(5);
    }
  }
  delay(3000);
  matrix.clear();
  int status = myIMU.begin();
  if (status == 1)
  {
    Serial.println("LIS3DH not found");
    while (1)
    {
      matrix.setTextWrap(false);
      for (int8_t x = 18; x >= -8 * 12; x--)
      {
        matrix.clear();
        matrix.setCursor(x, 0);
        matrix.print("LIS3DH not found");
        delay(50);
      }
    }
  }
  else
  {
    Serial.println("LIS3DH found");
  }

  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi  not present");
    // don't continue:
    while (true)
      ;
  }
  printMacAddress();
  Serial.println("Scanning available networks...");
  listNetworks();

  // Initialize BLE subsystem
  Serial.println("BLE begin");
  LBLE.begin();
  while (!LBLE.ready())
  {
    delay(10);
  }

  Serial.println("BLE ready, start scan (wait 5 seconds)");
  LBLECentral.scan();
  for (int i = 0; i < 5; ++i)
  {
    delay(1000);
    Serial.print(".");
  }

  // list advertisements found.
  Serial.print("Total ");
  Serial.print(LBLECentral.getPeripheralCount());
  Serial.println(" devices found:");
  Serial.println("idx\taddress\t\t\tflag\tRSSI");
  for (int i = 0; i < LBLECentral.getPeripheralCount(); ++i)
  {
    printDeviceInfo(i);
  }
  LBLECentral.stopScan();
  Serial.println("------scan stopped-------");

  matrix.setTextWrap(false);
  for (int8_t x = 18; x >= -8 * 12; x--)
  {
    matrix.clear();
    matrix.setCursor(x, 0);
    matrix.print("Button Test");
    delay(50);
  }

  while (1)
  {
    const int button_pushed = scanButton();
    if (button_pushed == NO_BUTTON)
    {
      continue;
    }
    matrix.clear();
    matrix.setTextColor(100 * analogRead(A0) / 512.0);
    if (button_pushed == UP_BUTTON || button_pushed == DOWN_BUTTON)
    {
      matrix.setCursor(4, 4);
    }
    else
    {
      matrix.setCursor(6, 4);
    }

    switch (button_pushed)
    {
    case UP_BUTTON:
      matrix.print("UP");
      break;
    case DOWN_BUTTON:
      matrix.print("DO");
      break;
    case LEFT_BUTTON:
      matrix.print("L");
      break;
    case RIGHT_BUTTON:
      matrix.print("R");
      break;
    case CENTER_BUTTON:
      matrix.print("C");
      break;
    case A_BUTTON:
      matrix.print("A");
      break;
    case B_BUTTON:
      matrix.print("B");
      break;
    case X_BUTTON:
      matrix.print("X");
      break;
    case Y_BUTTON:
      matrix.print("Y");
      break;
    case USR_BUTTON:
      matrix.print("U");
      break;
    }


  }
}