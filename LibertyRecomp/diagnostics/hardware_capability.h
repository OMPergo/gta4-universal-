#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <optional>

namespace LibertyRecomp::Diagnostics {

enum class CpuArchitecture {
  Unknown = 0,
  X86,
  X86_64,
  Arm32,
  Arm64,
  AppleSilicon
};

struct SimdCapabilities {
  bool neon = false;
  bool sse = false;
  bool sse2 = false;
  bool sse3 = false;
  bool ssse3 = false;
  bool sse4_1 = false;
  bool sse4_2 = false;
  bool avx = false;
  bool avx2 = false;
  bool avx512 = false;
};

enum class GpuVendor {
  Unknown = 0,
  QualcommAdreno,
  ArmMali,
  ArmImmortalis,
  SamsungXclipse,
  ImaginationPowerVR,
  Apple,
  Nvidia,
  Amd,
  Intel,
  Other
};

struct MemoryBudget {
  uint64_t physical_ram_bytes = 0;
  uint64_t available_ram_bytes = 0;
  uint64_t conservative_budget_bytes = 0;
};

struct VulkanCapabilityInfo {
  bool is_available = false;
  uint32_t api_version = 0;
  uint32_t driver_version = 0;
  uint32_t vendor_id = 0;
  uint32_t device_id = 0;
  std::string device_name;
  std::string driver_name;
  std::vector<std::string> supported_extensions;

  bool HasExtension(std::string_view extension_name) const;
};

enum class HardwareTier {
  MinimumExperimental = 0, // e.g. Samsung Galaxy A14 5G (Dimensity 700 / Mali-G57 MC2 / 4GB RAM)
  Mainstream,
  HighPerformance
};

struct HardwareReport {
  CpuArchitecture cpu_architecture = CpuArchitecture::Unknown;
  std::string cpu_brand;
  uint32_t cpu_core_count = 0;
  SimdCapabilities simd;

  MemoryBudget memory;

  GpuVendor gpu_vendor = GpuVendor::Unknown;
  std::string gpu_name;
  std::string driver_version_string;
  VulkanCapabilityInfo vulkan;

  HardwareTier recommended_tier = HardwareTier::MinimumExperimental;
  bool is_detection_confident = false;

  std::string ToJsonString() const;
};

// Pure classification helpers for deterministic unit testing
CpuArchitecture ClassifyCpuArchitecture(std::string_view arch_str, bool is_apple = false);
GpuVendor ClassifyGpuVendor(uint32_t vendor_id, std::string_view renderer_or_device_name);
MemoryBudget ComputeMemoryBudget(uint64_t physical_ram_bytes, uint64_t available_ram_bytes);
HardwareTier DetermineHardwareTier(const HardwareReport& report);

// Platform probing function
HardwareReport DetectHostHardwareCapabilities();

} // namespace LibertyRecomp::Diagnostics
