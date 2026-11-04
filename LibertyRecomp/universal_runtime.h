#pragma once

#include "diagnostics/hardware_capability.h"
#include "diagnostics/performance_tiers.h"
#include "gpu/gpu_compatibility.h"
#include "network/authoritative_session.h"
#include "installer/media_id_validator.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace LibertyRecomp {

enum class RuntimeInitState {
  Uninitialized = 0,
  HardwareProbed,
  VulkanInitialized,
  GameDataNotFound,
  GameDataValidated,
  RuntimeRunning,
  FatalError
};

struct RuntimeStatus {
  RuntimeInitState state = RuntimeInitState::Uninitialized;
  Diagnostics::HardwareReport hardware_report;
  Diagnostics::GraphicsQualitySettings active_graphics_settings;
  Gpu::GpuWorkaroundFlags gpu_workarounds;
  std::string game_data_directory;
  std::string status_message;
  std::string last_error_message;
};

class UniversalRuntime {
public:
  static UniversalRuntime& GetInstance();

  bool Initialize(const std::string& storage_path);
  bool VerifyGameData(const std::string& path);
  bool AttemptVulkanInitialization();
  void Shutdown();

  const RuntimeStatus& GetStatus() const { return status_; }

private:
  UniversalRuntime() = default;
  ~UniversalRuntime() = default;

  RuntimeStatus status_;
  std::unique_ptr<Gpu::PipelineCacheManager> pipeline_cache_;
};

} // namespace LibertyRecomp
