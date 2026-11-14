# Android A14 Stability Milestone

Status: **in progress — APK packaging is complete; stable gameplay is not proven.**

Target validation device: Samsung Galaxy A14 5G (SM-A146U, Dimensity 700 / Mali-G57 MC2, Android 15, ARM64).

## Verified build baseline

- Successful Android ARM64 APK workflow: [run 42830251474](https://github.com/OMPergo/gta4-universal-/actions/runs/42830251474).
- Build commit: `63f30ed5062fa306db1d0f44ffe593eddeade6e9`.
- The workflow verifies the debug APK package identity, signature, ARM64 native libraries, and uploads a signed debug APK plus an unsigned release APK and native symbols.
- This proves packaging and artifact production only. It does not prove sustained gameplay or performance.
- User-reported behavior: the installed app opens and accepts the user's own GTA IV files and TU8. This is not a repeatable runtime acceptance test.

## Device evidence currently available

The uploaded November 6 diagnostic archive was audited in the companion Title Update repository. It records:

- Mali-G57 MC2 does not expose Vulkan `shaderClipDistance`; the current native path warns that title clip planes are not applied.
- The GPU cannot sample BC1/BC2/BC3 formats directly, so DXT textures are decoded on the CPU.
- The captured profile reports approximately 8.79 FPS average, 113.83 ms average frame time, and a worst captured frame interval around 1.66 seconds.
- Audio queue underruns were reported in the diagnostic review.
- Separate user-reported sessions included native SIGSEGV crashes and a low-memory process termination. Exact native crash stacks still need to be symbolicated against the matching build's symbols.

These observations are a baseline, not proof that one of the warnings caused every crash or hitch.

## Engineering blockers and safe order

1. **Crash attribution first.** Match each tombstone / PC / build ID to the exact native symbols from the APK run. Record the faulting thread, library, function, and memory address. Do not guess at a fix from RSS alone.
2. **Mali clip-plane correctness.** The native renderer's supported user-clip semantic subset is narrower than the guest register model. Do not merely disable the rejection check or strip clip-distance outputs: that can render incorrect geometry. Implement a verified shader-side plane evaluation path with unit tests for enabled planes, sparse masks, clip disable, and cull-only behavior before enabling it on Mali.
3. **Texture bandwidth and memory.** The current fallback performs CPU DXT decoding. Profile decode time and staging/upload peaks. The separate adaptive resource-budget work is only a planner/accounting primitive until real texture, streaming, and decoder allocations reserve/release against it.
4. **Audio.** Add SDL device/stream lifecycle and XMA packet/decoded-frame/queue counters; then fix the measured underrun source.
5. **Frame pacing.** Capture separate CPU-submit, GPU-execution, and present-wait timings, plus process PSS/RSS and thermal state. Compare cold and warm runs before changing caps or queues.

## Acceptance gates

- [ ] APK installs and updates cleanly on the A14 without package/signature conflicts.
- [ ] User-owned GTA IV base game and TU8 validate and load.
- [ ] At least 10 minutes of repeatable gameplay without SIGSEGV, low-memory termination, or unbounded memory growth.
- [ ] Mali clip-plane tests pass and in-game geometry is correct.
- [ ] No persistent audio underruns; audio remains synchronized.
- [ ] Cold/warm performance captures include FPS, frame-time percentiles, CPU/GPU/present timing, PSS/RSS, and thermal state.
- [ ] A documented low-end target is met in an actual gameplay session; a successful CI build or menu launch is not sufficient.

## Immediate next deliverable

Symbolicate the latest A14 native crash against the matching debug-symbol artifact and publish a short root-cause report. In parallel, prototype the Mali shader-side clip-plane path behind a disabled-by-default capability gate. Keep both changes isolated from the known-good APK branch until tests pass.

## Data integrity

Do not include, redistribute, or modify the user's proprietary GTA IV game image, title update, keys, or saves. All device tests use the user's own legally obtained files.
