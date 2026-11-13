#pragma once
#include <atomic>
#include <cstdint>
namespace LibertyRecomp::Diagnostics {
struct ResourceBudgetInputs {
  uint64_t process_budget_bytes = 0;
  uint64_t available_system_bytes = 0;
  uint64_t preferred_texture_budget_bytes = 0;
  uint64_t preferred_streaming_budget_bytes = 0;
  uint64_t preferred_decode_scratch_bytes = 0;
};
struct ResourceBudgetPlan {
  uint64_t process_budget_bytes = 0;
  uint64_t safety_reserve_bytes = 0;
  uint64_t texture_bytes = 0;
  uint64_t streaming_bytes = 0;
  uint64_t decode_scratch_bytes = 0;
  uint64_t runtime_bytes = 0;
  uint64_t TotalAssignedBytes() const;
  bool IsValid() const;
};
ResourceBudgetPlan ComputeResourceBudgetPlan(const ResourceBudgetInputs& inputs);
class BudgetedResourcePool {
 public:
  explicit BudgetedResourcePool(uint64_t limit_bytes) : limit_bytes_(limit_bytes) {}
  bool TryReserve(uint64_t bytes);
  void Release(uint64_t bytes);
  uint64_t UsedBytes() const { return used_bytes_.load(std::memory_order_relaxed); }
  uint64_t LimitBytes() const { return limit_bytes_; }
 private:
  const uint64_t limit_bytes_;
  std::atomic<uint64_t> used_bytes_{0};
};
}  // namespace LibertyRecomp::Diagnostics
