# ZMOD-Liberty Mali implementation audit

Reference: [prollynotzjay/ZMOD-Liberty](https://github.com/prollynotzjay/ZMOD-Liberty), branch `mali-dimensity-8400`. It is an alpha fork of vaduur's Android port, based on the same Liberty Recompiled/ReXGlue engine family, but its native renderer source has diverged substantially from the XHYN submodule pinned by GTA4 Universal.

## Galaxy A14 evidence

Latest uploaded capture: `XHYN-IV-debug-20261106-085630-570.log.zip` (audit SHA-256: `db3cb3ac196f888c6c426984bda2c9d411746efb419e8e4942718a4c4c3ed354`).

- GPU: Mali-G57 MC2; Vulkan API version reported by the driver is 1.3.278.
- `shaderInt64=true`, `bufferDeviceAddress=true`.
- `shaderClipDistance=false` and `shaderCullDistance=false`.
- BC1/BC2/BC3 sampling is unavailable; the current native renderer already decodes DXT on the CPU.
- After startup, the capture reports sustained capped gameplay around 30 FPS, with typical P50 frame intervals around 33 ms and P95 around 36–38 ms.
- One early frame interval was about 983 ms, almost entirely outside the frame publisher (about 967 ms), with no shader compile or pipeline wait.
- A later hitch at frame 677 was about 132 ms: approximately 22 ms pipeline wait, 25 ms compile work, and 74 ms outside publish.
- PSS samples were roughly 0.9–1.07 GB with about 1.0–1.2 GB available RAM and no memory-pressure flag in those samples.
- The game process's previous exit is recorded as signal/status 11. The launcher process separately has a low-memory exit label. These are different processes; the report does not prove the game process was killed for low memory. No symbolized native tombstone is present.

## What is reusable

1. **Parallel DXT/texture conversion:** ZMOD splits large texture conversion into row strips and uses multiple CPU workers. The pinned XHYN source already has a gated `RunNativeTextureDecodeRows` path, but its helper is limited to one worker. This branch adapts that existing helper to a bounded pool of at most two helper threads, keeps the calling thread involved, and preserves the existing size/cpuset gates. It does not replace XHYN's DXT format conversion, 16-bit DXT1 path, or converted-texture disk cache.
2. **Thermal/performance ideas:** ZMOD's heat guard and reflection cadence are useful tuning references. They should be A/B tested separately after the worker change, rather than silently combining several performance variables.

## What must not be copied as a claimed fix

- **64-bit shader lowering:** ZMOD has a SPIR-V `shaderInt64` fallback. The A14 driver reports `shaderInt64=true`, so this fallback is not required by this device.
- **Clip-distance rewrite:** ZMOD rewrites unsupported `ClipDistance`/`CullDistance` declarations into ordinary output locations so modules can load, but its own README explicitly says Mali clip planes are skipped and reflections/cut-off geometry can be wrong. This is not equivalent to correct primitive clipping.
- **Blind renderer replacement:** ZMOD's native renderer source differs substantially from the XHYN submodule pinned here. Replacing the whole submodule would risk discarding GTA4 Universal's native renderer, diagnostics, installer, and integration work.

## Remaining engineering work

- Implement a real native user-clip-plane path for the six Xenos planes, including correct plane-equation capture and primitive clipping semantics. The A14 exposes neither clip-distance nor cull-distance support, so simply remapping SPIR-V outputs or forcing a Vulkan feature is not a solution.
- Capture a symbolized tombstone for the signal-11 exit and correlate it with the last game-thread/native log events.
- Run a controlled A/B test of the bounded decode pool on the A14, then measure sustained frame-time P50/P95, memory, and heat over a longer session.
