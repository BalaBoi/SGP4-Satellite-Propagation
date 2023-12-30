#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <future>

#include "Satellite.h"

static const std::string inputFilename {"27000sats.txt"};
static const LatLongRegion region {{16.66673, 103.58196},
{69.74973,-120.64459},
{-21.09096, -119.71009},
{-31.32309, -147.79778}};   //regions cannot wrap around, regions are defined by lines 
                            //that appear straight in the cylindrical projection of the 
                            //assumed sphere of the Earth
    

StateVector state_vector_task(const Satellite& satellite, const double& time) {
    auto out = satellite.findStateVector(time);
    if (Satellite::containedIn(out, region)) { //Checking if satellite is within region
        out.withinRegion = true;
    }
    return out;
}

std::vector<StateVector> satellite_task(const Satellite& satellite) {
    std::vector<std::future<StateVector>> state_vector_futures;
    double max_t = Satellite::time(0,0,5,0,0,0);
    double delta_t = Satellite::time(0,0,0,0,0,1);
    for (double t=0; t<=max_t; t+=delta_t) {
        state_vector_futures.push_back(std::async(state_vector_task, satellite, t));
    }
    std::vector<StateVector> state_vectors;
    for (auto& state_vector_future: state_vector_futures) {
        state_vectors.push_back(state_vector_future.get());
    }
    return state_vectors;
}

int main(int argc, char const *argv[])
{
    auto satellites = Satellite::readSatelliteFile(inputFilename);
    std::vector<std::future<std::vector<StateVector>>> satellite_futures;
    for (auto& satellite: satellites) {//launching tasks
        satellite_futures.push_back(std::async(satellite_task, satellite));
    }

    std::vector<StateVector> state_vectors;
    for (auto& future: satellite_futures) {
        for (auto& sv: future.get()) {
            state_vectors.push_back(sv);
        }
    }
    
    std::cout << "done" << std::endl;
    return 0;
}
