#include "universal_runtime.h"
#include <filesystem>
#include <sstream>

namespace LibertyRecomp {

UniversalRuntime& UniversalRuntime::GetInstance() {
  static UniversalRuntime instance;
  return instance;
}

bool UniversalRuntime::Initialize(const std::string& storage_path) {
  status_.game_data_directory = storage_path;

  // 1. Probe hardware capabilities
  status_.hardware_report = Diagnostics::DetectHostHardwareCapabilities();
  status_.active_graphics_settings = Diagnostics::GetTunedSettingsForTier(
      status_.hardware_report.recommended_tier, status_.hardware_report);
  status_.gpu_workarounds = Gpu::EvaluateGpuWorkarounds(status_.hardware_report);

  status_.state = RuntimeInitState::HardwareProbed;
  status_.status_message = "Hardware capabilities probed. Tier: " +
      std::string(status_.hardware_report.recommended_tier == Diagnostics::HardwareTier::MinimumExperimental ? "Minimum (A14 5G 540p)" :
                  status_.hardware_report.recommended_tier == Diagnostics::HardwareTier::Mainstream ? "Mainstream (720p)" : "High Performance (1080p)");

  // Initialize pipeline cache directory
  std::string cache_dir = storage_path + "/shader_cache";
  pipeline_cache_ = std::make_unique<Gpu::PipelineCacheManager>(cache_dir);
  pipeline_cache_->InitializeCache();

  // 2. Initialize Vulkan
  if (!AttemptVulkanInitialization()) {
    return false;
  }

  // 3. Inspect game data path
  if (!VerifyGameData(storage_path + "/game")) {
    status_.state = RuntimeInitState::GameDataNotFound;
    status_.status_message =
        "GTA IV game data is missing or cannot yet be verified. The installer must parse XEX metadata and validate the matching Title Update 8 before launch.";
    return true; // Non-fatal for engine startup: launcher handles installation
  }

  // 4. Start Engine Execution when game data is confirmed
  return StartEngineExecution();
}

bool UniversalRuntime::AttemptVulkanInitialization() {
  if (status_.hardware_report.vulkan.is_available) {
    // Capability probing is not Vulkan initialization. The actual instance,
    // physical-device, logical-device, and swapchain setup must be supplied by
    // the graphics backend before this runtime can claim VulkanInitialized.
    status_.state = RuntimeInitState::VulkanCapabilityDetected;
    status_.status_message =
        "Vulkan support detected, but the Vulkan device/backend is not integrated yet.";
    status_.last_error_message = status_.status_message;
    return false;
  }

  // Hardware-specific workaround configuration
  if (status_.hardware_report.gpu_vendor == Diagnostics::GpuVendor::ArmMali) {
    status_.gpu_workarounds.mali_clip_distance_workaround = true;
    status_.gpu_workarounds.mali_subpass_load_fallback = true;
  }

  status_.state = RuntimeInitState::FatalError;
  status_.status_message = "Vulkan is unavailable on this device; runtime initialization cannot continue.";
  status_.last_error_message = status_.status_message;
  return false;
}

bool UniversalRuntime::VerifyGameData(const std::string& path) {
  std::error_code ec;
  if (!std::filesystem::exists(path, ec)) {
    return false;
  }

  std::string default_xex = path + "/default.xex";
  if (!std::filesystem::exists(default_xex, ec)) {
    return false;
  }

  // Do not validate a user's image using hard-coded Title ID / Media ID values.
  // Those values must be parsed from the selected XEX and checked against the
  // installed TU8 metadata before the game data can be trusted.
  status_.status_message =
      "GTA IV default.xex was found, but XEX metadata and Title Update 8 validation are not integrated yet.";
  status_.last_error_message = status_.status_message;
  return false;
}

bool UniversalRuntime::StartEngineExecution() {
  status_.state = RuntimeInitState::FatalError;
  status_.is_engine_ready = false;
  status_.is_game_code_executing = false;
  status_.status_message =
      "GTA IV cannot start: the ReXGlue runtime and generated PowerPC game entry point are not linked into this build.";
  status_.last_error_message = status_.status_message;
  return false;
}

void UniversalRuntime::Shutdown() {
  if (pipeline_cache_) {
    pipeline_cache_->SaveCache({});
  }
  status_.is_game_code_executing = false;
  status_.is_engine_ready = false;
  status_.state = RuntimeInitState::Uninitialized;
}

} // namespace LibertyRecomp
