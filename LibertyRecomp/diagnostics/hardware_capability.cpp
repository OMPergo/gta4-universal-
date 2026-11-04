#include "hardware_capability.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <thread>
#include <cstring>
#include <fstream>

#if defined(__linux__) || defined(__ANDROID__)
#include <sys/sysinfo.h>
#include <unistd.h>
#if defined(__arm__) || defined(__aarch64__)
#include <sys/auxv.h>
#include <asm/hwcap.h>
#endif
#elif defined(__APPLE__)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

#if defined(HAVE_VULKAN)
#include <vulkan/vulkan.h>
#endif

namespace LibertyRecomp::Diagnostics {

static std::string ToLower(std::string_view sv) {
  std::string result;
  result.reserve(sv.size());
  for (char c : sv) {
    result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
  }
  return result;
}

bool VulkanCapabilityInfo::HasExtension(std::string_view extension_name) const {
  for (const auto& ext : supported_extensions) {
    if (ext == extension_name) {
      return true;
    }
  }
  return false;
}

CpuArchitecture ClassifyCpuArchitecture(std::string_view arch_str, bool is_apple) {
  std::string lower = ToLower(arch_str);
  if (is_apple) {
    if (lower.find("arm") != std::string_view::npos || lower.find("aarch64") != std::string_view::npos) {
      return CpuArchitecture::AppleSilicon;
    }
  }
  if (lower == "x86_64" || lower == "amd64" || lower == "x64") {
    return CpuArchitecture::X86_64;
  }
  if (lower == "x86" || lower == "i386" || lower == "i686") {
    return CpuArchitecture::X86;
  }
  if (lower == "aarch64" || lower == "arm64") {
    return is_apple ? CpuArchitecture::AppleSilicon : CpuArchitecture::Arm64;
  }
  if (lower.find("arm") != std::string_view::npos) {
    return CpuArchitecture::Arm32;
  }
  return CpuArchitecture::Unknown;
}

GpuVendor ClassifyGpuVendor(uint32_t vendor_id, std::string_view renderer_or_device_name) {
  // Standard PCI / Khronos Vendor IDs
  switch (vendor_id) {
    case 0x5143: return GpuVendor::QualcommAdreno;
    case 0x13B5: {
      std::string lower = ToLower(renderer_or_device_name);
      if (lower.find("immortalis") != std::string::npos) {
        return GpuVendor::ArmImmortalis;
      }
      return GpuVendor::ArmMali;
    }
    case 0x144D: return GpuVendor::SamsungXclipse;
    case 0x1010: return GpuVendor::ImaginationPowerVR;
    case 0x106B: return GpuVendor::Apple;
    case 0x10DE: return GpuVendor::Nvidia;
    case 0x1002: return GpuVendor::Amd;
    case 0x8086: return GpuVendor::Intel;
    default: break;
  }

  // String fallback
  std::string lower = ToLower(renderer_or_device_name);
  if (lower.empty()) {
    return GpuVendor::Unknown;
  }

  if (lower.find("adreno") != std::string::npos || lower.find("qualcomm") != std::string::npos) {
    return GpuVendor::QualcommAdreno;
  }
  if (lower.find("immortalis") != std::string::npos) {
    return GpuVendor::ArmImmortalis;
  }
  if (lower.find("mali") != std::string::npos) {
    return GpuVendor::ArmMali;
  }
  if (lower.find("xclipse") != std::string::npos) {
    return GpuVendor::SamsungXclipse;
  }
  if (lower.find("powervr") != std::string::npos || lower.find("rogue") != std::string::npos) {
    return GpuVendor::ImaginationPowerVR;
  }
  if (lower.find("apple") != std::string::npos) {
    return GpuVendor::Apple;
  }
  if (lower.find("nvidia") != std::string::npos || lower.find("geforce") != std::string::npos) {
    return GpuVendor::Nvidia;
  }
  if (lower.find("amd") != std::string::npos || lower.find("radeon") != std::string::npos) {
    return GpuVendor::Amd;
  }
  if (lower.find("intel") != std::string::npos) {
    return GpuVendor::Intel;
  }

  return GpuVendor::Other;
}

MemoryBudget ComputeMemoryBudget(uint64_t physical_ram_bytes, uint64_t available_ram_bytes) {
  MemoryBudget budget;
  budget.physical_ram_bytes = physical_ram_bytes;
  budget.available_ram_bytes = available_ram_bytes;

  if (physical_ram_bytes == 0) {
    budget.conservative_budget_bytes = 0;
    return budget;
  }

  const uint64_t four_gb = 4ULL * 1024ULL * 1024ULL * 1024ULL;
  const uint64_t six_gb = 6ULL * 1024ULL * 1024ULL * 1024ULL;

  if (physical_ram_bytes <= four_gb) {
    uint64_t max_allowable = 1536ULL * 1024ULL * 1024ULL;
    uint64_t avail_portion = available_ram_bytes > 0 ? (available_ram_bytes * 6) / 10 : max_allowable;
    budget.conservative_budget_bytes = std::min(max_allowable, avail_portion);
  } else if (physical_ram_bytes <= six_gb) {
    uint64_t max_allowable = 2560ULL * 1024ULL * 1024ULL;
    uint64_t avail_portion = available_ram_bytes > 0 ? (available_ram_bytes * 7) / 10 : max_allowable;
    budget.conservative_budget_bytes = std::min(max_allowable, avail_portion);
  } else {
    uint64_t max_allowable = 4096ULL * 1024ULL * 1024ULL;
    uint64_t avail_portion = available_ram_bytes > 0 ? (available_ram_bytes * 75) / 100 : max_allowable;
    budget.conservative_budget_bytes = std::min(max_allowable, avail_portion);
  }

  return budget;
}

HardwareTier DetermineHardwareTier(const HardwareReport& report) {
  const uint64_t four_gb = 4ULL * 1024ULL * 1024ULL * 1024ULL;
  const uint64_t six_gb = 6ULL * 1024ULL * 1024ULL * 1024ULL;

  if (report.memory.physical_ram_bytes > 0 && report.memory.physical_ram_bytes <= four_gb) {
    return HardwareTier::MinimumExperimental;
  }

  std::string lower_gpu = ToLower(report.gpu_name);
  if (report.gpu_vendor == GpuVendor::ArmMali) {
    if (lower_gpu.find("g57") != std::string::npos ||
        lower_gpu.find("g52") != std::string::npos ||
        lower_gpu.find("g51") != std::string::npos ||
        lower_gpu.find("g31") != std::string::npos) {
      return HardwareTier::MinimumExperimental;
    }
  }

  if (report.gpu_vendor == GpuVendor::QualcommAdreno) {
    if (lower_gpu.find("505") != std::string::npos ||
        lower_gpu.find("506") != std::string::npos ||
        lower_gpu.find("610") != std::string::npos ||
        lower_gpu.find("612") != std::string::npos) {
      return HardwareTier::MinimumExperimental;
    }
  }

  if (report.memory.physical_ram_bytes > 0 && report.memory.physical_ram_bytes <= six_gb) {
    return HardwareTier::Mainstream;
  }

  if (report.cpu_architecture == CpuArchitecture::AppleSilicon ||
      report.gpu_vendor == GpuVendor::Apple ||
      report.gpu_vendor == GpuVendor::Nvidia ||
      report.gpu_vendor == GpuVendor::Amd ||
      report.gpu_vendor == GpuVendor::ArmImmortalis) {
    return HardwareTier::HighPerformance;
  }

  return HardwareTier::Mainstream;
}

std::string HardwareReport::ToJsonString() const {
  std::ostringstream ss;
  ss << "{\n";
  ss << "  \"cpu\": {\n";
  ss << "    \"architecture\": \"" << [this]() {
    switch (cpu_architecture) {
      case CpuArchitecture::X86: return "x86";
      case CpuArchitecture::X86_64: return "x86-64";
      case CpuArchitecture::Arm32: return "ARM32";
      case CpuArchitecture::Arm64: return "ARM64";
      case CpuArchitecture::AppleSilicon: return "AppleSilicon";
      default: return "Unknown";
    }
  }() << "\",\n";
  ss << "    \"brand\": \"" << cpu_brand << "\",\n";
  ss << "    \"core_count\": " << cpu_core_count << ",\n";
  ss << "    \"simd\": {\n";
  ss << "      \"neon\": " << (simd.neon ? "true" : "false") << ",\n";
  ss << "      \"sse\": " << (simd.sse ? "true" : "false") << ",\n";
  ss << "      \"sse2\": " << (simd.sse2 ? "true" : "false") << ",\n";
  ss << "      \"sse3\": " << (simd.sse3 ? "true" : "false") << ",\n";
  ss << "      \"ssse3\": " << (simd.ssse3 ? "true" : "false") << ",\n";
  ss << "      \"sse4_1\": " << (simd.sse4_1 ? "true" : "false") << ",\n";
  ss << "      \"sse4_2\": " << (simd.sse4_2 ? "true" : "false") << ",\n";
  ss << "      \"avx\": " << (simd.avx ? "true" : "false") << ",\n";
  ss << "      \"avx2\": " << (simd.avx2 ? "true" : "false") << ",\n";
  ss << "      \"avx512\": " << (simd.avx512 ? "true" : "false") << "\n";
  ss << "    }\n";
  ss << "  },\n";
  ss << "  \"memory\": {\n";
  ss << "    \"physical_ram_bytes\": " << memory.physical_ram_bytes << ",\n";
  ss << "    \"available_ram_bytes\": " << memory.available_ram_bytes << ",\n";
  ss << "    \"conservative_budget_bytes\": " << memory.conservative_budget_bytes << "\n";
  ss << "  },\n";
  ss << "  \"gpu\": {\n";
  ss << "    \"vendor\": \"" << [this]() {
    switch (gpu_vendor) {
      case GpuVendor::QualcommAdreno: return "Qualcomm Adreno";
      case GpuVendor::ArmMali: return "ARM Mali";
      case GpuVendor::ArmImmortalis: return "ARM Immortalis";
      case GpuVendor::SamsungXclipse: return "Samsung Xclipse";
      case GpuVendor::ImaginationPowerVR: return "Imagination PowerVR";
      case GpuVendor::Apple: return "Apple";
      case GpuVendor::Nvidia: return "NVIDIA";
      case GpuVendor::Amd: return "AMD";
      case GpuVendor::Intel: return "Intel";
      case GpuVendor::Other: return "Other";
      default: return "Unknown";
    }
  }() << "\",\n";
  ss << "    \"name\": \"" << gpu_name << "\",\n";
  ss << "    \"driver_version\": \"" << driver_version_string << "\",\n";
  ss << "    \"vulkan\": {\n";
  ss << "      \"available\": " << (vulkan.is_available ? "true" : "false") << ",\n";
  ss << "      \"api_version\": " << vulkan.api_version << ",\n";
  ss << "      \"vendor_id\": " << vulkan.vendor_id << ",\n";
  ss << "      \"device_id\": " << vulkan.device_id << ",\n";
  ss << "      \"extension_count\": " << vulkan.supported_extensions.size() << "\n";
  ss << "    }\n";
  ss << "  },\n";
  ss << "  \"recommended_tier\": \"" << [this]() {
    switch (recommended_tier) {
      case HardwareTier::MinimumExperimental: return "MinimumExperimental";
      case HardwareTier::Mainstream: return "Mainstream";
      case HardwareTier::HighPerformance: return "HighPerformance";
    }
    return "MinimumExperimental";
  }() << "\",\n";
  ss << "  \"detection_confident\": " << (is_detection_confident ? "true" : "false") << "\n";
  ss << "}\n";
  return ss.str();
}

static void ProbeVulkanAndGpu(HardwareReport& report) {
#if defined(HAVE_VULKAN)
  VkInstance instance = VK_NULL_HANDLE;
  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "LibertyCityUniversal";
  app_info.apiVersion = VK_API_VERSION_1_1;

  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  if (vkCreateInstance(&create_info, nullptr, &instance) == VK_SUCCESS) {
    uint32_t device_count = 0;
    if (vkEnumeratePhysicalDevices(instance, &device_count, nullptr) == VK_SUCCESS && device_count > 0) {
      std::vector<VkPhysicalDevice> devices(device_count);
      vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

      VkPhysicalDeviceProperties props;
      vkGetPhysicalDeviceProperties(devices[0], &props);

      report.vulkan.is_available = true;
      report.vulkan.api_version = props.apiVersion;
      report.vulkan.driver_version = props.driverVersion;
      report.vulkan.vendor_id = props.vendorID;
      report.vulkan.device_id = props.deviceID;
      report.vulkan.device_name = props.deviceName;

      std::ostringstream ss;
      ss << (props.driverVersion >> 22) << "." << ((props.driverVersion >> 12) & 0x3FF) << "." << (props.driverVersion & 0xFFF);
      report.vulkan.driver_name = ss.str();

      uint32_t ext_count = 0;
      if (vkEnumerateDeviceExtensionProperties(devices[0], nullptr, &ext_count, nullptr) == VK_SUCCESS && ext_count > 0) {
        std::vector<VkExtensionProperties> exts(ext_count);
        vkEnumerateDeviceExtensionProperties(devices[0], nullptr, &ext_count, exts.data());
        for (const auto& ext : exts) {
          report.vulkan.supported_extensions.push_back(ext.extensionName);
        }
      }

      report.gpu_vendor = ClassifyGpuVendor(props.vendorID, props.deviceName);
      report.gpu_name = props.deviceName;
      report.driver_version_string = ss.str();
    }
    vkDestroyInstance(instance, nullptr);
  }
#else
  // Fallback probing via /proc or system info when Vulkan is not linked or unavailable
#if defined(__linux__) || defined(__ANDROID__)
  std::ifstream f("/sys/class/drm/card0/device/vendor");
  if (f.is_open()) {
    std::string line;
    if (std::getline(f, line)) {
      try {
        uint32_t vid = std::stoul(line, nullptr, 16);
        report.gpu_vendor = ClassifyGpuVendor(vid, "");
      } catch (...) {}
    }
  }
#elif defined(__APPLE__)
  report.gpu_vendor = GpuVendor::Apple;
  report.gpu_name = "Apple GPU";
#endif
#endif
}

HardwareReport DetectHostHardwareCapabilities() {
  HardwareReport report;
  report.cpu_core_count = std::thread::hardware_concurrency();

#if defined(__x86_64__) || defined(_M_X64)
  report.cpu_architecture = CpuArchitecture::X86_64;
  report.cpu_brand = "x86-64 Processor";
  report.simd.sse = true;
  report.simd.sse2 = true;
#if defined(__SSE3__)
  report.simd.sse3 = true;
#endif
#if defined(__SSSE3__)
  report.simd.ssse3 = true;
#endif
#if defined(__SSE4_1__)
  report.simd.sse4_1 = true;
#endif
#if defined(__SSE4_2__)
  report.simd.sse4_2 = true;
#endif
#if defined(__AVX__)
  report.simd.avx = true;
#endif
#if defined(__AVX2__)
  report.simd.avx2 = true;
#endif
#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__APPLE__)
  report.cpu_architecture = CpuArchitecture::AppleSilicon;
  report.cpu_brand = "Apple Silicon";
#else
  report.cpu_architecture = CpuArchitecture::Arm64;
  report.cpu_brand = "ARM64 Processor";
#endif
  report.simd.neon = true;
#elif defined(__arm__) || defined(_M_ARM)
  report.cpu_architecture = CpuArchitecture::Arm32;
  report.cpu_brand = "ARM32 Processor";
#elif defined(__i386__) || defined(_M_IX86)
  report.cpu_architecture = CpuArchitecture::X86;
  report.cpu_brand = "x86 Processor";
#else
  report.cpu_architecture = CpuArchitecture::Unknown;
#endif

  // Memory detection
#if defined(__linux__) || defined(__ANDROID__)
  struct sysinfo si;
  if (sysinfo(&si) == 0) {
    uint64_t total = static_cast<uint64_t>(si.totalram) * si.mem_unit;
    uint64_t free_ram = static_cast<uint64_t>(si.freeram) * si.mem_unit;
    report.memory = ComputeMemoryBudget(total, free_ram);
    report.is_detection_confident = true;
  }
#elif defined(__APPLE__)
  int mib[2] = {CTL_HW, HW_MEMSIZE};
  uint64_t total = 0;
  size_t length = sizeof(total);
  if (sysctl(mib, 2, &total, &length, NULL, 0) == 0) {
    report.memory = ComputeMemoryBudget(total, total / 2);
    report.is_detection_confident = true;
  }
#elif defined(_WIN32)
  MEMORYSTATUSEX mem_status;
  mem_status.dwLength = sizeof(mem_status);
  if (GlobalMemoryStatusEx(&mem_status)) {
    report.memory = ComputeMemoryBudget(mem_status.ullTotalPhys, mem_status.ullAvailPhys);
    report.is_detection_confident = true;
  }
#endif

  ProbeVulkanAndGpu(report);
  report.recommended_tier = DetermineHardwareTier(report);
  return report;
}

} // namespace LibertyRecomp::Diagnostics
