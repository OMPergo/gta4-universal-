# GTA IV Universal source revision and runtime integration audit

**Audit date:** 2026-11-08  
**Target branch:** `integration/theft4-ios-xhyn-multiplatform`  
**Primary repository:** `OMPergo/gta4-universal-`

This is a source-backed integration decision record. It does not claim that either
reference runtime has been merged into Universal.

## 1. Source snapshots inspected

| Source | Inspected revision / reference | Role | Integration status |
|---|---|---|---|
| [Theft4](https://github.com/KoreanSeats1/Theft4) | `88115fefad59cfcb62d17c0c9548a7a900c01a23` (main snapshot used for this audit) | iOS UIKit application, AOT runtime startup, graphics, controller/audio platform work | Read-only reference; not copied |
| [OMPergo XHYN mirror](https://github.com/OMPergo/XHYN-IV-recomp-multiplayer-1.0) | `main` branch inspected; exact commit SHA still needs to be emitted by CI and pinned | Android app, installer, SDL/input, touch layouts, driver/performance controls | Read-only reference; not copied |
| [XHYN upstream](https://github.com/XHYN-PH/XHYN-IV-recomp) | Test12 release lineage; upstream main is newer than the older Test7/Test8 history | Android product behavior and runtime fixes | Comparison reference only; do not silently substitute for the OMPergo mirror |
| Universal | branch HEAD at time of audit | diagnostics/performance/GPU compatibility/session scaffolding | Active integration destination |

The Theft4 revision was identified from the repository's main-branch source links and
read directly by commit SHA. The XHYN mirror's checked-in README identifies Test12
as its product baseline, but the GitHub file connector did not expose the branch
commit SHA. The parallel workflow therefore records `git rev-parse HEAD` at runtime.
Do not declare the mirror pinned until that SHA is captured and committed here.

## 2. Theft4 findings that change our integration plan

The inspected Theft4 tree is substantially more than an iOS launcher:

- Root CMake uses CMake 3.29+ and C++23, and its dependency setup is coordinated
  by `tools/setup_repo.py`; a plain ZIP or shallow source copy is not a valid
  reproducible build.
- `LIBERTY_RECOMP_IOS_CORE_ONLY` is an explicit early CMake path. It builds
  the iOS core/link probe without game assets, but it is not the full game app.
- `ios/CMakeLists.txt` owns the UIKit/Objective-C++ app and bridge targets.
  The full game-code archive is separately gated by `THEFT4_BUILD_GAME_CODE`.
  Actual guest startup is separately gated by `THEFT4_ENABLE_GAME_STARTUP`.
- The documented full iOS build uses the `ios-device-debug` preset, Xcode,
  initialized dependencies, a user-supplied development team for signing, and
  an iOS 26 deployment target in the documented startup configuration.
- The current 0.3 architecture record describes a direct Metal graphics path.
  Earlier Vulkan-over-MoltenVK bring-up is historical; do not assume the old
  renderer route is still the default.
- The startup record describes real AOT GTA IV execution and a rendered 3D
  scene on the developer's tested M5 iPad. Its release guide now reports full
  3D, decoded audio, and the first player-control state. That is strong evidence
  for the iOS reference, not proof that it runs on iPhone 12/13 or in Universal.
- Theft4's own records still distinguish successful compile/startup from full
  gameplay/device-floor/performance acceptance. We must preserve those gates.

### Consequence

Do not copy only `ios/` into the current Universal CMake project and expect a
working iOS app. The app shell is coupled to the selected ReXGlue runtime,
generated GTA IV code, dependency patches, assets, renderer, and CMake target
graph. Importing the full runtime is a planned source promotion, not a small
launcher patch.

## 3. XHYN Android findings

The OMPergo mirror README describes its source as a fork of XHYN IV / the Android
LibertyRecomp port. The Android reference already contains the game runtime,
game-file installation flow, SDL/controller/touch handling, graphics settings,
driver support, diagnostics and device-specific performance work.

Useful product behavior to map into shared Universal contracts:

- Shared logical actions and controller mapping; Android SDL/GameController
  remains an Android adapter.
- Touch layout profiles, geometry, opacity, context, and separate Mobile/Xbox
  layouts.
- Graphics/performance preferences and explicit requested-versus-active state.
- Save/data-file operations and diagnostics/report export.
- Installer validation and progress semantics.

Do not import Android Java/Kotlin UI or Android NDK/SDL glue into the iOS target.
Share behavior and data contracts, not platform-specific implementation files.

## 4. Shared runtime lineage: current decision

Both references identify LibertyRecomp and ReXGlue as their foundation. That
does not establish that their checked-in runtime trees are byte-identical or
API-compatible. Theft4 contains a substantial iOS-specific app/runtime/renderer
adaptation; XHYN contains Android-specific integration and product changes.

**Decision:** use the selected Theft4/LibertyRecomp/ReXGlue runtime revision as
the initial iOS source-of-truth candidate. Treat XHYN as the Android implementation
and behavior reference. Before promoting shared source, compare:

1. Exact ReXGlue and generated GTA IV source revisions in both repositories.
2. Every relevant submodule gitlink SHA, not merely the URLs in `.gitmodules`.
3. Local dependency patches and their reproducible setup scripts.
4. Generated source manifests, ABI/compiler requirements, and license notices.
5. Android/iOS differences in kernel, filesystem, audio, input, renderer, and
   game-startup services.

If the runtime revisions diverge, do not merge both copies. Select one baseline
and port the missing platform changes deliberately.

## 5. Parallel validation plan

The ten-lane workflow is an initial validation matrix, not ten full runtime builds.
Its current successful checks prove the Universal host/core tests, current
session-scaffold tests, source inventory, docs and repository hygiene. The iOS
simulator/device lanes were cancelled before their build steps in the first run;
they have not passed.

Next useful parallel lanes, after immutable source pins are committed:

1. Universal host core build and complete unit tests.
2. Universal iOS Simulator core/link build.
3. Universal iPhoneOS ARM64 core/link build.
4. Theft4 pinned iOS core/link probe.
5. Theft4 pinned full app configure/dependency preflight (no signing secrets).
6. Theft4 generated AOT source inventory and compile-only checks.
7. XHYN pinned Android Gradle/resource validation.
8. XHYN ARM64 native runtime compile/package validation.
9. Shared settings/protocol contract tests.
10. License, provenance, and artifact-boundary checks.

Full Theft4 app/device signing and Android APK assembly must remain separate from
these source-only lanes because they have different SDK, dependency, signing and
game-data requirements. A lane that lacks its required toolchain or private
user-supplied game files must report `blocked/skipped`, not fabricate a pass.

## 6. License and content boundaries

Theft4 and XHYN identify GPL-3.0 project licensing, while submodules and vendored
dependencies retain their own licenses. Verify the actual file origin and license
before importing each component. Preserve notices and source attribution.

Do not put GTA IV game data, TU8, encryption keys, user saves, private crash
reports, device identifiers, Apple signing keys, or provisioning profiles in
the public repository or ordinary CI artifacts.


## 7. Direct shared-runtime comparison: the two trees are not interchangeable

The following files were fetched from the pinned Theft4 snapshot and the OMPergo
XHYN mirror. The blob SHAs are exact content identifiers, not commit SHAs.

| File | Theft4 blob SHA | XHYN mirror blob SHA | Result |
|---|---|---|---|
| `glue/rexglue-sdk-main/CMakeLists.txt` | `5d5667499f5d57be48cc7fb8529153c971b36e3a` | `fef3fea0eab4cedadfad4a89a3979d2b2aee108a` | Diverged |
| `glue/rexglue-sdk-main/gta4-recomp/generated/sources.cmake` | `1b9e7487e2831983a98ac62db1331de21d1429be` | `6765efc6dc72cb1b31a47ae4308f253a7fc2a6c1` | Diverged |
| `glue/rexglue-sdk-main/src/system/runtime.cpp` | `eff2da89ccd14f36c2dd5a3bb5ba91170a316557` | `1aad87ae6b3edfe9ab35667564a5ffd176ba313c` | Diverged |
| `tools/setup_repo.py` | `46d450866e69f057a904586d4aee941549f85fed` | `5c90f673280becb51e39bad02334f4b60971a4d1` | Diverged |
| `CMakePresets.json` | `09e8475cf7c78a9294838b175cb76678c2170bc8` | `2f2e056687e48016cb0ed51f03e310d9f5a5f86a` | Diverged |

Specific differences observed:

- Theft4's ReXGlue CMake adds `REXGLUE_CORE_ONLY`, `REXGLUE_RUNTIME_ONLY`,
  `REXGLUE_HEADLESS_KERNEL`, and headless audio/XMA/input build switches.
  Those are essential to its staged iOS core/runtime bring-up. XHYN's CMake
  instead carries Android-specific library naming and compiler optimization
  logic. Neither file can replace the other wholesale.
- The generated AOT source manifest in XHYN contains five additional generated
  translation units, `gta4_recomp.84.cpp` through `gta4_recomp.88.cpp`, that
  are not listed by Theft4's manifest. The two generated source sets therefore
  require a provenance and registration/ABI comparison before they can be
  considered one title build.
- `runtime.cpp` differs in runtime filesystem/data-mod policy and clock policy.
  These are behavioral changes, not just formatting.
- Theft4's setup helper has a `--check-export` mode to validate a frozen,
  already-patched dependency source export without Git writes. The XHYN mirror's
  helper does not expose that mode.
- Theft4's CMake presets include iOS device/simulator core-only presets; the
  XHYN mirror's preset file does not contain those iOS core presets.
- The two `.gitmodules` files fetched in this audit have the same blob SHA,
  but that only proves their URL/config text matches. It does not prove the
  gitlink commit pointers in each repository match.

### Integration decision updated

**Do not select either complete ReXGlue directory as a drop-in replacement for
the other.** Use Theft4's iOS-specific CMake/runtime adaptations as the iOS
starting point, then selectively port Android-independent fixes from XHYN after
each change is reviewed against the Theft4 interfaces and generated AOT source
manifest. Keep XHYN's Android optimizations, Java/Kotlin UI, SDL Android glue,
driver packaging and touch-control UI in the Android adapter.

The next code-level comparison should inspect the five extra XHYN generated
translation units and the corresponding registration/function-table changes,
then map shared runtime fixes file-by-file. No generated code should be deleted,
renamed, or mixed until that provenance comparison is complete.


### Generated-code inventory follow-up

The five XHYN-only generated chunks are present in the OMPergo mirror and absent
from the pinned Theft4 tree at the same paths. Their XHYN mirror blob identifiers
are:

| Generated source | XHYN mirror blob SHA | Theft4 snapshot |
|---|---|---|
| `gta4_recomp.84.cpp` | `79286bed098d1b9d729f426d89e9f41905070d13` | Not present |
| `gta4_recomp.85.cpp` | `49b396abb9fbed43c8a2f1a4e87f3ed2537294f4` | Not present |
| `gta4_recomp.86.cpp` | `b2397cef9ef07fbc7824246a638dd27db2e579a1` | Not present |
| `gta4_recomp.87.cpp` | `2400706362d6179bd656bfa04587714276abc37c` | Not present |
| `gta4_recomp.88.cpp` | `ac3a9561cf57a3a7fb5105621a3c8e65c6154711` | Not present |

The generated registration source also differs: Theft4 blob
`7dc9078ce6b7797e24cc625676b1815d1bae1ece`; XHYN mirror blob
`7bb3816c824dc2362146cfd0fd1e55af1f67420d`. The GitHub content endpoint did
not return the large generated file bodies in this audit, so these hashes are
identity evidence only; they are not a semantic function-by-function diff.

This means the five chunks must not simply be appended to Theft4's source list.
The next audit must identify which generated functions/registrations they add,
whether the XHYN source generator/config differs, and whether the new chunks
depend on runtime APIs not present in the Theft4 baseline.


## 9. Runtime source is now pinned inside Universal

The branch now contains a real Git submodule at `runtime/theft4`, pinned to
Theft4 commit `88115fefad59cfcb62d17c0c9548a7a900c01a23`. This makes the
selected iOS runtime source part of the Universal checkout instead of merely
a separate workflow checkout. The submodule is source-only; no GTA IV game data,
keys, saves, or signing material are included.

The OMPergo Android reference mirror is now pinned for reproducible comparison
at `be597d42d12abb04a552fd87e3853028499ae835`. It remains a CI/audit reference
rather than a second full runtime copy, because importing both divergent
generated runtimes would duplicate and potentially conflict game code.

The iOS core workflow now checks out the Universal branch, initializes
`runtime/theft4`, verifies the exact submodule SHA, and runs the device and
simulator core/link matrix from that shared-project path. The workflow is
compile/link validation only. It does not yet prove a complete UIKit app,
installable IPA, GTA IV boot, or multiplayer.

The Universal root CMake project still builds its existing diagnostics/session
core independently. The next integration gate is to expose a controlled
runtime build target/preset without forcing the iOS-specific CMake 3.29/C++23
tree into every host/Android build. After the pinned iOS core/link matrix is
green, wire the runtime's real startup and platform bridge targets into the
Universal app shells. Do not label the current protocol scaffold as gameplay
networking.


## 10. Android runtime source is now pinned inside Universal

The XHYN Android runtime is now a second Git submodule at `runtime/xhyn-android`,
pinned to `be597d42d12abb04a552fd87e3853028499ae835`. Its Gradle project is a
packaging layer; the actual ARM64 native runtime is built first by
`os/android/scripts/build_native.sh`. The APK workflow must therefore build
native libraries before invoking Gradle. It must not report a successful APK
when native runtime libraries were absent.

Universal now exposes opt-in targets
`universal_android_arm64_runtime` and `universal_android_arm64_apk` when
`UNIVERSAL_ENABLE_XHYN_ANDROID_RUNTIME=ON`. These targets are separate from the
iOS runtime targets and do not force Android SDK/NDK requirements on host builds.

The Android reference's current Gradle configuration targets API 35, minSdk 29,
and ARM64. The native build consumes Android NDK 29.0.14206865. These values
must be rechecked against the pinned build files when upgrading the source.
