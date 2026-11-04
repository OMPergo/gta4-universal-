#include "../../LibertyRecomp/diagnostics/hardware_capability.h"
#include "../../LibertyRecomp/diagnostics/performance_tiers.h"
#include "../../LibertyRecomp/gpu/gpu_compatibility.h"
#include "../../LibertyRecomp/network/authoritative_session.h"
#include "../../installer/media_id_validator.h"
#include "../../LibertyRecomp/universal_runtime.h"

#include <iostream>
#include <cassert>

using namespace LibertyRecomp;

void TestEngineExecutionLifecycle() {
  UniversalRuntime& runtime = UniversalRuntime::GetInstance();
  assert(runtime.Initialize("/tmp/test_gta4_runtime") == true);
  const auto& status = runtime.GetStatus();
  assert(status.state == RuntimeInitState::GameDataNotFound);
  assert(status.status_message.find("Legally obtained GTA IV") != std::string::npos);

  // Test execution state transition
  assert(runtime.StartEngineExecution() == true);
  assert(runtime.GetStatus().state == RuntimeInitState::GameCodeExecuting);
  assert(runtime.GetStatus().is_game_code_executing == true);
  runtime.Shutdown();
  assert(runtime.GetStatus().state == RuntimeInitState::Uninitialized);
  std::cout << "[PASS] Engine Execution Lifecycle & Game Data Verification" << std::endl;
}

int main() {
  std::cout << "========================================" << std::endl;
  std::cout << "LIBERTY CITY UNIVERSAL INTEGRATED TEST SUITE" << std::endl;
  std::cout << "========================================" << std::endl;

  // 1. Hardware Capability
  Diagnostics::HardwareReport report = Diagnostics::DetectHostHardwareCapabilities();
  assert(report.cpu_architecture != Diagnostics::CpuArchitecture::Unknown);
  std::cout << "[PASS] Hardware Capability Probing (Detected: "
            << (report.cpu_architecture == Diagnostics::CpuArchitecture::X86_64 ? "x86-64" : "ARM64")
            << ", RAM: " << (report.memory.physical_ram_bytes / (1024 * 1024)) << " MB)" << std::endl;

  // 2. Performance Tiering (Galaxy A14 5G Verification)
  Diagnostics::HardwareReport a14;
  a14.cpu_architecture = Diagnostics::CpuArchitecture::Arm64;
  a14.gpu_vendor = Diagnostics::GpuVendor::ArmMali;
  a14.gpu_name = "Mali-G57 MC2";
  a14.memory.physical_ram_bytes = 4ULL * 1024ULL * 1024ULL * 1024ULL;
  a14.memory.conservative_budget_bytes = 1536ULL * 1024ULL * 1024ULL;

  Diagnostics::GraphicsQualitySettings a14_settings = Diagnostics::GetTunedSettingsForTier(Diagnostics::HardwareTier::MinimumExperimental, a14);
  assert(a14_settings.internal_height == 540);
  assert(a14_settings.enable_fsr == true);
  assert(a14_settings.streaming_worker_count == 1);
  std::cout << "[PASS] Low-End Tier Settings (Samsung Galaxy A14 5G 540p FSR profile validated)" << std::endl;

  // 3. GPU Workarounds
  Gpu::GpuWorkaroundFlags mali_flags = Gpu::EvaluateGpuWorkarounds(a14);
  assert(mali_flags.mali_clip_distance_workaround == true);
  std::cout << "[PASS] GPU Workaround Evaluation (Mali clip-distance active)" << std::endl;

  // 4. Authoritative Multiplayer Session
  Network::AuthoritativeSession session(true, 0x1122334455667788ULL, "Niko_Host");
  assert(session.IsHost() == true);
  assert(session.AddGuestPeer(1, 0x9988776655443322ULL, "GuestPedestrian") == true);
  assert(session.CanPeerTriggerMissionScript(0) == true);
  assert(session.CanPeerTriggerMissionScript(1) == false);
  std::cout << "[PASS] Authoritative Multiplayer (Host controls Niko, Guest script isolated)" << std::endl;

  // 5. Media ID Validator
  Installer::ValidationResult usa_res = Installer::ValidateGta4Media(0x545407F2, 0x6AC07221);
  assert(usa_res.status == Installer::MediaIdValidationStatus::ValidSupportedUsaTU8);

  Installer::ValidationResult alt_res = Installer::ValidateGta4Media(0x545407F2, 0x0675959C, "06759S9C");
  assert(alt_res.status == Installer::MediaIdValidationStatus::AlternativeLegitimateMediaIdPendingTU8);
  std::cout << "[PASS] Media ID Validator (USA 6AC07221 & Alt 06759S9C handled)" << std::endl;

  // 6. Engine Execution Lifecycle
  TestEngineExecutionLifecycle();

  std::cout << "========================================" << std::endl;
  std::cout << "ALL 6 SUBSYSTEM TEST SUITES PASSED (100% SUCCESS)" << std::endl;
  std::cout << "========================================" << std::endl;
  return 0;
}
