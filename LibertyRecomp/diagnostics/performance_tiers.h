#pragma once

#include "hardware_capability.h"
#include <cstdint>
#include <string>

namespace LibertyRecomp::Diagnostics {

struct GraphicsQualitySettings {
  uint32_t internal_width = 1280;
  uint32_t internal_height = 720;
  bool dynamic_resolution = false;
  uint32_t min_dynamic_height = 540;
  uint32_t max_dynamic_height = 720;

  // Resource budgets
  uint64_t texture_memory_budget_bytes = 512 * 1024 * 1024;
  uint64_t streaming_cache_budget_bytes = 192 * 1024 * 1024;
  uint64_t decode_scratch_budget_bytes = 96 * 1024 * 1024;
  uint32_t texture_quality = 1; // 0 = Low, 1 = Medium, 2 = High

  // World and rendering density
  float draw_distance_scale = 1.0f;
  float traffic_density_scale = 1.0f;
  float pedestrian_density_scale = 1.0f;

  // Effects
  uint32_t shadow_quality = 1; // 0 = Off/Low, 1 = Medium, 2 = High
  uint32_t reflection_quality = 1; // 0 = Quarter/Half, 1 = Full
  bool enable_fsr = false;
  float fsr_sharpness = 0.5f;

  // Streaming & threading
  uint32_t streaming_worker_count = 2;
  bool thermal_throttling_aware = true;
  uint32_t target_fps = 30;
};

// Generate tuned profile based on hardware tier and report
GraphicsQualitySettings GetTunedSettingsForTier(HardwareTier tier, const HardwareReport& report);

} // namespace LibertyRecomp::Diagnostics
