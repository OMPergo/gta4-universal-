# Performance Engineering Guide

## Key Optimization Strategies
1. **Dynamic Memory Budgeting**:
   - `ComputeMemoryBudget()` ensures the game never exceeds safe limits on memory-constrained devices.
   - On 4 GB devices, aggressive asset unloading prevents Android OS low-memory killer (LMK) eviction.

2. **GPU Workarounds & Shader Pipeline Caching**:
   - Mali GPUs benefit from clip-distance workarounds and deferred subpass loads.
   - Pre-compiled pipeline caches prevent hitching during dynamic shader binding.

3. **Frame Pacing & CPU Affinity**:
   - CPU thread pool limits worker threads according to CPU cluster topology (big.LITTLE).
   - Audio ring buffers maintain a fixed credit margin to avoid underruns without introducing latency.
