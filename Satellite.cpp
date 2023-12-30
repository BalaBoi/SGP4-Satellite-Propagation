#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <vector>
#include "SGP4.h"

#include "Satellite.h"

static const double PI = 3.14159265358979323846;

std::vector<Satellite> Satellite::readSatelliteFile(const std::string& inputFileName) {
    std::fstream inputFile(inputFileName);

    if (!inputFile.is_open()) {
        std::cerr << "Could not open file" << std::endl;
        return std::vector<Satellite>();
    }

    std::vector<Satellite> satelliteList;
    std::string line1, line2;

    while (std::getline(inputFile, line1) && std::getline(inputFile, line2)) {
        satelliteList.push_back(Satellite(line1, line2));
    }

    return satelliteList;
}

StateVector Satellite::findStateVector(double time) const {
    double r[3], v[3];
    double p, a, ecc, incl, omega, argp, nu, m, arglat, truelon, lonper;
    auto tempSatRec = satelliteRecord;
    SGP4Funcs::sgp4(tempSatRec, time, r, v); //propagation
    SGP4Funcs::rv2coe_SGP4(r, v, tempSatRec.mus, p, a, 
    ecc, incl, omega, argp, nu, m, arglat, truelon, lonper); //from rv to classical orbital elements
    StateVector out = {{r[0], r[1], r[2]}, //P
    {v[0], v[1], v[2]}, //V
    arglat*180.0/PI, //Lat
    truelon*180.0/PI, //Long
    SGP4Funcs::mag_SGP4(r) - satelliteRecord.radiusearthkm, //Alt
    this}; //pointer to satellite
    return out;
}

bool Satellite::containedIn(const StateVector& sv, const LatLongRegion& region) {
    double P[2] {sv.latitude, sv.longitude};
    //check if sv is in triangles ABD or BCD
    return (insideTriangle(P, region.A, region.B, region.C) || 
    insideTriangle(P, region.B, region.C, region.D));
}

double Satellite::time(int year, int month, int day, int hour, int minute, double seconds) {
    double jday, jdayFrac;
    SGP4Funcs::jday_SGP4(year, month, day, hour, minute, seconds, jday, jdayFrac);
    return jday+jdayFrac;
}

Satellite::Satellite(std::string tle_1, std::string tle_2) {
    typerun = 'c'; //catalog mode as we're reading from the testfile containing TLE data
    typeinput = 'e'; //epoch input type
    opsmode = 'i'; //best operation mode for smooth behaviour
    whichconst = wgs84; //most recent values for constants

    char *temp1{strdup(tle_1.c_str())}, *temp2{strdup(tle_2.c_str())};
    SGP4Funcs::twoline2rv(temp1, temp2, typerun, typeinput, opsmode,
    whichconst,  startmfe, stopmfe, deltamin, satelliteRecord); //converts TLE to startmfe, stopmfe, deltamin and satelliteRecord
    free(temp1);
    free(temp2);
    
}

bool Satellite::insideTriangle(const double P[2], const double A[2], const double B[2], const double C[2]) {
    //Cross products ABxAP, BCxBP and CAxCP will be all negative or all positive if P is inside ABC
    double AB[3] {B[0]-A[0], B[1]-A[1], 0}, 
    BC[3] {C[0]-B[0], C[1]-B[1], 0}, 
    CA[3] {A[0]-C[0], A[1]-C[1], 0};
    double AP[3] {P[0]-A[0], P[1]-A[1], 0},
    BP[3] {P[0]-B[0], P[1]-B[1], 0},
    CP[3] {P[0]-C[0], P[1]-C[1], 0};
    
    double crossAB[3], crossBC[3], crossCA[3]; 
    SGP4Funcs::cross_SGP4(AB, AP, crossAB);
    SGP4Funcs::cross_SGP4(BC, BP, crossBC);
    SGP4Funcs::cross_SGP4(CA, CP, crossCA);

    return ((crossAB[2]>=0 && crossBC[2]>=0 && crossCA[2]>=0) || 
    (crossAB[2]<=0 && crossBC[2]<=0 && crossCA[2]<=0));
}
