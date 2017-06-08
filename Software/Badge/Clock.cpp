#include "Clock.hpp"
#include <hal_gpt.h>
#include <LWiFi.h>
#include <WiFiUdp.h>

// For RTC update time
static unsigned long g_epoch = 0;
static bool g_RTCFlag = 0;

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
static const char *NTP_server = "time.nist.gov";
static const int NTP_PACKET_SIZE = 48;           ///< NTP time stamp is in the first 48 bytes of the message
static byte packetBuffer[NTP_PACKET_SIZE] = {0}; ///< Buffer to hold incoming and outgoing NTP packets

unsigned long getEpoch()
{
    return g_epoch;
}

void RTC_Tick(void *epoch_addr)
{
    unsigned long *epoch = (unsigned long *)epoch_addr;
    *epoch += 1;
    g_RTCFlag = 1;
}

bool getAndClearUpdateFlag()
{
    if (g_RTCFlag)
    {
        g_RTCFlag = 0;
        return true;
    }
    return false;
}

void startClock()
{
    hal_gpt_running_status_t running_status;
    hal_gpt_status_t ret_status;

    //Get the running status to check if this port is in use or not.
    if (HAL_GPT_STATUS_OK != hal_gpt_get_running_status(HAL_GPT_1, &running_status))
    { //Handle the error, if the timer is running.
        //error handle
    }
    //Set the GPT base environment.
    if (HAL_GPT_STATUS_OK != hal_gpt_init(HAL_GPT_1))
    {
        //error handle
    }
    hal_gpt_register_callback(HAL_GPT_1, RTC_Tick, &g_epoch); //Register a user callback.
    hal_gpt_start_timer_ms(HAL_GPT_1, 1000, HAL_GPT_TIMER_TYPE_REPEAT);
}

//#################### UTC ####################
unsigned long sendNTPpacket(const char *host)
{
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);

    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0;          // Stratum, or type of clock
    packetBuffer[2] = 6;          // Polling Interval
    packetBuffer[3] = 0xEC;       // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

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

unsigned long getNetworkUTCTime()
{
    unsigned long epoch_return = 0;
    Serial.println("Starting connection to server...");
    const unsigned int localPort = 2390; // local port to listen for UDP packets
    Udp.begin(localPort);
    sendNTPpacket(NTP_server); // send an NTP packet to a time server

    while (!Udp.parsePacket())
    {
        delay(50);
    }
    Serial.println("packet received");

    if (Udp.available())
    {
        // We've received a packet, read the data from it
        const size_t readSize = Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
        Serial.print("Incoming NTP size=");
        Serial.println(readSize);

        //the timestamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, esxtract the two words:

        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        const unsigned long secsSince1900 = highWord << 16 | lowWord;
        Serial.print("Seconds since Jan 1 1900 = ");
        Serial.println(secsSince1900);

        const unsigned long epoch = secsSince1900;
        epoch_return = epoch;

        // print the hour, minute and second:
        Serial.print("The UTC time is ");      // UTC is the time at Greenwich Meridian (GMT)
        Serial.print((epoch % 86400L) / 3600); // print the hour (86400 equals secs per day)
        Serial.print(':');
        if (((epoch % 3600) / 60) < 10)
        {
            // In the first 10 minutes of each hour, we'll want a leading '0'
            Serial.print('0');
        }
        Serial.print((epoch % 3600) / 60); // print the minute (3600 equals secs per minute)
        Serial.print(':');
        if ((epoch % 60) < 10)
        {
            // In the first 10 seconds of each minute, we'll want a leading '0'
            Serial.print('0');
        }
        Serial.println(epoch % 60); // print the second
    }

    return epoch_return;
}

#define SECONDS_FROM_1900_TO_2000 3155673600UL;
static const uint8_t daysIntheMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

struct tm NTPparse(uint32_t t)
{
    t -= SECONDS_FROM_1900_TO_2000; // bring to 2000 timestamp from 1970
    struct tm NTPtime;
    NTPtime.tm_sec = t % 60;
    t /= 60;
    NTPtime.tm_min = t % 60;
    t /= 60;
    NTPtime.tm_hour = t % 24;

    uint32_t days = t / 24;
    uint8_t leap;
    int yOff = 0;
    int m = 0;
    int d = 0;

    for (yOff = 0;; ++yOff)
    {
        leap = yOff % 4 == 0;
        if (days < 365 + leap)
            break;
        days -= 365 + leap;
    }
    for (m = 1;; ++m)
    {
        uint8_t daysPerMonth = daysIntheMonth[m - 1];
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

void syncClock()
{
    g_epoch = getNetworkUTCTime();

    // UTC+8 for TW/BJ time
    g_epoch = g_epoch + 8 * 3600;

    Serial.print("Sync epoch = ");
    Serial.println(g_epoch);
}