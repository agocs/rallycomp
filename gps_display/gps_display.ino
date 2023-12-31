#include <Adafruit_GrayOLED.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>

#include <splash.h>
#include <Adafruit_SH110X.h>

#include <Adafruit_GPS.h>

//my stuff
#include "./lib.cpp"

// what's the name of the hardware serial port?
#define GPSSerial Serial1

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

uint32_t timer = millis();


Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);

// OLED FeatherWing buttons map to different pins depending on board:
#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
#elif defined(ESP32) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)
  #define BUTTON_A 15
  #define BUTTON_B 32
  #define BUTTON_C 14
#elif defined(ARDUINO_STM32_FEATHER)
  #define BUTTON_A PA15
  #define BUTTON_B PC7
  #define BUTTON_C PC5
#elif defined(TEENSYDUINO)
  #define BUTTON_A  4
  #define BUTTON_B  3
  #define BUTTON_C  8
#elif defined(ARDUINO_NRF52832_FEATHER)
  #define BUTTON_A 31
  #define BUTTON_B 30
  #define BUTTON_C 27
#else // 32u4, M0, M4, nrf52840, esp32-s2 and 328p
  #define BUTTON_A  9
  #define BUTTON_B  6
  #define BUTTON_C  5
#endif

void setup() {
  Serial.begin(115200);

  Serial.println("Starting the GPS");
  delay(250); // wait for the OLED to power up
  display.begin(0x3C, true); // Address 0x3C default

  Serial.println("OLED begun");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setRotation(1);
  
  // text display tests
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println("Setting up GPS");
  display.display(); // actually display all of the above

  
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ); // 5 Hz update rate

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
  
}



void loop() {

  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    //Serial.print(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another

    fix f;
    agocsParseFix(&f, GPS.latitude, GPS.lat, GPS.longitude, GPS.lon, GPS.altitude);
    instant i;
    i.year = GPS.year;
    i.month = GPS.month;
    i.day = GPS.day;
    i.hours = GPS.hour;
    i.minutes = GPS.minute;
    i.seconds = GPS.seconds;
    i.milliseconds = GPS.milliseconds;
    f.time = i;
    addFix(f);

    velocity v;
    calcCurrentSpeed(&v, true);

    char output[18];
    makeScreenOutput(output, v);

    velocity newV;
    calcCurrentSpeed(&newV, false);

    double speed_mph = toMiles(v.speed_kmh);
    float gps_speed = GPS.speed * 1.15;
    double speed_without_alt = toMiles(newV.speed_kmh);

    sprintf(output, "%5.2f\n%5.2f\n%5.2f", speed_mph, gps_speed, speed_without_alt);

    Serial.printf("calculated:%f,gps:%f,no_alt:%f\n", speed_mph, gps_speed, speed_without_alt);

    display.clearDisplay();
    display.setCursor(0,0);
    display.println(output);
    display.display();

  }

  // approximately every 2 seconds or so, print out the current stats
  // if (millis() - timer > 2000) {
  //   timer = millis(); // reset the timer
  //   display.clearDisplay();
  //   Serial.print("\nTime: ");
  //   if (GPS.hour < 10) { Serial.print('0'); }
  //   Serial.print(GPS.hour, DEC); Serial.print(':');
  //   if (GPS.minute < 10) { Serial.print('0'); }
  //   Serial.print(GPS.minute, DEC); Serial.print(':');
  //   if (GPS.seconds < 10) { Serial.print('0'); }
  //   Serial.print(GPS.seconds, DEC); Serial.print('.');
  //   if (GPS.milliseconds < 10) {
  //     Serial.print("00");
  //   } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
  //     Serial.print("0");
  //   }
  //   Serial.println(GPS.milliseconds);
  //   Serial.print("Date: ");
  //   Serial.print(GPS.day, DEC); Serial.print('/');
  //   Serial.print(GPS.month, DEC); Serial.print("/20");
  //   Serial.println(GPS.year, DEC);
  //   Serial.print("Fix: "); Serial.print((int)GPS.fix);
  //   Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
  //   if (GPS.fix) {
  //     Serial.print("Location: ");
  //     Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
  //     Serial.print(", ");
  //     Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
  //     Serial.print("Speed (knots): "); Serial.println(GPS.speed);
  //     Serial.print("Angle: "); Serial.println(GPS.angle);
  //     Serial.print("Altitude: "); Serial.println(GPS.altitude);
  //     Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
  //     Serial.print("Antenna status: "); Serial.println((int)GPS.antenna);
  //   }
  // }


  delay(10);
  yield();
}
