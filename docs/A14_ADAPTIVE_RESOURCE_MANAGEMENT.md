# A14 Adaptive Resource Management — Implementation Status

## What this change implements

- A deterministic resource-budget planner that converts the hardware tier's process ceiling and preferred texture/streaming/decode budgets into bounded subsystem ceilings.
- A 20% safety reserve for allocator overhead, transient decode spikes, and runtime state.
- Independent caps for texture (25% of usable memory), streaming cache (12.5%), and decode scratch (6.25%).
- A thread-safe accounting primitive named BudgetedResourcePool that future asset, texture, and decoder allocators can use to reserve before allocating and release after freeing.
- Startup integration that applies the calculated texture, streaming-cache, and decode-scratch ceilings to the active graphics settings.
- A host unit test covering low-memory planning, unknown memory detection, and reservation limits.

## Important boundary

This is the first enforceable policy layer, not a complete streaming or memory-eviction implementation. Existing asset/texture/audio allocators still need to call BudgetedResourcePool (or an equivalent central allocator) for these ceilings to constrain real allocations. This change does not yet transcode BC formats, change shader behavior, or prove an FPS improvement.

## Next implementation slices

1. Route texture uploads and decoded DXT/BC staging allocations through the texture and decode pools.
2. Add cache eviction based on last-use/frame age and priority, with safe GPU lifetime tracking.
3. Implement a device-capability-selected BC1/BC2/BC3 transcode path and cache converted output.
4. Replace the Mali clip-distance flag with a verified shader-side clipping path; do not enable a workaround unless the renderer actually implements it.
5. Add separate CPU submit, GPU execution, and present-wait timing, plus process PSS/RSS and thermal sampling.
6. Instrument SDL audio device/stream lifecycle and XMA packet/decoded-frame counters.
7. Verify the A14 profile is active in the actual game runtime and run repeatable cold/warm 5–10 minute comparisons.

All work must continue to use the user's original legally obtained game files without requiring a modified game image.
