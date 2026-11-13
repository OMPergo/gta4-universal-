#include "resource_budget.h"
#include <algorithm>
#include <limits>
namespace LibertyRecomp::Diagnostics {
uint64_t ResourceBudgetPlan::TotalAssignedBytes() const {
  uint64_t total = 0;
  const uint64_t parts[] = {safety_reserve_bytes, texture_bytes, streaming_bytes,
                            decode_scratch_bytes, runtime_bytes};
  for (uint64_t part : parts) {
    if (part > std::numeric_limits<uint64_t>::max() - total)
      return std::numeric_limits<uint64_t>::max();
    total += part;
  }
  return total;
}
bool ResourceBudgetPlan::IsValid() const {
  if (process_budget_bytes == 0) return false;
  uint64_t total = 0;
  const uint64_t parts[] = {safety_reserve_bytes, texture_bytes, streaming_bytes,
                            decode_scratch_bytes, runtime_bytes};
  for (uint64_t part : parts) {
    if (part > process_budget_bytes - total) return false;
    total += part;
  }
  return true;
}
ResourceBudgetPlan ComputeResourceBudgetPlan(const ResourceBudgetInputs& inputs) {
  ResourceBudgetPlan plan;
  if (inputs.process_budget_bytes == 0) return plan;
  uint64_t ceiling = inputs.process_budget_bytes;
  if (inputs.available_system_bytes > 0)
    ceiling = std::min(ceiling, inputs.available_system_bytes);
  plan.process_budget_bytes = ceiling;
  plan.safety_reserve_bytes = ceiling / 5;
  const uint64_t usable = ceiling - plan.safety_reserve_bytes;
  plan.texture_bytes = std::min(inputs.preferred_texture_budget_bytes, usable / 4);
  plan.streaming_bytes = std::min(inputs.preferred_streaming_budget_bytes, usable / 8);
  plan.decode_scratch_bytes = std::min(inputs.preferred_decode_scratch_bytes, usable / 16);
  plan.runtime_bytes = usable - plan.texture_bytes - plan.streaming_bytes -
                       plan.decode_scratch_bytes;
  return plan;
}
bool BudgetedResourcePool::TryReserve(uint64_t bytes) {
  uint64_t current = used_bytes_.load(std::memory_order_relaxed);
  for (;;) {
    if (bytes > limit_bytes_ || current > limit_bytes_ - bytes) return false;
    if (used_bytes_.compare_exchange_weak(current, current + bytes,
          std::memory_order_acq_rel, std::memory_order_relaxed)) return true;
  }
}
void BudgetedResourcePool::Release(uint64_t bytes) {
  uint64_t current = used_bytes_.load(std::memory_order_relaxed);
  for (;;) {
    const uint64_t next = bytes >= current ? 0 : current - bytes;
    if (used_bytes_.compare_exchange_weak(current, next,
          std::memory_order_acq_rel, std::memory_order_relaxed)) return;
  }
}
}  // namespace LibertyRecomp::Diagnostics
