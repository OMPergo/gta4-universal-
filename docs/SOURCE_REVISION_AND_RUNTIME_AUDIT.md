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
