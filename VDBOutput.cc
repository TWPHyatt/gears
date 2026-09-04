//
// Created by Thomas W. P. Knight on 24/04/2026.
//

#include <openvdb/openvdb.h>
#include "VDBOutput.hh"
#include <iostream>
#include <G4SteppingManager.hh>
#include <G4SteppingVerbose.hh>

openvdb::FloatGrid::Ptr VDBOutput::MakeGrid(const std::string &name) {
  auto grid = openvdb::FloatGrid::create(0.0f);
  grid->setName(name);
  grid->setTransform(openvdb::math::Transform::createLinearTransform(fVoxelSize));
  grid->insertMeta("units_keV", openvdb::StringMetadata("keV"));  // energy units label
  grid->insertMeta("voxel_mm",  openvdb::FloatMetadata((float)fVoxelSize));  // " voxel size label"
  return grid;
}

VDBOutput::VDBOutput(double voxelSize)
    : fVoxelSize(voxelSize),
      fTotalGrid(nullptr),
      fTotalAccessor(openvdb::FloatGrid::create(0.0f)->getAccessor()),  // temp
      fEventGrid(nullptr),
      fEventAccessor(openvdb::FloatGrid::create(0.0f)->getAccessor()) // temp
{
  G4cout << "[VDB] constructor called." << G4endl;
  openvdb::initialize();
  fTotalGrid    = MakeGrid("total_energy_deposition");
  fEventGrid    = MakeGrid("event_0");  // name placeholder
  fTotalAccessor = fTotalGrid->getAccessor();
  fEventAccessor = fEventGrid->getAccessor();
}

void VDBOutput::Fill(double x, double y, double z, double de_keV){
  ///  Fill is called once per simulation step
  ///  Takes world-space position of that step and the energy deposited,converts to a voxel index
  ///  Then adds that energy to the voxel

  G4cout << " > [VDB] Fill() called." << G4endl;

  if (de_keV <= 0.0) return;

  // Convert world position (in mm) to voxel index
  openvdb::Vec3d worldPos(x, y, z);
  openvdb::Vec3d indexPos = fTotalGrid->worldToIndex(worldPos);

  openvdb::Coord xyz(
      static_cast<int>(std::floor(indexPos.x())),
      static_cast<int>(std::floor(indexPos.y())),
      static_cast<int>(std::floor(indexPos.z()))
  );

  // set voxel value to accumulated energy (kev)
  float de = static_cast<float>(de_keV);
  fTotalAccessor.setValue(xyz, fTotalAccessor.getValue(xyz) + de);
  fEventAccessor.setValue(xyz, fEventAccessor.getValue(xyz) + de);
}

void VDBOutput::SaveEvent(int eventNumber){
  /// Name the event grid, store it, then create a fresh one

  G4cout << "[VDB] SaveEvent() called. #" << eventNumber << G4endl;

  fEventGrid->setName("event_" + std::to_string(eventNumber));

  fEventGrids.push_back(fEventGrid);

  fEventGrid     = MakeGrid("event_current");
  fEventAccessor = fEventGrid->getAccessor();

}

void VDBOutput::WriteVDB(const std::string &filename) {
  ///  Write the grid to a vdb file

  G4cout << "[VDB] WriteVDB() called: " << filename << G4endl;

  openvdb::GridPtrVec grids;

  // Per-event grids
  for (auto &g : fEventGrids)
    grids.push_back(g);

  // Total grid
  grids.push_back(fTotalGrid);

  openvdb::io::File file(filename);
  file.write(grids);
  file.close();

  G4cout << "[VDB] Written " << grids.size() << " grids (" << fEventGrids.size() << " events + 1 total) to "
  << filename << G4endl;
}

void VDBOutput::WriteNVDB(const std::string &filename)
{
  G4cout << "[NanoVDB] WriteNVDB() called: " << filename << G4endl;

  std::vector<nanovdb::GridHandle<nanovdb::HostBuffer>> nanoGrids;

  // Per-event grids
  for (const auto &grid : fEventGrids) {nanoGrids.push_back(nanovdb::tools::openToNanoVDB(grid));}

  // Total grid
  nanoGrids.push_back(nanovdb::tools::openToNanoVDB(fTotalGrid));

  nanovdb::io::writeGrids(filename, nanoGrids);

  G4cout << "[NanoVDB] Written " << nanoGrids.size() << " grids to " << filename  << G4endl;
}

void VDBOutput::Reset() {
  ///  discard the current grid and make a new one
  ///  to be used at the end of each beamOn?

  G4cout << "[VDB] Reset() called." << G4endl;

  fEventGrids.clear();

  fTotalGrid = MakeGrid("total_energy_deposition");
  fTotalAccessor = fTotalGrid->getAccessor();
  fEventGrid = MakeGrid("event_current");
  fEventAccessor = fEventGrid->getAccessor();
}