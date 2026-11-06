# Mali-G57 Renderer Compatibility and Performance Work

Status: investigation branch; no renderer behavior changed yet.

## Target device and evidence

The current diagnostic capture is from a Samsung SM-A146U (MediaTek MT6833, Mali-G57 MC2), Android 15, Vulkan 1.3.

The 2026-11-06 capture did not demonstrate the intended low-workload profile: the internal target was 1280x574, upscaler was Native, frame cap was 30, reflections were Original resolution, shadow interval was every frame, shader workers was 1, queued frames was 3, extended texture reuse was off, and thermal/FPS guards were off. The swapchain was 2408x1080. Do not use that capture as an A/B test of the suggested profile.

Observed frame intervals in the capture were commonly about 90-105 ms, with P95 roughly 147-169 ms. There were isolated pipeline compile/wait spikes (one around 546 ms), but other long frames had negligible compile/wait time. Presenter handoff waits were about 27-30 ms in early capped windows. These data do not identify one proven sustained bottleneck. `publish_ms` includes waits and is not GPU execution time.

## Confirmed clipping compatibility gap

The pinned ReXGlue source is XHYN-IV-recomp commit `025986db1a24aec73ebccc60aac4c235fd820da4`.

Relevant source:
- `glue/rexglue-sdk-main/include/rex/ui/vulkan/clip_cull_rewrite.h`
- `glue/rexglue-sdk-main/src/ui/vulkan/vulkan_util.cpp`
- `glue/rexglue-sdk-main/src/ui/vulkan/vulkan_device.cpp`
- `tools/xenia-master-1/src/xenia/gpu/spirv_shader_translator.cc`
- `glue/rexglue-sdk-main/src/graphics/gta4_native/native_clip_control.h`

The Vulkan device reports when `shaderClipDistance` is unavailable. The current SPIR-V compatibility rewrite is deliberately limited: it can strip or relocate certain standalone declarations, but it does not emulate clip-plane behavior. It rejects active `gl_PerVertex` member use rather than submitting an invalid module. The translator also initializes its clip-distance feature from the physical-device feature bit. Therefore, merely removing the capability/decorations or forcing the feature on is not a valid Mali fix.

The native clip-control policy currently recognizes six Xenos user clip-plane enable bits but classifies all except plane 0, and cull-only use, as unsupported. Unit tests explicitly encode this limitation. Do not loosen those checks without implementing and testing the actual shader semantics.

### Correct implementation direction

1. Trace where native GTA draw state supplies the enabled user clip-plane equations and how the native shader variants receive guest-space/clip-space position.
2. Add a device-independent shader path for Mali that computes the enabled plane distances from the same guest-space position and equations used by the hardware path.
3. Preserve clipping semantics at primitive boundaries. A fragment-only discard approximation is not automatically equivalent to fixed-function clipping (especially for depth, interpolation, and cull-only modes); do not ship it as exact parity without tests.
4. Handle all enabled plane combinations and cull-only behavior, or explicitly reject unsupported combinations with a precise diagnostic.
5. Add SPIR-V validation and rendering tests for inside, outside, crossing, multiple planes, and cull-only cases. Compare output against a feature-capable Vulkan reference device.
6. Keep the existing feature-capable hardware path unchanged.

## Performance work plan

The engine already contains bounded CPU/GPU profiling structures, including exclusive GPU ranges and CPU ranges for texture preparation, command recording, submission, presenter acquire/present, pipeline compile jobs, and pipeline waits. Use these existing measurements before adding more per-draw instrumentation.

Priority:
1. Capture a correctly activated A14 low-workload profile and log every effective setting, not just requested settings.
2. Record representative warm gameplay frames with CPU phase timings, GPU range timings, texture decode/upload bytes, pipeline cache misses/creates, and presenter acquire/submit/present times.
3. Separate first-use shader compilation hitches from steady-state frame cost.
4. Measure DXT decode time and texture upload time independently. The current texture decode worker is bounded and only parallelizes sufficiently large jobs; do not increase worker count without measurement.
5. Only then tune render scale, reflection/shadow cadence, texture conversion, frames in flight, and queue/present policy. On this device, avoid increasing queued frames or cache budgets blindly because memory pressure and latency matter.

## Acceptance criteria

- Mali-G57 no longer silently loses active title clip-plane behavior.
- SPIR-V validation passes for both the feature-capable path and fallback path.
- No regression on devices that already support shaderClipDistance.
- A/B captures show improved sustained frame time (median and P95), not just a higher configured FPS cap.
- Peak PSS and low-memory termination risk do not regress.
- A real device test is required; CI success alone is not gameplay verification.

## Current state

This branch records the source-level findings and safe implementation plan only. It intentionally does not claim a clipping fix or performance gain. The next code change must be made at the native shader input/variant boundary where the plane equations and clip-space position are available, not by changing only the SPIR-V declaration scrubber.
