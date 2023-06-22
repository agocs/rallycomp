#include "lib.cpp"
#include <stdio.h>

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

bool testParseFix(){
    bool passed = true;

    /* code */
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

    printf("About to start assertions...\n");
    passed = passed & floatAssertEquals(0, parsed.latitude, "Zeros latitude");
    passed = passed & floatAssertEquals(0, parsed.longitude, "Zeros longitude");

    //TODO: Make a test from live GPS data
    //4740.6709N, 12225.7666W
    parseFix(&parsed, 4740.6709, 'N', 12225.7666, 'W');
    passed = passed & floatAssertEquals(47.677845, parsed.latitude, "Recorded data latitude");
    passed = passed & floatAssertEquals(-122.429443, parsed.longitude, "Recorded data longitude");

    return passed;
}

int main(int argc, char const *argv[])
{
    bool passed = true;
    passed = passed & testParseFix();

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