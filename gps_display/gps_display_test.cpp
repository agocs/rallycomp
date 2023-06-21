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
