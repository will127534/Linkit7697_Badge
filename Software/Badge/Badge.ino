
#include <LWiFi.h>
#include <WiFiUdp.h>
#include <LFlash.h>
#include <hal_wdt.h>
#include "Control.hpp"
#include "Clock.hpp"
#include "Snake.hpp"
#include "Tetris.hpp"
#include "Factory.hpp"

#include "icon.c"

const unsigned int MAX_WIFI_INPUT_BUF = 64;
int g_appID = 0;         ///< The "App" user selected, ranges from 0 to 3
const int APP_COUNT = 4; ///< Number of apps in menu

void setup()
{

  initButtons();

  // If `A-B-X-Y` are pressed (active LOW) at the same time,
  // enter Factory Test mode (and never leaves)
  delay(1000);
  if (!digitalRead(A_BUTTON) &&
      !digitalRead(B_BUTTON) &&
      !digitalRead(X_BUTTON) &&
      !digitalRead(Y_BUTTON))
  {
    jumpToUnitTest();
  }

  // Normal boot flow
  Serial.begin(115200);
  Serial.setTimeout(60000);
  Serial.println("Normal bootup...");

  // Initialize LED Matrix
  Serial.println("Init LED Matrix...");
  if (!matrix.begin())
  {
    Serial.println("LED Matrix not found.");
    while (1)
    {
      delay(100);
    }
  }
  matrix.setRotation(1);
  matrix.setTextWrap(false);
  Serial.println("LED Matrix found.");

  // Boot Animation
  Serial.println("Boot Animation...");
  animation(Bike, 16, 0, 0, 18, 16, 100);
  delay(1000);
  drawframe(Trophy, 0, 0, 18, 16);
  delay(3000);
  Serial.println("Boot Animation ended.");

  // Try connecting to Wi-Fi Network.
  const bool connected = connectToWiFi();

  // Update our clock time from network
  if (connected)
  {
    syncClock();
  }
  startClock();

  showMenu();
}

void loop()
{
  if (digitalRead(USR_BUTTON))
  {
    showMenu();
  }
  switch (g_appID)
  {
  case 1:
    Snake();
    showMenu();
    break;
  case 2:
    tetris();
    showMenu();
    break;
  case 3:
    UpdateWifiSetting();
    showMenu();
    break;
  case 0:
    if (getAndClearUpdateFlag())
    {
      const unsigned long epoch = getEpoch();
      displayTime(epoch);
    }
  }
}

/// \returns true if connected to Wi-Fi AP
/// \returns false if failed to connect or there is no SSID/PASSWD setting
bool connectToWiFi()
{
  LFlash.begin();
  // Wi-Fi SSID and Password should be less than 64 bytes or less.
  char ssid[MAX_WIFI_INPUT_BUF] = {0};
  char pass[MAX_WIFI_INPUT_BUF] = {0};
  uint32_t bufLen = MAX_WIFI_INPUT_BUF;

  printMacAddress();
  LFlash.read("USER", "SSID", (uint8_t *)ssid, &bufLen);
  LFlash.read("USER", "PASSWD", (uint8_t *)pass, &bufLen);
  Serial.print("Stored SSID:");
  Serial.println(ssid);
  Serial.print("Stored PASSWD:");
  Serial.println(pass);

  if (ssid[0] == 0 || pass[0] == 0)
  {
    // Empty SSID and Password => Update Wifi Setting
    Serial.println("Wifi Setting not found. Please select Wi-Fi setting from menu.");
    drawframe(Wifi_not_found, 0, 0, 18, 16);
    delay(2000);

    return false;
  }

  // Wi-Fi animation
  Serial.println("Wifi Setting found...");
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  for (int i = 0; i < 3; i++)
  {
    animation(Wifi_search, 4, 0, 0, 18, 16, 50);
  }

  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  const int status = WiFi.begin(ssid, pass);

  // If fail to connect, show failure icon
  if (status != WL_CONNECTED)
  {
    Serial.println("Unable to Connect to Wifi AP.");
    drawframe(Wifi_not_found, 0, 0, 18, 16);
    delay(2000);
    return false;
  }

  // connected, show animation.
  Serial.println("Wi-Fi connected.");
  printWifiStatus();
  for (int i = 0; i < 3; i++)
  {
    animation(Wifi_search, 4, 0, 0, 18, 16, 50);
  }
  return true;
}

void showMenu()
{
  bool exit = 0;
  uint8_t counter = 0;
  while (!exit)
  {
    const int button = scanButtonUp();

    if (button == LEFT_BUTTON)
    {
      g_appID--;
      if (g_appID < 0)
      {
        g_appID += APP_COUNT;
      }
    }
    if (button == RIGHT_BUTTON)
    {
      g_appID++;
      if (g_appID > APP_COUNT - 1)
      {
        g_appID -= APP_COUNT;
      }
    }
    if (button == CENTER_BUTTON)
    {
      exit = 1;
    }
    int light = 100 * analogRead(A0) / 512.0;
    counter++;
    matrix.setFrame((counter) % 2);
    matrix.clear();
    matrix.drawTriangle(17, 7, 15, 5, 15, 9, light);
    matrix.drawTriangle(0, 7, 2, 5, 2, 9, light);
    drawframe(Menu + 100 * g_appID, 4, 3, 10, 10);

    matrix.displayFrame(counter % 2);
    delay(50);
  }
}

//#################### Reset ####################
void reset()
{
  Serial.println("Restarting");
  hal_wdt_config_t wdt_config;
  wdt_config.mode = HAL_WDT_MODE_RESET;
  wdt_config.seconds = 0;
  hal_wdt_init(&wdt_config); //set WDT as t0 mode.
  hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
  while (1)
  {
    Serial.println("Restarting");
    hal_wdt_software_reset();
    delay(100);
  }
}

//#################### LED Matrix Display ####################
void animation(const unsigned char *arr, int frame_count, int x0, int y0, int x_len, int y_len, int frame_speed)
{
  for (int frame = 0; frame < frame_count; frame++)
  {
    matrix.displayFrame(frame % 2);
    matrix.setFrame((frame + 1) % 2);
    drawframe(arr + x_len * y_len * frame, x0, y0, x_len, y_len);
    delay(frame_speed);
  }
  matrix.displayFrame((frame_count) % 2);
}

void drawframe(const unsigned char *arr, int x0, int y0, int x_len, int y_len)
{
  float light = analogRead(A0) / 512.0;
  //matrix.clear();
  //matrix.drawBitmap(0, 0, Icon1, 18, 18, 120);
  for (int x = 0; x < x_len; x++)
  {
    for (int y = 0; y < y_len; y++)
    {
      matrix.drawPixel(x + x0, y + y0, arr[y * x_len + x] * light);
    }
  }
}

void displayTime(unsigned long epoch)
{
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

  matrix.setFrame(epoch % 2);
  matrix.clear();

  float light = analogRead(A0) / 512.0;
  Serial.print("LED light value=");
  Serial.println(light);

  matrix.drawPixel(8, 4, epoch % 2 * 127 * light);
  matrix.drawPixel(8, 5, epoch % 2 * 127 * light);
  drawframe(digits + date.tm_hour / 10 * 15, 0, 2, 3, 5);
  drawframe(digits + date.tm_hour % 10 * 15, 4, 2, 3, 5);
  drawframe(digits + date.tm_min / 10 * 15, 10, 2, 3, 5);
  drawframe(digits + date.tm_min % 10 * 15, 14, 2, 3, 5);

  matrix.drawPixel(8, 11, 127 * light);
  matrix.drawPixel(9, 11, 127 * light);

  drawframe(digits + date.tm_mon / 10 * 15, 0, 9, 3, 5);
  drawframe(digits + date.tm_mon % 10 * 15, 4, 9, 3, 5);
  drawframe(digits + date.tm_mday / 10 * 15, 11, 9, 3, 5);
  drawframe(digits + date.tm_mday % 10 * 15, 15, 9, 3, 5);

  matrix.displayFrame(epoch % 2);
}

//#################### WIFI ####################
void printWifiStatus()
{
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

void UpdateWifiSetting()
{
  Serial.println("Please enter Wifi SSID:");
  drawframe(Wifi_input, 0, 0, 18, 16);

  char SSID[MAX_WIFI_INPUT_BUF] = {0};
  char PASSWD[MAX_WIFI_INPUT_BUF] = {0};

  Serial.readBytesUntil('\n', SSID, MAX_WIFI_INPUT_BUF);

  Serial.println(SSID);

  Serial.println("Please enter Wifi password:");

  Serial.readBytesUntil('\n', PASSWD, MAX_WIFI_INPUT_BUF);

  for (int i = 0; i < MAX_WIFI_INPUT_BUF; i++)
  {
    if (SSID[i] == '\r' || SSID[i] == '\n')
    {
      SSID[i] = 0;
    }
    if (PASSWD[i] == '\r' || PASSWD[i] == '\n')
    {
      PASSWD[i] = 0;
    }
  }
  Serial.println(PASSWD);

  LFlashStatus s1 = LFlash.write("USER", "SSID", LFLASH_STRING_DATA, (uint8_t *)SSID, MAX_WIFI_INPUT_BUF);
  LFlashStatus s2 = LFlash.write("USER", "PASSWD", LFLASH_STRING_DATA, (uint8_t *)PASSWD, MAX_WIFI_INPUT_BUF);

  if (s1 == 0 && s2 == 0)
  {
    Serial.println("Write Success");
  }
  else
  {
    Serial.println("Write Error");
  }
  reset();
}
