#include "LibertyRecomp/diagnostics/resource_budget.h"
#include <cstdint>
#include <iostream>
using namespace LibertyRecomp::Diagnostics;
static bool Check(bool ok, const char* message) {
  if (!ok) std::cerr << "[FAIL] " << message << '\n';
  return ok;
}
int main() {
  constexpr uint64_t mb = 1024ULL * 1024ULL;
  ResourceBudgetInputs input;
  input.process_budget_bytes = 1536 * mb;
  input.available_system_bytes = 1200 * mb;
  input.preferred_texture_budget_bytes = 384 * mb;
  input.preferred_streaming_budget_bytes = 192 * mb;
  input.preferred_decode_scratch_budget_bytes = 96 * mb;
  const auto plan = ComputeResourceBudgetPlan(input);
  bool ok = true;
  ok &= Check(plan.process_budget_bytes == 1200 * mb, "available memory tightens process ceiling");
  ok &= Check(plan.safety_reserve_bytes == 240 * mb, "20 percent safety reserve");
  ok &= Check(plan.texture_bytes <= 240 * mb, "texture cap");
  ok &= Check(plan.streaming_bytes <= 120 * mb, "streaming cap");
  ok &= Check(plan.decode_scratch_bytes <= 60 * mb, "decode cap");
  ok &= Check(plan.IsValid(), "plan stays within process ceiling");
  ok &= Check(!ComputeResourceBudgetPlan({}).IsValid(), "unknown budget is not valid");
  ResourceBudgetPlan overflow;
  overflow.process_budget_bytes = UINT64_MAX;
  overflow.safety_reserve_bytes = UINT64_MAX;
  overflow.texture_bytes = 1;
  ok &= Check(!overflow.IsValid(), "overflowing plan rejected");
  BudgetedResourcePool pool(100);
  ok &= Check(pool.TryReserve(60), "first reservation succeeds");
  ok &= Check(!pool.TryReserve(50), "over-budget reservation rejected");
  ok &= Check(pool.UsedBytes() == 60, "usage tracks reservation");
  pool.Release(20);
  ok &= Check(pool.UsedBytes() == 40, "release updates usage");
  pool.Release(1000);
  ok &= Check(pool.UsedBytes() == 0, "release saturates at zero");
  ok &= Check(!pool.TryReserve(101), "oversized reservation rejected");
  if (!ok) return 1;
  std::cout << "[PASS] Resource budget planning and accounting\n";
  return 0;
}
