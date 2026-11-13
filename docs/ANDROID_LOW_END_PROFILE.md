# Android low-end device profile

The Android runtime is pinned to XHYN source commit
`be597d42d12abb04a552fd87e3853028499ae835`. The source remains an external
Git submodule; this repository stores a deterministic build-time patch at
`tools/apply_android_low_end_profile.py`.

## Automatic detection

The Universal Android app applies the low-end defaults on first launch when
Android reports **6 GiB RAM or less**. Detection currently uses total device RAM
only; it does **not** independently detect Mali-G57 or other GPU models. The
Galaxy A14 5G qualifies because its reported total RAM is below this threshold.
Devices above the threshold retain the upstream baseline defaults.

The Universal Android application ID is `com.ompergo.gta4universal`, so it
installs separately from the upstream XHYN app and starts with its own settings
directory. Existing installations are not overwritten.

## Low-end defaults

- 720p render target with FSR1 Performance upscaling
- 30 FPS frame cap
- Two queued frames instead of three
- 256 shadow map and sun-shadow interval disabled
- Quarter-resolution world, water, and mirror reflections
- Near-only reflection capture
- Dynamic draw distance, reduced to 0.65
- Traffic, parked-car, pedestrian, and scenario density scales at 0.30
- One shader/pipeline worker, pipeline prewarm and lookahead enabled
- Converted-texture disk cache enabled
- Android performance hints and thermal guard enabled
- GPU turbo disabled to avoid forcing sustained heat/power draw
- Adaptive audio buffering retained

On the first Universal launch on a detected low-end device, the profile is applied once and recorded with a marker in the app's settings directory. This intentionally replaces conflicting inherited graphics values for the initial A14 test; subsequent launches preserve player edits. The profile does not alter game data, title updates, saves, or user files.

## Mali-specific findings and guardrails

The November 9, 2026 Galaxy A14 diagnostic capture reports that Mali-G57 MC2 lacks native BC1/BC2/BC3
texture sampling. The pinned runtime already has CPU fallback decoders for
those formats and a bounded texture decode path with at most two helper threads that respects
Android CPU affinity. Do not force extra decode threads without device traces.

The capture also reports that `shaderClipDistance` is unavailable. The
current SPIR-V compatibility rewrite explicitly omits unsupported clip planes;
it does not emulate clipping. Do not describe this as fixed or turn on unrelated
depth-clamp fallbacks. A proper software/shader clipping implementation needs
separate correctness work.

## Build outputs

The Android build workflow compiles the native runtime in Release mode and
strips packaged shared libraries, while preserving separate native symbols.
It packages:
- `app-debug.apk`: debug-key signed and installable on ARM64 Android devices.
- `app-release-unsigned.apk`: release-variant package, unsigned until a release
  signing key is configured.
- Native symbols archive for crash/tombstone analysis.

Minimum Android version remains API 29 (Android 10), target API 35, ABI
`arm64-v8a`. User-owned GTA IV base-game data and TU8 are not included.
