# XHYN IV r2 Test12 Android UI Port Inventory

## Pinned upstream reference

- Repository: https://github.com/XHYN-PH/XHYN-IV-recomp
- Branch: `xhyn/r2-test12`
- Exact inspected commit: `025986db1a24aec73ebccc60aac4c235fd820da4`
- Commit: https://github.com/XHYN-PH/XHYN-IV-recomp/commit/025986db1a24aec73ebccc60aac4c235fd820da4
- Release: `0.5.9.3-xhyn-r2-test12`

The upstream release notes describe a full Android ARM64 native build and UI regression suite. Those upstream results are not proof that this repository builds or runs the same code.

## Exact Android entry points inspected

| Upstream file | Inspected blob SHA | Role |
|---|---|---|
| `os/android/app/src/main/java/com/libertyrecomp/LauncherActivity.java` | `08566c0dbf10c5f5f0bd23126c31bf6868800c15` | Main launcher, game-file installation, driver selection, logs, save tools, settings and navigation |
| `os/android/app/src/main/java/com/libertyrecomp/LibertyActivity.java` | `dbc3675500bce989a34e87708b23a1bb3d9828cd` | SDL game activity, native environment, resources, runtime launch and touch/game bridge |
| `os/android/app/src/main/java/com/libertyrecomp/TouchEditorActivity.java` | `18edc2cd61ffb04dcdd8e85fa5cdd6701b32f369` | Full-screen touch-layout editor with save, undo/redo, guides and import/export |
| `os/android/app/src/main/AndroidManifest.xml` | `87292a9012b6cbe78d41c923fd6d7dfbb9fdcc9d` | Activity/process declarations, Vulkan requirement, permissions and SDL metadata |
| `os/android/app/build.gradle.kts` | `a6eb796813f5f351409e75aa538979888b449fae` | Android namespace/application ID, SDL Java source, ABI and native packaging |
| `os/android/app/src/main/res/values/strings.xml` | `af1803dd4a18b6ce581ec6fea7e1310b819e0a71` | App display name |

## Why the launcher cannot be copied by itself

The upstream launcher is an approximately 86 KB, 1,393-line Android activity. It is integrated with many same-package project services and screens, including:

- `ControllerNavigation`, `LauncherArtView`, `ModernUi`
- `LauncherCrashCapture`, `PreviousExitCapture`, `LauncherCrashReport`, `CrashReports`
- `MobileConfig`, `DriverBridge`, `DriverPolicy`, `BundledDrivers`, `DriverPackages`, `TurnipSettingsUi`
- `Installer`, `InstallerLoad`, `GameDataCheck`, `GameStorage`
- `TouchLayoutDocument`, `TouchEditorActivity`, `TouchDefaults`
- `SaveData`, `AchievementsUi`, `ModOverlayUi`, `SessionHistory`, `DebugLogArchive`
- `MobileSettingsUi`, `PerformanceLaunchOptions`, `UiPreferences`

Those classes depend on native JNI methods, JSON/config files, Android resources, drawable assets, SDL Java classes, and native libraries. Copying only the launcher would create a broken build and nonfunctional buttons.

## Compatibility blockers to resolve before porting

1. **Application identity:** upstream Gradle uses namespace `com.libertyrecomp` but application ID `com.xhyn.ivrecomp`. The current project must be checked for its existing application ID, signing key, storage layout, and save paths before changing identity. A package-ID change can install as a separate app and will not automatically inherit private app data.
2. **Native ABI contract:** upstream `LibertyActivity` loads the actual SDL/ReXGlue game runtime and calls native interfaces that are not implemented by the current `libLibertyUniversalCore.so` scaffold. The native API must be integrated as a matching set.
3. **SDL version contract:** upstream Gradle explicitly compiles SDL Java classes from the same SDL3 source tree used to build its native SDL library. Mixing versions can break SDL startup.
4. **Manifest and process model:** upstream runs the game activity in a separate `:game` process and configures SDL/Vulkan, storage, and input. These declarations must be reconciled with this app rather than copied without review.
5. **Resource/data contract:** launcher features depend on bundled touch defaults, icons, fonts, driver archives, resource files, configuration schemas, and installer/native services. The complete dependency set must be ported.
6. **APK budget:** upstream packages multiple native runtime, renderer, SDL and optional driver libraries. Measure the resulting ARM64 APK before promising the 100 MiB limit; game assets remain user-supplied and outside the APK.

## Current-project status

The Friday branch still has a placeholder launcher, thin SDL activity, empty touch editor, and a support-only native core. No XHYN UI Java sources or resources have been copied into the app yet. The earlier preservation-plan commit is a specification, not an implementation.

## Safe port sequence

1. Inventory all upstream Java sources, XML/resources, assets, native JNI entry points, Gradle configuration, and license notices at the pinned commit.
2. Map the upstream package ID, storage paths, settings schema, save migration, and signing/update behavior against this project.
3. Import the complete UI dependency set as a coherent port, preserving the XHYN screen flow and artwork.
4. Integrate the matching ReXGlue/SDL3/native runtime contract; do not leave the UI wired to fake or missing native methods.
5. Add UI/resource and Java regression tests, build ARM64, inspect the APK contents, and run an Android smoke test.
6. Only then consider the port complete. Keep the default branch untouched until the test APK is verified.

## Licensing

XHYN IV documents its source as GPL-3.0 and credits vaduur/LibertyRecompAndroid, OZORDI/LibertyRecomp, ReXGlue, SDL, Mesa, libadrenotools, and adapted touch artwork. Preserve the upstream license files, notices, and component-specific licenses when importing source or assets.
