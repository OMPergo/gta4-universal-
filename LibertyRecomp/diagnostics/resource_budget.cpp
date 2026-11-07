#include "resource_budget.h"

#include <algorithm>
#include <limits>

namespace LibertyRecomp::Diagnostics {

uint64_t ResourceBudgetPlan::TotalAssignedBytes() const {
  return safety_reserve_bytes + texture_bytes + streaming_bytes +
         decode_scratch_bytes + runtime_bytes;
}

bool ResourceBudgetPlan::IsValid() const {
  return process_budget_bytes > 0 && TotalAssignedBytes() <= process_budget_bytes;
}

ResourceBudgetPlan ComputeResourceBudgetPlan(const ResourceBudgetInputs& inputs) {
  ResourceBudgetPlan plan;
  // A zero process budget means detection failed. Do not invent a budget.
  if (inputs.process_budget_bytes == 0) return plan;

  // Available system memory can only tighten the process ceiling. Keep a
  // 20% reserve for allocator overhead, transient spikes, and non-pool state.
  uint64_t ceiling = inputs.process_budget_bytes;
  if (inputs.available_system_bytes > 0) {
    ceiling = std::min(ceiling, inputs.available_system_bytes);
  }
  plan.process_budget_bytes = ceiling;
  plan.safety_reserve_bytes = ceiling / 5;
  uint64_t usable = ceiling - plan.safety_reserve_bytes;

  // Each pool is independently capped; leftover capacity remains available
  // to the general runtime rather than being silently assigned to textures.
  const uint64_t texture_cap = usable / 4;
  const uint64_t streaming_cap = usable / 8;
  const uint64_t decode_cap = usable / 16;
  plan.texture_bytes = std::min(inputs.preferred_texture_budget_bytes, texture_cap);
  plan.streaming_bytes = std::min(inputs.preferred_streaming_budget_bytes, streaming_cap);
  plan.decode_scratch_bytes = std::min(inputs.preferred_decode_scratch_bytes, decode_cap);
  plan.runtime_bytes = usable - plan.texture_bytes - plan.streaming_bytes -
                       plan.decode_scratch_bytes;
  return plan;
}

bool BudgetedResourcePool::TryReserve(uint64_t bytes) {
  uint64_t current = used_bytes_.load(std::memory_order_relaxed);
  for (;;) {
    if (bytes > limit_bytes_ || current > limit_bytes_ - bytes) return false;
    if (used_bytes_.compare_exchange_weak(current, current + bytes,
                                          std::memory_order_acq_rel,
                                          std::memory_order_relaxed)) {
      return true;
    }
  }
}

void BudgetedResourcePool::Release(uint64_t bytes) {
  uint64_t current = used_bytes_.load(std::memory_order_relaxed);
  for (;;) {
    const uint64_t next = bytes >= current ? 0 : current - bytes;
    if (used_bytes_.compare_exchange_weak(current, next,
                                          std::memory_order_acq_rel,
                                          std::memory_order_relaxed)) {
      return;
    }
  }
}

} // namespace LibertyRecomp::Diagnostics
