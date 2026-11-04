# XHYN IV R2 Test12 UI Preservation Plan

## Product decision

Keep the XHYN IV r2 Test12 Android user interface as the product UI. Do not replace it with a new launcher, simplified placeholder screen, or a different navigation system during runtime integration.

The upstream reference is:
- Repository: https://github.com/XHYN-PH/XHYN-IV-recomp
- Reference branch: xhyn/r2-test12
- Reference release: 0.5.9.3-xhyn-r2-test12

## Verified upstream UI capabilities

The upstream README and `docs/FORK-DIFFERENCES.md` describe:
- Redesigned launcher and in-game menus, categorized/searchable settings, explanations, and save/discard prompts.
- Game-file installation flow for the user's Xbox 360 GTA IV USA image and Title Update 8.
- Driver selection and driver import.
- Separate editable mobile and Xbox touch layouts, gyro options, and fullscreen touch editor.
- Physical controller configuration and input preview.
- Video, graphics, performance, and overlay settings.
- Save backup/restore, individual save-slot tools, and diagnostics/problem reports.
- Compatible loose Xbox data-file mod management.

## Current repository gap

The current Friday branch does not yet contain that UI implementation. Its `LauncherActivity` is a minimal programmatic screen with a single "Launch Game Engine" button, and `TouchEditorActivity` is empty. The current `LibertyActivity` is a thin SDL activity that loads `libLibertyUniversalCore.so`.

Therefore, UI preservation means porting the upstream Android application layer, not merely retaining the current placeholder activities.

## Integration rules

1. Treat the XHYN r2 Test12 Android app layer as the reference implementation for launcher, settings, installer, touch editor, in-game menus, and supporting resources.
2. Port the related Java/Kotlin sources, XML resources, drawables, assets, manifest declarations, and required Gradle dependencies as a coherent set. Do not copy isolated screens while omitting their backing services or resources.
3. Preserve the navigation structure, wording, interaction patterns, and visual assets unless a compatibility fix is required.
4. Keep game execution and native runtime integration behind the existing UI contracts wherever possible. UI buttons must call real implemented services; do not leave fake success states.
5. Preserve user-owned game data and saves during upgrades. Do not change storage locations or application identity without a migration and install/upgrade compatibility review.
6. Retain required upstream and artwork attributions and all third-party license notices.
7. Keep the APK target (100 MiB) as a measured build constraint, not a reason to remove required UI resources or native runtime components blindly.

## Acceptance gates

- Launcher navigation and all major screens match the XHYN r2 Test12 experience.
- Game-file selection, installation progress, validation, and error states are functional.
- Driver selection, settings persistence, touch-layout editing, controller configuration, and diagnostics work.
- Every visible action is connected to a real implementation or clearly marked unavailable; no placeholder success claims.
- Existing game data and saves are preserved across an update.
- Build and UI smoke tests pass before merging into the default branch.

## Next implementation step

Inventory the exact upstream Android source/resource tree at the pinned r2 Test12 revision, map each component to the current project, then port the Android UI layer before wiring the real native runtime into its launch and installer services.
