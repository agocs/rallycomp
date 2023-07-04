#include <stdio.h>
#include <math.h>
#include <cstdint>
#include <cmath>

// struct screenInfo {
//   String time;
//   String current_speed;
//   String odometer;
//   String average_speed;
// };

////     //parseDateTime(uint8_t year, uint8_t month, uint8_t day,
//     //                 uint8_t hours, uint8_t minutes, uint8_t seconds,
//     //                 uint16_t milliseconds)
struct instant{
  uint8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint16_t milliseconds;
};

struct fix{
  float latitude;
  float longitude;
  float altitude;
  instant *time;
};

struct displacement{
  double distanceMeters;
  double bearingDegrees;
};

int parseFix (struct fix *output, float latitude, char lat_dir, float longitude, char lon_dir, float altitude) {
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
  (*output).altitude = altitude;
  return 0;
}


const double EarthRadius = 6371000.0;  // Earth's radius in meters

double toRadians(double degrees) {
  return degrees * M_PI / 180.0;
}

double toDegrees(double radians) {
  return radians * 180.0 / M_PI;
}

double calculateDisplacement(struct displacement *output, const fix& fix1, const fix& fix2) {
  double lat1 = toRadians(fix1.latitude);
  double lon1 = toRadians(fix1.longitude);
  double lat2 = toRadians(fix2.latitude);
  double lon2 = toRadians(fix2.longitude);

  double deltaLat = lat2 - lat1;
  double deltaLon = lon2 - lon1;

  double a = std::sin(deltaLat / 2) * std::sin(deltaLat / 2) +
             std::cos(lat1) * std::cos(lat2) *
             std::sin(deltaLon / 2) * std::sin(deltaLon / 2);
  double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

  double y = std::sin(deltaLon) * std::cos(lat2);
  double x = std::cos(lat1) * std::sin(lat2) -
             std::sin(lat1) * std::cos(lat2) * std::cos(deltaLon);

  double bearing = std::atan2(y, x);
  bearing = toDegrees(bearing);

  // Ensure the bearing is within the range [0, 360)
  if (bearing < 0) {
    bearing += 360.0;
  }

  double distance = EarthRadius * c;

  float altitude_difference = fix2.altitude - fix1.altitude;

  distance = sqrt (pow(distance, 2) + pow(altitude_difference, 2));

  (*output).distanceMeters = distance;
  (*output).bearingDegrees = bearing;

  return distance;
}

//This really only needs to work when start and end are within like 2 seconds of each other, max
//returns difference in milliseconds
int calculateTimeDifference(struct instant *start, struct instant *end){
   int result = end->milliseconds - start->milliseconds;
   result += (end->seconds - start->seconds) * 1000;
   result += (end->minutes - start->minutes) * 60 * 1000;
   result += (end->hours - start->hours) * 60 * 60 * 1000;

   //okay realistically we're only going to see the same day or the next day here
   //This solution should take care of month boundaries, year boundaries, and leap years
   if (end->day != start->day){
    result += 24 * 60 * 60 * 1000;
   }
  return result;
};


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