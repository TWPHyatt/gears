//
// Created by Thomas W. P. Knight on 24/04/2026.
//

#include <openvdb/openvdb.h>
#include "VDBOutput.hh"
#include <iostream>
#include <G4SteppingManager.hh>
#include <G4SteppingVerbose.hh>

VDBOutput::VDBOutput(double voxelSize)
    : fVoxelSize(voxelSize),
      fGrid(openvdb::FloatGrid::create(0.0f)),
      fAccessor(fGrid->getAccessor())
{
  G4cout << "[VDB] constructor called." << G4endl;

  openvdb::initialize();
  fGrid->setName("energy_deposition");
  //fGrid->setGridClass(openvdb::GRID_FOG_VOLUME);
  fGrid->setTransform(openvdb::math::Transform::createLinearTransform(fVoxelSize));

  fGrid->insertMeta("units_keV",  openvdb::StringMetadata("keV"));   // energy units label
  fGrid->insertMeta("voxel_mm",   openvdb::FloatMetadata((float)voxelSize)); // " voxel size label"
}

void VDBOutput::Fill(double x, double y, double z, double de_keV){
  ///  Fill is called once per simulation step
  ///  Takes world-space position of that step and the energy deposited,converts to a voxel index
  ///  Then adds that energy to the voxel

  G4cout << " > [VDB] Fill() called." << G4endl;

  if (de_keV <= 0.0) return;

  // Convert world position (in mm) to voxel index
  openvdb::Vec3d worldPos(x, y, z);
  openvdb::Vec3d indexPos = fGrid->worldToIndex(worldPos);

  openvdb::Coord xyz(
      static_cast<int>(std::floor(indexPos.x())),
      static_cast<int>(std::floor(indexPos.y())),
      static_cast<int>(std::floor(indexPos.z()))
  );

  // set voxel value to accumulated energy (kev)
  fAccessor.setValue(xyz, fAccessor.getValue(xyz) + static_cast<float>(de_keV));
}

void VDBOutput::Write(const std::string &filename) {
  ///  Write the grid to a vdb file

  G4cout << "[VDB] Write() called." << G4endl;


}

void VDBOutput::Reset() {
  ///  discard the current grid and make a new one
  ///  to be used at the end of each beamOn?
  G4cout << "[VDB] Rest() called." << G4endl;
}