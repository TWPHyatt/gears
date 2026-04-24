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
  openvdb::initialize();

  fGrid->setName("energy_deposition");
  fGrid->setGridClass(openvdb::GRID_FOG_VOLUME);
  fGrid->setTransform(
      openvdb::math::Transform::createLinearTransform(fVoxelSize)
  );

  G4cout << "[VDB] constructor called." << G4endl;
}

void VDBOutput::Fill(double x, double y, double z, double de_keV){
  G4cout << "[VDB] Fill() called." << G4endl;
}


void VDBOutput::Write(const std::string &filename) {
  G4cout << "[VDB] Write() called." << G4endl;
}

void VDBOutput::Reset() {
  G4cout << "[VDB] Rest() called." << G4endl;
}