#include "gpu_compatibility.h"
#include <fstream>
#include <filesystem>

namespace LibertyRecomp::Gpu {

GpuWorkaroundFlags EvaluateGpuWorkarounds(const Diagnostics::HardwareReport& report) {
  GpuWorkaroundFlags flags;

  if (report.gpu_vendor == Diagnostics::GpuVendor::ArmMali) {
    flags.mali_clip_distance_workaround = true;
    flags.mali_subpass_load_fallback = true;
    flags.mali_disable_depth_bounds = true;
    flags.enable_pipeline_cache = true;
    flags.aggressive_pipeline_prewarming = false;
  } else if (report.gpu_vendor == Diagnostics::GpuVendor::QualcommAdreno) {
    flags.adreno_use_system_vulkan_driver = true;
    flags.adreno_allow_turnip_experimental = true;
    flags.adreno_use_libadrenotools = true;
    flags.enable_pipeline_cache = true;
  } else if (report.gpu_vendor == Diagnostics::GpuVendor::SamsungXclipse) {
    flags.xclipse_strict_memory_barrier = true;
    flags.enable_pipeline_cache = true;
  } else if (report.gpu_vendor == Diagnostics::GpuVendor::Apple ||
             report.cpu_architecture == Diagnostics::CpuArchitecture::AppleSilicon) {
    flags.apple_use_metal_backend = true;
    flags.apple_use_moltenvk = false;
    flags.moltenvk_synchronization2_workaround = true;
  }

  return flags;
}

PipelineCacheManager::PipelineCacheManager(std::string cache_directory_path)
    : cache_path_(std::move(cache_directory_path)) {}

bool PipelineCacheManager::InitializeCache() {
  std::error_code ec;
  std::filesystem::path path(cache_path_);
  if (!std::filesystem::exists(path)) {
    if (!std::filesystem::create_directories(path, ec)) {
      return false;
    }
  }

  std::filesystem::path cache_file = path / "pipeline.bin";
  if (std::filesystem::exists(cache_file)) {
    cached_data_size_ = std::filesystem::file_size(cache_file, ec);
  } else {
    cached_data_size_ = 0;
  }
  return true;
}

bool PipelineCacheManager::SaveCache(const std::vector<uint8_t>& pipeline_data) {
  std::error_code ec;
  std::filesystem::path path(cache_path_);
  if (!std::filesystem::exists(path)) {
    std::filesystem::create_directories(path, ec);
  }

  std::filesystem::path cache_file = path / "pipeline.bin";
  std::ofstream out(cache_file, std::ios::binary | std::ios::trunc);
  if (!out.is_open()) {
    return false;
  }

  out.write(reinterpret_cast<const char*>(pipeline_data.data()), pipeline_data.size());
  if (out.good()) {
    cached_data_size_ = pipeline_data.size();
    return true;
  }
  return false;
}

std::vector<uint8_t> PipelineCacheManager::LoadCacheData() const {
  std::filesystem::path cache_file = std::filesystem::path(cache_path_) / "pipeline.bin";
  std::ifstream in(cache_file, std::ios::binary);
  if (!in.is_open()) {
    return {};
  }

  return std::vector<uint8_t>((std::istreambuf_iterator<char>(in)),
                              std::istreambuf_iterator<char>());
}

} // namespace LibertyRecomp::Gpu
