//
// Created by Thomas W. P. Knight on 24/04/2026.
//

#ifndef GEARS_VDBOUTPUT_HH
#define GEARS_VDBOUTPUT_HH

#include <openvdb/openvdb.h>
#include <string>

class VDBOutput {
public:
    VDBOutput(double voxelSize = 1.0); // default voxel size 1 mm
    ~VDBOutput() = default;

    /// Accumulate energy deposition at world position (mm) into the grid
    void Fill(double x, double y, double z, double de_keV);

    /// Write the current grid to a .vdb file
    void Write(const std::string &filename);

    /// Reset the grid (e.g. between runs)
    void Reset();

private:
    double fVoxelSize; // voxel size in mm
    openvdb::FloatGrid::Ptr fGrid;
    openvdb::FloatGrid::Accessor fAccessor;
};

#endif //GEARS_VDBOUTPUT_HH
