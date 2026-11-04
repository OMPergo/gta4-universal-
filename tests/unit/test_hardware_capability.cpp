#include "../../LibertyRecomp/diagnostics/hardware_capability.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace LibertyRecomp::Diagnostics;

void TestCpuClassification() {
  assert(ClassifyCpuArchitecture("x86_64") == CpuArchitecture::X86_64);
  assert(ClassifyCpuArchitecture("amd64") == CpuArchitecture::X86_64);
  assert(ClassifyCpuArchitecture("x86") == CpuArchitecture::X86);
  assert(ClassifyCpuArchitecture("i686") == CpuArchitecture::X86);
  assert(ClassifyCpuArchitecture("aarch64", false) == CpuArchitecture::Arm64);
  assert(ClassifyCpuArchitecture("arm64", true) == CpuArchitecture::AppleSilicon);
  assert(ClassifyCpuArchitecture("armv7l") == CpuArchitecture::Arm32);
  assert(ClassifyCpuArchitecture("unknown_arch") == CpuArchitecture::Unknown);
  std::cout << "[PASS] CPU Architecture Classification" << std::endl;
}

void TestGpuClassification() {
  // Vendor ID tests
  assert(ClassifyGpuVendor(0x5143, "") == GpuVendor::QualcommAdreno);
  assert(ClassifyGpuVendor(0x13B5, "Mali-G57") == GpuVendor::ArmMali);
  assert(ClassifyGpuVendor(0x13B5, "Immortalis-G715") == GpuVendor::ArmImmortalis);
  assert(ClassifyGpuVendor(0x144D, "") == GpuVendor::SamsungXclipse);
  assert(ClassifyGpuVendor(0x1010, "") == GpuVendor::ImaginationPowerVR);
  assert(ClassifyGpuVendor(0x106B, "") == GpuVendor::Apple);
  assert(ClassifyGpuVendor(0x10DE, "") == GpuVendor::Nvidia);
  assert(ClassifyGpuVendor(0x1002, "") == GpuVendor::Amd);
  assert(ClassifyGpuVendor(0x8086, "") == GpuVendor::Intel);

  // String fallback tests
  assert(ClassifyGpuVendor(0, "Adreno (TM) 619") == GpuVendor::QualcommAdreno);
  assert(ClassifyGpuVendor(0, "Mali-G57 MC2") == GpuVendor::ArmMali);
  assert(ClassifyGpuVendor(0, "Samsung Xclipse 920") == GpuVendor::SamsungXclipse);
  assert(ClassifyGpuVendor(0, "PowerVR Rogue GE8320") == GpuVendor::ImaginationPowerVR);
  assert(ClassifyGpuVendor(0, "Apple M2 Max") == GpuVendor::Apple);
  assert(ClassifyGpuVendor(0, "Some Unknown GPU") == GpuVendor::Other);
  assert(ClassifyGpuVendor(0, "") == GpuVendor::Unknown);
  std::cout << "[PASS] GPU Vendor Classification" << std::endl;
}

void TestMemoryBudgetCalculation() {
  // Low-end test (e.g., Samsung Galaxy A14 5G with 4GB RAM)
  const uint64_t four_gb = 4ULL * 1024ULL * 1024ULL * 1024ULL;
  const uint64_t two_gb = 2ULL * 1024ULL * 1024ULL * 1024ULL;
  auto budget_4g = ComputeMemoryBudget(four_gb, two_gb);
  assert(budget_4g.physical_ram_bytes == four_gb);
  assert(budget_4g.conservative_budget_bytes <= 1536ULL * 1024ULL * 1024ULL);
  assert(budget_4g.conservative_budget_bytes > 0);

  // High-end test (16GB RAM)
  const uint64_t sixteen_gb = 16ULL * 1024ULL * 1024ULL * 1024ULL;
  const uint64_t eight_gb = 8ULL * 1024ULL * 1024ULL * 1024ULL;
  auto budget_16g = ComputeMemoryBudget(sixteen_gb, eight_gb);
  assert(budget_16g.conservative_budget_bytes <= 4096ULL * 1024ULL * 1024ULL);

  // Zero-RAM edge case
  auto budget_zero = ComputeMemoryBudget(0, 0);
  assert(budget_zero.conservative_budget_bytes == 0);

  std::cout << "[PASS] Memory Budget Calculation" << std::endl;
}

void TestHardwareTierDetermination() {
  // Target: Samsung Galaxy A14 5G profile
  HardwareReport a14_report;
  a14_report.cpu_architecture = CpuArchitecture::Arm64;
  a14_report.gpu_vendor = GpuVendor::ArmMali;
  a14_report.gpu_name = "Mali-G57 MC2";
  a14_report.memory.physical_ram_bytes = 4ULL * 1024ULL * 1024ULL * 1024ULL;
  assert(DetermineHardwareTier(a14_report) == HardwareTier::MinimumExperimental);

  // Target: Mainstream profile (6GB RAM, Mali-G78)
  HardwareReport mainstream_report;
  mainstream_report.cpu_architecture = CpuArchitecture::Arm64;
  mainstream_report.gpu_vendor = GpuVendor::ArmMali;
  mainstream_report.gpu_name = "Mali-G78";
  mainstream_report.memory.physical_ram_bytes = 6ULL * 1024ULL * 1024ULL * 1024ULL;
  assert(DetermineHardwareTier(mainstream_report) == HardwareTier::Mainstream);

  // Target: Apple Silicon High-End profile
  HardwareReport apple_report;
  apple_report.cpu_architecture = CpuArchitecture::AppleSilicon;
  apple_report.gpu_vendor = GpuVendor::Apple;
  apple_report.memory.physical_ram_bytes = 16ULL * 1024ULL * 1024ULL * 1024ULL;
  assert(DetermineHardwareTier(apple_report) == HardwareTier::HighPerformance);

  std::cout << "[PASS] Hardware Tier Determination" << std::endl;
}

void TestVulkanExtensionLookup() {
  VulkanCapabilityInfo vk;
  vk.is_available = true;
  vk.supported_extensions = {"VK_KHR_swapchain", "VK_EXT_custom_border_color"};
  assert(vk.HasExtension("VK_KHR_swapchain"));
  assert(vk.HasExtension("VK_EXT_custom_border_color"));
  assert(!vk.HasExtension("VK_KHR_ray_tracing"));
  std::cout << "[PASS] Vulkan Extension Lookup" << std::endl;
}

void TestHostProbing() {
  HardwareReport report = DetectHostHardwareCapabilities();
  assert(report.cpu_architecture != CpuArchitecture::Unknown);
  assert(report.cpu_core_count > 0);
  std::string json = report.ToJsonString();
  assert(!json.empty());
  assert(json.find("\"cpu\"") != std::string::npos);
  assert(json.find("\"memory\"") != std::string::npos);
  std::cout << "[PASS] Host Capability Probing" << std::endl;
  std::cout << "Detected Host Report:\n" << json << std::endl;
}

int main() {
  std::cout << "Running Hardware Capability Unit Tests..." << std::endl;
  TestCpuClassification();
  TestGpuClassification();
  TestMemoryBudgetCalculation();
  TestHardwareTierDetermination();
  TestVulkanExtensionLookup();
  TestHostProbing();
  std::cout << "All Hardware Capability Unit Tests Passed Successfully!" << std::endl;
  return 0;
}
