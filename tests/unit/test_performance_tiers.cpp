#include "../../LibertyRecomp/diagnostics/performance_tiers.h"
#include <cassert>
#include <iostream>

using namespace LibertyRecomp::Diagnostics;

void TestMinimumExperimentalTier() {
  HardwareReport a14_report;
  a14_report.cpu_architecture = CpuArchitecture::Arm64;
  a14_report.gpu_vendor = GpuVendor::ArmMali;
  a14_report.gpu_name = "Mali-G57 MC2";
  a14_report.memory.physical_ram_bytes = 4ULL * 1024ULL * 1024ULL * 1024ULL;
  a14_report.memory.conservative_budget_bytes = 1536ULL * 1024ULL * 1024ULL;

  GraphicsQualitySettings settings = GetTunedSettingsForTier(HardwareTier::MinimumExperimental, a14_report);
  assert(settings.internal_height == 540);
  assert(settings.dynamic_resolution == true);
  assert(settings.min_dynamic_height == 480);
  assert(settings.texture_quality == 0);
  assert(settings.shadow_quality == 0);
  assert(settings.reflection_quality == 0);
  assert(settings.enable_fsr == true);
  assert(settings.streaming_worker_count == 1);
  assert(settings.target_fps == 30);
  assert(settings.texture_memory_budget_bytes <= (1536ULL * 1024ULL * 1024ULL * 4) / 10);

  std::cout << "[PASS] Minimum/Experimental Tier Settings" << std::endl;
}

void TestMainstreamTier() {
  HardwareReport mainstream_report;
  mainstream_report.cpu_architecture = CpuArchitecture::Arm64;
  mainstream_report.gpu_vendor = GpuVendor::ArmMali;
  mainstream_report.gpu_name = "Mali-G78";
  mainstream_report.memory.physical_ram_bytes = 6ULL * 1024ULL * 1024ULL * 1024ULL;
  mainstream_report.memory.conservative_budget_bytes = 2560ULL * 1024ULL * 1024ULL;

  GraphicsQualitySettings settings = GetTunedSettingsForTier(HardwareTier::Mainstream, mainstream_report);
  assert(settings.internal_height == 720);
  assert(settings.dynamic_resolution == true);
  assert(settings.texture_quality == 1);
  assert(settings.target_fps == 30);
  assert(settings.streaming_worker_count == 2);

  std::cout << "[PASS] Mainstream Tier Settings" << std::endl;
}

void TestHighPerformanceTier() {
  HardwareReport high_report;
  high_report.cpu_architecture = CpuArchitecture::AppleSilicon;
  high_report.gpu_vendor = GpuVendor::Apple;
  high_report.memory.physical_ram_bytes = 16ULL * 1024ULL * 1024ULL * 1024ULL;
  high_report.memory.conservative_budget_bytes = 4096ULL * 1024ULL * 1024ULL;

  GraphicsQualitySettings settings = GetTunedSettingsForTier(HardwareTier::HighPerformance, high_report);
  assert(settings.internal_height == 1080);
  assert(settings.dynamic_resolution == false);
  assert(settings.texture_quality == 2);
  assert(settings.target_fps == 60);
  assert(settings.streaming_worker_count == 4);

  std::cout << "[PASS] High Performance Tier Settings" << std::endl;
}

int main() {
  std::cout << "Running Performance Tier Unit Tests..." << std::endl;
  TestMinimumExperimentalTier();
  TestMainstreamTier();
  TestHighPerformanceTier();
  std::cout << "All Performance Tier Unit Tests Passed Successfully!" << std::endl;
  return 0;
}
