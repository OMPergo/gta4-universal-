# GTA IV Universal — Final Build 1.0 Release Track

Status: **Alpha 1.0.1 build preparation**. This is a release-candidate track, not a claim that the game is stable or multiplayer-ready.

## Naming
- Alpha artifacts: `gta4-universal-1.0-alpha.N`
- Beta artifacts: `gta4-universal-1.0-beta.N`
- Final release: **Final Build 1.0**, version `1.0.0`

## Alpha acceptance
1. Host core compiles and all CTest tests pass.
2. Android ARM64 native runtime and signed debug APK package successfully.
3. APK identity, signature, expected ARM64 native libraries, and SHA-256 are verified.
4. The user installs on the Galaxy A14 and supplies their own legally obtained GTA IV base game and TU8.
5. Launch, game-data validation, crash capture, and repeatable short gameplay are recorded separately from CI packaging.

## Beta acceptance
1. Alpha blockers are fixed or explicitly documented.
2. Repeatable 10-minute gameplay test with no native crash or low-memory termination.
3. Mali clip-plane correctness, audio continuity, and memory/frame-time telemetry are validated on device.
4. Multiplayer is not called functional until two devices complete a real synchronized session.

## Final 1.0 acceptance
- Android release artifact is signed through an intentional release-signing process.
- iOS app build is produced and an Apple-compliant installation path is confirmed; an unsigned IPA alone is not installable.
- Critical gameplay, stability, and platform-specific acceptance gates pass.
- No proprietary GTA IV game data, title update, keys, or saves are packaged.

## Current known limitations
- Prior Android CI proved APK packaging, not sustained gameplay.
- Galaxy A14 observations include low frame rate, audio underruns, Mali clip-distance limitations, and user-reported crashes. Root causes still require matching-symbol analysis.
- Resource-budget planning sets policy ceilings only. Existing allocators do not yet enforce those ceilings.
- Multiplayer protocol/session scaffolding is not proof of online or cross-platform gameplay.
- The latest iOS build attempt failed during toolchain configuration; no installable iOS package is currently verified.

## Build sequence
1. Alpha 1.0.1: run host tests and Android ARM64 APK workflow from this exact branch.
2. Alpha 1.0.2: address verified compile, packaging, or device blockers.
3. Beta 1.0.1: device acceptance and regression pass.
4. Final Build 1.0: only after the acceptance gates above are met.
