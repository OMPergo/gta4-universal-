#include "../../LibertyRecomp/gpu/gpu_compatibility.h"
#include <cassert>
#include <iostream>
#include <filesystem>

using namespace LibertyRecomp::Gpu;
using namespace LibertyRecomp::Diagnostics;

void TestMaliWorkarounds() {
  HardwareReport mali_report;
  mali_report.gpu_vendor = GpuVendor::ArmMali;
  mali_report.gpu_name = "Mali-G57 MC2";

  GpuWorkaroundFlags flags = EvaluateGpuWorkarounds(mali_report);
  assert(flags.mali_clip_distance_workaround == true);
  assert(flags.mali_subpass_load_fallback == true);
  assert(flags.mali_disable_depth_bounds == true);
  assert(flags.enable_pipeline_cache == true);
  assert(flags.aggressive_pipeline_prewarming == false);

  std::cout << "[PASS] Mali GPU Workarounds" << std::endl;
}

void TestAdrenoWorkarounds() {
  HardwareReport adreno_report;
  adreno_report.gpu_vendor = GpuVendor::QualcommAdreno;
  adreno_report.gpu_name = "Adreno 619";

  GpuWorkaroundFlags flags = EvaluateGpuWorkarounds(adreno_report);
  assert(flags.adreno_use_system_vulkan_driver == true);
  assert(flags.adreno_allow_turnip_experimental == true);
  assert(flags.adreno_use_libadrenotools == true);
  assert(flags.enable_pipeline_cache == true);

  std::cout << "[PASS] Adreno GPU Workarounds" << std::endl;
}

void TestAppleWorkarounds() {
  HardwareReport apple_report;
  apple_report.cpu_architecture = CpuArchitecture::AppleSilicon;
  apple_report.gpu_vendor = GpuVendor::Apple;

  GpuWorkaroundFlags flags = EvaluateGpuWorkarounds(apple_report);
  assert(flags.apple_use_metal_backend == true);
  assert(flags.moltenvk_synchronization2_workaround == true);

  std::cout << "[PASS] Apple / Metal GPU Workarounds" << std::endl;
}

void TestPipelineCachePersistence() {
  std::string test_dir = "test_pipeline_cache_dir";
  PipelineCacheManager cache_mgr(test_dir);
  assert(cache_mgr.InitializeCache() == true);

  std::vector<uint8_t> dummy_pipeline = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
  assert(cache_mgr.SaveCache(dummy_pipeline) == true);
  assert(cache_mgr.GetCachedPipelineDataSize() == dummy_pipeline.size());

  std::vector<uint8_t> loaded = cache_mgr.LoadCacheData();
  assert(loaded == dummy_pipeline);

  std::filesystem::remove_all(test_dir);
  std::cout << "[PASS] Pipeline Cache Real Filesystem Persistence" << std::endl;
}

int main() {
  std::cout << "Running GPU Compatibility Unit Tests..." << std::endl;
  TestMaliWorkarounds();
  TestAdrenoWorkarounds();
  TestAppleWorkarounds();
  TestPipelineCachePersistence();
  std::cout << "All GPU Compatibility Unit Tests Passed Successfully!" << std::endl;
  return 0;
}
