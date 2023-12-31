# SGP4-Satellite-Propagation
C++ Code to convert a text file containing TLE data to time, P(position) and V(velocity) of the projected path for t = 0 to a given number of days and time-step, using the SGP4 model.
Code files other than Satellite.h, Satellite.cpp, main.cpp and test.cpp are taken from https://celestrak.org/publications/AIAA/2006-6753/.
# Setup
`
g++ -o out main.cpp Satellite.cpp SGP4.cpp
`
All configuration information is present in main.cpp as file-local global variables.
# Code structure
Parallelization is done through futures using std::async. There is a satellite_task for each satellite data in the TLE file. Each satellite_task delegates work to group_tasks that sequentially deal with calculations related to a group of time slices, the size of which is specified.
Interfacing with the SGP4 library is mainly done through the Satellite class. The instance method findStateVector finds the position and velocity values for a given time and checks if the corresponding Latitude and Longitude values lie within a region specified by 4 latlong coordinates. The method used to find belonging is by checking if the point belongs to two triangles that make up the quadrilateral. A region cannot be of the type that wraps around from -180 longitude to 180. This is so because allowing for it would leave the region unspecified with just the data of the four points.
# Summary Table
For my specific machine, a group size of 5000 seemed appropriate and gave the quickest time of 4secs.
