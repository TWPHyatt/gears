//
// Created by Thomas W. P. Knight on 24/04/2026.
//

#ifndef GEARS_VDBOUTPUT_HH
#define GEARS_VDBOUTPUT_HH

#include <openvdb/openvdb.h>
#include <string>
#include <vector>

class VDBOutput {
public:
    VDBOutput(double voxelSize = 1.0); // default voxel size 1 mm
    ~VDBOutput() = default;

    // Accumulate energy deposition at world position (mm) into the grid
    void Fill(double x, double y, double z, double de_keV);

    // Current event grid
    void SaveEvent(int eventNumber);

    // Write the grid to a .vdb file
    void Write(const std::string &filename);

    // Reset the grid
    void Reset();

private:
    double fVoxelSize; // voxel size in mm
    openvdb::FloatGrid::Ptr fTotalGrid;  // for accumulated energy over all events
    openvdb::FloatGrid::Accessor fTotalAccessor;  // ^^^
    openvdb::FloatGrid::Ptr fEventGrid;  // for accumulated energy over a single event (one per event)
    openvdb::FloatGrid::Accessor fEventAccessor;  // ^^^

    std::vector<openvdb::FloatGrid::Ptr> fEventGrids;  // vector to store all fEventEnergyGrid

    openvdb::FloatGrid::Ptr MakeGrid(const std::string &name);

};

#endif //GEARS_VDBOUTPUT_HH
