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
  EngineInitializing,
  GameCodeExecuting,
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
  bool is_engine_ready = false;
  bool is_game_code_executing = false;
};

class UniversalRuntime {
public:
  static UniversalRuntime& GetInstance();

  bool Initialize(const std::string& storage_path);
  bool VerifyGameData(const std::string& path);
  bool AttemptVulkanInitialization();
  bool StartEngineExecution();
  void Shutdown();

  const RuntimeStatus& GetStatus() const { return status_; }

private:
  UniversalRuntime() = default;
  ~UniversalRuntime() = default;

  RuntimeStatus status_;
  std::unique_ptr<Gpu::PipelineCacheManager> pipeline_cache_;
};

} // namespace LibertyRecomp
