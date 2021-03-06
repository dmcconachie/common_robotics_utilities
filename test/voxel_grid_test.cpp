#include <cstdint>
#include <iostream>

#include <common_robotics_utilities/dynamic_spatial_hashed_voxel_grid.hpp>
#include <common_robotics_utilities/serialization.hpp>
#include <common_robotics_utilities/voxel_grid.hpp>

namespace common_robotics_utilities
{
namespace voxel_grid_test
{
void TestVoxelGridIndexLookup()
{
  const voxel_grid::GridSizes grid_sizes(1.0, 20.0, 20.0, 20.0);
  voxel_grid::VoxelGrid<int32_t> test_grid(grid_sizes, 0);
  // Load with special values
  int32_t fill_val = 1;
  std::vector<int32_t> check_vals;
  for (int64_t x_index = 0; x_index < test_grid.GetNumXCells(); x_index++)
  {
    for (int64_t y_index = 0; y_index < test_grid.GetNumYCells(); y_index++)
    {
      for (int64_t z_index = 0; z_index < test_grid.GetNumZCells(); z_index++)
      {
        test_grid.SetValue(x_index, y_index, z_index, fill_val);
        check_vals.push_back(fill_val);
        fill_val++;
      }
    }
  }
  std::vector<uint8_t> buffer;
  voxel_grid::VoxelGrid<int32_t>::Serialize(
      test_grid, buffer, serialization::SerializeMemcpyable<int32_t>);
  const voxel_grid::VoxelGrid<int32_t> read_grid
      = voxel_grid::VoxelGrid<int32_t>::Deserialize(
          buffer, 0, serialization::DeserializeMemcpyable<int32_t>).first;
  // Check the values
  int32_t check_index = 0;
  bool pass = true;
  for (int64_t x_index = 0; x_index < read_grid.GetNumXCells(); x_index++)
  {
    for (int64_t y_index = 0; y_index < read_grid.GetNumYCells(); y_index++)
    {
      for (int64_t z_index = 0; z_index < read_grid.GetNumZCells(); z_index++)
      {
        const int32_t check_val = check_vals.at(check_index);
        const int32_t ref_val
            = read_grid.GetImmutable(x_index, y_index, z_index).Value();
        const int32_t index_ref_val
            = read_grid.GetImmutable(
                voxel_grid::GridIndex(x_index, y_index, z_index)).Value();
        if (ref_val != check_val || index_ref_val != check_val)
        {
          pass = false;
        }
        check_index++;
      }
    }
  }
  if (pass)
  {
    std::cout << "VG-I-Lookup - All checks pass" << std::endl;
  }
  else
  {
    std::cout << "*** VG-I-Lookup - Checks failed ***" << std::endl;
  }
}

void TestVoxelGridLocationLookup()
{
  const voxel_grid::GridSizes grid_sizes(1.0, 20.0, 20.0, 20.0);
  voxel_grid::VoxelGrid<int32_t> test_grid(grid_sizes, 0);
  // Load with special values
  int32_t fill_val = 1;
  std::vector<int32_t> check_vals;
  for (double x_pos = -9.5; x_pos <= 9.5; x_pos += 1.0)
  {
    for (double y_pos = -9.5; y_pos <= 9.5; y_pos += 1.0)
    {
      for (double z_pos = -9.5; z_pos <= 9.5; z_pos += 1.0)
      {
        test_grid.SetValue(x_pos, y_pos, z_pos, fill_val);
        check_vals.push_back(fill_val);
        fill_val++;
      }
    }
  }
  std::vector<uint8_t> buffer;
  voxel_grid::VoxelGrid<int32_t>::Serialize(
      test_grid, buffer, serialization::SerializeMemcpyable<int32_t>);
  const voxel_grid::VoxelGrid<int32_t> read_grid
      = voxel_grid::VoxelGrid<int32_t>::Deserialize(
          buffer, 0, serialization::DeserializeMemcpyable<int32_t>).first;
  // Check the values
  int32_t check_index = 0;
  bool pass = true;
  for (double x_pos = -9.5; x_pos <= 9.5; x_pos += 1.0)
  {
    for (double y_pos = -9.5; y_pos <= 9.5; y_pos += 1.0)
    {
      for (double z_pos = -9.5; z_pos <= 9.5; z_pos += 1.0)
      {
        const int32_t check_val = check_vals.at(check_index);
        const int32_t ref_val
            = read_grid.GetImmutable(x_pos, y_pos, z_pos).Value();
        const int32_t ref_val_3d
            = read_grid.GetImmutable3d(
                Eigen::Vector3d(x_pos, y_pos, z_pos)).Value();
        const int32_t ref_val_4d
            = read_grid.GetImmutable4d(
                Eigen::Vector4d(x_pos, y_pos, z_pos, 1.0)).Value();
        if (ref_val != check_val || ref_val_3d != check_val
            || ref_val_4d != check_val)
        {
          pass = false;
        }
        check_index++;
        const voxel_grid::GridIndex query_index
            = read_grid.LocationToGridIndex(x_pos, y_pos, z_pos);
        const voxel_grid::GridIndex query_index_3d
            = read_grid.LocationToGridIndex3d(
                Eigen::Vector3d(x_pos, y_pos, z_pos));
        const voxel_grid::GridIndex query_index_4d
            = read_grid.LocationToGridIndex4d(
                Eigen::Vector4d(x_pos, y_pos, z_pos, 1.0));
        if (query_index != query_index_3d || query_index != query_index_4d)
        {
          pass = false;
        }
        const Eigen::Vector4d query_location
            = read_grid.GridIndexToLocation(query_index);
        if (x_pos != query_location(0) || y_pos != query_location(1)
            || z_pos != query_location(2) || 1.0 != query_location(3))
        {
          pass = false;
        }
        const voxel_grid::GridIndex found_query_index
            = read_grid.LocationToGridIndex4d(query_location);
        if (found_query_index != query_index)
        {
            pass = false;
        }
      }
    }
  }
  if (pass)
  {
    std::cout << "VG-L-Lookup - All checks pass" << std::endl;
  }
  else
  {
    std::cout << "*** VG-L-Lookup - Checks failed ***" << std::endl;
  }
}

void TestDSHVoxelGridLocationLookup()
{
  const voxel_grid::GridSizes chunk_sizes(1.0, 4.0, 4.0, 4.0);
  voxel_grid::DynamicSpatialHashedVoxelGrid<int32_t> test_grid(chunk_sizes, 0);
  // Load with special values
  int32_t fill_val = 1;
  std::vector<int32_t> check_vals;
  for (double x_pos = -9.5; x_pos <= 9.5; x_pos += 1.0)
  {
    for (double y_pos = -9.5; y_pos <= 9.5; y_pos += 1.0)
    {
      for (double z_pos = -9.5; z_pos <= 9.5; z_pos += 1.0)
      {
        test_grid.SetValue(
            x_pos, y_pos, z_pos, voxel_grid::DSHVGSetType::SET_CELL, fill_val);
        check_vals.push_back(fill_val);
        fill_val++;
      }
    }
  }
  std::vector<uint8_t> buffer;
  voxel_grid::DynamicSpatialHashedVoxelGrid<int32_t>::Serialize(
      test_grid, buffer, serialization::SerializeMemcpyable<int32_t>);
  const voxel_grid::DynamicSpatialHashedVoxelGrid<int32_t> read_grid
      = voxel_grid::DynamicSpatialHashedVoxelGrid<int32_t>::Deserialize(
          buffer, 0, serialization::DeserializeMemcpyable<int32_t>).first;
  // Check the values
  int32_t check_index = 0;
  bool pass = true;
  for (double x_pos = -9.5; x_pos <= 9.5; x_pos += 1.0)
  {
    for (double y_pos = -9.5; y_pos <= 9.5; y_pos += 1.0)
    {
      for (double z_pos = -9.5; z_pos <= 9.5; z_pos += 1.0)
      {
        const int32_t check_val = check_vals.at(check_index);
        const auto lookup = read_grid.GetImmutable(x_pos, y_pos, z_pos);
        const int32_t ref_val = lookup.Value();
        if (ref_val != check_val)
        {
          pass = false;
        }
        if (lookup.FoundStatus() != voxel_grid::DSHVGFoundStatus::FOUND_IN_CELL)
        {
          pass = false;
        }
        check_index++;
      }
    }
  }
  if (pass)
  {
    std::cout << "DSHVG - All checks pass" << std::endl;
  }
  else
  {
    std::cout << "*** DSHVG - Checks failed ***" << std::endl;
  }
}

void DoTest()
{
  TestVoxelGridIndexLookup();
  TestVoxelGridLocationLookup();
  TestDSHVoxelGridLocationLookup();
}
}  // namespace voxel_grid_test
}  // namespace common_robotics_utilities

int main(int, char**)
{
  common_robotics_utilities::voxel_grid_test::DoTest();
  return EXIT_SUCCESS;
}
