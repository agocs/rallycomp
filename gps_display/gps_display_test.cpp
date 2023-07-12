#include "lib.cpp"
#include <stdio.h>
#include <time.h>
#include <math.h>

bool intAssertEquals(int expected, int test, char* message){
    if (expected != test){
        printf("TEST FAILED: %s\n", message);
        printf("\texpected: %i\n", expected);
        printf("\t.....got: %i\n", test);
        return false;
    }
    printf("Test passed: %s\n", message);
    return true;
}

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

bool boolAssertEquals(bool expected, bool test, char *message){
    if (expected != test){
        printf("TEST FAILED: %s\n", message);
        printf("\texpected: %d\n", expected);
        printf("\t.....got: %d\n", test);
    }
}

void printFix(fix toPrint){
    printf("\tlongitude: %f\n", toPrint.longitude);
    printf("\tlatitude:  %f\n", toPrint.latitude);
    printf("\taltitude:  %f\n", toPrint.altitude);
    printf("\ttime: %i:%i:%i.%i\n", toPrint.time.hours, toPrint.time.minutes, toPrint.time.seconds, toPrint.time.milliseconds);
}

bool testParseFix(){
    bool passed = true;

    fix parsed;
    agocsParseFix(&parsed, 5107.0017737, 'N', 11402.3291611, 'W', 100.0);
    //latitude 51 degrees 07.0017737 minutes
    //should be 51.1167
    //longitude 114 degrees 02.3291611 minutes
    //should be 114.03882

    printf("About to start assertions...\n");
    passed = passed & floatAssertEquals(51.1167F, parsed.latitude, "First latitude");
    passed = passed & floatAssertEquals(-114.03882F, parsed.longitude, "First longitude");
    passed = passed & floatAssertEquals(100, parsed.altitude, "First Altitude");

    agocsParseFix(&parsed, 0, 'N', 0, 'E', 0);
    passed = passed & floatAssertEquals(0, parsed.latitude, "Zeros latitude");
    passed = passed & floatAssertEquals(0, parsed.longitude, "Zeros longitude");
    passed = passed & floatAssertEquals(0, parsed.altitude, "zero altitude");

    //4740.6709N, 12225.7666W
    agocsParseFix(&parsed, 4740.6709, 'N', 12225.7666, 'W', 102.6);
    passed = passed & floatAssertEquals(47.677845, parsed.latitude, "Recorded data latitude");
    passed = passed & floatAssertEquals(-122.429443, parsed.longitude, "Recorded data longitude");
    passed = passed & floatAssertEquals(102.6, parsed.altitude, "Recorded data altitude");

    return passed;
}

bool testCalculateDisplacement(){
    bool passed = true;

    //47.677845,-122.429443 to 47.694186, -122.418624
    //gcmapper.com tells me 1990 meters at a bearing of 029 degrees.

    fix fix1;
    fix1.latitude = 47.677845;
    fix1.longitude = -122.429443;
    fix1.altitude = 50;

    fix fix2;
    fix2.latitude = 47.694186;
    fix2.longitude = -122.418624;
    fix2.altitude = 100;

    displacement output;

    calculateDisplacement(&output, fix1, fix2, true);

    //1989.448794 m flat
    //1990.077009 m with altitude
    passed = passed & doubleAssertEquals(1990.077009, output.distanceMeters, "Displacement distance");
    passed = passed & doubleAssertEquals(24.016263, output.bearingDegrees, "Displacement degrees");

    //one minute of latitude
    //should be exactly one nautical mile
    //or 1852ish meters

    fix fix3;
    fix3.latitude = 0;
    fix3.longitude = 0;
    fix3.altitude = 0;

    fix fix4;
    fix4.latitude = 0.016666666666; //one minute of latitude
    fix4.longitude = 0;
    fix4.altitude = 0;

    displacement output2;

    calculateDisplacement(&output2, fix3, fix4, true);

    passed = passed & doubleAssertEquals(1853.248874, output2.distanceMeters, "Displacement distance, one NM");
    passed = passed & doubleAssertEquals(0, output2.bearingDegrees, "Displacement degrees, one NM straight north");

    return passed;
}

bool testCalculateTimeDifference(){
    bool passed = true;

    instant time1;
    instant time2;

    time1.year = 2023;
    time1.month = 7;
    time1.day = 3;
    time1.hours = 21;
    time1.minutes = 36;
    time1.seconds = 18;
    time1.milliseconds = 400;

    time2.year = 2023;
    time2.month = 7;
    time2.day = 3;
    time2.hours = 21;
    time2.minutes = 36;
    time2.seconds = 18;
    time2.milliseconds = 800;

    int difference1 = calculateTimeDifference(time1, time2);
    passed = passed & intAssertEquals(400, difference1, "400 ms");

    time2.seconds = 19;
    time2.milliseconds = 400;

    int difference2 = calculateTimeDifference(time1, time2);
    passed = passed & intAssertEquals(1000, difference2, "second boundary");

    time2.minutes = 37;
    time2.milliseconds = 401;

    int difference3 = calculateTimeDifference(time1, time2);
    passed = passed & intAssertEquals(61001, difference3, "one minute and one ms");

    //let's test a day boundary

    time1.hours = 23;
    time1.minutes = 59;
    time1.seconds = 59;
    time1.milliseconds = 900;

    time2.day = 4;
    time2.hours = 0;
    time2.minutes = 0;
    time2.seconds = 0;
    time2.milliseconds = 100;

    int difference4 = calculateTimeDifference(time1, time2);
    passed = passed & intAssertEquals(200, difference4, "day boundary");

    //month boundary?

    time1.day = 31;

    time2.month = 8;
    time2.day = 1;

    int difference5 = calculateTimeDifference(time1, time2);
    passed = passed & intAssertEquals(200, difference5, "month boundary");

    return passed;
}

bool testAddFixes(){
    bool passed = true;

    for (int i = 0; i < 10; i++){
        fix fix1;
        fix1.latitude = 0;
        fix1.longitude = (float)i / 100;
        fix1.altitude = 0;

        instant time1;
        time1.year = 2023;
        time1.month = 7;
        time1.day = 3;
        time1.hours = 21;
        time1.minutes = 36;
        time1.seconds = i / 10;
        time1.milliseconds = i * 100;

        if (time1.milliseconds >= 1000){
            time1.milliseconds = time1.milliseconds % 1000;
        }

        fix1.time = time1;
        addFix(fix1);
    }

    passed = passed & intAssertEquals(9, fixes_head, "fixes_head should be 9 when fixes is full");
    passed = passed & boolAssertEquals(true, fixes_populated, "fixes_populated should be true at this point");

    //let's verify the contents of fixes to make sure I haven't effed something up.
    passed = passed & floatAssertEquals(.09, fixes[9].longitude, "fix[9].longitude should be .1 after all the insertions");
    passed = passed & floatAssertEquals(0, fixes[0].longitude, "the first fix.longitude should still be 0");

    for (int i = 0; i < 15; i++){
        fix fix1;
        fix1.latitude = 0;
        fix1.longitude = (float)i / 100;
        fix1.altitude = 0;

        instant time1;
        time1.year = 2023;
        time1.month = 7;
        time1.day = 3;
        time1.hours = 21;
        time1.minutes = 36;
        time1.seconds = i / 10;
        time1.milliseconds = i * 100;

        if (time1.milliseconds >= 1000){
            time1.milliseconds = time1.milliseconds % 1000;
        }

        fix1.time = time1;
        addFix(fix1);
    }

    passed = passed & intAssertEquals(4, fixes_head, "fixes_head should be 4 after adding a few more fixes.");

    velocity velo;
    double currentSpeed = calcCurrentSpeed(&velo);
    //probably someething absurdly high

    //0 N, 0.05 W to 0 N, 0.14 W in 0.9 seconds
    //gcmap.com says that's 10km, but I think it's dropping a few decimal points
    //Wolfram Alpha says 40,000 km/hour

    passed = passed & doubleAssertEquals(40030.173526, currentSpeed, "currentSpeed from these fixes");


    return passed;
}

bool testOutput(){
    bool passed = true;
    velocity velo;
    velo.speed_kmh = 12.345;
    velo.bearingDegrees = 1.234;

    char output[15];
    makeScreenOutput(output, velo);
    printf("%s\n", output);
    return true;
}

int main(int argc, char const *argv[])
{
    bool passed = true;
    passed = passed & testParseFix();
    passed = passed & testCalculateDisplacement();
    passed = passed & testCalculateTimeDifference();
    passed = passed & testAddFixes();
    passed = passed & testOutput();
    // passed = passed & testParseDateTime();

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