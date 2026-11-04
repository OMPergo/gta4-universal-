#include "performance_tiers.h"
#include <algorithm>

namespace LibertyRecomp::Diagnostics {

GraphicsQualitySettings GetTunedSettingsForTier(HardwareTier tier, const HardwareReport& report) {
  GraphicsQualitySettings settings;

  switch (tier) {
    case HardwareTier::MinimumExperimental: {
      // Configuration tailored specifically for targets like Samsung Galaxy A14 5G
      // (Dimensity 700 / Mali-G57 MC2 / 4GB RAM)
      settings.internal_width = 960;
      settings.internal_height = 540;
      settings.dynamic_resolution = true;
      settings.min_dynamic_height = 480;
      settings.max_dynamic_height = 540;

      // Restrict texture budget to safe headroom (~350MB) within the 1.5GB total app budget
      settings.texture_memory_budget_bytes = 384ULL * 1024ULL * 1024ULL;
      settings.texture_quality = 0; // Low

      settings.draw_distance_scale = 0.6f;
      settings.traffic_density_scale = 0.5f;
      settings.pedestrian_density_scale = 0.5f;

      settings.shadow_quality = 0; // Low
      settings.reflection_quality = 0; // Half/Quarter resolution
      settings.enable_fsr = true; // Use FSR1 upscaling from 540p to native display
      settings.fsr_sharpness = 0.6f;

      // Avoid CPU saturation on entry octa-core (2 Cortex-A76 + 6 Cortex-A55)
      settings.streaming_worker_count = 1;
      settings.thermal_throttling_aware = true;
      settings.target_fps = 30;
      break;
    }

    case HardwareTier::Mainstream: {
      settings.internal_width = 1280;
      settings.internal_height = 720;
      settings.dynamic_resolution = true;
      settings.min_dynamic_height = 540;
      settings.max_dynamic_height = 720;

      settings.texture_memory_budget_bytes = 768ULL * 1024ULL * 1024ULL;
      settings.texture_quality = 1; // Medium

      settings.draw_distance_scale = 1.0f;
      settings.traffic_density_scale = 0.8f;
      settings.pedestrian_density_scale = 0.8f;

      settings.shadow_quality = 1; // Medium
      settings.reflection_quality = 1; // Medium
      settings.enable_fsr = true;
      settings.fsr_sharpness = 0.5f;

      settings.streaming_worker_count = 2;
      settings.thermal_throttling_aware = true;
      settings.target_fps = 30;
      break;
    }

    case HardwareTier::HighPerformance: {
      settings.internal_width = 1920;
      settings.internal_height = 1080;
      settings.dynamic_resolution = false;
      settings.min_dynamic_height = 720;
      settings.max_dynamic_height = 1080;

      settings.texture_memory_budget_bytes = 1536ULL * 1024ULL * 1024ULL;
      settings.texture_quality = 2; // High

      settings.draw_distance_scale = 1.2f;
      settings.traffic_density_scale = 1.0f;
      settings.pedestrian_density_scale = 1.0f;

      settings.shadow_quality = 2; // High
      settings.reflection_quality = 1; // Full
      settings.enable_fsr = false;
      settings.fsr_sharpness = 0.0f;

      settings.streaming_worker_count = 4;
      settings.thermal_throttling_aware = true;
      settings.target_fps = 60;
      break;
    }
  }

  // If memory budget is strictly constrained, ensure texture budget doesn't exceed 40% of total budget
  if (report.memory.conservative_budget_bytes > 0) {
    uint64_t max_texture_budget = (report.memory.conservative_budget_bytes * 4) / 10;
    settings.texture_memory_budget_bytes = std::min(settings.texture_memory_budget_bytes, max_texture_budget);
  }

  return settings;
}

} // namespace LibertyRecomp::Diagnostics
