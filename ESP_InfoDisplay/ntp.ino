/**
  NTP time synchronization
  Name: NTP.ino
  Purpose: Functions for NTP time synchronization with TimeLib sync mechanism.

  @author basti8909
  @version 1.0 16/08/17
*/

/*-------- NTP code ----------*/
#include <WiFiUdp.h>

#define NTP_SERVER_NAME   "time.nist.gov"
#define NTP_LOCAL_PORT    8888
#define NTP_TIME_ZONE     1
#define NTP_PACKET_SIZE   48 // NTP time is in the first 48 bytes of message

byte NTP_packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
WiFiUDP Udp;

void NTP_startSync() 
{
  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(NTP_LOCAL_PORT);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(NTP_getTime);
  setSyncInterval(1);
}

time_t NTP_getTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(NTP_SERVER_NAME, ntpServerIP);
  Serial.print(NTP_SERVER_NAME);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  NTP_sendPacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 450) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(NTP_packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)NTP_packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)NTP_packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)NTP_packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)NTP_packetBuffer[43];
      setSyncInterval(3600);
      unsigned long epoch = secsSince1900 - 2208988800UL;   // convert to unix timestamp
      epoch += 3600 * NTP_TIME_ZONE;                        // Add timezone
      tmElements_t pretime;
      breakTime(epoch,pretime);
      int real_year=pretime.Year+1970;
      // Check for DST
      if (summertime_EU(real_year,pretime.Month,pretime.Day,pretime.Hour,1)) epoch+=3600;
      return epoch;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void NTP_sendPacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(NTP_packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  NTP_packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  NTP_packetBuffer[1] = 0;     // Stratum, or type of clock
  NTP_packetBuffer[2] = 6;     // Polling Interval
  NTP_packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  NTP_packetBuffer[12] = 49;
  NTP_packetBuffer[13] = 0x4E;
  NTP_packetBuffer[14] = 49;
  NTP_packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(NTP_packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

boolean summertime_EU(int year, byte month, byte day, byte hour, byte tzHours)
// European Daylight Savings Time calculation by "jurs" for German Arduino Forum
// input parameters: "normal time" for year, month, day, hour and tzHours (0=UTC, 1=MEZ)
// return value: returns true during Daylight Saving Time, false otherwise
{
  if (month<3 || month>10) return false; // keine Sommerzeit in Jan, Feb, Nov, Dez
  if (month>3 && month<10) return true; // Sommerzeit in Apr, Mai, Jun, Jul, Aug, Sep
  if (month==3 && (hour + 24 * day)>=(1 + tzHours + 24*(31 - (5 * year /4 + 4) % 7)) || month==10 && (hour + 24 * day)<(1 + tzHours + 24*(31 - (5 * year /4 + 1) % 7)))
    return true;
  else
    return false;
}

