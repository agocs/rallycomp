#include "lib.cpp"
#include <stdio.h>
#include <time.h>
#include <math.h>

bool floatAssertEquals(float expected, float test, char *message){
    if (expected != test) {
        printf("TEST FAILED: %s\n", message);
        printf("\texpected: %f\n", expected);
        printf("\t.....got: %f\n", test);
        return false;
    }
    printf("Test passed: %s\n", message);
    return true;
}

bool doubleAssertEquals(double expected, double test, char *message){
    if (std::abs(expected - test) > 0.0001) {
        printf("TEST FAILED: %s\n", message);
        printf("\texpected: %f\n", expected);
        printf("\t.....got: %f\n", test);
        printf("\t        -----------------\n");
        printf("\t....diff: %f\n", expected - test);
        return false;
    }
    printf("Test passed: %s\n", message);
    return true;
}

bool testParseFix(){
    bool passed = true;

    fix parsed;
    parseFix(&parsed, 5107.0017737, 'N', 11402.3291611, 'W');
    //latitude 51 degrees 07.0017737 minutes
    //should be 51.1167
    //longitude 114 degrees 02.3291611 minutes
    //should be 114.03882

    printf("About to start assertions...\n");
    passed = passed & floatAssertEquals(51.1167F, parsed.latitude, "First latitude");
    passed = passed & floatAssertEquals(-114.03882F, parsed.longitude, "First longitude");

    parseFix(&parsed, 0, 'N', 0, 'E');
    passed = passed & floatAssertEquals(0, parsed.latitude, "Zeros latitude");
    passed = passed & floatAssertEquals(0, parsed.longitude, "Zeros longitude");

    //4740.6709N, 12225.7666W
    parseFix(&parsed, 4740.6709, 'N', 12225.7666, 'W');
    passed = passed & floatAssertEquals(47.677845, parsed.latitude, "Recorded data latitude");
    passed = passed & floatAssertEquals(-122.429443, parsed.longitude, "Recorded data longitude");

    return passed;
}

bool testCalculateDisplacement(){
    bool passed = true;

    //47.677845,-122.429443 to 47.694186, -122.418624
    //gcmapper.com tells me 1990 meters at a bearing of 029 degrees.

    fix fix1;
    fix1.latitude = 47.677845;
    fix1.longitude = -122.429443;

    fix fix2;
    fix2.latitude = 47.694186;
    fix2.longitude = -122.418624;

    displacement output;

    calculateDisplacement(&output, fix1, fix2);

    passed = passed & doubleAssertEquals(1989.448794, output.distanceMeters, "Displacement distance");
    passed = passed & doubleAssertEquals(24.016263, output.bearingDegrees, "Displacement degrees");

    //one minute of latitude
    //should be exactly one nautical mile
    //or 1852ish meters

    fix fix3;
    fix3.latitude = 0;
    fix3.longitude = 0;

    fix fix4;
    fix4.latitude = 0.016666666666; //one minute of latitude
    fix4.longitude = 0;

    displacement output2;

    calculateDisplacement(&output2, fix3, fix4);

    passed = passed & doubleAssertEquals(1853.248874, output2.distanceMeters, "Displacement distance, one NM");
    passed = passed & doubleAssertEquals(0, output2.bearingDegrees, "Displacement degrees, one NM straight north");

    return passed;
}

int main(int argc, char const *argv[])
{
    bool passed = true;
    passed = passed & testParseFix();
    passed = passed & testCalculateDisplacement();

    if (!passed){
        printf("ONE OR MORE TESTS FAILED!!!!\n");
    } else {
        printf("Tests passed!! =)\n");
    }
}


/*
Some NMEA data

 $GLGSA,A,3,78,68,85,79,69,,,,,,,,1.37,1.10,0.82*1C
 $GNGGA,041753.000,4740.6710,N,12225.7710,W,1,10,1.10,102.6,M,-17.3,M,,*4C
 $GNRMC,041753.000,A,4740.6710,N,12225.7710,W,0.15,0.00,220623,,,A*61
 $GNVTG,0.00,T,,M,0.12,N,0.23,K,A*21
 $GPGSA,A,3,03,25,31,32,26,,,,,,,,1.37,1.10,0.82*0E
*/