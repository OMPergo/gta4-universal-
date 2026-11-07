#include "../../LibertyRecomp/diagnostics/resource_budget.h"
#include <cassert>
#include <cstdint>
#include <iostream>

using namespace LibertyRecomp::Diagnostics;

int main() {
  constexpr uint64_t mb = 1024ULL * 1024ULL;
  ResourceBudgetInputs a14;
  a14.process_budget_bytes = 1536 * mb;
  a14.available_system_bytes = 1200 * mb;
  a14.preferred_texture_budget_bytes = 384 * mb;
  a14.preferred_streaming_budget_bytes = 192 * mb;
  a14.preferred_decode_scratch_bytes = 96 * mb;

  const auto plan = ComputeResourceBudgetPlan(a14);
  assert(plan.process_budget_bytes == 1200 * mb);
  assert(plan.safety_reserve_bytes == 240 * mb);
  assert(plan.texture_bytes <= 240 * mb);
  assert(plan.streaming_bytes <= 120 * mb);
  assert(plan.decode_scratch_bytes <= 60 * mb);
  assert(plan.IsValid());

  ResourceBudgetInputs unknown;
  assert(ComputeResourceBudgetPlan(unknown).process_budget_bytes == 0);

  BudgetedResourcePool pool(100);
  assert(pool.TryReserve(60));
  assert(!pool.TryReserve(50));
  assert(pool.UsedBytes() == 60);
  pool.Release(20);
  assert(pool.UsedBytes() == 40);
  pool.Release(1000);
  assert(pool.UsedBytes() == 0);
  assert(!pool.TryReserve(101));

  std::cout << "[PASS] Adaptive resource budget planning and pool accounting" << std::endl;
  return 0;
}
