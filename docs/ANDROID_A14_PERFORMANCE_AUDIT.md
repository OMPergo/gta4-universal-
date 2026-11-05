# Android Repository and Galaxy A14 5G Performance Audit

Audit date: 2026-11-05  
Audited branch: `friday/android-runtime-audit`  
Pinned Android port: `XHYN-PH/XHYN-IV-recomp` commit `025986db1a24aec73ebccc60aac4c235fd820da4`

## Executive findings

1. **The Android APK contains the actual upstream GTA IV runtime.** The Android workflow checks out the pinned XHYN submodule, runs its dependency setup, builds the Vulkan driver proxy, runs `os/android/scripts/build_native.sh 2`, stages the native runtime, and then packages it with Gradle. The earlier statement that the APK was only a launcher shell was inaccurate.
2. **The separate `LibertyUniversalCore` is not the game engine.** Root `CMakeLists.txt` builds a support library containing hardware detection, performance-tier configuration, GPU flags, multiplayer scaffolding, a media-ID helper, and a small JNI bridge. The actual Android game activity extends SDL3's `SDLActivity`, and the installer loads `libliberty_install.so`; the gameplay engine comes from the pinned XHYN/ReXGlue build.
3. **The Galaxy A14 has reached gameplay, but it is not stable.** The supplied device reports show severe frame pacing, native memory growth, Android low-memory termination, and a later native signal-11 exit. The root cause is not yet isolated. These are different failure modes and must not be treated as one proven bug.
4. **The quickest controlled performance experiment is already supported by the upstream settings UI.** Lower render/output workload and reflection cost first; test two shader workers only as a separate A/B test. Do not raise memory budgets or enable extended texture retention on a 4 GB device.
5. **CI success is not a device-performance result.** The workflow verifies source setup, native build, host unit tests, ARM64 ELF files, APK contents, and APK size. It does not run a sustained Galaxy A14 gameplay test.

## Repository structure and build path

The main repository tree contains 147 entries, including the pinned submodule gitlink. The XHYN submodule is pinned to commit `025986db1a24aec73ebccc60aac4c235fd820da4`.

The Android workflow performs this sequence:

1. Checkout repository and submodules.
2. Restore upstream Java/UI resources into the app source tree without overwriting project-owned files.
3. Set up JDK 17, Android SDK 35, and NDK 29.
4. Run XHYN's `tools/setup_repo.py` and its `--check`.
5. Build the Android Vulkan driver proxy.
6. Build the real native runtime using `build_native.sh 2`, with `STRIP=0` so native symbols remain available for crash symbolication.
7. Stage `libmain.so`, `libliberty_install.so`, ReXGlue runtime/GPU plugins, SDL3, C++ runtime, Vulkan proxy, and game resources.
8. Build and run the root support-library host tests.
9. Build `LibertyUniversalCore` for ARM64 and package the Android app.
10. Verify AArch64 ELF headers, required libraries inside the APK, checksum, and a 200,000,000-byte APK ceiling; upload the APK artifact.

The real native build is driven by the pinned upstream scripts, not by the root CMake target. The root CMake target is a separate support library and its tests must not be mistaken for tests of the game engine.

## Verified APK contents

The extracted successful Android artifact used for this audit contains a 72,094,576-byte APK (about 68.7 MiB), with 146,823,797 bytes of uncompressed ZIP entries. Important ARM64 libraries include:

| Library | Approximate uncompressed size |
|---|---:|
| `libmain.so` | 43.6 MB |
| `librexgpu-gta4-native.so` | 29.6 MB |
| `librexruntime.so` | 8.9 MB |
| `librexgpu-xenos.so` | 4.5 MB |
| `libSDL3.so` | 1.6 MB |
| `libc++_shared.so` | 1.3 MB |
| `libLibertyUniversalCore.so` | 1.0 MB |
| `libliberty_install.so` | 0.24 MB |

The APK also contains the Vulkan proxy, controller prompts, font atlases, and launcher resources. This verifies that the download is not the earlier 2.4 MB launcher-only shell. It does not prove a particular device can sustain gameplay.

## Android app identity and compatibility

- Application ID: `com.xhyn.ivrecomp`
- Namespace: `com.libertyrecomp`
- Version: `0.5.9.3-universal-alpha` (version code 40)
- ABI: `arm64-v8a` only
- Minimum Android API: 29
- Target/compile SDK: 35
- Manifest requires Vulkan 1.1 and declares a separate `:game` process.
- The manifest sets `android:largeHeap="true"`; this does not increase physical RAM or Vulkan heap capacity and is not a fix for native-memory pressure.

The build is a debug/development APK, not a stable signed release. A new GitHub runner can produce a different debug signing certificate. If Android refuses to install it over an older build, back up game data and saves before uninstalling the old package.

## Galaxy A14 5G: evidence and likely bottlenecks

The target is a 4 GB-class Samsung Galaxy A14 5G with Dimensity 700 / Mali-G57 MC2 and a 720p display.

The supplied reports establish:

- Gameplay has loaded after the Rockstar intro and Nico loading screen.
- Frame delivery was poor, with reported periods around 21–22 FPS and drops into the 8–14 FPS range; frame intervals exceeded 100 ms and a stall reached roughly 504 ms.
- The process reached approximately 1.3–1.7 GB RSS in separate exits. One exit was Android low-memory termination; another was signal 11. RSS is not a subsystem breakdown and does not by itself identify the crashing allocation.
- Renderer observations included texture memory growth, a shrinking texture budget, texture evictions/re-uploads, and long pipeline-compilation/wait intervals.
- The device did not expose `shaderClipDistance` in the reported Vulkan capabilities. That is a compatibility concern to investigate, but it is not proven to be the cause of the signal-11 crash.
- The supplied crash bundle did not include a native tombstone/backtrace sufficient to symbolize the fault.

The strongest current hypotheses are renderer texture/cache churn under memory pressure, pipeline compilation stalls, and a separate native lifetime/compatibility defect. They remain hypotheses until the next capture correlates them with the failing frame and native stack.

## Settings already present in the pinned Android port

The pinned port exposes these controls in Settings:

- Output resolution: 854x480, 540p, 720p, 900p, 1080p.
- Upscaler: native or FSR1, with FSR quality modes.
- Frame cap: 20–120 FPS; default choice is 30.
- Adaptive draw distance and draw-distance scale.
- Shadow-map size, shadow distance, and sun-shadow interval.
- Reflection resolution, water/mirror reflection resolution, reflection AA, capture distance, and water/environment update intervals.
- Graphics shader workers (1–5).
- Pipeline prewarm and lookahead.
- Extended texture reuse and converted-texture disk cache.

The bundled `default_args.txt` already sets adaptive draw distance on, minimum dynamic draw distance 0.80, reflection capture distance near, traffic/parked-car/ped/scenario-ped density scales to 0.4, and native max queued frames to 3. `MobileConfig` defaults FXAA, native upscaling, 256 shadow-map size, near reflections, no extended texture reuse, and converted-texture disk cache on. The settings UI documents FSR1 as useful when the GPU is the bottleneck, and describes two shader workers as a cautious test before trying more.

## Recommended controlled A14 test profile

Apply these through the app's existing Settings UI, not by editing generated binaries:

| Setting | First test value | Why |
|---|---|---|
| Output resolution | 540p | Reduces pixel work and render-target memory on the 720p panel. |
| Upscaler | FSR1 | Tests whether the Mali GPU is the limiting stage. |
| FSR quality | Balanced | A moderate first step; compare against Quality and Performance. |
| Frame limit | 30 FPS | Avoids chasing an unrealistic higher target on this device. |
| Sun shadows | Off | Removes a costly shadow pass for the test. |
| Reflection resolution | Quarter | Reduces reflection render-target cost and memory. |
| Water updates | Every 2–3 frames | Reduces repeated reflection work. |
| Other/environment reflection updates | Every 2–3 frames | Same, with some visual update delay. |
| Reflection distance | Near | Already the bundled default; keep it. |
| Draw distance | 0.5–0.65 | Reduces CPU/GPU scene work; compare against adaptive mode. |
| Graphics shader workers | 1 baseline, then 2 A/B | Two may shorten pipeline preparation stalls but can compete for CPU and heat. |
| Pipeline prewarm/lookahead | On | Already enabled by defaults; retain for the first test. |
| Extended texture reuse | Off | Avoids retaining extra texture memory on a 4 GB device. |
| Converted-texture disk cache | On | Can reduce repeated conversion work without keeping the same extra working set resident. |
| GPU driver | System/automatic Mali driver | Do not force an Adreno/Turnip driver on Mali. |

This is a diagnostic profile, not a guaranteed fix. If the game is CPU-bound or stalls on shader compilation, lowering resolution alone will not solve it. Compare one change group at a time and record average frame time, 1% lows, thermal state, PSS/RSS, texture budget/evictions, and pipeline wait time.

## Memory policy: what not to do

The upstream `mobile_memory.h` computes a texture-retention limit from physical RAM, currently available RAM, and a reserved headroom amount. On a 4 GB-class device this intentionally keeps the extra idle texture-retention budget small and responds to low available memory. Do not blindly raise texture budgets or turn on extended texture reuse: that can hide eviction churn briefly while increasing the chance of another low-memory kill.

The separate root `ComputeMemoryBudget()` function is not the same as the upstream renderer's Vulkan/texture retention policy. Its configured tier values are not proof that the game renderer is using those budgets.

## Next diagnostic work, in priority order

1. **Capture a symbolized native tombstone.** Keep the unstripped diagnostic native libraries, export the Android crash/tombstone and `last_launch.txt`, and run `ndk-stack` against the exact matching build symbols. The signal-11 report needs a faulting PC and native stack.
2. **Run a repeatable A/B route.** Same save, same route, same driver, same temperature starting point. Compare default settings against the A14 profile.
3. **Correlate renderer memory and frame-time telemetry.** Determine whether the rising bytes are host texture payload/metadata, Vulkan texture images, reflections/surfaces, geometry conversions, or pending fence-retired resources. Track allocation and release identities, not just total RSS.
4. **Investigate pipeline compilation separately.** Compare worker count 1 vs 2, warm-cache vs cold-cache, pipeline prewarm/lookahead, and compile/wait spans.
5. **Check Mali-specific pipeline compatibility.** Reproduce the missing `shaderClipDistance` capability and inspect affected shader/pipeline variants; do not disable clipping or change depth behavior globally without a targeted test.
6. **Keep the app launcher and game runtime separate in diagnostics.** The game is in `:game`; capture the correct process and distinguish launcher exits from game exits.

## Repository inconsistencies corrected by this audit

- The old README and Android runtime-gap document said the APK lacked ReXGlue and generated game code. That was false for the Android workflow because it builds and packages the pinned XHYN submodule's native engine.
- The root CMake target is support/scaffolding code and is not the native gameplay engine. The JNI bridge in that target is not the SDL game's launch bridge.
- The iOS target is still a SwiftUI launcher shell; the Android native runtime is not automatically an iOS runtime.
- A passing APK build is not a successful device gameplay/performance test.

## Bottom line

There is a real GTA IV ARM64 runtime in the Android APK, and the A14 has reached gameplay. The next gains are likely to come from reducing reflection/shadow/render-target work, testing FSR1 and a second shader worker, then fixing the measured texture/cache and pipeline stalls. The signal-11 fault still needs a symbolized native stack; no honest audit can claim a single proven root cause yet.
