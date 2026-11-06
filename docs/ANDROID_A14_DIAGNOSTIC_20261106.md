# Galaxy A14 diagnostic audit — 2026-11-06 capture

## Evidence

- Evidence repository: https://github.com/OMPergo/gta-4-title-8-update
- Machine-generated package and log report: https://github.com/OMPergo/gta-4-title-8-update/blob/main/AUDIT_REPORT.md
- Capture: XHYN-IV-debug-20261106-053947-653.log.zip
- Capture SHA-256: 717f2abd9b2381619b93d18e35c25938e9765f2d576565777d49fca2d46c20b5
- Device: Samsung SM-A146U, Android 15, MediaTek MT6833, Mali-G57 MC2
- Driver: Android system Vulkan loader / system Mali driver

## Main findings

### 1. The A14 low-memory profile was not active

The captured runtime settings show:
- Internal output preset: 720p; actual render target reported as 1280×574.
- Upscaler: Native (no FSR).
- Frame cap: 30.
- Reflections: Original resolution.
- Shadow interval: every frame.
- Shader workers: 1.
- Queued frames: 3.
- Extended texture reuse: off.
- Converted texture disk cache: on.
- Android thermal guard: off.
- GPU Turbo: off.
- FPS guard: off.

Therefore this capture is not a valid test of the recommended A14 reduced-workload profile. It does not establish whether that profile helps or hurts. Do not repeat the previous settings advice as if it had already been applied.

The display swapchain was created at 2408×1080, while the game renderer reports 1280×574 internal rendering. These are separate surfaces/resolutions; future diagnostics must record both.

### 2. Mali compatibility issues are now confirmed in the live render path

The system driver reports:
- GPU: Mali-G57 MC2.
- Vulkan API 1.3.
- shaderClipDistance is unavailable.
- The runtime logs that GTA title clip planes are not applied.
- The native renderer records repeated pipeline rejections with reason shader-clip-distance-unsupported.
- The GPU cannot sample BC1/BC2/BC3 texture formats natively. DXT1 is decoded on CPU to A1R5G5B5; DXT3/DXT5 are decoded on CPU to R8G8B8A8.

The clip-distance issue is a rendering correctness/compatibility defect. It must be handled with a deliberate shader/software clipping fallback or a validated alternate pipeline. Do not assume the separate depth-clamp fallback setting solves shader clip distance; those are different Vulkan capabilities.

CPU DXT decoding is a credible performance cost on this device. It is directly reported by the renderer, but its exact contribution to the measured frame time is not yet isolated.

### 3. Frame pacing points away from sustained shader compilation as the only bottleneck

The trace contains 12 recent 64-present windows and four worst 64-present windows.

- Recent windows show median frame intervals commonly around 90–105 ms, with P95 values reaching roughly 147–169 ms. That is far below the 30 FPS cap.
- In most recent windows, pipeline wait and compile-job time are zero.
- A separate worst hitch includes approximately 546 ms of compile-job time and 503 ms of pipeline wait.
- Another recorded 312 ms hitch has only about 0.8 ms compile-job time and 0.12 ms pipeline wait.
- Presenter telemetry shows handoff waits around 27–30 ms in early capped windows. Publisher wall time includes waits and is explicitly not a GPU-time measurement.

Conclusion: pipeline compilation can cause isolated severe hitches, but it does not explain the sustained slow recent windows by itself. The next investigation should instrument render submission, GPU completion/fences, present handoff, CPU texture decode, and thread scheduling separately.

### 4. Memory and exit records

- Last-session properties report approximately 1,692 MB RAM for the game session.
- The game process previous-exit record is signal/status 11.
- A separate launcher-process previous-exit record is labeled Low memory.
- These are separate process records. The signal-11 game exit must not be described as a proven Android low-memory kill.
- No native tombstone/backtrace is present in this archive, so the faulting instruction and stack remain unknown.

### 5. Missing-file probes need VFS/layout verification

The runtime records 21 failed opens across 16 paths, including:
- game:\xbox360\audio\config\engineSettings.xml
- game:\xbox360\audio\config\curves.dat12
- game:\xbox360\audio\config\categories.dat15
- game:\xbox360\audio\config\sounds.dat15
- game:\xbox360\audio\config\effects.dat11
- update:\shaders\rage_postfx_e2dcl\rage_postfx_e2dcl\rage_postfx_e2.dcl

These are probes, not yet proof that every resource is required for boot. The upstream installer extracts common.rpf, xbox360.rpf, and audio.rpf into directory trees, so we must verify that extraction completed and that the VFS mount exposes the expected game:\xbox360\... paths. The TU8 package's own file table contains the update shader files separately; it does not contain the base-game audio configuration.

## Verified USA TU8 package metadata

The uploaded package was structurally parsed using the upstream STFS layout:
- Size: 3,715,072 bytes — matches the upstream known TU8 v8 package size.
- STFS magic: LIVE.
- Content type: 0x000B0000 (Title Update installer).
- Title ID: 0x545407F2.
- Media ID: 0x6AC07221 (USA).
- Version: 5; base version: 5 (as encoded in this package metadata).
- 22 file-table entries parsed.
- default.xexp: 2,582,528 bytes; SHA-256 480aee5e2b42707791e7571bb8407c5bb3f6c7534f07f9beb426db4cfc648fd3.
- update.img: 831,488 bytes; SHA-256 6307fdfa815d5fa9ad64dc585c813e564efd6a51e29568590f274e74786ff82e.

The full file table and per-file hashes are in the evidence report. The parser confirms STFS structure and metadata; it does not perform RSA signature verification.

## Next engineering actions

1. Fix the actual Mali shader path: trace the shader-clip-distance-unsupported rejection to the shader translator/pipeline builder. Implement and test a proper software clip-distance path for devices without shaderClipDistance; preserve equivalent clip semantics. Do not merely suppress the warning or force unsupported features on.
2. Measure CPU DXT conversion: add counters/timers for bytes decoded, decode duration, conversion allocations, and cache hits. Consider a supported GPU-side transcode/format path only after checking device extensions and image-format support.
3. Instrument present handoff: split CPU command recording, queue submission, GPU fence wait, acquire/present wait, and Android display timing. Current publish_ms is not enough to assign the bottleneck.
4. Verify RPF extraction and VFS mounts: inspect the installed game directory for xbox360/audio/config and compare expected RPF extraction paths with the actual VFS mount.
5. Run one controlled A14 A/B test: apply the low-memory profile visibly in Settings, verify the effective runtime-settings line and last-session.properties, then repeat the same route and temperature starting point.
6. Capture a native tombstone: preserve the exact unstripped symbols for this APK and collect the game-process tombstone for the signal-11 exit.

## Important distinction

This diagnostic archive is from a build/run that reached game rendering. CI success and successful launch do not establish stable gameplay. No APK changes have been made from this audit yet.
