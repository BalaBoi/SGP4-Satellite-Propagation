#ifndef SATELLITE_H
#define SATELLITE_H
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <vector>

#include "SGP4.h"

class Satellite;

struct StateVector {
    double position[3];
    double velocity[3];
    double latitude;
    double longitude;
    double altitude;
    const Satellite* satellite;
    bool withinRegion = false;
};

struct LatLongRegion {
    double A[2];
    double B[2]; 
    double C[2]; 
    double D[2];
};

class Satellite {
public:
    static std::vector<Satellite> readSatelliteFile(const std::string& inputFileName);
    StateVector findStateVector(double time) const;
    static bool containedIn(const StateVector &sv, const LatLongRegion &region);
    static double time(int year, int month, int day, int hour, int minute, double seconds);
    Satellite(std::string tle_1, std::string tle_2);
private:
    char typerun, typeinput, opsmode;
    gravconsttype whichconst;
    double startmfe, stopmfe, deltamin;
    elsetrec satelliteRecord;

    static bool insideTriangle(const double P[2], const double A[2], const double B[2], const double C[2]);
};
#endif