
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include "SGP4.h"

char typerun, typeinput, opsmode;
gravconsttype whichconst;


int main(int argc, char const *argv[])
{
    typerun = 'c'; //catalog mode as we're reading from the testfile containing TLE data
    typeinput = 'e'; //epoch input type
    opsmode = 'i'; //best operation mode for smooth behaviour

    whichconst = wgs84; //most recent values for constants

    char inputFileName[] {"30sats.txt"};
    std::fstream inputFile(inputFileName);

    if (!inputFile.is_open()) {
        std::cerr << "Could not open file" << std::endl;
        return 1;
    }

    std::string line1, line2;

    while (std::getline(inputFile, line1) && std::getline(inputFile, line2)) {
        double startmfe, stopmfe, deltamin;
        elsetrec satelliteRecord;
        auto tempLine1 = strdup(line1.c_str());
        auto tempLine2 = strdup(line2.c_str());
        SGP4Funcs::twoline2rv(tempLine1, tempLine2, typerun, typeinput, opsmode, whichconst,  startmfe, stopmfe, deltamin, satelliteRecord);
        free(tempLine1);
        free(tempLine2);
        double r[3], v[3];
        SGP4Funcs::sgp4(satelliteRecord, 0.0, r, v);
        std::cout << r[0] << " " << r[1] << " " << r[2] << " " << v[0] << " " << v[1] << " " << v[2] << std::endl;
        double p, a, ecc, incl, omega, argp, nu, m, arglat, truelon, lonper;
        SGP4Funcs::rv2coe_SGP4(r, v, satelliteRecord.mus, p, a, ecc, incl, omega, argp, nu, m, arglat, truelon, lonper);
        std::cout << arglat << " " << truelon << std::endl;
    }

    return 0;
}
