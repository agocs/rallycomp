#include <stdio.h>
#include <math.h>

// struct screenInfo {
//   String time;
//   String current_speed;
//   String odometer;
//   String average_speed;
// };

struct fix{
  float latitude;
  float longitude;
};

int parseFix (struct fix *output, float latitude, char lat_dir, float longitude, char lon_dir) {
  //this might be a little lossy, but hopefully not very
  //move decimal point two places to the left
  latitude = latitude / 100.0;
  float lat_degrees = floor(latitude);
  float lat_minutes = (latitude - lat_degrees) * 100.0;
  lat_degrees = lat_degrees + (lat_minutes / 60.0); //hopefully the compiler optimizes this

  if (lat_dir == 'S') {
    lat_degrees = lat_degrees * -1;
  }

  longitude = longitude / 100.0;
  float lon_degrees = floor(longitude);
  float lon_minutes = (longitude - lon_degrees) * 100.0;
  lon_degrees = lon_degrees + (lon_minutes / 60.0);
  
  if (lon_dir == 'W'){
    lon_degrees = lon_degrees * -1;
  }

  (*output).latitude = lat_degrees;
  (*output).longitude = lon_degrees;
  return 0;
}

// int makeScreenInfo(struct screenInfo *output) {
//   //time
//   //23:59:59 -- 8 chars
//   char time[8];
//   sprintf(time, "%d:%d:%d", GPS.hour, GPS.minute, GPS.seconds);

//   //current_speed
//   //GPSRMC fix doesn't have enough precision
//   //TODO: instantaneous speed
//   //TODO: odometer
//   //TODO: average speed

//   return 1;
// }