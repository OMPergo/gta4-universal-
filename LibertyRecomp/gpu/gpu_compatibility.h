#pragma once

#include "../diagnostics/hardware_capability.h"
#include <string>
#include <vector>
#include <cstdint>

namespace LibertyRecomp::Gpu {

struct GpuWorkaroundFlags {
  // ARM Mali workarounds
  bool mali_clip_distance_workaround = false;
  bool mali_subpass_load_fallback = false;
  bool mali_disable_depth_bounds = false;

  // Qualcomm Adreno & Turnip
  bool adreno_use_system_vulkan_driver = true;
  bool adreno_allow_turnip_experimental = false;
  bool adreno_use_libadrenotools = false;
  std::string adreno_turnip_custom_driver_path;

  // Samsung Xclipse
  bool xclipse_strict_memory_barrier = false;

  // Apple & MoltenVK
  bool apple_use_metal_backend = false;
  bool apple_use_moltenvk = false;
  bool moltenvk_synchronization2_workaround = false;

  // General pipeline & shader cache
  bool enable_pipeline_cache = true;
  bool aggressive_pipeline_prewarming = false;
};

// Evaluate necessary workarounds based on hardware capability report
GpuWorkaroundFlags EvaluateGpuWorkarounds(const Diagnostics::HardwareReport& report);

class PipelineCacheManager {
public:
  explicit PipelineCacheManager(std::string cache_directory_path);
  bool InitializeCache();
  bool SaveCache(const std::vector<uint8_t>& pipeline_data);
  std::vector<uint8_t> LoadCacheData() const;
  size_t GetCachedPipelineDataSize() const { return cached_data_size_; }

private:
  std::string cache_path_;
  size_t cached_data_size_ = 0;
};

} // namespace LibertyRecomp::Gpu
